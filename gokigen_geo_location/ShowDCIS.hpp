
#include <QZSSDCX.h>
#include <QZQSM.h>

#define QZSS_DCWORD_INDEX 10
#define QZSS_DCMESSAGE_INDEX 14

class ShowDCIS
{
  // ----- 災危通報の情報を表示する
private:
  bool _isDumped = false;
  int _messageIndex = 0;
  int _isClear = false;
  DCXDecoder _dcx_decoder;
  QZQSM _dc_report;
  
  void _dumpQZSSSubFrameMessage(uint8_t buffer[])
  {
    // QZSSメッセージをサブフレームごと格納し、ダンプする
    Serial.print("[QZSS]");
    uint8_t numWords = buffer[QZSS_DCWORD_INDEX];   // ワード数
    Serial.printf("[numWords:%d] ", numWords);
    uint8_t dcr[numWords * 4];   // サブフレームデータの格納先
    for (int i = 0; i < numWords * 4; i += 4) // 1ワードごとに処理
    {
      dcr[i + 0] = buffer[QZSS_DCMESSAGE_INDEX + i + 3];
      dcr[i + 1] = buffer[QZSS_DCMESSAGE_INDEX + i + 2];
      dcr[i + 2] = buffer[QZSS_DCMESSAGE_INDEX + i + 1];
      dcr[i + 3] = buffer[QZSS_DCMESSAGE_INDEX + i + 0];
      Serial.printf("%02x%02x%02x%02x ", dcr[i + 0], dcr[i + 1], dcr[i + 2], dcr[i + 3]);
    }
    Serial.println();
  }

  void _doDecodeAndDisplay(uint8_t *dcrMessage)
  {
    uint8_t numWords = *(dcrMessage + QZSS_DCWORD_INDEX); 
    uint8_t dcr[numWords * 4];
    for (int i = 0; i < numWords * 4; i += 4)
    {
      dcr[i + 0] = dcrMessage[QZSS_DCMESSAGE_INDEX + i + 3];
      dcr[i + 1] = dcrMessage[QZSS_DCMESSAGE_INDEX + i + 2];
      dcr[i + 2] = dcrMessage[QZSS_DCMESSAGE_INDEX + i + 1];
      dcr[i + 3] = dcrMessage[QZSS_DCMESSAGE_INDEX + i + 0];
    }

    uint8_t pab = dcr[0];
    uint8_t mt = dcr[1] >> 2;
    if (pab == 0x53 || pab == 0x9A || pab == 0xC6)
    {
      if (mt == 43)
      {
        // 災害・危機管理通報サービス（DC Report）のメッセージ内容を表示
        _dc_report.Decode(dcr);
        Serial.print("DCR: ");
        Serial.println(_dc_report.GetReport());
        M5.Display.printf("%s\r\n", _dc_report.GetReport());
      }
      else if (mt == 44)
      {
        // 災害・危機管理通報サービス（拡張）（DCX）のメッセージ内容を表示
        bool result = _dcx_decoder.decode(dcr);
        if (!result)
        {
          Serial.println("ERR>DCX DECODE FAILURE...");
        }
        else
        {
          Serial.println("DCX: ");
          _dcx_decoder.printSummary(Serial, _dcx_decoder.r);
          Serial.println("- - -");
          _dcx_decoder.printAll(Serial, _dcx_decoder.r);
          //_dcx_decoder.printSummary(&M5.Display, _dcx_decoder.r);
          //_dcx_decoder.printAll(&M5.Display, _dcx_decoder.r);
        }
      }
      else
      {
        // 受信メッセージを(Serialに)ダンプ
        Serial.print("[Unknown2]");
        _dumpQZSSSubFrameMessage(dcrMessage);        
      }
    }
    else
    {
        // 受信メッセージを(Serialに)ダンプ
        Serial.println("[Unknown1]");
        _dumpQZSSSubFrameMessage(dcrMessage);
    }
  }

public:
  ShowDCIS()
  {

  }

  void drawScreen(SensorDataHolder *dataHolder, TouchPositionHandler *touchPos, UbxMessageParser *messageParser)
  {
    if (_isClear)
    {
      M5.Display.clear();
      _isClear = false;
      _isDumped = false;
    }
    M5.Display.setCursor(0,0);
    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::efontJA_16_b);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.printf("### 災危通報 ###\r\n");

    int messageCount = messageParser->getMessageCount();
    if (_messageIndex > (messageCount - 1))
    {
      _messageIndex = 0;
    }

    // ----- 災危通報の表示
    M5.Display.setCursor(0,20);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setFont(&fonts::efontJA_16);
    if (messageCount == 0)
    {
      M5.Display.printf("受信メッセージなし");
      return;
    }
    M5.Display.printf("受信メッセージ： %3d/%3d\r\n", (_messageIndex + 1), messageCount);

    // -----------------------------------------------
    //   ここで災危通報メッセージの表示を（１件づつ）行う
    // -----------------------------------------------
    if (!_isDumped)
    {
      M5.Display.setCursor(0,70);
      M5.Display.setTextSize(1);
      _doDecodeAndDisplay(messageParser->getQZSSdcrMessage(_messageIndex));
      _isDumped = true;
    }

    // ----- 表示メッセージの変更確認 -----
    if (touchPos->isPressed())
    {
      //----- タッチパネルが押されたことを検出
      int posX = touchPos->getTouchX();
      if (posX < 160)
      {
        // ひとつ減らす
        _messageIndex--;
      }
      else
      {
        // ひとつふやす
        _messageIndex++;
      }
      if (_messageIndex < 0)
      {
        // メッセージ最大数を設定する
        _messageIndex = messageCount - 1;
        _isDumped = false;
      }
      if (_messageIndex >= (messageCount - 1))
      {
        // 先頭バッファを設定する
        _messageIndex = 0;
        _isDumped = false;
      }
      _isClear = true; // 次回表示でメッセージを消去する

      // ----- バイブレーション
      makeVibration(VIBRATION_MIDDLE, VIBRATION_TIME_MIDDLE);

      // ---- 開放する
      touchPos->resetPosition();
    }
  }
};
