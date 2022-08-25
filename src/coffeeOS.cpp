
#include "coffeeOS.h"

CoffeeOS::CoffeeOS() {}

/*! @brief This function initializes the rotary encoder.
 */
void CoffeeOS::rotaryEncoder_init() {
    pinMode(ROT_BUTTON, INPUT_PULLUP);
    pinMode(ROT_CLK, INPUT);
    pinMode(ROT_DT, INPUT);
}

void CoffeeOS::setup()
{
#ifdef DEBUG
	Serial.begin(115200);
#endif

    rotaryEncoder_init();

	// Benutzer-Liste neu laden
	user = (user_t *)calloc(1, sizeof(user_t));
	loadUsers(&user);
	for (uint8_t i = 0; i < getElements(user, NULL); i++)
	{
		Serial.printf("Name: %s\n", user[i].c_name);
		Serial.print("UID: ");
		for (uint8_t c = 0; c < sizeof(user[i].cardUID); c++)
		{
			Serial.printf(" %d ", user[i].cardUID[c]);
		}
		Serial.printf("\n");
	}
	Serial.printf("\n");

	// Display initialisieren
	char *msgs[] = {(char *)"SSD1306 @ 0x3C", (char *)"SSD1306 @ 0x3D", (char *)"SH1106 @ 0x3C", (char *)"SH1106 @ 0x3D"};

	int checkDisplay = obdI2CInit(&obd, OLED_128x64, OLED_ADDR, FLIP180, INVERT, USE_HW_I2C, OLED_SDA, OLED_SCL, RESET_PIN, 800000L);
	obdSetContrast(&obd, brightness);
	obdSetBackBuffer(&obd, ucBackBuffer);
	obdSetTextWrap(&obd, 1);

	obdFill(&obd, 0, 1);
	obdWriteString(&obd, 0, 0, 1, msgs[checkDisplay], FONT_8x8, 0, 1);

	nfc.begin(PN532_SDA, PN532_SCL);

	uint32_t versiondata = nfc.getFirmwareVersion();

	if (!versiondata)
	{
		Serial.println("PN532 not found!");
		obdWriteString(&obd, 0, 0, 2, (char *)"PN532 not found!", FONT_8x8, 0, 1);
		delay(2000);
	}

	Serial.println("PN532 found!");
	obdWriteString(&obd, 0, 0, 2, (char *)"PN532 init", FONT_8x8, 0, 1);

	// Maximum Retries
	nfc.setPassiveActivationRetries(0xFF);
	nfc.SAMConfig();
	Serial.println("PN532 configured");
	obdWriteString(&obd, 0, 0, 3, (char *)"PN532 config", FONT_8x8, 0, 1);

	WiFi.mode(WIFI_STA);
	WiFi.begin(initial_ssid, initial_password);
	Serial.print("Connecting to WiFi.");
	obdWriteString(&obd, 0, 0, 4, (char *)"WiFi connect", FONT_8x8, 0, 1);
	count = 0;
	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print('.');
		obdWriteString(&obd, 0, count * 8, 5, (char *)".", FONT_8x8, 0, 1);
		count++;
		delay(500);
		if (millis() - lastMillis == 10000)
			ESP.restart();
		lastMillis = millis();
	}

	IPAddress ip = WiFi.localIP();
	Serial.printf("\nWifi connected!\nIP address: %d.%d.%d.%d\n\n", ip[0], ip[1], ip[2], ip[3]);

	server.begin();

	// StartupScreen
	startUpScreen();

	void startUpScreen()
	{
		obdFill(&obd, 0, 1);
		obdWriteString(&obd, 0, 0, 1, (char *)"ESP32 - ", FONT_16x16, 0, 1);
		obdWriteString(&obd, 0, 0, 3, (char *)"Coffee- ", FONT_16x16, 0, 1);
		obdWriteString(&obd, 0, 0, 5, (char *)"inator  ", FONT_16x16, 0, 1);
	}

	delay(2000);

	/*uint8_t *pAnim = (uint8_t *) animation;
	lastMillis = millis();
	while(millis() - lastMillis <= 5000)
	{ // play all frames (automatically starts over when it reaches the end)
	  pAnim = obdPlayAnimFrame(&obd, (uint8_t *)animation, pAnim, sizeof(animation));
	  delay(20); // simplistic rate control; should manage the variable time per frame
	}*/

	ESP32Encoder::useInternalWeakPullResistors = UP;
	encoder.attachFullQuad(ROT_CLK, ROT_DT);
	encoder.clearCount();
	encoder.pauseCount();
}

void CoffeeOS::loop()
{

	int8_t index;
	int8_t userIndex;
	int8_t prevIndex = -1;
	uint8_t len;

	uint8_t success;
	uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
	uint8_t uidLength;					   // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

	// loadUsers(&user);

	switch (menuID)
	{
	case 0:
	{ //____________________________________ Hauptmenü (Getränkeauswahl)
		Serial.println("Main Menu");

		index = 0;
		char *header = "*** AUSWAHL ***";
		len = (int)(sizeof(menu0) / sizeof(menu0[0])) - 2;

		// Encoder wieder reaktivieren
		encoder.resumeCount();
		encoder.clearCount();

		while (menuID == 0)
		{

			index = (int8_t)(encoder.getCount() / 4);
			if (index < 0)
			{
				encoder.setCount(0);
				index = 0;
			}
			if (index > len)
			{
				encoder.setCount(len * 4);
				index = len;
			}

			if (prevIndex != index)
			{
				obdFill(&obd, 0, 0);
				obdWriteString(&obd, 0, centerOffset(header, 8), 0, header, FONT_8x8, 1, 0);
				sprintf(sTemp, "%d", index);
				obdWriteString(&obd, 0, 4, 1, sTemp, FONT_8x8, 1, 0);
				obdWriteString(&obd, 0, 0, 3, menu0[index], FONT_12x16, 0, 0);
				obdDumpBuffer(&obd, NULL);
			}

			if (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
			{
				while (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
					;

				if (index >= 0 && index <= 17)
					menuID = 1;
				else
					menuID = 3;

				onOff = 0;
				for (uint8_t i = 0; i < 5; i++)
				{
					obdWriteString(&obd, 0, 0, 3, menu0[index], FONT_12x16, onOff, 1);
					delay(50);
					onOff = !onOff;
				}
			}

			prevIndex = index;
			delay(20);
		}

		encoder.pauseCount();
	}
	break;

	case 1:
	{ //____________________________________ Verbindung mit Kaffeemaschine
		Serial.println("1# Coffee Machine Query");
		obdFill(&obd, 0, 1);

		delay(500);
		menuID = 2;
	}
	break;

	case 2:
	{ //____________________________________ Getränk wird zubereitet --> Bildschirmschoner
		Serial.println("2# Drink in Production");

		delay(500);
		menuID = 0;
	}
	break;

	case 3:
	{ //____________________________________ Menü --> Einstellungen
		Serial.println("Settings Menu");

		index = 0;
		char *header = "[SETTINGS]";
		len = (int)(sizeof(menu1) / sizeof(menu1[0])) - 2;

		// Encoder wieder reaktivieren
		encoder.resumeCount();
		encoder.clearCount();

		while (menuID == 3)
		{

			index = (int8_t)(encoder.getCount() / 4);
			if (index < 0)
			{
				encoder.setCount(0);
				index = 0;
			}
			if (index > len)
			{
				encoder.setCount(len * 4);
				index = len;
			}

			if (prevIndex != index)
			{
				obdFill(&obd, 0, 0);
				obdWriteString(&obd, 0, centerOffset(header, 8), 0, header, FONT_8x8, 0, 0);

				for (uint8_t i = 0; i <= len; i++)
				{
					if (i == index)
						obdWriteString(&obd, 0, 0, i + 1, menu1[i], FONT_8x8, 1, 0);
					else
						obdWriteString(&obd, 0, 0, i + 1, menu1[i], FONT_8x8, 0, 0);
				}

				obdDumpBuffer(&obd, NULL);
			}

			if (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
			{
				while (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
					;

				onOff = 0;
				for (uint8_t i = 0; i < 5; i++)
				{
					obdWriteString(&obd, 0, 0, index + 1, menu1[index], FONT_8x8, onOff, 1);
					delay(50);
					onOff = !onOff;
				}

				switch (index)
				{ //------------------------ Sub-Menü
				case 0:
					menuID = 0; //__________________ Hauptmenü
					break;

				case 1:
				{ //____________________________ User-Info
					Serial.println("User_Info");

					obdFill(&obd, 0, 1);
					obdWriteString(&obd, 0, 0, 0, (char *)"Waiting for Card", FONT_6x8, 0, 1);

					nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);

					while (index == 1)
					{

						if (digitalRead(PN532_IRQ) == LOW)
						{
							success = nfc.readDetectedPassiveTargetID(uid, &uidLength);
							Serial.println(success ? "Read successful" : "Read failed (not a card?)");
							if (!success)
								break;

							obdWriteString(&obd, 0, 0, 1, (char *)"Card Found!", FONT_6x8, 0, 1);

							sprintf(sTemp, "%d byte UID:", uidLength);
							obdWriteString(&obd, 0, 0, 2, sTemp, FONT_6x8, 0, 1);

							sprintf(sTemp, "UID: %d %d %d %d", uid[0], uid[1], uid[2], uid[3]);
							obdWriteString(&obd, 0, 0, 3, sTemp, FONT_6x8, 1, 1);
							Serial.printf("\n%d %d %d %d %d %d %d\n", uid[0], uid[1], uid[2], uid[3], uid[4], uid[5], uid[6]);

							userIndex = getElements(user, uid);
							Serial.println(userIndex);

							if (userIndex == -1)
							{
								obdWriteString(&obd, 0, 0, 5, (char *)"User N/A", FONT_8x8, 0, 1);
								Serial.println("User N/A");
							}
							else
							{
								obdWriteString(&obd, 0, 0, 5, user[userIndex].c_name, FONT_8x8, 0, 1);
								Serial.println(user[userIndex].c_name);

								balance = 0;
								for (uint8_t i = 0; i < (sizeof(prices) / sizeof(prices[0])); i++)
									balance += user[userIndex].memory[i] * prices[i];

								sprintf(sTemp, "Offen: %4.2f Euro", balance);
								obdWriteString(&obd, 0, 0, 6, sTemp, FONT_6x8, 0, 1);
							}

							while (digitalRead(ROT_BUTTON) != ROT_BUTTON_PRESS)
								;
							while (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
								;
							delay(500);
							index = 99;
						}
					}
				}
				break;

				case 2:
				{ //____________________________ User-Reset
					Serial.println("User-Reset");

					int8_t index1 = 0;
					char *header = "[RESET]";
					int8_t elements = getElements(user, NULL) - 1;

					while (index == 2)
					{

						index1 = (int8_t)(encoder.getCount() / 4);
						if (index1 < 0)
						{
							encoder.setCount(0);
							index1 = 0;
						}
						if (index1 > userIndex)
						{
							encoder.setCount(elements * 4);
							index = elements;
						}

						lastMillis = millis();
						if (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
							while (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
								if (millis() - lastMills > 2000)
								{
									obdWriteString(&obd, 0, 0, 4, "Yes=long | No=short", FONT_6x8, 0, 1);
									delay(1000);
									lastMillis = millis();
									if (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
										while (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
											if (millis() - lastMillis > 2000)
											{
												obdWriteString(&obd, 0, 0, 5, "Admin Confirmation", FONT_6x8, 0, 1);
												nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);

												while (digitalRead(PN532_IRQ))
													;
												success = nfc.readDetectedPassiveTargetID(uid, &uidLength);
												if (!success)
													break;

												userIndex = getElements(user, uid);
												if (userIndex == 0 || userIndex == 3)
												{
													obdWriteString(&obd, 0, 0, 6, "Confirmed", FONT_6x8, 0, 1);
													user[elements].memory = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
													saveUsers(user);
												}
											}
								}

						if (prevIndex != index1)
						{
							obdFill(&obd, 0, 0);
							obdWriteString(&obd, 0, centerOffset(header, 8), 0, header, FONT_8x8, 0, 0);
							obdWriteString(&obd, 0, centerOffset(header, 8), 1, user[index1].c_name, FONT_8x8, 1, 0);

							balance = 0;
							for (uint8_t i = 0; i < (sizeof(prices) / sizeof(prices[0])); i++)
								balance += user[index1].memory[i] * prices[i];
							sprintf(sTemp, "Offen: %4.2f Euro", balance);
							obdWriteString(&obd, 0, 0, 2, sTemp, FONT_8x8, 1, 0);

							obdDumpBuffer(&obd, NULL);
						}
						prevIndex = index1;

						index = 99;
					}
					break;

				case 3:
				{ //____________________________ Netzwerk-Info
					Serial.println("Network-Info");
					index = 99;
				}
				break;

				case 4:
				{ //____________________________ Helligkeit
					Serial.println("Brightness");

					encoder.setCount((brightness * 400) / 255);

					while (index == 3)
					{

						brightness = (int16_t)(encoder.getCount() / 4);
						Serial.println(brightness);
						brightness = ((brightness * 255) / 100);
						Serial.println(brightness);
						if (brightness < 2)
						{
							encoder.setCount(4);
							brightness = 2;
						}
						if (brightness > 255)
						{
							encoder.setCount(100 * 4);
							brightness = 255;
						}

						if (prevIndex != brightness)
						{
							obdSetContrast(&obd, brightness);
							obdFill(&obd, 0, 0);
							obdWriteString(&obd, 0, centerOffset(menu1[3], 12), 0, menu1[3], FONT_12x16, 0, 0);

							sprintf(sTemp, "%d", brightness * 100 / 255);
							obdWriteString(&obd, 0, centerOffset(sTemp, 16), 3, sTemp, FONT_16x16, 0, 0);

							obdDumpBuffer(&obd, NULL);
							Serial.println(brightness);
						}

						if (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
						{
							while (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
								;
							index = 99;
						}

						prevIndex = brightness;
						delay(20);
					}
				}
				break;

				default:
					menuID = 3;
					break;
				}
				}

				prevIndex = index;
				delay(20);
			}

			encoder.pauseCount();
		}
		break;

	default:
		Serial.println("Return to Main Menu");
		menuID = 0;
		break;
	}
	}
}

void webServerCode(void *parameter)
{
	for (;;)
	{
		WiFiClient client = server.available();

		if (client)
		{
			Serial.println("New Client");
			String currentLine = "";

			while (client.connected())
			{
				if (client.available())
				{
					char c = client.read();
					Serial.print(c);
					header += c;

					if (currentLine.length() == 0)
					{
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println("Connection: close");
						client.println();
					}
					vTaskDelay(1);
					yield();
				}
			}
		}
	}
}

// counting elements in arrays
template <typename T>
int elementCount(T **arrayPtr) { return sizeof(*arrayPtr) / sizeof(**arrayPtr); }

// Bildschirmschoner
void hibernate()
{
	Serial.println("Entering Screen-Saver");

	obdFill(&obd, 0, 1);
	obdSetContrast(&obd, 16);
	attachInterrupt(digitalPinToInterrupt(ROT_CLK), confirm, CHANGE);
	// unsigned char x = 0;
	int x, y;
	bool pixelOnOff = 1;

	while (pressConfirm == false)
	{

		/*obdFill(&obd, x, 1);
		x = random(0, 255);
		delay(500);*/

		for (x = 0; x <= 128; x++)
		{
			for (y = 0; y <= 64; y++)
				obdSetPixel(&obd, x, y, pixelOnOff, 1);
		}
		// obdDumpBuffer(&obd, NULL);
		pixelOnOff = !pixelOnOff;
	}

	detachInterrupt(digitalPinToInterrupt(ROT_CLK));
	obdSetContrast(&obd, brightness);
	obdFill(&obd, 0, 1);
	pressConfirm = false;
	return;
}

// Interrupt-Routine für Button-Press
void confirm() { pressConfirm = true; }

void saveUsers(user_t *user)
{
	pref.begin("settings");
	int8_t elements = getElements(user, NULL);
	if (elements == -1)
	{
		Serial.println("Saving Users failed!");
		pref.end();
		return;
	}
	pref.putBytes("userList", user, (elements + 1) * sizeof(user_t));
	pref.end();
	Serial.println("Saving Users successful!");
}

void loadUsers(user_t **user)
{
	pref.begin("settings");
	size_t len = pref.getBytesLength("userList");

	if (len == 0)
	{
		Serial.println("Loading Users failed!");
		return;
	}

	char *buffer = (char *)malloc(len);
	pref.getBytes("userList", buffer, len);
	*user = (user_t *)realloc(buffer, len);
	if (*user == NULL)
	{
		Serial.println("Loading Users failed!");
		pref.end();
		return;
	}

	free(buffer);
	pref.end();
	Serial.println("Loading Users successful!");
}

int8_t getElements(user_t *user, uint8_t *UID)
{
	int8_t elementCount = 0;

	if (UID == NULL)
	{
		while (strcmp(user[elementCount].c_name, ""))
		{
			if (elementCount > 126)
			{
				return -1;
			}
			elementCount++;
		}
	}
	else
	{
		int8_t maxElements = getElements(user, NULL);
		size_t len = sizeof(((user_t){0}).cardUID);
		while (strncmp((const char *)user[elementCount].cardUID, (const char *)UID, len))
		{
			if (elementCount > 126 || elementCount == (maxElements - 1))
			{
				return -1;
			}
			elementCount++;
		}
	}

	return elementCount;
}

uint8_t centerOffset(char *textElement, uint8_t fontWidth)
{
	return (128 - (strlen(textElement) * fontWidth)) / 2;
}