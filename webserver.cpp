#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "webserver.h"
#include "config.h"
#include "wifi.h"

AsyncWebServer server(80);


bool setupWebserver(){
  server.on("/query_wifi_list", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Scanning networks");

    WiFi.scanNetworksAsync([request](int count){
      Serial.println("Done scanning...Building response");
      AsyncResponseStream *response = request->beginResponseStream("text/plain");
      for(int i=0;i<count;i++)
      {
        Serial.printf("%s, %s\n", WiFi.SSID(i).c_str(), WiFi.BSSIDstr(i).c_str());
        response->print(WiFi.SSID(i));
        response->print(',');
        response->println(WiFi.BSSIDstr(i));
      }
      Serial.println("Sending response");
      request->send(response);
    });
    
  });
  server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request){
    Serial.println("Recieved req");

    if(!request->hasParam("homeSSID", true)){
        request->send(400);
        return;
    }
 
    if(!request->hasParam("homePassword", true)){
        request->send(400);
        return;
    }

    strcpy(config.homeSSID, request->getParam("homeSSID", true)->value().c_str());
    strcpy(config.homePassword, request->getParam("homePassword", true)->value().c_str());

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

bool serverStarted=false;

void startWebserver(){
  Serial.println("Starting webserver");
  server.begin();
  serverStarted=true;
}

void stopWebserver(){
  Serial.println("Stopping webserver");
  server.end();
  serverStarted=false;
}

void handleWebserver(){
  bool isAp =( wifiState==WifiState::AP);
  if(isAp!=serverStarted){
    if(isAp)startWebserver();
    else stopWebserver();
  }
}