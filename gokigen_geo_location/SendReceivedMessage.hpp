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

  void _sendSensorData(Stream &stream)
  {
    DynamicJsonDocument msgJson(65535);
    msgJson["result"] = true;
    msgJson["battery"] = _dataHolder->getBatteryLevel();
    msgJson["imu_temp"] = _dataHolder->getImuTemperature();
    msgJson["temperature"] = _dataHolder->getTemperature();
    msgJson["pressure"] = _dataHolder->getPressure();
    msgJson["altitude"] = _dataHolder->getAltitude();
    msgJson["acc_x"] = _dataHolder->getAccelX();
    msgJson["acc_y"] = _dataHolder->getAccelY();
    msgJson["acc_z"] = _dataHolder->getAccelZ();
    msgJson["gyro_x"] = _dataHolder->getGyroX();
    msgJson["gyro_y"] = _dataHolder->getGyroY();
    msgJson["gyro_z"] = _dataHolder->getGyroZ();
    msgJson["mag_x"] = _dataHolder->getMagX();
    msgJson["mag_y"] = _dataHolder->getMagY();
    msgJson["mag_z"] = _dataHolder->getMagZ();
    msgJson["is_gps"] = gps.location.isValid();
    msgJson["lat"] = gps.location.lat();
    msgJson["lng"] = gps.location.lng();
    msgJson["alt"] = gps.altitude.meters();
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
    else if (str.indexOf("CMD:GETSENSOR") == 0)
    {
      // ----- センサーデータを取得し送信する
      _sendSensorData(stream);
      return (true);
    }
    return (false);
  }
};
