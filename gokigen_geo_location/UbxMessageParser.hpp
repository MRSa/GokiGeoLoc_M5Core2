#include <QZSSDCX.h> // https://github.com/SWITCHSCIENCE/QZSSDCX

// UBXメッセージの最大長を定義
// (QZSSメッセージは長くなる可能性があるらしい)
#define UBX_BUFFER_SIZE 280
#define MAX_STORE_MESSAGE_SIZE 800  // 過去のメッセージ保持数
class UbxMessageParser
{
private:
  DCXDecoder _dcx_decoder;

  // UBXメッセージ受信時のデータ格納バッファ
  int _currentUbxMessageIndex = 0;
  uint8_t _ubxMessageBuffer[UBX_BUFFER_SIZE + 1];

  uint16_t _nofDcrMessage = 0;
  uint16_t _nofDcrMessageIndex = 0;
  uint8_t _QZSSdcrMessage[MAX_STORE_MESSAGE_SIZE][UBX_BUFFER_SIZE + 1];
  struct tm _QZSSdcrReceiveDateTime[MAX_STORE_MESSAGE_SIZE];
  uint16_t _QZSSdcrMessageSize[MAX_STORE_MESSAGE_SIZE];

/*
  void _dumpReceivedMessage()
  {
    // ペイロード長とメッセージ長を計算する
    uint16_t payloadLength = (_ubxMessageBuffer[5] << 8) | _ubxMessageBuffer[4];
    uint16_t totalMessageLength = 6 + payloadLength + 2;  // メッセージ全体の長さ (ヘッダ6バイト + ペイロード + チェックサム2バイト)

    // 受信したデータ部分のヘッダをダンプ
    Serial.println("\n--- UBX Message Received ---");
    Serial.print("Class: "); Serial.print(_ubxMessageBuffer[2], HEX);
    Serial.print(", ID: "); Serial.print(_ubxMessageBuffer[3], HEX);
    Serial.print(", Payload Length: "); Serial.println(payloadLength);
    
    // バイナリデータを16進数でダンプ
    Serial.println("UBX Binary Data Dump:");
    for (int index = 0; index < totalMessageLength; index++)
    {
      Serial.print(_ubxMessageBuffer[index], HEX); Serial.print(" ");
    }
    Serial.println();
  }
*/
/*
  bool _checkCheckSum(uint16_t totalMessageLength)
  {
    // チェックサムの検証（推奨）
    uint8_t ckA = 0;
    uint8_t ckB = 0;
    for (int i = 2; i < (totalMessageLength - 2); i++)
    {
      ckA += _ubxMessageBuffer[i];
      ckB += ckA;
    }
    if (ckA == _ubxMessageBuffer[totalMessageLength - 2] && ckB == _ubxMessageBuffer[totalMessageLength - 1]) {
      // Serial.println("Checksum: OK!");
      return true;
    } else {
      // Serial.println("Checksum: ERROR!");
      return false;
    }
  }
*/
/*  
  void _dumpQZSSSubFrameMessage()
  {
    // QZSSメッセージをサブフレームごと格納し、ダンプする
    Serial.print("[QZSS]");
    uint8_t numWords = _ubxMessageBuffer[10];   // ワード数
    Serial.printf("[numWords:%d] ", numWords);
    uint8_t dcr[numWords * 4];   // サブフレームデータの格納先
    for (int i = 0; i < numWords * 4; i += 4) // 1ワードごとに処理
    {
      dcr[i + 0] = _ubxMessageBuffer[14 + i + 3];
      dcr[i + 1] = _ubxMessageBuffer[14 + i + 2];
      dcr[i + 2] = _ubxMessageBuffer[14 + i + 1];
      dcr[i + 3] = _ubxMessageBuffer[14 + i + 0];
      Serial.printf("%02x%02x%02x%02x ", dcr[i + 0], dcr[i + 1], dcr[i + 2], dcr[i + 3]);
    }
    Serial.println();
  }
*/

  bool _isStoreQZSSMessage()
  {
    // --------- 受信した QZSSメッセージを保管しておくかどうか判断する
    uint8_t numWords = _ubxMessageBuffer[10];   // ワード数
    uint8_t dcr[numWords * 4];                  // サブフレームデータの格納先
    for (int i = 0; i < numWords * 4; i += 4)   // 1ワードごとに処理
    {
      dcr[i + 0] = _ubxMessageBuffer[14 + i + 3];
      dcr[i + 1] = _ubxMessageBuffer[14 + i + 2];
      dcr[i + 2] = _ubxMessageBuffer[14 + i + 1];
      dcr[i + 3] = _ubxMessageBuffer[14 + i + 0];
    }
    uint8_t pab = dcr[0];
    uint8_t mt = dcr[1] >> 2;

    if (pab == 0x53 || pab == 0x9A || pab == 0xC6)
    {
      // PAB(preanble)がパターンA(0x53), パターンB(0x9A), パターンC(0xC6)でかつ、
      // MT(MessageType) が 43(DCR) または MT:44(DCX) 以外のデータは保管しない。
      // (詳細は、仕様書 IS-QZSS-L1S-xxx 参照)
      if (mt == 43)
      {
        // DCRメッセージ(気象情報等)を受信した
        //Serial.println(" --- Received DCR Message! ---");
        return (true);
      }
      if (mt == 44)
      {
        // DCXメッセージ(J-Alert等)を受信した
        bool isDecode = _dcx_decoder.decode(dcr);
        if (!isDecode)
        {
          // ----- デコードできないメッセージ、保管しない
          return (false);
        }
        if ((_dcx_decoder.r.dcx_msg_type == DCX_MSG_NULL)||(_dcx_decoder.r.dcx_msg_type == DCX_MSG_UNKOWN))
        {
          // ----- DCX NULLメッセージを受信（この場合は保管しない）
          //Serial.println(" --- NULL DCX Message(ignore) ---");
          return (false);
        }
        //Serial.println(" --- Received DCX Message! ---");
        return (true);
      }
    }
    return (false);
  }

public:
  UbxMessageParser()
  {

  }

  void putBuffer(uint8_t &data)
  {
    if (_currentUbxMessageIndex < UBX_BUFFER_SIZE)
    {
      _ubxMessageBuffer[_currentUbxMessageIndex] = data;
      _currentUbxMessageIndex++;
    }
  }

  void clear()
  {
    _currentUbxMessageIndex = 0;
  }

  bool begin()
  {
    // ----- GNSSモジュールの初期化処理 （必要ならここに追加する） -----

    return (true);
  }

  bool parseUBX(uint8_t receiveByte)
  {
    // バッファオーバーフローのチェック
    if (_currentUbxMessageIndex >= UBX_BUFFER_SIZE)
    {
      // バッファがいっぱいになったら、パースを中断してfalseを返す
      _currentUbxMessageIndex = 0;
      return false;
    }
    
    // 受信データをバッファに格納
    putBuffer(receiveByte);
    
    // メッセージヘッダ（6バイト）が揃うまで待つ
    if (_currentUbxMessageIndex < 6)
    {
      return true; // まだそろっていないので処理を継続
    }
    
    // ペイロード長とメッセージ長を計算する
    uint16_t payloadLength = (_ubxMessageBuffer[5] << 8) | _ubxMessageBuffer[4];
    uint16_t totalMessageLength = 6 + payloadLength + 2;  // メッセージ全体の長さ (ヘッダ6バイト + ペイロード + チェックサム2バイト)
    
    // メッセージ長、全てのデータが揃っているか確認
    if (_currentUbxMessageIndex < totalMessageLength)
    {
      return true; // まだそろっていないので処理を継続
    }
    
    // ----- ここから、メッセージが完全に受信された後の処理 -----
    
    // ----- メッセージが SFBRX で 送信元が QZSS かどうかを判別する
    if ((_ubxMessageBuffer[2] != 0x02)||(_ubxMessageBuffer[3] != 0x13)||(_ubxMessageBuffer[6] != 0x05))
    {
      // 必要なメッセージではなかったので、バッファをリセットしてfalseを返す
      _currentUbxMessageIndex = 0;
      return false;
    }

    //// ----- 受信したメッセージのダンプ
    //_dumpReceivedMessage();

    //// ----- チェックサム計算の実行結果
    //bool checkResult = _checkCheckSum(totalMessageLength);

    // ----- QZSS サブフレームメッセージをダンプする
    //_dumpQZSSSubFrameMessage();

    // ----- QZSS サブフレームメッセージを保管する (前回データを消してから上書きする)
    if (_isStoreQZSSMessage())
    {
      memset(_QZSSdcrMessage[_nofDcrMessageIndex], 0x00, (UBX_BUFFER_SIZE + 1));
      memcpy(_QZSSdcrMessage[_nofDcrMessageIndex], _ubxMessageBuffer, totalMessageLength);

      // ----- 保管したメッセージサイズを保管する
      _QZSSdcrMessageSize[_nofDcrMessageIndex] = totalMessageLength;

      // ----- 受信日時を保管する
      struct tm currentTime;
      getLocalTime(&currentTime);
      memcpy(&_QZSSdcrReceiveDateTime[_nofDcrMessageIndex], &currentTime, sizeof(struct tm));
      _nofDcrMessageIndex++;

      //Serial.print("  STORE : "); Serial.print(totalMessageLength); Serial.print(" bytes.  ");
      //Serial.print(_nofDcrMessageIndex); Serial.print("/"); Serial.println(_nofDcrMessage);
      if (_nofDcrMessage < MAX_STORE_MESSAGE_SIZE)
      {
        // 保管メッセージ数をインクリメント（最大数までたまっていたら更新しない）
        _nofDcrMessage++;
      }

      if (_nofDcrMessageIndex >= MAX_STORE_MESSAGE_SIZE)
      {
        // ----- 受信して格納するのは MAX_STORE_MESSAGE_SIZE 個
        // -----  (バッファの先頭から入れ直す) ------
        _nofDcrMessageIndex = 0;
        _nofDcrMessage = MAX_STORE_MESSAGE_SIZE;
      }
    }

    // 処理が完了したので、バッファをリセットしてfalseを返す
    _currentUbxMessageIndex = 0;
    return false;
  }

  int getLastMessageIndex()
  {
    if ((_nofDcrMessage < MAX_STORE_MESSAGE_SIZE)||(_nofDcrMessageIndex == (MAX_STORE_MESSAGE_SIZE - 1)))
    {
      return (0);
    }
    return (_nofDcrMessageIndex);
  }

  int getMessageCount()
  {
    return (_nofDcrMessage);
  }

  int getMessageBufferSize()
  {
    return MAX_STORE_MESSAGE_SIZE;
  }

  struct tm *getQZSSdcrReceivedDateTime(int index)
  {
    return (&_QZSSdcrReceiveDateTime[index]);    
  }

  uint8_t *getQZSSdcrMessage(int index)
  {
    return (_QZSSdcrMessage[index]);
  }

  uint16_t getQZSSdcrMessageSize(int index)
  {
    return (_QZSSdcrMessageSize[index]);
  }
};
