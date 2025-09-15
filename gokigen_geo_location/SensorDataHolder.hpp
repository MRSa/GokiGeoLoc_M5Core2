
class SensorDataHolder
{
private:
  // ----- 各種センサー情報
  MyBmp280Sensor& _bmp;
  double _temperature = 0.0d;  // 温度
  double _pressure = 0.0d;     // 圧力
  double _altitude = 0.0d;     // 高度
  double _waterBoilingPoint = 100.0d; // 沸点
  float _imuTemperature = 0.0f;  // 温度(IMU)
  m5::imu_data_t _imuData;     // IMUデータ
  uint8_t _batteryLevel = 0;   // バッテリ残量
  struct tm _timeInfo;         // 時刻情報

  float _getMagneticDeclination(float latitude, float longitude)
  {
    // --- ざっくり偏角は 西偏 8° とする。
    return (-8.0f);
  }

public:
  SensorDataHolder(MyBmp280Sensor &bmp280) : _bmp(bmp280)
  {
  }

  uint8_t getBatteryLevel() { return _batteryLevel; }
  float getImuTemperature() { return _imuTemperature;  }
  double getTemperature() { return _temperature; }
  double getPressure() { return _pressure; }
  double getAltitude() { return _altitude; }
  double getWaterBoilingPoint() { return _waterBoilingPoint; } 
  float getAccelX() { return _imuData.accel.x; }  // 重力加速度 (X軸)
  float getAccelY() { return _imuData.accel.y; }  // 重力加速度 (Y軸)
  float getAccelZ() { return _imuData.accel.z; }  // 重力加速度 (Z軸)
  float getGyroX() { return _imuData.gyro.x; }    // ジャイロ   (X軸)
  float getGyroY() { return _imuData.gyro.y; }    // ジャイロ   (Y軸)
  float getGyroZ() { return _imuData.gyro.z; }    // ジャイロ   (Z軸)
  float getMagX() { return _imuData.mag.x; } 
  float getMagY() { return _imuData.mag.y; }
  float getMagZ() { return _imuData.mag.z; }

  struct tm *getTimeInfo() { return &_timeInfo; }

  bool isEnableGeomagneticSensor()
  {
    if ((_imuData.mag.x == 0.0f)&&((_imuData.mag.y == 0.0f))&&(_imuData.mag.z == 0.0f))
    {
      return false;
    }
    float magX = _imuData.mag.x;
    float magY = _imuData.mag.y;
    float magZ = _imuData.mag.z;
    if (!M5.Imu.getMag(&magX, &magY, &magZ))
    {
      return false;
    }
    return true;
  }

  float getCompassHeadingDegree(float latitude, float longitude)
  {
    if (!isEnableGeomagneticSensor())
    {
      // ----- コンパスが無効な場合は、-1° にする
      return (0.0f);
    }
    float magX = _imuData.mag.x;
    float magY = _imuData.mag.y;
    float magZ = _imuData.mag.z;

    // 方位（Heading）を計算
    float heading = atan2(magY, magX) * 180.0 / PI;

    // 負の値になった場合は360を加算して正規化
    if (heading < 0.0f)
    {
      heading += 360.0f;
    }
    // 360度を超えた場合は調整
    if (heading > 360.0f)
    {
      heading -= 360.0f;
    } 
    
    // 磁気偏角を適用して真北に補正
    heading += _getMagneticDeclination(latitude, longitude);

    // 負の値になった場合は360を加算して正規化
    if (heading < 0.0f)
    {
      heading += 360.0f;
    }

    // 360度を超えた場合は調整
    if (heading > 360.0f)
    {
      heading -= 360.0f;
    } 
    return (heading);   
  }

  void updateSensorData()
  {
    // バッテリ残量を取得
    _batteryLevel = M5.Power.getBatteryLevel();
    
    // 温度と圧力と高度(と沸点)を取得する
    if (_bmp.update())
    {
      _temperature = _bmp.readTemperature();
      _pressure = _bmp.readPressure();
      _altitude = _bmp.readAltitude();
      _waterBoilingPoint = _bmp.waterBoilingPoint((float) _pressure);
    }

    // IMUの情報を取得
    if (M5.Imu.update() > 0)
    {
      M5.Imu.getImuData(&_imuData);
      M5.Imu.getTemp(&_imuTemperature);
    }
    
    // 現在時刻を取得
    if(!getLocalTime(&_timeInfo))
    {
      // Serial.println("--- Failed to obtain time ---");
    }
  }

};
