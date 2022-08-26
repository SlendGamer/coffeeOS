
#include "coffeeOS.h"

CoffeeOS::CoffeeOS() = default;

void loadUsers() {
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
}


/*! @brief This function initializes the rotary encoder.
 */
void CoffeeOS::rotaryEncoder_init() {
    pinMode(ROT_BUTTON, INPUT_PULLUP);
    pinMode(ROT_CLK, INPUT);
    pinMode(ROT_DT, INPUT);

    ESP32Encoder::useInternalWeakPullResistors = UP;
    encoder.attachFullQuad(ROT_CLK, ROT_DT);
    encoder.clearCount();
    encoder.pauseCount();
}

void startUpScreen() {
    obdFill(&obd, 0, 1);
    obdWriteString(&obd, 0, 0, 1, (char *)"ESP32 - ", FONT_16x16, 0, 1);
    obdWriteString(&obd, 0, 0, 3, (char *)"Coffee- ", FONT_16x16, 0, 1);
    obdWriteString(&obd, 0, 0, 5, (char *)"inator  ", FONT_16x16, 0, 1);
    delay(2000);
}

void CoffeeOS::display_init() {
    char *msgs[] = {(char *)"SSD1306 @ 0x3C", (char *)"SSD1306 @ 0x3D", (char *)"SH1106 @ 0x3C", (char *)"SH1106 @ 0x3D"};

    int checkDisplay = obdI2CInit(&obd, OLED_128x64, OLED_ADDR, FLIP180, INVERT, USE_HW_I2C, OLED_SDA, OLED_SCL, RESET_PIN, 800000L);
    obdSetContrast(&obd, brightness);
    obdSetBackBuffer(&obd, ucBackBuffer);
    obdSetTextWrap(&obd, 1);

    obdFill(&obd, 0, 1);
    obdWriteString(&obd, 0, 0, 1, msgs[checkDisplay], FONT_8x8, 0, 1);
}

void CoffeeOS::nfcReader_init() {
    nfc.begin(PN532_SDA, PN532_SCL);

    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("PN532 not found!");
        obdWriteString(&obd, 0, 0, 2, (char *) "PN532 not found!", FONT_8x8, 0, 1);
        delay(2000);
    }

    Serial.println("PN532 found!");
    obdWriteString(&obd, 0, 0, 2, (char *)"PN532 init", FONT_8x8, 0, 1);

    nfc.setPassiveActivationRetries(0xFF); // set maximum retries
    nfc.SAMConfig();
    Serial.println("PN532 configured");
    obdWriteString(&obd, 0, 0, 3, (char *)"PN532 config", FONT_8x8, 0, 1);
}

#ifdef ENABLE_WIFI
void wifi_init() {
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
}
#endif

void CoffeeOS::setup() {

#ifdef DEBUG
	Serial.begin(115200);
#endif

    loadUsers(); // load user list
    rotaryEncoder_init(); // initialise rotary encoder
	display_init(); // initialise display
    nfcReader_init(); // initialise nfc reader

#ifdef ENABLE_WIFI
    wifi_init(); // initialise wifi connection
#endif

	// StartupScreen
	startUpScreen();
}

void CoffeeOS::loop() {
	int8_t index;
	int8_t userIndex;
	int8_t prevIndex = -1;
	uint8_t len;

	uint8_t success;
	uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
	uint8_t uidLength;					   // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

	// loadUsers(&user);

    select_menu(menuID);
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
int elementCount(T **arrayPtr) { return (sizeof(*arrayPtr) / sizeof(**arrayPtr)); }


// Interrupt-Routine für Button-Press
void confirm() { pressConfirm = true; }

uint8_t centerOffset(char *textElement, uint8_t fontWidth)
{
	return (128 - (strlen(textElement) * fontWidth)) / 2;
}