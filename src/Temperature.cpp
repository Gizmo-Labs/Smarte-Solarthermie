/********************************************************
  Bibliotheken
********************************************************/
#include "Prototypes.h"

/********************************************************
  1-Wire Temperaturmessung
********************************************************/
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
DeviceAddress devAddr[ONE_WIRE_MAX_DEV];

float fTemp_Snapshot = 0.0;
float fTemp_Ruecklauf = 0.0;
float fTemp_Vorlauf = 0.0;
float fTemp_Warmwasser = 0.0;
float fTemp_Puffer_Unten = 0.0;
float fTemp_Puffer_Mitte = 0.0;
float fTempDev[ONE_WIRE_MAX_DEV];
float fTempDevLast[ONE_WIRE_MAX_DEV];
float fTempChip = 0.0;

int iNumberOfDevices = 0;
long lLastTemp = 0;

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
Adafruit_MAX31865 Collector_West = Adafruit_MAX31865(10);
Adafruit_MAX31865 Collector_East = Adafruit_MAX31865(15);

float fTemp_Collector_East = 0.0;
float fLast_Temp_Collector_East = 0.0;
float fTemp_Collector_West = 0.0;
float fLast_Temp_Collector_West = 0.0;

int8_t iFault_West = 0;
int8_t iFault_East = 0;

/********************************************************
  Konvertiere I2C-Adresse zu String
********************************************************/
String GetAddressToString(DeviceAddress deviceAddress)
{
    String str = "";
    for (uint8_t i = 0; i < 8; i++)
    {
        if (deviceAddress[i] < 16)
            str += String(0, HEX);
        str += String(deviceAddress[i], HEX);
    }
    return str;
}

/********************************************************
  Konvertiere MAX31865-Fehler zu Sting
********************************************************/
String ConvertFaultToString()
{
    iFault_West = Collector_West.readFault();
    iFault_East = Collector_East.readFault();

    if (((iFault_West)&MAX31865_FAULT_HIGHTHRESH) || ((iFault_East)&MAX31865_FAULT_HIGHTHRESH))
    {
        return "Fehler Kollektorsensoren -> Unterbrechung!";
    }
    else if (((iFault_West)&MAX31865_FAULT_LOWTHRESH) || ((iFault_East)&MAX31865_FAULT_LOWTHRESH))
    {
        return "Fehler Kollektorsensoren -> Kurzschluss!";
    }
    else if (((iFault_West)&MAX31865_FAULT_REFINLOW) || ((iFault_East)&MAX31865_FAULT_REFINLOW))
    {
        return "Fehler Kollektorsensoren -> REFIN -> 0.85 x Bias!";
    }
    else if (((iFault_West)&MAX31865_FAULT_REFINHIGH) || ((iFault_East)&MAX31865_FAULT_REFINHIGH))
    {
        return "Fehler Kollektorsensoren -> REFIN -< 0.85 x Bias - FORCE - open!";
    }
    else if (((iFault_West)&MAX31865_FAULT_RTDINLOW) || ((iFault_East)&MAX31865_FAULT_RTDINLOW))
    {
        return "Fehler Kollektorsensoren -> RTDIN -< 0.85 x Bias - FORCE - open!";
    }
    else if (((iFault_West)&MAX31865_FAULT_OVUV) || ((iFault_East)&MAX31865_FAULT_OVUV))
    {
        return "Fehler Kollektorsensoren -> Über- oder Unterspannung!";
    }
    else
    {
        Collector_East.clearFault();
        Collector_West.clearFault();
        return "Kollektorsensoren -> Keine Fehler";
    }
}

/********************************************************
  MAX31865 initalisieren
********************************************************/
void initMAX()
{
    SPI.begin(13, 12, 11);
    Collector_West.begin(MAX31865_2WIRE);
    Collector_East.begin(MAX31865_2WIRE);
}

/********************************************************
  Parametrierung DS18B20
 ********************************************************/
void initDS18B20()
{
    temp_sensor_start();
    DS18B20.begin();

    iNumberOfDevices = DS18B20.getDeviceCount();
    lLastTemp = millis();
    DS18B20.requestTemperatures();

    // Ausgabe der Adresse von allen erkannten Teilnehmern
    for (int i = 0; i < iNumberOfDevices; i++)
    {
        // Suche nach Adressen
        if (DS18B20.getAddress(devAddr[i], i))
        {
            Serial.println("*************************");
            Serial.print("Sensor erkannt ");
            Serial.print(i + 1, DEC);
            Serial.println(" mit Adresse: " + GetAddressToString(devAddr[i]));
            Serial.println("*************************");
            Serial.println();
        }
    }
    buildStatusJson("Es wurden " + String(iNumberOfDevices) + " DS18B20 Sensoren erkannt!");
    delay(2000);
}

/********************************************************
    Methode zur Temperaturmessung mit 1-Wire
  ********************************************************/
void TempLoop(long now)
{
    if (now - lLastTemp > MESSINTERVALL)
    {
        for (int i = 0; i < iNumberOfDevices; i++)
        {
            fTempDevLast[i] = fTempDev[i];
            fTemp_Snapshot = DS18B20.getTempC(devAddr[i]);
            fTempDev[i] = fTemp_Snapshot;

            if (fTempDev[i] < 0)
            {
                fTempDev[i] = fTempDevLast[i];
            }
        }

        DS18B20.setWaitForConversion(false);
        DS18B20.requestTemperatures();
        lLastTemp = millis();

        fTemp_Ruecklauf = fTempDev[4];
        fTemp_Vorlauf = fTempDev[3];
        fTemp_Warmwasser = fTempDev[2];
        fTemp_Puffer_Unten = fTempDev[1];
        fTemp_Puffer_Mitte = fTempDev[0];

        fTemp_Collector_East = Collector_East.temperature(RNOMINAL, 4280.0);    
        fTemp_Collector_West = Collector_West.temperature(RNOMINAL, 4298.0);        
        temp_sensor_read_celsius(&fTempChip);
    }
}
