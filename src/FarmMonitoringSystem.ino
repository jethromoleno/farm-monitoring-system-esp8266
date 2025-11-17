#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include "DHT.h"
#include <ThingSpeak.h> // always include thingspeak header file after other header files and custom macros

char ssid[] = "ZTE_2.4G_VEHbs3";   // your network SSID (name)
char pass[] = "Moleno_94";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = 1518745;
const char * myWriteAPIKey = "GYD1ESC349Z479BB";

#define MUX_A D3
#define MUX_B D2
#define MUX_C D1
#define DHTPIN D4
#define DHTTYPE DHT11

#define ANALOG_INPUT A0

int ambtemp=0;
int hum=0;
float waterlvl;
float lightint;
int height;
long duration;
int distance;
const int initialHeight = 33;
const int trigPin = 12;  //D6
const int echoPin = 14;  //D5

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  dht.begin();
  Serial.begin(115200);  // Initialize serial
  while (!Serial) {
    ;
  }
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);     
  pinMode(MUX_C, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void changeMux(int c, int b, int a) {
  digitalWrite(MUX_A, a);
  digitalWrite(MUX_B, b);
  digitalWrite(MUX_C, c);
}

void loop() {
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.print("ZTE_2.4G_VEHbs3");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");

  }

  int v = ThingSpeak.writeField(myChannelNumber, 1, ambtemp, myWriteAPIKey);
  int w = ThingSpeak.writeField(myChannelNumber, 2, hum, myWriteAPIKey);
  int x = ThingSpeak.writeField(myChannelNumber, 3, waterlvl, myWriteAPIKey);
  int y = ThingSpeak.writeField(myChannelNumber, 4, lightint, myWriteAPIKey);
  int z = ThingSpeak.writeField(myChannelNumber, 5, height, myWriteAPIKey);

  changeMux(LOW, LOW, HIGH);
  lightint = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 1 pin of Mux (Read the values from photoresistor)
  Serial.println("Light Intensity: ");
  Serial.println(lightint);
  delay(1000);

  changeMux(LOW, HIGH, LOW);
  waterlvl = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 2 pin of Mux (Read the values from the water level sensor)
  if (waterlvl<=100)
    { Serial.println("Water Level: Empty"); }
    else if (waterlvl>100 && waterlvl<=300)
    { Serial.println("Water Level: Low"); }
    else if (waterlvl>300 && waterlvl<=680)
    { Serial.println("Water Level: Medium"); }
    else if (waterlvl>680)
    { Serial.println("Water Level: High"); }
  Serial.println("Water Level: ");
  Serial.println(waterlvl);
  delay(1000);

  ambtemp = dht.readTemperature(); // Reads the temperature from DHT11 sensor
  Serial.println("Ambient Temperature: ");
  Serial.println(ambtemp);
  delay(1000);

  hum = dht.readHumidity(); // Reads the humidity from DHT11 sensor
  Serial.println("Humidity: ");
  Serial.println(hum);
  delay(1000);

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
  distance = duration*0.034/2;
  height = initialHeight - distance; //calculates the plant height
  // Prints the distance on the Serial Monitor
  Serial.print("Height: ");
  Serial.println(height);
  delay(2000);

  delay(5000); // Wait 10 seconds to update the channel again
}
