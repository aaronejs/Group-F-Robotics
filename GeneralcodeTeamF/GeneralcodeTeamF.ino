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

#define MOTOR1A 18 // left wheel forward
#define MOTOR1B 5 // left wheel reverse
#define ENA 18
#define CHA 0
#define MOTOR2A 17// right wheel forward
#define MOTOR2B 16// right wheel reverse
#define ENB 17
#define CHB 1
#define LED_BUILTIN 2



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
const int CW = 1;
const int CCW = 2;
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

#define motor2 2
#define motor1 1



//Oled display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)data:image/pjpeg;base64,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
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire); //for oled display
Adafruit_VL53L0X lox = Adafruit_VL53L0X();  // for distance sensor
Robojax_L298N_DC_motor motor(MOTOR1A, MOTOR1B, ENA, CHA, MOTOR2A, MOTOR2B, ENB, CHB);

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

//-------------------------------------------------------------------------------------VOID SETUP----------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------Movement Functions-----------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------VOID LOOP-----------------------------------------------------------------------------------------

void loop() {

//-------------------------------------------------------------------------HTTPS TASK REQUEST GET FOR BOT-ID-----------------------------------------------------------------------------
 
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
//-------------------------------------------------------------------------------HTTPS REQUEST END---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

 
//----------------------------------------------------------------------------------MAZE CODE--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------Declaring Maze Variables-------------------------------------------------------------------------------------

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
//-------------------------------------------------------------------------------------RELAY CODE----------------------------------------------------------------------------------------
    
 

//----------------------------------------------------------------------------STOP GAME AND FOLLOW THE LINE------------------------------------------------------------------------------
  if((strcmp(task, "idle") == 0) || (strcmp(task, "Idle") == 0)){
      lineFollow();
  }
}
