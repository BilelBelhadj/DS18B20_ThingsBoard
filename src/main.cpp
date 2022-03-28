/********************************************************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
#include "WIFIConnector_MKR1000.h"
#include "MQTTConnector.h"
#include <sstream>
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2 


int DS1 = 0, DS2 = 1 ;
String adrs, macStr ;
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/ 

DeviceAddress insideThermometer, outsideThermometer;

/*
std::string convert_int(DeviceAddress n)
{
   std::stringstream ss;
   ss << n;
   return ss.str();
}

*/

void printAddress(DeviceAddress deviceAddress)
{ 
  adrs = "";
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    adrs += String(deviceAddress[i], HEX);
  }
  Serial.println();
  Serial.println(adrs);
}



void setup(void) 
{ 

  wifiConnect();                  //Branchement au réseau WIFI
  MQTTConnect();                  //Branchement au broker MQTT
  // start serial port 
  Serial.begin(9600); while (!Serial)
  {
    /* code */
  }
  
  Serial.println("Dallas Temperature IC Control Library Demo"); 

  sensors.begin(); 
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(outsideThermometer, 1)) Serial.println("Unable to find address for Device 1");

  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  Serial.print("Device 1 Address: ");
  printAddress(outsideThermometer);
  Serial.println();


  byte mac[6];
  WiFi.macAddress(mac);
  macStr = printMacAddress(mac);
  Serial.println("macAdresee : ");
  Serial.println(macStr);

  appendPayloadStr("adresse mac", macStr);
  sendPayloadStr();

} 



void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  printAddress(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC));
  appendPayload(adrs, tempC);  //Ajout de l'etat de climatisation au message MQTT
  sendPayload();  
}
void loop(void) 
{ 


// code temperature by index

  sensors.requestTemperatures(); //to issue a global temperature 
 // request to all devices on the bus 
/********************************************************************/
  Serial.print(" Requesting temperatures..."); 
  sensors.requestTemperatures(); // Send the command to get temperature readings 
  Serial.println("DONE"); 

  printTemperature(insideThermometer);
  printTemperature(outsideThermometer);
/********************************************************************/
 Serial.println("Temperature 1 is: "); 
  Serial.println(sensors.getTempCByIndex(DS1)); // Why "byIndex"?  
   // You can have more than one DS18B20 on the same bus.  
   // 0 refers to the first IC on the wire 

  Serial.println("Temperature 2 is: "); 

  Serial.println(sensors.getTempCByIndex(DS2));



  delay(3000);
   
} 