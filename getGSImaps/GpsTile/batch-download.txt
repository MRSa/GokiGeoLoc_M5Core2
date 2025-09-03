batch-download_XX.bat について

curlコマンドを用いて、GSI Mapをダウンロードしてローカルフォルダに格納するだけのものです。
何も考えずにX軸, Y軸の範囲を端から端までダウンロードさせます。
既にファイルが存在した場合はスキップします。

バッチ処理の最後に、ダウンロード成功数、ダウンロード失敗数を表示します。

取得するファイル
https://cyberjapandata.gsi.go.jp/xyz/std/(zoom level)/(タイルX座標)/(タイルY座標).png

地理院地図
https://maps.gsi.go.jp/#16/36.104622/140.084546/&base=std
