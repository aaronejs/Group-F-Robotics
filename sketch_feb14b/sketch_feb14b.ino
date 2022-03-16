#include "Adafruit_VL53L0X.h"
#include <analogWrite.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
Adafruit_VL53L0X distSensor = Adafruit_VL53L0X();
int leftColorPin = 39;
int rightColorPin = 34;
int lColorVal, rColorVal, val = 0;
int rForward = 18;
int rReverse = 5;
int lForward = 16;
int lReverse = 17;
int spd = 180;
String turn;

// defines pins numbers
const int trigPin = 25;
const int echoPin = 26;
// defines variables
long duration;
int distance;

//

int turnStatus = 0;
int cycle = 1;
int state = 0;
unsigned long timerDelay = 10000;

unsigned long previousMillis = 0;
unsigned long turningMillis = 0;

const long interval = 250;
const long turningInterval = 4500;
const long reverseInterval = 1000;
//


int lArr[10];
int rArr[10];

void setup() {
  Serial.begin(115200);
  pinMode(rForward, OUTPUT);
  pinMode(rReverse, OUTPUT);
  pinMode(lForward, OUTPUT);
  pinMode(lReverse, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
}

void loop() {
  sensorInfo(3);
  //dist2();
  maze();
}

int dist2(){  
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  return distance;
}

void maze(){
    display.setCursor(120, 24);

    drive(1, "brake", 0);
    if(dist() < 175){
      drive(1, "brake", 0);
      delay(250);
      if(dist2() > 15){
        analogWrite(lForward, 0);
        analogWrite(lReverse, 160);
        analogWrite(rForward, 165);
        analogWrite(rReverse, 0);

        display.print("L");
      }else{
        analogWrite(lForward, 165);
        analogWrite(lReverse, 0);
        analogWrite(rForward, 0);
        analogWrite(rReverse, 160);
        display.print("R");
      }
    }else{
      analogWrite(lForward, 0);
      analogWrite(lReverse, 165);
      analogWrite(rForward, 0);
      analogWrite(rReverse, 165);
      display.print("F");
    }
    display.display();
}

void maze2(){  
  timerDelay = 600000;
  VL53L0X_RangingMeasurementData_t measure;
  unsigned long currentMillis = millis();
  unsigned long currentTurningMillis = millis();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
    
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
  } 
  if (cycle > 3){
    currentTurningMillis = millis();
    if (currentTurningMillis - turningMillis > reverseInterval) {
      turningMillis = currentTurningMillis;
      cycle = 1;
      turnStatus = 0;
    }

    analogWrite(lForward, 0);
    analogWrite(lReverse, 155);
    analogWrite(rForward, 0);
    analogWrite(rReverse, 180);
    //motor.rotate(motor1, 40, CCW);
    //motor.rotate(motor2, 75, CCW);
//            robot.rotate(motor1, 100, CCW);
//            robot.rotate(motor2, 0, CW);
  }else if(dist() <= 275 && turnStatus == 0){//left
    currentTurningMillis = millis();
    if (currentTurningMillis - turningMillis > (turningInterval + (cycle * 500))) {
      turningMillis = currentTurningMillis;
      turnStatus = 1;
      cycle += 1;
    }
    analogWrite(lForward, 180);
    analogWrite(lReverse, 0);
    analogWrite(rForward, 0);
    analogWrite(rReverse, 180);
   //motor.rotate(motor1, 0, CW);
  //  motor.rotate(motor2, 100, CCW);
    delay(250);
    analogWrite(lForward, 0);
    analogWrite(lReverse, 0);
    analogWrite(rForward, 0);
    analogWrite(rReverse, 0);
  //  motor.rotate(motor1, 100, CW);
  //  motor.rotate(motor2, 100, CW);
    
    display.print("Distance (mm): "); display.println(dist());
    display.println("Turning...left");
    //delay(100);
    display.display();
    display.clearDisplay();
    //delay(100)
  }else if(dist() <= 275 && turnStatus == 1){
    currentTurningMillis = millis();
    if (currentTurningMillis - turningMillis > (turningInterval + (cycle * 500))) {
    turningMillis = currentTurningMillis;
    turnStatus = 0;
    cycle += 1;
    }
    analogWrite(lForward, 0);
    analogWrite(lReverse, 180);
    analogWrite(rForward, 180);
    analogWrite(rReverse, 0);
  //  motor.rotate(motor1, 100, CCW);
  //  motor.rotate(motor2, 0, CW);
    delay(250);
    analogWrite(lForward, 0);
    analogWrite(lReverse, 0);
    analogWrite(rForward, 0);
    analogWrite(rReverse, 0);
  //  motor.rotate(motor1, 100, CW);
  //  motor.rotate(motor2, 100, CW);
    
    display.print("Distance (mm): "); display.println(dist());
    display.println("Turning...right");
    //delay(100);
    display.display();
    display.clearDisplay();
    //delay(100) 
  }else{
    analogWrite(lForward, 0);
    analogWrite(lReverse, 180);
    analogWrite(rForward, 0);
    analogWrite(rReverse, 180);
  //  motor.rotate(motor1, 60, CW);
  //  motor.rotate(motor2, 60, CW);
    display.print("Distance (mm): "); display.println(dist());
    display.println("VROOM VROOM!");
    //delay(100);
    display.display();
    display.clearDisplay();
    //delay(100)
  }
} 

void lineTracking() {
  display.setCursor(0, 16);
  int x = 100;
  
  if (lColor()-400 > x || rColor() > x) {
    if (abs((lColor()) - rColor()) > 500) {
      drive(1, "forward", spd);
      display.print("forward");
      if (lColor() > rColor()) {
        turn = "left";
      } else {
        turn = "right";
      }
    } else {
      drive(1, "forward", spd);
      display.print("forward SECONDARY");
    }
  }else{
    if(turn == "left"){
      drive(1, "left", spd);
      display.print(turn);
    }else if (turn == "right") {
      drive(1, "right", spd);
      display.print(turn);
    }else{
      drive(1, "forward", spd);
      display.print("FORWARD 2");
    }
  }
  display.display();
}

bool drive(int x, String y, int spd) {
  int z = 180;
  if (x == 1) {
    if (y == "forward") {
      analogWrite(lForward, spd);
      analogWrite(lReverse, 0);
      analogWrite(rForward, spd);
      analogWrite(rReverse, 0);
    } else if (y == "reverse") {
      analogWrite(lForward, 0);
      analogWrite(lReverse, spd);
      analogWrite(rForward, 0);
      analogWrite(rReverse, spd);
    } else if (y == "right") {
      analogWrite(lForward, z);
      analogWrite(lReverse, 0);
      analogWrite(rForward, 0);
      analogWrite(rReverse, z-10);
    } else if (y == "left") {
      analogWrite(lForward, 0);
      analogWrite(lReverse, z-10);
      analogWrite(rForward, z);
      analogWrite(rReverse, 0);
    } else if (y == "brake") {
      analogWrite(lForward, 0);
      analogWrite(lReverse, 0);
      analogWrite(rForward, 0);
      analogWrite(rReverse, 0);
    }
    return true;
  } else
    return false;
}

int lColor() {
  return analogRead(leftColorPin);
}

int rColor() {
  return analogRead(rightColorPin);
}

int dist() {
  if (!distSensor.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }

  VL53L0X_RangingMeasurementData_t measure;
  distSensor.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    return measure.RangeMilliMeter;
  } else {
    return -1;
  }
}

void sensorInfo(int x) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("R-color: ");
  display.print(rColor());
  display.setCursor(0, 8);
  display.print("L-color: ");
  display.print(lColor());
  display.setCursor(0, 16);
  display.print("Dist left: ");
  display.print(dist2());
  display.setCursor(0, 24);
  display.print("Dist front: ");
  display.print(dist());

  display.display();
}
