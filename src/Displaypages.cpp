/********************************************************
  Bibliotheken
********************************************************/
#include "Prototypes.h"

/********************************************************
  Externe Referenzen
********************************************************/
extern Preferences storeParameter;
extern LOGIC *logic_data_t;
extern PARAMETER *parameter_data_t;
extern TIMER *timer_data_t;
extern TEMPORARY *temporary_data_t;

/********************************************************
    Methode für ersten Refresh einer neu geladenen Seite
********************************************************/
void firstRefresh()
{

    if (myNex.currentPageId != myNex.lastCurrentPageId)
    {
        newPageLoaded = true;

        switch (myNex.currentPageId)
        {
        case 0:
            refreshPage0();
            break;

        case 1:
            refreshPage1();
            break;

        case 2:
            refreshPage2();
            break;

        case 3:
            refreshPage3();
            break;

        case 4:
            refreshPage4();
            break;

        case 5:
            refreshPage5();
            break;

        case 6:
            refreshPage6();
            break;

        case 7:
            refreshPage7();
            break;

        case 8:
            refreshPage8();
            break;

        case 9:
            refreshPage9();
            break;

        case 10:
            refreshPage10();
            break;

        case 11:
            refreshPage11();
            break;

        case 12:
            refreshPage12();
            break;

        case 13:
            refreshPage13();
            break;
        }

        newPageLoaded = false;

        myNex.lastCurrentPageId = myNex.currentPageId;
    }
}

/********************************************************
    Methode für zyklischen Refresh der aktuellen Seite
  ********************************************************/
void refreshCurrentPage()
{
    if ((millis() - pageRefreshTimer) > DISPLAY_REFRESH_RATE)
    {
        switch (myNex.currentPageId)
        {
        case 0:
            refreshPage0();
            break;

        case 1:
            refreshPage1();
            break;

        case 2:
            refreshPage2();
            break;

        case 3:
            refreshPage3();
            break;

        case 4:
            refreshPage4();
            break;

        case 5:
            refreshPage5();
            break;

        case 6:
            refreshPage6();
            break;

        case 7:
            refreshPage7();
            break;

        case 8:
            refreshPage8();
            break;

        case 9:
            refreshPage9();
            break;

        case 10:
            refreshPage10();
            break;

        case 11:
            refreshPage11();
            break;

        case 12:
            refreshPage12();
            break;

        case 13:
            refreshPage13();
            break;
        }
    }

    pageRefreshTimer = millis();
}

/********************************************************
    Methode für Inhalte von Seite 1

    Systemübersicht und Temperaturanzeigen
    Faktor 10 wegen Datenformat von Nextion Display
  ********************************************************/
void refreshPage0()
{
    // Schreibe Temperatur Warmwasser in entsprechendes Feld
    int iTemp_Warmwasser = fTemp_Warmwasser * 10;
    myNex.writeNum("TempWW.val", iTemp_Warmwasser);

    // Schreibe Temperatur Vorlauf in entsprechendes Feld
    int iTemp_Vorlauf = fTemp_Vorlauf * 10;
    myNex.writeNum("TempVL.val", iTemp_Vorlauf);

    // Schreibe Temperatur Rücklauf in entsprechendes Feld
    int iTemp_Ruecklauf = fTemp_Ruecklauf * 10;
    myNex.writeNum("TempRL.val", iTemp_Ruecklauf);

    // Schreibe Temperatur Puffer Mitte in entsprechendes Feld
    int iTemp_Puffer_Mitte = fTemp_Puffer_Mitte * 10;
    myNex.writeNum("TempPuM.val", iTemp_Puffer_Mitte);

    // Schreibe Temperatur Puffer Unten in entsprechendes Feld
    int iTemp_Puffer_Unten = fTemp_Puffer_Unten * 10;
    myNex.writeNum("TempPuU.val", iTemp_Puffer_Unten);

    // Schreibe Temperatur Kollektor Ost in entsprechendes Feld
    int iTemp_Collector_East = fTemp_Collector_East * 10;
    myNex.writeNum("TempEast.val", iTemp_Collector_East);

    // Schreibe Temperatur Kollektor Ost in entsprechendes Feld
    int iTemp_Collector_West = fTemp_Collector_West * 10;
    myNex.writeNum("TempWest.val", iTemp_Collector_West);

    // Schreibe in HMI und zeige Symbol Solarpumpe in Übersicht Ostseite
    if (((logic_data_t->FlagPumpEast == true) && (myNex.readNumber("Sidechoice.val") == 0)) || ((logic_data_t->FlagPumpWest == true) && (myNex.readNumber("Sidechoice.val") == 1)))
    {
        myNex.writeStr("VarSolarpump.val=127");
    }
    else
    {
        myNex.writeStr("VarSolarpump.val=0");
    }

    // Schreibe in HMI und zeige Symbol Umladepumpe in Übersicht
    if (logic_data_t->FlagLoadPump == true)
    {
        myNex.writeStr("VarLoadpump.val=127");
    }
    else
    {
        myNex.writeStr("VarLoadpump.val=0");
    }

    // Schreibe in HMI und zeige Symbol Umschaltventil in Übersicht
    if (logic_data_t->FlagValve == true)
    {
        myNex.writeStr("VarValve.val=127");
    }
    else
    {
        myNex.writeStr("VarValve.val=0");
    }
}

/********************************************************
    Methode für Inhalte von Seite 2
  ********************************************************/
void refreshPage1()
{
}

/********************************************************
    Methode für Inhalte von Seite 3
  ********************************************************/
void refreshPage2()
{
    // Schreibe Wert für Leistung in entsprechendes Feld
    int iWatt = Watt * 1;
    myNex.writeNum("Power.val", iWatt);

    // Schreibe Wert für Spannung in entsprechendes Feld
    int iVolt = Volt * 100;
    myNex.writeNum("Voltage.val", iVolt);

    // Schreibe Wert für Strom in entsprechendes Feld
    int iAmpere = Ampere * 100;
    myNex.writeNum("Current.val", iAmpere);

    // Schreibe Wert für Arbeit in entsprechendes Feld
    int iArbeit = (Arbeit / 1000) * 10;
    myNex.writeNum("Work.val", iArbeit);
}

/********************************************************
    Methode für Inhalte von Seite 4
  ********************************************************/
void refreshPage3()
{
}

/********************************************************
    Methode für Inhalte von Seite 5

    Parameter für Maximal-Begrenzungen Solarkreis Ost
    Parameter für Maximal-Begrenzungen Solarkreis West
  ********************************************************/
void refreshPage4()
{
    // Maximal-Begrenzung Ausschaltschwelle von Solarkreis Ost
    temporary_data_t->iOldMaxLimitOffPumpEast = parameter_data_t->iMaxLimitOffPumpEast;
    parameter_data_t->iMaxLimitOffPumpEast = myNex.readNumber("MaxOffEast.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldMaxLimitOffPumpEast != parameter_data_t->iMaxLimitOffPumpEast)
    {
        temporary_data_t->iOldMaxLimitOffPumpEast = parameter_data_t->iMaxLimitOffPumpEast;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpEast = false;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    // Maximal-Begrenzung Einschaltschwelle von Solarkreis Ost
    temporary_data_t->iOldMaxLimitOnPumpEast = parameter_data_t->iMaxLimitOnPumpEast;
    parameter_data_t->iMaxLimitOnPumpEast = myNex.readNumber("MaxOnEast.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldMaxLimitOnPumpEast != parameter_data_t->iMaxLimitOnPumpEast)
    {
        temporary_data_t->iOldMaxLimitOnPumpEast = parameter_data_t->iMaxLimitOnPumpEast;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpEast = false;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    //***********************************************************************

    // Maximal-Begrenzung Ausschaltschwelle von Solarkreis West
    temporary_data_t->iOldMaxLimitOffPumpWest = parameter_data_t->iMaxLimitOffPumpWest;
    parameter_data_t->iMaxLimitOffPumpWest = myNex.readNumber("MaxOffWest.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldMaxLimitOffPumpWest != parameter_data_t->iMaxLimitOffPumpWest)
    {
        temporary_data_t->iOldMaxLimitOffPumpWest = parameter_data_t->iMaxLimitOffPumpWest;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpWest = false;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    // Maximal-Begrenzung Einschaltschwelle von Solarkreis West
    temporary_data_t->iOldMaxLimitOnPumpWest = parameter_data_t->iMaxLimitOnPumpWest;
    parameter_data_t->iMaxLimitOnPumpWest = myNex.readNumber("MaxOnWest.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldMaxLimitOnPumpWest != parameter_data_t->iMaxLimitOnPumpWest)
    {
        temporary_data_t->iOldMaxLimitOnPumpWest = parameter_data_t->iMaxLimitOnPumpWest;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpWest = false;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }
}

/********************************************************
    Methode für Inhalte von Seite 6

    Parameter für Minimal-Begrenzungen Solarkreis Ost
    Parameter für Minimal-Begrenzungen Solarkreis West
  ********************************************************/
void refreshPage5()
{
    // Minimal-Begrenzung Ausschaltschwelle von Solarkreis Ost
    temporary_data_t->iOldMinLimitOffPumpEast = parameter_data_t->iMinLimitOffPumpEast;
    parameter_data_t->iMinLimitOffPumpEast = myNex.readNumber("MinOffEast.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldMinLimitOffPumpEast != parameter_data_t->iMinLimitOffPumpEast)
    {
        temporary_data_t->iOldMinLimitOffPumpEast = parameter_data_t->iMinLimitOffPumpEast;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpEast = false;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    // Minimal-Begrenzung Einschaltschwelle von Solarkreis Ost
    temporary_data_t->iOldMinLimitOnPumpEast = parameter_data_t->iMinLimitOnPumpEast;
    parameter_data_t->iMinLimitOnPumpEast = myNex.readNumber("MinOnEast.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldMinLimitOnPumpEast != parameter_data_t->iMinLimitOnPumpEast)
    {
        temporary_data_t->iOldMinLimitOnPumpEast = parameter_data_t->iMinLimitOnPumpEast;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpEast = false;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    //***********************************************************************

    // Minimal-Begrenzung Ausschaltschwelle von Solarkreis West
    temporary_data_t->iOldMinLimitOffPumpWest = parameter_data_t->iMinLimitOffPumpWest;
    parameter_data_t->iMinLimitOffPumpWest = myNex.readNumber("MinOffWest.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldMinLimitOffPumpWest != parameter_data_t->iMinLimitOffPumpWest)
    {
        temporary_data_t->iOldMinLimitOffPumpWest = parameter_data_t->iMinLimitOffPumpWest;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpWest = false;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    // Minimal-Begrenzung Einschaltschwelle von Solarkreis West
    temporary_data_t->iOldMinLimitOnPumpWest = parameter_data_t->iMinLimitOnPumpWest;
    parameter_data_t->iMinLimitOnPumpWest = myNex.readNumber("MinOnWest.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldMinLimitOnPumpWest != parameter_data_t->iMinLimitOnPumpWest)
    {
        temporary_data_t->iOldMinLimitOnPumpWest = parameter_data_t->iMinLimitOnPumpWest;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpWest = false;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }
}

/********************************************************
    Methode für Inhalte von Seite 7

    Parameter für Differenzen Solarkreis Ost
    Parameter für Differenzen Solarkreis West
  ********************************************************/
void refreshPage6()
{
    // Differenz Ausschaltschwelle von Solarkreis Ost
    temporary_data_t->iOldDiffOffPumpEast = parameter_data_t->iDiffOffPumpEast;
    parameter_data_t->iDiffOffPumpEast = myNex.readNumber("DiffOffEast.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldDiffOffPumpEast != parameter_data_t->iDiffOffPumpEast)
    {
        temporary_data_t->iOldDiffOffPumpEast = parameter_data_t->iDiffOffPumpEast;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpEast = false;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    // Differenz Einschaltschwelle von Solarkreis Ost
    temporary_data_t->iOldDiffOnPumpEast = parameter_data_t->iDiffOnPumpEast;
    parameter_data_t->iDiffOnPumpEast = myNex.readNumber("DiffOnEast.val");

    // Falls sich der Wert geändert hat, speichere persistent ab
    // und schalte Pumpe aus, wegen Parameteränderung
    if (temporary_data_t->iOldDiffOnPumpEast != parameter_data_t->iDiffOnPumpEast)
    {
        temporary_data_t->iOldDiffOnPumpEast = parameter_data_t->iDiffOnPumpEast;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpEast = false;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    //***********************************************************************

    // Differenz Ausschaltschwelle von Solarkreis West
    temporary_data_t->iOldDiffOffPumpWest = parameter_data_t->iDiffOffPumpWest;
    parameter_data_t->iDiffOffPumpWest = myNex.readNumber("DiffOffWest.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldDiffOffPumpWest != parameter_data_t->iDiffOffPumpWest)
    {
        temporary_data_t->iOldDiffOffPumpWest = parameter_data_t->iDiffOffPumpWest;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpWest = false;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    // Differenz Einschaltschwelle von Solarkreis West
    temporary_data_t->iOldDiffOnPumpWest = parameter_data_t->iDiffOnPumpWest;
    parameter_data_t->iDiffOnPumpWest = myNex.readNumber("DiffOnWest.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldDiffOnPumpWest != parameter_data_t->iDiffOnPumpWest)
    {
        temporary_data_t->iOldDiffOnPumpWest = parameter_data_t->iDiffOnPumpWest;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpWest = false;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }
}

/********************************************************
    Methode für Inhalte von Seite 8

    Auswahl der Betriebsart Hand oder Automatik
    Manuelles Schalten der Ausgänge
  ********************************************************/
void refreshPage7()
{
    // Wenn Betriebsart Hand aktiv
    if (myNex.readNumber("page7.sw0.val") == 0)
    {
        // Setze Flag Automatik-Betrieb zurück
        logic_data_t->FlagIsAutomatic = false;

        // Schreibe Werte persistent in Speicher
        storeParameter.putBool("IsAutomatic", false);

        // Lese HMI und schalte Ausgang für Solarpumpe Ost
        logic_data_t->FlagPumpEast = myNex.readNumber("page7.sw1.val");
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        storeParameter.putBool("FlagPumpEast", logic_data_t->FlagPumpEast);

        // Lese HMI und schalte Ausgang für Solarpumpe West
        logic_data_t->FlagPumpWest = myNex.readNumber("page7.sw2.val");
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        storeParameter.putBool("FlagPumpWest", logic_data_t->FlagPumpWest);

        // Lese HMI und schalte Ausgang für Umladepumpe
        logic_data_t->FlagLoadPump = myNex.readNumber("page7.sw3.val");
        pcf8574.digitalWrite(2, !logic_data_t->FlagLoadPump);
        storeParameter.putBool("FlagLoadPump", logic_data_t->FlagLoadPump);

        // Lese HMI und schalte Ausgang für Umschaltventil
        logic_data_t->FlagValve = myNex.readNumber("page7.sw4.val");
        pcf8574.digitalWrite(3, !logic_data_t->FlagValve);
        storeParameter.putBool("FlagValve", logic_data_t->FlagValve);

        sendLogic();
    }
    else if (logic_data_t->FlagIsAutomatic == false)
    {
        // Setze Flag Automatik-Betrieb
        logic_data_t->FlagIsAutomatic = true;

        // Schreibe Werte persistent in Speicher
        storeParameter.putBool("IsAutomatic", true);

        // Bei Wechsel der Betriebsart -> Relais Aus
        logic_data_t->FlagPumpEast = false;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        storeParameter.putBool("FlagPumpEast", logic_data_t->FlagPumpEast);

        logic_data_t->FlagPumpWest = false;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        storeParameter.putBool("FlagPumpEast", logic_data_t->FlagPumpWest);

        logic_data_t->FlagLoadPump = false;
        pcf8574.digitalWrite(2, !logic_data_t->FlagLoadPump);
        storeParameter.putBool("FlagLoadPump", logic_data_t->FlagLoadPump);

        logic_data_t->FlagValve = false;
        pcf8574.digitalWrite(3, !logic_data_t->FlagValve);
        storeParameter.putBool("FlagValve", logic_data_t->FlagValve);

        sendLogic();
    }
}

/********************************************************
    Methode für Inhalte von Seite 9

    Parameter für Kollektor-Übertemperatur Solarkreis Ost
    Parameter für Kollektor-Übertemperatur Solarkreis West
  ********************************************************/
void refreshPage8()
{
    // Wenn Kollektor-Übertemperatur-Schutz Ost aktiv
    if (myNex.readNumber("page8.sw0.val") == 0)
    {
        logic_data_t->FlagIsHeatProtectEast = false;

        // Schreibe Werte persistent in Speicher
        storeParameter.putBool("IsHeatPrtEast", false);
        sendLogic();
    }
    else
    {
        logic_data_t->FlagIsHeatProtectEast = true;

        // Schreibe Werte persistent in Speicher
        storeParameter.putBool("IsHeatPrtEast", true);
        sendLogic();
    }

    // Wenn Kollektor-Übertemperatur-Schutz West aktiv
    if (myNex.readNumber("page8.sw1.val") == 0)
    {
        logic_data_t->FlagIsHeatProtectWest = false;

        // Schreibe Werte persistent in Speicher
        storeParameter.putBool("IsHeatPrtWest", false);
        sendLogic();
    }
    else
    {
        logic_data_t->FlagIsHeatProtectWest = true;

        // Schreibe Werte persistent in Speicher
        storeParameter.putBool("IsHeatPrtWest", true);
        sendLogic();
    }

    // Kollektor-Übertemperatur Ausschaltschwelle Solarkreis Ost
    temporary_data_t->iOldOverOffCollectorEast = parameter_data_t->iOverOffCollectorEast;
    parameter_data_t->iOverOffCollectorEast = myNex.readNumber("OverOffEast.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldOverOffCollectorEast != parameter_data_t->iOverOffCollectorEast)
    {
        temporary_data_t->iOldOverOffCollectorEast = parameter_data_t->iOverOffCollectorEast;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpEast = false;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    // Kollektor-Übertemperatur Einschaltschwelle Solarkreis Ost
    temporary_data_t->iOldOverOnCollectorEast = parameter_data_t->iOverOnCollectorEast;
    parameter_data_t->iOverOnCollectorEast = myNex.readNumber("OverOnEast.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldOverOnCollectorEast != parameter_data_t->iOverOnCollectorEast)
    {
        temporary_data_t->iOldOverOnCollectorEast = parameter_data_t->iOverOnCollectorEast;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpEast = false;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    //***********************************************************************

    // Kollektor-Übertemperatur Ausschaltschwelle Solarkreis West
    temporary_data_t->iOldOverOffCollectorWest = parameter_data_t->iOverOffCollectorWest;
    parameter_data_t->iOverOffCollectorWest = myNex.readNumber("OverOffWest.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldOverOffCollectorWest != parameter_data_t->iOverOffCollectorWest)
    {
        temporary_data_t->iOldOverOffCollectorWest = parameter_data_t->iOverOffCollectorWest;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpWest = false;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    // Kollektor-Übertemperatur Einschaltschwelle Solarkreis Ost
    temporary_data_t->iOldOverOnCollectorWest = parameter_data_t->iOverOnCollectorWest;
    parameter_data_t->iOverOnCollectorWest = myNex.readNumber("OverOnWest.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldOverOnCollectorWest != parameter_data_t->iOverOnCollectorWest)
    {
        temporary_data_t->iOldOverOnCollectorWest = parameter_data_t->iOverOnCollectorWest;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpWest = false;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }
}

/********************************************************
    Methode für Inhalte von Seite 10

    Parameter für Kollektor-Frostschutz Solarkreis Ost
    Parameter für Kollektor-Frostschutz Solarkreis West
  ********************************************************/
void refreshPage9()
{
    // Wenn Kollektor-Frostschutz-Schutz Ost aktiv
    if (myNex.readNumber("page9.sw0.val") == 0)
    {
        logic_data_t->FlagIsFreezeProtectEast = false;

        // Schreibe Werte persistent in Speicher
        storeParameter.putBool("IsFreezePrtEast", false);
        logic_data_t->FlagPumpEast = false;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpWest);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }
    else
    {
        logic_data_t->FlagIsFreezeProtectEast = true;

        // Schreibe Werte persistent in Speicher
        storeParameter.putBool("IsFreezePrtEast", true);
        sendLogic();
    }

    // Wenn Kollektor-Übertemperatur-Schutz West aktiv
    if (myNex.readNumber("page9.sw1.val") == 0)
    {
        logic_data_t->FlagIsFreezeProtectWest = false;

        // Schreibe Werte persistent in Speicher
        storeParameter.putBool("IsFreezePrtWest", false);
        logic_data_t->FlagPumpWest = false;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }
    else
    {
        logic_data_t->FlagIsFreezeProtectWest = true;

        // Schreibe Werte persistent in Speicher
        storeParameter.putBool("IsFreezePrtWest", true);
        sendLogic();
    }

    // Kollektor-Frostschutz Ausschaltschwelle Solarkreis Ost
    temporary_data_t->iOldColdOffCollectorEast = parameter_data_t->iColdOffCollectorEast;
    parameter_data_t->iColdOffCollectorEast = myNex.readNumber("ColdOffEast.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldColdOffCollectorEast != parameter_data_t->iColdOffCollectorEast)
    {
        temporary_data_t->iOldColdOffCollectorEast = parameter_data_t->iColdOffCollectorEast;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpEast = false;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    // Kollektor-Frostschutz Einschaltschwelle Solarkreis Ost
    temporary_data_t->iOldColdOnCollectorEast = parameter_data_t->iColdOnCollectorEast;
    parameter_data_t->iColdOnCollectorEast = myNex.readNumber("ColdOnEast.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldColdOnCollectorEast != parameter_data_t->iColdOnCollectorEast)
    {
        temporary_data_t->iOldColdOnCollectorEast = parameter_data_t->iColdOnCollectorEast;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpEast = false;
        pcf8574.digitalWrite(0, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    //***********************************************************************

    // Kollektor-Frostschutz Ausschaltschwelle Solarkreis West
    temporary_data_t->iOldColdOffCollectorWest = parameter_data_t->iColdOffCollectorWest;
    parameter_data_t->iColdOffCollectorWest = myNex.readNumber("ColdOffWest.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldColdOffCollectorWest != parameter_data_t->iColdOffCollectorWest)
    {
        temporary_data_t->iOldColdOffCollectorWest = parameter_data_t->iColdOffCollectorWest;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpWest = false;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    // Kollektor-Übertemperatur Einschaltschwelle Solarkreis Ost
    temporary_data_t->iOldColdOnCollectorWest = parameter_data_t->iColdOnCollectorWest;
    parameter_data_t->iColdOnCollectorWest = myNex.readNumber("ColdOnWest.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldColdOnCollectorWest != parameter_data_t->iColdOnCollectorWest)
    {
        temporary_data_t->iOldColdOnCollectorWest = parameter_data_t->iColdOnCollectorWest;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagPumpWest = false;
        pcf8574.digitalWrite(1, !logic_data_t->FlagPumpWest);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }
}

/********************************************************
    Methode für Inhalte von Seite 11

    Parameter für Kollektor-Kühlfunktion Solarkreis Ost
    Parameter für Kollektor-Kühlfunktion Solarkreis West
  ********************************************************/
void refreshPage10()
{
    /********************************************************
    Parameter für Kollektor-Kühlfunktion Solarkreis Ost
    ********************************************************/
    temporary_data_t->iOldCoolingEast = parameter_data_t->iCoolingEast;
    parameter_data_t->iCoolingEast = myNex.readNumber("CoolOnEast.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldCoolingEast != parameter_data_t->iCoolingEast)
    {
        temporary_data_t->iOldCoolingEast = parameter_data_t->iCoolingEast;
    }

    // Freigabezeit Kühlfunktion von Kollektorfeld Ost --> Zehnerstelle Stunden
    temporary_data_t->iOldReleaseHourTenEast = timer_data_t->iReleaseHourTenEast;
    timer_data_t->iReleaseHourTenEast = myNex.readNumber("FrgHourTen.val");

    // Freigabezeit Kühlfunktion von Kollektorfeld Ost -. Einerstelle Stunden
    temporary_data_t->iOldReleaseHourOneEast = timer_data_t->iReleaseHourOneEast;
    timer_data_t->iReleaseHourOneEast = myNex.readNumber("FrgHourOne.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if ((temporary_data_t->iOldReleaseHourTenEast != timer_data_t->iReleaseHourTenEast) || (temporary_data_t->iOldReleaseHourOneEast != timer_data_t->iReleaseHourOneEast))
    {
        temporary_data_t->iOldReleaseHourTenEast = timer_data_t->iReleaseHourTenEast;
        temporary_data_t->iOldReleaseHourOneEast = timer_data_t->iReleaseHourOneEast;
    }

    // Freigabezeit Kühlfunktion von Kollektorfeld Ost -. Zehnerstelle Minuten
    temporary_data_t->iOldReleaseMinTenEast = timer_data_t->iReleaseMinTenEast;
    timer_data_t->iReleaseMinTenEast = myNex.readNumber("FrgMinTen.val");

    // Freigabezeit Kühlfunktion von Kollektorfeld Ost -. Einerstelle Minuten
    temporary_data_t->iOldReleaseMinOneEast = timer_data_t->iReleaseMinOneEast;
    timer_data_t->iReleaseMinOneEast = myNex.readNumber("FrgMinOne.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if ((temporary_data_t->iOldReleaseMinTenEast != timer_data_t->iReleaseMinTenEast) || (temporary_data_t->iOldReleaseMinOneEast != timer_data_t->iReleaseMinOneEast))
    {
        temporary_data_t->iOldReleaseMinTenEast = timer_data_t->iReleaseMinTenEast;
        temporary_data_t->iOldReleaseMinOneEast = timer_data_t->iReleaseMinOneEast;
    }

    // Sperrzeit Kühlfunktion von Kollektorfeld Ost --> Zehnerstelle Stunden
    temporary_data_t->iOldBlockHourTenEast = timer_data_t->iBlockHourTenEast;
    timer_data_t->iBlockHourTenEast = myNex.readNumber("SprHourTen.val");

    // Sperrzeit Kühlfunktion von Kollektorfeld Ost -. Einerstelle Stunden
    temporary_data_t->iOldBlockHourOneEast = timer_data_t->iBlockHourOneEast;
    timer_data_t->iBlockHourOneEast = myNex.readNumber("SprHourOne.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if ((temporary_data_t->iOldBlockHourTenEast != timer_data_t->iBlockHourTenEast) || (temporary_data_t->iOldBlockHourOneEast != timer_data_t->iBlockHourOneEast))
    {
        temporary_data_t->iOldBlockHourTenEast = timer_data_t->iBlockHourTenEast;
        temporary_data_t->iOldBlockHourOneEast = timer_data_t->iBlockHourOneEast;
    }

    // Sperrzeit Kühlfunktion von Kollektorfeld Ost -. Zehnerstelle Minuten
    temporary_data_t->iOldBlockMinTenEast = timer_data_t->iBlockMinTenEast;
    timer_data_t->iBlockMinTenEast = myNex.readNumber("SprMinTen.val");

    // Sperrzeit Kühlfunktion von Kollektorfeld Ost -. Einerstelle Minuten
    temporary_data_t->iOldBlockMinOneEast = timer_data_t->iBlockMinOneEast;
    timer_data_t->iBlockMinOneEast = myNex.readNumber("SprMinOne.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if ((temporary_data_t->iOldBlockMinTenEast != timer_data_t->iBlockMinTenEast) || (temporary_data_t->iOldBlockMinOneEast != timer_data_t->iBlockMinOneEast))
    {
        temporary_data_t->iOldBlockMinTenEast = timer_data_t->iBlockMinTenEast;
        temporary_data_t->iOldBlockMinOneEast = timer_data_t->iBlockMinOneEast;
    }

    /********************************************************
        Parameter für Kollektor-Kühlfunktion Solarkreis West
    ********************************************************/
    temporary_data_t->iOldCoolingWest = parameter_data_t->iCoolingWest;
    parameter_data_t->iCoolingWest = myNex.readNumber("CoolOnWest.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldCoolingWest != parameter_data_t->iCoolingWest)
    {
        temporary_data_t->iOldCoolingWest = parameter_data_t->iCoolingWest;
    }

    // Freigabezeit Kühlfunktion von Kollektorfeld West --> Zehnerstelle Stunden
    temporary_data_t->iOldReleaseHourTenWest = timer_data_t->iReleaseHourTenWest;
    timer_data_t->iReleaseHourTenWest = myNex.readNumber("FrgHourTen1.val");

    // Freigabezeit Kühlfunktion von Kollektorfeld West -. Einerstelle Stunden
    temporary_data_t->iOldReleaseHourOneWest = timer_data_t->iReleaseHourOneWest;
    timer_data_t->iReleaseHourOneWest = myNex.readNumber("FrgHourOne1.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if ((temporary_data_t->iOldReleaseHourTenWest != timer_data_t->iReleaseHourTenWest) || (temporary_data_t->iOldReleaseHourOneWest != timer_data_t->iReleaseHourOneWest))
    {
        temporary_data_t->iOldReleaseHourTenWest = timer_data_t->iReleaseHourTenWest;
        temporary_data_t->iOldReleaseHourOneWest = timer_data_t->iReleaseHourOneWest;
    }

    // Freigabezeit Kühlfunktion von Kollektorfeld West -. Zehnerstelle Minuten
    temporary_data_t->iOldReleaseMinTenWest = timer_data_t->iReleaseMinTenWest;
    timer_data_t->iReleaseMinTenWest = myNex.readNumber("FrgMinTen1.val");

    // Freigabezeit Kühlfunktion von Kollektorfeld West -. Einerstelle Minuten
    temporary_data_t->iOldReleaseMinOneWest = timer_data_t->iReleaseMinOneWest;
    timer_data_t->iReleaseMinOneWest = myNex.readNumber("FrgMinOne1.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if ((temporary_data_t->iOldReleaseMinTenWest != timer_data_t->iReleaseMinTenWest) || (temporary_data_t->iOldReleaseMinOneWest != timer_data_t->iReleaseMinOneWest))
    {
        temporary_data_t->iOldReleaseMinTenWest = timer_data_t->iReleaseMinTenWest;
        temporary_data_t->iOldReleaseMinOneWest = timer_data_t->iReleaseMinOneWest;
    }

    // Sperrzeit Kühlfunktion von Kollektorfeld West --> Zehnerstelle Stunden
    temporary_data_t->iOldBlockHourTenWest = timer_data_t->iBlockHourTenWest;
    timer_data_t->iBlockHourTenWest = myNex.readNumber("SprHourTen1.val");

    // Sperrzeit Kühlfunktion von Kollektorfeld West -. Einerstelle Stunden
    temporary_data_t->iOldBlockHourOneWest = timer_data_t->iBlockHourOneWest;
    timer_data_t->iBlockHourOneWest = myNex.readNumber("SprHourOne1.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if ((temporary_data_t->iOldBlockHourTenWest != timer_data_t->iBlockHourTenWest) || (temporary_data_t->iOldBlockHourOneWest != timer_data_t->iBlockHourOneWest))
    {
        temporary_data_t->iOldBlockHourTenWest = timer_data_t->iBlockHourTenWest;
        temporary_data_t->iOldBlockHourOneWest = timer_data_t->iBlockHourOneWest;
    }

    // Sperrzeit Kühlfunktion von Kollektorfeld West - Zehnerstelle Minuten
    temporary_data_t->iOldBlockMinTenWest = timer_data_t->iBlockMinTenWest;
    timer_data_t->iBlockMinTenWest = myNex.readNumber("SprMinTen1.val");

    // Sperrzeit Kühlfunktion von Kollektorfeld West - Einerstelle Minuten
    temporary_data_t->iOldBlockMinOneWest = timer_data_t->iBlockMinOneWest;
    timer_data_t->iBlockMinOneWest = myNex.readNumber("SprMinOne1.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if ((temporary_data_t->iOldBlockMinTenWest != timer_data_t->iBlockMinTenWest) || (temporary_data_t->iOldBlockMinOneWest != timer_data_t->iBlockMinOneWest))
    {
        temporary_data_t->iOldBlockMinTenWest = timer_data_t->iBlockMinTenWest;
        temporary_data_t->iOldBlockMinOneWest = timer_data_t->iBlockMinOneWest;
    }
}

/********************************************************
    Methode für Inhalte von Seite 12

    Anzeige der Wärmemengenzähler
  ********************************************************/
void refreshPage11()
{
}

/********************************************************
    Methode für Inhalte von Seite 13

    Parameter für Umladepumpe
********************************************************/
void refreshPage12()
{
    /********************************************************
        Parameter für Sensorauswahl Schaltpunkt oben
    ********************************************************/
    temporary_data_t->iOldSelectUpLoadpump = parameter_data_t->iSelectUpLoadpump;
    parameter_data_t->iSelectUpLoadpump = myNex.readNumber("SelectUp.val");

    if (temporary_data_t->iOldSelectUpLoadpump != parameter_data_t->iSelectUpLoadpump)
    {
        temporary_data_t->iOldSelectUpLoadpump = parameter_data_t->iSelectUpLoadpump;        

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagLoadPump = false;
        pcf8574.digitalWrite(2, !logic_data_t->FlagLoadPump);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    /********************************************************
        Parameter für Sensorauswahl Schaltpunkt unten
    ********************************************************/
    temporary_data_t->iOldSelectDownLoadpump = parameter_data_t->iSelectDownLoadpump;
    parameter_data_t->iSelectDownLoadpump = myNex.readNumber("SelectDown.val");

    if (temporary_data_t->iOldSelectDownLoadpump != parameter_data_t->iSelectDownLoadpump)
    {
        temporary_data_t->iOldSelectDownLoadpump = parameter_data_t->iSelectDownLoadpump;        

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagLoadPump = false;
        pcf8574.digitalWrite(2, !logic_data_t->FlagLoadPump);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    /********************************************************
        Parameter für Differenz Ausschalten Umladepumpe
    ********************************************************/
    temporary_data_t->iOldDiffOffLoadpump = parameter_data_t->iDiffOffLoadpump;
    parameter_data_t->iDiffOffLoadpump = myNex.readNumber("SuppOff.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldDiffOffLoadpump != parameter_data_t->iDiffOffLoadpump)
    {
        temporary_data_t->iOldDiffOffLoadpump = parameter_data_t->iDiffOffLoadpump;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagLoadPump = false;
        pcf8574.digitalWrite(2, !logic_data_t->FlagLoadPump);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    /********************************************************
        Parameter für Differenz Einschalten Umladepumpe
    ********************************************************/
    temporary_data_t->iOldDiffOnLoadpump = parameter_data_t->iDiffOnLoadpump;
    parameter_data_t->iDiffOnLoadpump = myNex.readNumber("SuppOn.val");

    // Falls sich der Wert geändert hat, speichere persistent ab
    // und schalte Pumpe aus, wegen Parameteränderung
    if (temporary_data_t->iOldDiffOnLoadpump != parameter_data_t->iDiffOnLoadpump)
    {
        temporary_data_t->iOldDiffOnLoadpump = parameter_data_t->iDiffOnLoadpump;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagLoadPump = false;
        pcf8574.digitalWrite(2, !logic_data_t->FlagLoadPump);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    /********************************************************
        Parameter für Freigabe Umladepumpe
    ********************************************************/
    temporary_data_t->iOldTempLoadpump = parameter_data_t->iTempLoadpump;
    parameter_data_t->iTempLoadpump = myNex.readNumber("SuppRelease.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldTempLoadpump != parameter_data_t->iTempLoadpump)
    {
        temporary_data_t->iOldTempLoadpump = parameter_data_t->iTempLoadpump;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagLoadPump = false;
        pcf8574.digitalWrite(2, !logic_data_t->FlagLoadPump);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }
}

/********************************************************
    Methode für Inhalte von Seite 14

    Parameter für Umschaltventil
********************************************************/
void refreshPage13()
{
    /********************************************************
        Parameter für Differenz Ausschalten Umschaltventil
    ********************************************************/
    temporary_data_t->iOldDiffOffValve = parameter_data_t->iDiffOffValve;
    parameter_data_t->iDiffOffValve = myNex.readNumber("ValveOff.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldDiffOffValve != parameter_data_t->iDiffOffValve)
    {
        temporary_data_t->iOldDiffOffValve = parameter_data_t->iDiffOffValve;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagValve = false;
        pcf8574.digitalWrite(3, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    /********************************************************
        Parameter für Differenz Einschalten Umschaltventil
    ********************************************************/
    temporary_data_t->iOldDiffOnValve = parameter_data_t->iDiffOnValve;
    parameter_data_t->iDiffOnValve = myNex.readNumber("ValveOn.val");

    // Falls sich der Wert geändert hat, speichere persistent ab
    // und schalte Pumpe aus, wegen Parameteränderung
    if (temporary_data_t->iOldDiffOnValve != parameter_data_t->iDiffOnValve)
    {
        temporary_data_t->iOldDiffOnValve = parameter_data_t->iDiffOnValve;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagValve = false;
        pcf8574.digitalWrite(3, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }

    /********************************************************
        Parameter für Freigabe Umschaltventil
    ********************************************************/
    temporary_data_t->iOldValveRelease = parameter_data_t->iValveRelease;
    parameter_data_t->iValveRelease = myNex.readNumber("ValveRelease.val");

    // Falls sich der Wert geändert hat, wird neuer Wert zum alten Wert
    if (temporary_data_t->iOldValveRelease != parameter_data_t->iValveRelease)
    {
        temporary_data_t->iOldValveRelease = parameter_data_t->iValveRelease;

        // Schalte Ausgang ab, wegen geänderter Bedingungen
        logic_data_t->FlagValve = false;
        pcf8574.digitalWrite(3, !logic_data_t->FlagPumpEast);
        myNex.writeStr("ScreenDim.val=0");
        sendLogic();
    }
}

/********************************************************
  Parameter-Inhalte von Seite 5 senden
  Befehl --> printh 23 02 54 01 von Nextion auf
  Taste "Werte speichern" triggert Funktion

  Die Werte aus dem nicht persistenten Struct werden
  dann persistent in den Flash-Speicher geschrieben
********************************************************/
void trigger1()
{
    // Seite aktualisieren
    refreshPage4();

    // Schreibe Werte persistent in Speicher
    storeParameter.putUChar("MaxOnPumpEast", parameter_data_t->iMaxLimitOnPumpEast);
    storeParameter.putUChar("MaxOnPumpWest", parameter_data_t->iMaxLimitOnPumpWest);
    storeParameter.putUChar("MaxOffPumpEast", parameter_data_t->iMaxLimitOffPumpEast);
    storeParameter.putUChar("MaxOffPumpWest", parameter_data_t->iMaxLimitOffPumpWest);

    sendMaxLimits();

    // Blende Icon Gespeichert OK kurz ein
    myNex.writeStr("p0.aph=127");
    delay(2000);
    myNex.writeStr("p0.aph=0");
}

/********************************************************
  Parameter-Inhalte von Seite 6 senden
  Befehl --> printh 23 02 54 02 von Nextion auf
  Taste "Werte speichern" triggert Funktion

  Die Werte aus dem nicht persistenten Struct werden
  dann persistent in den Flash-Speicher geschrieben
********************************************************/
void trigger2()
{
    // Seite aktualisieren
    refreshPage5();

    // Schreibe Werte persistent in Speicher
    storeParameter.putUChar("MinOnPumpEast", parameter_data_t->iMinLimitOnPumpEast);
    storeParameter.putUChar("MinOnPumpWest", parameter_data_t->iMinLimitOnPumpWest);
    storeParameter.putUChar("MinOffPumpEast", parameter_data_t->iMinLimitOffPumpEast);
    storeParameter.putUChar("MinOffPumpWest", parameter_data_t->iMinLimitOffPumpWest);

    sendMinLimits();

    // Blende Icon Gespeichert OK kurz ein
    myNex.writeStr("p0.aph=127");
    delay(2000);
    myNex.writeStr("p0.aph=0");
}

/********************************************************
  Parameter-Inhalte von Seite 7 senden
  Befehl --> printh 23 02 54 03 von Nextion auf
  Taste "Werte speichern" triggert Funktion

  Die Werte aus dem nicht persistenten Struct werden
  dann persistent in den Flash-Speicher geschrieben
********************************************************/
void trigger3()
{
    // Seite aktualisieren
    refreshPage6();

    // Schreibe Werte persistent in Speicher
    storeParameter.putUChar("DiffOnPumpEast", parameter_data_t->iDiffOnPumpEast);
    storeParameter.putUChar("DiffOnPumpWest", parameter_data_t->iDiffOnPumpWest);
    storeParameter.putUChar("DiffOffPumpEast", parameter_data_t->iDiffOffPumpEast);
    storeParameter.putUChar("DiffOffPumpWest", parameter_data_t->iDiffOffPumpWest);

    sendDiffValues();

    // Blende Icon Gespeichert OK kurz ein
    myNex.writeStr("p0.aph=127");
    delay(2000);
    myNex.writeStr("p0.aph=0");
}

/********************************************************
  Parameter-Inhalte von Seite 9 senden
  Befehl --> printh 23 02 54 04 von Nextion auf
  Taste "Werte speichern" triggert Funktion

  Die Werte aus dem nicht persistenten Struct werden
  dann persistent in den Flash-Speicher geschrieben
********************************************************/
void trigger4()
{
    // Seite aktualisieren
    refreshPage8();

    // Schreibe Werte persistent in Speicher
    storeParameter.putUChar("OverOnEast", parameter_data_t->iOverOnCollectorEast);
    storeParameter.putUChar("OverOnWest", parameter_data_t->iOverOnCollectorWest);
    storeParameter.putUChar("OverOffEast", parameter_data_t->iOverOffCollectorEast);
    storeParameter.putUChar("OverOffWest", parameter_data_t->iOverOffCollectorWest);

    sendOverTempValues();

    // Blende Icon Gespeichert OK kurz ein
    myNex.writeStr("p0.aph=127");
    delay(2000);
    myNex.writeStr("p0.aph=0");
}

/********************************************************
  Parameter-Inhalte von Seite 10 senden
  Befehl --> printh 23 02 54 05 von Nextion auf
  Taste "Werte speichern" triggert Funktion

  Die Werte aus dem nicht persistenten Struct werden
  dann persistent in den Flash-Speicher geschrieben
********************************************************/
void trigger5()
{
    // Seite aktualisieren
    refreshPage9();

    // Schreibe Werte persistent in Speicher
    storeParameter.putChar("ColdOnEast", parameter_data_t->iColdOnCollectorEast);
    storeParameter.putChar("ColdOnWest", parameter_data_t->iColdOnCollectorWest);
    storeParameter.putChar("ColdOffEast", parameter_data_t->iColdOffCollectorEast);
    storeParameter.putChar("ColdOffWest", parameter_data_t->iColdOffCollectorWest);

    sendFreezeProtectValues();

    // Blende Icon Gespeichert OK kurz ein
    myNex.writeStr("p0.aph=127");
    delay(2000);
    myNex.writeStr("p0.aph=0");
}

/********************************************************
  Parameter-Inhalte von Seite 11 senden
  Befehl --> printh 23 02 54 06 von Nextion auf
  Taste "Werte speichern" triggert Funktion

  Die Werte aus dem nicht persistenten Struct werden
  dann persistent in den Flash-Speicher geschrieben
********************************************************/
void trigger6()
{
    // Seite aktualisieren
    refreshPage10();

    // Schreibe Werte für Kühlfunktion Ost persistent in Speicher
    storeParameter.putUChar("RelHrTenEast", timer_data_t->iReleaseHourTenEast);
    storeParameter.putUChar("RelHrOneEast", timer_data_t->iReleaseHourOneEast);
    storeParameter.putUChar("RelMinTenEast", timer_data_t->iReleaseMinTenEast);
    storeParameter.putUChar("RelMinOneEast", timer_data_t->iReleaseMinOneEast);

    storeParameter.putUChar("BlkHrTenEast", timer_data_t->iBlockHourTenEast);
    storeParameter.putUChar("BlkHrOneEast", timer_data_t->iBlockHourOneEast);
    storeParameter.putUChar("BlkMinTenEast", timer_data_t->iBlockMinTenEast);
    storeParameter.putUChar("BlkMinOneEast", timer_data_t->iBlockMinOneEast);

    storeParameter.putUChar("CoolingEast", parameter_data_t->iCoolingEast);

    sendTimerEast();

    // Schreibe Werte für Kühlfunktion West persistent in Speicher
    storeParameter.putUChar("RelHrTenWest", timer_data_t->iReleaseHourTenWest);
    storeParameter.putUChar("RelHrOneWest", timer_data_t->iReleaseHourOneWest);
    storeParameter.putUChar("RelMinTenWest", timer_data_t->iReleaseMinTenWest);
    storeParameter.putUChar("RelMinOneWest", timer_data_t->iReleaseMinOneWest);

    storeParameter.putUChar("BlkHrTenWest", timer_data_t->iBlockHourTenWest);
    storeParameter.putUChar("BlkHrOneWest", timer_data_t->iBlockHourOneWest);
    storeParameter.putUChar("BlkMinTenWest", timer_data_t->iBlockMinTenWest);
    storeParameter.putUChar("BlkMinOneWest", timer_data_t->iBlockMinOneWest);

    storeParameter.putUChar("CoolingWest", parameter_data_t->iCoolingWest);

    sendTimerWest();

    // Blende Icon Gespeichert OK kurz ein
    myNex.writeStr("p0.aph=127");
    delay(2000);
    myNex.writeStr("p0.aph=0");
}

/********************************************************
  Parameter-Inhalte von Seite 13 senden
  Befehl --> printh 23 02 54 07 von Nextion auf
  Taste "Werte speichern" triggert Funktion

  Die Werte aus dem nicht persistenten Struct werden
  dann persistent in den Flash-Speicher geschrieben
********************************************************/
void trigger7()
{
    // Seite aktualisieren
    refreshPage12();

    // Schreibe Werte persistent in Speicher
    storeParameter.putUChar("SelectUpLoad", parameter_data_t->iSelectUpLoadpump);
    storeParameter.putUChar("SelectDownLoad", parameter_data_t->iSelectDownLoadpump);
    storeParameter.putUChar("TempLoad", parameter_data_t->iTempLoadpump);
    storeParameter.putUChar("DiffOnLoad", parameter_data_t->iDiffOnLoadpump);
    storeParameter.putUChar("DiffOffLoad", parameter_data_t->iDiffOffLoadpump);

    sendLoadPumpValues();

    // Blende Icon Gespeichert OK kurz ein
    myNex.writeStr("p0.aph=127");
    delay(2000);
    myNex.writeStr("p0.aph=0");
}

/********************************************************
  Parameter-Inhalte von Seite 14 senden
  Befehl --> printh 23 02 54 08 von Nextion auf
  Taste "Werte speichern" triggert Funktion

  Die Werte aus dem nicht persistenten Struct werden
  dann persistent in den Flash-Speicher geschrieben
********************************************************/
void trigger8()
{
    // Seite aktualisieren
    refreshPage13();

    // Schreibe Werte persistent in Speicher
    storeParameter.putUChar("DiffOnValve", parameter_data_t->iDiffOnValve);
    storeParameter.putUChar("DiffOffValve", parameter_data_t->iDiffOffValve);
    storeParameter.putUChar("TempValve", parameter_data_t->iValveRelease);

    sendValveValues();

    // Blende Icon Gespeichert OK kurz ein
    myNex.writeStr("p0.aph=127");
    delay(2000);
    myNex.writeStr("p0.aph=0");
}
