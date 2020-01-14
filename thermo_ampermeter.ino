#include <OneWire.h>

OneWire ds(2);
float temps[100];
int delta = 50;


float temperature() {
  byte data[12];
  byte i;
  byte addr[8];
  byte present = 0;
  
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
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

void setup() {
  Serial.begin(9600);
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
  Serial.println(50*(temps[99]-temps[79])/delta);
  // 
}
