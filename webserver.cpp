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

    request->send(200, "text/plain", config.deviceID);
  });
}