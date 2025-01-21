/********************************************************
  Bibliotheken
********************************************************/
#include "Prototypes.h"

/********************************************************
  Globale Variablen
********************************************************/
Preferences storeParameter;

/********************************************************
  Structs auf PSRAM allokieren
********************************************************/
LOGIC *logic_data_t = (LOGIC *)heap_caps_malloc(sizeof(LOGIC), MALLOC_CAP_SPIRAM);
PARAMETER *parameter_data_t = (PARAMETER *)heap_caps_malloc(sizeof(PARAMETER), MALLOC_CAP_SPIRAM);
TEMPORARY *temporary_data_t = (TEMPORARY *)heap_caps_malloc(sizeof(TEMPORARY), MALLOC_CAP_SPIRAM);
TIMER *timer_data_t = (TIMER *)heap_caps_malloc(sizeof(TIMER), MALLOC_CAP_SPIRAM);

/********************************************************
  Zeitserver
********************************************************/
NTPtime NTPch("fritz.box");

strDateTime dateTime;
uint8_t actualHour;
uint8_t actualMinute;
uint8_t actualSecond;
uint8_t actualYear;
uint8_t actualMonth;
uint8_t actualDay;
uint8_t actualDayOfWeek;

/********************************************************
  PZEM004T Smart Meter

  Tx an Smart Meter --> RxD1 = Pin 5 an ESP32S3
  Rx an Smart Meter --> TxD1 = Pin 4 an ESP32S3

********************************************************/
HardwareSerial PzemSerial(1);
PZEM004T pzem(&PzemSerial, 4, 5);
IPAddress ip(192, 168, 1, 1);

float Volt = 0.0;
float Ampere = 0.0;
float Watt = 0.0;
float Arbeit = 0.0;
float Waerme = 0.0;

unsigned long previousMillis = 0;

volatile bool pzemrdy = false;

/********************************************************
  PCF8574 I/O-Expander
********************************************************/
TwoWire I2C = TwoWire(0);
PCF8574 pcf8574(&I2C, 0x20);

/********************************************************
  I2C Schnittstelle initialisieren
  ********************************************************/
void initTwoWire()
{
  I2C.begin(9, 8, 400000U);
  delay(1000);

  // Alle 8 Ports als Digitale Ausgänge deklarieren
  for (int i = 0; i < 8; i++)
  {
    pcf8574.pinMode(i, OUTPUT);
    pcf8574.digitalWrite(i, logic_data_t->OutputLow);
  }

  if (pcf8574.begin())
  {
    buildStatusJson("PCF8574 gestartet!");
    delay(2000);
  }
  else
  {
    buildStatusJson("PCF8574 nicht gestartet!");
    delay(2000);
  }
}

/*********************************************************
   Starte Setup
********************************************************/
void setup()
{
  /********************************************************
    Serielle Schnittstelle für Debugging initialisieren
  ********************************************************/
  Serial.begin(115200);
  Serial.println(ARDUINO_BOARD);

  /********************************************************
    Display-Schnittstelle initialisieren
  ********************************************************/
  initDisplayInterface();

  /********************************************************
    MQTT initialisieren
  ********************************************************/
  initMqtt();

  /********************************************************
    WiFi-Verbindung starten
  ********************************************************/
  WiFiStart();

  /********************************************************
    DS18B20-Sensor initalisieren
  ********************************************************/
  initDS18B20();

  /********************************************************
    Port-Expander initalisieren
  ********************************************************/
  initTwoWire();

  /********************************************************
     Starte erst Hardware-SPI und dann die MAX31865-Boards
  ********************************************************/
  initMAX();

  /********************************************************
    Start OTA-Service
  ********************************************************/
  //MDNS.addService("http", "tcp", 80);

  initWebserver();
  initFileSystem();

  /********************************************************
    Start PZEM004T
  ********************************************************/
  int pzem_count = 0;

  while ((!pzemrdy) && pzem_count < 9)
  {
    pzem_count++;
    pzemrdy = pzem.setAddress(ip);
    delay(1000);
  }

  if ((!pzemrdy) && pzem_count >= 10)
  {
    ESP.restart();
  }

  if (pzemrdy)
  {
    buildStatusJson("PZEM004 gestartet!");
  }
  else
  {
    buildStatusJson("PZEM004 fehlerhaft!");
  }

  getData();

  /********************************************************
    Einstellungen aus dem persistenten Speicher holen
  ********************************************************/
  storeParameter.begin("storedParams", false);
  restoreParams();

  /********************************************************
    Schreibe gespeicherte Ausgangswerte
  ********************************************************/
  auto restoreOutputs = []()
  {
    pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
    pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
    pcf8574.digitalWrite(2, !logic_data_t->FlagLoadPump);
    pcf8574.digitalWrite(3, !logic_data_t->FlagValve);
  };

  sendStorage();
  sendStatusChip();

  /*****************************************************/
  /* Zeit vom Server holen                             */
  /* Erster Parameter: Zeitversatz als Dezimalzahl     */
  /* Zweiter Parameter: 1 für Sommerzeit               */
  /*****************************************************/
  while (dateTime.valid != true)
  {
    dateTime = NTPch.getNTPtime(1.0, 1);
    Serial.println("Kontaktiere Zeit-Server...");
  }
  delay(2000);

  if (dateTime.valid)
  {
    NTPch.printDateTime(dateTime);

    actualHour = dateTime.hour;
    actualMinute = dateTime.minute;
    actualSecond = dateTime.second;
    actualYear = dateTime.year;
    actualMonth = dateTime.month;
    actualDay = dateTime.day;
    actualDayOfWeek = dateTime.dayofWeek;
  }
  /*****************************************************/

  /********************************************************
    Display-Startinhalte initialisieren
  ********************************************************/
  initDisplayContent();
}

void loop()
{
  /********************************************************
    Handle Display
  ********************************************************/
  myNex.NextionListen();
  refreshCurrentPage();
  firstRefresh();

  /********************************************************
    Webserver
  ********************************************************/
  ws.cleanupClients();

  /********************************************************
    Zyklische Temperaturmessung
  ********************************************************/
  long t = millis();
  TempLoop(t);

  /********************************************************
    Zyklisch Allgemeine Daten behandeln
  ********************************************************/
  if (updateInterval())
  {
    digitalClock();
    getData();
    sendMeasurements();

    if (WiFi.isConnected())
    {
      dateTime = NTPch.getNTPtime(1.0, 1);
    }
    else
    {
      actualHour = 0;
      actualMinute = 0;
    }

    // ******************************** Zeitabgleich ***********************************
    // *********************************************************************************
    if (dateTime.valid)
    {
      actualHour = dateTime.hour;
      actualMinute = dateTime.minute;
      actualSecond = dateTime.second;
      actualYear = dateTime.year;
      actualMonth = dateTime.month;
      actualDay = dateTime.day;
      actualDayOfWeek = dateTime.dayofWeek;
    }

    /********************************************************
      Differenz-Regelung Solarpumpe Ost
    ********************************************************/
    int iTemp_Warmwasser = fTemp_Warmwasser * 10;
    int iTemp_Puffer_Unten = fTemp_Puffer_Unten * 10;
    int iTemp_Kollektor_Ost = fTemp_Collector_East * 10;

    if (logic_data_t->FlagIsAutomatic == true)
    {
      // Wenn Kollektorfühler < Kollektor-Übertemperatur Einschaltschwelle --> Standard 110° C
      if (((iTemp_Kollektor_Ost < parameter_data_t->iOverOnCollectorEast * 10) && (logic_data_t->FlagIsHeatProtectEast == true)) || (logic_data_t->FlagIsHeatProtectEast == false))
      {
        if (DEBUG_SOLAR_EAST)
          Serial.println("Kollektortemp Ost < Übertemp Ost");

        // Wenn Puffer Oben < Maximal-Begrenzung Einschaltschwelle --> Standard 70° C
        if (iTemp_Warmwasser < parameter_data_t->iMaxLimitOnPumpEast * 10)
        {
          if (DEBUG_SOLAR_EAST)
            Serial.println("Puffer oben < Maximal-Begrenzung");

          // Wenn Kollektorfühler > Puffer unten --> 1. Bedingung
          if (iTemp_Kollektor_Ost > iTemp_Puffer_Unten)
          {
            if (DEBUG_SOLAR_EAST)
              Serial.println("Kollektortemp Ost > Puffer unten");

            // Wenn Differenz zwischen Kollektorfühler
            // und Puffer unten >= Parameter Diff Ein --> Pumpe Ein --> Standard 8 K
            if (((iTemp_Kollektor_Ost - iTemp_Puffer_Unten) >= parameter_data_t->iDiffOnPumpEast * 10) && (logic_data_t->FlagPumpEast == false))
            {
              if (DEBUG_SOLAR_EAST)
              {
                Serial.println("Differenz überschritten!");
                Serial.println("Differenz Kollektortemp Ost - Puffer unten = " + String(iTemp_Kollektor_Ost - iTemp_Puffer_Unten) + " °C");
              }

              // Wenn die Zeitschaltuhr nicht aktiv ist
              if (logic_data_t->FlagFromTimerLoop == false)
              {
                if (DEBUG_SOLAR_EAST)
                  Serial.println("Anforderung Solarpumpe Ost aktiv!");

                logic_data_t->FlagPumpEast = true;
                pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
                myNex.writeStr("ScreenDim.val=0");
                sendLogic();
              }
            }
            // Wenn Differenz zwischen Kollektorfühler
            // und Puffer unten <= Parameter Diff Aus --> Pumpe Aus --> Standard 4 K
            else if (((iTemp_Kollektor_Ost - iTemp_Puffer_Unten) <= parameter_data_t->iDiffOffPumpEast * 10) && (logic_data_t->FlagPumpEast == true))
            {
              if (DEBUG_SOLAR_EAST)
              {
                Serial.println("Differenz unterschritten!");
                Serial.println("Differenz Kollektortemp Ost - Puffer unten = " + String(iTemp_Kollektor_Ost - iTemp_Puffer_Unten) + " °C");
              }

              // Wenn die Zeitschaltuhr nicht aktiv ist
              if (logic_data_t->FlagFromTimerLoop == false)
              {
                if (DEBUG_SOLAR_EAST)
                  Serial.println("Anforderung Solarpumpe Ost inaktiv!");

                logic_data_t->FlagPumpEast = false;
                pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
                myNex.writeStr("ScreenDim.val=0");
                sendLogic();
              }
            }
          }
        }
      }
      // Wenn Kollektorfühler >= Kollektor-Übertemperatur Ausschaltschwelle --> Standard 130° C
      else if (((iTemp_Kollektor_Ost >= parameter_data_t->iOverOffCollectorEast * 10) && (logic_data_t->FlagIsHeatProtectEast == true)) && (logic_data_t->FlagPumpEast == true))
      {
        if (logic_data_t->FlagFromTimerLoop == false)
        {
          if (DEBUG_SOLAR_EAST)
            Serial.println("Anforderung Solarpumpe Ost inaktiv!");

          logic_data_t->FlagPumpEast = false;
          pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
          myNex.writeStr("ScreenDim.val=0");
          sendLogic();
        }
      }
      // Wenn Puffer Oben >= Maximal-Begrenzung Ausschaltschwelle --> Standard 75° C
      else if ((iTemp_Warmwasser >= parameter_data_t->iMaxLimitOffPumpEast * 10) && (logic_data_t->FlagPumpEast == true))
      {
        if (DEBUG_SOLAR_EAST)
        {
          Serial.println("Differenz unterschritten!");
          Serial.println("Differenz Kollektortemp Ost - Puffer unten = " + String(iTemp_Kollektor_Ost - iTemp_Puffer_Unten) + " °C");
        }

        if (logic_data_t->FlagFromTimerLoop == false)
        {
          logic_data_t->FlagPumpEast = false;
          pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
          myNex.writeStr("ScreenDim.val=0");
          sendLogic();
        }
      }
    }

    /********************************************************
      Frostschutz Kollektorfeld Ost
    ********************************************************/
    if (iTemp_Kollektor_Ost < iTemp_Puffer_Unten)
    {
      // Wenn Kollektorfühler < Kollektor-Frostschutz Einschaltschwelle --> Standard 2° C
      if (((iTemp_Kollektor_Ost < parameter_data_t->iColdOnCollectorEast * 10) && (logic_data_t->FlagIsFreezeProtectEast == true)) && (logic_data_t->FlagPumpEast == false))
      {
        logic_data_t->FlagPumpEast = true;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
      }
      // Wenn Kollektorfühler >= Kollektor-Frostschutz Ausschaltschwelle --> Standard 4° C
      else if (((iTemp_Kollektor_Ost >= parameter_data_t->iColdOffCollectorEast * 10) && (logic_data_t->FlagIsFreezeProtectEast == true)) && (logic_data_t->FlagPumpEast == true))
      {
        logic_data_t->FlagPumpEast = false;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
      }
    }

    /********************************************************
      Kühlfunktion Kollektorfeld Ost
    ********************************************************/
    int iTemp_Puffer_Mitte = fTemp_Puffer_Mitte * 10;

    // Wenn Puffer Mitte > Sollwert Kühlfunktion --> Standard 80° C
    if (iTemp_Puffer_Mitte > parameter_data_t->iCoolingEast * 10)
    {
      uint8_t iFreigabezeit_Ost_Stunden = timer_data_t->iReleaseHourTenEast * 10 + timer_data_t->iReleaseHourOneEast;
      uint8_t iFreigabezeit_Ost_Minuten = timer_data_t->iReleaseMinTenEast * 10 + timer_data_t->iReleaseMinOneEast;

      // Wenn Freigabezeit Ost erreicht ist --> Umladepumpe Ein + Solarpumpe Ost Ein
      if ((iFreigabezeit_Ost_Stunden == actualHour) && (iFreigabezeit_Ost_Minuten == actualMinute) && (logic_data_t->FlagLoadPump == false) && (logic_data_t->FlagPumpEast == false))
      {
        logic_data_t->FlagLoadPump = true;
        logic_data_t->FlagPumpEast = true;
        logic_data_t->FlagFromTimerLoop = true;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        pcf8574.digitalWrite(2, !logic_data_t->FlagLoadPump);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
      }
    }

    uint8_t iSperrzeit_Ost_Stunden = timer_data_t->iBlockHourTenEast * 10 + timer_data_t->iBlockHourOneEast;
    uint8_t iSperrzeit_Ost_Minuten = timer_data_t->iBlockMinTenEast * 10 + timer_data_t->iBlockMinOneEast;

    // Wenn Sperrzeit Ost erreicht ist --> Umladepumpe Aus
    if ((iSperrzeit_Ost_Stunden == actualHour) && (iSperrzeit_Ost_Minuten == actualMinute) && (logic_data_t->FlagLoadPump == true) && (logic_data_t->FlagPumpEast == true))
    {
      logic_data_t->FlagLoadPump = false;
      logic_data_t->FlagPumpEast = false;
      logic_data_t->FlagFromTimerLoop = false;
      pcf8574.digitalWrite(0, !logic_data_t->FlagLoadPump);
      pcf8574.digitalWrite(2, !logic_data_t->FlagLoadPump);
      myNex.writeStr("ScreenDim.val=0");
      sendLogic();
    }

    /********************************************************
      Kühlfunktion Kollektorfeld West
    ********************************************************/

    // Wenn Puffer Mitte > Sollwert Kühlfunktion --> Standard 80° C
    if (iTemp_Puffer_Mitte > parameter_data_t->iCoolingWest * 10)
    {
      uint8_t iFreigabezeit_West_Stunden = timer_data_t->iReleaseHourTenWest * 10 + timer_data_t->iReleaseHourOneWest;
      uint8_t iFreigabezeit_West_Minuten = timer_data_t->iReleaseMinTenWest * 10 + timer_data_t->iReleaseMinOneWest;

      // Wenn Freigabezeit West erreicht ist --> Umladepumpe Ein
      if ((iFreigabezeit_West_Stunden == actualHour) && (iFreigabezeit_West_Minuten == actualMinute) && (logic_data_t->FlagLoadPump == false) && (logic_data_t->FlagPumpWest == false))
      {
        logic_data_t->FlagLoadPump = true;
        logic_data_t->FlagPumpWest = true;
        logic_data_t->FlagFromTimerLoop = true;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        pcf8574.digitalWrite(2, !logic_data_t->FlagLoadPump);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
      }
    }

    uint8_t iSperrzeit_West_Stunden = timer_data_t->iBlockHourTenWest * 10 + timer_data_t->iBlockHourOneWest;
    uint8_t iSperrzeit_West_Minuten = timer_data_t->iBlockMinTenWest * 10 + timer_data_t->iBlockMinOneWest;

    // Wenn Sperrzeit West erreicht ist --> Umladepumpe Aus
    if ((iSperrzeit_West_Stunden == actualHour) && (iSperrzeit_West_Minuten == actualMinute) && (logic_data_t->FlagLoadPump == true) && (logic_data_t->FlagPumpWest == true))
    {
      logic_data_t->FlagLoadPump = false;
      logic_data_t->FlagPumpWest = false;
      logic_data_t->FlagFromTimerLoop = false;
      pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
      pcf8574.digitalWrite(2, !logic_data_t->FlagLoadPump);
      myNex.writeStr("ScreenDim.val=0");
      sendLogic();
    }

    /********************************************************
      Differenz-Regelung Solarpumpe West
    ********************************************************/
    int iTemp_Kollektor_West = fTemp_Collector_West * 10;

    if (logic_data_t->FlagIsAutomatic == true)
    {
      // Wenn Kollektorfühler < Kollektor-Übertemperatur Einschaltschwelle -. Standard 110° C
      if (((iTemp_Kollektor_West < parameter_data_t->iOverOnCollectorWest * 10) && (logic_data_t->FlagIsHeatProtectWest == true)) || (logic_data_t->FlagIsHeatProtectWest == false))
      {
        if (DEBUG_SOLAR_WEST)
          Serial.println("Kollektortemp West < Übertemp West");

        // Wenn Puffer Oben < Maximal-Begrenzung Einschaltschwelle --> Standard 70° C
        if (iTemp_Warmwasser < parameter_data_t->iMaxLimitOnPumpWest * 10)
        {
          if (DEBUG_SOLAR_WEST)
            Serial.println("Puffer oben < Maximal-Begrenzung");

          // Wenn Kollektorfühler > Puffer unten -. 1. Bedingung
          if (iTemp_Kollektor_West > iTemp_Puffer_Unten)
          {
            if (DEBUG_SOLAR_WEST)
              Serial.println("Kollektortemp West > Puffer unten");

            // Wenn Differenz zwischen Kollektorfühler
            // und Puffer unten >= Parameter Diff Ein --> Pumpe Ein --> Standard 8 K
            if (((iTemp_Kollektor_West - iTemp_Puffer_Unten) >= parameter_data_t->iDiffOnPumpWest * 10) && (logic_data_t->FlagPumpWest == false))
            {
              if (DEBUG_SOLAR_WEST)
              {
                Serial.println("Differenz überschritten!");
                Serial.println("Differenz Kollektortemp West - Puffer unten = " + String(iTemp_Kollektor_Ost - iTemp_Puffer_Unten) + " °C");
              }

              if (logic_data_t->FlagFromTimerLoop == false)
              {
                if (DEBUG_SOLAR_WEST)
                  Serial.println("Anforderung Solarpumpe West aktiv!");

                logic_data_t->FlagPumpWest = true;
                pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
                myNex.writeStr("ScreenDim.val=0");
                sendLogic();
              }
            }
            // Wenn Differenz zwischen Kollektorfühler
            // und Puffer unten <= Parameter Diff Aus --> Pumpe Aus --> Standard 4 K
            else if (((iTemp_Kollektor_West - iTemp_Puffer_Unten) <= parameter_data_t->iDiffOffPumpWest * 10) && (logic_data_t->FlagPumpWest == true))
            {
              if (DEBUG_SOLAR_WEST)
              {
                Serial.println("Differenz unterschritten!");
                Serial.println("Differenz Kollektortemp West - Puffer unten = " + String(iTemp_Kollektor_Ost - iTemp_Puffer_Unten) + " °C");
              }

              if (logic_data_t->FlagFromTimerLoop == false)
              {
                if (DEBUG_SOLAR_WEST)
                  Serial.println("Anforderung Solarpumpe West inaktiv!");

                logic_data_t->FlagPumpWest = false;
                pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
                myNex.writeStr("ScreenDim.val=0");
                sendLogic();
              }
            }
          }
        }
      }
      // Wenn Kollektorfühler >= Kollektor-Übertemperatur Ausschaltschwelle --> Standard 130° C
      else if (((iTemp_Kollektor_West >= parameter_data_t->iOverOffCollectorWest * 10) && (logic_data_t->FlagIsHeatProtectWest == true)) && (logic_data_t->FlagPumpWest == true))
      {
        if (logic_data_t->FlagFromTimerLoop == false)
        {
          if (DEBUG_SOLAR_WEST)
            Serial.println("Anforderung Solarpumpe West inaktiv!");

          logic_data_t->FlagPumpWest = false;
          pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
          myNex.writeStr("ScreenDim.val=0");
          sendLogic();
        }
      }
      // Wenn Puffer Oben >= Maximal-Begrenzung Ausschaltschwelle --> Standard 75° C
      else if ((iTemp_Warmwasser >= parameter_data_t->iMaxLimitOffPumpWest * 10) && (logic_data_t->FlagPumpWest == true))
      {
        if (DEBUG_SOLAR_WEST)
        {
          Serial.println("Differenz unterschritten!");
          Serial.println("Differenz Kollektortemp West - Puffer unten = " + String(iTemp_Kollektor_Ost - iTemp_Puffer_Unten) + " °C");
        }

        if (logic_data_t->FlagFromTimerLoop == false)
        {
          logic_data_t->FlagPumpWest = false;
          pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
          myNex.writeStr("ScreenDim.val=0");
          sendLogic();
        }
      }
    }

    /********************************************************
      Frostschutz Kollektorfeld West
    ********************************************************/
    if (iTemp_Kollektor_West < iTemp_Puffer_Unten)
    {
      // Wenn Kollektorfühler < Kollektor-Frostschutz Einschaltschwelle --> Standard 2° C
      if (((iTemp_Kollektor_West < parameter_data_t->iColdOnCollectorWest * 10) && (logic_data_t->FlagIsFreezeProtectWest == true)) && (logic_data_t->FlagPumpWest == false))
      {
        logic_data_t->FlagPumpWest = true;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
      }
      // Wenn Kollektorfühler >= Kollektor-Frostschutz Ausschaltschwelle -. Standard 4° C
      else if (((iTemp_Kollektor_West >= parameter_data_t->iColdOffCollectorWest * 10) && (logic_data_t->FlagIsFreezeProtectWest == true)) && (logic_data_t->FlagPumpWest == true))
      {
        logic_data_t->FlagPumpWest = false;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
      }
    }

    /********************************************************
      Regelung Umschaltventil
    ********************************************************/
    int iTemp_Ruecklauf = fTemp_Ruecklauf * 10;

    if (logic_data_t->FlagIsAutomatic == true)
    {
      // Wenn Differenz zwischen Rücklauf Heizung
      // und Puffer mitte >= Parameter Diff Ein
      // und Freigabetemperatur erreicht --> Ventil Ein --> Standard 10 K
      if (((iTemp_Puffer_Mitte - iTemp_Ruecklauf) >= parameter_data_t->iDiffOnValve * 10) && (logic_data_t->FlagValve == false))
      {
        if (DEBUG_VALVE)
        {
          Serial.println("Differenz überschritten!");
          Serial.println("Differenz Puffer Mitte - Rücklauf = " + String(iTemp_Puffer_Mitte - iTemp_Ruecklauf) + " °C");
        }

        // Solltemperatur erreicht --> Standard 50 °C
        if (iTemp_Puffer_Mitte > parameter_data_t->iValveRelease * 10)
        {
          if (DEBUG_VALVE)
            Serial.println("Anforderung Umschaltventil aktiv!");

          logic_data_t->FlagValve = true;
          pcf8574.digitalWrite(3, !logic_data_t->FlagValve);
          myNex.writeStr("ScreenDim.val=0");
          sendLogic();
        }
      }
      // Wenn Differenz zwischen Rücklauf Heizung
      // und Puffer mitte <= Parameter Diff Aus --> Pumpe Aus --> Standard 5 K
      else if (((iTemp_Puffer_Mitte - iTemp_Ruecklauf) <= parameter_data_t->iDiffOffValve * 10) && (logic_data_t->FlagValve == true))
      {
        if (DEBUG_VALVE)
          Serial.println("Anforderung Umschaltventil inaktiv!");

        logic_data_t->FlagValve = false;
        pcf8574.digitalWrite(3, !logic_data_t->FlagValve);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
      }
    }

    /********************************************************
      Regelung Umladepumpe
    ********************************************************/
    int iTemp_Vorlauf = fTemp_Vorlauf * 10;
    int iTemp_Schaltpunkt_Oben = 0;
    int iTemp_Schaltpunkt_Unten = 0;

    if (logic_data_t->FlagIsAutomatic == true)
    {
      // Auswertung welcher Sensor für Schaltpunkt oben
      switch (parameter_data_t->iSelectUpLoadpump)
      {
      case 0:
        iTemp_Schaltpunkt_Oben = iTemp_Warmwasser;
        break;

      case 1:
        iTemp_Schaltpunkt_Oben = iTemp_Puffer_Mitte;
        break;

      case 2:
        iTemp_Schaltpunkt_Oben = iTemp_Puffer_Unten;
        break;

      case 3:
        iTemp_Schaltpunkt_Oben = iTemp_Vorlauf;
        break;

      case 4:
        iTemp_Schaltpunkt_Oben = iTemp_Ruecklauf;
        break;
      }

      // Auswertung welcher Sensor für Schaltpunkt unten
      switch (parameter_data_t->iSelectDownLoadpump)
      {
      case 0:
        iTemp_Schaltpunkt_Unten = iTemp_Warmwasser;
        break;

      case 1:
        iTemp_Schaltpunkt_Unten = iTemp_Puffer_Mitte;
        break;

      case 2:
        iTemp_Schaltpunkt_Unten = iTemp_Puffer_Unten;
        break;

      case 3:
        iTemp_Schaltpunkt_Unten = iTemp_Vorlauf;
        break;

      case 4:
        iTemp_Schaltpunkt_Unten = iTemp_Ruecklauf;
        break;
      }

      // Wenn Differenz zwischen Sensor Schaltpunkt oben
      // und Schaltpunkt unten >= Parameter Diff Ein
      // und Freigabetemperatur erreicht --> Umladepumpe Ein --> Standard 10 K
      if (((iTemp_Schaltpunkt_Oben - iTemp_Schaltpunkt_Unten) >= parameter_data_t->iDiffOnLoadpump * 10) && (logic_data_t->FlagLoadPump == false))
      {
        // Solltemperatur erreicht --> Standard 50 °C
        if (iTemp_Schaltpunkt_Oben > parameter_data_t->iTempLoadpump * 10)
        {
          logic_data_t->FlagLoadPump = true;
          pcf8574.digitalWrite(2, !logic_data_t->FlagLoadPump);
          myNex.writeStr("ScreenDim.val=0");
          sendLogic();
        }
      }
      // Wenn Differenz zwischen Sensor Schaltpunkt Unten
      // und Schaltpunkt unten <= Parameter Diff Aus --> Umladepumpe Aus --> Standard 5 K
      else if (((iTemp_Schaltpunkt_Oben - iTemp_Schaltpunkt_Unten) <= parameter_data_t->iDiffOffLoadpump * 10) && (logic_data_t->FlagLoadPump == true))
      {
        logic_data_t->FlagLoadPump = false;
        pcf8574.digitalWrite(2, !logic_data_t->FlagLoadPump);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
      }
    }
  }
}

/********************************************************
    Methode für Smart-Meter
********************************************************/
void getData()
{
  // *************************** Spannungswert einlesen ******************************
  // *********************************************************************************
  Volt = pzem.voltage(ip); // Minimum 0.0 Volt festlegen
  if (Volt <= 0.0)
    Volt = 0.0;

  // *************************** Stromstärke einlesen ********************************
  // *********************************************************************************
  Ampere = pzem.current(ip); // Minimum 0.0 Ampere festlegen
  if (Ampere <= 0.0)
  {
    Ampere = 0.0;
  }

  // **************************** Leistungswert einlesen *****************************
  // *********************************************************************************
  Watt = pzem.power(ip);
  if (Watt <= 0.0)
  {
    Watt = 0.0;
  }

  // *************************** Arbeitswert einlesen ********************************
  // *********************************************************************************
  Arbeit = pzem.energy(ip);
  if (Arbeit <= 0.0)
  {
    Arbeit = 0.0;
  }
}

/********************************************************
    Methode für Intervallcheck
********************************************************/
bool updateInterval()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= MESSINTERVALL)
  {
    previousMillis = currentMillis;
    return true;
  }
  else
  {
    return false;
  }
}

/********************************************************
  Lese gespeicherte Parameter von Flash

  Die gelesenen persistenten Werte
  werden in das Display geschrieben und in den Struct
 ********************************************************/
void restoreParams()
{
  // Schreibe persistente Daten "Handbetrieb" in das Display zurück
  myNex.writeNum("page7.sw1.val", storeParameter.getBool("FlagPumpEast"));
  myNex.writeNum("page7.sw2.val", storeParameter.getBool("FlagPumpWest"));
  myNex.writeNum("page7.sw3.val", storeParameter.getBool("FlagLoadPump"));
  myNex.writeNum("page7.sw4.val", storeParameter.getBool("FlagValve"));
  delay(100);

  // Schreibe persistente Daten "Handbetrieb" in den Struct zurück
  logic_data_t->FlagPumpEast = storeParameter.getBool("FlagPumpEast");
  logic_data_t->FlagPumpWest = storeParameter.getBool("FlagPumpWest");
  logic_data_t->FlagLoadPump = storeParameter.getBool("FlagLoadPump");
  logic_data_t->FlagValve = storeParameter.getBool("FlagValve");
  delay(100);

  // Schreibe persistente Daten "Maximal-Begrenzung" in das Display zurück
  myNex.writeNum("page4.MaxOnEast.val", storeParameter.getUChar("MaxOnPumpEast", 75));
  myNex.writeNum("page4.MaxOnWest.val", storeParameter.getUChar("MaxOnPumpWest", 75));
  myNex.writeNum("page4.MaxOffEast.val", storeParameter.getUChar("MaxOffPumpEast", 70));
  myNex.writeNum("page4.MaxOffWest.val", storeParameter.getUChar("MaxOffPumpWest", 70));
  delay(100);

  // Schreibe persistente Daten "Maximal-Begrenzung" in den Struct zurück
  parameter_data_t->iMaxLimitOnPumpEast = storeParameter.getUChar("MaxOnPumpEast", 75);
  parameter_data_t->iMaxLimitOnPumpWest = storeParameter.getUChar("MaxOnPumpWest", 75);
  parameter_data_t->iMaxLimitOffPumpEast = storeParameter.getUChar("MaxOffPumpEast", 70);
  parameter_data_t->iMaxLimitOffPumpWest = storeParameter.getUChar("MaxOffPumpWest", 70);
  delay(100);

  // Schreibe persistente Daten "Minimal-Begrenzung" in das Display zurück
  myNex.writeNum("page5.MinOnEast.val", storeParameter.getUChar("MinOnPumpEast", 5));
  myNex.writeNum("page5.MinOnWest.val", storeParameter.getUChar("MinOnPumpWest", 5));
  myNex.writeNum("page5.MinOffEast.val", storeParameter.getUChar("MinOffPumpEast", 0));
  myNex.writeNum("page5.MinOffWest.val", storeParameter.getUChar("MinOffPumpWest", 0));
  delay(100);

  // Schreibe persistente Daten "Minimal-Begrenzung" in den Struct zurück
  parameter_data_t->iMinLimitOnPumpEast = storeParameter.getUChar("MinOnPumpEast", 5);
  parameter_data_t->iMinLimitOnPumpWest = storeParameter.getUChar("MinOnPumpWest", 5);
  parameter_data_t->iMinLimitOffPumpEast = storeParameter.getUChar("MinOffPumpEast", 0);
  parameter_data_t->iMinLimitOffPumpWest = storeParameter.getUChar("MinOffPumpWest", 0);
  delay(100);

  // Schreibe persistente Daten "Differenzwerte" in das Display zurück
  myNex.writeNum("page6.DiffOnEast.val", storeParameter.getUChar("DiffOnPumpEast", 8));
  myNex.writeNum("page6.DiffOnWest.val", storeParameter.getUChar("DiffOnPumpWest", 8));
  myNex.writeNum("page6.DiffOffEast.val", storeParameter.getUChar("DiffOffPumpEast", 4));
  myNex.writeNum("page6.DiffOffWest.val", storeParameter.getUChar("DiffOffPumpWest", 4));
  delay(100);

  // Schreibe persistente Daten "Differenzwerte" in den Struct zurück
  parameter_data_t->iDiffOnPumpEast = storeParameter.getUChar("DiffOnPumpEast", 8);
  parameter_data_t->iDiffOnPumpWest = storeParameter.getUChar("DiffOnPumpWest", 8);
  parameter_data_t->iDiffOffPumpEast = storeParameter.getUChar("DiffOffPumpEast", 4);
  parameter_data_t->iDiffOffPumpWest = storeParameter.getUChar("DiffOffPumpWest", 4);
  delay(100);

  // Schreibe persistente Daten "Kollektor-Übertemperatur" in das Display zurück
  myNex.writeNum("page8.OverOnEast.val", storeParameter.getUChar("OverOnEast", 110));
  myNex.writeNum("page8.OverOnWest.val", storeParameter.getUChar("OverOnWest", 110));
  myNex.writeNum("page8.OverOffEast.val", storeParameter.getUChar("OverOffEast", 130));
  myNex.writeNum("page8.OverOffWest.val", storeParameter.getUChar("OverOffWest", 130));
  delay(100);

  // Schreibe persistente Daten "Kollektor-Übertemperatur" in den Struct zurück
  parameter_data_t->iOverOnCollectorEast = storeParameter.getUChar("OverOnEast", 110);
  parameter_data_t->iOverOnCollectorWest = storeParameter.getUChar("OverOnWest", 110);
  parameter_data_t->iOverOffCollectorEast = storeParameter.getUChar("OverOffEast", 130);
  parameter_data_t->iOverOffCollectorWest = storeParameter.getUChar("OverOffWest", 130);
  delay(100);

  // Schreibe persistente Daten "Kollektor-Frostschutz" in das Display zurück
  myNex.writeNum("page9.ColdOnEast.val", storeParameter.getChar("ColdOnEast", 2));
  myNex.writeNum("page9.ColdOnWest.val", storeParameter.getChar("ColdOnWest", 2));
  myNex.writeNum("page9.ColdOffEast.val", storeParameter.getChar("ColdOffEast", 4));
  myNex.writeNum("page9.ColdOffWest.val", storeParameter.getChar("ColdOffWest", 4));
  delay(100);

  // Schreibe persistente Daten "Kollektor-Frostschutz" in den Struct zurück
  parameter_data_t->iColdOnCollectorEast = storeParameter.getChar("ColdOnEast", 2);
  parameter_data_t->iColdOnCollectorWest = storeParameter.getChar("ColdOnWest", 2);
  parameter_data_t->iColdOffCollectorEast = storeParameter.getChar("ColdOffEast", 4);
  parameter_data_t->iColdOffCollectorWest = storeParameter.getChar("ColdOffWest", 4);
  delay(100);

  // Schreibe persistente Daten "Sollwert Kühlfunktion Ost" in das Display zurück
  myNex.writeNum("page10.CoolOnEast.val", storeParameter.getUChar("CoolingEast", 80));
  // Schreibe persistente Daten "Sollwert Kühlfunktion Ost" in den Struct zurück
  parameter_data_t->iCoolingEast = storeParameter.getUChar("CoolingEast", 80);
  delay(100);

  // Schreibe persistente Daten "Freigabezeit Kühlfunktion Ost" in das Display zurück
  myNex.writeNum("page10.FrgHourTen.val", storeParameter.getUChar("RelHrTenEast", 1));
  myNex.writeNum("page10.FrgHourOne.val", storeParameter.getUChar("RelHrOneEast", 2));
  myNex.writeNum("page10.FrgMinTen.val", storeParameter.getUChar("RelMinTenEast", 0));
  myNex.writeNum("page10.FrgMinOne.val", storeParameter.getUChar("RelMinOneEast", 0));
  delay(100);

  // Schreibe persistente Daten "Freigabezeit Kühlfunktion Ost" in den Struct zurück
  timer_data_t->iReleaseHourTenEast = storeParameter.getUChar("RelHrTenEast", 1);
  timer_data_t->iReleaseHourOneEast = storeParameter.getUChar("RelHrOneEast", 2);
  timer_data_t->iReleaseMinTenEast = storeParameter.getUChar("RelMinTenEast", 0);
  timer_data_t->iReleaseMinOneEast = storeParameter.getUChar("RelMinOneEast", 0);
  delay(100);

  // Schreibe persistente Daten "Sperrzeit Kühlfunktion Ost" in das Display zurück
  myNex.writeNum("page10.SprHourTen.val", storeParameter.getUChar("BlkHrTenEast", 1));
  myNex.writeNum("page10.SprHourOne.val", storeParameter.getUChar("BlkHrOneEast", 2));
  myNex.writeNum("page10.SprMinTen.val", storeParameter.getUChar("BlkMinTenEast", 0));
  myNex.writeNum("page10.SprMinOne.val", storeParameter.getUChar("BlkMinOneEast", 0));
  delay(100);

  // Schreibe persistente Daten "Sperrzeit Kühlfunktion Ost" in den Struct zurück
  timer_data_t->iBlockHourTenEast = storeParameter.getUChar("BlkHrTenEast", 1);
  timer_data_t->iBlockHourOneEast = storeParameter.getUChar("BlkHrOneEast", 2);
  timer_data_t->iBlockMinTenEast = storeParameter.getUChar("BlkMinTenEast", 0);
  timer_data_t->iBlockMinOneEast = storeParameter.getUChar("BlkMinOneEast", 0);
  delay(100);

  // Schreibe persistente Daten "Sollwert Kühlfunktion West" in das Display zurück
  myNex.writeNum("page10.CoolOnWest.val", storeParameter.getUChar("CoolingWest", 80));
  // Schreibe persistente Daten "Sollwert Kühlfunktion West" in den Struct zurück
  parameter_data_t->iCoolingWest = storeParameter.getUChar("CoolingWest", 80);
  delay(100);

  // Schreibe persistente Daten "Freigabezeit Kühlfunktion West" in das Display zurück
  myNex.writeNum("page10.FrgHourTen1.val", storeParameter.getUChar("RelHrTenWest", 1));
  myNex.writeNum("page10.FrgHourOne1.val", storeParameter.getUChar("RelHrOneWest", 2));
  myNex.writeNum("page10.FrgMinTen1.val", storeParameter.getUChar("RelMinTenWest", 0));
  myNex.writeNum("page10.FrgMinOne1.val", storeParameter.getUChar("RelMinOneWest", 0));
  delay(100);

  // Schreibe persistente Daten "Freigabezeit Kühlfunktion West" in den Struct zurück
  timer_data_t->iReleaseHourTenWest = storeParameter.getUChar("RelHrTenWest", 1);
  timer_data_t->iReleaseHourOneWest = storeParameter.getUChar("RelHrOneWest", 2);
  timer_data_t->iReleaseMinTenWest = storeParameter.getUChar("RelMinTenWest", 0);
  timer_data_t->iReleaseMinOneWest = storeParameter.getUChar("RelMinOneWest", 0);
  delay(100);

  // Schreibe persistente Daten "Sperrzeit Kühlfunktion West" in das Display zurück
  myNex.writeNum("page10.SprHourTen1.val", storeParameter.getUChar("BlkHrTenWest", 1));
  myNex.writeNum("page10.SprHourOne1.val", storeParameter.getUChar("BlkHrOneWest", 2));
  myNex.writeNum("page10.SprMinTen1.val", storeParameter.getUChar("BlkMinTenWest", 0));
  myNex.writeNum("page10.SprMinOne1.val", storeParameter.getUChar("BlkMinOneWest", 0));
  delay(100);

  // Schreibe persistente Daten "Sperrzeit Kühlfunktion West" in den Struct zurück
  timer_data_t->iBlockHourTenWest = storeParameter.getUChar("BlkHrTenWest", 1);
  timer_data_t->iBlockHourOneWest = storeParameter.getUChar("BlkHrOneWest", 2);
  timer_data_t->iBlockMinTenWest = storeParameter.getUChar("BlkMinTenWest", 0);
  timer_data_t->iBlockMinOneWest = storeParameter.getUChar("BlkMinOneWest", 0);
  delay(100);

  // Schreibe persistente Daten "Parameter Umschaltventil" in das Display zurück
  myNex.writeNum("page13.ValveOn.val", storeParameter.getUChar("DiffOnValve", 10));
  myNex.writeNum("page13.ValveOff.val", storeParameter.getUChar("DiffOffValve", 5));
  myNex.writeNum("page13.ValveRelease.val", storeParameter.getUChar("TempValve", 50));
  delay(100);

  // Schreibe persistente Daten "Parameter Umschaltventil" in den Struct zurück
  parameter_data_t->iDiffOnValve = storeParameter.getUChar("DiffOnValve", 10);
  parameter_data_t->iDiffOffValve = storeParameter.getUChar("DiffOffValve", 5);
  parameter_data_t->iValveRelease = storeParameter.getUChar("TempValve", 50);
  delay(100);

  // Schreibe persistente Daten "Parameter Umladepumpe" in das Display zurück
  myNex.writeNum("page12.SelectUp.val", storeParameter.getUChar("SelectUpLoad", 0));
  myNex.writeNum("page12.SelectDown.val", storeParameter.getUChar("SelectDownLoad", 3));
  myNex.writeNum("page12.SuppRelease.val", storeParameter.getUChar("TempLoad", 50));
  myNex.writeNum("page12.SuppOn.val", storeParameter.getUChar("DiffOnLoad", 10));
  myNex.writeNum("page12.SuppOff.val", storeParameter.getUChar("DiffOffLoad", 4));
  delay(100);

  // Schreibe persistente Daten "Parameter Umladepumpe" in den Struct zurück
  parameter_data_t->iSelectUpLoadpump = storeParameter.getUChar("SelectUpLoad", 0);
  parameter_data_t->iSelectDownLoadpump = storeParameter.getUChar("SelectDownLoad", 3);
  parameter_data_t->iTempLoadpump = storeParameter.getUChar("TempLoad", 50);
  parameter_data_t->iDiffOnLoadpump = storeParameter.getUChar("DiffOnLoad", 10);
  parameter_data_t->iDiffOffLoadpump = storeParameter.getUChar("DiffOffLoad", 4);
}
