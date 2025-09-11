
#include <QZQSM.h>    // https://github.com/baggio63446333/QZQSM
#include <QZSSDCX.h>  // https://github.com/SWITCHSCIENCE/QZSSDCX

#include "MyStringStream.hpp"

#define QZSS_DCWORD_INDEX 10
#define QZSS_DCMESSAGE_INDEX 14

class ShowDCIS
{
  // ----- 災危通報の情報を表示する
private:
  int _fontSize = 3;
  bool _isDumped = false;
  bool _makeVibration = false;
  int _messageIndex = 0;
  int _isClear = false;
  DCXDecoder _dcx_decoder;
  QZQSM _dc_report;

  void _applyFontSize()
  {
    switch (_fontSize)
    {
      case 0:
        M5.Display.setFont(&fonts::efontJA_10);
        break;
      case 1:
        M5.Display.setFont(&fonts::efontJA_12);
        break;
      case 2:
        M5.Display.setFont(&fonts::efontJA_14);
        break;
      case 4:
        M5.Display.setFont(&fonts::efontJA_24);
        break;
      case 3:
      default:
        M5.Display.setFont(&fonts::efontJA_16);
        break;        
    }
  }
  
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
          M5.Display.printf("DCX(Decode ERROR)\r\n");
        }
        else
        {
          MyStringStream dcxMessage(16384);
          Serial.println("DCX: ");
          _dcx_decoder.printSummary(dcxMessage, _dcx_decoder.r);
          Serial.println(dcxMessage.getString().c_str());
          Serial.println("- - -");
          //_dcx_decoder.printAll(Serial, _dcx_decoder.r);
          //_dcx_decoder.printSummary(&M5.Display, _dcx_decoder.r);
          //_dcx_decoder.printAll(&M5.Display, _dcx_decoder.r);
          M5.Display.printf("[DCX]\r\n%s", dcxMessage.getString().c_str());
        }
      }
      else
      {
        // 受信メッセージを(Serialに)ダンプ
        Serial.print("[Unknown2]");
        _dumpQZSSSubFrameMessage(dcrMessage); 
        M5.Display.printf("DCX: mt:%d\r\n", mt);
      }
    }
    else
    {
        // 受信メッセージを(Serialに)ダンプ
        Serial.println("[Unknown1]");
        _dumpQZSSSubFrameMessage(dcrMessage);
        M5.Display.printf("DCX: pab:%02x mt:%d\r\n", pab, mt);
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
    M5.Display.printf("## 災危通報 ##\r\n");

    // ----- 操作領域にマーキング
    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::efontJA_14_b);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setCursor(308,0);
    M5.Display.printf("F");
    M5.Display.drawRect(304,0, 15,15, TFT_WHITE);

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
    M5.Display.setFont(&fonts::efontJA_16);
    M5.Display.printf("受信メッセージ： %3d/%3d\r\n", (_messageIndex + 1), messageCount);

    // ----- メッセージの移動タッチ位置のマーク
    //M5.Display.setCursor(308,0);
    //M5.Display.printf("F");
    M5.Display.drawRect(304,30, 15,15, TFT_WHITE);
    M5.Display.drawRect(264,30, 15,15, TFT_WHITE);
    M5.Display.drawRect(224,30, 15,15, TFT_WHITE);

    // ----- メッセージのフォントサイズを決定する

    // -----------------------------------------------
    //   ここで災危通報メッセージの表示を（１件づつ）行う
    // -----------------------------------------------
    if (!_isDumped)
    {
      _applyFontSize();
      M5.Display.setCursor(0,45);
      M5.Display.setTextSize(1);
      displayCurrentJstTime("受信日時： ", messageParser->getQZSSdcrReceivedDateTime(_messageIndex));
      _doDecodeAndDisplay(messageParser->getQZSSdcrMessage(_messageIndex));
      _isDumped = true;
    }

    // ----- 表示メッセージの変更確認 -----
    if (touchPos->isPressed())
    {
      //----- タッチパネルが押されたことを検出
      int posX = touchPos->getTouchX();
      int posY = touchPos->getTouchY();
      _makeVibration = false;
      if ((posX > 300)&&(posY < 25))
      {
        // --- 右上タッチで、フォントサイズを変更
        _fontSize--;
        if (_fontSize < 0)
        {
          // ---- 最小サイズよりも小さい場合は、最大サイズにする
          _fontSize = 4;
        }
        _makeVibration = true;
      }
      else if ((posX > 280)&&(posY > 30)&&(posY < 60))
      {
        // ----- 受信メッセージ数の横あたりの右 : 末尾の場所に
        _messageIndex = messageCount - 1;
        _makeVibration = true;
      }
      else if ((posX > 250)&&(posX < 275)&&(posY > 30)&&(posY < 60))
      {
        // ----- 受信メッセージ数の横あたりの中 : 最新の場所に
        _messageIndex = messageParser->getLastMessageIndex() - 1;
        _makeVibration = true;
      }
      else if ((posX > 220)&&(posX < 245)&&(posY > 30)&&(posY < 60))
      {
        // ----- 受信メッセージ数の横あたりの左 : 先頭に

        _messageIndex = 0;
        _makeVibration = true;
      }
      else if (posY < 70)
      {
        // タイトル周辺をタッチした場合には、反応しないようにする
        _makeVibration = false;
      }
      else if (posX < 160)
      {
        // 左半分をタッチ、表示メッセージをひとつ減らす
        _messageIndex--;
        _makeVibration = true;
      }
      else
      {
        // 右半分をタッチ、表示メッセージをひとつふやす
        _messageIndex++;
        _makeVibration = true;
      }
      if (_messageIndex < 0)
      {
        // メッセージ最大数を設定する
        _messageIndex = messageCount - 1;
        Serial.print("Count LAST [");
        Serial.print(_messageIndex);
        Serial.println("]");
        _isDumped = false;
      }
      if (_messageIndex > (messageCount - 1))
      {
        // 先頭バッファを設定する
        _messageIndex = 0;
        Serial.print("Count FIRST [");
        Serial.print(messageCount);
        Serial.println("]");
        _isDumped = false;
      }
      // ----- バイブレーション
      if (_makeVibration)
      {
        _isClear = true; // 次回表示でメッセージを消去する
        makeVibration(VIBRATION_MIDDLE, VIBRATION_TIME_SHORT);
      }

      // ---- 開放する
      touchPos->resetPosition();
      _makeVibration = false;
    }
  }
};
