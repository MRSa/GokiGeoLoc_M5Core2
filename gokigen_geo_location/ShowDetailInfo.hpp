class ShowDetailInfo
{

private:


public:
  ShowDetailInfo()
  {

  }

  void drawScreen(TinyGPSPlus &gps, SensorDataHolder *dataHolder)
  {
    uint8_t batteryLevel = dataHolder->getBatteryLevel(); // 電池残量
    struct tm timeinfo = dataHolder->getTimeInfo(); // 時刻

    // M5.Display.clear();  // ちらつくので実行しない
    M5.Display.setCursor(0,0);
    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::efontJA_16_b);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.printf("### 詳細情報 ###\r\n");
    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::efontJA_16);
    M5.Display.setCursor(0,20);
    M5.Display.printf("時刻: %04d-%02d-%02d %02d:%02d:%02d\r\n" 
                    ,timeinfo.tm_year + 1900
                    ,timeinfo.tm_mon + 1
                    ,timeinfo.tm_mday
                    ,timeinfo.tm_hour + 9  // UTC -> JST
                    ,timeinfo.tm_min
                    ,timeinfo.tm_sec
                    );
    // M5.Display.printf("衛星: %d\r\n", gps.satellites.value());
    M5.Display.printf("緯度: %.6f\r\n経度: %.6f\r\n高度: %.2f m\r\n", gps.location.lat(), gps.location.lng(), gps.altitude.meters());
    M5.Display.printf("温度: %.1f ℃\r\n気圧: %.1f hPa\r\n", dataHolder->getTemperature(), (dataHolder->getPressure() / 100.0f));
    M5.Display.printf("電池: %02d %%\r\n", batteryLevel);
  }
};
