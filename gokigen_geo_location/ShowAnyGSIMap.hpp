
class ShowAnyGSIMap
{

private:
  GsiMapDrawer *_myCanvas = NULL;
  int _GsiMapSize = GSI_MAP_PIXEL;
  int _zoomLevel = 16;
  int _tileX = 58211;
  int _tileY = 25806;
  int _posX = 127;
  int _posY = 127;

public:
  ShowAnyGSIMap()
  {
      // ----- Canvasを作成
      _myCanvas = new GsiMapDrawer(DISPLAY_MAPSIZE, DISPLAY_MAPSIZE);
  }

  void drawScreen(TinyGPSPlus &gps, SensorDataHolder *dataHolder)
  {
    try
    {
      // ----- タイトル表示
      M5.Display.setCursor(0,0);
      M5.Display.setTextSize(1);
      M5.Display.setFont(&fonts::efontJA_16_b);
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Display.printf("## 地図 ##\r\n");
     
      // ----- 操作領域にマーキングを施す
      M5.Display.setTextSize(1);
      M5.Display.setFont(&fonts::efontJA_14_b);
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Display.setCursor(308,0);
      M5.Display.printf("Z");
      M5.Display.drawRect(304,0, 15, 15, TFT_WHITE);

      // ディスプレイに表示する左上座標を与えて地図表示
      int displayX = 105;
      int displayY = 25;
      _myCanvas->loadGsiMap(_zoomLevel, _tileX, _tileY, _posX, _posY, displayX, displayY);

      // 地図移動用のマーカー
      M5.Display.fillTriangle(205,  10, 193,  20, 217,  20, TFT_GREEN);  // 上
      M5.Display.fillTriangle(205, 239, 193, 229, 217, 229, TFT_GREEN);  // 下
      M5.Display.fillTriangle( 90, 125, 100, 113, 100, 137, TFT_GREEN);  // 左
      M5.Display.fillTriangle(319, 125, 309, 113, 309, 137, TFT_GREEN);  // 右

      // ----- 情報表示（左側）
      M5.Display.setCursor(0,25);
      M5.Display.setTextSize(1);
      M5.Display.setFont(&fonts::efontJA_14);
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
      displayCurrentJstDateOnly("", dataHolder->getTimeInfo()); // 年月日

      M5.Display.setCursor(0,44);
      M5.Display.setTextSize(1);
      M5.Display.setFont(&fonts::efontJA_14);
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
      displayCurrentJstTimeOnly("", dataHolder->getTimeInfo());  // 時分秒の表示

      // ----- 現在位置ボタンぽいやつ
      M5.Display.setCursor(5,142);
      M5.Display.setFont(&fonts::efontJA_14);
      M5.Display.printf("現在位置\r\n");
      M5.Display.drawRect(1, 140, 66, 18, TFT_WHITE);       

      // ----- 情報表示（左下）
      M5.Display.setCursor(0,205);
      M5.Display.setFont(&fonts::efontJA_14);
      M5.Display.printf("Zoom: %2d\r\n", _zoomLevel);

      M5.Display.setCursor(0,220);
      M5.Display.setFont(&fonts::efontJA_14);
      M5.Display.printf("電池: %02d%%\r\n", dataHolder->getBatteryLevel());
    }
    catch (...)
    {
      Serial.println("Picture Load Exception...");
    }
  }

  void touchedPosition(int posX, int posY)
  {
    // ----- 画面タッチ操作（地図表示モード） -----
    bool vibration = false;
    
    // ---- 左上(題名表示部分)が押されたとき
    if ((posX < 100)&&(posY < 50))
    {
      // ----- 現在位置表示モードに切り替える
      Serial.println("Change map Mode");
      showDisplayMode = SHOW_GSI_MAP;
      needClearScreen = true;
      vibration = true;
    }
    // ---- 「現在位置」が押されたとき
    else if ((posX < 80)&&(posY > 120)&&(posY < 210))
    {
      // ----- 現在位置(または初期位置)へ移動
      Serial.println("Move Current Location");
      
      double lat = gps.location.lat();
      double lng = gps.location.lng();
      if ((lat <= 0.0d)||(lng <= 0.0d))
      {
        // ----- 初期位置が異常データだった場合は(東京駅で)初期化
        lat = 35.68114d;
        lng = 139.767061d;
      }

      // ----- 画像タイル座標と画像内の座標をGPS緯度経度から決定
      GsiTileCoordinate tile(lat,  lng, _zoomLevel);
      _tileX = tile.getTileX();
      _tileY = tile.getTileY();
      _posX = tile.getPixelPosX();
      _posY = tile.getPixelPosY();

      vibration = true;
    }
    // ---- 上エリアが押されたとき
    else if ((posX < 260)&&(posX > 120)&&(posY < 60))
    {
      // ----- 位置を少し上にする
      Serial.println("Move Up");
      _posY = _posY - 80;
      if (_posY < 0)
      {
        _posY = _posY + _GsiMapSize;
        _tileY--;
      }

      vibration = true;
    }
    // ---- 下エリアが押されたとき
    else if ((posX < 260)&&(posX > 120)&&(posY > 200))
    {
      // ----- 位置を少し下にする
      Serial.println("Move Down");
      _posY = _posY + 80;
      if (_posY >= _GsiMapSize)
      {
        _posY = _posY - _GsiMapSize;
        _tileY++;
      }

      vibration = true;
    }
    // ---- 左エリアが押されたとき
    else if ((posX < 160)&&(posX > 90)&&(posY > 80)&&(posY < 180))
    {
      // ----- 位置を少し左にする
      Serial.println("Move Left");
      _posX = _posX - 80;
      if (_posX < 0)
      {
        _posX = _posX + _GsiMapSize;
        _tileX--;
      }

      vibration = true;
    }
    // ---- 右エリアが押されたとき
    else if ((posX > 280)&&(posY > 80)&&(posY < 180))
    {
      // ----- 位置を少し右にする
      Serial.println("Move Right");
      _posX = _posX + 80;
      if (_posX >= _GsiMapSize)
      {
        _posX = _posX - _GsiMapSize;
        _tileX++;
      }

      vibration = true;
    }
    // ===== 右上の場所を押したときの処理
    else if ((posX > 280)&&(posY < 60))
    {
      // ----- 画像タイル座標と画像内の座標をGPS緯度経度から決定
      GsiTileCoordinate orgTile(_tileX,  _tileY, _posX, _posY, _zoomLevel);
      double lat = orgTile.getLatitude();
      double lng = orgTile.getLongitude();

      // ----- ズームレベルを変更する
      //  (タイル座標をズームレベルに合わせて変更する必要あり)
      _zoomLevel = getNextZoomLevel(_zoomLevel);
      Serial.print("next zoom level ");
      Serial.println(_zoomLevel);

      // 変更されたズームレベルに合わせて、タイル位置を更新する (位置が変わる...)
      GsiTileCoordinate nextTile(lat, lng, _zoomLevel);
      _tileX = nextTile.getTileX();
      _tileY = nextTile.getTileY();
      _posX = nextTile.getPixelPosX();
      _posY = nextTile.getPixelPosY();

      needClearScreen = true;
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
