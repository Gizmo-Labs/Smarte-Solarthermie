/********************************************************
  Bibliotheken
********************************************************/
#pragma once
#include <DallasTemperature.h>
#include <Adafruit_MAX31865.h>
#include "driver/temp_sensor.h"
#include <SPI.h>

/********************************************************
  Funktions-Deklarationen Temperaturmessung
 ********************************************************/
void initDS18B20();
void initMAX();
void TempLoop(long now);
String ConvertFaultToString();

/********************************************************
  1-Wire Temperaturmessung
********************************************************/
#define ONE_WIRE_BUS 14
#define ONE_WIRE_MAX_DEV 5

extern OneWire oneWire;
extern DallasTemperature DS18B20;
extern DeviceAddress devAddr[];

extern float fTemp_Snapshot;
extern float fTemp_Ruecklauf;
extern float fTemp_Vorlauf;
extern float fTemp_Warmwasser;
extern float fTemp_Puffer_Unten;
extern float fTemp_Puffer_Mitte;
extern float fTempDev[ONE_WIRE_MAX_DEV];
extern float fTempDevLast[ONE_WIRE_MAX_DEV];
extern float fTempChip;

extern int iNumberOfDevices;
extern long lLastTemp;

/********************************************************
  MAX31865 Widerstands-Temperaturmessung

  Default Pins für Hardware-SPI sind folgende:

  CLK an MAX31865 Platine 1 --> Pin 13 an ESP32S3 (SCK)
  SDO an MAX31865 Platine 1 --> Pin 12 an ESP32S3 (MOSI)
  SDI an MAX31865 Platine 1 --> Pin 11 an ESP32S3 (MISO)

  Bei Verwendung von Hardware-SPI muss nur noch CS-Pin
  zugewiesen werden, siehe unten 10 + 15

  Jedes Board benötigt eigenen ChipSelect Pin!
  Pins CLK, SDO und SDI kann man parallel schalten!

  CS an MAX31865 Platine 1 --> Pin 10 an ESP32S3
  CS an MAX31865 Platine 2 --> Pin 15 an ESP32S3

 ********************************************************/
#define RNOMINAL 1000.0

extern Adafruit_MAX31865 Collector_West;
extern Adafruit_MAX31865 Collector_East;

extern float fTemp_Collector_East;
extern float fLast_Temp_Collector_East;
extern float fTemp_Collector_West;
extern float fLast_Temp_Collector_West;

extern int8_t iFault_West;
extern int8_t iFault_East;
