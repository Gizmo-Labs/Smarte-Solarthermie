/********************************************************
  Bibliotheken
********************************************************/
#include "Prototypes.h"

/********************************************************
  Externe Referenzen
********************************************************/
extern LOGIC *logic_data_t;

/********************************************************
  Nextion Display
********************************************************/
HardwareSerial Nex2(2);
EasyNex myNex(Nex2);

unsigned long pageRefreshTimer = millis();

String sDisplay_Prozent = "";
String sDisplay_Dim = "";

const char *cDisplay_Prozent = "";
const char *cDisplay_Dim = "";

uint8_t iDisplay_Prozent = 100;
uint8_t iDisplay_Dim = 5;

volatile bool newPageLoaded = false;

/********************************************************
    Serielle Schnittstelle + Display initialisieren

    Tx an Stecker von Display --> RxD2 = Pin 16 an ESP32S3
    Rx an Stecker von Display --> TxD2 = Pin 17 an ESP32S3

********************************************************/
void initDisplayInterface()
{
  Nex2.begin(115200, SERIAL_8N1, 16, 17);
  myNex.begin(115200);
}

/********************************************************
  Startparameter für Display
  Starte immer mit der Übersicht
  Lese zuletzt genutzte Betriebsart aus Flash
 ********************************************************/
void initDisplayContent()
{
  myNex.writeNum("page7.sw0.val", storeParameter.getBool("IsAutomatic", true));
  myNex.writeNum("page8.sw0.val", storeParameter.getBool("IsHeatPrtEast", true));
  myNex.writeNum("page8.sw1.val", storeParameter.getBool("IsHeatPrtWest", true));
  myNex.writeNum("page9.sw0.val", storeParameter.getBool("IsFreezePrtEast", true));
  myNex.writeNum("page9.sw1.val", storeParameter.getBool("IsFreezePrtWest", true));

  // Schreibe in Struct zurück
  logic_data_t->FlagIsAutomatic = storeParameter.getBool("IsAutomatic", true);
  logic_data_t->FlagIsHeatProtectEast = storeParameter.getBool("IsHeatPrtEast", true);
  logic_data_t->FlagIsHeatProtectWest = storeParameter.getBool("IsHeatPrtWest", true);
  logic_data_t->FlagIsFreezeProtectEast = storeParameter.getBool("IsFreezePrtEast", true);
  logic_data_t->FlagIsFreezeProtectWest = storeParameter.getBool("IsFreezePrtWest", true);

  logic_data_t->FlagFromTimerLoop = false;
  logic_data_t->FlagReserve5 = false;
  logic_data_t->FlagReserve6 = false;
  logic_data_t->FlagReserve7 = false;
  logic_data_t->FlagReserve8 = false;
  
  sendLogic();
  myNex.writeStr("page 0");
  myNex.writeStr("ScreenDim.val=0");
}

/********************************************************
  Digitaluhr
 ********************************************************/
void digitalClock()
{
  myNex.writeNum("ClockHourTen.val", actualHour / 10);
  myNex.writeNum("ClockHourOne.val", actualHour % 10);

  myNex.writeNum("ClockMinuteTen.val", actualMinute / 10);
  myNex.writeNum("ClockMinuteOne.val", actualMinute % 10);
}
