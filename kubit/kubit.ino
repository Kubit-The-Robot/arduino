#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <TM1640.h>
#include <TM16xxMatrixGFX.h>

#include <DFPlayerMini_Fast.h>

/************* Matrix led config *************/
#define MATRIX_NUMCOLUMNS 16
#define MATRIX_NUMROWS 8

const long animationTime = 3000;
const long blinkDuration = 250;
unsigned long previousMillis = 0;

String kubitState[11] = {
  "IS_ANGRY",
  "IS_BATTERY_LOW",
  "IS_DAMAGED",
  "IS_HUNGRY",
  "IS_SAD",
  "EATING",
  "ENERGIZING",
  "FLYING",
  "IDLE",
  "SMILING",
  "TALKING",
};

String defaultEyeState = "IS_OPEN";

String initialState = "IDLE";
String currentState = "IDLE";
String previousState = "INITIAL";

String nodemcuMessage;

DFPlayerMini_Fast myDFPlayer;

bool isPlaying = false;

void printDetail(uint8_t type, int value);

TM1640 leftMatrix(48, 50);
TM1640 rightMatrix(44, 46);

TM16xxMatrixGFX leftEye(&leftMatrix, MATRIX_NUMCOLUMNS, MATRIX_NUMROWS);    // TM16xx object, columns, rows
TM16xxMatrixGFX rightEye(&rightMatrix, MATRIX_NUMCOLUMNS, MATRIX_NUMROWS);    // TM16xx object, columns, rows

// Eye Shapes
void bigEye(TM16xxMatrixGFX eye) {
  eye.fillScreen(LOW);                 
  eye.drawLine(0, 5, 0, 10, HIGH);
  eye.drawLine(1, 4, 1, 11, HIGH);
  eye.fillRect(2, 3, 4, 10, HIGH);
  eye.drawLine(6, 4, 6, 11, HIGH);
  eye.drawLine(7, 5, 7, 10, HIGH);
  eye.write();
}

void blinkEye(TM16xxMatrixGFX eye, String state) {
  if (state == "IS_OPEN") {
    bigEye(eye);

    return;
  }

  closeEye(eye); 
}

void closeEye(TM16xxMatrixGFX eye) {
  eye.fillScreen(LOW);                 
  eye.drawLine(3, 3, 3, 12, HIGH);
  eye.drawLine(4, 3, 4, 12, HIGH);
  eye.write();
}

void damagedEye(TM16xxMatrixGFX eye) {
  eye.fillScreen(LOW);
  eye.drawLine(0, 4, 7, 11, HIGH);
  eye.drawLine(0, 5, 6, 11, HIGH);
  eye.drawLine(1, 4, 7, 10, HIGH);
  eye.drawLine(0, 11, 7, 4, HIGH);
  eye.drawLine(0, 10, 6, 4, HIGH);
  eye.drawLine(1, 11, 7, 5, HIGH);
  eye.write();
}

void happyEye(TM16xxMatrixGFX eye) {
  eye.fillScreen(LOW);
  eye.drawLine(2, 3, 2, 12, HIGH);
  eye.drawLine(3, 2, 3, 13, HIGH);
  eye.drawLine(4, 2, 4, 3, HIGH);
  eye.drawLine(4, 12, 4, 13, HIGH);
  eye.drawPixel(5, 2, HIGH);
  eye.drawPixel(5, 13, HIGH);
  eye.write();
}

void sadEye(TM16xxMatrixGFX eye) {
  eye.fillScreen(LOW);
  eye.drawPixel(2, 2, HIGH);
  eye.drawPixel(2, 13, HIGH);
  eye.drawLine(3, 2, 3, 3, HIGH);
  eye.drawLine(3, 12, 3, 13, HIGH);
  eye.drawLine(4, 2, 4, 13, HIGH);
  eye.drawLine(5, 3, 5, 12, HIGH);
  eye.write();
}

void smallEye(TM16xxMatrixGFX eye) {
  eye.fillScreen(LOW);                 
  eye.drawLine(2, 7, 2, 8, HIGH);
  eye.drawLine(3, 6, 3, 9, HIGH);
  eye.drawLine(4, 6, 4, 9, HIGH);
  eye.drawLine(5, 7, 5, 8, HIGH);
  eye.write();
}

// Kubit setState Functions
void setIsAngry() {
  leftEye.fillScreen(LOW);
  rightEye.fillScreen(LOW);

  leftEye.drawLine(0, 5, 0, 6, HIGH);
  leftEye.drawLine(1, 4, 1, 7, HIGH);
  leftEye.drawLine(2, 3, 2, 8, HIGH);
  leftEye.drawLine(3, 3, 3, 9, HIGH);
  leftEye.drawLine(4, 3, 4, 10, HIGH);
  leftEye.drawLine(5, 3, 5, 11, HIGH);
  leftEye.drawLine(6, 4, 6, 12, HIGH);
  leftEye.drawLine(7, 5, 7, 13, HIGH);
  
  rightEye.drawLine(0, 9, 0, 10, HIGH);
  rightEye.drawLine(1, 8, 1, 11, HIGH);
  rightEye.drawLine(2, 7, 2, 12, HIGH);
  rightEye.drawLine(3, 6, 3, 12, HIGH);
  rightEye.drawLine(4, 5, 4, 12, HIGH);
  rightEye.drawLine(5, 4, 5, 12, HIGH);
  rightEye.drawLine(6, 3, 6, 11, HIGH);
  rightEye.drawLine(7, 2, 7, 11, HIGH);

  leftEye.write();
  rightEye.write();
}

void setIsLowBattery() {
  sadEye(leftEye);
  sadEye(rightEye);
}

void setIsDamaged() {
  damagedEye(leftEye);
  damagedEye(rightEye);
}

void setIsHungry() {
  leftEye.fillScreen(LOW);
  rightEye.fillScreen(LOW);

  leftEye.drawLine(0, 12, 0, 12, HIGH);
  leftEye.drawLine(1, 11, 1, 12, HIGH);
  leftEye.drawLine(2, 10, 2, 12, HIGH);
  leftEye.drawLine(3, 9, 3, 12, HIGH);
  leftEye.drawLine(4, 8, 4, 12, HIGH);
  leftEye.drawLine(5, 7, 5, 12, HIGH);
  leftEye.drawLine(6, 6, 6, 12, HIGH);
  leftEye.drawLine(7, 5, 7, 11, HIGH);
  
  rightEye.drawLine(0, 3, 0, 3, HIGH);
  rightEye.drawLine(1, 3, 1, 4, HIGH);
  rightEye.drawLine(2, 3, 2, 5, HIGH);
  rightEye.drawLine(3, 3, 3, 6, HIGH);
  rightEye.drawLine(4, 3, 4, 7, HIGH);
  rightEye.drawLine(5, 3, 5, 8, HIGH);
  rightEye.drawLine(6, 3, 6, 9, HIGH);
  rightEye.drawLine(7, 4, 7, 10, HIGH);

  leftEye.write();
  rightEye.write();
}

void setIsSad() {
  sadEye(leftEye);
  sadEye(rightEye);
}

void setIsFlying() {
  leftEye.fillScreen(LOW);
  rightEye.fillScreen(LOW);

  leftEye.drawLine(1, 4, 1, 11, HIGH);
  leftEye.drawLine(2, 5, 2, 12, HIGH);
  leftEye.drawLine(3, 6, 3, 13, HIGH);
  leftEye.drawLine(4, 6, 4, 13, HIGH);
  leftEye.drawLine(5, 5, 5, 12, HIGH);
  leftEye.drawLine(6, 4, 6, 11, HIGH);
  
  rightEye.drawLine(1, 4, 1, 11, HIGH);
  rightEye.drawLine(2, 3, 2, 10, HIGH);
  rightEye.drawLine(3, 2, 3, 9, HIGH);
  rightEye.drawLine(4, 2, 4, 9, HIGH);
  rightEye.drawLine(5, 3, 5, 10, HIGH);
  rightEye.drawLine(6, 4, 6, 11, HIGH);

  leftEye.write();
  rightEye.write();
}

void setIsIdle(long currentMillis) {
  if (currentMillis - previousMillis >= animationTime) {
    previousMillis = currentMillis;
    
    if (defaultEyeState == "IS_OPEN") {
      defaultEyeState = "IS_CLOSED";
    } 
    else {
      defaultEyeState = "IS_OPEN";
    }
  }
  if (defaultEyeState == "IS_CLOSED" && currentMillis - previousMillis >= blinkDuration) {
    defaultEyeState = "IS_OPEN";
  }

  blinkEye(leftEye, defaultEyeState);
  blinkEye(rightEye, defaultEyeState);
}

void setEating() {
  closeEye(leftEye);
  closeEye(rightEye);
}

void setEnergizing() {
  smallEye(leftEye);
  smallEye(rightEye);
}

void setSmiling() {
  happyEye(leftEye);
  happyEye(rightEye);
}

void setTalking() {
  bigEye(leftEye);
  smallEye(rightEye);
}

void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(Serial1)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }

  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(15);
}

void loop() {
 unsigned long elapsedTime = millis();
 static unsigned long timer = millis();

  if (Serial.available() > 0) {
    nodemcuMessage = Serial.readString();
    
    currentState = nodemcuMessage;
    Serial.print(nodemcuMessage);
  }

  if (currentState == "IS_ANGRY") {
      setIsAngry();
  }
  else if (currentState == "IS_BATTERY_LOW") {
    setIsLowBattery();
  }
  else if (currentState == "IS_DAMAGED") {
    setIsDamaged();
  }
  else if (currentState == "IS_HUNGRY") {
    setIsHungry();
  }
  else if (currentState == "IS_SAD") {
    setIsSad();
  }
  else if (currentState == "EATING") {
    setEating();
  }
  else if (currentState == "ENERGIZING") {
    setEnergizing();
  }
  else if (currentState == "FLYING") {
    setIsFlying();
  }
  else if (currentState == "IDLE") {
    setIsIdle(elapsedTime);

    if (previousState != currentState) {
      myDFPlayer.playFromMP3Folder(13);

      previousState = currentState;
    }
  }
  else if (currentState == "SMILING") {
    setSmiling();
  }
  else if (currentState == "TALKING") {
    setTalking();
  }
  else {
    setIsIdle(elapsedTime);
  }
}
