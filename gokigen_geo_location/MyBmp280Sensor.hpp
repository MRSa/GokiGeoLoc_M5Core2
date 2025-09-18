
class MyBmp280Sensor
{
private:
  // キャリブレーションデータを格納する変数
  uint16_t _dig_T1;
  int16_t _dig_T2, _dig_T3;
  uint16_t _dig_P1;
  int16_t _dig_P2, _dig_P3, _dig_P4, _dig_P5, _dig_P6, _dig_P7, _dig_P8, _dig_P9;

  int32_t _t_fine = 0;   // 精密な温度・気圧計算に使用する変数

  uint32_t _adc_T = 0;   // センサーデータ（温度）
  uint32_t _adc_P = 0;   // センサーデータ（気圧）

  m5::I2C_Class &_i2c;
  
  bool _readCalibrationData()
  {
    // readRegisterメソッドを使用することで、レジスタアドレスの書き込みと
    // データの読み取りを一度に行うことができる。
    std::uint8_t buffer[24];
    if (!_i2c.readRegister(BMP280_SENSOR_ADDR, BMP280_CALIB_REG_START, buffer, 24, I2C_COMMUNICATION_SPEED))
    {
      return false; // 通信失敗
    }

    // バッファから値を読み出し、メンバ変数に格納
    _dig_T1 = buffer[0] | (buffer[1] << 8);
    _dig_T2 = buffer[2] | (buffer[3] << 8);
    _dig_T3 = buffer[4] | (buffer[5] << 8);
    _dig_P1 = buffer[6] | (buffer[7] << 8);
    _dig_P2 = buffer[8] | (buffer[9] << 8);
    _dig_P3 = buffer[10] | (buffer[11] << 8);
    _dig_P4 = buffer[12] | (buffer[13] << 8);
    _dig_P5 = buffer[14] | (buffer[15] << 8);
    _dig_P6 = buffer[16] | (buffer[17] << 8);
    _dig_P7 = buffer[18] | (buffer[19] << 8);
    _dig_P8 = buffer[20] | (buffer[21] << 8);
    _dig_P9 = buffer[22] | (buffer[23] << 8);

    return true;
  }

  bool _readSensorData()
  {
    // 6バイトの生データを読み取るためのバッファ
    std::uint8_t buffer[6];

    // ----- センサデータの読み取り
    if (!_i2c.readRegister(BMP280_SENSOR_ADDR, BMP280_PRESS_MSB_REG, buffer, 6, I2C_COMMUNICATION_SPEED))
    {
      // 読み取り失敗...エラー応答
      //Serial.print("BME280: Read Reg. Error.");
      _adc_T = 0;
      _adc_P = 0;
      return false;
    }

    // ----- センサデータの格納
    _adc_P = (static_cast<uint32_t>(buffer[0]) << 12) | (static_cast<uint32_t>(buffer[1]) << 4) | (static_cast<uint32_t>(buffer[2]) >> 4);
    _adc_T = (static_cast<uint32_t>(buffer[3]) << 12) | (static_cast<uint32_t>(buffer[4]) << 4) | (static_cast<uint32_t>(buffer[5]) >> 4);
    return true;
  }

public:
  MyBmp280Sensor(m5::I2C_Class &i2c) : _i2c(i2c)
  {
  }

  bool begin()
  {
    // 1. チップIDの確認（通信確認）
    // readRegister8を使用することで、レジスタの値を1バイト読み取る処理を簡潔に記述
    if (_i2c.readRegister8(BMP280_SENSOR_ADDR, BMP280_CHIP_ID_REG, I2C_COMMUNICATION_SPEED) != BMP280_CHIPID)
    {
      Serial.println("BMP280: Wrong Chip ID..."); // BEGIN
      return false; // チップIDが異なるので、初期化失敗
    }
    
    // 2. キャリブレーションデータの読み込み (_readCalibrationData()を呼び出す)
    if (!_readCalibrationData())
    {
      Serial.println("BMP280: failure to read a calibration data..."); // BEGIN
      return false;  // キャリブレーションデータの読み込みに失敗
    }
    
    // 3. センサーの初期設定
    // writeRegister8を使用することで、レジスタに1バイト書き込む処理を簡潔に記述
    if (!_i2c.writeRegister8(BMP280_SENSOR_ADDR, BMP280_CTRL_MEAS_REG, 0b00100111, I2C_COMMUNICATION_SPEED))
    {
      Serial.println("BMP280: failure to initialize sensor..."); // BEGIN
      return false; // 初期設定に失敗
    }
    return true;
  }

  bool update()
  {
    // ----- 微妙に仕様変更。 update()でデータ更新する
    return (_readSensorData());
  }

  double readTemperature()
  {
    // BMP280データシートの「Appendix 1: Computation formulae for 32 bit systems」セクションを参照
/*
    // ----- doubleバージョン
    double var1, var2, T;
    var1 = ((((double)_adc_T) / 16384.0d - ((double)_dig_T1) / 1024.0d) * ((double)_dig_T2));
    var2 = ((((double)_adc_T) / 131072.0d - ((double)_dig_T1) / 8192.0d) * (((double)_adc_T) / 131072.0d - ((double)_dig_T1) / 8192.0d)) * ((double)_dig_T3);

    _t_fine = (int32_t)(var1 + var2);

    T = (_t_fine / 5120.0d);
    return T;
*/
/**/
    // ----- 32bit integerバージョン
    int32_t var1, var2, T;
    var1 = ((((_adc_T>>3) - ((int32_t)_dig_T1<<1))) * ((int32_t)_dig_T2)) >> 11;
    var2 = (((((_adc_T>>4) - ((int32_t)_dig_T1)) * ((_adc_T>>4) - ((int32_t)_dig_T1))) >> 12) * ((int32_t)_dig_T3)) >> 14;

    _t_fine = var1 + var2;
    
    T = (_t_fine * 5 + 128) >> 8;
    
    return (double) T / 100.0d;
/**/
  }

  double readPressure()
  {
    // BMP280データシートの「Appendix 1: Computation formulae for 32 bit systems」セクションを参照
/*
    // ----- doubleバージョン
    double var1, var2, p;
    readTemperature();    
    var1 = ((double)_t_fine/2.0d) - 64000.0d;
    var2 = var1 * var1 * ((double)_dig_P6) / 32768.0d;
    var2 = var2 + var1 * ((double)_dig_P5) * 2.0d;
    var2 = (var2 / 4.0d)+(((double)_dig_P4) * 65536.0d);
    var1 = (((double)_dig_P3) * var1 * var1 / 524288.0d + ((double)_dig_P2) * var1) / 524288.0d;
    var1 = (1.0d + var1 / 32768.0d) * ((double)_dig_P1);
    if (var1 == 0.0d)
    {
      return 0; // avoid exception caused by division by zero
    }
    p = 1048576.0d - (double)_adc_P;
    p = (p - (var2 / 4096.0d)) * 6250.0d / var1;
    var1 = ((double)_dig_P9) * p * p / 2147483648.0d;
    var2 = p * ((double)_dig_P8) / 32768.0d;
    p = p + (var1 + var2 + ((double)_dig_P7)) / 16.0d;
    return p;
*/
/**/
    // ----- 32bit integerバージョン
    int32_t var1, var2;
    uint32_t p;
    readTemperature();    
    var1 = (((int32_t)_t_fine)>>1) - (int32_t) 64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)_dig_P6);
    var2 = var2 + ((var1*((int32_t)_dig_P5))<<1);
    var2 = (var2>>2)+(((int32_t)_dig_P4)<<16);
    var1 = (((_dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)_dig_P2) * var1)>>1))>>18;
    var1 =((((32768+var1))*((int32_t)_dig_P1))>>15);
    if (var1 == 0)
    {
      return 0.0d; // avoid exception caused by division by zero
    }
    p = (((uint32_t)(((int32_t)1048576)-_adc_P)-(var2>>12)))*3125;
    if (p < 0x80000000)
    {
      p = (p << 1) / ((uint32_t)var1);
    }
    else
    {
      p = (p / (uint32_t)var1) * 2;
    }
    var1 = (((int32_t)_dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
    var2 = (((int32_t)(p>>2)) * ((int32_t)_dig_P8))>>13;
    p = (uint32_t)((int32_t)p + ((var1 + var2 + _dig_P7) >> 4));

    return (double) p; // 単位はPa
/**/
  }

  double readAltitude(double seaLevelhPa = 1013.25d)
  {
    double altitude;
    double pressure = readPressure(); // in Si units for Pascal
    pressure /= 100.0d;   // convert from Pa to hPa
    altitude = 44330.0d * (1.0d - pow(pressure / seaLevelhPa, 0.1903d));

    return altitude;
  }

  double waterBoilingPoint(double pressure)
  {
    return (234.175d * log(pressure / 6.1078d)) / (17.08085d - log(pressure / 6.1078d));
  }

};
/*
  BMP280のデータ変換ロジックについて

  データの変換ロジックは、「Adafruit BMP280 Library」および BMP280 のデータシートを参照させていただきました。
  （ https://github.com/adafruit/Adafruit_BMP280_Library  データシート: https://cdn-shop.adafruit.com/datasheets/BST-BMP280-DS001-11.pdf）

    ※ https://raw.githubusercontent.com/adafruit/Adafruit_BMP280_Library/refs/heads/master/Adafruit_BMP280.cpp

     ...最初は、Adafruit_BMP280.h がそのまま使えていたのに、、なぜかWire1でうまく動かなくなって、、、M5Unifiedのm5::I2C_Class を使うよう調整しました。

*/
/*!
 *  @file Adafruit_BMP280.cpp
 *
 *  This is a library for the BMP280 orientation sensor
 *
 *  Designed specifically to work with the Adafruit BMP280 Sensor.
 *
 *  Pick one up today in the adafruit shop!
 *  ------> https://www.adafruit.com/product/2651
 *
 *  These sensors use I2C to communicate, 2 pins are required to interface.
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit andopen-source hardware by purchasing products
 *  from Adafruit!
 *
 *  K.Townsend (Adafruit Industries)
 *
 *  BSD license, all text above must be included in any redistribution
 */
