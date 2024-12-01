#include "esp_camera.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <Arduino.h>
#include <lvgl.h>
#include "ui.h"
// #define CrowPanel_24
//#define CrowPanel_28
#define CrowPanel_35

#if defined(CrowPanel_35)
/*screen resolution*/
static const uint16_t screenWidth = 480;
static const uint16_t screenHeight = 320;
uint16_t calData[5] = { 353, 3568, 269, 3491, 7 }; /*touch caldata*/

#elif defined(CrowPanel_24)
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;
uint16_t calData[5] = { 557, 3263, 369, 3493, 3 };

#elif defined(CrowPanel_28)
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;
uint16_t calData[5] = { 189, 3416, 359, 3439, 1 };
#endif


#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 15
#define SIOD_GPIO_NUM 4
#define SIOC_GPIO_NUM 5

#define Y2_GPIO_NUM 11
#define Y3_GPIO_NUM 9
#define Y4_GPIO_NUM 8
#define Y5_GPIO_NUM 10
#define Y6_GPIO_NUM 12
#define Y7_GPIO_NUM 18
#define Y8_GPIO_NUM 17
#define Y9_GPIO_NUM 16

#define VSYNC_GPIO_NUM 6
#define HREF_GPIO_NUM 7
#define PCLK_GPIO_NUM 13


TFT_eSPI tft = TFT_eSPI(); /* TFT entity */

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[screenWidth * screenHeight / 13];

//_______________________
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();

  int16_t x1 = screenWidth - area->x2 - 1;
  int16_t y1 = screenHeight - area->y2 - 1;

  tft.setAddrWindow(x1, y1, w, h);

  for (uint32_t i = 0; i < w * h; i++) {
    uint16_t color = color_p[w * h - 1 - i].full;  // 倒序绘制
    tft.pushColor(color);
  }

  tft.endWrite();

  lv_disp_flush_ready(disp);
}

uint16_t touchX, touchY;
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  bool touched = tft.getTouch(&touchX, &touchY, 600);
  if (!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR;

    data->point.x = screenWidth - touchX - 1;
    data->point.y = screenHeight - touchY - 1;

    Serial.print("Data x: ");
    Serial.println(data->point.x);
    Serial.print("Data y: ");
    Serial.println(data->point.y);
  }
}

void cameraInit() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 2;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);  //初始化失败
    return;
  } else {
    Serial.println("Camera init");
  }

  delay(3000);
}

void showingImage() {
  camera_fb_t* fb = esp_camera_fb_get();  // 获取帧缓冲
  if (!fb) {
    Serial.println("Failed to capture image");  // 无法捕获图片
    return;
  }

  if (fb->format != PIXFORMAT_JPEG) {
    Serial.println("no JPEG format");  // 非JPEG
  } else {
    Serial.println("Captured JPEG image");

    int offsetX = 0;  //左右偏移 +右 -左
    int offsetY = 40;  //上下偏移 +下 -上

    if (TJpgDec.drawJpg(offsetX, offsetY, (const uint8_t*)fb->buf, fb->len)) {
      Serial.println("JPEG drawn successfully.");
    } else {
      Serial.println("JPEG draw failed.");  // 超出画框
    }
  }

  esp_camera_fb_return(fb);  // 释放缓冲
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if (y >= tft.height()) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

void displayInit() {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);
}

void setup() {
  Serial.begin(115200); /*serial init */

  pinMode(48, OUTPUT);

  //LCD init
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTouch(calData);
  delay(100);

  //lvgl init
  lv_init();

  lv_disp_draw_buf_init(&draw_buf, buf1, NULL, screenWidth * screenHeight / 13);

  /* Initialize the display */
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /* Initialize the (dummy) input device driver */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  tft.fillScreen(TFT_BLACK);

  // lv_demo_widgets();  // LVGL demo
  ui_init();

  Serial.println("Setup done");
}


void loop() {
  lv_timer_handler();
  lv_tick_inc(5);  //放置5ms过去
  delay(5);
}
