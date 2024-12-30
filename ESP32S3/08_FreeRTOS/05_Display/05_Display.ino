#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

static const uint16_t screenWidth = 480;
static const uint16_t screenHeight = 320;
static const byte LENGTH = 40;

typedef struct {
  byte pin;
  int time;
} LEDFlash;


// Touch
bool touch(int x, int y) {
  uint16_t touchX = 0, touchY = 0;
  bool pressed = tft.getTouch(&touchX, &touchY);
  if (pressed) {
    if (touchX >= x && touchX <= (x + LENGTH) && touchY >= y && touchY <= (y + LENGTH)) {
      return true;
    }
  }
  return false;
}

// Display
void displayInit() {
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  uint16_t calData[5] = { 353, 3568, 269, 3491, 7 };
  tft.setTouch(calData);
  tft.fillRect(400, 200, LENGTH, LENGTH, TFT_GREEN);

  Serial.println("Display Init");
}

void displayTask(void *param) {
  displayInit();
  
  LEDFlash *pt = (LEDFlash *)param;
  byte pin = pt->pin;
  int time = pt->time;

  pinMode(pin, OUTPUT);

  while (1) {
    if (touch(400, 200) && digitalRead(20) != 1) {
      digitalWrite(pin, 1);
      vTaskDelay(time);
    } else if (touch(400, 200) && digitalRead(20) == 1) {
      digitalWrite(pin, 0);
    }
  }
}

void setup() {
  LEDFlash led1;
  led1.pin = 20;
  led1.time = 3000;

  xTaskCreate(displayTask, "display flash", 1024, (void *)&led1, 1, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}
