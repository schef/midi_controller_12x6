class AppTimer
{
private:
public:
  static uint32_t getMillis()
  {
    return millis();
  }

  static uint32_t millisPassed(uint32_t timestamp)
  {
    return getMillis() - timestamp;
  }
};

class ButtonParser
{
private:
  const uint8_t selectPin[6] = {15, 16, 17, 18, 19, 20};
  const uint8_t dataPin[12] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14};

  typedef struct
  {
    uint8_t selectIndex;
    uint8_t dataIndex;
    bool state;
    uint32_t stateTimestamp;
  } Key;

  Key keys[6][12];
  void (*onStateChange)(uint8_t selectIndex, uint8_t dataIndex, bool state) = nullptr;

  void initKeys()
  {
    for (uint32_t s = 0; s < sizeof(selectPin); s++)
    {
      pinMode(selectPin[s], OUTPUT);
      for (uint32_t d = 0; d < sizeof(dataPin); d++)
      {
        pinMode(dataPin[d], INPUT);
        Key &key = keys[s][d];
        key.selectIndex = s;
        key.dataIndex = d;
        key.state = false;
        key.stateTimestamp = 0;
      }
    }
  }

  void checkForButtonPress()
  {
    for (uint32_t s = 0; s < sizeof(selectPin); s++)
    {
      digitalWrite(selectPin[s], 1);
      for (uint32_t d = 0; d < sizeof(dataPin); d++)
      {
        Key &key = keys[s][d];
        bool state = digitalRead(dataPin[d]);
        if (state != key.state && AppTimer::millisPassed(key.stateTimestamp) >= 20)
        {
          key.state = state;
          key.stateTimestamp = AppTimer::getMillis();
          if (onStateChange)
          {
            onStateChange(key.selectIndex, key.dataIndex, state);
          }
        }
      }
      digitalWrite(selectPin[s], 0);
    }
  }

public:
  static ButtonParser &getInstance()
  {
    static ButtonParser instance;
    return instance;
  }

  void init()
  {
    initKeys();
  }

  void loop()
  {
    checkForButtonPress();
  }

  void registerOnStateChange(void (*func)(uint8_t selectPin, uint8_t dataPin, bool state))
  {
    onStateChange = func;
  }
};

class MidiPlayer
{
private:
public:
  static MidiPlayer &getInstance()
  {
    static MidiPlayer instance;
    return instance;
  }

  void handleNote(uint8_t channel, uint8_t pitch, uint8_t vel)
  {
    Serial.printf("handleNote c[%u] p[%u] v[%u]\n", channel, pitch, vel);
    usbMIDI.sendNoteOn(pitch, vel, channel);
  }

  void handleCc(uint8_t channel, uint8_t cc, uint8_t value)
  {
    Serial.printf("handleCc c[%u] cc[%u] v[%u]\n", channel, cc, value);
    usbMIDI.sendControlChange(cc, value, channel);
  }
};

class MidiHolder
{
private:
  bool midiRequests[128] = {0};
  bool midiInField[128] = {0};
  bool cmdRegister = false;

  float linearMapTo(float value, float inMin, float inMax, float outMin, float outMax)
  {
    if (value > inMax)
      value = inMax;
    if (value < inMin)
      value = inMin;
    float result = (((value - inMin) * (outMax - outMin)) / (inMax - inMin)) + outMin;
    if (result > outMax)
      result = outMax;
    if (result < outMin)
      result = outMin;
    return (result);
  }

  void checkVolume(uint8_t selectIndex, uint8_t dataIndex, bool state)
  {
    // selectIndex[0-5] and dataIndex[5]
    if (dataIndex == 5 && state)
    {
      {
        uint8_t volume = (uint8_t)linearMapTo(selectIndex * 1.0, 0.0, 5.0, 0.0, 127.0);
        Serial.printf("setVolume[%u]\n", volume);
        MidiPlayer::getInstance().handleCc(1, 7, volume);
      }
    }
  }

  void checkLeslie(uint8_t selectIndex, uint8_t dataIndex, bool state)
  {
    // selectIndex[0,1,2] and dataIndex[6]
    if (dataIndex == 6 && state)
    {
      switch (selectIndex)
      {
      case 0:
        Serial.printf("setLeslie[TREMOLO]\n");
        MidiPlayer::getInstance().handleCc(1, 1, 0);
        break;
      case 1:
        Serial.printf("setLeslie[OFF]\n");
        MidiPlayer::getInstance().handleCc(1, 1, 64);
        break;
      case 2:
        Serial.printf("setLeslie[CHORALE]\n");
        MidiPlayer::getInstance().handleCc(1, 1, 127);
        break;

      default:
        break;
      }
    }
  }

  void checkRegisters(uint8_t selectIndex, uint8_t dataIndex, bool state)
  {
    if (selectIndex == 5 && dataIndex == 6)
    {
      Serial.printf("cmdRegister[%u]\n", state);
      cmdRegister = state;
    }
    if (cmdRegister && state)
    {
      // selectIndex[0-5] = value 0-127, dataIndex[0-4, 7-11] drawbars
      uint8_t value = (uint8_t)linearMapTo(selectIndex * 1.0, 0.0, 5.0, 0.0, 127.0);
      switch (dataIndex)
      {
      case 0:
        Serial.printf("drawBar[16][%u]", value);
        MidiPlayer::getInstance().handleCc(1, 70, value);
        break;
      case 1:
        Serial.printf("drawBar[513][%u]", value);
        MidiPlayer::getInstance().handleCc(1, 71, value);
        break;
      case 2:
        Serial.printf("drawBar[8][%u]", value);
        MidiPlayer::getInstance().handleCc(1, 72, value);
        break;
      case 3:
        Serial.printf("drawBar[4][%u]", value);
        MidiPlayer::getInstance().handleCc(1, 73, value);
        break;
      case 4:
        Serial.printf("drawBar[223][%u]", value);
        MidiPlayer::getInstance().handleCc(1, 74, value);
        break;
      case 7:
        Serial.printf("drawBar[2][%u]", value);
        MidiPlayer::getInstance().handleCc(1, 75, value);
        break;
      case 8:
        Serial.printf("drawBar[135][%u]", value);
        MidiPlayer::getInstance().handleCc(1, 76, value);
        break;
      case 9:
        Serial.printf("drawBar[113][%u]", value);
        MidiPlayer::getInstance().handleCc(1, 77, value);
        break;
      case 10:
        Serial.printf("drawBar[1][%u]", value);
        MidiPlayer::getInstance().handleCc(1, 78, value);
        break;

      default:
        break;
      }
    }
  }

  void checkLowerManual(uint8_t selectIndex, uint8_t dataIndex, bool state)
  {
    if (dataIndex < 5)
    {
      //lowerKeys selectIndex[0-6], dataIndex[0-4], lowest pitch f-41
      uint8_t midiIndex = 41 + selectIndex * 5 + dataIndex * 1;
      Serial.printf("lowerManual[%u][%u]\n", midiIndex, state);
      MidiPlayer::getInstance().handleNote(1, midiIndex, state * 127);
    }
  }

  void checkUpperManual(uint8_t selectIndex, uint8_t dataIndex, bool state)
  {
    if (dataIndex > 6)
    {
      //lowerKeys selectIndex[0-6], dataIndex[7-11], lowest pitch f-65
      uint8_t midiIndex = 65 - 7 + selectIndex * 5 + dataIndex * 1;
      Serial.printf("upperManual[%u][%u]\n", midiIndex, state);
      MidiPlayer::getInstance().handleNote(1, midiIndex, state * 127);
    }
  }

  void onStateChange(uint8_t selectIndex, uint8_t dataIndex, bool state)
  {
    Serial.printf("keyIndex s[%u] d[%u] s[%u]\n", selectIndex, dataIndex, state);
    checkVolume(selectIndex, dataIndex, state);
    checkLeslie(selectIndex, dataIndex, state);
    checkRegisters(selectIndex, dataIndex, state);
    if (!cmdRegister)
    {
      checkLowerManual(selectIndex, dataIndex, state);
      checkUpperManual(selectIndex, dataIndex, state);
    }
  }

public:
  static MidiHolder &getInstance()
  {
    static MidiHolder instance;
    return instance;
  }

  void init()
  {
    ButtonParser::getInstance().registerOnStateChange(staticOnStateChange);
  }

  static void staticOnStateChange(uint8_t selectIndex, uint8_t dataIndex, bool state)
  {
    getInstance().onStateChange(selectIndex, dataIndex, state);
  }
};

void setup()
{
  Serial.begin(115200);
  ButtonParser::getInstance().init();
  MidiHolder::getInstance().init();
}

void loop()
{
  ButtonParser::getInstance().loop();
}