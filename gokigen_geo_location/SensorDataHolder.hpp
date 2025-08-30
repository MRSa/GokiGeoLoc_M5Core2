class SensorDataHolder
{

private:
  // ----- 各種センサー情報
  Adafruit_BMP280 bmp;
  double _temperature = 0.0d;  // 温度
  double _pressure = 0.0d;     // 圧力
  uint8_t _batteryLevel = 0;   // バッテリ残量
  struct tm _timeInfo;         // 時刻情報

public:
  SensorDataHolder(Adafruit_BMP280 &bmp280)
  {
    bmp = bmp280;
  }

  uint8_t getBatteryLevel() { return _batteryLevel; }
  double getTemperature() { return _temperature; }
  double getPressure() { return _pressure; }
  struct tm getTimeInfo() { return _timeInfo; }
  


  void updateSensorData()
  {
    // バッテリ残量を取得
    _batteryLevel = M5.Power.getBatteryLevel();
    
    // 温度と圧力を取得する
    _temperature = bmp.readTemperature();
    _pressure = bmp.readPressure();
    
    // 現在時刻を取得
    if(!getLocalTime(&_timeInfo))
    {
      Serial.println("--- Failed to obtain time ---");
    }
  }
};
