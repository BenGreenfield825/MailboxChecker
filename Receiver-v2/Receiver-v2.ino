/*
 *  Receiver code for mailbox checker
 *  Author: Ben Greenfield
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int buttonPin = 4;
bool buttonState;
const int ledPin = 5;
const int switchPin = 2;
bool switchState;

int ledState = LOW;             // ledState used to set the LED
unsigned long previousMillis = 0;        // will store last time LED was updated
long OnTime = 1000;           // milliseconds of on-time
long OffTime = 1000;          // milliseconds of off-time

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}

void loop() {
  switchState = digitalRead(switchPin);

  if(!switchState) {
    Serial.println("DEBUG");
    DebugMode();
  }
  else {
    Serial.println("NORMAL");
    digitalWrite(ledPin, LOW);
    ReceiveMessage();
  }
}

void ReceiveMessage() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
    if(!strcmp(text, "MAIL")) {  
      MessageIndicator();
    }
  }

  delay(100);
}

void MessageIndicator() { //make the led blink on and off (using millis) and look for button to be pressed to go back to normal mode
  while(true) {
    Serial.println("test");
    buttonState = digitalRead(buttonPin);
    Serial.println(buttonState);
    if(buttonState) {
      digitalWrite(ledPin, LOW);
      break;
    }
    
    unsigned long currentMillis = millis();
   
    if((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
    {
      ledState = LOW;  // Turn it off
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(ledPin, ledState);  // Update the actual LED
    }
    else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
    {
      ledState = HIGH;  // turn it on
      previousMillis = currentMillis;   // Remember the time
      digitalWrite(ledPin, ledState);    // Update the actual LED
    }
  }
}

void DebugMode() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
    digitalWrite(ledPin, HIGH);
  }
  else {
    Serial.println("No radio");
    digitalWrite(ledPin, LOW);
  }
  delay(100);
}
