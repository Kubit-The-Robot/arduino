#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <TM1640.h>
#include <TM16xxMatrixGFX.h>

#include "DFRobotDFPlayerMini.h"

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

DFRobotDFPlayerMini myDFPlayer;

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

  myDFPlayer.volume(10);
  myDFPlayer.disableLoop();
}

void loop() {
 unsigned long elapsedTime = millis();
 static unsigned long timer = millis();

  if (Serial.available() > 0) {
    nodemcuMessage = Serial.readString();
    
    currentState = nodemcuMessage;
    Serial.print(nodemcuMessage);
  }

  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
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
      myDFPlayer.play(13);

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

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
