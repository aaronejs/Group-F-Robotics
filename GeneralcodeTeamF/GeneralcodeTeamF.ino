#include <ArduinoJson.h>
#include <Arduino_JSON.h>
// #include <MPU9250.h>
#include <Robojax_L298N_DC_motor.h>
#include <SPI.h>
#include "Adafruit_VL53L0X.h" //including distance sensor library
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> //including display library
#include <WiFi.h>
#include <HTTPClient.h>

// motor 1 settings
#define IN1 2
#define IN2 4
#define ENA 3 // this pin must be PWM enabled pin

// motor 2 settings
#define IN3 7
#define IN4 8
#define ENB 9 // this pin must be PWM enabled pin

const int CCW = 2; // do not change
const int CW  = 1; // do not change

#define motor1 1 // do not change
#define motor2 2 // do not change

// use the line below for single motor
//Robojax_L298N_DC_motor motor(IN1, IN2, ENA, true);

// use the line below for two motors
Robojax_L298N_DC_motor motor(IN1, IN2, ENA, IN2, IN3, ENB, true);


const char* ssid = "SAMBA";
const char* password = "";

//Domain name with URL path or IP address with path
String serverName = "WELITERALLYDONTHAVEONE.";

unsigned long lastTime = 0;
// Timer set to 10 minutes (6000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 10000;



//RobotID variable
String robotID = "4";



//An MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
int status;

int isEngOn = 1; // Changed from boolean to int and checked it in the if statement
int irPin1 = 34;
int irPin2 = 39;
const int digital_pin = 4;
const int internal_LED = 2;
const int Speed = 60;
unsigned long previousMillis = 0;
unsigned long turningMillis = 0;
const long interval = 250;
const long turningInterval = 4500;
const long reverseInterval = 1000;
int turnStatus = 0;
int cycle = 1;
int state = 0;
int ldrValueLeft;
int ldrValueRight;
String currentTask = "";



//Oled display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire); //for oled display
Adafruit_VL53L0X lox = Adafruit_VL53L0X();  // for distance sensor


int slotPlayed = 0;

int wall = 0;
int wallAgainToRight = 0;
int wallAgainToLeft = 0;
int frontAgainDistance = 120;
int o = 1;
const char* task = "";

int requestCounter = 1;
int postCounter = 1;
int speedPost = 0;

//Woah, a setup!
int sendSpeedPost() {
    if (postCounter == 1) {
        return speedPost = random(11,100);
        postCounter = 2;
      }
  }
void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //setup display
  Wire.begin();
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X")); //checks if distance sensor works
    while(1);
  }
  pinMode(digital_pin, INPUT);
  pinMode(internal_LED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(irPin2, INPUT); //setup infrared sensor
  pinMode(irPin1, INPUT);
  sendSpeedPost();
  motor.begin();
  display.clearDisplay();   //clearing display
  display.display();
  //seting up display
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while(1) {}
  }
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    display.print("Connecting to WiFi");
    display.println("");
    delay(5000);
    display.display();
  }
  display.clearDisplay();
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 2 seconds (timerDelay variable), it will take 2 seconds before publishing the first reading.");
}

//Woah! Movement!
void lineFollow() {
    unsigned long currentMillis = millis();
    
    ldrValueRight = analogRead(irPin1);
    ldrValueLeft = analogRead(irPin2);
  
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.print("Left: "); display.println(ldrValueRight);
    display.print("Right: "); display.println(ldrValueLeft);
    //delay(100);
    display.setCursor(0,0);
    display.display();
    display.clearDisplay();

    //line detected by both
    if(ldrValueLeft >=50 && ldrValueRight >= 50){
      //stop
    motor.rotate(motor1, 60, CW);
    motor.rotate(motor2, 60, CW); 
  
    }
    //line detected by left sensor
    else if(ldrValueLeft >=50 && ldrValueRight <= 50){
      //turn left
      motor.rotate(motor1, 100, CW);
      motor.rotate(motor2, 100, CW);
      delay(35);
      motor.rotate(motor1, 0, CW);
      motor.rotate(motor2, 100, CCW);
    }
    //line detected by right sensor
    else if(ldrValueLeft <=50 && ldrValueRight >=50){
      //turn right
      motor.rotate(motor1, 100, CW);
      motor.rotate(motor2, 100, CW);
      delay(25);
      motor.rotate(motor1, 100, CCW);
      motor.rotate(motor2, 0, CW);
    }
    //line detected by none
    else if (currentMillis - previousMillis >= 9000) {
        previousMillis = currentMillis;
        motor.rotate(motor1, 100, CW);
        motor.rotate(motor2, 100, CW);
      } 
      
      
    
 }

void moveForward(){
  motor.rotate(motor1, 70, CW);
    motor.rotate(motor2, 62.5, CW);
}

void stopMovement(){
 motor.rotate(motor1, 100, CW);
    motor.rotate(motor2, 0, CW);
}

void turnRight(){
  motor.rotate(motor1, 30, CW);
    motor.rotate(motor2, 40, CCW);
}

void turnLeft(){
 motor.rotate(motor1, 60, CCW);
    motor.rotate(motor2, 80, CW);
}

void turnBackLeft(){
  motor.rotate(motor1, 40, CCW);
    motor.rotate(motor2, 60, CW);
}

void reverse(){
 motor.rotate(motor1, 40, CCW);
    motor.rotate(motor2, 60, CCW);
}

void rotateToRight(){
  motor.rotate(motor1, 40, CW);
    motor.rotate(motor2, 60, CCW);
}


void httpPost(int setData) {
  String S = "id=4&data=" + String(setData);
  
  //JSONVar myObject = JSON.parse(S);
      HTTPClient http;
      http.begin(serverName + "/setdata");
      http.addHeader("Content-Type" ,"application/x-www-form-urlencoded");

      int httpResponseCode = http.POST(S);

       if (httpResponseCode>0){
        //Object array1[] = new array1[2]{};
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
       }
      else{
        Serial.println("Error in sending");
      }
     http.end();
}

void requestTask(){
      HTTPClient http;
      String getTask = serverName + "/task?id=" + robotID;

      if(requestCounter == 1){
      http.begin(getTask);
      int httpResponseCode = http.GET();
      
      if(httpResponseCode > 0){
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        const size_t capacity = JSON_ARRAY_SIZE(100) + 5 * JSON_OBJECT_SIZE(100) + JSON_OBJECT_SIZE(100) + 200;
        Serial.println("\nStatuscode: " + String(httpResponseCode));

        DynamicJsonDocument doc(1000);
        
        DeserializationError error = deserializeJson(doc, payload);
        
        if(error){
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }
        
        task = doc["data"]["task"]; // "Race"
        bool state = doc["status"]; // true
        const char* message = doc["message"]; // "Request successful."
        currentTask = task;

        Serial.println(state);
        Serial.println(message);
        Serial.print("Task:");
        Serial.println(task);

      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
      }

      requestCounter = 2;
}
//WOAH! LOOPS!

void loop() {

//HTTP-Request-On-ID
 
  //HTTPS Request For Task Based on robot ID
  if((millis() - lastTime) > timerDelay){
    if(WiFi.status() == WL_CONNECTED){
      if (strcmp(task, "idle")) {
        httpPost(speedPost);
          requestTask();
          
        }
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  
    ldrValueRight = analogRead(irPin1);
    ldrValueLeft = analogRead(irPin2);
//Request-Ends-Here


 
//Maze-Code :D
  if((strcmp(task, "maze") == 0) || (strcmp(task, "Maze") == 0)){
    timerDelay = 600000;
    VL53L0X_RangingMeasurementData_t measure;
    unsigned long currentMillis = millis();
    unsigned long currentTurningMillis = millis();
    
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
    } 
    if (cycle > 3){
          currentTurningMillis = millis();
          if (currentTurningMillis - turningMillis > reverseInterval) {
          turningMillis = currentTurningMillis;
          cycle = 1;
          turnStatus = 0;
          }
          motor.rotate(motor1, 40, CCW);
          motor.rotate(motor2, 75, CCW);
//            robot.rotate(motor1, 100, CCW);
//            robot.rotate(motor2, 0, CW);
    } 
      //left
      else if (measure.RangeMilliMeter <= 175 && turnStatus == 0) {
        currentTurningMillis = millis();
        if (currentTurningMillis - turningMillis > (turningInterval + (cycle * 500))) {
          turningMillis = currentTurningMillis;
          turnStatus = 1;
          cycle += 1;
        }
        motor.rotate(motor1, 0, CW);
        motor.rotate(motor2, 100, CCW);
        delay(100);
        motor.rotate(motor1, 100, CW);
        motor.rotate(motor2, 100, CW);
        
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.print("Distance (mm): "); display.println(measure.RangeMilliMeter);
        display.println("Turning...left");
        //delay(100);
        display.setCursor(0,0);
        display.display();
        Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
        Serial.println("Turning...");
        display.clearDisplay();
        //delay(100)
//right
        } else if (measure.RangeMilliMeter <= 175 && turnStatus == 1) {
            currentTurningMillis = millis();
            if (currentTurningMillis - turningMillis > (turningInterval + (cycle * 500))) {
            turningMillis = currentTurningMillis;
            turnStatus = 0;
            cycle += 1;
            }
            
            motor.rotate(motor1, 100, CCW);
            motor.rotate(motor2, 0, CW);
            delay(100);
            motor.rotate(motor1, 100, CW);
            motor.rotate(motor2, 100, CW);
            
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0,0);
            display.print("Distance (mm): "); display.println(measure.RangeMilliMeter);
            display.println("Turning...right");
            //delay(100);
            display.setCursor(0,0);
            display.display();
            Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
            Serial.println("Turning...");
            display.clearDisplay();
            //delay(100) 
// vroom vroom
        } else {
       
        motor.rotate(motor1, 60, CW);
        motor.rotate(motor2, 60, CW);
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.print("Distance (mm): "); display.println(measure.RangeMilliMeter);
        display.println("VROOM VROOM!");
        //delay(100);
        display.setCursor(0,0);
        display.display();
        Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
        Serial.println("vroom vroom");
        display.clearDisplay();
        //delay(100)
    }

     
     // timerDelay = 10000;
      if((millis() - lastTime) > timerDelay){
        if(WiFi.status() == WL_CONNECTED){
          
        } else {
          Serial.println("WiFi Disconnected");
        }
        lastTime = millis();
      }
    
  }

  if(strcmp(task, "anything") == 0 || (strcmp(task, "Anything") == 0)){
    //fill in code...
  }


//STOP AND FOLLOW
  if((strcmp(task, "idle") == 0) || (strcmp(task, "Idle") == 0)){
      lineFollow();
  }
}
