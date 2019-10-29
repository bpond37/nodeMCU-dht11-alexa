/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#ifndef STASSID
#define STASSID "Jihyeon의 iPhone"
#define STAPSK  "1234567890"
#endif


#include "DHT.h"//++

#define DHTPIN D4     // Digital pin connected to the DHT sensor //++
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321 //++
DHT dht(DHTPIN, DHTTYPE); //핀넘버가 뭔지 , 타입이 뭔지 //++

const char* ssid = STASSID;
const char* password = STAPSK;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

float h;//++
float t;//++

void setup() {
  Serial.begin(115200);
  
  //dht begin
  dht.begin();//++
  // prepare LED
  pinMode(D0, OUTPUT);
  digitalWrite(D0, 0);

  //display begin
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("KIMJI HOME");
  display.display();

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA); 
  // Station mode => ip를 받아가는 모드
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

    display.clearDisplay();
    display.display();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("KIMJI HOME");
    display.println("Wifi connected");
    display.display();
    delay(2000);
  // Start the server
  server.begin();
  Serial.println(F("Server started"));
//  display.println("Server started");
//  display.display();

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  
  float h = dht.readHumidity();     // ++Read temperature as Celsius (the default)
  float t = dht.readTemperature();  // ++
  
    if (isnan(h) || isnan(t)) {   //ISNAN = IS NOT A NUMBER?   || => OR을 의미
    Serial.println(F("Failed to read from DHT sensor!"));
    
     display.clearDisplay();
     display.display();
     display.setTextSize(2);
     display.setTextColor(WHITE);
     display.setCursor(0,0);
     display.println("KIMJI HOME");
     display.println("FAIL DHT");
     display.display();
     delay(500);
   digitalWrite(D0, 0);
   delay(100);
   digitalWrite(D0, 1);
   delay(100);
    return;
  }
  
    display.clearDisplay();
    display.display();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("KIMJI HOME");
    display.print("T:");
    display.print((int)t);
    display.print("/H:");
    display.print((int)h);
    display.print("%");
    display.display();
  
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.println(F("°C "));
    
  delay(10000);
  // Check if a client has connected
  // server.available() => 계속 체크
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  // Match the request
  int val;
  if (req.indexOf(F("/gpio/0")) != -1) {
    val = 0;
    client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"));
  } else if (req.indexOf(F("/gpio/1")) != -1) {
    val = 1;
    client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"));
  } else if (req.indexOf(F("/gpio/2")) != -1) {
    client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"));
    client.print(F("Temperature is "));
    client.print((int)t);
    
    client.print(F(" degree celsius and Humidity is"));
    client.print((int)h);
    client.print(F("percent"));
    
  }  else {
    Serial.println(F("invalid request"));
    val = digitalRead(D0);
  }

  // Set LED according to the request
  digitalWrite(D0, val);

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
 /* client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now "));
  client.print((val) ? F("high") : F("low"));
  client.print(F("<br><br>Click <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio/1'>here</a> to switch LED GPIO on, or <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio/0'>here</a> to switch LED GPIO off.</html>"));*/

  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  Serial.println(F("Disconnecting from client"));
}
