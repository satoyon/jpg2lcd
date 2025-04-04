#include <Arduino.h>
#include "dma_st7789.hpp"
#include <JPEGDEC.h>

#define TFT_CS         17
#define TFT_RST        21
#define TFT_DC         20

// サンプル画像を埋め込む
__asm(\
  ".section \".rodata\" \n"\
  ".balign 4\n"\

  ".global _sample_picture\n"\
  ".global _picture_size\n"\

  "_sample_picture:\n"\
  ".incbin \"include/constructor.jpg\"\n"\
  ".set _picture_size, . - _sample_picture\n"\
  ".section \".text\"\n"\
);

extern const uint16_t _sample_picture[];
extern uint32_t _picture_size[];


// ST7789LCD（DMA付き）
dma_st7789 st7789_lcd = dma_st7789(TFT_CS, TFT_DC, TFT_RST);

// JPEGデコーダー
JPEGDEC jpeg;

// MCUを描画するコールバック関数
int mcu_draw(JPEGDRAW *draw)
{
  if( draw->iBpp == 16 ){
    st7789_lcd.transfer(draw->x, draw->y, draw->iWidth, draw->iHeight, (uint8_t *)(draw->pPixels) );
  }

  return 1;
}

void setup() {
  Serial1.begin(115200);
  // SPIクロック100MHz
  st7789_lcd.setSPISpeed(100*000*000);
  // 解像度240×240、SPIモード3
  st7789_lcd.init(240, 240, SPI_MODE3);
}


void loop() {
  st7789_lcd.fillScreen(ST77XX_BLACK);

  jpeg.openFLASH((uint8_t *)_sample_picture, (int)_picture_size, mcu_draw);
  jpeg.setPixelType(RGB565_BIG_ENDIAN);
  // 開始時間
  ulong start_time = micros();
  jpeg.decode(0,0,JPEG_USES_DMA);
  // 経過時間測定
  ulong elapsed = micros() - start_time;
  Serial1.printf("elapsed time %u usec.\n", elapsed);
  jpeg.close();
  sleep_ms(2000);
}

