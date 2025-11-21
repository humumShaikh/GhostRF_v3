// NRF24 Connection
// SCK=18, MISO=19, MOSI=23, CS=5, CE=4/15

#include <Arduino.h>
#include <RF24.h>
#include <Preferences.h>

SPIClass *hp = nullptr;

RF24 radio(4, 5);///CE,CSN
Preferences preferences;

byte i = 45;

unsigned int flag = 0;

const int interval = 10000;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

#define b1 34

void setup(void) {
  Serial.begin(115200);
  preferences.begin("Jammer", false);
  unsigned int counter = preferences.getUInt("counter", 0);
  pinMode(b1,INPUT);

  if((counter%2)==0){
    pinMode(13,OUTPUT);
    pinMode(14,OUTPUT);
    pinMode(26,OUTPUT);
    pinMode(27,OUTPUT);
    digitalWrite(13,HIGH);
    digitalWrite(14,HIGH);
    digitalWrite(26,LOW);
    digitalWrite(27,LOW);
    counter++;
    preferences.putUInt("counter", counter);
  }

  else if((counter%2)!=0){
  pinMode(13,OUTPUT);
  pinMode(14,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(27,OUTPUT);
  digitalWrite(13,LOW);
  digitalWrite(14,LOW);
  digitalWrite(26,HIGH);
  digitalWrite(27,HIGH);
  counter++;
  preferences.putUInt("counter", counter);
  }
  preferences.end();
  initHP();
}
void initHP() {
  hp = new SPIClass(VSPI);
  hp->begin();
  if (radio.begin(hp)) {
    delay(200);  
    radio.setAutoAck(false);
    radio.stopListening();
    radio.setRetries(0, 0);
    radio.setPayloadSize(5);   ////SET VALUE ON RF24.CPP
    radio.setAddressWidth(3);  ////SET VALUE ON RF24.CPP
    radio.setPALevel(RF24_PA_MAX, true);
    radio.setDataRate(RF24_2MBPS);
    radio.setCRCLength(RF24_CRC_DISABLED);
    radio.printPrettyDetails();
    radio.startConstCarrier(RF24_PA_MAX, i);
  } 
}

void loop() {
  for (int i = 0; i < 79; i++) {
    radio.setChannel(i);
    currentMillis = millis();
    if((currentMillis - previousMillis)>=interval){
      ESP.restart();
    }
  }
}