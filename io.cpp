#include <Arduino.h>
#include <ArduinoJson.h>

#include "io.h"
#include "shared.h"
#include "config.h"
#include "ws.h"

const int minSendInterval = 100;
const int maxSendInterval = 5000;


void handleInterrupts();
void handlePeriodic();
void handleIO() {
  handleInterrupts();
  handlePeriodic();
}

volatile bool pinChanged = false;

void ICACHE_RAM_ATTR  handleInterrupt() {
  pinChanged = true;
}

void handleInterrupts()
{
  if (pinChanged)
  {
    if (!readAndSend())
      pinChanged = false;
  }
}

unsigned long lastSend = 0;
void handlePeriodic()
{
  if (millis() - lastSend > maxSendInterval)
  {
    readAndSend();
  }
}

bool readAndSend()
{
  //prevent spamming
  if (millis() - lastSend < minSendInterval) {
    return true;
  }
  lastSend = millis();
  StaticJsonDocument<512> doc;
  doc["type"] = "data";
  doc["id"] = config.deviceID;
  doc["fromDevice"]=true;
  JsonObject data = doc.createNestedObject("data");
  JsonArray pinReadings = data.createNestedArray("pinReadings");
  for (int pin = 0; pin <= 16; pin++) {
    pinReadings.add(digitalRead(pin));
  }
  for (int pin = 17; pin <= 17; pin++) {
    pinReadings.add(analogRead(pin));
  }
  serializeJson(doc, jsonBuf, sizeof(jsonBuf));
  sendBuf();
  return false;
}

void handleCommand(const JsonObject& data) {
  const char* command = data["command"];

  if (strcmp(command, "digitalWrite") == 0) {
    int pin = data["pin"], value = data["value"];
    Serial.printf("digitalWrite pin=%d value=%d\n", pin , value);
    digitalWrite(pin, value);
    readAndSend();
    
  } else if (strcmp(command, "pinMode") == 0) {
    int pin = data["pin"], mode = data["mode"];
    Serial.printf("pinMode pin=%d mode=%d\n", pin , mode);
    pinMode(pin, mode);
    
  } else if (strcmp(command, "attachInterrupt") == 0) {
    //TODO also force pinmode to input?
    //TODO check if interrupt is already attached

    int pin = data["pin"], mode = data["mode"];
    Serial.printf("attachInturrupt pin=%d mode=%d\n", pin , mode);
    attachInterrupt(digitalPinToInterrupt(pin), handleInterrupt, mode);

  } else if (strcmp(command, "detachInterrupt") == 0) {
    int pin = data["pin"];
    Serial.printf("detachInturrupt pin=%d\n", pin);
    detachInterrupt(digitalPinToInterrupt(pin));

  } else {
    Serial.printf("didn't recognize command: %s\n", command);
  }
}
