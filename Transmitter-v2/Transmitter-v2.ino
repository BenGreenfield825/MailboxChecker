/*
 *  Transmitter code for mailbox checker
 *  Author: Ben Greenfield
 */

#include <LowPower.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

unsigned long radioStartTime;
unsigned long radioEndTime;
unsigned long lightStartTime;
unsigned long lightEndTime;

const int switchPin = 2;
bool switchState;

const int photoresisterPin = A5;
int lightThreshold = 50; //this is used as an offset for the light value - we should only trigger if the value changes by this much
int lightRaw;
int lightValue;
int prevLValue;
int lightCalibrateTime = 250; //lightCalibrateTime * 10 = time spent to calibrate the sensor (i.e. 100 would be 1000 millis or 1 sec)

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  pinMode(switchPin, INPUT_PULLUP); //use pullup to assume high value for floating

  pinMode(photoresisterPin, INPUT);
  CalibrateLightSensor();

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX); //changed to max power
  radio.stopListening();
}

void loop() {
  switchState = digitalRead(switchPin);

  if (!switchState) { //if slide switch is read as LOW, enter debug mode
    Serial.println("DEBUG MODE");
    DebugMode();
  }
  else {  //else enter normal operation
    CheckForMail();
  }
}

void CheckForMail() {
  lightStartTime = millis();
  lightEndTime = lightStartTime;
  while((lightEndTime - lightStartTime) <= 6000) {  //check light values for 6 seconds
    lightRaw = analogRead(photoresisterPin);
    lightValue = map(lightRaw, 1023, 0, 500, 0);
    Serial.print("Light value: ");
    Serial.println(lightValue);

    if (lightValue > (prevLValue + lightThreshold)) {
      Serial.println("Threshold passed! Sending trigger!");
      radioStartTime = millis();
      radioEndTime = radioStartTime;
      while ((radioEndTime - radioStartTime) <= 10000) { //send message for 10 seconds
        const char text[] = "MAIL";
        Serial.println(text);
        radio.write(&text, sizeof(text));
        radioEndTime = millis();
      }
    }
    lightEndTime = millis();
    prevLValue = lightValue;
    delay(100);
  }
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF); //power down for 4 seconds
  KeepBatteryPackOn();
}

//Give the sensor time to even out it's values at startup.
//(set the delay longer if you're placing it in the mailbox for the first time
//so that you have time to place it and shut the door)
void CalibrateLightSensor() {
  Serial.print("Calibrating light sensor...");
  for (int i = 0; i < 10; i++) {
    lightRaw = analogRead(photoresisterPin);
    lightValue = map(lightRaw, 1023, 0, 500, 0);
    prevLValue = lightValue;
    Serial.print(".");
    delay(lightCalibrateTime); //use delay instead of millis as we want this to be done before anything else
  }
  Serial.println();
}

//Most battery packs (including mine) shutoff if there is not enough current being drawn.
//This function tells the radio to send a message for a second to temporarily boost the current
void KeepBatteryPackOn() {
  unsigned long start = millis();
  unsigned long end = start;
  while ((end - start) <= 1000) {
    const char text[] = "STAYALIVE";
    Serial.println(text);
    radio.write(&text, sizeof(text));
    end = millis();
  }
}

void DebugMode() {
  const char text[] = "MAIL";
  Serial.println(text);
  radio.write(&text, sizeof(text));
}
