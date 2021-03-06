#include <Arduino.h>
#include <microTuple.h>

#define pinTrigger 13
#define pinEcho 12

#define pinLedGreen 11
#define pinLedYellow 10
#define pinLedRed 9

#define pinBuzzer 8

#define NEAR_DISTANCE 30
#define FAR_DISTANCE 60

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(pinTrigger, OUTPUT);
  pinMode(pinEcho, INPUT);
  pinMode(pinLedGreen, OUTPUT);
  pinMode(pinLedYellow, OUTPUT);
  pinMode(pinLedRed, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
}

void handleSonorousAlert(char distanceCode)
{
  boolean useAlert = false;
  int
      interval = 0,
      frequency = 0;

  switch (distanceCode)
  {
  case 'N':
    useAlert = true;
    interval = 100;
    frequency = 400;
    break;

  case 'M':
    useAlert = true;
    interval = 500;
    frequency = 600;
    break;

  case 'F':
    useAlert = false;
    break;
  }

  if (useAlert)
  {
    tone(pinBuzzer, frequency);
    delay(100);

    noTone(pinBuzzer);
    delay(interval);
  }
}

void handleVisualAlert(char distanceCode)
{
  int ledPins[] = {pinLedGreen, pinLedYellow, pinLedRed};

  for (const int &pin : ledPins)
    digitalWrite(pin, LOW);

  int pinToTurnOn = pinLedGreen;

  switch (distanceCode)
  {
  case 'N':
    pinToTurnOn = pinLedRed;
    break;

  case 'M':
    pinToTurnOn = pinLedYellow;
    break;

  case 'F':
    pinToTurnOn = pinLedGreen;
    break;
  }

  digitalWrite(pinToTurnOn, HIGH);
}

/**
 * N -> Near
 * M -> Medium
 * F -> Far
*/
void handleDistanceAlert(char distanceCode)
{
  handleVisualAlert(distanceCode);
  handleSonorousAlert(distanceCode);
}

/**
 * Returns MicroTuple<long distanceInCentimeters, char distanceCode> 
*/
MicroTuple<long, char> getDistanceData()
{
  digitalWrite(pinTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrigger, LOW);

  long pulseTime = pulseIn(pinEcho, HIGH);
  double distanceInCentimeters = 0.01715 * pulseTime;

  char distanceCode;

  if (distanceInCentimeters < NEAR_DISTANCE)
    distanceCode = 'N';
  else if (NEAR_DISTANCE <= distanceInCentimeters && distanceInCentimeters < FAR_DISTANCE)
    distanceCode = 'M';
  else
    distanceCode = 'F';

  MicroTuple<long, char> distanceData(distanceInCentimeters, distanceCode);

  return distanceData;
}

void loop()
{
  auto [distanceInCentimeters, distanceCode] = getDistanceData();

  Serial.println((String)distanceCode.get<0>() + " - " + distanceInCentimeters);

  handleDistanceAlert(distanceCode.get<0>());
}
