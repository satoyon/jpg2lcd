# Pico/Pico 2でPlatformIOを使いLCDにJPEG画像を表示するサンプル

[PlatformIO](https://piolabs.com/)に[Fork of Platformio Raspberry Pi RP2040: development platform for PlatformIO](https://github.com/maxgerhardt/platform-raspberrypi)をインストールしてPico/Pico 2の開発を行うサンプルです。

## Requirements

* Pico 2（Pico 1はplatform.iniの変更が必要）
* ST7789を使ったLCD（サンプルでは240×240ドットの[この製品](https://www.amazon.co.jp/dp/B07QG93NPB)を利用）

## Wire connection

|LCD側 | Pico側      | 機能 |
| ---- | ------------- | ----------------------------- |
|BLK   | 未接続        | バックライトコントロール端子 |
|DC    | GP20（26番）  | データ/コマンド切り替えピン |
|RES   | GP21（27番）  | リセット端子 |
|SDA   | GP19（25番）  | シリアルデータ入力 |
|SCL   | GP18（24番）  | SPIクロック入力 |
|VCC   | 3V3（36番）   | 電源 |
|GND   | GND（23番など）| GND |

## Libraries

以下のライブラリを利用しました

* [Adafruit_ST7789](https://github.com/adafruit/Adafruit-ST7735-Library/tree/master)
* [JPEGDEC](https://github.com/bitbank2/JPEGDEC/tree/master)
