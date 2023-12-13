#include <WebSocketsClient.h>
#include <ArduinoJson.h>

#include "ws.h"
#include "shared.h"
#include "config.h"
#include "io.h"
#include "wifi.h"

const char *wsPath = "/ws";

WebSocketsClient ws;

void handleWsEvent(WStype_t type, uint8_t *payload, size_t length);
bool setupWS()
{
  // event handler
  ws.onEvent(handleWsEvent);

  // try ever 5000 again if connection has failed
  ws.setReconnectInterval(10000);

  return 0;
}

const long refreshRate = 60 * 1000;
long lastConnection = 0;
int reconnectTries = 0, maxRetries = 30;
bool wsConnected = false;
void connectChannel();
bool handleMessage(char *payload, size_t length);
void refreshConnection() {
  if (millis() - lastConnection > refreshRate) {
    connectChannel();
  }
}

void connectChannel() {
  StaticJsonDocument<128> doc;
  doc["type"] = "connect";
  doc["id"] = config.deviceID;
  doc["fromDevice"]=true;
  serializeJson(doc, jsonBuf, sizeof(jsonBuf));
  sendBuf();
  lastConnection = millis();
}

void initWSConnection() {
  connectChannel();
}

void sendBuf() {
  ws.sendTXT(jsonBuf);
}

void handleWsEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
    case WStype_DISCONNECTED:
      wsConnected = false;
      Serial.printf("[WSc] Disconnected! Retry: %d\n", reconnectTries);
      reconnectTries++;
      break;
    case WStype_CONNECTED:
      wsConnected = true;
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      reconnectTries = 0; 
      initWSConnection();
      break;
    case WStype_TEXT:
      reconnectTries = 0;
      if (handleMessage((char*)payload, length)) {
        //TODO handle error
      }
      break;
    case WStype_BIN:
      //ignore
      break;
  }
}

bool handleMessage(char *payload, size_t length) {
  StaticJsonDocument<256> doc;
  Serial.println(payload);
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    Serial.println(err.c_str());
    return true;
  }
  JsonObject root = doc.as<JsonObject>();
  const char* command = root["type"];
  if (strcmp(command, "data") == 0) {
    handleCommand(root["data"]);
  } else if (strcmp(command, "rping") == 0) {
    return readAndSend();
  } else if (strcmp(command, "error") == 0) {
    Serial.println("Server sent error:");
    Serial.println(root["error"].as<const char*>());
    return true;
  }
  return false;
}

bool isWsRunning = false;

void handleWS() {
  bool shouldWsRun = (wifiState == WifiState::STA_CONNECTED);
  if(shouldWsRun!=isWsRunning){
    if(shouldWsRun){
      
      // server address, port and URL
      //ws.begin(config.wsHost, 80, wsPath); //TODO get ssl working
      ws.beginSSL(config.wsHost, 443, wsPath);
      isWsRunning=true;
    }else{
      
    }
  }

  if(shouldWsRun){
    if (wsConnected)
      refreshConnection();
    ws.loop();
  }
}
