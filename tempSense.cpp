#include "Arduino.h"
#include "math.h"
#include "CurieTime.h"

uint32_t pinTempSensor = 3;
uint32_t pinBrightSensor = 2;
uint32_t pinDoorSensor = 1;
uint32_t pinBuzzer = 4;
uint32_t pinLEDSocket = 2;
uint32_t pinTouch = 5;
uint32_t pinWater = 6;

// Sensor constants
const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k for Temp Sensor

// Adjustable by human?
const int TOO_HOT = 20; // Celsius
const int TOO_BRIGHT = 6; // 5.64 is lamp about 3 feet away
const int WAKE_UP_TIME = 8; // need a time class...
const int WAKE_UP_BRIGHTNESS = 10;

typedef enum {WAKEUP, SUNSCREEN, SUNGLASSES, UMBRELLA} alert_type;

float getTemperature()
{
    int a = analogRead(pinTempSensor);
    float R = R0*1023.0/a-1.0;
    // convert to temperature via datasheet
    float temperature = 1.0/(log10(R/R0)/B+1/298.15)-273.15;
    return(temperature);
}

float getBrightness()
{
    int sensorValue = analogRead(pinBrightSensor);
    float intensity = (float)(1023-sensorValue)*10/sensorValue;
    return(intensity);
}

boolean getDoorknob()
{
  return(digitalRead(pinTouch));
}

void soundBuzzer(int ms, int repeat)
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
	digitalWrite(LED_BUILTIN, HIGH);
	delay(ms);
	digitalWrite(pinLEDSocket, LOW);
	digitalWrite(LED_BUILTIN, LOW);
	delay(ms);
}

boolean getWater()
{
	return digitalRead(pinWater);
}

void signal( alert_type alert )
{
	switch(alert)
	{
		case WAKEUP: soundBuzzer(500,4);
		case SUNSCREEN: soundBuzzer(250,2);
		case SUNGLASSES: soundBuzzer(250,3);
		case UMBRELLA: soundBuzzer(2000,1);
		default: break;
	}
}

// float getUV(){} Include if we get UV sensor in time.
void action(float temp, float brightness, boolean water, boolean doorknob)
{
    if ( temp > TOO_HOT && brightness < TOO_BRIGHT && doorknob )
    {
        signal(SUNSCREEN);
        signal(SUNGLASSES);
    } else if( brightness < WAKE_UP_BRIGHTNESS && hour() > WAKE_UP_TIME )
    {
        signal(WAKEUP);
    } else if( water && doorknob )
    {
        signal(UMBRELLA);
    }
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinLEDSocket, OUTPUT);
  pinMode(pinWater, INPUT);
  pinMode(pinTouch, INPUT);
  soundBuzzer(20,3); // Works
  blinkLED(500); // Doesn't blink LED
  // TODO: Initialize WIFI/BLUETOOTH/Phone Connectivity as an output.
}

// connect with phone, LED, Speaker
void loop()
{
  delay(500);
  float temp = getTemperature();
  float bright = getBrightness();
  boolean isWater = getWater();
  boolean isLeaving = getDoorknob();
  action(temp, bright, isWater, isLeaving);
  //Serial.print("Water=");
  //Serial.print("\t");
  //Serial.println(isWater);
  //Serial.print("Doorknob=");
  //Serial.print("\t");
  //Serial.println(isLeaving);
  //Serial.print(hour());
  //Serial.print("\t");
  //Serial.print(minute());
  //Serial.print("\t");
  //Serial.println(second());
  Serial.print(temp);
  Serial.print("\t");
  Serial.println(bright);
}
