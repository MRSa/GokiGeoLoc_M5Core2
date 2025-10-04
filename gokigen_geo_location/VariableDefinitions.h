
// GNSSモジュール(シリアルポート)からデータを受信した時のステータス管理用
int parseMode = PARSEMODE_WAIT_START;
bool isHandledMessage = false;

// ----- 初回の時刻反映を実行したかどうか
bool isDateTimeApplied = false;

// ----- SDカードに入っている、ズームレベルの地図情報
bool storedZoomLevelList[MAX_ZOOM_COUNT];
char *dirNameIndex[MAX_ZOOM_COUNT];
char dirNameBuffer[DIR_NAME_BUFFER_SIZE];
int nofZoomLevel = 0;

// ----- 画面表示モード
int showDisplayMode = SHOW_GSI_MAP;
bool needClearScreen = false;
bool isScreenModeChanging = false;

// ----- 液晶パネルの輝度制御用
const int brightness_list[] = {255, 128, 64, 32, 16};
int i_brightness = 3;
int max_brightnessIndex = 4;

// ----- 画面上のビジーマーク表示用
uint8_t busyMarkerCount = 0;

// ----- PCからのシリアル通信で受信した文字を格納するStringオブジェクト
String incomingStringFromPC = "";
// シリアル通信でコマンド受信が完了したかを示すフラグ
bool stringCompleteFromPC = false;
