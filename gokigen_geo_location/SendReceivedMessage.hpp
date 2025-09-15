#include <ArduinoJson.h>

class SendReceivedMessage
{
private:
  SensorDataHolder *_dataHolder;
  UbxMessageParser *_messageParser;

  void _sendReceivedDcrMessages(Stream &stream)
  {
    int msgCount = _messageParser->getMessageCount();
    if (msgCount <= 0)
    {
      // ---- データがない...
      DynamicJsonDocument msgJson(1024);
      msgJson["result"] = false;
      msgJson["count"] = msgCount;
      String outputJson;
      serializeJson(msgJson, outputJson);
      stream.println(outputJson);
      stream.println("");
      return;
    }

    size_t docSize = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(msgCount);
    for (int index = 0; index < msgCount; index++)
    {
        uint16_t totalMessageLength = _messageParser->getQZSSdcrMessageSize(index);
        docSize += totalMessageLength * 4 + 1;
    }

    // ----- 受信データを応答する
    DynamicJsonDocument msgJson(docSize + 128);
    msgJson["result"] = true;
    msgJson["count"] = msgCount;

    JsonArray messages = msgJson.createNestedArray("messages");
    for (int index = 0; index < msgCount; index++)
    {
      MyStringStream stringStream;
      uint8_t *msg = _messageParser->getQZSSdcrMessage(index);
      uint16_t totalMessageLength = _messageParser->getQZSSdcrMessageSize(index);
      for (int pos = 0; pos < totalMessageLength; pos++)
      {
        //stringStream.print(msg[pos], HEX);
        stringStream.printf("%02x", msg[pos]);
      }
      messages.add(stringStream.getString().c_str());
    }
    String outputJson;
    serializeJson(msgJson, outputJson);
    stream.println(outputJson);
    stream.println("");
  }

public:
  SendReceivedMessage(SensorDataHolder *dataHolder, UbxMessageParser *messageParser)
  {
    _dataHolder = dataHolder;
    _messageParser = messageParser;
  }

  bool checkReceivedString(Stream &stream, String &str)
  {
    if (str.indexOf("CMD:RESTART") == 0)
    {
        Serial.println(" - - - RESTART - - - ");
        delay(WAIT_DUR);
        ESP.restart();
        return (true);
    }
    else if (str.indexOf("CMD:GETDCRMSG") == 0)
    {
      // ----- DCR/DCXメッセージを取得し送信する
      _sendReceivedDcrMessages(stream);
      return (true);
    }
    return (false);
  }
};
