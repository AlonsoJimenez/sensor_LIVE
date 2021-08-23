#include "WiFi.h"
#include "HTTPClient.h"

const char* ssid = "Finca Chicuagua";
const char* password =  "Chicuagua2019";
const unsigned int trigger = 12;
const unsigned int echo = 33;
const unsigned int serialRate = 9600;
bool occupied = false;
bool pastOccupied = false;


void setup() {
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(serialRate);
  Serial.begin(9600);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

}

void loop() {
  if (pastOccupied != occupied) {
    pastOccupied = occupied;
    callOcu();

  }
  occupied = false;
  // put your main code here, to run repeatedly:
  int distance = 11;
  int loopCount = 0;

  while (distance > 3 and distance < 35) {
    digitalWrite(trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);
    const unsigned long duration = pulseIn(echo, HIGH);
    if (duration == 0) {
      Serial.println("Warning: no pulse from sensor");
      break;
    }
    distance = duration / 29 / 2;
    Serial.println(distance);
    delay(1000);
    Serial.println("Primero va:" + loopCount);
    loopCount = loopCount + 1;
    if (loopCount == 10) {
      occupied = true;
      break;
    }
  }

  delay(100);
}

void callOcu() {

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    http.begin("http://live-cargador.uc.r.appspot.com/chargerOcupied");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", "Basic Y2FyZ2Fkb3I6bGl2ZTE=");
    String putData = "local=%5B9.854901%2C%20-83.907499%5D";
    http.addHeader("Content-Length", "36");
    http.addHeader("Host", "live-cargador.uc.r.appspot.com");
    int httpResponseCode = http.PUT(putData);

    if (httpResponseCode > 0) {

      String response = http.getString();

      Serial.println(httpResponseCode);
      Serial.println(response);

    } else {

      Serial.print("Error on sending PUT Request: ");
      Serial.println(httpResponseCode);

    }

    http.end();

  } else {
    Serial.println("Error in WiFi connection");
  }

  delay(10000);
}