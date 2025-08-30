#include <cmath>

class GsiTileCoordinate
{
private:
    int tileX;  // タイルのX座標
    int tileY;  // タイルのY座標
    int posX;   // タイル内のピクセル位置 (X座標)
    int posY;   // タイル内のピクセル位置 (Y座標)

public:
    GsiTileCoordinate(double latitude, double longitude, int zoomLevel)
    {
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
        tileX = static_cast<int>(floor(normalizedX * n));
        tileY = static_cast<int>(floor(normalizedY * n));

        // タイル内のピクセル座標の計算（タイルサイズは256ピクセルを想定）
        int tilePixelSize = 256;
        posX = static_cast<int>(floor((normalizedX * n - tileX) * tilePixelSize));
        posY = static_cast<int>(floor((normalizedY * n - tileY) * tilePixelSize));
    }

    // ----- 地理院タイルの位置を応答(getTileX(), getTileY())
    int getTileX() { return tileX; };
    int getTileY() { return tileY; };

    // ----- 地理院タイル内の位置（ピクセル）を応答(getPixelPosX(), getPixelPosY())
    int getPixelPosX()  { return posX; };
    int getPixelPosY()  { return posY; };

};
