#include <WiFi.h>
#include <HTTPClient.h>


const char* ssid = "AARON";
const char* password = "12345678";

//Domain name with URL path or IP address with path
String serverName = "WELITERALLYDONTHAVEONE.";


//RobotID variable
String robotID = "4";


int requestCounter = 1;
int postCounter = 1;
int speedPost = 0;

int sendSpeedPost() {
    if (postCounter == 1) {
        return speedPost = random(11,100);
        postCounter = 2;
      }
  }
void setup() {
  // Setup Code for Connecting to Wifi ofc

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

void loop() {
 httpPost(speedPost);
}
