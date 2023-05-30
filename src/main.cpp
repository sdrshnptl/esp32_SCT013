#include <Arduino.h>
#include "ArduinoJson.h"
#include <WiFi.h>
#include <AsyncUDP.h>
#include "EmonLib.h"                   // Include Emon Library

EnergyMonitor emon1;                   // Create an instance

AsyncUDP udp;

const char *ssid = "device";
const char *pass = "1223334444";

#define ct1_pin 36
#define ct2_pin 39

#define ct1_range 30
#define ct2_range 30

float ct1_reading = 0;
float ct2_reading = 0;
#define number_of_readings 2000
float ct1_readings[number_of_readings] = {0};
float ct1_averaged = 0;

float ct2_readings[number_of_readings] = {0};
float ct2_averaged = 0;

bool async_delay(unsigned long *_time, unsigned long period)
{
  if ((millis() - *_time) > period)
  {
    *_time = millis();
    return true;
  }
  return false;
}

void scan_ct1()
{
  static unsigned long time_of_ct1_scan = 0;
  if (async_delay(&time_of_ct1_scan, 500))
  {
    // float sum_of_ct1_readings = 0;
    // float sum_of_ct2_readings = 0;
    // for (int index = 0; index < number_of_readings; index++)
    // {
    //   ct1_readings[index] = analogReadMilliVolts(ct1_pin);
    //   ct2_readings[index] = analogReadMilliVolts(ct2_pin);
    //   sum_of_ct1_readings += ct1_readings[index];
    //   sum_of_ct2_readings += ct2_readings[index];
    // }
    // ct1_averaged = sum_of_ct1_readings / number_of_readings;
    // ct2_averaged = sum_of_ct2_readings / number_of_readings;
    double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  
    // Serial.print(Irms*230.0);	       // Apparent power
    // Serial.print(" ");
    // Serial.println(Irms);		       // Irms
    StaticJsonDocument<512> doc;
    // doc["ct1"] = ct1_averaged;
    // doc["ct2"] = ct2_averaged;

    doc["ApparantPower"] = (Irms*230);
    doc["Irms"] = (Irms);
    doc["ping"] = (millis() / 1000);
    String output_data;
    serializeJson(doc, output_data);
    udp.broadcastTo(output_data.c_str(), 2255);

  }
}

void setup()
{
  Serial.println(115200);
  // put your setup code here, to run once:
  pinMode(ct1_pin, INPUT);
  pinMode(ct2_pin, INPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("WiFi Failed");
    while (1)
    {
      delay(1000);
    }
  }
  emon1.current(ct2_pin, 111.1);             // Current: input pin, calibration.
}

void loop()
{
  // put your main code here, to run repeatedly:

  scan_ct1();
}
