# シリアル経由で送り付けるデータの解析pythonスクリプト群

## 災危通報のデータを受信し解析するスクリプト（**dcr_analysis.py**）

PCからシリアル経由で **CMD:GETDCRMSG\n** を送信すると、M5Stack Core2 から送り返してくれる災危通報のデータが入った JSONデータを解析するpythonスクリプトです。
起動時にオプションが指定されていない場合には標準入力から、-f ファイル名 でファイルから、 -i ポート名 でシリアルポートからデータを読み込みようになっています。

DCR/DCXメッセージの解析に [QZSS DCR Decoder : Azarashi](https://github.com/nbtk/azarashi) を使用していますが、ubloxメッセージで送信しているので、[NEO-M9NのUBX-RXM-SFRBXメッセージが正常にデコードされるよう修正 #18](https://github.com/nbtk/azarashi/pull/18) の pull request が取り込まれている必要があります。ご注意ください。

### 使用方法(dcr_analysis.py)

本スクリプトは、「標準出力からJSONデータを入力する」「ファイルからJSONデータを入力する」「シリアルポートからJSONデータを入力する」の3パターンで解析することができます。

- **dcr_analysis.py**
  - 標準入力から読み込んだJSONファイルを解析し、災危情報の解析結果を標準出力に出力します。
- **dcr_analysis.py -f ファイル名**
  - 指定したファイルから読み込んだJSONファイルを解析し、災危情報の解析結果を標準出力に出力します。
- **dcr_analysis.py -i シリアルポート名**
  - 指定したシリアルポートから読み込んだJSONファイルを解析し、災危情報の解析結果を標準出力に出力します。

### 使用するライブラリ(dcr_analysis.py)

本スクリプトで使用するライブラリです。事前にこれらのライブラリが使用できるように準備をお願いします。

- [pyserial](https://pypi.org/project/pyserial/)
  - 本スクリプトでシリアル通信を行い、JSONデータを取得するために使用します。 **pip install pyserial** でインストールをお願いします。

- [QZSS DCR Decoder : Azarashi](https://github.com/nbtk/azarashi)
  - 本スクリプトで使用する災危通報を解析するライブラリです。
  - ubloxバイナリデータを解析しているので、[NEO-M9NのUBX-RXM-SFRBXメッセージが正常にデコードされるよう修正 #18](https://github.com/nbtk/azarashi/pull/18) の pull request が取り込まれている必要があります。ご注意ください。([https://github.com/nbtk/azarashi](https://github.com/nbtk/azarashi) から スクリプトを取得し、そこに dcr_analysis.py をコピーしてスクリプト(dcr_analysis.py)を実行すると、動作するようです。)

## 現在のセンサ情報を取得するスクリプト (**get_sensor.py**)

PCからシリアル経由で **CMD:GETSENSOR\n** を送信すると、M5Stack Core2 からJSON形式で送り返してくれるセンサ情報を解析するスクリプトです。

### 使用方法(get_sensor.py)

- **get_sensor.py -i シリアルポート名**
  - 指定したシリアルポートから読み込んだJSONファイルを解析し、解析結果を標準出力に出力します。

### 使用するライブラリ(get_sensor.py)

本スクリプトで使用するライブラリです。事前にこれらのライブラリが使用できるように準備をお願いします。

- [pyserial](https://pypi.org/project/pyserial/)
  - 本スクリプトでシリアル通信を行い、JSONデータを取得するために使用します。 **pip install pyserial** でインストールをお願いします。

## サンプルJSONデータ

### CMD:GETDCRMSG

実際に受信したデータファイルは [getdcrmsg.json](getdcrmsg.json) です。本スクリプトでは、この形式でのデータが入力されることを期待しています。count は、配列 messages の個数です。（ messagesが 0 個の場合は、result は false となります。）

    {
       "result": true,
       "count": 11,
       "messages": [
          "b5620213280005020100083d0200c1f4ad9ac30280118b7f58f20d6219108865452c961ab1c01200203f3114dc381356",
          "b5620213280005020100083d0200c1f4ad9ac30280118b7f58f20d6219108865452c961ab1c01200203f3114dc381356",
          "b5620213280005020100083d0200c1f4ad530c058011d401a230f9a13a5e88ca3f5480065121130000002693074ab657",
          "b5620213280005020100083d0200c1f4ad9ac30280118b7f58f20d6219108865452c961ab1c01000203fb1642f716d15",
          "b5620213280005020100083d0200c1f4adc6cc028011cb015a2603828a40086a4550a81f41c111003061d4b7e926ba7c",
          "b5620213280005020100083d0200c1f4adc6cc028011cb015a2603828a40086a4550a81f41c11100306154cacfe5f240",
          "b5620213280005020100083d0200c1f4ad9ac30280118b7f58f20d6219108865452c961ab1c01200203fb169fafbc954",
          "b5620213280005020100083d0200c1f4ad9ac30280118b7f58f20d6219108865452c961ab1c01000203ff1279ad33d96",
          "b5620213280005020100083d0200c1f4ad9ac30280118b7f58f20d6219108865452c961ab1c01200203ff12a4f597d9f",
          "b5620213280005020100083d0200c1f4adc6cc028011cb015a2603828a40086a4550a81f41c110003061147236c19cda",
          "b5620213280005020100083d0200c1f4adc6cc028011cb015a2603828a40086a4550a81f41c1130030611484afcd3626"
       ]
    }

### CMD:GETSENSOR

以下のデータは、**CMD:GETSENSOR** で受信したデータです。

    {
      "result": true,
      "battery": 20,
      "imu_temp": 30.43555,
      "temperature": 35.69,
      "pressure": 102254,
      "altitude": -77.06008206,
      "acc_x": 0.002441,
      "acc_y": -0.543457,
      "acc_z": 0.851807,
      "gyro_x": 0.061035,
      "gyro_y": 0.488281,
      "gyro_z": -0.12207,
      "mag_x": 163.3331,
      "mag_y": -227.2813,
      "mag_z": 271.4142,
      "is_gps": true,
      "lat": 35.6809591,
      "lng": 139.7673068,
      "alt": 26.8,
      "heap": 3881052
    }

## 参考リンク

- [災害・危機管理通報サービス「災危通報」](https://qzss.go.jp/overview/services/sv08_dc-report.html)
- [パフォーマンススタンダード（PS-QZSS）/ ユーザインタフェース仕様書（IS-QZSS）/ システム概要 / 性能評価結果](https://qzss.go.jp/technical/download/ps-is-qzss.html)
- [QZSS DCR Decoder : Azarashi](https://github.com/nbtk/azarashi)
