# GokiGeoLoc_M5Core2

[M5Stack Core2](https://docs.m5stack.com/en/core/core2) に [Module GNSS(M135)](https://docs.m5stack.com/en/module/GNSS%20Module) をつなげて、地図や災危情報、センサデータを見えるようにしてみました。

![画面イメージ](https://github.com/MRSa/GokiGeoLoc_M5Core2/blob/main/images/gok_geo_loc.png?raw=true)

## 機能

本リポジトリでは、[M5Stack Core2](https://docs.m5stack.com/en/core/core2) に [Module GNSS(M135)](https://docs.m5stack.com/en/module/GNSS%20Module) を使用して、次の機能を実現します。

- **現在位置の表示**
  - [Module GNSS(M135)](https://docs.m5stack.com/ja/module/GNSS%20Module) で受信した緯度経度の情報から、M5Stack Core2のmicroSDカードに入れた[地理院地図](https://maps.gsi.go.jp/help/howtouse.html)を使って現在位置を表示します。地図のズームレベルの変更が可能です。
- **現在方位の表示**
  - [Module GNSS(M135)](https://docs.m5stack.com/ja/module/GNSS%20Module)、もしくは [Unit Mini IMU-Pro](https://docs.m5stack.com/ja/unit/IMU%20Pro%20Mini%20Unit) に搭載された地磁気センサを使用して北の方角を示します。
- **オフライン地図の表示**
  - あらかじめmicroSDカードにダウンロードした[地理院地図](https://maps.gsi.go.jp/help/howtouse.html)を画面に地図を表示します。地図のズームレベルや表示位置の移動が可能です。
- **災危通報の表示**
  - [Module GNSS(M135)](https://docs.m5stack.com/ja/module/GNSS%20Module) で受信した[災危通報](https://qzss.go.jp/technical/system/dcr.html)を最大999件蓄積し、表示します。情報の表示フォントサイズが変更できます。
- **搭載センサ情報の表示**
  - [Module GNSS(M135)](https://docs.m5stack.com/ja/module/GNSS%20Module) に搭載されたセンサ([BMI270](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi270-ds000.pdf), [BMM150](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmm150-ds001.pdf), [BMP280](https://www.bosch-sensortec.com/media/boschsensortec/downloads/product_flyer/bst-bmp280-fl000.pdf))のデータを表示することができます。画面の輝度を変更することができます。
- **USBシリアルポート経由でデータ転送**
  - USB シリアルポート経由で、センサの情報、蓄積した最大999件の[災危通報](https://qzss.go.jp/technical/system/dcr.html)を[M5Stack Core2](https://docs.m5stack.com/ja/core/core2)から転送することができます。転送後のデータを災危通報データに変換するpythonスクリプトも用意しました。

## システム構成

本リポジトリで使用するシステム構成を示します。

![システム構成図](https://github.com/MRSa/GokiGeoLoc_M5Core2/blob/main/images/system.png?raw=true)

- [M5Stack Core2](https://docs.m5stack.com/ja/core/core2)
  - [地理院地図](https://maps.gsi.go.jp/help/howtouse.html)を表示するために、microSDカードの搭載が必須です
- [Module GNSS(M135)](https://docs.m5stack.com/ja/module/GNSS%20Module)
- [Unit Mini IMU-Pro](https://docs.m5stack.com/ja/unit/IMU%20Pro%20Mini%20Unit) (オプション)
  - [Unit Mini IMU-Pro](https://docs.m5stack.com/ja/unit/IMU%20Pro%20Mini%20Unit) を [M5Stack Core2](https://docs.m5stack.com/ja/core/core2)に接続して起動すると、このユニットのセンサを優先して利用します
- PC等 (オプション)
  - [M5Stack Core2](https://docs.m5stack.com/ja/core/core2)とUSBシリアル通信でつなぎ、受信した災危通報メッセージを転送する場合に必要です

## 操作説明

電源を入れると、初期化処理を行った後、位置を検索した後、**現在位置表示** となります。

以下に画面イメージと操作を示します。

![画面イメージと操作](https://github.com/MRSa/GokiGeoLoc_M5Core2/blob/main/images/screen-transition.png?raw=true)

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

## PC等とのシリアル接続

[M5Stack Core2](https://docs.m5stack.com/ja/core/core2) のUSBシリアルポートを通して、PC等に受信した災危通報データの転送ができます。
転送レートは、**115200** bps と設定してください。なお、コマンド最後の **\n** は、0x0a (改行)を示します。

- **CMD:RESTART\n**
  - [M5Stack Core2](https://docs.m5stack.com/en/core/core2)  を再起動させます。

- **CMD:GETSENSOR\n**
  - [Module GNSS(M135)](https://docs.m5stack.com/ja/module/GNSS%20Module) に搭載された、現在のセンサデータ値、緯度経度情報をを取得します。

- **CMD:GETDCRMSG\n**
  - [M5Stack Core2](https://docs.m5stack.com/en/core/core2)で受信した、災危通報データを取得します。

PC等とのシリアル通信についての詳細は、[pc_receiver/readme.md](pc_receiver/readme.md) を参照してください。

## 画面説明

### 現在位置表示

![現在位置表示](https://github.com/MRSa/GokiGeoLoc_M5Core2/blob/main/images/location.png?raw=true)

衛星から拾った現在位置を地図の中心に表示します。合わせて、内臓センサの情報（気温と気圧）を表示します。なお、地図のズームレベルは16をデフォルトにしています。

### 地図表示

![地図表示](https://github.com/MRSa/GokiGeoLoc_M5Core2/blob/main/images/map.png?raw=true)

microSDカードに保存している地図を表示します。現在位置表示とは異なり、地図の表示位置を移動させることができます。

### 災危通報の表示

![災危通報表示](https://github.com/MRSa/GokiGeoLoc_M5Core2/blob/main/images/dcr.png?raw=true)

衛星（みちびき）から送られてくる、災危通報という情報を表示します。最大300件程度記憶しています。
なお、受信情報は電源を入れている間だけ記憶しています。電源をOFFすると忘れます。

### 詳細表示（文字で情報を表示）

![詳細表示](https://github.com/MRSa/GokiGeoLoc_M5Core2/blob/main/images/detail.png?raw=true)

センサ情報等を文字で表示します。

- **日時・時刻**
- **衛星**
- **緯度・経度**
- **高度**
- **気温**
- **気圧**
- **重力加速度(X, Y, Z)**
- **傾き(X, Y, Z)**
- **地磁気(X, Y, Z)**
- **方位**
- **電池残量**
- **画面輝度**

## 開発環境・参考情報等

### 地理院地図タイル画像の取得方法について

[地理院地図](https://maps.gsi.go.jp/help/howtouse.html)タイル画像をmicroSDに保管する方法については、[地理院地図タイル画像のmicroSD保管](getGSImaps/README.md)を参照してください。

### 開発環境等

開発環境等に関しては、[開発環境等](documents/build.md) を参照してください。

### 参考サイト等

本アプリを開発するのに参考にしたサイトは、[参考サイト等](documents/README.md) を参照してください。
