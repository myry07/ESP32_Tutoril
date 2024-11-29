#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  displayInit();
}

void loop() {
  // put your main code here, to run repeatedly:
}

void displayInit() {
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setCursor(10, 10);
  tft.println("hello I'm Jiaqi, a handsome");
  tft.println("man from East Asian - China");
}
