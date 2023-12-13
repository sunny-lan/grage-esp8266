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
  Serial.setDebugOutput(true);
  pinMode(config_pin, INPUT_PULLUP);
  
  if (
      setupConfig() 
       || setupOTA()
      || !setupWebserver()
      || setupWS()
  )
  {
    ESP.restart();
  }
}

int lastLevel = HIGH;

void onModeChanged(){
  Serial.printf("Config mode: %s\n", configMode?"on":"off");

}

void loop() 
{
  int pin = digitalRead(config_pin);
  configMode = (pin==LOW);

  if(pin!=lastLevel){
    onModeChanged();
  }

  handleWifi();
  handleOTA();
  handleWebserver();
  handleWS();

  if(configMode){
    if(wifiState==WifiState::AP){
    }
  }else{
    if(wifiState==WifiState::STA_CONNECTED) {
      handleIO();
    }
  }

  lastLevel = pin;
}
