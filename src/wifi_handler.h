#pragma once

#include <WiFi.h>

class wifi_handler {
public:
    wifi_handler(const char*, const char*);
    void init();
    void wifi_client();
    void parse_http_request(String);

    void tcp_send(const char*);
    
    WiFiServer server;
    WiFiClient client;

private:
    const char* ssid;
    const char* password;
    
    const char* tcp_host = "192.168.178.1";
    uint16_t tcp_host_port = 8080;
};
