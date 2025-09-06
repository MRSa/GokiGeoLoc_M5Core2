# GokiGeoLoc_M5Core2

M5Stack Core2 w/GNSS module(M135) Control

- [M5Stack Core2](https://docs.m5stack.com/ja/core/core2)
- [Module GNSS(M135)](https://docs.m5stack.com/en/module/GNSS%20Module)

## 機能

初期表示は、「地図表示」モードです。ズームレベルは16をデフォルトにしています。

- ボタンA : 地図表示 (現在位置を地図上に表示)
  - 右上をタッチすると、ズームレベルを変更できます。（複数のズームレベルの地図を入れていた場合）
- ボタンB : 受信した災危通報の表示
  - 右上をタッチすると、文字のサイズが変わります。
  - 左半分をタッチすると、前の災危情報を表示します。
  - 右半分をタッチすると、次の災危情報を表示します。
- ボタンC : 詳細表示 (文字で情報を表示)
  - 右上をタッチすると、画面の輝度が変わります。

## 地図の置き方

あらかじめ、SDカードに /GsiMap フォルダを作成し、その下に ズームレベルごと、数字名のサブフォルダを作成、その下に地理院地図のマップタイル画像（png形式の地図）を置きます。（例： /GsiMap/16/58180/25822.png  ← 地理院地図のズームレベル 16 の X:58180, Y:25822 である、png形式のタイル画像）

ズームレベルは16をデフォルトにしていますので、ズームレベル 16の地図をSDカードに格納することが望ましいです。
地図をまったりダウンロードする batファイルが、 getGSImaps/GsiMap/（ズームレベル） の下に置いてありますので、参考にしていただけると幸いです。

SDカードは、FAT32形式でフォーマットしてください。（FAT32形式でフォーマットされている microSDカードであれば、128GBでも認識するようです。）

## 参考情報

- [M5Unifiedライブラリの始め方](https://docs.m5stack.com/ja/arduino/m5unified/helloworld)
  - [M5Unified](https://github.com/m5stack/M5Unified)
- [M5GFX Setup](https://docs.m5stack.com/ja/arduino/m5gfx/m5gfx)
  - [M5GFX](https://github.com/M5Stack/M5GFX)
- [地理院地図](https://maps.gsi.go.jp/help/howtouse.html)
- [タイル座標確認ページ(地理院地図)](https://maps.gsi.go.jp/development/tileCoordCheck.html#16/35.6104/139.5950)
- [みちびき 災害・危機管理通報サービス](https://qzss.go.jp/technical/system/dcr.html)
- [ユーザインタフェース仕様書（IS-QZSS）: IS-QZSS-DCR-xxx および IS-QZSS-DCX-xxx](https://qzss.go.jp/technical/download/ps-is-qzss.html)
- [災害・危機管理通報サービス 試験用データ配信](https://qzss.go.jp/technical/dod/dc-report/test-data-distribution.html)
- [GPSシールドを使ってみちびき（QZSS）の災害・危機管理通報サービス（DC Report・DCX）を受信してみた。](https://www.switch-science.com/blogs/magazine/gps-qzss-dc-report-dcx-receiving)
  - QZSSDCX: [https://github.com/SWITCHSCIENCE/QZSSDCX](https://github.com/SWITCHSCIENCE/QZSSDCX)
-[u-blox GNSSモジュール+ESP32でみちびき（QZSS）の「災危通報」情報を受信する](https://qiita.com/ta-oot/items/d721de91bfcbd952574b)
  - QZQSM: [https://github.com/baggio63446333/QZQSM](https://github.com/baggio63446333/QZQSM)
- [Unit Mini IMU-Pro](https://docs.m5stack.com/ja/unit/IMU%20Pro%20Mini%20Unit)
  - [https://github.com/m5stack/M5Unit-IMU-Pro-Mini/](https://github.com/m5stack/M5Unit-IMU-Pro-Mini/)
