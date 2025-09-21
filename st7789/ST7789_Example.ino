#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();   // Invoke library

void setup(void) {
  Serial.begin(115200);
  Serial.print("ST7789 TFT Bitmap Test");

  tft.begin();     // initialize a ST7789 chip
  // tft.setSwapBytes(true); // Swap the byte order for pushImage() - corrects endianness

  tft.fillScreen(TFT_RED);
}

void loop() {
    tft.fillScreen(TFT_RED);
    delay(2000);
    tft.fillScreen(TFT_BLUE);
    delay(2000);
}