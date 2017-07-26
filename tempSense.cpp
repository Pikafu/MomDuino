#include "Arduino.h"
#include "math.h"

uint32_t pinTempSensor = 3;
uint32_t pinBrightSensor = 2;
uint32_t pinDoorSensor = 1;
uint32_t pinBuzzer = 4;
uint32_t pinLEDSocket = 2;

// Sensor constants
const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k for Temp Sensor

// Adjustable by human?
const int TOO_HOT = 300; //in Kelvin, ~80 Fahrenheit
const int TOO_BRIGHT = 0; // Light sensor may not be sensitive enough...
const int WAKE_UP_TIME = 8; // need a time class...
const int WAKE_UP_BRIGHTNESS = 0;

typedef enum {WAKEUP, SUNSCREEN, SUNGLASSES, UMBRELLA} alert_type;

float getTemperature()
{
    int a = analogRead(pinTempSensor);
    float R = R0*1023.0/a-1.0;
    // convert to temperature via datasheet
    float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; 
    return(temperature);
}

float getBrightness()
{
    int sensorValue = analogRead(pinBrightSensor);
    float intensity = (float)(1023-sensorValue)*10/sensorValue;
    return(intensity);
}

// Sound the buzzer however many times it needs to
void buzz(int ms, int repeat)
{
	for (int i = 0; i < repeat; i++)
	{
		digitalWrite(pinBuzzer, HIGH);
		delay(ms);
		digitalWrite(pinBuzzer, LOW);
		delay(ms);
	}
}

void blinkLED(int ms)
{
	digitalWrite(pinLEDSocket, HIGH);
	delay(ms);
	digitalWrite(pinLEDSocket, LOW);
	delay(ms);
}

// Touch sensor
// float getDoorknob() {}
// float getUV(){} Include if we get UV sensor in time.
/*
void action(float temp, float brightness, boolean doorknob)
{
    if ( temp > TOO_HOT && brightness > TOO_BRIGHT && doorknob )
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

void signal( alert_type alert )
{
	switch(alert)
	{
		case WAKEUP: buzz(50,1);
		case SUNSCREEN: buzz(20,3);
		case SUNGLASSES: blinkLED(50);
		case UMBRELLA: blinkLED(100);
		default: break;
	}
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinLEDSocket, OUTPUT);
  buzz(20,3); // Works
  blinkLED(500); // Doesn't blink LED
  // TODO: Initialize WIFI/BLUETOOTH/Phone Connectivity as an output.
}

// connect with phone, LED, Speaker
void loop()
{
	delay(100);
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
    //Serial.println(temp);
    //action(temp, bright, isLeaving);
}
