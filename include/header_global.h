#include "Arduino.h"
#include "Ticker.h"
#include <lvgl.h>
#include <demos/lv_demos.h>

#include <SPI.h>

#include <TFT_eSPI.h> // Hardware-specific library


void TaskWifi(void *pvParameters);