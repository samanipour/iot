#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();   // Invoke library

int cursorX = 10;     // فاصله از چپ
int cursorY = 10;     // موقعیت y شروع
int lineHeight = 35;  // فاصله بین خطوط


void setup(void) {
  Serial.begin(115200);
  Serial.print("ST7789 TFT Bitmap Test");

  tft.begin();     // initialize a ST7789 chip
  tft.setSwapBytes(true); // Swap the byte order for pushImage() - corrects endianness

  tft.setTextColor(TFT_GREEN, TFT_BLACK);  // رنگ متن سفید با پس‌زمینه مشکی
  // tft.setTextSize(2);

}

// تابع چاپ متن و رفتن به خط بعد
void printLine(String text, int size) {
  tft.setTextSize(size);
  tft.drawString(text, cursorX, cursorY);
  cursorY += lineHeight;  // y خط بعد رو افزایش میده
  // اگر رسیدیم به پایین صفحه، برگردیم بالا
  if(cursorY + lineHeight > tft.height()) {
    tft.fillScreen(TFT_BLACK);
    cursorY = 10;
  }
}



void loop() {
    tft.fillScreen(TFT_BLACK);
    cursorY = 10;  // بازنشانی موقعیت y به بالا

   
    // tft.setCursor(30, 30);        // مکان شروع
    printLine("Heart beat:",2);
    printLine("72",3);
    // printLine("Spo2: 150",2);
    // printLine("Air quality:",2);
    // printLine("324 ppm",2);


    // tft.drawString(" ",20,50);
    // tft.drawString(" ",20,70);
    // tft.drawString("Spo2:",20,90);
    // tft.drawString(" ",20,110);
    // tft.drawString(" ",20,130);
    // tft.drawString("Air quality:",20,150);


    delay(1500);

}

