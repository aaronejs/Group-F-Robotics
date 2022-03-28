#include "Adafruit_VL53L0X.h"
#include <analogWrite.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

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
int mazeSpd = 185;
int turm = 0;
String turn;
int game;

//
bool mazeGame = false;
bool line = false;
bool Stop = true;
bool Forward = false;
//

long duration;
int distance;
// defines pins numbers
const int trigPin = 25;
const int echoPin = 26;


const char* ssid     = "aaron";
const char* password = "12345678";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(rForward, OUTPUT);
  pinMode(rReverse, OUTPUT);
  pinMode(lForward, OUTPUT);
  pinMode(lReverse, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.begin();

  if (!distSensor.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
}

void loop(){
  wifiConn();

  if(mazeGame)
    maze();
  else if(line)
    lineTracking();
  else if(Stop)
    stopMove();
  else if(Forward)
    forward(180);
}

void wifiConn(){
   WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":

        executeCMD(currentLine);
      }
    }

    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void executeCMD(String game){
  if(game == "GET /MAZE"){
    line = false;  
    mazeGame = true;
    Forward = false;
    Stop = false;
  }else if(game == "GET /LINE"){
    line = true;
    mazeGame = false;
    Forward = false;
    Stop = false;
  }else if(game == "GET /STOP"){
    line = false;
    mazeGame = false;
    Forward = false;
    Stop = true;
  }else if(game == "GET /FORWARD"){
    line = false;
    mazeGame = false;
    Forward = true;
    Stop = false;
  }
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
    display.setTextSize(1.5);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    
    stopMove();
    if(dist() < 185){
      if(dist2() > 26 || turm == 1){
        turm = 1; 
        left(mazeSpd);
        delay(150);
        display.print("L");
      }else if(dist2() <= 26 || turm == 2){
        turm = 2;
        right(mazeSpd);
        delay(150);
        display.print("R");
      }
    }else{
      delay(10) ;
      turm = 0;
      forward(mazeSpd);
      display.print("F");
    }
    display.setCursor(0,8);
    display.println(dist2());
    display.setCursor(0,16);
    display.println(dist());
    display.display();
}


void lineTracking() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 16);
  int x = 100;
  
  if (lColor()-400 > x || rColor() > x) {
    if (abs((lColor()) - rColor()) > 500) {
      stopMove();
      delay(20);
      backward(spd);
      display.print("forward");
      if (lColor() > rColor()) {
        turn = "left";
      } else {
        turn = "right";
      }
    } else {
      stopMove();
      delay(20);
      backward(spd);
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
      stopMove();
      delay(20);
      backward(spd);
      display.print("FORWARD 2");
    }
  }
  display.display();
}

void forward(int Speed){
  analogWrite(lForward, 0);
  analogWrite(lReverse, Speed);
  analogWrite(rForward, 0);
  analogWrite(rReverse, Speed);
}
void left(int Speed){
  analogWrite(lForward, 0);
  analogWrite(lReverse, Speed+10);
  analogWrite(rForward, Speed+10);
  analogWrite(rReverse, 0);
}
void right(int Speed){
  analogWrite(lForward, Speed+10);
  analogWrite(lReverse, 0);
  analogWrite(rForward, 0);
  analogWrite(rReverse, Speed+10);
}
void backward(int Speed){
  analogWrite(lForward, Speed);
  analogWrite(lReverse, 0);
  analogWrite(rForward, Speed);
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
      analogWrite(rReverse, z);
    } else if (y == "left") {
      analogWrite(lForward, 0);
      analogWrite(lReverse, z);
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
