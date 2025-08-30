class TouchPositionHandler
{
  // ----- タッチ位置の保持
private:
  bool _isPressed = false;
  int _posX = -1;
  int _posY = -1;

public:
  TouchPositionHandler()
  {

  }

  int getTouchX() { return (_posX); }
  int getTouchY() { return (_posY); }

  void resetPosition()
  {
    _isPressed = false;
    _posX = -1;
    _posY = -1;
  }

  void handleTouchPosition()
  {
    auto t = M5.Touch.getDetail();
    _isPressed = t.isPressed();
    if (_isPressed)
    {
        _posX = t.x;
        _posY = t.y;
    }
    
    // ----- タッチ位置をシリアルで通知(押されているときのみ)
    if ((_isPressed)&&(_posX > 0)&&(_posY > 0))
    {
        Serial.print("Touch Position: ");
        Serial.print(_posX);
        Serial.print(",");
        Serial.print(_posY);
        Serial.print("  [");
        Serial.print(_isPressed);
        Serial.println("]");
    }
  }
};
