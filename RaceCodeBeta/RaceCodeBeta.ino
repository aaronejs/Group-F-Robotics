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
int spd = 195;
const int internal_LED = 2; //No idea if this is still in there, double check please :D
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

  if (!distSensor.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
}

void loop(){
  Race();
}




void moveForward(){
  analogWrite(lForward, 0);
  analogWrite(lReverse, spd);
  analogWrite(rForward, 0);
  analogWrite(rReverse, spd);
}

void stopMovement() {
  analogWrite(lForward, 0);
  analogWrite(lReverse, 0);
  analogWrite(rForward, 0);
  analogWrite(rReverse, 0);
}


void turnRight(){
  analogWrite(lForward, spd);
  analogWrite(lReverse, 0);
  analogWrite(rForward, 0);
  analogWrite(rReverse, spd);
}



    
  void Race(){

    display.println("Relay Race Game");
    display.display();
    
    int value = analogRead(leftColorPin);
    Serial.println(value, DEC);
    if(value >= 115){
      digitalWrite(internal_LED, HIGH); //turn off the led
      stopMovement();
    }
    else{
      digitalWrite(internal_LED, LOW); //turn on the led
      
      VL53L0X_RangingMeasurementData_t measure; //distance value variable
    
      
      distSensor.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

     if (measure.RangeStatus != 4) {  // checks if distance is too great
     //Serial.print("Front measure (mm): "); Serial.println(measure.RangeMilliMeter); //prints value thats read
     } else {
      // Serial.println(" out of range ");
     }
     
//Checking the front wall
     if(measure.RangeMilliMeter > 100){
       moveForward();
     }
     else{
       turnRight();
     }
    }

   }
