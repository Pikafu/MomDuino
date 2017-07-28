#include "Arduino.h"
#include "math.h"
#include "CurieTime.h"
#include "rgb_lcd.h"

// Define pins
uint32_t pinTempSensor = 3;
uint32_t pinBrightSensor = 2;
uint32_t pinDoorSensor = 1;
uint32_t pinLEDToggle = 3;
uint32_t pinBuzzer = 4;
uint32_t pinTouch = 5;
uint32_t pinWater = 6;

// Sensor constants
const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k for Temp Sensor

// Detection Thresholds
const int TOO_HOT = 72; // Farenheit
const int TOO_BRIGHT = 1; // 5.64 is lamp about 3 feet away
const int WAKE_UP_TIME = 8; // AM
const int WAKE_UP_BRIGHTNESS = 10;

// Prevent alarm from sounding off twice
int wakeupFlag = 0;

// LCD display
rgb_lcd lcd;

typedef enum {WAKEUP, SUNGLASSES, UMBRELLA} alert_type;

float getTemperature()
{
    int a = analogRead(pinTempSensor);
    float R = R0*1023.0/a-1.0;
    // convert to temperature via datasheet
    float temperature = 1.8*(1.0/(log(R/R0)/B+1/298.15)-273.15)+32;
    return(temperature);
}

// Brightness is now on log scale
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

boolean getWater()
{
	return digitalRead(pinWater);
}

void dispLCDTempBrightness(float temp, float bright)
{
	// Cast floats to ints for display
	//int intTemp = static_cast<int>(temp);
	int intBright = static_cast<int>(bright);
	// Concat
	lcd.clear();
	lcd.setCursor(0,0);
	//lcd.print("Temp:    "); // right justify temp reading
	lcd.print(temp);
	lcd.print(char(223)); // degrees
	lcd.print("F  ");
	lcd.print(hour());
	lcd.print(":");
	lcd.print(minute());
	lcd.print(":");
	lcd.print(second());
	lcd.setCursor(0,1);
	lcd.print("Bright:  ");
	lcd.print(bright);
}

void flashLCD(int duration, int repeat, int color, String row1, String row2)
{
	lcd.clear();
	for (int i = 0; i < repeat; i++)
	{
		lcd.setCursor(0,0);
		lcd.setColor(color);
		lcd.print(row1);
		lcd.setCursor(0,1);
		lcd.print(row2);
		delay(duration);
		lcd.setColorWhite();
		lcd.clear();
		delay(duration);
	}
}

void signal( alert_type alert )
{
	String rem1 = "DON'T FORGET";
	String rem2 = "WATER & SUNGLASSES!";
	String rem3 = "YOUR UMBRELLA!";
	String rem4 = "GOOD MORNING!";
	String rem5 = "RISE & SHINE!";
	switch(alert)
	{
		case WAKEUP:
			soundBuzzer(500,5);
			flashLCD(500,5,WHITE,rem4,rem5);
			break;
		case SUNGLASSES:
			soundBuzzer(100,10);
			flashLCD(500,5,RED,rem1,rem2);
			break;
		case UMBRELLA:
			soundBuzzer(1000,1);
			flashLCD(500,5,BLUE,rem1,rem3);
			break;
		default:
			break;
	}
}

// float getUV(){} Include if we get UV sensor in time.
void action(float temp, float brightness, boolean water, boolean doorknob) {
    if ( temp > TOO_HOT && brightness < TOO_BRIGHT && doorknob ) {
    	Serial.println("Sunglasses");
        signal(SUNGLASSES);
    } else if ( brightness < WAKE_UP_BRIGHTNESS && hour() >= WAKE_UP_TIME ) {
    	Serial.println("Wakeup");
    	digitalWrite(pinLEDToggle, HIGH);
    	if (!wakeupFlag) {
    		wakeupFlag = 1;
    		signal(WAKEUP);
    	}
    } else if ( water && doorknob ) {
    	Serial.println("Umbrella");
        signal(UMBRELLA);
    }
}

// Setup input and output pins
void setupPins()
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(pinBuzzer, OUTPUT);
	pinMode(pinWater, INPUT);
	pinMode(pinTouch, INPUT);
	pinMode(pinLEDToggle, OUTPUT);
}

// LCD Intro messsage
void setupLCD()
{
	lcd.begin(16,2);
	lcd.setColorWhite(); // start off white
    lcd.print("MomDuino is");
    lcd.setCursor(0,1);
    lcd.print("watching you ...");
    lcd.setCursor(0,0);
    delay(5000); // Delay before MomDuino starts displaying values
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  setTime(1501142300); // Set "Alarm" to sound 1 min. after power up
  setupPins();
  setupLCD();
  soundBuzzer(40,3); // Initialization cue
}

void printStatus(float temp, float bright, bool isWater, bool isLeaving)
{
	//Serial.print("Water=");
	//Serial.print("\t");
	//Serial.println(isWater);
	//Serial.print("Doorknob=");
	//Serial.print("\t");
	//Serial.println(isLeaving);
	Serial.print(hour());
	Serial.print("\t");
	Serial.print(minute());
	Serial.print("\t");
	Serial.println(second());
	Serial.print(temp);
	Serial.print("\t");
	Serial.print(bright);
	Serial.print("\t");
	Serial.print("Rain: ");
	Serial.print(isWater);
	Serial.print("\t");
	Serial.print("Door: ");
	Serial.println(isLeaving);
}

// Display temp and brightness on LCD
void loop()
{
  delay(250);
  float temp = getTemperature();
  float bright = getBrightness();
  boolean isWater = getWater();
  boolean isLeaving = getDoorknob();
  lcd.clear();
  dispLCDTempBrightness(temp, bright);
  action(temp, bright, isWater, isLeaving);
  printStatus(temp, bright, isWater, isLeaving);
}
