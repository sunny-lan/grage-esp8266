#include <ESP8266WiFi.h>
#include <DNSServer.h>        //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>      //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ArduinoOTA.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

char jsonBuf[512];

#include "config.h"
#include "io.h"
#include "ota.h"
#include "wifi.h"
#include "ws.h"


void setup()
{
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  if (
      setupConfig() ||
      setupIO() ||
      setupWifi(true) ||
      setupOTA() ||
      setupWS())
  {
    ESP.restart();
  }
}

void loop() 
{
  handleOTA();
  handleIO();
  handleWS();
}
