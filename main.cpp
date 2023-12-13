#include <Arduino.h>

#include "shared.h"
#include "config.h"
#include "io.h"
#include "ota.h"
#include "ws.h"
#include "wifi.h"
#include "webserver.h"

static const uint8_t config_pin = D2;

void setup()
{
  Serial.begin(115200);
  pinMode(config_pin, INPUT_PULLUP);
  
  if (
      setupConfig() ||
      setupOTA() ||
      !setupWebserver()
  )
  {
    ESP.restart();
  }
}

int lastLevel = HIGH;

void onModeChanged(){
  Serial.printf("Config mode: %s\n", configMode?"on":"off");
  if(configMode)
  {
    startWebserver();
  }else{
    stopWebserver();
  }
}

void loop() 
{
  int pin = digitalRead(config_pin);
  configMode = (pin==LOW);

  if(pin!=lastLevel){
    onModeChanged();
  }

  handleWifi();

  if(configMode){
    if(wifiState==WifiState::AP){
      handleOTA();
    }
  }else{
    if(wifiState==WifiState::STA_CONNECTED) {
      handleIO();
      handleWS();
    }
  }

  lastLevel = pin;
}
