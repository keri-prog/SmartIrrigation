#include <math.h>
#include <string.h> 
#include <dht.h>
#include <wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP280.h>

int Tx = 2;       // connect BT module TX to 2
int Rx = 3;       // connect BT module RX to 3
int soilSensorPin = A1; 
int waterSensorPin = A2; 
int soilSensorValue;  

#define dht_apin A0
#define sensorPower 7
dht DHT;
Adafruit_BMP280 bmp; 

SoftwareSerial bluetooth(Tx, Rx);

void setup() { 
    Serial.begin(9600); 
    bluetooth.begin(9600);
    delay(500);
    Serial.println("DHT11 Humidity & temperature Sensor\n\n");
    delay(1000);

    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

} 

int readSensor() {
	digitalWrite(sensorPower, HIGH);	// Turn the sensor ON
	delay(10);							// wait 10 milliseconds
	val = analogRead(sensorPin);		// Read the analog value form sensor
	digitalWrite(sensorPower, LOW);		// Turn the sensor OFF
	return val;							// send current reading
}

void loop() { 
    DHT.read11(dht_apin);
    soilSensorValue = analogRead(soilSensorPin);
    int level = readSensor();
    Serial.print(DHT.humidity);
    Serial.print(DHT.temperature); 
    Serial.print(bmp.readPressure()/100);
    Serial.print(bmp.readTemperature());
    Serial.print(level);
    
    if (sensorValue<limit) {
      Serial.print("Soil is moist");
    }
    else {
      Serial.print("Soil is not moist");
    }
    
    if (bluetooth.available()) {
      
      value = bluetooth.read();
     
      if (value =='0') {
        digitalWrite(blueLED, HIGH);
        bluetooth.println("Rain not expected");
      }
      
      if (value =='2') {
        digitalWrite(blueLED, LOW);
        bluetooth.println("Rain Expected");
      }
      
      if (value =='3') {
        digitalWrite(redLED, HIGH);
        bluetooth.println("Red LED on");
      }
      
      if (value =='4') {
        digitalWrite(redLED, LOW);
        bluetooth.println("Red LED off");
      }
    }

    delay(5000);  

}