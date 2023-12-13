#include <ESP8266WiFi.h>

#include "wifi.h"
#include "config.h"
#include "shared.h"

bool setupAP(){
    wifiState = WifiState::NONE;

    Serial.println("Changing to AP mode");
    if(!WiFi.mode(WIFI_AP)){
        Serial.println("Failed changing to AP mode");
        return false;
    }

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

void checkSTAConnected(){
    if(WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
        Serial.println("Waiting for connection to wifi");
        delay(1000);
        return;
    }

    wifiState = WifiState::STA_CONNECTED;

    Serial.println("Connection established!");  
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}

void handleWifi(){
    if(configMode){
        if(wifiState != WifiState::AP){
            setupAP();
        }
    }else{
        if(wifiState != WifiState::STA_CONNECTED){
            if(wifiState == WifiState::STA){
                checkSTAConnected();
            }else{
                setupSTA();
            }
        }
    }
}