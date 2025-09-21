# シリアル経由で送り付けるデータの解析pythonスクリプト **dcr_analysis.py**

## 概要

PCからシリアル経由で [CMD:GETDCRMSG\r\n]を送信すると、M5Stack Core2 から送り返してくれる災危通報のデータが入った JSONデータを解析するpythonスクリプトです。
起動時に ファイル名が指定されていればファイルから、なければ標準入力から読み込むようになっています。

DCR/DCXメッセージの解析に [QZSS DCR Decoder Azarashi](https://github.com/nbtk/azarashi) を使用していますが、ubloxメッセージで送信しているので、[NEO-M9NのUBX-RXM-SFRBXメッセージが正常にデコードされるよう修正 #18](https://github.com/nbtk/azarashi/pull/18) の pull request が取り込まれている必要があるようです。ご注意ください。

## サンプルデータ

実際に受信したデータファイルは [getdcrmsg.json](getdcrmsg.json) です。

## 注意

未使用です。(まだCopilotさんに生成してもらっただけです。。。これから確認...)
