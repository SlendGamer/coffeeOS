
#include "coffeeOS.h"
#include "wifi_handler.h"

wifi_handler::wifi_handler(const char* wifi_ssid, const char* wifi_password) {
    this->ssid = wifi_ssid;
    this->password = wifi_password;
}

void wifi_handler::init() {
    OBDISP& oled = CoffeeOS::instance().oled_display;

    DEBUG_PRINT("wifi_handler_init(): connecting to ");
    DEBUG_PRINTLN(ssid);

    // begin wifi connection
    WiFi.begin(ssid, password);

    // wait until connected
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        DEBUG_PRINT(".");
    }

    // debug and status messages
    DEBUG_PRINTLN("wifi_handler_init(): wifi connection established");
    DEBUG_PRINT("wifi_handler_init(): IP address: ");
    DEBUG_PRINTLN(WiFi.localIP());
    obdFill(&oled, 0, 1);
    obdWriteString(&oled, 0, 0, 1*8, (char*) "wifi init", FONT_6x8, 0, 1);
    delay(1000);
    
    // begin wifi server communication
    server.begin();
}

void wifi_handler::wifi_client() {
    client = server.available();   // listen for incoming clients

    if (client) {                             // if you get a client,
        Serial.println("wifi_client(): new client");           // print a message out the serial port
        String currentLine = "";                // make a String to hold incoming data from the client
        while (client.connected()) {            // loop while the client's connected
            if (client.available()) {             // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                Serial.write(c);                    // print it out the serial monitor
                if (c == '\n') {                    // if the byte is a newline character

                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();

                        // the content of the HTTP response follows the header:
                        client.print("<h2> CoffeeOS Web-interface </h2>");

                        client.print("<h3>Status:</h3>");
                        client.printf("Last user transaction: %s<br>", CoffeeOS::instance().my_manager.user_list.at(CoffeeOS::instance().last_user).first_name.c_str());
                        client.printf("Last product transaction: %s<br>", CoffeeOS::instance().my_manager.product_list.at(CoffeeOS::instance().last_product).name.c_str());

                        client.print("<h3>Commands:</h3>");
                        client.print("<a href=\"/serial_print_users\">Serial Print Users</a>: Print all users from list to serial monitor.<br>");
                        client.print("<a href=\"/save_users\">Save Users</a>: Save all users to the Flash via SPIFFS.<br>");
                        client.print("<a href=\"/load_users\">Load Users</a>: Load all users.<br>");
                        client.print("<a href=\"/load_default_users\">Load Default Users</a>: Load default users.<br>");

                        client.print("<a href=\"/serial_print_products\">Serial Print Products</a>: Print all products from list to serial monitor.<br>");
                        client.print("<a href=\"/save_products\">Save Products</a>: Save all products to the Flash via SPIFFS.<br>");
                        client.print("<a href=\"/load_products\">Save Users</a>: Load all products.<br>");
                        client.print("<a href=\"/load_default_products\">Load Default Products</a>: Load default products.<br>");

                        client.print("<a href=\"/restart\">Restart</a>: Restart the Microcontroller.<br>");
                        
                        // The HTTP response ends with another blank line:
                        client.println();
                        // break out of the while loop:
                        break;
                    } else {    // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                } else if (c != '\r') {  // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                }

                parse_http_request(currentLine);
                
            }
        }
        // close the connection:
        client.stop();
        Serial.println("wifi_client(): client disconnected");
    }
}

void wifi_handler::parse_http_request(String request) {
    if (request.endsWith("GET /serial_print_users")) CoffeeOS::instance().my_manager.serial_print_users();
    if (request.endsWith("GET /save_users")) CoffeeOS::instance().my_manager.save_users();
    if (request.endsWith("GET /load_users")) CoffeeOS::instance().my_manager.load_users();
    if (request.endsWith("GET /load_default_users")) CoffeeOS::instance().my_manager.load_default_users();

    if (request.endsWith("GET /serial_print_products")) CoffeeOS::instance().my_manager.serial_print_products();
    if (request.endsWith("GET /save_products")) CoffeeOS::instance().my_manager.save_products();
    if (request.endsWith("GET /load_products")) CoffeeOS::instance().my_manager.load_products();
    if (request.endsWith("GET /load_default_products")) CoffeeOS::instance().my_manager.load_default_products();

    if (request.endsWith("GET /restart")) CoffeeOS::instance().restart();
}

void wifi_handler::tcp_send(const char* message) {
  Serial.printf("connecting to %s %d \n", tcp_host, tcp_host_port);

  if (!client.connect(tcp_host, tcp_host_port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }
  Serial.println("sending data to server");

  client.printf(message);
  Serial.println("closing connection");
  client.stop();
}