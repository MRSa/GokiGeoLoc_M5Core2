#include <M5GFX.h>

class GsiMapDrawer
{
private:
  M5Canvas _drawCanvas;
  
  int _GsiMapSize = GSI_MAP_PIXEL;
  int _displaySizeX = 0;
  int _displaySizeY = 0;

/*
  M5Canvas bufferCanvas;
  int currentTile1X = 0;
  int currentTile1Y = 0;
  int currentTile2X = 0;
  int currentTile2Y = 0;
  int currentTile3X = 0;
  int currentTile3Y = 0;
  int currentTile4X = 0;
  int currentTile4Y = 0;
*/

  void _loadPictureSingleTile(int zoom, int tileX, int tileY, int topLeftX, int topLeftY)
  {
    // ---- 表示する sizeX, sizeX の画像を作り出す（最大４枚のタイルを毎回読み出す処理）

    //---- ロードするタイルのファイル名を生成する
    char formString[] = "%s/%d/%d/%d.png";
    char tile1[64];
    char tile2[64];
    char tile3[64];
    char tile4[64];
    sprintf(tile1, formString, DIRNAME_GSI_MAP_ROOT, zoom, tileX, tileY);
    sprintf(tile2, formString, DIRNAME_GSI_MAP_ROOT, zoom, tileX + 1, tileY);
    sprintf(tile3, formString, DIRNAME_GSI_MAP_ROOT, zoom, tileX, tileY + 1);
    sprintf(tile4, formString, DIRNAME_GSI_MAP_ROOT, zoom, tileX + 1, tileY + 1);

    // 左上のデータをロードする
    _drawCanvas.drawPngFile(SD, tile1, 0, 0, _GsiMapSize, _GsiMapSize, topLeftX, topLeftY);
    if (topLeftX + _displaySizeX > _GsiMapSize)
    {
      // 右側にはみ出しているので、はみ出したデータ部分をロードする
      _drawCanvas.drawPngFile(SD, tile2, (_GsiMapSize - topLeftX), 0, _GsiMapSize, _GsiMapSize, 0, topLeftY);

      if (topLeftY + _displaySizeY > _GsiMapSize)
      {
        // 右の下側にもはみ出しているので、下側のはみ出したデータ部分をロードする
        _drawCanvas.drawPngFile(SD, tile4, (_GsiMapSize - topLeftX), (_GsiMapSize - topLeftY), _GsiMapSize, _GsiMapSize, 0, 0);
      }
    }
    if (topLeftY + _displaySizeY > _GsiMapSize)
    {
      // 下側にもはみ出しているので、下側のはみ出したデータ部分をロードする
      _drawCanvas.drawPngFile(SD, tile3, 0, (_GsiMapSize - topLeftY), _GsiMapSize, _GsiMapSize, topLeftX, 0);
    }
  }

/*
  void _loadPictureCachedTile(int zoom, int tileX, int tileY, int topLeftX, int topLeftY)
  {
    // ---- ４枚のタイルを読み出し、キャッシュする処理
    //  (キャッシュしたそキャンバスから、必要なエリアだけ抽出して描画するため、、そのうち使いたい)
    bool result = false;

    //---- ロードするタイルのファイル名を生成する
    char formErr[] = "%s/none.png";
    char formString[] = "%s/%d/%d/%d.png";
    char errTile[64];
    char tile1[64];
    char tile2[64];
    char tile3[64];
    char tile4[64];
    sprintf(errTile, formErr, DIRNAME_GSI_MAP_ROOT);
    sprintf(tile1, formString, DIRNAME_GSI_MAP_ROOT, zoom, tileX, tileY);
    sprintf(tile2, formString, DIRNAME_GSI_MAP_ROOT, zoom, tileX + 1, tileY);
    sprintf(tile3, formString, DIRNAME_GSI_MAP_ROOT, zoom, tileX, tileY + 1);
    sprintf(tile4, formString, DIRNAME_GSI_MAP_ROOT, zoom, tileX + 1, tileY + 1);

    // ----- データをロードする（4枚のタイルを _GsiMapSizex2 x _GsiMapSizex2 のキャンバスに展開する）
    if ((currentTile1X != tileX)||(currentTile1Y != tileY))
    {
        result = bufferCanvas.drawPngFile(SD, tile1, 0, 0, _GsiMapSize, _GsiMapSize, 0, 0);
        if (!result)
        {
          bufferCanvas.drawPngFile(SD, errTile, 0, 0, _GsiMapSize, _GsiMapSize, 0, 0);
          //Serial.println("ERR>tile1: load Failure.");
          currentTile1X = -1;
          currentTile1Y = -1;
        }
        else
        {
          currentTile1X = tileX;
          currentTile1Y = tileY;
        }
    }
    if ((currentTile2X != (tileX + 1))||(currentTile2Y != tileY))
    {
      result = bufferCanvas.drawPngFile(SD, tile2, 0, 0, _GsiMapSize, _GsiMapSize, _GsiMapSize, 0);
      if (!result)
      {
        bufferCanvas.drawPngFile(SD, errTile, 0, 0, _GsiMapSize, _GsiMapSize, _GsiMapSize, 0);
        //Serial.println("ERR>tile2: load Failure.");
        currentTile2X = -1;
        currentTile2Y = -1;
      }
      else
      {
        currentTile2X = tileX + 1;
        currentTile2Y = tileY;
      }
    }
    if ((currentTile3X != (tileX))||(currentTile3Y != (tileY + 1)))
    {
      result = bufferCanvas.drawPngFile(SD, tile3, 0, 0, _GsiMapSize, _GsiMapSize, 0, _GsiMapSize);
      if (!result)
      {
        bufferCanvas.drawPngFile(SD, errTile, 0, 0, _GsiMapSize, _GsiMapSize, 0, _GsiMapSize);
        //Serial.println("ERR>tile3: load Failure.");
        currentTile3X = -1;
        currentTile3Y = -1;
      }
      else
      {
        currentTile3X = tileX;
        currentTile3Y = tileY + 1;
      }
    }
    if ((currentTile4X != (tileX + 1))||(currentTile4Y != (tileY + 1)))
    {
      result = bufferCanvas.drawPngFile(SD, tile4, 0, 0, _GsiMapSize, _GsiMapSize, _GsiMapSize, _GsiMapSize);
      if (!result)
      {
        bufferCanvas.drawPngFile(SD, tile4, 0, 0, _GsiMapSize, _GsiMapSize, _GsiMapSize, _GsiMapSize);
        //Serial.println("ERR>tile4: load Failure.");
        currentTile4X = -1;
        currentTile4Y = -1;
      }
      else
      {
        currentTile4X = tileX + 1;
        currentTile4Y = tileY + 1;
      }
    }
  }
*/

  void _drawGsiMap(int targetX, int targetY, int centerX, int centerY)
  {

    M5.Display.startWrite();

    // 地図を表示する場所に「白い枠」を表示
    M5.Display.drawRect((targetX - 1), (targetY - 1), (_displaySizeX + 2), (_displaySizeY + 2), TFT_WHITE);

    // ----- 描画領域を画面に転送
    _drawCanvas.pushSprite(&M5.Display, targetX, targetY);

    // ----- 検出場所にマークを付ける
    _drawCanvas.fillCircle(centerX, centerY, 5, TFT_VIOLET);

    M5.Display.endWrite();
  }

public:
  GsiMapDrawer(int displaySizeX, int displaySizeY, int mapSize = 256)
  {    
    //bufferCanvas.createSprite((mapSize * 2), (mapSize * 2));
    //bufferCanvas.fillSprite(TFT_BLACK);

    _drawCanvas.createSprite(displaySizeX, displaySizeY);
    _drawCanvas.fillSprite(TFT_LIGHTGREY);
    _GsiMapSize = mapSize;
    _displaySizeX = displaySizeX;
    _displaySizeY = displaySizeY;
  }

  void loadGsiMap(int zoom, int orgTileX, int orgTileY, int orgPosX, int orgPosY, int targetX, int targetY)
  {

    // 読み出し位置の調整（左上座標が中心座標よりも小さいタイルの場合の調整）
    int tileX = orgTileX;
    int tileY = orgTileY;
    int topLeftX = orgPosX - _displaySizeX / 2;
    int topLeftY = orgPosY - _displaySizeY / 2;

    if (topLeftX < 0)
    {
      // ---- X軸の調整
      tileX--;
      topLeftX = topLeftX + _GsiMapSize;
    }
    if (topLeftY < 0)
    {
      // ---- Y軸の調整
      tileY--;
      topLeftY = topLeftY + _GsiMapSize;
    }

    // Serial.println("--- Load START ---");
    // loadPictureCachedTile(zoom, tileX, tileY, topLeftX, topLeftY); // そのうちこっちでやりたい
    _loadPictureSingleTile(zoom, tileX, tileY, topLeftX, topLeftY);
    // Serial.println("--- Load FINISH ---");

    // 地図を画面に描画する
    _drawGsiMap(targetX, targetY, _displaySizeX / 2, _displaySizeY / 2);
  }

};
