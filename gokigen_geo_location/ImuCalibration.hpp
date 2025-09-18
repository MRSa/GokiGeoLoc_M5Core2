
static constexpr const uint8_t calib_value = 64;

class ImuCalibration
{
private:
  uint16_t _calib_countdown = 0;
  int prev_xpos[18];
  
  void _updateCalibration(bool clear = false)
  {
    if (_calib_countdown == 0)
    {
      clear = true;
    }

    if (clear)
    {
      memset(prev_xpos, 0, sizeof(prev_xpos));
      if (_calib_countdown > 0)
      {
        // キャリブレーションは地磁気だけにする。
        M5.Imu.setCalibration(0, 0, calib_value);
      }
      else
      {
        // キャリブレーションを止める
        M5.Imu.setCalibration(0, 0, 0);
        
        // save calibration values.
        M5.Imu.saveOffsetToNVS();

        _calib_countdown = 0;

        //Serial.println("----- Sensor Calibration Finished.");
      }
    }
}

public:
  ImuCalibration()
  {

  }

  void continueCalibration()
  {
    if (_calib_countdown > 0)
    {
      _calib_countdown--;
      _updateCalibration(false);
    }
  }

  bool isCalibrating()
  {
    return (_calib_countdown > 0);
  }

  void executeCalibration(uint16_t count)
  {
    _calib_countdown = count;
    _updateCalibration(true);
    //Serial.println("----- Sensor Calibration Started.");
  }

};
