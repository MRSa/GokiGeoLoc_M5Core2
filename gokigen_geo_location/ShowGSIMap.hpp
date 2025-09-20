
class ShowGSIMap
{

private:
  GsiMapDrawer *_myCanvas = NULL;
  int _zoomLevel = 16;
  bool _isMapDrawed = false;
  float _lastHeading = -1.0f;
  
  void _drawCompass(float centerX, float centerY, float radius, float headings)
  {
    // ----- 外枠を描画
    M5.Display.fillRect((centerX - radius), (centerY - radius), (radius * 2.0f), (radius * 2.0f), TFT_BLACK);
    // M5.Display.drawRect((centerX - radius), (centerY - radius), (radius * 2.0f), (radius * 2.0f), TFT_WHITE);
    M5.Display.drawCircle(centerX, centerY, radius, TFT_WHITE);

    float radian = (headings - 90.0f) * PI / 180.0f;
    float x = centerX + radius * cos(radian);
    float y = centerY + radius * sin(radian);

    // --- 方位の針を赤色で示す
    M5.Display.drawLine(centerX, centerY, x, y, RED);
    M5.Display.drawLine(centerX + 1, centerY, x, y, RED);
    M5.Display.drawLine(centerX, centerY + 1, x, y, RED);
    // M5.Display.fillTriangle(centerX + 5, centerY + 5, centerX - 5, centerY - 5, x, y, RED);
  }

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
      M5.Display.setCursor(0,0);
      M5.Display.setTextSize(1);
      M5.Display.setFont(&fonts::efontJA_16_b);
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Display.printf("## 位置 ##\r\n");

      if ((!gps.location.isUpdated())&&(!_isMapDrawed))
      {
        // ----- 1回地図を描画した後は、「位置特定中」表示はしない
        M5.Display.setCursor(110,30);
        M5.Display.setTextSize(1);
        M5.Display.setFont(&fonts::efontJA_16);
        M5.Display.setTextColor(TFT_VIOLET, TFT_BLACK);
        M5.Display.printf("位置特定中...\r\n");
        return;
      }

      // ----- 操作領域にマーキングを施す
      M5.Display.setTextSize(1);
      M5.Display.setFont(&fonts::efontJA_14_b);
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Display.setCursor(308,0);
      M5.Display.printf("Z");
      M5.Display.drawRect(304,0, 15, 15, TFT_WHITE);

      // ----- 緯度経度を取得
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
      
      // ----- 時刻表示 (地図の下)
      M5.Display.setCursor(105,225);
      M5.Display.setTextSize(1);
      M5.Display.setFont(&fonts::efontJA_14);
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
      displayCurrentJstTime("", dataHolder->getTimeInfo());

      // ----- 情報表示（左側）
      M5.Display.setCursor(0,25);
      M5.Display.setFont(&fonts::efontJA_16);
      M5.Display.printf("高度:\r\n %.2fm\r\n", gps.altitude.meters());
      M5.Display.setCursor(0,65);
      M5.Display.setFont(&fonts::efontJA_16);
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Display.printf("気圧:\r\n %.1f hPa\r\n", (dataHolder->getPressure() / 100.0d));
      M5.Display.setCursor(0,105);
      M5.Display.printf("気温:\r\n %.1f ℃\r\n", dataHolder->getTemperature());

      // ----- コンパス等表示エリア
      if (dataHolder->isEnableGeomagneticSensor())
      {
        // ----- 方位を表示
        float heading = dataHolder->getCompassHeadingDegree(lat, lng);
        if (abs(_lastHeading - heading) > 1.0f)
        {
          // ----- 変位がそれなりにある時だけ、描画し直し
          _drawCompass(45, 172, 30, heading);
        }
        _lastHeading = heading;  // 最新方位を保管
      }
      else
      {
        // ---- コンパスは表示しないので、領域を黒で塗りつぶし
        M5.Display.fillRect(15, 142, 60, 60, TFT_BLACK);  // X: 0-95(最大) Y:60(最大)
      }

      // ----- 画面左下の情報表示
      M5.Display.setCursor(0,205);
      M5.Display.setFont(&fonts::efontJA_14);
      M5.Display.printf("Zoom: %2d\r\n", _zoomLevel);

      M5.Display.setCursor(0,220);
      M5.Display.setFont(&fonts::efontJA_14);
      M5.Display.printf("電池: %02d%%\r\n", dataHolder->getBatteryLevel());

      _isMapDrawed = true;
    }
    catch (...)
    {
      Serial.println("Picture Load Exception...");
      _isMapDrawed = false;
    }
  }
  
  void touchedPosition(int posX, int posY)
  {
    bool vibration = false;

    // ---- 左上が押されたとき（マップモードの切り替え）
    if ((posX < 95)&&(posY < 45))
    {
      // ----- マップ表示モードを切り替える
      Serial.println("Change map Mode");
      showDisplayMode = SHOW_GSIMAP_ANY;
      needClearScreen = true;
      vibration = true;
    }
    // ---- コンパス表示エリア(仮)が押されたとき
    if ((posX < 95)&&(posY > 140)&&(posY < 210))
    {
      // ----- そのうち何か処理を入れたい
      Serial.println("Pushed the area.");
    }
    
    // ===== 右上の場所を押したときの処理 (ちょっと反応が悪いので考える...)
    if ((posX > 280)&&(posY < 60))
    {
      // ----- ズームレベルを変更する
      _zoomLevel = getNextZoomLevel(_zoomLevel);
      Serial.print("next zoom level ");
      Serial.println(_zoomLevel);
      
      vibration = true;
    }

    // ----- バイブレーション 実行
    if (vibration)
    {
      makeVibration(VIBRATION_MIDDLE, VIBRATION_TIME_SHORT);
      isHandledMessage = true;
    }
  }
};
