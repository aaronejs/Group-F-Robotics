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
int spd = 185;
int turm = 0;
String turn;

// defines pins numbers
const int trigPin = 25;
const int echoPin = 26;

void setup() {
  Serial.begin(115200);
  pinMode(rForward, OUTPUT);
  pinMode(rReverse, OUTPUT);
  pinMode(lForward, OUTPUT);
  pinMode(lReverse, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  if (!distSensor.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
}

void loop() {
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
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    
    stopMove();
    if(dist() < 200){
      if(dist2() > 21 || turm == 1){
        turm = 1; 
        left();
        display.print("L");
      }else if(dist2() < 20 || turm == 2){
        turm = 2;
        right();
        display.print("R");
      }
    }else{
      //delay(25) ;
      turm = 0;
      forward();
      display.print("F");
    }
    display.display();
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

void forward(){
  analogWrite(lForward, 0);
  analogWrite(lReverse, spd);
  analogWrite(rForward, 0);
  analogWrite(rReverse, spd);
}
void left(){
  analogWrite(lForward, 0);
  analogWrite(lReverse, spd);
  analogWrite(rForward, spd);
  analogWrite(rReverse, 0);
}
void right(){
  analogWrite(lForward, spd);
  analogWrite(lReverse, 0);
  analogWrite(rForward, 0);
  analogWrite(rReverse, spd);
}
void backward(){
  analogWrite(lForward, spd);
  analogWrite(lReverse, 0);
  analogWrite(rForward, spd);
  analogWrite(rReverse, 0);
}
void stopMove(){
  analogWrite(lForward, 0);
  analogWrite(lReverse, 0);
  analogWrite(rForward, 0);
  analogWrite(rReverse, 0);
}


bool drive(int x, String y, int spd) {
  int z = 180;
  if (x == 1) {
    if (y == "forward") {
      
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
  VL53L0X_RangingMeasurementData_t measure;
  distSensor.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    return measure.RangeMilliMeter;
  } else {
    return 99999;
  }
}

void sensorInfo() {
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
  
  //display.setCursor(0, 24);
  //display.print("Dist front: ");
  //display.print(dist());

  display.display();
}
