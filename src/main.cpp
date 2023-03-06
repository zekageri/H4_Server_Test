#include <Arduino.h>
#include <FS.h>
#include <WiFi.h>
#include <LittleFS.h>

#include <H4.h>
#include <H4AsyncWebServer.h>

const char* ssid = "Ant-LAN and the WiFi";
const char* pass = "Random221105";

H4AsyncWebServer server(80);
H4 h4(115200);
H4AW_HTTPHandlerSSE* eventServer = nullptr;
int eventServerClients = 0;
boolean connectToWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    uint8_t status = WiFi.waitForConnectResult();
    if (status == WL_CONNECTED) {
        return true;
    }
    return false;
}

void setupServer() {
    server.on("/", HTTP_GET, [](H4AW_HTTPHandler* request) {
        request->sendFile("/index.html");
    });

    eventServer = new H4AW_HTTPHandlerSSE("/evt");
    eventServer->onChange([=](size_t nClients){
        eventServerClients = nClients;
    });
    server.addHandler(eventServer);
    server.begin();
}

void sendEvent(){
    if(!eventServerClients){ return; }
    eventServer->send(stringFromInt(millis()).data(),"millis");
}

void h4setup(){
    HAL_FS.begin(); 

    if ( !connectToWiFi()) {
        Serial.printf("Failed to connect to %s\n", ssid);
        return;
    }

    Serial.printf("Connected to %s\n", ssid);
    setupServer();
    h4.every(500,[=]{
        sendEvent();
    });
}