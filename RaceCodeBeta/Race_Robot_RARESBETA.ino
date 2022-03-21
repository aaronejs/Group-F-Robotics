#include "Adafruit_VL53L0X.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

const byte led_gpio = 17; // the PWM pin the LED is attached to
const byte led_gpio1 = 5; // the PWM pin the LED is attached to

const byte back1 = 16; // the PWM pin the LED is attached to
const byte back2 = 18; // the PWM pin the LED is attached to
int leftSensor = 39; 
int rightSensor = 34; 

int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

// the setup routine runs once when you press reset:
void setup() {
  ledcAttachPin(led_gpio, 0); // assign a led pins to a channel
  ledcAttachPin(led_gpio1, 1); // assign a led pins to a channel
  ledcAttachPin(back1, 2); // assign a led pins to a channel
  ledcAttachPin(back2, 3); // assign a led pins to a channel

  // Initialize channels
  // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  ledcSetup(0, 4000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(1, 4000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(2, 4000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(3, 4000, 8); // 12 kHz PWM, 8-bit resolution

  pinMode (leftSensor, INPUT); // sensor pin INPUT
  pinMode (rightSensor, INPUT); // sensor pin INPUT

  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n"));
}

// the loop routine runs over and over again forever:
void loop() {
   VL53L0X_RangingMeasurementData_t measure;
    

  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  //displayDistance();



  
  
  if(measure.RangeMilliMeter > 200) { //increased range so it turn earlier  
    goForward(150);
  } 
  else if ( measure.RangeMilliMeter < 100 )
  {
    goStop();
    delay(100);
    turnLeft();
    delay(3000);
    // we need delay for turning extra!!! 
  }
  else
  {
    goStop();
    turnRight();
    delay(700);
    
  }

    
    

    //goBack();
    //delay(1200);
    //turnRight();
    //delay(1000);
 
     
    
 
}
int goForward(int speed){
    ledcWrite(0, speed); //right
    ledcWrite(1, speed); // left
    ledcWrite(2, 0); 
    ledcWrite(3, 0); 
}

int goStop(){
    ledcWrite(0, 0); //right
    ledcWrite(1, 0); // left
    ledcWrite(2, 0); 
    ledcWrite(3, 0); 
}

int goBack(){
    ledcWrite(0, 0); 
    ledcWrite(1, 0); 
    ledcWrite(2, 0); 
    ledcWrite(3, 0); 
}

int turnLeft(){
    ledcWrite(0, 255); 
    ledcWrite(1, 0); 
    ledcWrite(2, 0); 
    ledcWrite(3, 255); 
}

int turnRight(){
    ledcWrite(0, 0); 
    ledcWrite(1, 0); //stanga
    ledcWrite(2, 0); // 
    ledcWrite(3, 0); 
}

int displayDistance(){
  VL53L0X_RangingMeasurementData_t measure;
    

  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  
  // Display static text
  display.println(measure.RangeMilliMeter);
  display.display(); 
}
