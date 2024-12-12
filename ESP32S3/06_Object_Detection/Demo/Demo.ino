#include <Car_inferencing.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/edgeimpulse/fomo.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "fotos.h"

using eloq::camera;
using eloq::ei::fomo;

TFT_eSPI tft = TFT_eSPI();


/**
 * 
 */
void setup() {
  delay(3000);
  Serial.begin(115200);
  Serial.println("__EDGE IMPULSE FOMO (NO-PSRAM)__");

  pinMode(19, OUTPUT);

  // camera settings
  // replace with your own model!
  camera.pinout.wroom_s3();
  camera.brownout.disable();
  // NON-PSRAM FOMO only works on 96x96 (yolo) RGB565 images
  camera.resolution.yolo();
  camera.pixformat.rgb565();

  // init camera
  while (!camera.begin().isOk())
    Serial.println(camera.exception.toString());

  Serial.println("Camera OK");

  displayInit();
  Serial.println("Display OK");

  Serial.println("Put object in front of camera");
}


void loop() {
  // capture picture
  if (!camera.capture().isOk()) {
    Serial.println(camera.exception.toString());
    return;
  }

  // run FOMO
  if (!fomo.run().isOk()) {
    Serial.println(fomo.exception.toString());
    return;
  }

  // how many objects were found?
  Serial.printf(
    "Found %d object(s) in %dms\n",
    fomo.count(),
    fomo.benchmark.millis());

  // if no object is detected, return
  // if (!fomo.foundAnyObject())
  //   return;

  // if you expect to find a single object, use fomo.first

  if (fomo.foundAnyObject()) {
    Serial.printf(
      "Found %s at (x = %d, y = %d) (size %d x %d). "
      "Proba is %.2f\n",
      fomo.first.label,
      fomo.first.x,
      fomo.first.y,
      fomo.first.width,
      fomo.first.height,
      fomo.first.proba);

    tft.setCursor(25, 10);
    tft.println(fomo.first.x);
    tft.setCursor(25, 20);
    tft.println(fomo.first.y);
    digitalWrite(19, 1);

    tft.setCursor(80, 20);
    tft.println("Obj det");

    tft.pushImage(80, 50, MAN_HEIGHT, MAN_WIDTH, man);
  } else {
    digitalWrite(19, 0);
    tft.fillRect(25, 10, 30, 10, TFT_BLACK);
    tft.fillRect(25, 20, 30, 10, TFT_BLACK);
    tft.fillRect(80, 50, MAN_HEIGHT, MAN_WIDTH, TFT_BLACK);
    tft.fillRect(80, 20, 50, 10, TFT_BLACK);
    Serial.println("no Obj");
  }

  // if you expect to find many objects, use fomo.forEach
  // if (fomo.count() > 1) {
  //   fomo.forEach([](int i, bbox_t bbox) {
  //     Serial.printf(
  //       "#%d) Found %s at (x = %d, y = %d) (size %d x %d). "
  //       "Proba is %.2f\n",
  //       i + 1,
  //       bbox.label,
  //       bbox.x,
  //       bbox.y,
  //       bbox.width,
  //       bbox.height,
  //       bbox.proba
  //     );
  //   });
  // }
}


void displayInit() {
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(1);
  tft.setCursor(10, 10);
  tft.print("x: ");
  tft.setCursor(10, 20);
  tft.print("y: ");
} 