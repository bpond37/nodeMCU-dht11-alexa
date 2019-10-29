#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);

#include "DHT.h"//++

#define DHTPIN D4     // Digital pin connected to the DHT sensor //++
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321 //++
DHT dht(DHTPIN, DHTTYPE); //핀넘버가 뭔지 , 타입이 뭔지 //++

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

}
