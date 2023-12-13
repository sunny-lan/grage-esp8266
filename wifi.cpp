#include <ESP8266WiFi.h>

#include "wifi.h"
#include "config.h"
#include "shared.h"

bool setupAP(){
    wifiState = WifiState::NONE;
    Serial.println("Changing to AP mode");
    
  #ifdef ESP8266
    // @bug workaround for bug #4372 https://github.com/esp8266/Arduino/issues/4372
    if(!WiFi.mode(WIFI_AP)) {
      #ifdef WM_DEBUG_LEVEL
      DEBUG_WM(WM_DEBUG_ERROR,F("[ERROR] enableAP failed!"));
      #endif
      return false;
    }
    delay(500); // workaround delay
  #endif

    IPAddress local_IP(192,168,1,2);
    IPAddress gateway(192,168,1,1);
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
    
    delay(500); // slight delay to make sure we get an AP IP
    
    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());

    wifiState = WifiState::AP;

    return true;
}

WifiState wifiState = WifiState::NONE;

bool setupSTA(){
    wifiState = WifiState::NONE;
    
    Serial.println("Changing to station mode");
    if(!WiFi.mode(WIFI_STA)){
        Serial.println("Failed changing to STA mode");
        return false;
    }

    Serial.print("Connecting to ");
    Serial.print(config.homeSSID); Serial.println(" ...");
    wl_status_t code = WiFi.begin(config.homeSSID, config.homePassword);             // Connect to the network
    
    wifiState = WifiState::STA;

    return true;
}

bool checkSTAConnected(){
    if(WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
        Serial.println("Waiting for connection to wifi");
        return false;
    }

    wifiState = WifiState::STA_CONNECTED;

    Serial.println("Connection established!");  
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
    return true;
}

void handleWifi(){
    bool res=true;
    if(configMode){
        if(wifiState != WifiState::AP){
            res=setupAP();
        }
    }else{
        if(wifiState != WifiState::STA_CONNECTED){
            if(wifiState == WifiState::STA){
                res=checkSTAConnected();
            }else{
                res=setupSTA();
            }
        }
    }
    if(!res)delay(1000);
}