#pragma once

struct Config{
  char wsHost[256];
  char deviceName[32];
  char deviceID[64];
  char apSSID[32];
  char apPassword[32];
  char homeSSID[64];
  char homePassword[64];
};
extern Config config;

bool setupConfig();

bool loadConfig();
bool saveConfig();