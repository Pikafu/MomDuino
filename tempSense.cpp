//#include <math.h>
#include "Arduino.h"

uint32_t pinTempSensor = 3;
uint32_t pinBrightSensor = 2;
uint32_t pinDoorSensor = 1;

// Sensor constants
const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k for Temp Sensor

// Adjustable by human?
const int TOO_HOT = 300; //in Kelvin, ~80 Fahrenheit
const int TOO_BRIGHT = 0; // Light sensor may not be sensitive enough...
const int WAKE_UP_TIME = 8; // need a time class...
const int WAKE_UP_BRIGHTNESS = 0;

typedef enum {WAKEUP, SUNSCREEN, SUNGLASSES, UMBRELLA} alert_type;

/*
void setup()
{
	// initialize digital pin LED_BUILTIN as an output.
	pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
	digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
	delay(1000);                       // wait for a second
	digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
	delay(1000);
}
*/

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // TODO: Initialize digital pin SPEAKER as an output.
  // TODO: Initialize WIFI/BLUETOOTH/Phone Connectivity as an output.
}

float getTemperature()
{
    int a = analogRead(pinTempSensor);
    float R = R0*1023.0/a-1.0;
    // convert to temperature via datasheet
    float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; 
    return(temperature);
}

float getBrightness(){
    int sensorValue = analogRead(pinBrightSensor);
    float intensity = (float)(1023-sensorValue)*10/sensorValue;
    return(intensity);
}

//float getDoorknob(){}
//float getUV(){} //Include if we get UV sensor in time.

/*void action(float temp, float brightness, boolean doorknob)
{
    if( temp > TOO_HOT && brightness > TOO_BRIGHT && doorknob )
    {
        signal(SUNSCREEN);
    } else if( brightness > WAKE_UP_BRIGHTNESS && clock > WAKE_UP_TIME )
    {
        signal(WAKEUP);
    } else if( isRaining() )
    {
        signal(UMBRELLA);
    }
}
*/

void signal( alert_type alert ){} 
// connect with phone, LED, Speaker


void loop()
{
    float temp = getTemperature();
    float bright = getBrightness();
    //boolean isLeaving = getDoorknob();
    Serial.print(temp);
    Serial.print("\t");
    Serial.print(" deg C");
    Serial.print("\t");
    Serial.print(bright);
    Serial.print("\t");
    Serial.print(" bright");
    Serial.print("\n");
    delay(100);
    //Serial.println(temp);
//    action(temp, bright, isLeaving);
}
