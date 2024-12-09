/**
 * Face detection
 * ONLY WORKS ON ESP32S3
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/face/detection.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "fotos.h"

using eloq::camera;
using eloq::face_t;
using eloq::face::detection;

TFT_eSPI tft = TFT_eSPI();

/**
 * 
 */
void setup() {
  delay(3000);
  Serial.begin(115200);
  Serial.println("___FACE DETECTION___");

  pinMode(19, OUTPUT);

  // camera settings
  // !!!!REPLACE WITH YOUR OWN MODEL!!!!
  camera.pinout.wroom_s3();  // e.g. xiao(), lilygo_tcamera_s3(), ...
  camera.brownout.disable();
  // face detection only works at 240x240
  camera.resolution.face();
  camera.quality.high();

  // you can choose fast detection (50ms)
  detection.fast();
  // or accurate detection (80ms)
  detection.accurate();

  // you can set a custom confidence score
  // to consider a face valid
  // (in range 0 - 1, default is 0.5)
  detection.confidence(0.7);

  // init camera
  while (!camera.begin().isOk())
    Serial.println(camera.exception.toString());

  Serial.println("Camera OK");

  displayInit();
  Serial.println("Display OK");

  Serial.println("Awaiting for face...");
}

/**
 * 
 */
void loop() {
  // capture picture
  if (!camera.capture().isOk()) {
    Serial.println(camera.exception.toString());
    return;
  }

  // run detection
  if (!detection.run().isOk()) {
    Serial.println(detection.exception.toString());
    return;
  }

  Serial.printf(
    "Face(s) detected in %dms!\n",
    detection.benchmark.millis());

  // you can access the first detected face
  // at detection.first
  Serial.printf(
    " > face #1 located at (%d, %d)\n"
    "   proba is %.2f\n",
    detection.first.x,
    detection.first.y,
    detection.first.score);

  // if you expect multiple faces, you use forEach
  if (detection.found()) {
    if (detection.count() >= 1) {
      detection.forEach([](int i, face_t face) {
        Serial.printf(
          " > face #%d located at (%d, %d)\n",
          i + 1,
          face.x,
          face.y);

        tft.setCursor(25, 10);
        tft.println(detection.first.x);
        tft.setCursor(25, 20);
        tft.println(detection.first.y);
        digitalWrite(19, 1);

        tft.setCursor(80, 20);
        tft.println("has face");

        tft.pushImage(80, 50, MAN_HEIGHT, MAN_WIDTH, man);
      });
    }
  } else {
    digitalWrite(19, 0);
    tft.fillRect(25, 10, 30, 10, TFT_BLACK);
    tft.fillRect(25, 20, 30, 10, TFT_BLACK);
    tft.fillRect(80, 50, MAN_HEIGHT, MAN_WIDTH, TFT_BLACK);
    tft.fillRect(80, 20, 50, 10, TFT_BLACK);
    Serial.println("no face");
  }
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
