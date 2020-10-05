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

  void handleNote(uint8_t pitch, uint8_t channel, bool state)
  {
    Serial.printf("[%u][%u]-[%u]", pitch, channel, state);

    if (state)
    {
      usbMIDI.sendNoteOn(pitch, state * 127, channel);
    }
    else
    {
      usbMIDI.sendNoteOff(pitch, state * 127, channel);
    }
  }

  void handleCc(uint8_t cc, uint8_t channel, uint8_t value)
  {
    Serial.printf("[%u][%u]-[%u]", cc, channel, value);
    usbMIDI.sendControlChange(cc, value, channel);
  }
};

class MidiHolder
{
private:
  bool midiRequests[128] = {0};
  bool midiInField[128] = {0};

  void onStateChange(uint8_t selectIndex, uint8_t dataIndex, bool state)
  {
    Serial.printf("key[%u][%u]-[%u]\n", selectIndex, dataIndex, state);
    if (selectIndex == 5 && dataIndex == 5)
    {
      if (state) {
        MidiPlayer::getInstance().handleCc(7, 1, 127);
      }
    }
    else if (selectIndex == 5 && dataIndex == 6)
    {
    }
    else if (selectIndex == 4 && dataIndex == 5)
    {
      if (state) {
        MidiPlayer::getInstance().handleCc(7, 1, 106);
      }
    }
    else if (selectIndex == 4 && dataIndex == 6)
    {
    }
    else if (selectIndex == 3 && dataIndex == 5)
    {
      if (state) {
        MidiPlayer::getInstance().handleCc(7, 1, 85);
      }
    }
    else if (selectIndex == 3 && dataIndex == 6)
    {
    }
    else if (selectIndex == 2 && dataIndex == 5)
    {
      if (state) {
        MidiPlayer::getInstance().handleCc(7, 1, 64);
      }
    }
    else if (selectIndex == 2 && dataIndex == 6)
    {
      if (state) {
        MidiPlayer::getInstance().handleCc(1, 1, 0);
      }
    }
    else if (selectIndex == 1 && dataIndex == 5)
    {
      if (state) {
        MidiPlayer::getInstance().handleCc(7, 1, 43);
      }
    }
    else if (selectIndex == 1 && dataIndex == 6)
    {
      if (state) {
        MidiPlayer::getInstance().handleCc(1, 1, 127);
      }
    }
    else if (selectIndex == 0 && dataIndex == 5)
    {
      if (state) {
        MidiPlayer::getInstance().handleCc(7, 1, 22);
      }
    }
    else if (selectIndex == 0 && dataIndex == 6)
    {
      if (state) {
        MidiPlayer::getInstance().handleCc(1, 1, 64);
      }
    }
    else
    {
      if (dataIndex < 5)
      {
        //lowerKeys selectIndex[0-6], dataIndex[0-4], midiStart f[41]
        uint8_t midiIndex = 29 + selectIndex * 5 + dataIndex * 1;
        Serial.printf("midi[%lu]\n", midiIndex);
        MidiPlayer::getInstance().handleNote(midiIndex, 2, state);
      }
      else if (dataIndex > 6)
      {
        //lowerKeys selectIndex[0-6], dataIndex[7-11], midiStart f[65]
        uint8_t midiIndex = 53 - 7 + selectIndex * 5 + dataIndex * 1;
        Serial.printf("midi[%lu]\n", midiIndex);
        MidiPlayer::getInstance().handleNote(midiIndex, 1, state);
      }
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