# coffeeOS

CoffeeOS is a software framework designed for controlling coffee machines via WLAN. It includes the necessary functionalities to manage product selection, user input via a rotary encoder, and communication through an OLED display and NFC reader for user authentication. The system is based on an ESP32 microcontroller with Wi-Fi connectivity and it has a simple barebones webserver for usage statistics.

The project originated as a study project in university for a project report. In the making of this project the coffee machine 'Miele CM 7350' was used.

## Features

- Rotary Encoder Input: Allows the user to navigate through product menus.
- OLED Display Support: Displays user-relevant information such as product names and status updates.
- NFC Reader Support: Utilizes the PN532 NFC module for RFID interactions.
- Wi-Fi Connectivity: Supports Wi-Fi connection for remote operations.
- Menu Management: Handles the selection and display of products.

![Menu Example](images/coffeeos-menu-example.gif)

## Setup

1. Clone the repository and ensure you have the necessary libraries installed:

- Adafruit PN532
- ESP32Encoder
- Wire, SPI (for I2C communication)

2. Connect the necessary hardware:

- Rotary Encoder: Connect to the appropriate pins for user input.
- OLED Display: I2C-connected OLED display for displaying text and other information.
- NFC Reader (PN532): Connect via I2C for RFID reading functionality.

3. Edit the hardware pin configuration in the code (`coffeeOS.h`).

4. Compile and upload the `coffeeOS.cpp` to your ESP32 using the Arduino IDE or PlatformIO.

## License

MIT License.
Copyright © 2022 Tim Prüß  

See [LICENSE](LICENSE) for Details.
