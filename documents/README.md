# 参考サイト等

本アプリケーションを作成する上で参考にしたサイトです。

## 地図関連

- [地理院地図](https://maps.gsi.go.jp/help/howtouse.html)
- [地理院タイルの仕様](https://maps.gsi.go.jp/development/siyou.html)
- [地理院タイルのご利用について](https://maps.gsi.go.jp/development/ichiran.html)
- [国土地理院コンテンツ利用規約](https://www.gsi.go.jp/kikakuchousei/kikakuchousei40182.html)
- [タイル座標確認ページ(地理院地図)](https://maps.gsi.go.jp/development/tileCoordCheck.html#16/35.6104/139.5950)

## 災危通報関連

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

## 地磁気関連

- [地図と位置情報](https://internet.watch.impress.co.jp/docs/column/chizu3/1431412.html)
- [国土地理院 磁気図](https://www.gsi.go.jp/buturisokuchi/menu03_magnetic_chart.html)
- [国土地理院 地磁気測量](https://vldb.gsi.go.jp/sokuchi/geomag/menu_04/index.html)
- [国土地理院 地磁気を知る](https://www.gsi.go.jp/buturisokuchi/menu01_index.html)

## プログラミング関連

- [M5Unifiedライブラリの始め方](https://docs.m5stack.com/ja/arduino/m5unified/helloworld)
  - [M5Unified](https://github.com/m5stack/M5Unified)
  - [M5Unified IMU Class](https://docs.m5stack.com/en/arduino/m5unified/imu_class)
  - [IMU sample Code](https://github.com/m5stack/M5Unified/blob/master/examples/Basic/Imu/Imu.ino)
- [M5GFX Setup](https://docs.m5stack.com/ja/arduino/m5gfx/m5gfx)
  - [M5GFX](https://github.com/M5Stack/M5GFX)

## センサー関連

- [Adafruit_BMP280_Library](https://github.com/adafruit/Adafruit_BMP280_Library/tree/master)
- [BMP280 Digital Pressure Sensor(datasheet)](https://cdn-shop.adafruit.com/datasheets/BST-BMP280-DS001-11.pdf)
- [u-blox Protocol Specification](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/products/module/GNSS%20Module/u-blox8-M8_ReceiverDescrProtSpec__UBX-13003221__Public.pdf)
- [u-blox u-center](https://www.u-blox.com/en/product/u-center)
