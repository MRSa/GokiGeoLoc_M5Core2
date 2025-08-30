

class ShowGSIMap
{

private:
  GsiMapDrawer *_myCanvas = NULL;
  int _zoomLevel = 16;

public:
  ShowGSIMap()
  {
      // ----- Canvasを作成
      _myCanvas = new GsiMapDrawer(DISPLAY_MAPSIZE, DISPLAY_MAPSIZE);

  }

  void drawScreen(TinyGPSPlus &gps, SensorDataHolder *dataHolder)
  {
    try
    {
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
    }
    catch (...)
    {
      Serial.println("Picture Load Exception...");
    }
  }
};
