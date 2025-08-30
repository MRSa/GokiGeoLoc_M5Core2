// ------------------------------
//   定数等の定義
// ------------------------------

// ----- USBシリアル通信
#define SERIAL_BAUDRATE_PC 115200

// ----- 画面レイアウト関連
#define DISPLAY_MAPSIZE 200

// ----- I2C センサのアドレス
#define BIM270_SENSOR_ADDR 0x68
#define BMP280_SENSOR_ADDR 0x76

// ----- GPS(GNSS Module)関連
#define SERIAL_BAUDRATE_GPS 38400
#define UBLOX_SERIAL Serial2

// GNSSモジュール(シリアルポート)からデータを受信した時のステータス管理用
#define PARSEMODE_WAIT_START 0
#define PARSEMODE_WAIT_UBX 1
#define PARSEMODE_READ_UBX 2
#define PARSEMODE_READ_NMEA 3
#define PARSEMODE_NMEA_WAIT_LF 4


// 画面表示モード
#define SHOW_GSI_MAP 0 // 地理院地図表示モード
#define SHOW_DCIS    1 // 災危通報表示モード
#define SHOW_DETAIL  2 // 詳細(文字表示)モード
