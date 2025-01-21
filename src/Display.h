/********************************************************
  Bibliotheken
********************************************************/
#pragma once
#include <EasyNextionLibrary.h>
#include <HardwareSerial.h>

/********************************************************
  Funktions-Deklarationen
********************************************************/
void initDisplayInterface();
void initDisplayContent();
void digitalClock();

/********************************************************
  Nextion Display
********************************************************/
#define DISPLAY_REFRESH_RATE 50

extern HardwareSerial Nex2;
extern EasyNex myNex;

extern unsigned long pageRefreshTimer;
extern volatile bool newPageLoaded;

extern String sDisplay_Prozent;
extern String sDisplay_Dim;

extern const char *cDisplay_Prozent;
extern const char *cDisplay_Dim;

extern uint8_t iDisplay_Prozent;
extern uint8_t iDisplay_Dim;
