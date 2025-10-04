#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <TinyGPSPlus.h>
#include <M5Unified.h>

// ----- 関数のプロトタイプ宣言 (実体は、UtilityFunctions.hh に定義)
void changeDisplayBrightness();  // 画面の輝度を変更
int getDisplayBrightness();      // 現在の画面の輝度を知る
int getNextZoomLevel(int zoomLevel);   // 地図のズームレベルを変更する
void makeVibration(int strength, int delayTime);   // バイブレーションを実行
void displayCurrentJstTime(char *header, struct tm *timeinfo);  // 現在時刻を画面表示
void displayCurrentJstDateOnly(char *header, struct tm *gmt_timeinfo);
void displayCurrentJstTimeOnly(char *header, struct tm *gmt_timeinfo);

void drawBusyMarker();  // 画面右下に動作中マーカーを表示する
void applyDateTime();   // GPSから受信した時刻をシステムに設定する

// ----- GPSのメッセージ処理用
TinyGPSPlus gps;

#include "ConstantDefinitions.h"
#include "VariableDefinitions.h"
#include "CalculateMD5Hash.hpp"

#include "MyBmp280Sensor.hpp"
#include "GsiTileCoordinate.hpp"
#include "GsiMapDrawer.hpp"
#include "SDcardHandler.hpp"
#include "UbxMessageParser.hpp"
#include "SensorDataHolder.hpp"
#include "TouchPositionHandler.hpp"
#include "ShowGSIMap.hpp"
#include "ShowAnyGSIMap.hpp"
#include "ShowDCIS.hpp"
#include "ShowDetailInfo.hpp"
#include "SendReceivedMessage.hpp"

// ----- 圧力と温度のセンサ (BMP280 / I2C)
MyBmp280Sensor bmp280(M5.In_I2C);

// ----- いろいろな内部クラス
SDcardHandler *cardHandler = NULL;
UbxMessageParser *ubxMessageParser = NULL;
SendReceivedMessage *sendReceivedMessage = NULL;

ShowGSIMap *gsiMapDrawer = NULL;
ShowAnyGSIMap *anyGsiMapDrawer = NULL;
ShowDCIS *dicsDrawer = NULL;
ShowDetailInfo *detailDrawer = NULL;
SensorDataHolder *sensorDataHolder = NULL;
TouchPositionHandler *touchPositionHandler = NULL;

// ----- ユーティリティ関数群の定義
#include "UtilityFunctions.hh"

void setup()
{
  // ----- M5 Unified の初期化処理
  auto cfg = M5.config();
  cfg.serial_baudrate = SERIAL_BAUDRATE_PC;
  cfg.internal_imu = false;  // external -> internal の順に初期化したい場合はfalseにする
  cfg.external_imu = true;
  M5.begin(cfg);

  // ----- Display
  M5.Display.init();
  M5.Display.setRotation(1);
  M5.Display.setBrightness(brightness_list[i_brightness]);
  M5.Display.setTextSize(2);
  M5.Display.println("Initializing");

  // ----- PCとのシリアル通信
  Serial.begin(SERIAL_BAUDRATE_PC);
  Serial.println("Initializing...\n");

  // ----- Battery
  M5.Power.begin();

  // ----- IMU
  M5.Imu.loadOffsetFromNVS();
  // M5 IMU PRO( https://docs.m5stack.com/ja/unit/IMU%20Pro%20Mini%20Unit )を増設した場合
  // 使用する場合、Unit GNSSの動作に合わせて、"M5" の刻印面を液晶の向きに合わせる
  auto imuResult = M5.Imu.begin(&M5.Ex_I2C);
  if (!imuResult)
  {
    imuResult = M5.Imu.begin(&M5.In_I2C);  // Unit GNSS のセンサを利用する場合
  }
  else
  {
    Serial.println("Use External IMU.");
  }
  if (imuResult)
  {
    auto imuType = M5.Imu.getType(); 
    Serial.print("IMU is ready. : ");
    Serial.println(imuType, HEX); // m5::imu_t::imu_bmi270 == 6
  }
  else
  {
    Serial.println("  IMU is disabled...");
  }

  // ----- BMP280 : Pressure / Temperature sensor
  if (!bmp280.begin())
  {
    Serial.println("  BMP280 start Failure...");
  }
  else
  {
    Serial.println("BMP280 is ready.");
  }

  // ----- SDカードの初期化
  cardHandler = new SDcardHandler();
  if (!cardHandler->isCardReady())
  {
    // Print a message if SD card initialization failed or if the SD card does not exist.
    M5.Display.print("\n SD card not detected\n");
    Serial.println("  SD card not detected");
  }
  else
  {
    M5.Display.print("\nSD card detected\n");
    Serial.println("SD card detected");
  }

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
    nofZoomLevel = cardHandler->listDirectory(DIRNAME_GSI_MAP_ROOT, dirNameIndex, dirNameBuffer, MAX_ZOOM_COUNT, DIR_NAME_BUFFER_SIZE);
    for (int index = 0; index < nofZoomLevel; index++)
    {
      int levelIndex = String(dirNameIndex[index]).toInt();
      storedZoomLevelList[levelIndex] = true;
    }
    Serial.print("Zoom level: ");
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
    Serial.println("  The SD card can not access.");
  }

  // ----- センサデータ保持クラスの準備 
  sensorDataHolder = new SensorDataHolder(bmp280);

  // ----- タッチ位置を記憶するクラスの準備
  touchPositionHandler = new TouchPositionHandler();

  // ----- データを送信するクラスの準備
  sendReceivedMessage = new SendReceivedMessage(sensorDataHolder, ubxMessageParser);

  // ----- 画面描画クラスの準備
  gsiMapDrawer = new ShowGSIMap();
  dicsDrawer = new ShowDCIS();
  detailDrawer = new ShowDetailInfo();
  anyGsiMapDrawer = new ShowAnyGSIMap();

  //----- setup() が終了したことを画面とシリアルに通知する
  delay(WAIT_DUR); // 少し待つ

  //  シリアルで通知
  Serial.println("\n- - - - - - ");
  Serial.println("Initialization finished");

  // 画面に表示
  M5.Display.clear();
  M5.Display.setCursor(0,0);
  M5.Display.println("Initialization finished");
  M5.Display.setTextSize(1);
  M5.Display.setFont(&fonts::efontJA_14);
  M5.Display.setCursor(205,222);
  M5.Display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  M5.Display.println("GOKIGEN Project");    
  needClearScreen = true;
  Serial.println("- - - - - - \n");

  delay(1600); // 少し待つ
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

  // シリアルデータのチェックと読み取りを非同期で行う
  checkSerialInput();
  
  // シリアルからのコマンド受信が完了していたら処理を行う
  if (stringCompleteFromPC)
  {
    // 受信したコマンド文字列を処理する
    sendReceivedMessage->checkReceivedString(Serial, incomingStringFromPC);
    
    // コマンド文字列の処理後、フラグと文字列をリセット
    incomingStringFromPC = "";
    stringCompleteFromPC = false;
    Serial.println("");
  }

/*  
  if (Serial.available() > 0)
  {
    // --- PCサイドからのコマンド受付コード 0x0a まで読み出す
    String readString = Serial.readStringUntil(0x0a);
    if (sendReceivedMessage->checkReceivedString(Serial, readString))
    {
      // ----- PCからのコマンドを受け付けた場合
      Serial.println("");
      Serial.println("");
      // ----- そのまま継続
      //return;
    }
  }
*/

// ----- シリアルバッファにデータがある限り、全てのバイトを処理する
  //  (メッセージが受信できた時は抜けて画面更新処理などを実行する)
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

  // ----- タッチパネルの操作
  if (touchPositionHandler->isPressed())
  {
    //----- タッチパネルが押されたことを検出
    int touchPosX = touchPositionHandler->getTouchX();
    int touchPosY = touchPositionHandler->getTouchY();

    switch (showDisplayMode)
    {
      case SHOW_DCIS:
        // 災危通報表示モード
        dicsDrawer->touchedPosition(touchPosX, touchPosY, ubxMessageParser);
        break;
      case SHOW_DETAIL:
        // 詳細(文字表示)モード
        detailDrawer->touchedPosition(touchPosX, touchPosY);
        break;
      case SHOW_GSIMAP_ANY:
        // 地理院地図表示モード (移動可)
        anyGsiMapDrawer->touchedPosition(touchPosX, touchPosY);
        break;
      case SHOW_GSI_MAP:
      default:
        // 地理院地図表示モード
        gsiMapDrawer->touchedPosition(touchPosX, touchPosY);
        break;
    }
    // ---- 開放する
    touchPositionHandler->resetPosition();
  }

  // ----- 画面更新が必要なときのみ画面を更新する
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
        dicsDrawer->drawScreen(ubxMessageParser);
        break;
      case SHOW_DETAIL:
        // 詳細(文字表示)モード
        detailDrawer->drawScreen(gps, sensorDataHolder);
        break;
      case SHOW_GSIMAP_ANY:
        // 地理院地図表示モード (移動可)
        anyGsiMapDrawer->drawScreen(gps, sensorDataHolder);
        break;
      case SHOW_GSI_MAP:
      default:
        // 地理院地図表示モード
        gsiMapDrawer->drawScreen(gps, sensorDataHolder);
        break;
    }

    // ----- 画面更新用のフラグを落とす
    isScreenModeChanging = false;
    isHandledMessage = false;

    // ----- メッセージを受信していることを示すマークを表示する
    drawBusyMarker();
  }
}
