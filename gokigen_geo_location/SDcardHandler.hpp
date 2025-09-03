
// ----- SDカード制御用
#define SD_SPI_CS_PIN   4
#define SD_SPI_SCK_PIN  18
#define SD_SPI_MISO_PIN 38
#define SD_SPI_MOSI_PIN 23
#define SPI_SD_SPEED 25000000

class SDcardHandler
{
private:
  bool isSdCardMounted = false;

public:
  SDcardHandler()
  {    
    SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
    isSdCardMounted = SD.begin(SD_SPI_CS_PIN, SPI, SPI_SD_SPEED);
  }

  bool isCardReady() { return isSdCardMounted; }

  int listDirectory(const char *dirname, char *index[], char *buffer, int maxIndexSize, int maxBufferSize)
  {
    int nofIndex = 0;
    int storedBufferSize = 0;
    File root = SD.open(dirname);
    if (!root)
    {
      Serial.println("Failed to open directory");
      return (-1);
    }
    if (!root.isDirectory())
    {
      Serial.println("Not a directory");
      return (-1);
    }
    
    File file = root.openNextFile();
    while (file)
    {
      if (file.isDirectory())
      {
        int charLength = strlen(file.name());
        if (((charLength + 1 + storedBufferSize) < maxBufferSize)&&(nofIndex < maxIndexSize))
        {
          index[nofIndex] = &buffer[storedBufferSize];
          memcpy(&buffer[storedBufferSize], file.name(), charLength);
          storedBufferSize += charLength;
          buffer[storedBufferSize] = 0x00;
          storedBufferSize++;
          nofIndex++;
        }
        else
        {
          return (nofIndex);
        }
      }
      file = root.openNextFile();
    }
    return (nofIndex);
  }

};
