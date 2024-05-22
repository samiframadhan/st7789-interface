#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <TelnetStream.h>

#ifndef SERIAL_ROS
  #define SER Serial
#endif

#define TELNET(fn) {\
  if(!otaProgressing)\
  fn;\
}

// put function declarations here:
void getDHCPAttention();
const char* ssid = "Nintendo";
const char* pass = "papahbaik";
bool wifiInitiated = false;
bool wifiConnected = false;
bool otaProgressing = false;
bool last_stat = LOW;
u16_t timer_wifi_init = 0;
u8_t ledfadecount = 0;

Ticker led;
Ticker wifi;

void WiFiEvent(WiFiEvent_t event);
void wifi_init();

void onStartFn();
void onProgressFn(unsigned int progress, unsigned int total);
void onEndFn();
void onErrorFn(ota_error_t error);

void ledBlink();
void ledBlinkTwice();
void ledBlinkThrice();
// void ledDimming();
// void ledDimmingFast();

void OTASetup();