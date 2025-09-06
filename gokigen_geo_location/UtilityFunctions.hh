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

int scanI2CAddresses()
{
  Serial.println("Scanning I2C ...");
  int nDevices = 0;
	for(int address = 1; address < 127; address++ ) 
	{
		Wire.beginTransmission(address);
		int error = Wire.endTransmission();
		if (error == 0)
		{
			Serial.print("I2C device found at address 0x");
			if (address < 16)
      {
				Serial.print("0");
      }
			Serial.print(address,HEX);
			Serial.println("	!");
			nDevices++;
		}
		else if (error == 4) 
		{
			Serial.print("Unknown error at address 0x");
			if (address < 16)
      { 
				Serial.print("0");
      }
			Serial.println(address,HEX);
		}		
	}
	if (nDevices == 0)
  {
		Serial.println("No I2C devices found.\n");
  }
	else
	{
		Serial.print("done   ");
		Serial.print("nDevices = ");
		Serial.println(nDevices);
	}
	return nDevices;
}
