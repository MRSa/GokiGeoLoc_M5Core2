
class ShowGSIMap
{

private:
  GsiMapDrawer *_myCanvas = NULL;
  int _zoomLevel = 16;
  bool isMapDrawed = false;

public:
  ShowGSIMap()
  {
      // ----- Canvasを作成
      _myCanvas = new GsiMapDrawer(DISPLAY_MAPSIZE, DISPLAY_MAPSIZE);
  }

  void drawScreen(TinyGPSPlus &gps, SensorDataHolder *dataHolder, TouchPositionHandler *touchPos)
  {
    try
    {
      M5.Display.setCursor(0,0);
      M5.Display.setTextSize(1);
      M5.Display.setFont(&fonts::efontJA_16_b);
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Display.printf("地図表示\r\n");

      if ((!gps.location.isUpdated())&&(!isMapDrawed))
      {
        // ----- 1回地図を描画した後は、「位置特定中」表示はしない
        M5.Display.setCursor(110,30);
        M5.Display.setTextSize(1);
        M5.Display.setFont(&fonts::efontJA_16);
        M5.Display.setTextColor(TFT_VIOLET, TFT_BLACK);
        M5.Display.printf("位置特定中...\r\n");
        return;
      }

      double lat = gps.location.lat();
      double lng = gps.location.lng();

      // ----- 画像タイル座標と画像内の座標をGPS緯度経度から決定
      GsiTileCoordinate tile(lat,  lng, _zoomLevel);

      // ディスプレイに表示する左上座標を与えて地図表示
      int displayX = 105;
      int displayY = 25;
      _myCanvas->loadGsiMap(_zoomLevel, tile.getTileX(), tile.getTileY(), tile.getPixelPosX(), tile.getPixelPosY(), displayX, displayY);
      
      // ----- 緯度経度表示
      M5.Display.setCursor(105,0);
      M5.Display.setTextSize(1);
      M5.Display.setFont(&fonts::efontJA_16);
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Display.printf("緯度:%3.3f 経度:%3.3f\r\n", lat, lng);
      
      // ----- 時刻表示 (左下)
      struct tm timeinfo = dataHolder->getTimeInfo();
      M5.Display.setCursor(105,225);
      M5.Display.setTextSize(1);
      M5.Display.setFont(&fonts::efontJA_14);
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Display.printf("%04d-%02d-%02d %02d:%02d:%02d\r\n" 
                    ,timeinfo.tm_year + 1900
                    ,timeinfo.tm_mon + 1
                    ,timeinfo.tm_mday
                    ,timeinfo.tm_hour + 9  // UTC -> JST
                    ,timeinfo.tm_min
                    ,timeinfo.tm_sec
      );

      // ----- 情報表示（左側）
      M5.Display.setCursor(5,25);
      M5.Display.setFont(&fonts::efontJA_16);
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Display.printf("高度:%.2fm\r\n", gps.altitude.meters());
      M5.Display.printf("気温:%.1f ℃\r\n気圧:%.1fhPa\r\n", dataHolder->getTemperature(), (dataHolder->getPressure() / 100.0f));
      M5.Display.printf("電池:%02d%%\r\n", dataHolder->getBatteryLevel());


      if (touchPos->isPressed())
      {
        //----- タッチパネルが押されたことを検出



        // ---- 押された場所をSerial出力
        Serial.print("Touch Position:[");
        Serial.print(touchPos->getTouchX());
        Serial.print(",");
        Serial.print(touchPos->getTouchY());
        Serial.println("]");

        // ----- バイブレーション
        makeVibration(VIBRATION_WEAK, VIBRATION_TIME_MIDDLE);

        // ---- 開放する
        touchPos->resetPosition();
      }
      isMapDrawed = true;
    }
    catch (...)
    {
      Serial.println("Picture Load Exception...");
      isMapDrawed = false;
    }
  }
};
