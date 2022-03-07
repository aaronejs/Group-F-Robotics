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
int spd = 220;
String turn;
unsigned long time_now = 0;

int lArr[10];
int rArr[10];

void setup() {
  Serial.begin(115200);
  pinMode(rForward, OUTPUT);
  pinMode(rReverse, OUTPUT);
  pinMode(lForward, OUTPUT);
  pinMode(lReverse, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
}

void loop() {

  sensorInfo(0);
  lineTracking();
}

void lineTracking() {
  time_now = millis();
  display.setCursor(0, 16);
  int x = 1000;
  
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
  int z = 190;
  if (x == 1) {
    if (y == "forward") {
      analogWrite(lForward, spd);
      analogWrite(lReverse, 0);
      analogWrite(rForward, spd);
      analogWrite(rReverse, 0);
    } else if (y == "reverse") {
      analogWrite(lForward, 50);
      analogWrite(lReverse, spd);
      analogWrite(rForward, 50);
      analogWrite(rReverse, spd);
    } else if (y == "right") {
      analogWrite(lForward, z-10);
      analogWrite(lReverse, 0);
      analogWrite(rForward, 0);
      analogWrite(rReverse, z);
    } else if (y == "left") {
      analogWrite(lForward, 0);
      analogWrite(lReverse, z);
      analogWrite(rForward, z-10);
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
  if (x > 3) {
    display.setCursor(0, 16);
    display.print("Dist: ");
    display.print(dist());
  }
  display.display();
}
