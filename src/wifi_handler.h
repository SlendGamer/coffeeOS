/*!
    @file     wifi_handler.h
    @author   Tim Prüß

    Contains the declarations of class and members (functions).
*/

#pragma once

// extern includes
#include <WiFi.h>

/*!
    @brief  Contains all functions for the establishing and managing the wifi connection and web interface.
*/
class wifi_handler {
public:
    wifi_handler(const char*, const char*);
    void init();
    void wifi_client();
    void parse_http_request(String);

    void cmd_send(int);
    
    WiFiServer server; /*! Instance of class WiFiServer */
    WiFiClient client; /*! Instance of class WiFiClient */

private:
    void tcp_send(const char*);

    const char* ssid; /*! Stores the wifi ssid */
    const char* password; /*! Stores the wifi pre-shared key (password) */
    
    const char* tcp_host = "192.168.178.1"; /*! Stores the adress to send the tcp message to */
    uint16_t tcp_host_port = 8080; /*! Stores port to connect to when sending the tcp message */

    const char* cmd_lookup[4] = {
        "command1",
        "command2",
        "command3",
        "command4"
    };
};
