#include <FS.h>
#include <Arduino.h>

#include "config.h"

Config config;

void setConfigDefaults();

bool setupConfig()
{
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) 
  {
    Serial.println("mounted file system");
  }
  else
  {
    Serial.println("failed to mount file system");
    return 1;
  }

  if(loadConfig() || strlen(config.deviceID)==0)
  {
    Serial.println("failed to load config file, load defaults");
    setConfigDefaults();
  }

  Serial.print("before config wsHost = "); Serial.println(config.wsHost);
  Serial.print("before config deviceName = "); Serial.println(config.deviceName);
  Serial.print("before config deviceID = "); Serial.println(config.deviceID);
  Serial.print("before config apSSID = "); Serial.println(config.apSSID);
  Serial.print("before config apPassword = "); Serial.println(config.apPassword);
  
  return 0;
}


const char *configFilePath = "/config.bin";

bool loadConfig()
{
  if (SPIFFS.exists(configFilePath)) 
  {
    //file exists, reading and loading
    Serial.println("reading config file");
    File configFile = SPIFFS.open(configFilePath, "r");
    
    if (configFile) 
    {
      Serial.println("opened config file");
      size_t amt_read = configFile.readBytes((char*)&config, sizeof(config));
      
      Serial.printf("read config file sz=%zu expected=%zu",
        amt_read, sizeof(config));

      configFile.close();
      

      if(amt_read!=sizeof(config)){
        Serial.println("size not matching - loadconfig failed");
        return true; 
      }
      return 0;
    }
  }

  return 1;
}

char randomChar(char st, char ed)
{
  byte rng = random(0, ed-st);
  return rng + st;
}

void genRandomID(char st, char ed, int len)
{
  for(int i=0;i<len;i++)
  {
    config.deviceID[i] = randomChar(st, ed);
  }
  config.deviceID[len]='\0';
}

void setConfigDefaults()
{
  strcpy(config.wsHost, "grage.azurewebsites.net");
  strcpy(config.deviceName, "grage0000");
  strcpy(config.apSSID, "grage0000");
  strcpy(config.apPassword, "00000000");
  genRandomID('a','z', 32);
}

bool saveConfig()
{
    File configFile = SPIFFS.open(configFilePath, "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
      return 1;
    }

    Serial.println("wroting to config file");
    configFile.write((char*)&config, sizeof(config));
    
    Serial.println("wrote config file");
    configFile.close();
    return 0;
}
