#include "camera_tft.h"
#include "esp_camera.h"
#include <JPEGDecoder.h>
#include <Arduino.h>

static Adafruit_ST7735* tft = nullptr;
static bool active = false;

// ===== CAMERA PINS =====
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39

#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     13

// ================= JPEG DRAW =================
static void drawJPEG(int xpos, int ypos)
{
  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;

  while (JpegDec.read()) {
    pImg = JpegDec.pImage;

    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    tft->drawRGBBitmap(mcu_x, mcu_y, pImg, mcu_w, mcu_h);
  }
}

//////////////////////////////////////////////////////////////
// INIT
//////////////////////////////////////////////////////////////
void camera_init(Adafruit_ST7735* display)
{
  tft = display;
}

//////////////////////////////////////////////////////////////
// START (CRITICAL FIXED ORDER)
//////////////////////////////////////////////////////////////
void camera_start()
{
  if (active) return;
  if (!tft) return;

  delay(200);   // 🔥 VERY IMPORTANT (camera stability)

  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;

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
  config.pin_href  = HREF_GPIO_NUM;

  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;

  config.pin_pwdn  = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;

  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size   = FRAMESIZE_QVGA;
  config.jpeg_quality = 9;
  config.fb_count     = 2;

  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.println("Camera failed");
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_hmirror(s, 0);
  s->set_vflip(s, 1);

  active = true;
}

//////////////////////////////////////////////////////////////
// STOP
//////////////////////////////////////////////////////////////
void camera_stop()
{
  if (!active) return;

  esp_camera_deinit();
  active = false;

  if (tft) tft->fillScreen(ST77XX_BLACK);
}

//////////////////////////////////////////////////////////////
// LOOP
//////////////////////////////////////////////////////////////
void camera_loop()
{
  if (!active || !tft) return;

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) return;

  JpegDec.decodeArray(fb->buf, fb->len);

  int x = (128 - JpegDec.width) / 2;
  int y = (160 - JpegDec.height) / 2;

  drawJPEG(x, y);

  esp_camera_fb_return(fb);
}

//////////////////////////////////////////////////////////////
bool camera_isActive()
{
  return active;
}