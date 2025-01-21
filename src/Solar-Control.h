/********************************************************
  Bibliotheken
********************************************************/
#pragma once
#include <PCF8574.h>
#include <PZEM004T.h>
#include <TimeLib.h>
#include <NTPtimeESP.h>
#include <Preferences.h>

/********************************************************
  Funktions-Deklarationen
********************************************************/
void getData();
bool updateInterval();
void restoreParams();
void restoreOutputs();
String ConvertResetReasonToString(esp_reset_reason_t reason);

/********************************************************
  Globale Variablen
********************************************************/
extern Preferences storeParameter;

/********************************************************
  Zeitserver
********************************************************/
extern NTPtime NTPch;
extern strDateTime dateTime;
extern uint8_t actualHour;
extern uint8_t actualMinute;
extern uint8_t actualSecond;
extern uint8_t actualYear;
extern uint8_t actualMonth;
extern uint8_t actualDay;
extern uint8_t actualDayOfWeek;

/********************************************************
  PZEM004T Smart Meter

  Tx an Smart Meter --> RxD1 = Pin 5 an ESP32S3
  Rx an Smart Meter --> TxD1 = Pin 4 an ESP32S3

********************************************************/
#define MESSINTERVALL 4000
#define DEBUG_SOLAR_EAST false
#define DEBUG_SOLAR_WEST false
#define DEBUG_VALVE true

extern HardwareSerial PzemSerial;
extern PZEM004T pzem;
extern IPAddress ip;

extern volatile bool pzemrdy;

extern float Volt;
extern float Ampere;
extern float Watt;
extern float Arbeit;
extern float Waerme;

extern unsigned long previousMillis;

/********************************************************
  Deklaration Struct Solar-Regelung
********************************************************/
typedef struct
{
  volatile bool FlagPumpEast;
  volatile bool FlagPumpWest;
  volatile bool FlagLoadPump;
  volatile bool FlagValve;
  volatile bool FlagReserve5;
  volatile bool FlagReserve6;
  volatile bool FlagReserve7;
  volatile bool FlagReserve8;
  volatile bool FlagIsAutomatic;  
  volatile bool FlagFromTimerLoop;
  volatile bool FlagIsHeatProtectEast;
  volatile bool FlagIsHeatProtectWest;
  volatile bool FlagIsFreezeProtectEast;
  volatile bool FlagIsFreezeProtectWest;
  uint8_t OutputLow;
  uint8_t OutputHigh;
} LOGIC;

/********************************************************
  Deklaration Struct flüchtige Parameter
********************************************************/
typedef struct
{
  uint8_t iOldMaxLimitOffPumpEast;
  uint8_t iOldMaxLimitOnPumpEast;
  uint8_t iOldMaxLimitOffPumpWest;
  uint8_t iOldMaxLimitOnPumpWest;
  uint8_t iOldMinLimitOffPumpEast;
  uint8_t iOldMinLimitOnPumpEast;
  uint8_t iOldMinLimitOffPumpWest;
  uint8_t iOldMinLimitOnPumpWest;
  uint8_t iOldDiffOnPumpWest;
  uint8_t iOldDiffOffPumpWest;
  uint8_t iOldDiffOnPumpEast;
  uint8_t iOldDiffOffPumpEast;
  uint8_t iOldDiffOnValve;
  uint8_t iOldDiffOffValve;
  uint8_t iOldDiffOnLoadpump;
  uint8_t iOldDiffOffLoadpump;
  uint8_t iOldSelectUpLoadpump;
  uint8_t iOldSelectDownLoadpump;
  uint8_t iOldTempLoadpump;
  uint8_t iOldValveRelease;
  uint8_t iOldOverOffCollectorEast;
  uint8_t iOldOverOnCollectorEast;
  uint8_t iOldOverOffCollectorWest;
  uint8_t iOldOverOnCollectorWest;
  uint8_t iOldCoolingEast;
  uint8_t iOldCoolingWest;
  uint8_t iOldReleaseHourTenEast;
  uint8_t iOldReleaseHourOneEast;
  uint8_t iOldReleaseMinTenEast;
  uint8_t iOldReleaseMinOneEast;
  uint8_t iOldReleaseHourTenWest;
  uint8_t iOldReleaseHourOneWest;
  uint8_t iOldReleaseMinTenWest;
  uint8_t iOldReleaseMinOneWest;
  uint8_t iOldBlockHourTenEast;
  uint8_t iOldBlockHourOneEast;
  uint8_t iOldBlockMinTenEast;
  uint8_t iOldBlockMinOneEast;
  uint8_t iOldBlockHourTenWest;
  uint8_t iOldBlockHourOneWest;
  uint8_t iOldBlockMinTenWest;
  uint8_t iOldBlockMinOneWest;  
  int8_t iOldColdOffCollectorEast;
  int8_t iOldColdOnCollectorEast;
  int8_t iOldColdOffCollectorWest;
  int8_t iOldColdOnCollectorWest;  
} TEMPORARY;

/********************************************************
  Deklaration Struct persistente Parameter
********************************************************/
typedef struct
{
  uint8_t iMaxLimitOffPumpEast;
  uint8_t iMaxLimitOnPumpEast;
  uint8_t iMaxLimitOffPumpWest;
  uint8_t iMaxLimitOnPumpWest;
  uint8_t iMinLimitOffPumpEast;
  uint8_t iMinLimitOnPumpEast;
  uint8_t iMinLimitOffPumpWest;
  uint8_t iMinLimitOnPumpWest;
  uint8_t iDiffOnPumpEast;
  uint8_t iDiffOffPumpEast;
  uint8_t iDiffOnPumpWest;
  uint8_t iDiffOffPumpWest;
  uint8_t iDiffOnLoadpump;
  uint8_t iDiffOffLoadpump;
  uint8_t iSelectUpLoadpump;
  uint8_t iSelectDownLoadpump;
  uint8_t iTempLoadpump;
  uint8_t iDiffOnValve;
  uint8_t iDiffOffValve;
  uint8_t iValveRelease;
  uint8_t iOverOffCollectorEast;
  uint8_t iOverOnCollectorEast;
  uint8_t iOverOffCollectorWest;
  uint8_t iOverOnCollectorWest;
  uint8_t iCoolingEast;
  uint8_t iCoolingWest;
  int8_t iColdOffCollectorEast;
  int8_t iColdOnCollectorEast;
  int8_t iColdOffCollectorWest;
  int8_t iColdOnCollectorWest;  
} PARAMETER;

/********************************************************
  Deklaration Struct persistente Schaltuhren
********************************************************/
typedef struct
{
  uint8_t iReleaseHourTenEast;
  uint8_t iReleaseHourOneEast;
  uint8_t iReleaseMinTenEast;
  uint8_t iReleaseMinOneEast;
  uint8_t iBlockHourTenEast;
  uint8_t iBlockHourOneEast;
  uint8_t iBlockMinTenEast;
  uint8_t iBlockMinOneEast;
  uint8_t iReleaseHourTenWest;
  uint8_t iReleaseHourOneWest;
  uint8_t iReleaseMinTenWest;
  uint8_t iReleaseMinOneWest;
  uint8_t iBlockHourTenWest;
  uint8_t iBlockHourOneWest;
  uint8_t iBlockMinTenWest;
  uint8_t iBlockMinOneWest;
} TIMER;

/********************************************************
  PCF8574 I/O-Expander
********************************************************/
extern TwoWire I2C;
extern PCF8574 pcf8574;
#define SDA 9
#define SCL 8
