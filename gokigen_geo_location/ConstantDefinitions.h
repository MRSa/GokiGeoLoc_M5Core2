// ------------------------------
//   定数等の定義
// ------------------------------

// ----- USBシリアル通信
#define SERIAL_BAUDRATE_PC 115200
#define SERIAL_CMD_MSGLENGTH_MAX 256

// ----- 画面レイアウト関連
#define DISPLAY_MAPSIZE 200

// ----- I2C センサのアドレス
#define BMI270_SENSOR_ADDR 0x69
#define BMP280_SENSOR_ADDR 0x76

#define I2C_COMMUNICATION_SPEED 400000

// BMP280のチップID
#define BMP280_CHIPID 0x58

// BMP280のレジスタアドレス
#define BMP280_CHIP_ID_REG 0xD0
#define BMP280_CALIB_REG_START 0x88
#define BMP280_CTRL_MEAS_REG 0xF4
#define BMP280_PRESS_MSB_REG 0xF7
#define BMP280_TEMP_MSB_REG 0xFA

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
#define SHOW_GSI_MAP 0     // 地理院地図表示モード
#define SHOW_DCIS    1    // 災危通報表示モード
#define SHOW_DETAIL  2    // 詳細(文字表示)モード
#define SHOW_GSIMAP_ANY 3 // 地理院地図表示モード（移動可）

// バイブレーション
#define VIBRATION_WEAK    80
#define VIBRATION_MIDDLE  120
#define VIBRATION_STRONG  250

#define VIBRATION_TIME_LONG 500
#define VIBRATION_TIME_MIDDLE 250
#define VIBRATION_TIME_SHORT 100

// SDカードの地図格納フォルダ
#define MAX_ZOOM_COUNT  21
#define DIR_NAME_BUFFER_SIZE  300
#define DIRNAME_GSI_MAP_ROOT  "/GsiMap"

// 待ち時間 (ms)
#define WAIT_DUR 300

#define GSI_MAP_PIXEL 256
