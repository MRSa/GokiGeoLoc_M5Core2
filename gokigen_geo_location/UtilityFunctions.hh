////////////////////////////////
// UtilityFunctions.hh
////////////////////////////////

void changeDisplayBrightness()
{
  i_brightness--;
  if (i_brightness < 0)
  {
    i_brightness = max_brightnessIndex;
  }
  M5.Display.setBrightness(brightness_list[i_brightness]);
}

int getDisplayBrightness()
{
  return brightness_list[i_brightness];
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

void displayCurrentJstTime(char *header, struct tm *gmt_timeinfo)
{
  struct tm jsttime;  // JSTの保管場所
   
  // GMTからJSTに変換するため、9時間（32400秒）を加算
  time_t rawtime = mktime(gmt_timeinfo);
  rawtime += 9 * 60 * 60; // 9時間 * 60分 * 60秒 = 32400秒
  struct tm *timeinfo = gmtime(&rawtime);
  memcpy(&jsttime, timeinfo, sizeof(struct tm));

  // ----- 現在時刻を応答する
  M5.Display.printf("%s%04d-%02d-%02d %02d:%02d:%02d\r\n"
    ,header
    ,jsttime.tm_year + 1900
    ,jsttime.tm_mon + 1
    ,jsttime.tm_mday
    ,jsttime.tm_hour
    ,jsttime.tm_min
    ,jsttime.tm_sec
  );
}

void displayCurrentJstDateOnly(char *header, struct tm *gmt_timeinfo)
{
  struct tm jsttime;  // JSTの保管場所
   
  // GMTからJSTに変換するため、9時間（32400秒）を加算
  time_t rawtime = mktime(gmt_timeinfo);
  rawtime += 9 * 60 * 60; // 9時間 * 60分 * 60秒 = 32400秒
  struct tm *timeinfo = gmtime(&rawtime);
  memcpy(&jsttime, timeinfo, sizeof(struct tm));

  // ----- 現在時刻だけを応答する
  M5.Display.printf("%s%04d-%02d-%02d\r\n"
    ,header
    ,jsttime.tm_year + 1900
    ,jsttime.tm_mon + 1
    ,jsttime.tm_mday
  );
}

void displayCurrentJstTimeOnly(char *header, struct tm *gmt_timeinfo)
{
  struct tm jsttime;  // JSTの保管場所
   
  // GMTからJSTに変換するため、9時間（32400秒）を加算
  time_t rawtime = mktime(gmt_timeinfo);
  rawtime += 9 * 60 * 60; // 9時間 * 60分 * 60秒 = 32400秒
  struct tm *timeinfo = gmtime(&rawtime);
  memcpy(&jsttime, timeinfo, sizeof(struct tm));

  // ----- 現在時刻だけを応答する
  M5.Display.printf("%s%02d:%02d:%02d\r\n"
    ,header
    ,jsttime.tm_hour
    ,jsttime.tm_min
    ,jsttime.tm_sec
  );
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

// 非同期でシリアル入力の1バイトを読み取る関数
void checkSerialInput()
{
  while (Serial.available())
  {
    // 新しいバイトを読み込む
    char inChar = (char)Serial.read();

    // 終端文字0x0a (LF)を検出した場合
    if (inChar == 0x0a)
    {
      // 処理対象の文字列として確定
      stringCompleteFromPC = true;
    } 
    // キャリッジリターン0x0d (CR)は無視する
    else if (inChar == 0x0d)
    {
      // do nothing
    }
    else
    if (incomingStringFromPC.length() >= SERIAL_CMD_MSGLENGTH_MAX)
    {
      // メッセージ長が長すぎるので、ここでいったん区切る
      // (処理対象の文字列として確定させる)
      stringCompleteFromPC = true;
      // 注意: このinCharはincomingStringに追加されない
    }
    // それ以外の文字を文字列に追加
    else
    {
      incomingStringFromPC += inChar;
    }
  }
}