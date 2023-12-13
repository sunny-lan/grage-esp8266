#pragma once


enum struct WifiState {
    NONE,
    AP,
    STA,
    STA_CONNECTED
};

extern WifiState wifiState;

void handleWifi();