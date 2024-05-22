#include "header_global.h"
#include "header_wifi.h"

IPAddress ip(192, 168, 40, 109);
IPAddress dns(192, 168, 40, 1);
IPAddress gateway(192, 168, 40, 1);
IPAddress subnet(255, 255, 255, 0);

bool run_wifi_init = false;
bool ota_initiated = false;
bool dhcp_get_client_name = true;

void getDHCPAttention(){
  const uint16_t port = 80;
  const char * host = "google.com"; // ip or dns

  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  while (!client.connect(host, port)) {
    Serial.println("Connection failed.");
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void TaskWifi(void *pvParameters){
    // Setup here
    pinMode(2, OUTPUT);
    wifi_init();
    // wifi.once(1, [](){run_wifi_init = true;});
    wifi.attach(1, [](){
      dhcp_get_client_name = true;
    });

    // Loop here
    while(true){
        // if(run_wifi_init)
        if(ota_initiated){
          ArduinoOTA.handle();
        }
        if(WiFi.status() == WL_CONNECTED){
          if(!otaProgressing)
          TELNET(TelnetStream.printf("Stack size : %d \n", uxTaskGetStackHighWaterMark(NULL)));
          // log_i("Stack size : %d", uxTaskGetStackHighWaterMark(NULL));
          if(dhcp_get_client_name){
            getDHCPAttention();
            dhcp_get_client_name = false;
            TELNET(TelnetStream.println("Pass DHCP"));
          }
        }
        delay(1000);
    }
}

void wifi_init(){
  if(!wifiInitiated){
    // WiFi.config(ip, gateway, subnet, dns);
    WiFi.onEvent(WiFiEvent);
    WiFi.setHostname("ESP-OTA");
    WiFi.begin(ssid, pass);
    
    wifiInitiated = true;
    timer_wifi_init = millis();
  }
  else if(!wifiConnected && millis() - timer_wifi_init > 2000){  // Ini boros loop
    wifiInitiated = false;
  }
  run_wifi_init = false;
}


void WiFiEvent(WiFiEvent_t event){
    switch(event) {
        case ARDUINO_EVENT_WIFI_STA_START:
            if(!led.active()) led.detach();
            ledfadecount = 0;
            led.attach(1, ledBlink);
            log_i("STA Start");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            log_i("STA Connected");
            if(!led.active()) led.detach();
            ledfadecount = 0;
            led.attach(1, ledBlinkTwice);
            wifiConnected = true;
            if(wifi.active()) wifi.detach();
            OTASetup();
            // WiFi.enableIpV6();
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            log_i("STA Got IP");
            if(!led.active()) led.detach();
            ledfadecount = 0;
            led.attach(1, ledBlinkThrice);
            log_i("Subnet Mask: %s", WiFi.subnetMask().toString());
            log_i("Gateway IP: %s", WiFi.gatewayIP().toString());
            log_i("DNS1 IP: %s", WiFi.dnsIP(0).toString());
            log_i("DNS1 IP: %s", WiFi.dnsIP(1).toString());
            Serial.println(WiFi.localIP().toString());

            TelnetStream.begin(23);
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            log_i("STA Disconnected");
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            log_i("STA Stopped");
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            log_i("AP STA IP Assigned");
            log_i("%s",WiFi.softAPBroadcastIP().toString());
            log_i("%s",WiFi.softAPIP().toString());
        default:
            break;
    }
}

void OTASetup(){
  ArduinoOTA.onStart(onStartFn)
      .onProgress(onProgressFn)
      .onEnd(onEndFn)
      .onError(onErrorFn);
  ArduinoOTA.setPort(3141);
  ArduinoOTA.begin();
  ota_initiated = true;
}

#pragma region Callback Functions
void onStartFn(){
  if(!led.active()) led.detach();
  led.attach_ms(200, ledBlinkTwice);
  Serial.println("OTA Started");
}

void onErrorFn(ota_error_t res){
  ESP.restart();
  Serial.printf("Error occured: %d", res);
}

void onProgressFn(unsigned int progress, unsigned int total){
  otaProgressing = true;
}

void onEndFn(){
  Serial.println("OTA Ended");
}
#pragma endregion

#pragma region LED Blinks
void ledBlink(){ // WIP: Gak ngeblink kalau udah jadi analogWrite
  switch (ledfadecount)
  {
  case 0:
    digitalWrite(2, HIGH);
    ledfadecount ++;
    break;
  case 1:
    digitalWrite(2, LOW);
    ledfadecount = 0;
    break;

  default:
    ledfadecount = 0;
    break;
  }
}

void ledBlinkTwice(){ // WIP: Gak ngeblink kalau udah jadi analogWrite
  switch (ledfadecount)
  {
  case 0:
    digitalWrite(2, HIGH);
    if(led.active())led.detach();
    led.attach_ms(200, ledBlinkTwice);
    ledfadecount ++;
    break;
  case 1:
    digitalWrite(2, LOW);
    ledfadecount ++;
    break;
  case 2:
    digitalWrite(2, HIGH);
    ledfadecount ++;
    break;
  case 3:
    digitalWrite(2, LOW);
    ledfadecount ++;
    break;
  case 4:
    digitalWrite(2, HIGH);
    ledfadecount ++;
    break;
  case 5:
    digitalWrite(2, LOW);
    if(led.active())led.detach();
    led.attach(2, ledBlinkTwice);
    ledfadecount = 0;
    break;

  default:
    ledfadecount = 0;
    break;
  }
}

void ledBlinkThrice(){ // WIP: Gak ngeblink kalau udah jadi analogWrite
  switch (ledfadecount)
  {
  case 0:
    digitalWrite(2, HIGH);
    if(led.active())led.detach();
    led.attach_ms(200, ledBlinkThrice);
    ledfadecount = 1;
    break;
  case 1:
    digitalWrite(2, LOW);
    ledfadecount ++;
    break;
  case 2:
    digitalWrite(2, HIGH);
    ledfadecount ++;
    break;
  case 3:
    digitalWrite(2, LOW);
    ledfadecount ++;
    break;
  case 4:
    digitalWrite(2, HIGH);
    ledfadecount ++;
    break;
  case 5:
    digitalWrite(2, LOW);
    ledfadecount ++;
    break;
  case 6:
    digitalWrite(2, HIGH);
    ledfadecount ++;
    break;
  case 7:
    digitalWrite(2, LOW);
    if(led.active())led.detach();
    led.attach(3, ledBlinkThrice);
    ledfadecount = 0;
    break;

  default:
    ledfadecount = 0;
    break;
  }
}

void ledDimming(){
  ledfadecount += 10;
  analogWrite(2, ledfadecount);
}

void ledDimmingFast(){
  ledfadecount += 20;
  analogWrite(2, ledfadecount);
}

#pragma endregion