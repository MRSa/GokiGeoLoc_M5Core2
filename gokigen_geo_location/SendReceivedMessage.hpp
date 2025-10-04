#include <ArduinoJson.h>

class SendReceivedMessage
{
private:
  SensorDataHolder *_dataHolder;
  UbxMessageParser *_messageParser;

  void _sendErrorResponse(Stream &stream, int count) {
    StaticJsonDocument<128> errJson;
    errJson["result"] = false;
    errJson["count"] = count;
    serializeJson(errJson, stream);
  }

  void _sendReceivedDcrMessages(Stream &stream)
  {
    int msgCount = _messageParser->getMessageCount();
    if (msgCount <= 0)
    {
      _sendErrorResponse(stream, msgCount);
      return;
    }

    // JSON ドキュメントの必要なメモリサイズを正確に計算
    size_t docSize = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(msgCount);
    for (int index = 0; index < msgCount; index++)
    {
      uint16_t totalMessageLength = _messageParser->getQZSSdcrMessageSize(index);
      size_t stringLength = totalMessageLength * 2;
      // 1バイトは2桁の16進数に変換されるため、サイズは totalMessageLength * 2
      docSize += JSON_STRING_SIZE(stringLength) + stringLength + 1; // +1 はヌル終端文字
    }

    // 動的 JSON ドキュメントを確保
    DynamicJsonDocument msgJson(docSize + 256); // 予備のバッファとして +256
    if (msgJson.capacity() == 0)
    {
      // ----- メッセージの確保に失敗(エラー応答する)
      _sendErrorResponse(stream, -2); // -2: DynamicJsonDocument確保失敗
      return;
    }

    msgJson["result"] = true;
    msgJson["count"] = msgCount;

    JsonArray messages = msgJson.createNestedArray("messages");
    for (int index = 0; index < msgCount; index++)
    {
      uint8_t *msg = _messageParser->getQZSSdcrMessage(index);
      uint16_t totalMessageLength = _messageParser->getQZSSdcrMessageSize(index);

      // 16進数文字列を格納するのに必要なバッファを確保 (1バイトあたり2文字 + ヌル終端文字)
      size_t bufferSize = totalMessageLength * 2 + 1;
      char *hexBuffer = (char *)malloc(bufferSize);
      if (!hexBuffer)
      {
        // ----- メモリ確保失敗(エラー応答する)
        _sendErrorResponse(stream, -1); // -1: メッセージバッファ確保失敗
        return;
      }
      char *p = hexBuffer;

      // 効率的なループで16進数文字列を生成
      for (int pos = 0; pos < totalMessageLength; pos++)
      {
        p += sprintf(p, "%02x", msg[pos]);
      }
    
      // JSON配列に直接文字列を追加
      messages.add(hexBuffer);
      free(hexBuffer); // 確保したバッファを解放
    }
    serializeJson(msgJson, stream);
  }

  void _sendSensorData(Stream &stream)
  {
    StaticJsonDocument<1024> msgJson;
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
    msgJson["heap"] = esp_get_free_heap_size();
    serializeJson(msgJson, stream);
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
