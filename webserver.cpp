#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "webserver.h"
#include "config.h"

AsyncWebServer server(80);

bool setupAP(){
    IPAddress local_IP(1,1,1,2);
    IPAddress gateway(1,1,1,1);
    IPAddress subnet(255,255,255,0);

    Serial.println("Setting soft-AP configuration ... ");
    if(!WiFi.softAPConfig(local_IP, gateway, subnet)){
        Serial.println("Failed softAPConfig");
        return false;
    }

    Serial.printf("Creating soft ap ssid=%s pass=%s ... \n", config.apSSID, config.apPassword);
    if(!WiFi.softAP(config.apSSID,config.apPassword)){
        Serial.println("Failed softAP");
        return false;
    }
    
    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());

    return true;
}

bool setupWebserver(){
  server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request){
    if(!request->hasParam("homeSSID")){
        request->send(400);
        return;
    }
    
    request->send(200);
  });
}