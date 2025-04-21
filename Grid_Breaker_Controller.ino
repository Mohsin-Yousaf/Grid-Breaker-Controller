#include "ACS712.h"

int BreakerState = 5;

int BreakerRemote = 4;
int BreakerButton = 3;         // the number of the input pin
int BreakerPin = 10;       // the number of the output pin

int WarningPin = 12;
int HighLoad = 13;

int BreakerPinState = LOW;      // the current state of the output pin
int BreakerButtonCurrentValue;           // the current reading from the input pin
int BreakerButtonOldValue = LOW;    // the previous reading from the input pin
int BreakerRemoteCurrentValue;           // the current reading from the input pin
int BreakerRemoteOldValue = LOW;    // the previous reading from the input pin


// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long BreakerToggleTime = 0;           // the last time the output pin was toggled
unsigned long debounce = 200;   // the debounce time, increase if the output flickers

ACS712  ACSa(A0, 5.0, 1023, 185);
ACS712  ACSb(A1, 5.0, 1023, 185);
ACS712  ACSc(A2, 5.0, 1023, 185);


void setup()
{
  pinMode(BreakerState, OUTPUT);

  pinMode(BreakerRemote, INPUT);
  pinMode(BreakerButton,  INPUT);

  pinMode(BreakerPin, OUTPUT);
  pinMode(WarningPin, OUTPUT);
  pinMode(HighLoad, OUTPUT);
  
  digitalWrite(HighLoad, LOW);
  digitalWrite(BreakerState, LOW);

  Serial.begin(9200);
  while (!Serial);

  ACSa.autoMidPoint();
  ACSb.autoMidPoint();
  ACSc.autoMidPoint();



}

void loop()
{
  
////////////////////// Breaker Control ////////////////////

  BreakerRemoteCurrentValue = digitalRead(BreakerRemote);
  BreakerButtonCurrentValue = digitalRead(BreakerButton);

  // if the input just went from LOW and HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time
  //if (BreakerButtonCurrentValue == HIGH && BreakerButtonOldValue == LOW  && millis() - BreakerToggleTime > debounce) || (BreakerRemoteCurrentValue == HIGH && BreakerRemoteOldValue == LOW  && millis() - BreakerToggleTime > debounce)
  if (((BreakerButtonCurrentValue == HIGH && BreakerButtonOldValue == LOW) || 
     (BreakerRemoteCurrentValue == HIGH && BreakerRemoteOldValue == LOW)) &&
    (millis() - BreakerToggleTime > debounce))
  {
    if (BreakerPinState == HIGH)
      BreakerPinState = LOW;
    else
      BreakerPinState = HIGH;
    BreakerToggleTime = millis();

  }
  digitalWrite(BreakerPin, BreakerPinState);
  digitalWrite(BreakerState, BreakerPinState);

  BreakerRemoteOldValue = BreakerRemoteCurrentValue;
  BreakerButtonOldValue = BreakerButtonCurrentValue;

  float avgA = 0;
  float avgB = 0;
  float avgC = 0;

  
  for (int i = 0; i < 3; i++)
  {
    delay(10);
    avgA += ACSa.mA_AC();
    avgB += ACSb.mA_AC();
    avgC += ACSc.mA_AC();
  }
  
  float cA = avgA / 3.0;
  float cB = avgB / 3.0;
  float cC = avgC / 3.0;

  Serial.print("  MilliCurrentA: ");
  Serial.println(cA);

    if (cA >=500 || cB >=500 || cC >=500)
  {
    Serial.print("Current Exceeded");
    digitalWrite(BreakerPin, LOW);
    BreakerPinState = LOW;
    
    digitalWrite(HighLoad, HIGH);
    digitalWrite(WarningPin, HIGH);
    delay(2000);
    digitalWrite(HighLoad, LOW);
    digitalWrite(WarningPin, LOW);

    }

}
