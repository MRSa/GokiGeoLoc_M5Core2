#include <cmath>

class GsiTileCoordinate
{
private:
    int _tileX;  // タイルのX座標
    int _tileY;  // タイルのY座標
    int _posX;   // タイル内のピクセル位置 (X座標)
    int _posY;   // タイル内のピクセル位置 (Y座標)

    int _zoomLevel;

    double _latitude;
    double _longitude;

    const int _tilePixelSize = 256; // タイルサイズは256ピクセルを想定

public:
    GsiTileCoordinate(double latitude, double longitude, int zoomLevel)
    {
        // 緯度経度、ズームレベルを記憶
        _latitude = latitude;
        _longitude = longitude;
        _zoomLevel = zoomLevel;

        // 緯度経度をラジアンに変換
        double lat_rad = latitude * M_PI / 180.0;
        double lon_rad = longitude * M_PI / 180.0;

        // 国土地理院タイル座標計算
        // 参考: https://maps.gsi.go.jp/development/siyou.html#tile

        // 正規化された経度・緯度座標（0.0〜1.0）
        double n = static_cast<double>(1 << zoomLevel);  // ビットシフトで2の累乗を高速かつ正確に計算 : pow(2.0, zoomLevel) と等価
        double normalizedX = (lon_rad + M_PI) / (2 * M_PI);
        double normalizedY = (1 - log(tan(lat_rad) + (1 / cos(lat_rad))) / M_PI) / 2;

        // 国土地理院タイル座標の計算
        _tileX = static_cast<int>(floor(normalizedX * n));
        _tileY = static_cast<int>(floor(normalizedY * n));

        // タイル内のピクセル座標の計算（タイルサイズは256ピクセルを想定）
        _posX = static_cast<int>(floor((normalizedX * n - _tileX) * _tilePixelSize));
        _posY = static_cast<int>(floor((normalizedY * n - _tileY) * _tilePixelSize));
    }


    GsiTileCoordinate(int tileX, int tileY, int posX, int posY, int zoomLevel)
    {
        // 引数をメンバー変数に格納
        _tileX = tileX;
        _tileY = tileY;
        _posX = posX;
        _posY = posY;
        _zoomLevel = zoomLevel;
        
        // タイル座標とピクセル位置から正規化された座標を計算
        double n = static_cast<double>(1 << zoomLevel);
        double normalizedX = (static_cast<double>(tileX) + static_cast<double>(posX) / _tilePixelSize) / n;
        double normalizedY = (static_cast<double>(tileY) + static_cast<double>(posY) / _tilePixelSize) / n;
        
        // 逆メルカトル図法で緯度経度を計算
        _longitude = normalizedX * 360.0 - 180.0;
        double radian_lat = atan(sinh(M_PI * (1 - 2 * normalizedY)));
        _latitude = radian_lat * 180.0 / M_PI;
    }

    // ----- 地理院タイルの位置を応答(getTileX(), getTileY())
    int getTileX() { return _tileX; };
    int getTileY() { return _tileY; };

    // ----- 地理院タイル内の位置（ピクセル）を応答(getPixelPosX(), getPixelPosY())
    int getPixelPosX()  { return _posX; };
    int getPixelPosY()  { return _posY; };

    // ----- 緯度経度情報を応答(getLatitude(), getLongitude())
    double getLatitude() { return _latitude; };
    double getLongitude() { return _longitude; };
};
