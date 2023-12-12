#include <ESP8266WiFi.h>
#include <DNSServer.h>        //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <ArduinoOTA.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

char jsonBuf[512];

#include "config.h"
#include "io.h"
#include "ota.h"
#include "setupSwitch.h"
#include "ws.h"

static const uint8_t config_pin = D2;

void setup()
{
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  if (
      setupConfig() ||
      setupIO() ||
      setupSwitch() ||
      setupOTA() ||
      setupWS())
  {
    ESP.restart();
  }
}

void loop() 
{
  if(digitalRead(config_pin) == LOW){
    handleIO();
    handleWS();
  }else{
    handleOTA();
  }
}
