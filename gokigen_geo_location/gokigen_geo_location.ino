#include "SD.h"
#include <SPI.h>
#include <TinyGPSPlus.h>
#include <Adafruit_BMP280.h>
#include <M5Unified.h>

// ----- 関数のプロトタイプ宣言
void makeVibration(int strength, int delayTime);
int getNextZoomLevel(int zoomLevel);

#include "GsiTileCoordinate.hpp"
#include "GsiMapDrawer.hpp"
#include "SDcardHandler.hpp"
#include "UbxMessageParser.hpp"
#include "ConstantDefinitions.hpp"
#include "SensorDataHolder.hpp"
#include "TouchPositionHandler.hpp"
#include "ShowGSIMap.hpp"
#include "ShowDCIS.hpp"
#include "ShowDetailInfo.hpp"

// ----- 圧力と温度のセンサ (BMP280 / I2C)
Adafruit_BMP280 bmp(&Wire1);

// ----- GPSのメッセージ処理用
TinyGPSPlus gps;

// GNSSモジュール(シリアルポート)からデータを受信した時のステータス管理用
int parseMode = PARSEMODE_WAIT_START;

// ----- 液晶パネルの輝度制御
const int brightness_list[] = {255, 128, 64, 32, 16};
int i_brightness = 3;

// ----- 初回の時刻反映を実行したかどうか
bool isDateTimeApplied = false;
uint8_t busyMarkerCount = 0;

SDcardHandler *cardHandler = NULL;
UbxMessageParser *ubxMessageParser = NULL;

ShowGSIMap *gsiMapDrawer = NULL;
ShowDCIS *dicsDrawer = NULL;
ShowDetailInfo *detailDrawer = NULL;
SensorDataHolder *sensorDataHolder = NULL;
TouchPositionHandler *touchPositionHandler = NULL;

// ----- SDカードに入っている、ズームレベルの地図情報
#define MAX_ZOOM_COUNT  21
#define DIR_NAME_BUFFER_SIZE  300
bool storedZoomLevelList[MAX_ZOOM_COUNT];
char *dirNameIndex[MAX_ZOOM_COUNT];
char dirNameBuffer[DIR_NAME_BUFFER_SIZE];
int nofZoomLevel = 0;

// 画面表示モード
int showDisplayMode = SHOW_GSI_MAP;
bool needClearScreen = false;
bool isScreenModeChanging = false;

void drawBusyMarker()
{
  // ---- メッセージ受信を示す表示を画面下部(右下端)に
  busyMarkerCount++;
  if (busyMarkerCount > 4)
  {
    busyMarkerCount = 0;
  }

  M5.Display.setCursor(310,228);
  M5.Display.setFont(&fonts::efontJA_10);
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  switch (busyMarkerCount)
  {
    case 3:
      M5.Display.printf("/");
      break;
    case 2:
      M5.Display.printf("|");
      break;
    case 1:
      M5.Display.printf("\\");
      break;

    case 0:
    default:
      M5.Display.printf("-");
      break;
  }
}

int getNextZoomLevel(int zoomLevel)
{
  try
  {
    int index = zoomLevel + 1;
    for (; index < MAX_ZOOM_COUNT; index++)
    {
      if (storedZoomLevelList[index] == true)
      {
        // ---- 次に有効なzoom levelを応答する
        return (index);
      }
    }
    if (index >= MAX_ZOOM_COUNT)
    {
      for (index = 0; index < MAX_ZOOM_COUNT; index++)
      {
        // ----- 最初に有効な zoom levelを応答する
        if (storedZoomLevelList[index] == true)
        {
          return (index);
        }
      }
    }
  }
  catch(...)
  {
    // なにもしない
  }
  return zoomLevel;  // zoom level は変更しない
}

void makeVibration(int strength, int delayTime)
{
  // ----- バイブレーションで操作をフィードバックする
  M5.Power.setVibration(strength);
  delay(delayTime);
  M5.Power.setVibration(0);
}

void applyDateTime()
{
    // ----- GPSから受信した時刻をシステムに設定する処理
    m5::rtc_datetime_t dt;
      
    // Set the date
    dt.date.year = gps.date.year();
    dt.date.month = gps.date.month();
    dt.date.date = gps.date.day();
      
    // Set the time
    dt.time.hours = gps.time.hour();
    dt.time.minutes = gps.time.minute();
    dt.time.seconds = gps.time.second();

    // Set RTC time
    M5.Rtc.setDateTime(dt);
    M5.Rtc.setSystemTimeFromRtc();
}

void setup()
{
  // ----- M5 Unified の初期化処理
  auto cfg = M5.config();
  cfg.serial_baudrate = SERIAL_BAUDRATE_PC;
  M5.begin(cfg);

  // ----- Display
  M5.Display.init();
  M5.Display.setRotation(1);
  M5.Display.setBrightness(brightness_list[i_brightness]);
  M5.Display.setTextSize(2);
  M5.Display.println("Initializing");

  // ----- Battery
  M5.Power.begin();

  // ----- PCとのシリアル通信
  Serial.begin(SERIAL_BAUDRATE_PC);
  Serial.println("Initializing...");

  // ----- SDカードの初期化
  cardHandler = new SDcardHandler();
  if (!cardHandler->isCardReady())
  {
    // Print a message if SD card initialization failed or if the SD card does not exist.
    M5.Display.print("\n SD card not detected\n");
    Serial.print("\n SD card not detected\n");
  }
  else
  {
    M5.Display.print("\n SD card detected\n");
    Serial.print("\n SD card detected\n");
  }

  // ----- BMP280 : Pressure / Temperature sensor
  bmp.begin(BMP280_SENSOR_ADDR);

  // GPSモジュールとの接続 (NEO-M9N)
  UBLOX_SERIAL.begin(SERIAL_BAUDRATE_GPS, SERIAL_8N1, 13, 14);

  // ----- ublox メッセージ解析クラスを作成
  ubxMessageParser = new UbxMessageParser();
  ubxMessageParser->begin();

  // ----- ズームレベルリストの初期化
  for (int index = 0; index < MAX_ZOOM_COUNT; index++)
  {
    storedZoomLevelList[index] = false;
  }
  if (cardHandler->isCardReady())
  {
    // --- ディレクトリ名をチェック(変更可能なズームレベルの確認)
    nofZoomLevel = cardHandler->listDirectory("/GpsTile", dirNameIndex, dirNameBuffer, MAX_ZOOM_COUNT, DIR_NAME_BUFFER_SIZE);
    for (int index = 0; index < nofZoomLevel; index++)
    {
      int levelIndex = String(dirNameIndex[index]).toInt();
      storedZoomLevelList[levelIndex] = true;
    }
    Serial.print("Supported Zoom level: ");
    for (int index = 0; index < MAX_ZOOM_COUNT; index++)
    {
      if (storedZoomLevelList[index] == true)
      {
        Serial.print(index);
        Serial.print(" ");
      }
    }
    Serial.println("");
  }
  else
  {
    Serial.println("The SD card can not access.");
  }

  // ----- センサデータ保持クラスの準備 
  sensorDataHolder = new SensorDataHolder(bmp);

  // ----- タッチ位置を記憶するクラスの準備
  touchPositionHandler = new TouchPositionHandler();

  // ----- 画面描画クラスの準備
  gsiMapDrawer = new ShowGSIMap();
  dicsDrawer = new ShowDCIS();
  detailDrawer = new ShowDetailInfo();

  //----- setup() が終了したことを画面とシリアルに通知する

  //  シリアルで通知
  Serial.println("- - - - - - ");
  Serial.println("  Initialization finished");

  // 画面に表示
  M5.Display.clear();
  M5.Display.setCursor(0,0);
  M5.Display.println("Initialization finished");
  needClearScreen = true;
  Serial.println("- - - - -");

  delay(1500); // 少し待つ
  //M5.Display.clear();
}

void loop()
{
  // ----- ステータス更新
  M5.update();
  sensorDataHolder->updateSensorData();
  if (!isScreenModeChanging)
  {
    if (M5.Touch.isEnabled())
    {
      touchPositionHandler->handleTouchPosition();
    }
    if (M5.BtnA.isPressed())
    {
      // ----- ボタンA: 地図表示モードに切り替え
      isScreenModeChanging = true;
      needClearScreen = true;
      showDisplayMode = SHOW_GSI_MAP;
      Serial.println("BtnA: GSI MAP MODE");
      makeVibration(VIBRATION_WEAK, VIBRATION_TIME_MIDDLE);
    }
    if (M5.BtnB.isPressed())
    {
      // ----- ボタンB: 災危通報表示モードに切り替え
      isScreenModeChanging = true;
      needClearScreen = true;
      showDisplayMode = SHOW_DCIS;
      Serial.println("BtnB: DICS MODE");
      makeVibration(VIBRATION_WEAK, VIBRATION_TIME_MIDDLE);
    }
    if (M5.BtnC.isPressed())
    {
      // ----- ボタンC: 詳細(文字)表示モードに切り替え
      isScreenModeChanging = true;
      needClearScreen = true;
      showDisplayMode = SHOW_DETAIL;
      Serial.println("BtnC: DETAIL MODE");
      makeVibration(VIBRATION_WEAK, VIBRATION_TIME_MIDDLE);
    }
  }

  // ----- シリアルバッファにデータがある限り、全てのバイトを処理する
  //  (メッセージが受信できた時は抜けて画面更新処理などを実行する)
  bool isHandledMessage = false;
  if (UBLOX_SERIAL.available() > 0)
  {
    uint8_t incomingByte = UBLOX_SERIAL.read();
    switch (parseMode)
    {
      case PARSEMODE_WAIT_START:
        if (incomingByte == 0xB5)
        {
          parseMode = PARSEMODE_WAIT_UBX;
        }
        else if (incomingByte == '$')
        {
          parseMode = PARSEMODE_READ_NMEA;
          gps.encode(incomingByte);
        }
        // 他の不明なバイトは無視して次の開始バイトを待つ
        break;

      case PARSEMODE_WAIT_UBX:
        if (incomingByte == 0x62)
        {
          uint8_t dummyByte = 0xB5;
          ubxMessageParser->clear();
          ubxMessageParser->putBuffer(dummyByte);
          ubxMessageParser->putBuffer(incomingByte);
          parseMode = PARSEMODE_READ_UBX;
          //Serial.println("RECEIVED UBX MESSAGE.");
        }
        else
        {
          // 0xB5の次に0x62が来なかった場合は、開始待ちモードに戻る
          parseMode = PARSEMODE_WAIT_START;
        }
        break;
      
      case PARSEMODE_READ_UBX:
        // parseUBX関数に1バイトずつデータを渡し、メッセージが完了したらfalseを返す
        if (!ubxMessageParser->parseUBX(incomingByte))
        {
          parseMode = PARSEMODE_WAIT_START;
          isHandledMessage = true;
        }
        break;

      case PARSEMODE_READ_NMEA:
        // NMEAメッセージを受信したときは、処理をTinyGPSPlusに任せ、改行コード(LF)でメッセージの終わりと判断
        gps.encode(incomingByte);
        if (incomingByte == '\n')
        {
          parseMode = PARSEMODE_WAIT_START;
          isHandledMessage = true;
        }
        break;
      
      default:
        // 受信データが不正(不定)な状態の場合は先頭データの開始待ちに戻る
        parseMode = PARSEMODE_WAIT_START;
        break;
    }
  }

  // ----- メッセージがそろったときのみ画面を更新する
  if (isHandledMessage)
  {
    if (gps.location.isUpdated())
    {
      if ((!isDateTimeApplied)&&(gps.time.isValid()))
      {
        // 時刻設定
        applyDateTime();
        isDateTimeApplied = true; // GPSからの時刻設定は１回のみとする。
      }
    }

    if (needClearScreen)
    {
        // ---- 画面クリアを実施
        M5.Display.clear();
        needClearScreen = false;
    }
    switch (showDisplayMode)
    {
      case SHOW_DCIS:
        // 災危通報表示モード
        dicsDrawer->drawScreen(sensorDataHolder, touchPositionHandler, ubxMessageParser);
        break;
      case SHOW_DETAIL:
        // 詳細(文字表示)モード
        detailDrawer->drawScreen(gps, sensorDataHolder, touchPositionHandler);
        break;
      case SHOW_GSI_MAP:
      default:
        // 地理院地図表示モード
        gsiMapDrawer->drawScreen(gps, sensorDataHolder, touchPositionHandler);
        break;
    }
    isScreenModeChanging = false;

    // ----- メッセージを受信していることを示すマークを表示する
    drawBusyMarker();
  }
}
