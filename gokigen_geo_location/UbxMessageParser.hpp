
// UBXメッセージの最大長を定義
// (QZSSメッセージは長くなる可能性があるらしい)
#define UBX_BUFFER_SIZE 400

class UbxMessageParser
{
private:
  // UBXメッセージ受信時のデータ格納バッファ
  int currentUbxMessageIndex = 0;
  uint8_t ubxMessageBuffer[UBX_BUFFER_SIZE + 1];

  void _dumpReceivedMessage()
  {
    // ペイロード長とメッセージ長を計算する
    uint16_t payloadLength = (ubxMessageBuffer[5] << 8) | ubxMessageBuffer[4];
    uint16_t totalMessageLength = 6 + payloadLength + 2;  // メッセージ全体の長さ (ヘッダ6バイト + ペイロード + チェックサム2バイト)

    // 受信したデータ部分のヘッダをダンプ
    Serial.println("\n--- UBX Message Received ---");
    Serial.print("Class: "); Serial.print(ubxMessageBuffer[2], HEX);
    Serial.print(", ID: "); Serial.print(ubxMessageBuffer[3], HEX);
    Serial.print(", Payload Length: "); Serial.println(payloadLength);
    
    // バイナリデータを16進数でダンプ
    Serial.println("UBX Binary Data Dump:");
    for (int index = 0; index < totalMessageLength; index++)
    {
      Serial.print(ubxMessageBuffer[index], HEX); Serial.print(" ");
    }
    Serial.println();
  }

  bool _checkCheckSum(uint16_t totalMessageLength)
  {
    // チェックサムの検証（推奨）
    uint8_t ckA = 0;
    uint8_t ckB = 0;
    for (int i = 2; i < (totalMessageLength - 2); i++)
    {
      ckA += ubxMessageBuffer[i];
      ckB += ckA;
    }
    if (ckA == ubxMessageBuffer[totalMessageLength - 2] && ckB == ubxMessageBuffer[totalMessageLength - 1]) {
      // Serial.println("Checksum: OK!");
      return true;
    } else {
      // Serial.println("Checksum: ERROR!");
      return false;
    }
  }

public:
  UbxMessageParser()
  {

  }

  void putBuffer(uint8_t &data)
  {
    if (currentUbxMessageIndex < UBX_BUFFER_SIZE)
    {
      ubxMessageBuffer[currentUbxMessageIndex] = data;
      currentUbxMessageIndex++;
    }
  }

  void clear()
  {
    currentUbxMessageIndex = 0;
  }

  bool begin()
  {

/**
  // ----- GNSSモジュールの初期化処理 （必要ならここに追加する） -----

  // UBX-CFG-GNSS (0x06 0x3E)メッセージの送信
  sendUBX_CFG_GNSS_RXM_RAWX();

  // UBX-CFG-GNSS (0x06 0x3E)メッセージの送信
  sendUBX_CFG_GNSS();
**/ 
    return (true);
  }

  bool parseUBX(uint8_t receiveByte)
  {
    // バッファオーバーフローのチェック
    if (currentUbxMessageIndex >= UBX_BUFFER_SIZE)
    {
      // バッファがいっぱいになったら、パースを中断してfalseを返す
      currentUbxMessageIndex = 0;
      return false;
    }
    
    // 受信データをバッファに格納
    putBuffer(receiveByte);
    
    // メッセージヘッダ（6バイト）が揃うまで待つ
    if (currentUbxMessageIndex < 6)
    {
      return true; // まだそろっていないので処理を継続
    }
    
    // ペイロード長とメッセージ長を計算する
    uint16_t payloadLength = (ubxMessageBuffer[5] << 8) | ubxMessageBuffer[4];
    uint16_t totalMessageLength = 6 + payloadLength + 2;  // メッセージ全体の長さ (ヘッダ6バイト + ペイロード + チェックサム2バイト)
    
    // メッセージ長、全てのデータが揃っているか確認
    if (currentUbxMessageIndex < totalMessageLength)
    {
      return true; // まだそろっていないので処理を継続
    }
    
    // ----- ここから、メッセージが完全に受信された後の処理 -----
    
    // ----- メッセージが SFBRX で 送信元が QZSS かどうかを判別する
    if ((ubxMessageBuffer[2] != 0x02)||(ubxMessageBuffer[3] != 0x13)||(ubxMessageBuffer[6] != 0x05))
    {
      // 必要なメッセージではなかったので、バッファをリセットしてfalseを返す
      currentUbxMessageIndex = 0;
      return false;
    }

    // ----- 受信したメッセージのダンプ
    _dumpReceivedMessage();

    // ----- チェックサム計算の実行結果
    bool checkResult = _checkCheckSum(totalMessageLength);
    
    // ----- ここからUBXメッセージの具体的な解析ロジック
    uint8_t msgClass = ubxMessageBuffer[2];
    uint8_t msgId = ubxMessageBuffer[3];
    if (msgClass == 0x02 && msgId == 0x15) { 
      Serial.println(">>> This is a UBX-RXM-RAWX message (raw measurement data)!");
    } else if (msgClass == 0x05) {
      if (msgId == 0x01) {
        Serial.println(">>> This is UBX-ACK-ACK message!");
      } else if (msgId == 0x00) {
        Serial.println(">>> This is UBX-ACK-NAK message!");
      }
    }
    
    // 処理が完了したので、バッファをリセットしてfalseを返す
    currentUbxMessageIndex = 0;
    return false;
  }


/**
// UBX-CFG-GNSS (0x06 0x3E)メッセージの送信関数
void sendUBX_CFG_GNSS_RXM_RAWX()
{
  // UBX-CFG-MSGメッセージでRXM-RAWX (0x02, 0x15) を有効にする
  uint8_t ubxConfig[] = {
    0xB5, 0x62,      // 同期バイト
    0x06, 0x01,      // クラス (CFG), ID (MSG)
    0x08, 0x00,      // 長さ
    0x02, 0x15,      // ペイロード: msgClass, msgID
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, // UART1の出力レートを1に設定
    0x00, 0x00       // チェックサムは後で計算
  };

  // チェックサムの計算
  uint8_t ckA = 0;
  uint8_t ckB = 0;
  for (int i = 2; i < 14; i++) { // ヘッダー以降の全バイト
    ckA += ubxConfig[i];
    ckB += ckA;
  }
  ubxConfig[14] = ckA;
  ubxConfig[15] = ckB;

  // メッセージを送信
  UBLOX_SERIAL.write(ubxConfig, sizeof(ubxConfig));
  Serial.println("Sent UBX-CFG-MSG to enable RXM-RAWX message.");

  // u-bloxモジュールが設定変更を処理するための短い遅延
  delay(100);
}



// UBX-CFG-GNSS (0x06 0x3E)メッセージの送信関数
void sendUBX_CFG_GNSS()
{
    // メッセージのペイロードを定義
    // ヘッダー (クラス/ID/長さ) は含まず、ペイロードのみを定義
    // 構造: msgVer, numTrkCh, numRes, [GNSSシステム設定ブロック]
    // -------------------------------------------------------------------------------------------------------------------------------------------------------
    // msgVer (uint8_t): 0x00
    // numTrkCh (uint8_t): 0xFF (モジュールに追跡チャンネル数を自動で設定させる)
    // numRes (uint8_t): GNSSシステム設定ブロックの数
    // [GNSSシステム設定ブロック] (GNSSconfig_t):
    //   gnssId (uint8_t): GNSSシステムのID (例: QZSSは 5)
    //   resTrkCh (uint8_t): 予約 (0x00)
    //   maxTrkCh (uint8_t): 最大追跡チャンネル数 (0x08)
    //   flags (uint32_t): 設定フラグ
    //     bit 0: enable (1=有効, 0=無効)
    //     bits 2-3: SigCfgMask (00=L1, 01=L1+L5)
    // -------------------------------------------------------------------------------------------------------------------------------------------------------

    // QZSSを有効化する設定ブロック
    uint8_t qzssConfigBlock[] = {
        0x05,       // gnssId: 5 (QZSS)
        0x00,       // resTrkCh: 予約
        0x08,       // maxTrkCh: 8
        0x01, 0x00, 0x00, 0x00 // flags: bit0 = 1 (有効化), その他のビットは 0
    };
    
    // UBXメッセージの全体の長さを計算 (ペイロードの長さ + ヘッダー 6バイト + チェックサム 2バイト)
    const uint8_t payloadLength = 3 + sizeof(qzssConfigBlock); // msgVer(1) + numTrkCh(1) + numRes(1) + qzssConfigBlock(8)
    const uint8_t totalLength = 6 + payloadLength + 2;

    uint8_t ubxMessage[totalLength];
    
    // ヘッダーの構築
    ubxMessage[0] = 0xB5; // 同期バイト1
    ubxMessage[1] = 0x62; // 同期バイト2
    ubxMessage[2] = 0x06; // クラス
    ubxMessage[3] = 0x3E; // ID
    ubxMessage[4] = payloadLength & 0xFF; // ペイロード長 (下位バイト)
    ubxMessage[5] = (payloadLength >> 8) & 0xFF; // ペイロード長 (上位バイト)

    // ペイロードの構築
    ubxMessage[6] = 0x00; // msgVer
    ubxMessage[7] = 0xFF; // numTrkCh
    ubxMessage[8] = 0x01; // numRes: 1つのGNSS設定ブロックを送信

    // QZSS設定ブロックをペイロードにコピー
    memcpy(&ubxMessage[9], qzssConfigBlock, sizeof(qzssConfigBlock));

    // チェックサムの計算
    uint8_t ckA = 0;
    uint8_t ckB = 0;
    for (int i = 2; i < 6 + payloadLength; i++) {
        ckA += ubxMessage[i];
        ckB += ckA;
    }
  
    // チェックサムの追加
    ubxMessage[6 + payloadLength] = ckA;
    ubxMessage[6 + payloadLength + 1] = ckB;

    // メッセージの送信
    UBLOX_SERIAL.write(ubxMessage, totalLength);
    Serial.println("Sent UBX-CFG-GNSS message to enable QZSS.");

  // u-bloxモジュールが設定変更を処理するための短い遅延
  delay(100); 
}
**/

};
