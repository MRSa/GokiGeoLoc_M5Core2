# GokiGeoLoc_M5Core2

[M5Stack Core2](https://docs.m5stack.com/en/core/core2) に [Module GNSS(M135)](https://docs.m5stack.com/en/module/GNSS%20Module) をつなげて、地図や災危情報、センサデータを見えるようにしてみた件

## 機能

本リポジトリでは、[M5Stack Core2](https://docs.m5stack.com/en/core/core2) に [Module GNSS(M135)](https://docs.m5stack.com/en/module/GNSS%20Module) を使用して、次の機能を実現します。

- **現在位置の表示**
  - [Module GNSS(M135)](https://docs.m5stack.com/ja/module/GNSS%20Module) で受信した緯度経度の情報から、M5Stack Core2のmicroSDカードに入れた[地理院地図](https://maps.gsi.go.jp/help/howtouse.html)を使って現在位置を表示する
- **現在方位の表示**
  - [Module GNSS(M135)](https://docs.m5stack.com/ja/module/GNSS%20Module)、もしくは [Unit Mini IMU-Pro](https://docs.m5stack.com/ja/unit/IMU%20Pro%20Mini%20Unit) に搭載された地磁気センサで北の方角を示す
- **オフライン地図の表示**
  - あらかじめmicroSDカードにダウンロードした[地理院地図](https://maps.gsi.go.jp/help/howtouse.html)を画面に地図を表示する
- **災危通報の表示**
  - [Module GNSS(M135)](https://docs.m5stack.com/ja/module/GNSS%20Module) で受信した[災危通報](https://qzss.go.jp/technical/system/dcr.html)を最大999件蓄積し、表示する
- **搭載センサ情報の表示**
  - [Module GNSS(M135)](https://docs.m5stack.com/ja/module/GNSS%20Module) に搭載されたセンサ([BMI270](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi270-ds000.pdf), [BMM150](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmm150-ds001.pdf), [BMP280](https://www.bosch-sensortec.com/media/boschsensortec/downloads/product_flyer/bst-bmp280-fl000.pdf))のデータを表示する
- **USBシリアルポート経由でデータ転送**
  - USB シリアルポート経由で、センサの情報、蓄積した最大999件の[災危通報](https://qzss.go.jp/technical/system/dcr.html)を[M5Stack Core2](https://docs.m5stack.com/ja/core/core2)から転送する

## システム構成

本リポジトリで使用するシステム構成を示します。

![システム構成図](https://github.com/MRSa/GokiGeoLoc_M5Core2/blob/main/images/system.png?raw=true)

- [M5Stack Core2](https://docs.m5stack.com/ja/core/core2)
  - [地理院地図](https://maps.gsi.go.jp/help/howtouse.html)を表示するために、microSDカードの搭載が必要
- [Module GNSS(M135)](https://docs.m5stack.com/ja/module/GNSS%20Module)
- [Unit Mini IMU-Pro](https://docs.m5stack.com/ja/unit/IMU%20Pro%20Mini%20Unit) (オプション)
  - [Unit Mini IMU-Pro](https://docs.m5stack.com/ja/unit/IMU%20Pro%20Mini%20Unit) を [M5Stack Core2](https://docs.m5stack.com/ja/core/core2)に接続して起動すると、このユニットのセンサを優先して利用します
- PC等 (オプション)
  - [M5Stack Core2](https://docs.m5stack.com/ja/core/core2)とUSBシリアル通信でつなぎ、受信した災危通報メッセージを転送する場合

## ビルド環境・使用ライブラリ

- [Arduino IDE](https://www.arduino.cc/en/software/)
- [M5unified](https://github.com/m5stack/M5Unified)
- [M5GFX](https://github.com/m5stack/M5GFX)
- [TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus)
- [QZQSM](https://github.com/baggio63446333/QZQSM)
- [QZSSDCX](https://github.com/SWITCHSCIENCE/QZSSDCX)
- [ArduinoJson](https://arduinojson.org/)

## 操作説明

電源を入れると、初期化処理を行った後、**現在位置表示** となります。
以下に画面イメージと操作を示します。

![画面イメージと操作](https://github.com/MRSa/GokiGeoLoc_M5Core2/blob/main/images/instruction.png?raw=true)

- **左ボタン** : 現在位置表示/地図表示 (現在位置を地図中心に表示、地図を表示)
  - 左上の題名部分をタッチすることで、**現在位置表示** と **地図表示** を切り替えることができます。
  - 地図表示では、画面上の上下左右をタッチして表示位置を移動させることができます。
  - 複数のズームレベルの地図をSDカードに入れていた場合、右上(Zマーク)をタッチすると、ズームレベルを変更できます。
- **中ボタン** : 受信した災危通報の表示
  - 災危通報は、最大999件蓄積することができます。
  - 右上(Fマーク)をタッチすると、文字のサイズが変わります。情報が多く、画面からはみ出す場合には文字サイズの変更を試してください。
  - 画面の左半分をタッチすると、前の災危情報を表示します。
  - 画面の右半分をタッチすると、次の災危情報を表示します。
  - 画面右上に、メッセージを先頭、末尾に移動するボタンがあります。
- **右ボタン** : 詳細表示 (文字で情報を表示)
  - センサの情報を一覧で表示します。
  - 右上(Bマーク)をタッチすると、画面の輝度を変えることができます。だんだん明るくなり、最大輝度になった後は、最小輝度になります。

## 地図タイルの置き方

地図を表示するためには、あらかじめ国土地理院の[地理院タイル](https://maps.gsi.go.jp/development/siyou.html)の画像をダウンロードし、SDカードに置いておく必要があります。
あらかじめ、SDカードに **/GsiMap** フォルダを作成し、その下に ズームレベルごと、数字名のサブフォルダを作成、その下に地理院地図のマップタイル画像（png形式の地図）を置きます。
（例： /GsiMap/16/58180/25822.png  ← 地理院地図のズームレベル 16 の X:58180, Y:25822 である、png形式のタイル画像）

ズームレベルは16をデフォルトにしていますので、ズームレベル 16の地図をSDカードに格納することが望ましいです。
地図をまったりダウンロードする batファイルが、 getGSImaps/GsiMap/（ズームレベル） の下に置いてありますので、参考にしていただけると幸いです。

SDカードは、FAT32形式でフォーマットしてください。FAT32形式でフォーマットされている microSDカードであれば、128GBでも認識するようです。

## 画面説明

### 地図表示

衛星から拾った現在位置を地図の中心に表示します。合わせて、内臓センサの情報（気温と気圧）を表示します。なお、地図のズームレベルは16をデフォルトにしています。

### 災危通報の表示

衛星（みちびき）から送られてくる、災危通報という情報を表示します。最大300件程度記憶しています。
なお、受信情報は電源を入れている間だけ記憶しています。電源をOFFすると忘れます。

### 詳細表示（文字で情報を表示）

センサ情報等を文字で表示します。

## 参考情報

### 地図関連

- [地理院地図](https://maps.gsi.go.jp/help/howtouse.html)
- [地理院タイルの仕様](https://maps.gsi.go.jp/development/siyou.html)
- [地理院タイルのご利用について](https://maps.gsi.go.jp/development/ichiran.html)
- [国土地理院コンテンツ利用規約](https://www.gsi.go.jp/kikakuchousei/kikakuchousei40182.html)
- [タイル座標確認ページ(地理院地図)](https://maps.gsi.go.jp/development/tileCoordCheck.html#16/35.6104/139.5950)

### 災危通報関連

- [みちびき 災害・危機管理通報サービス](https://qzss.go.jp/technical/system/dcr.html)
- [みちびき 災害・危機管理通報サービス（拡張）のサービス開始及び共通EWSメッセージフォーマットの採用について](https://qzss.go.jp/info/information/dcx_240401.html)
- [ユーザインタフェース仕様書（IS-QZSS）: IS-QZSS-DCR-xxx および IS-QZSS-DCX-xxx](https://qzss.go.jp/technical/download/ps-is-qzss.html)
- [災害・危機管理通報サービス 試験用データ配信](https://qzss.go.jp/technical/dod/dc-report/test-data-distribution.html)
- [災害・危機管理通報サービス（拡張）試験用データ配信](https://qzss.go.jp/technical/dod/dc-report/dcx-test-data-distribution.html)

- [u-blox GNSSモジュール+ESP32でみちびき（QZSS）の「災危通報」情報を受信する](https://qiita.com/ta-oot/items/d721de91bfcbd952574b)
  - QZQSM: [https://github.com/baggio63446333/QZQSM](https://github.com/baggio63446333/QZQSM)
- [GPSシールドを使ってみちびき（QZSS）の災害・危機管理通報サービス（DC Report・DCX）を受信してみた。](https://www.switch-science.com/blogs/magazine/gps-qzss-dc-report-dcx-receiving)
  - QZSSDCX: [https://github.com/SWITCHSCIENCE/QZSSDCX](https://github.com/SWITCHSCIENCE/QZSSDCX)
- [QZSS DCR Decoder Azarashi](https://github.com/nbtk/azarashi)
  - [azarashi(PyPI)](https://pypi.org/project/azarashi/)
  - シリアル経由でPCに送り付けるJSONデータフォーマットの解釈に使用します。ただし、[NEO-M9NのUBX-RXM-SFRBXメッセージが正常にデコードされるよう修正 #18](https://github.com/nbtk/azarashi/pull/18) が取り込まれている必要があるようです。

### 地磁気関連

- [地図と位置情報](https://internet.watch.impress.co.jp/docs/column/chizu3/1431412.html)
- [国土地理院 磁気図](https://www.gsi.go.jp/buturisokuchi/menu03_magnetic_chart.html)
- [国土地理院 地磁気測量](https://vldb.gsi.go.jp/sokuchi/geomag/menu_04/index.html)
- [国土地理院 地磁気を知る](https://www.gsi.go.jp/buturisokuchi/menu01_index.html)

### プログラミング関連

- [M5Unifiedライブラリの始め方](https://docs.m5stack.com/ja/arduino/m5unified/helloworld)
  - [M5Unified](https://github.com/m5stack/M5Unified)
  - [M5Unified IMU Class](https://docs.m5stack.com/en/arduino/m5unified/imu_class)
  - [IMU sample Code](https://github.com/m5stack/M5Unified/blob/master/examples/Basic/Imu/Imu.ino)
- [M5GFX Setup](https://docs.m5stack.com/ja/arduino/m5gfx/m5gfx)
  - [M5GFX](https://github.com/M5Stack/M5GFX)

### センサー関連

- [Adafruit_BMP280_Library](https://github.com/adafruit/Adafruit_BMP280_Library/tree/master)
- [BMP280 Digital Pressure Sensor(datasheet)](https://cdn-shop.adafruit.com/datasheets/BST-BMP280-DS001-11.pdf)
- [u-blox Protocol Specification](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/products/module/GNSS%20Module/u-blox8-M8_ReceiverDescrProtSpec__UBX-13003221__Public.pdf)
- [u-blox u-center](https://www.u-blox.com/en/product/u-center)
