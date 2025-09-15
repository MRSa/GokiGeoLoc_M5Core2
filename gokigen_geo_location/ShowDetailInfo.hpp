#include "ImuCalibration.hpp"

class ShowDetailInfo
{

private:
  ImuCalibration calibrator;

public:
  ShowDetailInfo()
  {

  }

  void drawScreen(TinyGPSPlus &gps, SensorDataHolder *dataHolder, TouchPositionHandler *touchPos)
  {
    uint8_t batteryLevel = dataHolder->getBatteryLevel(); // 電池残量

    // M5.Display.clear();  // ちらつくので実行しない
    M5.Display.setCursor(0,0);
    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::efontJA_16_b);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.printf("## 詳細情報 ##\r\n");

    // ----- 操作領域にマーキング 
    // (輝度の変更)
    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::efontJA_14_b);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setCursor(308,0);
    M5.Display.printf("B");
    M5.Display.drawRect(304,0, 15,15, TFT_WHITE);
    // (キャリブレーションの実行)
    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::efontJA_14_b);
    if (calibrator.isCalibrating())
    {
      // ----- キャリブレーション実行中
      M5.Display.fillRect(250,222, 50,17, TFT_WHITE);
      M5.Display.setTextColor(TFT_BLACK, TFT_WHITE);
    }
    else
    {
      // ---- キャリブレーション未実行
      M5.Display.fillRect(250,222, 50,17, TFT_BLACK);  
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    M5.Display.setCursor(254,223);
    M5.Display.printf("Calib.");
    M5.Display.drawRect(250,222, 50,17, TFT_WHITE);

    // ----- 各種情報の表示
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setFont(&fonts::efontJA_16);
    M5.Display.setCursor(0,20);
    displayCurrentJstTime("時刻: ", dataHolder->getTimeInfo());
    if (!gps.location.isValid())
    {
      if (!gps.hdop.isValid())
      {
        // ----- 衛星からの電波を受信できていない場合
        M5.Display.printf("衛星: 無効(%2d)      \r\n", gps.satellites.value());
      }
      else
      {
        // ----- 衛星からの電波を受信できているが、十分でない場合
        M5.Display.printf("衛星: 受信(%2d)      \r\n", gps.satellites.value());
      }
    }
    else
    {
      M5.Display.printf("衛星: 有効(%2d)    \r\n", gps.satellites.value());      
    }
    M5.Display.printf("緯度: %3.6f  経度: %3.6f      \r\n高度: %.2f m (BMP280: %.2f m)       \r\n", gps.location.lat(), gps.location.lng(), gps.altitude.meters(), dataHolder->getAltitude());
    M5.Display.printf("気温: %.1f ℃ (BMI270: %.1f ℃)      \r\n", dataHolder->getTemperature(), dataHolder->getImuTemperature());
    M5.Display.printf("気圧: %.1f hPa\r\n", (dataHolder->getPressure() / 100.0d));
    // M5.Display.printf("沸点: %.1f ℃\r\n", dataHolder->getWaterBoilingPoint());   // 取得してみたが、、、あきらかに値がおかしいので取得をやめる
    M5.Display.printf("重力: X:%-2.1f y:%-2.1f z:%-2.1f       \r\n", dataHolder->getAccelX(), dataHolder->getAccelY(), dataHolder->getAccelZ());
    M5.Display.printf("傾き: X:%-3.1f y:%-3.1f z:%-3.1f       \r\n", dataHolder->getGyroX(), dataHolder->getGyroY(), dataHolder->getGyroZ());
    M5.Display.printf("磁力: X:%-3.1f y:%-3.1f z:%-3.1f       \r\n", dataHolder->getMagX(), dataHolder->getMagY(), dataHolder->getMagZ());
    if (dataHolder->isEnableGeomagneticSensor())
    {
      M5.Display.printf("方位: %.1f °     \r\n", dataHolder->getCompassHeadingDegree(gps.location.lat(), gps.location.lng()));      
    }
    else
    {
      M5.Display.printf("方位: 無効       \r\n");         
    }
    M5.Display.printf("電池: %02d %%\r\n", batteryLevel);
    M5.Display.printf("輝度: %02x\r\n", getDisplayBrightness());

    // ----- 画面タッチ時の操作 -----
    if (touchPos->isPressed())
    {
      //----- タッチパネルが押されたことを検出
      int posX = touchPos->getTouchX();
      int posY = touchPos->getTouchY();
      if ((posX > 280)&&(posY < 60))
      {
        // 右上をタッチすることで、ディスプレイの明るさを変更する
        changeDisplayBrightness();

        // ----- バイブレーション
        makeVibration(VIBRATION_MIDDLE, VIBRATION_TIME_SHORT);
      }
      else if ((posX > 280)&&(posY < 200))
      {
        // 右下をタッチすることで、センサのキャリブレーションに入る

        // ----- バイブレーション
        makeVibration(VIBRATION_MIDDLE, VIBRATION_TIME_SHORT);

        // ----- センサのキャリブレーションを実行
        calibrator.executeCalibration(101);
      }

      // ---- 開放する
      touchPos->resetPosition();
    }
    // ----- キャリブレーション中 or 実行のためのトリガをクラスに入力
    calibrator.continueCalibration();
  }
};
