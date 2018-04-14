#include <Wire.h> 
#include <SoftwareSerial.h>
#include <idDHTLib.h>
#include "StableButton.h"

#define BT1 A3
#define BT2 9
#define BT3 8
#define BT4 A2

uint8_t RelayPinArray[] = {4, 5, 6, 7} ;
uint8_t ButtonPinArray[] = {BT1, BT2, BT3, BT4};

String RelayState = "0000R";

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

String _inputString = "";         // a string to hold incoming data
boolean _stringComplete = false;  // whether the string is complete

SoftwareSerial _Serial(10,11); //RX & TX

int idDHTLibPin = 2; //Digital pin for comunications
int idDHTLibIntNumber = 0; //interrupt number (must be the one that use the previus defined pin (see table above)

//declaration
void dhtLib_wrapper(); // must be declared before the lib initialization

// Lib instantiate
idDHTLib DHTLib(idDHTLibPin,idDHTLibIntNumber,dhtLib_wrapper);

uint32_t LastTime = 0;

void setup() 
{
  Serial.begin(9600);
  while (!Serial) {}
  _Serial.begin(9600);
  _Serial.println(RelayState);
  
  inputString.reserve(200);

  InitPinMode(); 
  StableButton.Init(ButtonPinArray, 4);
}

void loop() 
{
  serialEvent(); 
  
  if (stringComplete) 
  {
    Serial.println(inputString);

    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  if (_stringComplete) 
  {
    Serial.println(_inputString);

    for (uint8_t i = 0; i < 4; i++)
    {
      if (_inputString.substring(0, 3) == String(i) + "_1")
      {
        digitalWrite(RelayPinArray[i], LOW);
        RelayState[i] = '1';
        //_Serial.println(RelayState);
      }
      else if (_inputString.substring(0, 3) == String(i) + "_0")
      {
        digitalWrite(RelayPinArray[i], HIGH);
        RelayState[i] = '0';
        //_Serial.println(RelayState);
      }
    }

    if (_inputString.substring(0, 2) == "gR")    // request relay state, gR = get relay
    {
      _Serial.println(RelayState);
    }
      
    // clear the string:
    _inputString = "";
    _stringComplete = false;
  }

  if(millis() - LastTime > 4999)
  {
    LastTime = millis();
    //ReadAndSendValueSensor();
  }

  CheckButton();
  
}

void serialEvent() 
{
  while (Serial.available()) 
  {
    char inChar = (char)Serial.read();
    inputString += inChar;
    
    if (inChar == '\n') 
    {      
      stringComplete = true;
    }
  }

  while (_Serial.available()) 
  {
    char inChar = (char)_Serial.read();
    _inputString += inChar;
    
    if (inChar == '\n') 
    {      
      _stringComplete = true;
    }
  }
}

void dhtLib_wrapper() {
  DHTLib.dht11Callback();
}

void InitPinMode()
{
  for (uint8_t i = 0; i < 4; i++)
  {
    pinMode(RelayPinArray[i], OUTPUT);
    digitalWrite(RelayPinArray[i], HIGH);
  }
}

void CheckButton()
{
  for (uint8_t i = 0; i < 4; i++)
  {
    if( StableButton.IsPressed(ButtonPinArray[i]))
    {
      if( digitalRead(RelayPinArray[i]) == HIGH )
      {
        digitalWrite(RelayPinArray[i], LOW);
        _Serial.println(String(i) + "_1");
        RelayState[i] = '1';
      }
      else
      {
        digitalWrite(RelayPinArray[i], HIGH);
        _Serial.println(String(i) + "_0");
        RelayState[i] = '0';
      }    
      
    }
  }
}

void ReadAndSendValueSensor()
{
  DHTLib.acquire();
  while (DHTLib.acquiring())
  {
    ;
  }
  _Serial.print("H");
  _Serial.print(DHTLib.getHumidity(), 2);
  _Serial.print("-");
  _Serial.print("T");
  _Serial.println(DHTLib.getCelsius(), 2); 

  Serial.print("H");
  Serial.print(DHTLib.getHumidity(), 2);
  Serial.print("-");
  Serial.print("T");
  Serial.println(DHTLib.getCelsius(), 2); 
}


