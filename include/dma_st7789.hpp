#ifndef __DMA_ST7789
#define __DMA_ST7789

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#include "hardware/dma.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

class dma_st7789 : public Adafruit_ST7789 {

    private:
        volatile bool data_transfer_flag;
        int dma_channel;
        dma_channel_config c;
        static dma_st7789   *instance_ptr;  // インスタンス
    
        // 割り込みハンドラ（動的）
        void dma_interrupt_handler() {
            // 割り込みフラグをクリア
            dma_hw->ints0 = 1u << dma_channel;
            // SPI転送の終了処理
            this->endWrite();
            // 転送中フラグをリセット
            data_transfer_flag = false;
        };
    
    public:
        // コンストラクタ
        dma_st7789(int8_t cs, int8_t dc, int8_t rst) : Adafruit_ST7789(cs, dc, rst) {
            data_transfer_flag = false;
            instance_ptr = this;    // インスタンスを保存

            // DMAの設定
            dma_channel = dma_claim_unused_channel(true);
            c = dma_channel_get_default_config(dma_channel);
            channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
            channel_config_set_read_increment(&c, true);
            channel_config_set_write_increment(&c, false);
            channel_config_set_dreq(&c, DREQ_SPI0_TX);
            dma_channel_set_irq0_enabled(dma_channel, true);
    
            // 割り込みハンドラとして静的関数を登録
            irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler_static);
            irq_set_enabled(DMA_IRQ_0, true);
        };
    
        // 転送開始
        void transfer(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *buffer) {
            // 1つ前（があればそれ）のDMAの転送完了を待つ
            (void)wait_for_transfer();
    
            uint size = width * height * sizeof(uint16_t);
            this->setAddrWindow(x, y, width, height);
            this->startWrite();
            data_transfer_flag = true;
            // 転送
            dma_channel_configure(
                dma_channel,
                &c,
                &spi_get_hw(spi0)->dr,
                buffer,
                size,
                true
            );
        };
    
        // 転送完了待機（タイムアウト付き）
        bool wait_for_transfer(int timeout_ms = 1000) {
            bool retval = true;
    
            ulong start_time = millis();
            while( data_transfer_flag ) {
                if( (millis() - start_time) > timeout_ms)
                 break;
            }
            if(data_transfer_flag) retval = false;
    
            return retval;
        };
    
        // 割り込みハンドラ（静的）
        static void dma_irq_handler_static(void) {
            if(instance_ptr) {
                instance_ptr->dma_interrupt_handler();
            }
        };
};

// インスタンスを初期化
dma_st7789  *dma_st7789::instance_ptr = nullptr;

#endif
