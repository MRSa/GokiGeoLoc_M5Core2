// DcxMessageAdapter.hpp : DcxMessageを受信するために Streamを継承して実装
#include <Stream.h>
#include <WString.h>

class MyStringStream : public Stream
{
private:
    String _buffer;
    size_t _maxSize;
    size_t _readIndex;

public:
    MyStringStream(size_t maxSize = 65535) : _maxSize(maxSize), _readIndex(0)
    {
        // コンストラクタ
    }
    ~MyStringStream()
    {
        // デストラクタ
    }

    // ----- Streamクラスの仮想関数をオーバーライド -----

    int available()
    {
        // available(): バッファから読み込み可能なバイト数を返す
        return _buffer.length() - _readIndex;
    }

    int read()
    {
        // read(): バッファから1バイト読み込んで返す
        if (available() > 0) {
            return _buffer.charAt(_readIndex++);
        }
        return -1;        
    }
    
    int peek()
    {
        // peek(): 次に読み込むバイトを返すが、読み込み位置は進めない
        if (available() > 0) {
            return _buffer.charAt(_readIndex);
        }
        return -1;
    }
    
    size_t write(uint8_t c)
    {
        // write(uint8_t c): 1バイトをバッファに書き込む
        if (_buffer.length() < _maxSize)
        {
            _buffer += (char)c;
            return 1;
        }
        return 0; // バッファサイズ上限に達した
    }

    size_t write(const uint8_t *buffer, size_t size)
    {
        // write(const uint8_t *buffer, size_t size): バッファ全体を書き込む
        size_t bytesWritten = 0;
        for (size_t i = 0; i < size; ++i)
        {
            if (write(buffer[i]) == 1)
            {
                bytesWritten++;
            }
            else
            {
                break;
            }
        }
        return bytesWritten;
    }

    // ----- MyStringStream 独自のメソッド
    void clear()
    {
        // clear(): バッファの内容をクリアし、読み込み位置をリセット
        _buffer = "";
        _readIndex = 0;
    }
    
    String& getString()
    {
        // getString(): 現在のバッファのStringオブジェクトを返す
        return _buffer;
    }
};
