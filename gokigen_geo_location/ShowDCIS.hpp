class ShowDCIS
{
  // ----- 災危通報の情報を表示する
private:


public:
  ShowDCIS()
  {

  }

  void drawScreen()
  {
    // M5.Display.clear();  // ちらつくので実行しない
    M5.Display.setCursor(0,0);
    M5.Display.setTextSize(2);
    M5.Display.setFont(&fonts::efontJA_16_b);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.printf("### 災危通報 ###\r\n");
  }
};
