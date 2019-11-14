#define STRING_0 40
#define STRING_1 STRING_0 + 5
#define STRING_2 STRING_1 + 5
#define STRING_3 STRING_0 + 12 * 2
#define STRING_4 STRING_3 + 5
#define STRING_5 STRING_4 + 5

const uint8_t selectPin[] = {15, 16, 17, 18, 19, 20};
const uint8_t dataPin[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14};

struct Key {
  bool state;
  uint8_t selectPin;
  uint8_t dataPin;
  uint32_t lastTimestamp;
  bool shouldExecute;
};

struct Key keys[sizeof(selectPin) * sizeof(dataPin)];

uint8_t chord[12] = {0};

void initPins() {
  for (uint8_t i = 0; i < sizeof(selectPin); i++) {
    pinMode(selectPin[i], OUTPUT);
  }
  for (uint8_t i = 0; i < sizeof(dataPin); i++) {
    pinMode(dataPin[i], INPUT);
  }
}

void initKeys() {
  for (uint8_t s = 0; s < sizeof(selectPin); s++) {
    for (uint8_t d = 0; d < sizeof(dataPin); d++) {
      struct Key &key = keys[s * sizeof(dataPin) + d];
      key.state = false;
      key.selectPin = s;
      key.dataPin = d;
      key.lastTimestamp = 0;
      key.shouldExecute = false;
    }
  }
}

void checkSwitchStatus() {
  for (uint8_t s = 0; s < sizeof(selectPin); s++) {
    digitalWrite(selectPin[s], HIGH);
    for (uint8_t d = 0; d < sizeof(dataPin); d++) {
      bool buttonState = digitalRead(dataPin[d]);
      struct Key &key = keys[s * sizeof(dataPin) + d];
      if (buttonState != key.state && millis() - key.lastTimestamp > 2) {
        key.state = buttonState;
        key.lastTimestamp = millis();
        key.shouldExecute = true;
        Serial.print("State change: ");
        Serial.print(s, DEC);
        Serial.print(":");
        Serial.print(d, DEC);
        Serial.println("");
      }
    }
    digitalWrite(selectPin[s], LOW);
  }
}

// void handleNoteOn(uint8_t s, int d)
// {
//   if ((millis() - lastDebounceTime[s][d]) > debounceDelay)
//   {
//     int channel = calculateChannel(s, d);
//     if (channel == 2)
//     {

//       usbMIDI.sendControlChange(calculatePitch(s, d), 127, 1);
//       keyNoteOld[s][d] = keyNote[s][d];
//       lastDebounceTime[s][d] = millis();
//     }
//     else
//     {
//       usbMIDI.sendNoteOn(calculatePitch(s, d), calculateVelocity(),
//       calculateChannel(s, d)); keyNoteOld[s][d] = keyNote[s][d];
//       lastDebounceTime[s][d] = millis();
//     }
//   }
// }

// void handleNoteOff(uint8_t s, int d)
// {
//   if ((millis() - lastDebounceTime[s][d]) > debounceDelay)
//   {
//     int channel = calculateChannel(s, d);
//     if (channel == 2)
//     {
//       usbMIDI.sendControlChange(calculatePitch(s, d), 0, 1);
//       keyNoteOld[s][d] = keyNote[s][d];
//       lastDebounceTime[s][d] = millis();
//     }
//     else
//     {
//       usbMIDI.sendNoteOff(calculatePitch(s, d), calculateVelocity(),
//       calculateChannel(s, d)); keyNoteOld[s][d] = keyNote[s][d];
//       lastDebounceTime[s][d] = millis();
//     }
//   }
// }

void handleNote(uint8_t pitch, uint8_t channel, bool state) {
  if (state) {
    usbMIDI.sendNoteOn(pitch, state * 127, channel);
  } else {
    usbMIDI.sendNoteOff(pitch, state * 127, channel);
  }
}

void allNotesOff() {
  for (uint8_t i = 0; i < 127; i++) {
    handleNote(i, 1, false);
  }
}

bool isKey(struct Key &key, uint8_t select, uint8_t data) {
  return key.selectPin == select && key.dataPin == data;
}

void handleChord(uint8_t pitch, uint8_t channel, bool state) {
  for (uint8_t i = 0; i < sizeof(chord); i++) {
    if (chord[i]) {
      handleNote(pitch + i, channel, state);
    }
  }
}

void resposneToSwitchStatus() {
  for (uint32_t i = 0; i < sizeof(keys) / sizeof(Key); i++) {
    struct Key &key = keys[i];
    if (key.shouldExecute) {
      key.shouldExecute = false;
      if (isKey(key, 0, 0)) {
        handleNote(STRING_0 + 0, 1, key.state);
      } else if (isKey(key, 0, 1)) {
        handleNote(STRING_0 + 1, 1, key.state);
      } else if (isKey(key, 0, 2)) {
        handleNote(STRING_0 + 2, 1, key.state);
      } else if (isKey(key, 0, 3)) {
        handleNote(STRING_0 + 3, 1, key.state);
      } else if (isKey(key, 0, 4)) {
        handleNote(STRING_0 + 4, 1, key.state);
      } else if (isKey(key, 0, 5)) {
        handleNote(STRING_0 + 5, 1, key.state);

      } else if (isKey(key, 1, 0)) {
        handleNote(STRING_1 + 0, 1, key.state);
      } else if (isKey(key, 1, 1)) {
        handleNote(STRING_1 + 1, 1, key.state);
      } else if (isKey(key, 1, 2)) {
        handleNote(STRING_1 + 2, 1, key.state);
      } else if (isKey(key, 1, 3)) {
        handleNote(STRING_1 + 3, 1, key.state);
      } else if (isKey(key, 1, 4)) {
        handleNote(STRING_1 + 4, 1, key.state);
      } else if (isKey(key, 1, 5)) {
        handleNote(STRING_1 + 5, 1, key.state);

      } else if (isKey(key, 2, 0)) {
        handleNote(STRING_2 + 0, 1, key.state);
      } else if (isKey(key, 2, 1)) {
        handleNote(STRING_2 + 1, 1, key.state);
      } else if (isKey(key, 2, 2)) {
        handleNote(STRING_2 + 2, 1, key.state);
      } else if (isKey(key, 2, 3)) {
        handleNote(STRING_2 + 3, 1, key.state);
      } else if (isKey(key, 2, 4)) {
        handleNote(STRING_2 + 4, 1, key.state);
      } else if (isKey(key, 2, 5)) {
        handleNote(STRING_2 + 5, 1, key.state);

      } else if (isKey(key, 3, 0)) {
        handleChord(STRING_3 + 0, 1, key.state);
      } else if (isKey(key, 3, 1)) {
        handleChord(STRING_3 + 1, 1, key.state);
      } else if (isKey(key, 3, 2)) {
        handleChord(STRING_3 + 2, 1, key.state);
      } else if (isKey(key, 3, 3)) {
        handleChord(STRING_3 + 3, 1, key.state);
      } else if (isKey(key, 3, 4)) {
        handleChord(STRING_3 + 4, 1, key.state);
      } else if (isKey(key, 3, 5)) {
        handleChord(STRING_3 + 5, 1, key.state);

      } else if (isKey(key, 4, 0)) {
        handleChord(STRING_4 + 0, 1, key.state);
      } else if (isKey(key, 4, 1)) {
        handleChord(STRING_4 + 1, 1, key.state);
      } else if (isKey(key, 4, 2)) {
        handleChord(STRING_4 + 2, 1, key.state);
      } else if (isKey(key, 4, 3)) {
        handleChord(STRING_4 + 3, 1, key.state);
      } else if (isKey(key, 4, 4)) {
        handleChord(STRING_4 + 4, 1, key.state);
      } else if (isKey(key, 4, 5)) {
        handleChord(STRING_4 + 5, 1, key.state);

      } else if (isKey(key, 5, 0)) {
        handleChord(STRING_5 + 0, 1, key.state);
      } else if (isKey(key, 5, 1)) {
        handleChord(STRING_5 + 1, 1, key.state);
      } else if (isKey(key, 5, 2)) {
        handleChord(STRING_5 + 2, 1, key.state);
      } else if (isKey(key, 5, 3)) {
        handleChord(STRING_5 + 3, 1, key.state);
      } else if (isKey(key, 5, 4)) {
        handleChord(STRING_5 + 4, 1, key.state);
      } else if (isKey(key, 5, 5)) {
        handleChord(STRING_5 + 5, 1, key.state);

      } else if (isKey(key, 5, 6)) {
        allNotesOff();

      } else if (isKey(key, 5, 7)) {
        // select M7
        chord[0] = true;   // c
        chord[1] = false;  // c#
        chord[2] = false;  // d
        chord[3] = false;  // d#
        chord[4] = true;   // e
        chord[5] = false;  // f
        chord[6] = false;  // f#
        chord[7] = true;   // g
        chord[8] = false;  // g#
        chord[9] = false;  // a
        chord[10] = false; // b
        chord[11] = true;  // h

      } else if (isKey(key, 5, 8)) {
        // select 7
        chord[0] = true;   // c
        chord[1] = false;  // c#
        chord[2] = false;  // d
        chord[3] = false;  // d#
        chord[4] = true;   // e
        chord[5] = false;  // f
        chord[6] = false;  // f#
        chord[7] = true;   // g
        chord[8] = false;  // g#
        chord[9] = false;  // a
        chord[10] = true;  // b
        chord[11] = false; // h

      } else if (isKey(key, 5, 9)) {
        // select m7
        chord[0] = true;   // c
        chord[1] = false;  // c#
        chord[2] = false;  // d
        chord[3] = true;   // d#
        chord[4] = false;  // e
        chord[5] = false;  // f
        chord[6] = false;  // f#
        chord[7] = true;   // g
        chord[8] = false;  // g#
        chord[9] = false;  // a
        chord[10] = true;  // b
        chord[11] = false; // h

      } else if (isKey(key, 5, 10)) {
        // select m7b5
        chord[0] = true;   // c
        chord[1] = false;  // c#
        chord[2] = false;  // d
        chord[3] = true;   // d#
        chord[4] = false;  // e
        chord[5] = false;  // f
        chord[6] = true;   // f#
        chord[7] = false;  // g
        chord[8] = false;  // g#
        chord[9] = false;  // a
        chord[10] = true;  // b
        chord[11] = false; // h

      } else if (isKey(key, 5, 11)) {
        // select dim7
        chord[0] = true;   // c
        chord[1] = false;  // c#
        chord[2] = false;  // d
        chord[3] = true;   // d#
        chord[4] = false;  // e
        chord[5] = false;  // f
        chord[6] = true;   // f#
        chord[7] = false;  // g
        chord[8] = false;  // g#
        chord[9] = true;   // a
        chord[10] = false; // b
        chord[11] = false; // h
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Setup");
  initPins();
  initKeys();
}

void loop() {
  checkSwitchStatus();
  resposneToSwitchStatus();
}