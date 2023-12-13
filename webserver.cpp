#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "webserver.h"
#include "config.h"

AsyncWebServer server(80);

bool setupWebserver(){
  server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request){
    if(!request->hasParam("homeSSID")){
        request->send(400);
        return;
    }
 
    if(!request->hasParam("homePassword")){
        request->send(400);
        return;
    }

    strcpy(config.homeSSID, request->getParam("homeSSID")->value().c_str());
    strcpy(config.homePassword, request->getParam("homePassword")->value().c_str());

    Serial.printf("Recieved config ssid=%s pass=%s\n", config.homeSSID, config.homePassword);
    
    if(saveConfig()){
      Serial.println("Save config failed");
      request->send(500, "text/plain", config.deviceID);
      return;
    }

    Serial.printf("Save config success. Sending response %s\n", config.deviceID);

    request->send(200, "text/plain", config.deviceID);
  });

  return true;
}

void startWebserver(){
  Serial.println("Starting webserver");
  server.begin();
}

void stopWebserver(){
  Serial.println("Stopping webserver");
  server.end();
}