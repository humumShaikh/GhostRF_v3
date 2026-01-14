#include <Arduino.h>
#include <Adafruit_SH110X.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106G oled = Adafruit_SH1106G(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

#define buttonPin PB4
#define inputPin PB3
#define freqSelectionPin PA3
#define modeSelectionPin PA5

#define outputHighPin PB1
#define outputLowPin PB0

#define resetPin PB8

int buttonNew;
int buttonOld = 1;

int counter = 0;
int f = 0;
int setFreq = 0;

float tb = 1/setFreq;
float tb2 = tb/2;

int amiCount = 0;

float startTime = 0;
float endTime = 0;

int mulFactorGiver(){
  int val;
  int cal = analogRead(modeSelectionPin);
  if(cal>=1 && cal<=250){
    val = 1;
  }
  else if(cal>=251 && cal<=500){
    val = 10;
  }
  else if(cal>=501 && cal<=750){
    val = 100;
  }
  else{
    val = 1000;
  }
  return val;
}

int countGiver(){
  buttonNew = digitalRead(buttonPin);

  if(buttonOld==0 && buttonNew==1){
    counter++;
  }
  buttonOld = buttonNew;
  delay(100);
  return counter;
}

int selectVal(){
 int x = analogRead(modeSelectionPin);
  return x;
}

void resetChecker(){
  if(countGiver()==3){
    digitalWrite(resetPin,LOW);
  }
}

void drawArrowRight(){
  oled.fillTriangle(100,35 , 100,55 , 120,45 , SH110X_WHITE);
}

void drawArrowLeft(){
  oled.fillTriangle(28,35 , 28,55 , 8,45 , SH110X_WHITE);
}

void UPNRZ(){
  if(digitalRead(inputPin)==HIGH){
    digitalWrite(outputLowPin,LOW);
    digitalWrite(outputHighPin,HIGH);
  }
  else{
    digitalWrite(outputLowPin,LOW);
    digitalWrite(outputHighPin,LOW);
  }
}

void UPRZ(){
  if(digitalRead(inputPin)==HIGH){
    digitalWrite(outputLowPin,LOW);
    digitalWrite(outputHighPin,HIGH);
    delay(tb2);
    digitalWrite(outputLowPin,LOW);
    digitalWrite(outputHighPin,LOW);
    delay(tb2);
  }
  else{
    digitalWrite(outputLowPin,LOW);
    digitalWrite(outputHighPin,HIGH);
  }
}

void BPNRZ(){
  if(digitalRead(inputPin)==HIGH){
    digitalWrite(outputLowPin,LOW);
    digitalWrite(outputHighPin,HIGH);
  }
  else{
    digitalWrite(outputLowPin,HIGH);
    digitalWrite(outputHighPin,LOW);
  }
}

void BPRZ(){
  if(digitalRead(inputPin)==HIGH){
    digitalWrite(outputLowPin,LOW);
    digitalWrite(outputHighPin,HIGH);
    delay(tb2);
    digitalWrite(outputLowPin,LOW);
    digitalWrite(outputHighPin,LOW);
    delay(tb2);
  }
  else{
    digitalWrite(outputLowPin,HIGH);
    digitalWrite(outputHighPin,LOW);
    delay(tb2);
    digitalWrite(outputLowPin,LOW);
    digitalWrite(outputHighPin,LOW);
    delay(tb2);
  }
}

void AMINRZ(){
  if(digitalRead(inputPin)==HIGH){
    if((amiCount%2)==0){
      digitalWrite(outputLowPin,LOW);
      digitalWrite(outputHighPin,HIGH);
      amiCount++;
    }
    else{
      digitalWrite(outputLowPin,HIGH);
      digitalWrite(outputHighPin,LOW);
      amiCount++;
    }
  }
  else{
    digitalWrite(outputLowPin,LOW);
    digitalWrite(outputHighPin,LOW);
  }
}

void AMIRZ(){
  if(digitalRead(inputPin)==HIGH){
    if((amiCount%2)==0){
      digitalWrite(outputLowPin,LOW);
      digitalWrite(outputHighPin,HIGH);
      delay(tb2);
      digitalWrite(outputLowPin,LOW);
      digitalWrite(outputHighPin,LOW);
      amiCount++;

    }
    else{
      digitalWrite(outputLowPin,HIGH);
      digitalWrite(outputHighPin,LOW);
      delay(tb2);
      digitalWrite(outputLowPin,LOW);
      digitalWrite(outputHighPin,LOW);
      amiCount++;
    }
  }
  else{
    digitalWrite(outputLowPin,LOW);
    digitalWrite(outputHighPin,LOW);
  }
}

void MANCHESTER(){
  if(digitalRead(inputPin)==HIGH){
    digitalWrite(outputLowPin,LOW);
    digitalWrite(outputHighPin,HIGH);
    delay(tb2);
    digitalWrite(outputLowPin,HIGH);
    digitalWrite(outputHighPin,LOW);
    delay(tb2);
  }
  else{
    digitalWrite(outputLowPin,HIGH);
    digitalWrite(outputHighPin,LOW);
    delay(tb2);
    digitalWrite(outputLowPin,LOW);
    digitalWrite(outputHighPin,HIGH);
    delay(tb2);
  }
}

void setup(){

  pinMode(resetPin,OUTPUT);
  digitalWrite(resetPin,HIGH);

  Serial.begin(115200);

  oled.begin(0x3c);
  oled.clearDisplay();
  oled.setTextColor(SH110X_WHITE);
  oled.setTextSize(1);

  pinMode(buttonPin,INPUT);
  pinMode(freqSelectionPin,INPUT);
  pinMode(modeSelectionPin,INPUT);
  pinMode(inputPin,INPUT);
  pinMode(outputHighPin,OUTPUT);
  pinMode(outputLowPin,OUTPUT);

}

void loop(){

  while((counter!=1) && (counter!=2)){
    oled.setTextSize(1);
    oled.clearDisplay();
    int F = analogRead(freqSelectionPin);
    f = F*0.1;
    oled.setCursor(0,13);
    oled.print("Freq : ");
    oled.print(f);
    oled.println(" Hz");
    oled.setCursor(0,33);
    oled.print("X Factor : ");
    oled.println(mulFactorGiver());
    oled.display();
    countGiver();
  }
  setFreq = f*mulFactorGiver();

  while(counter!=2){
    oled.setTextSize(1);
    oled.clearDisplay();
    oled.setCursor(26,3);
    oled.print("Set Frequency");
    oled.setCursor(10,25);
    oled.setTextSize(2);
    oled.print(setFreq);
    oled.println(" Hz");
    oled.display();
    countGiver();
  }

  while((selectVal()>=1) && (selectVal()<=150)){
    startTime = micros();
    oled.setTextSize(2);
    oled.clearDisplay();
    oled.setCursor(16,3);
    oled.print("UNIPOLAR");
    oled.setCursor(42,24);
    oled.print("NRZ");
    drawArrowRight();
    oled.display();
    UPNRZ();
    resetChecker();
    endTime = micros();
    Serial.println(endTime-startTime);
  }

  while((selectVal()>=150) && (selectVal()<=300)){
    oled.setTextSize(2);
    oled.clearDisplay();
    oled.setCursor(16,3);
    oled.print("UNIPOLAR");
    oled.setCursor(52,24);
    oled.print("RZ");
    drawArrowRight();
    drawArrowLeft();
    oled.display();
    UPRZ();
    resetChecker();
  }

  while((selectVal()>=300) && (selectVal()<=450)){
    oled.setTextSize(2);
    oled.clearDisplay();
    oled.setCursor(22,3);
    oled.print("BIPOLAR");
    oled.setCursor(46,24);
    oled.print("NRZ");
    drawArrowRight();
    drawArrowLeft();
    oled.display();
    BPNRZ();
    resetChecker();
  }

  while((selectVal()>=450) && (selectVal()<=600)){
    oled.setTextSize(2);
    oled.clearDisplay();
    oled.setCursor(22,3);
    oled.print("BIPOLAR");
    oled.setCursor(52,24);
    oled.print("RZ");
    drawArrowRight();
    drawArrowLeft();
    oled.display();
    BPRZ();
    resetChecker();
  }

  while((selectVal()>=600) && (selectVal()<=750)){
    oled.setTextSize(2);
    oled.clearDisplay();
    oled.setCursor(32,3);
    oled.print("A.M.I");
    oled.setCursor(44,24);
    oled.print("NRZ");
    drawArrowRight();
    drawArrowLeft();
    oled.display();
    AMINRZ();
    resetChecker();
  }

  while((selectVal()>=750) && (selectVal()<=900)){
    oled.setTextSize(2);
    oled.clearDisplay();
    oled.setCursor(32,3);
    oled.print("A.M.I");
    oled.setCursor(50,24);
    oled.print("RZ");
    drawArrowRight();
    drawArrowLeft();
    oled.display();
    AMIRZ();
    resetChecker();
  }

  while((selectVal()>=900) && (selectVal()<=1023)){
    oled.setTextSize(2);
    oled.clearDisplay();
    oled.setCursor(4,3);
    oled.print("MANCHESTER");
    drawArrowLeft();
    oled.display();
    resetChecker();
  }

}