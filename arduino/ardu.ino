#include <math.h>
#include <string.h>
#include "DHT.h"
#include <Servo.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP280.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define waterSensorPower 7
#define waterSensorPin A0
#define soilSensorPin A3
#define servoPin 5
#define Tx 2
#define Rx 3

Servo myservo;
Adafruit_BMP280 bmp;
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial bluetooth(Tx, Rx);

void setup()
{

  myservo.attach(9);

  pinMode(waterSensorPower, OUTPUT);
  digitalWrite(waterSensorPower, LOW);

  pinMode(soilSensorPower, OUTPUT);

  Serial.begin(9600);
  bluetooth.begin(9600);

  bmp.begin(0x76);
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  dht.begin();
}

int readWaterLevel()
{
  digitalWrite(waterSensorPower, HIGH);
  delay(10);
  int val = analogRead(waterSensorPin);
  digitalWrite(waterSensorPower, LOW);
  return val;
}

int readSoilWetness()
{
  delay(10);
  int val = analogRead(soilSensorPin);
  return val;
}

void loop()
{
  delay(5000);

  if (bluetooth.available())
  {
    int value = bluetooth.read();
    bluetooth.println("Read value %d", value);
  }

  String command;
  command = Serial.readStringUntil('\n');
  command.trim();

  bool wet, rain;
  int level = readWaterLevel();

  if (command == "rain")
    rain = true;
  else
    rain = false;

  if (level > 50)
  {
    prevWet = wet;
    wet = true;
  }
  else
  {
    prevWet = wet;
    wet = false;
  }

  if (wet ^ prevWet)
  {
    if (prevWet && rain)
    {
      for (pos = 0; pos <= 180; pos += 1)
      {
        myservo.write(pos);
        delay(15);
      }
    }
    else
    {
      for (pos = 180; pos >= 0; pos -= 1)
      {
        myservo.write(pos);
        delay(15);
      }
    }
  }

  Serial.print("Soil Wet: ");
  Serial.println(level);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(t);
  Serial.print(" ");
  Serial.print(bmp.readPressure() / 100);
  Serial.print(" ");
  Serial.print(h);
  Serial.print(" ");
  Serial.println(level);
}