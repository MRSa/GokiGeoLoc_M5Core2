
class ShowDetailInfo
{

private:


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
    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::efontJA_14_b);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setCursor(308,0);
    M5.Display.printf("B");
    M5.Display.drawRect(304,0, 15,15, TFT_WHITE);

    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::efontJA_16);
    M5.Display.setCursor(0,20);
    displayCurrentJstTime("時刻: ", dataHolder->getTimeInfo());

    // M5.Display.printf("衛星: %d\r\n", gps.satellites.value());
    M5.Display.printf("緯度: %.6f\r\n経度: %.6f\r\n高度: %.2f m (BMP280: %.2f m)\r\n", gps.location.lat(), gps.location.lng(), gps.altitude.meters(), dataHolder->getAltitude());
    M5.Display.printf("気温: %.1f ℃\r\n気圧: %.1f hPa\r\n", dataHolder->getTemperature(), (dataHolder->getPressure() / 100.0d));
    // M5.Display.printf("沸点: %.1f ℃\r\n", dataHolder->getWaterBoilingPoint());   // 取得してみたが、、、あきらかに値がおかしいので取得をやめる
    M5.Display.printf("重力: X:%-2.1f y:%-2.1f z:%-2.1f       \r\n", dataHolder->getAccelX(), dataHolder->getAccelY(), dataHolder->getAccelZ());
    M5.Display.printf("傾き: X:%-3.1f y:%-3.1f z:%-3.1f       \r\n", dataHolder->getGyroX(), dataHolder->getGyroY(), dataHolder->getGyroZ());
    // M5.Display.printf("磁力: X:%-3.1f y:%-3.1f z:%-3.1f       \r\n", dataHolder->getMagX(), dataHolder->getMagY(), dataHolder->getMagZ()); // 磁力は取得できないようだ
    M5.Display.printf("電池: %02d %%\r\n", batteryLevel);
    M5.Display.printf("輝度: %02x\r\n", getDisplayBrightness());

    // ----- ディスプレイの明るさ変更 -----
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

      // ---- 開放する
      touchPos->resetPosition();
    }
  }
};
