#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 //number of columns
#define SCREEN_HEIGHT 64 //number of rows
#define OLED_RESET -1 //Reset pin (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c // I2C (InterIntegrated circuit) address for the display

//DHT var(pin, type)
DHT dht(4, DHT22); //Need to write it for this sensor in this format

//Adafruit_SSD1306 var(parameters)
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //initialises the type of display used

int revolutions = 0;
unsigned long initial = 0;
unsigned long current = 0;
bool si = false;

void setup() {
  Serial.begin(115200);
  dht.begin();
  oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  
  pinMode(34, INPUT); //AQI
  pinMode(26, INPUT); //Hall
  
  pinMode(33, INPUT); //LDR
  pinMode(13, OUTPUT); //LED
}

void loop() {
  // put your main code here, to run repeatedly:
  float a = dht.readTemperature();
  float b = dht.readHumidity();

  bool sf = digitalRead(26);

  if (sf == 1 && si == 0) {
    initial = millis(); // Record the start time
  }
  
  if (sf == 0 && si == 1) {
    current = millis(); // Record the end time
    revolutions++; // Increment revolution count
  }
  
  si = sf;
  
  if (revolutions > 0) {
    unsigned long elapsed = current - initial;
    
    if (elapsed > 0) { // To avoid division by zero
      float rpm = (float)60000 / elapsed;

      Serial.print("Elapsed Time (ms): ");
      Serial.println(elapsed);
      Serial.print("RPM: ");
      Serial.println(rpm);
      
      oled.clearDisplay();
      oled.setTextSize(1);
      oled.setTextColor(SSD1306_WHITE);
      oled.setCursor(0,10);      
      oled.print("Elapsed Time (ms): ");
      oled.println(elapsed);
      oled.print("RPM: ");
      oled.println(rpm);
      oled.display();
      revolutions = 0;
      initial = 0;
      current = 0;
      delay(1000);

    }
  }

  int sensorValue = analogRead(34);
  float V2 = sensorValue * 0.0012210012210;
  float RS = 10/V2*(5-V2);
  float ratio = RS/74;
  double ppm = 116.3*pow(ratio, -2.7);

  Serial.print("Temperature: ");
  Serial.println(a);
  //Serial.print('C');
  
  Serial.print("Humidity: ");
  Serial.println(b);

  Serial.print("AQI: ");
  Serial.println(V2);
  
  Serial.print("PPM: ");
  Serial.println(ppm);

  //PINS: SCL 22, SDA 21  
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0,10);
  oled.print("Temperature:" );
  oled.println(a);
  oled.print("Humidity: ");
  oled.println(b);
  oled.print("AQI: ");
  oled.println(V2);
  oled.print("PPM: ");
  oled.println(ppm);
  oled.display();

  int f = analogRead(33);
  Serial.println(f);
  int g = map(f, 0, 4095, 0, 3.3);
  if (g < 2){
    digitalWrite(13, HIGH);
  }
  if (g >= 2){
    digitalWrite(13, LOW);
  }

  delay(1000); //dht usually takes 250 
}
