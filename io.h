#pragma once
#include <ArduinoJson.h>
void handleIO();
void handleCommand(const JsonObject& data);
bool readAndSend();