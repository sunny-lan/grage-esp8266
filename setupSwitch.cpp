#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "setupSwitch.h"
#include "config.h"

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


void handleSwitch() {
  if(digitalRead(config_pin) == LOW){
    Serial.println("On demand portal requested by switch");
    
  }
}
