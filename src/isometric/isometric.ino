#define BLACK_FIRST_NOTE 40
#define WHITE_FIRST_NOTE 64

const int selectPin[] = {15, 16, 17, 18, 19, 20};
const int dataPin[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14};

int keyNote[6][12];
int keyNoteOld[6][12];

long lastDebounceTime[6][12];
long debounceDelay = 2;

int sensorPin = A9;
int sensorValue = 0;

#define INMIN 690
#define INMAX 1010
#define OUTMIN 16
#define OUTMAX 127

#define VALUE_DIFERENCE 2
#define NUMBER_OF_SAMPLES 10

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

int calculateChannel(int d)
{
  if (d <= 5)
  {
    return(2);
  }
  else
  {
    return(1);
  }
}

int calculateVelocity()
{
  return(100);
}

void handleNoteOn(int s, int d)
{
  if ((millis() - lastDebounceTime[s][d]) > debounceDelay)
  {
    usbMIDI.sendNoteOn(calculatePitch(s, d), calculateVelocity(), calculateChannel(d));
    keyNoteOld[s][d] = keyNote[s][d];
    lastDebounceTime[s][d] = millis();
  }
}

void handleNoteOff(int s, int d)
{
  if ((millis() - lastDebounceTime[s][d]) > debounceDelay)
  {
    usbMIDI.sendNoteOff(calculatePitch(s, d), calculateVelocity(), calculateChannel(d));
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

int resultArray[NUMBER_OF_SAMPLES] = {0};
int result = 0;
int oldResult = 0;

float linearMapTo(float value, float inMin, float inMax, float outMin, float outMax)
{
  if (value > inMax) value = inMax;
  if (value < inMin) value = inMin;
  float result = (((value - inMin) * (outMax - outMin)) / (inMax - inMin)) + outMin;
  if (result > outMax) result = outMax;
  if (result < outMin) result = outMin;
  //Serial.printf("liner: value: %d, result %d\r\n", (int)value, (int)result);
  return(result);
}

int calcAverage(int *buffer)
{
  int result = 0;
  for (int i = 0; i < NUMBER_OF_SAMPLES; i++)
  {
    result += buffer[i];
  }
  result = result / NUMBER_OF_SAMPLES;
  return(result);
}

void setup()
{                
  Serial.begin(38400);
  initPins();
}

void readAnalogValues()
{
  // read the value from the sensor:
  for (int i = 0; i < NUMBER_OF_SAMPLES; i++)
  {
    sensorValue = analogRead(sensorPin);
    int tempResult = linearMapTo(sensorValue, INMIN, INMAX, OUTMIN, OUTMAX);
    //Serial.printf("value: %d, result %d\r\n", sensorValue, tempResult);
    resultArray[i] = tempResult;
    
  }
  result = calcAverage(resultArray);
  if ((result != oldResult) && (abs(result-oldResult) > VALUE_DIFERENCE))
  {
    //Serial.printf("value: %d\n", result);
    usbMIDI.sendControlChange(11, result, 1);
    oldResult = result;
}
}

void loop()
{
  checkSwitchStatus();
  resposneToSwitchStatus();
  readAnalogValues();
}
