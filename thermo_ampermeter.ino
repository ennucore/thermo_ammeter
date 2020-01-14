#include <OneWire.h>
/*#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
*/
OneWire ds(2);
float temps[100];
int delta = 50;

#define kilogram 1
#define joule 1
#define degree 1
#define ampere 1
#define volt 1


float temperature() {
  byte data[12];
  byte i;
  byte addr[8];
  byte present = 0;
  
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return 0;
  }  
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);
    present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  int16_t raw = (data[1] << 8) | data[0];
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  
  float temp = (float)raw / 16.0;

  return temp;
}


float heatCapacity(float temp) {
  return 200 * joule/degree; // todo
}


float resistance(float temp) {
  return 1000 * volt/ampere; // todo
}

float heatLoss(float temp) {
  return 0; // todo
}


void setup() {
  Serial.begin(9600);
  /*display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();*/
  pinMode(A2, INPUT);
}


void loop() {
  float temp = temperature();
  // shift the array left and add new value to it
  for(int i=0;i<99;i++){
    temps[i] = temps[i+1];
  }
  temps[99] = temp;
  delay(delta);
  Serial.print(temp);
  Serial.print(" ");
  float temperatureDerivative = 50*(temps[99]-temps[79])/delta;
  Serial.print(temperatureDerivative);
  Serial.print(" ");
  float current = sqrt((heatCapacity(temp)*temperatureDerivative + heatLoss(temp))/resistance(temp));
  Serial.println(current);
  // display.print(temp);
}
