#define BLACK_FIRST_NOTE 40
#define WHITE_FIRST_NOTE 64

const int selectPin[] = {15, 16, 17, 18, 19, 20};
const int dataPin[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14};

int keyNote[6][12];
int keyNoteOld[6][12];

long lastDebounceTime[6][12];
long debounceDelay = 5;

void initPins()
{
  for (int i = 0; i < 6; i++)
  {
    pinMode(selectPin[i], OUTPUT);
  }
  for (int i = 0; i < 12; i++)
  {
    pinMode(dataPin[i], INPUT);
  }
}

void checkSwitchStatus()
{
  for (int s = 0; s < 6; s++)
  {
    digitalWrite(selectPin[s], HIGH);
    for (int d = 0; d < 12; d++)
    {
      keyNote[s][d] = digitalRead(dataPin[d]);
    }
    digitalWrite(selectPin[s], LOW);
  }
}

int calculatePitch(int s, int d)
{
  int pitch;
  if (d <= 5)
  {
    pitch = BLACK_FIRST_NOTE + s*5 + d;
  }
  else
  {
    pitch = WHITE_FIRST_NOTE + s*5 + d-6;
  }
  return(pitch);
}

int calculateChannel(int s)
{
  //s+1
  return(1);
}

int calculateVelocity()
{
  return(100);
}

void handleNoteOn(int s, int d)
{
  if ((millis() - lastDebounceTime[s][d]) > debounceDelay)
  {
    usbMIDI.sendNoteOn(calculatePitch(s, d), calculateVelocity(), calculateChannel(s));
    keyNoteOld[s][d] = keyNote[s][d];
    lastDebounceTime[s][d] = millis();
  }
}

void handleNoteOff(int s, int d)
{
  if ((millis() - lastDebounceTime[s][d]) > debounceDelay)
  {
    usbMIDI.sendNoteOff(calculatePitch(s, d), calculateVelocity(), calculateChannel(s));
    keyNoteOld[s][d] = keyNote[s][d];
    lastDebounceTime[s][d] = millis();
  }
}

int isNoteOn(int s, int d)
{
  if (keyNote[s][d] == 1 && keyNote[s][d] != keyNoteOld[s][d])
  {
    return(1);
  }
  else
  {
    return(0);
  }
}

int isNoteOff(int s, int d)
{
  if (keyNote[s][d] == 0 && keyNote[s][d] != keyNoteOld[s][d])
  {
    return(1);
  }
  else
  {
    return(0);
  }
}

void resposneToSwitchStatus()
{
  for (int s = 0; s < 6; s++)
  {
    for (int d = 0; d < 12; d++)
    {
      if (isNoteOn(s, d))
      {
        handleNoteOn(s, d);
      }
      else if (isNoteOff(s, d))
      {
        handleNoteOff(s, d);
      }
    }
  }
}

void setup()
{                
  Serial.begin(38400);
  initPins();
}

void loop()
{
  checkSwitchStatus();
  resposneToSwitchStatus();
}
