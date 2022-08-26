#include "coffeeOS.h"

// Settings-Menu
char *menu1[] = {
    (char *)"<<<<",
    (char *)"User-Info",
    (char *)"User-Reset",
    (char *)"Netzwerk-Info",
    (char *)"Helligkeit",
    nullptr
};

/*! @brief Array of function pointers to the menu functions */
void (*menu_func[])(Adafruit_PN532& nfc_reader) {
    &main_menu,
    &first_menu,
    &second_menu,
    &settings_menu
};

/*! @brief This function calls the according menu function based on menu id. */
void select_menu(unsigned int menu_id, Adafruit_PN532& nfc_reader) {
    if(menu_id >= elementCount(menu_func)) {
        Serial.println("Return to Main Menu");
        menuID = 0;
    }
    else menu_func[menu_id]();
}

/*! @brief Menu function for the product selection. */
void main_menu(Adafruit_PN532& nfc_reader) {
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

/*! @brief Menu function that shows the connection status to the coffee machine. */
void first_menu(Adafruit_PN532& nfc_reader) {
    Serial.println("1# Coffee Machine Query");
    obdFill(&obd, 0, 1);

    delay(500);
    menuID = 2;
}

/*! @brief Menu function that shows status info on product preparation. */
void second_menu(Adafruit_PN532& nfc_reader) {
    Serial.println("2# Drink in Production");
    delay(500);
    menuID = 0;
}

/*! @brief Menu function that shows settings. */
void settings_menu(Adafruit_PN532& nfc_reader) {
    Serial.println("Settings Menu");

    index = 0;
    char *header = "[SETTINGS]";
    len = (int)(sizeof(menu1) / sizeof(menu1[0])) - 2;

    // Encoder wieder reaktivieren
    encoder.resumeCount();
    encoder.clearCount();

    while (menuID == 3) {

        index = (int8_t) (encoder.getCount() / 4);
        if (index < 0) {
            encoder.setCount(0);
            index = 0;
        }
        if (index > len) {
            encoder.setCount(len * 4);
            index = len;
        }

        if (prevIndex != index) {
            obdFill(&obd, 0, 0);
            obdWriteString(&obd, 0, centerOffset(header, 8), 0, header, FONT_8x8, 0, 0);

            for (uint8_t i = 0; i <= len; i++) {
                if (i == index)
                    obdWriteString(&obd, 0, 0, i + 1, menu1[i], FONT_8x8, 1, 0);
                else
                    obdWriteString(&obd, 0, 0, i + 1, menu1[i], FONT_8x8, 0, 0);
            }

            obdDumpBuffer(&obd, NULL);
        }

        if (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS) {
            while (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS);

            onOff = 0;
            for (uint8_t i = 0; i < 5; i++) {
                obdWriteString(&obd, 0, 0, index + 1, menu1[index], FONT_8x8, onOff, 1);
                delay(50);
                onOff = !onOff;
            }

            switch (index) { //------------------------ Sub-Menü
                case 0:
                    menuID = 0; //__________________ Hauptmenü
                    break;

                case 1: { //____________________________ User-Info
                    Serial.println("User_Info");

                    obdFill(&obd, 0, 1);
                    obdWriteString(&obd, 0, 0, 0, (char *) "Waiting for Card", FONT_6x8, 0, 1);

                    nfc_reader.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);

                    while (index == 1) {

                        if (digitalRead(PN532_IRQ) == LOW) {
                            success = nfc_reader.readDetectedPassiveTargetID(uid, &uidLength);
                            Serial.println(success ? "Read successful" : "Read failed (not a card?)");
                            if (!success)
                                break;

                            obdWriteString(&obd, 0, 0, 1, (char *) "Card Found!", FONT_6x8, 0, 1);

                            sprintf(sTemp, "%d byte UID:", uidLength);
                            obdWriteString(&obd, 0, 0, 2, sTemp, FONT_6x8, 0, 1);

                            sprintf(sTemp, "UID: %d %d %d %d", uid[0], uid[1], uid[2], uid[3]);
                            obdWriteString(&obd, 0, 0, 3, sTemp, FONT_6x8, 1, 1);
                            Serial.printf("\n%d %d %d %d %d %d %d\n", uid[0], uid[1], uid[2], uid[3], uid[4], uid[5],
                                          uid[6]);

                            userIndex = getElements(user, uid);
                            Serial.println(userIndex);

                            if (userIndex == -1) {
                                obdWriteString(&obd, 0, 0, 5, (char *) "User N/A", FONT_8x8, 0, 1);
                                Serial.println("User N/A");
                            } else {
                                obdWriteString(&obd, 0, 0, 5, user[userIndex].c_name, FONT_8x8, 0, 1);
                                Serial.println(user[userIndex].c_name);

                                balance = 0;
                                for (uint8_t i = 0; i < (sizeof(prices) / sizeof(prices[0])); i++)
                                    balance += user[userIndex].memory[i] * prices[i];

                                sprintf(sTemp, "Offen: %4.2f Euro", balance);
                                obdWriteString(&obd, 0, 0, 6, sTemp, FONT_6x8, 0, 1);
                            }

                            while (digitalRead(ROT_BUTTON) != ROT_BUTTON_PRESS);
                            while (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS);
                            delay(500);
                            index = 99;
                        }
                    }
                }
                    break;

                case 2: { //____________________________ User-Reset
                    Serial.println("User-Reset");

                    int8_t index1 = 0;
                    char *header = "[RESET]";
                    int8_t elements = getElements(user, NULL) - 1;

                    while (index == 2) {

                        index1 = (int8_t) (encoder.getCount() / 4);
                        if (index1 < 0) {
                            encoder.setCount(0);
                            index1 = 0;
                        }
                        if (index1 > userIndex) {
                            encoder.setCount(elements * 4);
                            index = elements;
                        }

                        lastMillis = millis();
                        if (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
                            while (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
                                if (millis() - lastMills > 2000) {
                                    obdWriteString(&obd, 0, 0, 4, "Yes=long | No=short", FONT_6x8, 0, 1);
                                    delay(1000);
                                    lastMillis = millis();
                                    if (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
                                        while (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS)
                                            if (millis() - lastMillis > 2000) {
                                                obdWriteString(&obd, 0, 0, 5, "Admin Confirmation", FONT_6x8, 0, 1);
                                                nfc_reader.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);

                                                while (digitalRead(PN532_IRQ));
                                                success = nfc_reader.readDetectedPassiveTargetID(uid, &uidLength);
                                                if (!success)
                                                    break;

                                                userIndex = getElements(user, uid);
                                                if (userIndex == 0 || userIndex == 3) {
                                                    obdWriteString(&obd, 0, 0, 6, "Confirmed", FONT_6x8, 0, 1);
                                                    user[elements].memory = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                                             0, 0, 0, 0};
                                                    saveUsers(user);
                                                }
                                            }
                                }

                        if (prevIndex != index1) {
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

                    case 3: { //____________________________ Netzwerk-Info
                        Serial.println("Network-Info");
                        index = 99;
                    }
                    break;

                    case 4: { //____________________________ Helligkeit
                        Serial.println("Brightness");

                        encoder.setCount((brightness * 400) / 255);

                        while (index == 3) {

                            brightness = (int16_t) (encoder.getCount() / 4);
                            Serial.println(brightness);
                            brightness = ((brightness * 255) / 100);
                            Serial.println(brightness);
                            if (brightness < 2) {
                                encoder.setCount(4);
                                brightness = 2;
                            }
                            if (brightness > 255) {
                                encoder.setCount(100 * 4);
                                brightness = 255;
                            }

                            if (prevIndex != brightness) {
                                obdSetContrast(&obd, brightness);
                                obdFill(&obd, 0, 0);
                                obdWriteString(&obd, 0, centerOffset(menu1[3], 12), 0, menu1[3], FONT_12x16, 0, 0);

                                sprintf(sTemp, "%d", brightness * 100 / 255);
                                obdWriteString(&obd, 0, centerOffset(sTemp, 16), 3, sTemp, FONT_16x16, 0, 0);

                                obdDumpBuffer(&obd, NULL);
                                Serial.println(brightness);
                            }

                            if (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS) {
                                while (digitalRead(ROT_BUTTON) == ROT_BUTTON_PRESS);
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
}

/*! @brief Function to set a screensaver to the display. */
void screensaver(Adafruit_PN532& nfc_reader)
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
}