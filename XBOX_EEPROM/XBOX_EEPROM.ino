#include <Wire.h>
#include <EEPROM.h>    
 
#define EEPROM_ADDRESS 0x50    //Address of Xbox EEPROM is 0x54
#define XBOX_EEPROM_SIZE 256

unsigned int address = 0;
unsigned int tempRead = 0;
boolean receivedData = false;
 
void setup(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Wire.begin();  

//DON'T USE!!! This writes to the Xbox's EEPROM
//while(address < XBOX_EEPROM_SIZE) {
//  writeEEPROM(EEPROM_ADDRESS, address, EEPROMdata[address]);
//  address++;
//}  
  Serial.write("GO\0");
  getEEPROM();
}
 
void loop()
{
  if(Serial.available() > 0) {
    tempRead = Serial.read(); //Expects only one byte sent from PC
    receivedData = true;
    getEEPROM();
    receivedData = false;
  }
}

void getEEPROM()
{
  bool xboxDetected = testEEPROM(EEPROM_ADDRESS);
  if(xboxDetected) {
  //    Serial.print("EEPROM Found at: 0x");
  //    Serial.println(EEPROM_ADDRESS, HEX);
    address = 0;
    while(address < XBOX_EEPROM_SIZE) {
      if(receivedData == true)
        Serial.write(readEEPROM(EEPROM_ADDRESS, address)); //Send Xbox EEPROM to PC
      else
        EEPROM.write(address, readEEPROM(EEPROM_ADDRESS, address)); //Save Xbox EEPROM to Arduino
        address++;
    }
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else if(!xboxDetected && receivedData) {
    address = 0;
    while(address < XBOX_EEPROM_SIZE) {
      Serial.write(EEPROM.read(address)); //Send Saved Xbox EEPROM to PC
      address++;
    }
  }
  else {
    Serial.print("EEPROM NOT FOUND!\0");
  }   
}

int testEEPROM(int deviceaddress)
{
  Wire.beginTransmission(deviceaddress);
  return Wire.endTransmission() == 0;
}
 
void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data ) 
{
  //Set address on EEPROM to write
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data); // Write data
  Wire.endTransmission();
 
  delay(5);
}
 
byte readEEPROM(int deviceaddress, unsigned int eeaddress ) 
{
  byte rdata = 0xFF;

  //Set address on EEPROM to read
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
 
  Wire.requestFrom(deviceaddress,1);
 
  if (Wire.available())
    rdata = Wire.read();
 
  return rdata;
}

