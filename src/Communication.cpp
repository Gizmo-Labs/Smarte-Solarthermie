/********************************************************
  Bibliotheken
********************************************************/
#include "Prototypes.h"

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}

#include <AsyncMQTT_ESP32.h>

/********************************************************
  Definition Globale Variablen Kommunikation
********************************************************/
String IP = "IP148";
const char *host = "Solar-Control";
const char *Publish_Json = "Keller/Solar/Publisher";
const char *Subscribe_Json = "Keller/Solar/Controls";

SpiRamAllocator allocator;

/********************************************************
  Externe Referenzen
********************************************************/
extern LOGIC *logic_data_t;
extern PARAMETER *parameter_data_t;
extern TIMER *timer_data_t;

/********************************************************
  MQTT-Handler
********************************************************/
#define MQTT_HOST IPAddress(192, 168, 178, 200)
#define MQTT_PORT 1883

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

/********************************************************
  Starte WiFi-Verbindung
********************************************************/
void WiFiStart()
{
  /************* Vor Verbinden --> Trennen *************/
  WiFi.mode(WIFI_MODE_NULL);
  /*****************************************************/

  WiFi.setAutoReconnect(false);
  WiFi.setHostname(host);

  /****************** Jetzt Verbinden ******************/
  WiFi.mode(WIFI_STA);
  WiFi.begin("FURZ!Box 7362 SL", "301180mst");

  /****************** SSID anzeigen ********************/
  Serial.println("SSID: " + WiFi.SSID());
  /*****************************************************/

  /******************* IP anzeigen *********************/
  String ipaddress = WiFi.localIP().toString();
  Serial.println("IP-ADDRESSE: " + ipaddress);
  /*****************************************************/

  /******************* MAC anzeigen ********************/
  Serial.println("MAC-ADDRESSE: " + WiFi.macAddress());
  /*****************************************************/
}

/********************************************************
  Starte WiFi-Verbindung [Kurzform]
********************************************************/
void connectToWifi()
{
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin("FURZ!Box 7362 SL", "301180mst");
}

/********************************************************
  Konvertiere ESP32 Reset-Grund zu String
********************************************************/
String ConvertResetReasonToString(esp_reset_reason_t reason)
{
  switch (reason)
  {
  case ESP_RST_UNKNOWN:
    return "Resetgrund unbekannt";
    break;

  case ESP_RST_POWERON:
    return "Reset durch Poweron";
    break;

  case ESP_RST_EXT:
    return "Reset von Extern";
    break;

  case ESP_RST_SW:
    return "Reset durch Software";
    break;

  case ESP_RST_PANIC:
    return "Reset durch Exception";
    break;

  case ESP_RST_INT_WDT:
    return "Reset durch Interupt Watchdog";
    break;

  case ESP_RST_DEEPSLEEP:
    return "Reset nach Deep Sleep";
    break;

  case ESP_RST_BROWNOUT:
    return "Brownout Reset";
    break;

  case ESP_RST_SDIO:
    return "Reset durch SDIO";
    break;

  default:
    return "Resetgrund unbekannt";
    break;
  }
}

/********************************************************
  Starte Mqtt-Verbindung
********************************************************/
void connectToMqtt()
{
  if (DEBUG_COMMUNICATION)
    Serial.println("Verbinde mit MQTT-Broker...");
  mqttClient.connect();
}

/********************************************************
  WiFi-Eventhandling
********************************************************/
void WiFiEvent(WiFiEvent_t event)
{
  if (DEBUG_COMMUNICATION)
    Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event)
  {
  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.println(WiFi.localIP());
    connectToMqtt();
    break;

  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("WiFi lost connection");
    xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    xTimerStart(wifiReconnectTimer, 0);
    break;
  }
}

/********************************************************
  Trennlinie auf serieller Schnittstelle
********************************************************/
void printSeparationLine()
{
  if (DEBUG_COMMUNICATION)
    Serial.println("************************************************");
}

/********************************************************
  Event für MQTT-Verbindung erfolgreich aufgebaut
********************************************************/
void onMqttConnect(bool sessionPresent)
{
  if (DEBUG_COMMUNICATION)
  {
    Serial.print("Verbunden mit MQTT-Broker: ");
    Serial.print(MQTT_HOST);
    Serial.print(", Port: ");
    Serial.println(MQTT_PORT);
  }

  printSeparationLine();

  if (DEBUG_COMMUNICATION)
  {
    Serial.print("Session aktiv: ");
    Serial.println(sessionPresent);
  }

  printSeparationLine();

  uint16_t packetIdSub1 = mqttClient.subscribe(Subscribe_Json, 0);
  if (DEBUG_COMMUNICATION)
  {
    Serial.print("Subscribing at QoS 0, packetId: ");
    Serial.println(packetIdSub1);
  }

  printSeparationLine();

  uint16_t packetIdSub2 = mqttClient.subscribe(Publish_Json, 0);
  if (DEBUG_COMMUNICATION)
  {
    Serial.print("Subscribing at QoS 0, packetId: ");
    Serial.println(packetIdSub2);
  }

  printSeparationLine();

  buildStatusJson("Hallo von Keller Solar Publisher");
  buildStatusJson("Hallo von Keller Solar Subscriber");
}

/********************************************************
  Event für MQTT-Verbindung getrennt
********************************************************/
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  (void)reason;

  if (DEBUG_COMMUNICATION)
    Serial.println("Getrennt von MQTT.");

  if (WiFi.isConnected())
  {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

/********************************************************
  Event für MQTT-Subscribe
********************************************************/
void onMqttSubscribe(const uint16_t &packetId, const uint8_t &qos)
{
  if (DEBUG_COMMUNICATION)
  {
    Serial.println("Subscribe quittiert.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
    Serial.print("  qos: ");
    Serial.println(qos);
  }
}

/********************************************************
  Event für MQTT-Unsubscribe
********************************************************/
void onMqttUnsubscribe(const uint16_t &packetId)
{
  if (DEBUG_COMMUNICATION)
  {
    Serial.println("Unsubscribe quittiert.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
  }
}

/********************************************************
  Event für MQTT-Nachricht erhalten
********************************************************/
void onMqttMessage(char *topic, char *payload, const AsyncMqttClientMessageProperties &properties,
                   const size_t &len, const size_t &index, const size_t &total)
{
  (void)payload;

  if (DEBUG_COMMUNICATION)
  {
    Serial.println("Publish empfangen!");
    Serial.print("Topic: ");
    Serial.println(topic);
    Serial.print("Payload: ");
    Serial.println(payload);
  }

  JsonDocument doc(&allocator);

  // Deserialisieren vom JSON-Payload
  DeserializationError error = deserializeJson(doc, payload);

  if (error)
  {
    if (DEBUG_COMMUNICATION)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    }
    return;
  }

  // Bei Message an Topic "Keller/Solar/Controls" ...
  if (String(topic).indexOf("Keller/Solar/Controls") >= 0)
  {
    cDisplay_Prozent = doc["IPSView.ESP_IP148_Display_Prozent"];
    sDisplay_Prozent = String(cDisplay_Prozent);
    iDisplay_Prozent = sDisplay_Prozent.toInt();

    cDisplay_Dim = doc["IPSView.ESP_IP148_Display_Dimmzeit"];
    sDisplay_Dim = String(cDisplay_Dim);
    iDisplay_Dim = sDisplay_Dim.toInt();

    myNex.writeStr("page 3");

    myNex.writeNum("dim", iDisplay_Prozent);
    myNex.writeNum("page3.SetBrightness.val", iDisplay_Prozent);
    myNex.writeNum("page3.PercentBright.val", iDisplay_Prozent);
    myNex.writeStr("ScreenDim.val=0");
    myNex.writeNum("page3.SetDimTime.val", iDisplay_Dim);
    myNex.writeNum("page3.MinutesDim.val", iDisplay_Dim);

    delay(1000);
    myNex.writeStr("page 0");
  }
}

/********************************************************
  Event für MQTT-Publish erfolgt
********************************************************/
void onMqttPublish(const uint16_t &packetId)
{
  if (DEBUG_COMMUNICATION)
  {
    Serial.println("Publish erledigt.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
  }
}

/********************************************************
  MQTT-Verbindung initialisieren
********************************************************/
void initMqtt()
{
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0,
                                    reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0,
                                    reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
}

/********************************************************
  Speicherinhalt senden
********************************************************/
void sendStorage()
{
  String message;
  JsonDocument status(&allocator);

  status["Maximalwert Ost Einschaltschwelle"] = storeParameter.getUChar("MaxOnPumpEast", 75);
  status["Maximalwert West Einschaltschwelle"] = storeParameter.getUChar("MaxOnPumpWest", 75);
  status["Maximalwert Ost Ausschaltschwelle"] = storeParameter.getUChar("MaxOffPumpEast", 70);
  status["Maximalwert West Ausschaltschwelle"] = storeParameter.getUChar("MaxOffPumpWest", 70);

  status["Minimalwert Ost Einschaltschwelle"] = storeParameter.getUChar("MinOnPumpEast", 5);
  status["Minimalwert West Einschaltschwelle"] = storeParameter.getUChar("MinOnPumpWest", 5);
  status["Minimalwert Ost Ausschaltschwelle"] = storeParameter.getUChar("MinOffPumpEast", 0);
  status["Minimalwert West Ausschaltschwelle"] = storeParameter.getUChar("MinOffPumpWest", 0);

  status["Differenz Ost Einschaltschwelle"] = storeParameter.getUChar("DiffOnPumpEast", 8);
  status["Differenz West Einschaltschwelle"] = storeParameter.getUChar("DiffOnPumpWest", 8);
  status["Differenz Ost Ausschaltschwelle"] = storeParameter.getUChar("DiffOffPumpEast", 4);
  status["Differenz West Ausschaltschwelle"] = storeParameter.getUChar("DiffOffPumpWest", 4);

  status["Übertemperatur Ost Einschaltschwelle"] = storeParameter.getUChar("OverOnEast", 110);
  status["Übertemperatur West Einschaltschwelle"] = storeParameter.getUChar("OverOnWest", 110);
  status["Übertemperatur Ost Ausschaltschwelle"] = storeParameter.getUChar("OverOffEast", 130);
  status["Übertemperatur West Ausschaltschwelle"] = storeParameter.getUChar("OverOffWest", 130);

  status["Frostschutz Ost Einschaltschwelle"] = storeParameter.getChar("ColdOnEast", 2);
  status["Frostschutz West Einschaltschwelle"] = storeParameter.getChar("ColdOnWest", 2);
  status["Frostschutz Ost Ausschaltschwelle"] = storeParameter.getChar("ColdOffEast", 4);
  status["Frostschutz West Ausschaltschwelle"] = storeParameter.getChar("ColdOffWest", 4);

  status["Sollwert Kühlfunktion Ost"] = storeParameter.getUChar("CoolingEast", 80);
  status["Freigabezeit Ost Zehnerstelle Stunde"] = storeParameter.getUChar("RelHrTenEast", 1);
  status["Freigabezeit Ost Einerstelle Stunde"] = storeParameter.getUChar("RelHrOneEast", 2);
  status["Freigabezeit Ost Zehnerstelle Minute"] = storeParameter.getUChar("RelMinTenEast", 0);
  status["Freigabezeit Ost Einerstelle Minute"] = storeParameter.getUChar("RelMinOneEast", 0);

  status["Sperrzeit Ost Zehnerstelle Stunde"] = storeParameter.getUChar("BlkHrTenEast", 1);
  status["Sperrzeit Ost Einerstelle Stunde"] = storeParameter.getUChar("BlkHrOneEast", 2);
  status["Sperrzeit Ost Zehnerstelle Minute"] = storeParameter.getUChar("BlkMinTenEast", 0);
  status["Sperrzeit Ost Einerstelle Minute"] = storeParameter.getUChar("BlkMinOneEast", 0);

  status["Sollwert Kühlfunktion West"] = storeParameter.getUChar("CoolingWest", 80);
  status["Freigabezeit West Zehnerstelle Stunde"] = storeParameter.getUChar("RelHrTenWest", 1);
  status["Freigabezeit West Einerstelle Stunde"] = storeParameter.getUChar("RelHrOneWest", 2);
  status["Freigabezeit West Zehnerstelle Minute"] = storeParameter.getUChar("RelMinTenWEst", 0);
  status["Freigabezeit West Einerstelle Minute"] = storeParameter.getUChar("RelMinOneWest", 0);

  status["Sperrzeit West Zehnerstelle Stunde"] = storeParameter.getUChar("BlkHrTenWest", 1);
  status["Sperrzeit West Einerstelle Stunde"] = storeParameter.getUChar("BlkHrOneWest", 2);
  status["Sperrzeit West Zehnerstelle Minute"] = storeParameter.getUChar("BlkMinTenWest", 0);
  status["Sperrzeit West Einerstelle Minute"] = storeParameter.getUChar("BlkMinOneWest", 0);

  status["Umschaltventil Einschaltschwelle"] = storeParameter.getUChar("DiffOnValve", 10);
  status["Umschaltventil Ausschaltschwelle"] = storeParameter.getUChar("DiffOffValve", 5);
  status["Umschaltventil Freigabetemperatur"] = storeParameter.getUChar("TempValve", 50);

  status["Umladepumpe Sensor Oben"] = parameter_data_t->iSelectUpLoadpump;
  status["Umladepumpe Sensor Unten"] = parameter_data_t->iSelectDownLoadpump;
  status["Umladepumpe Einschaltschwelle"] = parameter_data_t->iDiffOnLoadpump;
  status["Umladepumpe Ausschaltschwelle"] = parameter_data_t->iDiffOffLoadpump;
  status["Umladepumpe Freigabetemperatur"] = parameter_data_t->iTempLoadpump;

  serializeJsonPretty(status, message);

  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}

/********************************************************
  Minimal-Limits senden
********************************************************/
void sendMinLimits()
{
  String message;
  JsonDocument status(&allocator);

  // Publish angepasste Parameter
  status["IPSView.ESP_" + IP + "_MinLimitOnPumpEast"] = parameter_data_t->iMinLimitOnPumpEast;
  status["IPSView.ESP_" + IP + "_MinLimitOnPumpWest"] = parameter_data_t->iMinLimitOnPumpWest;
  status["IPSView.ESP_" + IP + "_MinLimitOffPumpEast"] = parameter_data_t->iMinLimitOffPumpEast;
  status["IPSView.ESP_" + IP + "_MinLimitOffPumpWest"] = parameter_data_t->iMinLimitOffPumpWest;

  serializeJsonPretty(status, message);

  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}

/********************************************************
  Maximal-Limits senden
********************************************************/
void sendMaxLimits()
{
  String message;
  JsonDocument status(&allocator);

  // Publish angepasste Parameter
  status["IPSView.ESP_" + IP + "_MaxLimitOnPumpEast"] = parameter_data_t->iMaxLimitOnPumpEast;
  status["IPSView.ESP_" + IP + "_MaxLimitOnPumpWest"] = parameter_data_t->iMaxLimitOnPumpWest;
  status["IPSView.ESP_" + IP + "_MaxLimitOffPumpEast"] = parameter_data_t->iMaxLimitOffPumpEast;
  status["IPSView.ESP_" + IP + "_MaxLimitOffPumpWest"] = parameter_data_t->iMaxLimitOffPumpWest;

  serializeJsonPretty(status, message);

  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}

/********************************************************
  Differenzwert senden
********************************************************/
void sendDiffValues()
{
  String message;
  JsonDocument status(&allocator);

  // Publish angepasste Parameter
  status["IPSView.ESP_" + IP + "_DiffOnPumpEast"] = parameter_data_t->iDiffOnPumpEast;
  status["IPSView.ESP_" + IP + "_DiffOnPumpWest"] = parameter_data_t->iDiffOnPumpWest;
  status["IPSView.ESP_" + IP + "_DiffOffPumpEast"] = parameter_data_t->iDiffOffPumpEast;
  status["IPSView.ESP_" + IP + "_DiffOffPumpWest"] = parameter_data_t->iDiffOffPumpWest;

  serializeJsonPretty(status, message);

  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}

/********************************************************
  Übertemperatur-Grenzwerte senden
********************************************************/
void sendOverTempValues()
{
  String message;
  JsonDocument status(&allocator);

  // Publish angepasste Parameter
  status["IPSView.ESP_" + IP + "_OverOnCollectorEast"] = parameter_data_t->iOverOnCollectorEast;
  status["IPSView.ESP_" + IP + "_OverOnCollectorWest"] = parameter_data_t->iOverOnCollectorWest;
  status["IPSView.ESP_" + IP + "_OverOffCollectorEast"] = parameter_data_t->iOverOffCollectorEast;
  status["IPSView.ESP_" + IP + "_OverOffCollectorWest"] = parameter_data_t->iOverOffCollectorWest;

  serializeJsonPretty(status, message);

  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}

/********************************************************
  Frostschutz-Grenzwerte senden
********************************************************/
void sendFreezeProtectValues()
{
  String message;
  JsonDocument status(&allocator);

  // Publish angepasste Parameter
  status["IPSView.ESP_" + IP + "_ColdOnCollectorEast"] = parameter_data_t->iColdOnCollectorEast;
  status["IPSView.ESP_" + IP + "_ColdOnCollectorWest"] = parameter_data_t->iColdOnCollectorWest;
  status["IPSView.ESP_" + IP + "_ColdOffCollectorEast"] = parameter_data_t->iColdOffCollectorEast;
  status["IPSView.ESP_" + IP + "_ColdOffCollectorWest"] = parameter_data_t->iColdOffCollectorWest;

  serializeJsonPretty(status, message);

  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}

/********************************************************
  Schaltzeiten Kühlfunktion Kollektor Ost senden
********************************************************/
void sendTimerEast()
{
  String message;
  JsonDocument status(&allocator);

  // Publish angepasste Parameter
  status["IPSView.ESP_" + IP + "_ReleaseHourTenEast"] = timer_data_t->iReleaseHourTenEast;
  status["IPSView.ESP_" + IP + "_ReleaseHourOneEast"] = timer_data_t->iReleaseHourOneEast;
  status["IPSView.ESP_" + IP + "_ReleaseMinuteTenEast"] = timer_data_t->iReleaseMinTenEast;
  status["IPSView.ESP_" + IP + "_ReleaseMinuteOneEast"] = timer_data_t->iReleaseMinOneEast;

  status["IPSView.ESP_" + IP + "_BlockHourTenEast"] = timer_data_t->iBlockHourTenEast;
  status["IPSView.ESP_" + IP + "_BlockHourOneEast"] = timer_data_t->iBlockHourOneEast;
  status["IPSView.ESP_" + IP + "_BlockMinuteTenEast"] = timer_data_t->iBlockMinTenEast;
  status["IPSView.ESP_" + IP + "_BlockMinuteOneEast"] = timer_data_t->iBlockMinOneEast;

  status["IPSView.ESP_" + IP + "_CoolingEast"] = parameter_data_t->iCoolingEast;

  serializeJsonPretty(status, message);

  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}

/********************************************************
  Schaltzeiten Kühlfunktion Kollektor West senden
********************************************************/
void sendTimerWest()
{
  String message;
  JsonDocument status(&allocator);

  // Publish angepasste Parameter
  status["IPSView.ESP_" + IP + "_ReleaseHourTenWest"] = timer_data_t->iReleaseHourTenWest;
  status["IPSView.ESP_" + IP + "_ReleaseHourOneWest"] = timer_data_t->iReleaseHourOneWest;
  status["IPSView.ESP_" + IP + "_ReleaseMinuteTenWest"] = timer_data_t->iReleaseMinTenWest;
  status["IPSView.ESP_" + IP + "_ReleaseMinuteOneWest"] = timer_data_t->iReleaseMinOneWest;

  status["IPSView.ESP_" + IP + "_BlockHourTenWest"] = timer_data_t->iBlockHourTenWest;
  status["IPSView.ESP_" + IP + "_BlockHourOneWest"] = timer_data_t->iBlockHourOneWest;
  status["IPSView.ESP_" + IP + "_BlockMinuteTenWest"] = timer_data_t->iBlockMinTenWest;
  status["IPSView.ESP_" + IP + "_BlockMinuteOneWest"] = timer_data_t->iBlockMinOneWest;

  status["IPSView.ESP_" + IP + "_CoolingWest"] = parameter_data_t->iCoolingWest;

  serializeJsonPretty(status, message);

  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}

/********************************************************
  Steuerlogik-Inhalte senden
********************************************************/
void sendLogic()
{
  String message;
  JsonDocument doc;

  doc["IPSView.ESP_" + IP + "_Status_Solarpumpe_Ost"] = logic_data_t->FlagPumpEast;
  doc["IPSView.ESP_" + IP + "_Status_Solarpumpe_West"] = logic_data_t->FlagPumpWest;
  doc["IPSView.ESP_" + IP + "_Status_Umladepumpe"] = logic_data_t->FlagLoadPump;
  doc["IPSView.ESP_" + IP + "_Status_Umschaltventil"] = logic_data_t->FlagValve;
  doc["IPSView.ESP_" + IP + "_Status_Relais_5"] = logic_data_t->FlagReserve5;
  doc["IPSView.ESP_" + IP + "_Status_Relais_6"] = logic_data_t->FlagReserve6;
  doc["IPSView.ESP_" + IP + "_Status_Relais_7"] = logic_data_t->FlagReserve7;
  doc["IPSView.ESP_" + IP + "_Status_Relais_8"] = logic_data_t->FlagReserve8;
  doc["IPSView.ESP_" + IP + "_Status_Auto_Hand"] = logic_data_t->FlagIsAutomatic;
  doc["IPSView.ESP_" + IP + "_Temperaturschutz_Ost"] = logic_data_t->FlagIsHeatProtectEast;
  doc["IPSView.ESP_" + IP + "_Temperaturschutz_West"] = logic_data_t->FlagIsHeatProtectWest;
  doc["IPSView.ESP_" + IP + "_Frostschutz_Ost"] = logic_data_t->FlagIsFreezeProtectEast;
  doc["IPSView.ESP_" + IP + "_Frostschutz_West"] = logic_data_t->FlagIsFreezeProtectWest;

  serializeJsonPretty(doc, message);

  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}

/********************************************************
  Parameter Umschaltventil senden
********************************************************/
void sendValveValues()
{
  String message;
  JsonDocument status(&allocator);

  // Publish angepasste Parameter
  status["IPSView.ESP_" + IP + "_DiffOnValve"] = parameter_data_t->iDiffOnValve;
  status["IPSView.ESP_" + IP + "_DiffOffValve"] = parameter_data_t->iDiffOffValve;
  status["IPSView.ESP_" + IP + "_TempValveOn"] = parameter_data_t->iValveRelease;

  serializeJsonPretty(status, message);

  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}

/********************************************************
  Parameter Umladepumpe senden
********************************************************/
void sendLoadPumpValues()
{
  String message;
  JsonDocument status(&allocator);

  // Publish angepasste Parameter
  status["IPSView.ESP_" + IP + "_SelectUpLoadpump"] = parameter_data_t->iSelectUpLoadpump;
  status["IPSView.ESP_" + IP + "_SelectDownLoadpump"] = parameter_data_t->iSelectDownLoadpump;
  status["IPSView.ESP_" + IP + "_DiffOnLoadpump"] = parameter_data_t->iDiffOnLoadpump;
  status["IPSView.ESP_" + IP + "_DiffOffLoadpump"] = parameter_data_t->iDiffOffLoadpump;
  status["IPSView.ESP_" + IP + "_TempLoadpump"] = parameter_data_t->iTempLoadpump;

  serializeJsonPretty(status, message);

  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}

/********************************************************
  Messdaten + Status senden
********************************************************/
void sendMeasurements()
{
  String message;
  JsonDocument doc;

  doc["IPSView.ESP_" + IP + "_Signal"] = WiFi.RSSI();
  doc["IPSView.ESP_" + IP + "_Resetgrund"] = ConvertResetReasonToString(esp_reset_reason());
  doc["IPSView.ESP_" + IP + "_Chiptemperatur"] = serialized(String(fTempChip, 1));
  doc["IPSView.ESP_" + IP + "_Kollektortemperatur Ost"] = serialized(String(fTemp_Collector_East, 1));
  doc["IPSView.ESP_" + IP + "_Kollektortemperatur West"] = serialized(String(fTemp_Collector_West, 1));
  doc["IPSView.ESP_" + IP + "_Vorlauftemperatur"] = serialized(String(fTemp_Vorlauf, 1));
  doc["IPSView.ESP_" + IP + "_Ruecklauftemperatur"] = serialized(String(fTemp_Ruecklauf, 1));
  doc["IPSView.ESP_" + IP + "_Warmwassertemperatur"] = serialized(String(fTemp_Warmwasser, 1));
  doc["IPSView.ESP_" + IP + "_Puffertemperatur Mitte"] = serialized(String(fTemp_Puffer_Mitte, 1));
  doc["IPSView.ESP_" + IP + "_Puffertemperatur Unten"] = serialized(String(fTemp_Puffer_Unten, 1));

  doc["IPSView.ESP_" + IP + "_Status_Spannung"] = serialized(String(Volt, 1));
  doc["IPSView.ESP_" + IP + "_Status_Strom"] = serialized(String(Ampere, 2));
  doc["IPSView.ESP_" + IP + "_Status_Leistung"] = serialized(String(Watt, 0));
  doc["IPSView.ESP_" + IP + "_Status_Arbeit"] = serialized(String(Arbeit, 0));

  doc["IPSView.ESP_" + IP + "_Fehler_MAX31865"] = ConvertFaultToString();

  serializeJsonPretty(doc, message);

  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}

/********************************************************
  Status senden
********************************************************/
void buildStatusJson(String text)
{
  JsonDocument status;
  String message;

  status["Statusmeldung"] = text;
  serializeJsonPretty(status, message);
  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}

/********************************************************
  Status von Heap + PSRAM senden
********************************************************/
void sendStatusStorage()
{
  JsonDocument status;
  String message;

  status["IPSView.ESP_" + IP + "_Heap_Size"] = serialized(String(ESP.getHeapSize()));
  status["IPSView.ESP_" + IP + "_Free_Heap"] = serialized(String(ESP.getFreeHeap()));
  status["IPSView.ESP_" + IP + "_Max_Alloc_Heap"] = serialized(String(ESP.getMaxAllocHeap()));
  status["IPSView.ESP_" + IP + "_Min_Free_Heap"] = serialized(String(ESP.getMinFreeHeap()));
  status["IPSView.ESP_" + IP + "_Flash_Chip_Mode"] = serialized(String(ESP.getFlashChipMode()));
  status["IPSView.ESP_" + IP + "_Flash_Chip_Size"] = serialized(String(ESP.getFlashChipSize()));
  status["IPSView.ESP_" + IP + "_Flash_Chip_Speed"] = serialized(String(ESP.getFlashChipSpeed()));
  status["IPSView.ESP_" + IP + "_Free_Sketch_Space"] = serialized(String(ESP.getFreeSketchSpace()));
  status["IPSView.ESP_" + IP + "_Psram_Size"] = serialized(String(ESP.getPsramSize()));
  status["IPSView.ESP_" + IP + "_Free_Psram"] = serialized(String(ESP.getFreePsram()));
  status["IPSView.ESP_" + IP + "_Used_Psram"] = serialized(String(ESP.getPsramSize() - ESP.getFreePsram()));
  status["IPSView.ESP_" + IP + "_Max_Alloc_Psram"] = serialized(String(ESP.getMaxAllocPsram()));
  status["IPSView.ESP_" + IP + "_Min_Free_Psram"] = serialized(String(ESP.getMinFreePsram()));

  serializeJsonPretty(status, message);
  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}

/********************************************************
  Status von Chip senden
********************************************************/
void sendStatusChip()
{
  JsonDocument status;
  String message;

  status["IPSView.ESP_" + IP + "_Chip_Cores"] = serialized(String(ESP.getChipCores()));
  status["IPSView.ESP_" + IP + "_Chip_Model"] = String(ESP.getChipModel());
  status["IPSView.ESP_" + IP + "_Chip_Revision"] = serialized(String(ESP.getChipRevision()));
  status["IPSView.ESP_" + IP + "_CPU_Frequence"] = serialized(String(ESP.getCpuFreqMHz()));
  status["IPSView.ESP_" + IP + "_Cycle_Count"] = serialized(String(ESP.getCycleCount()));

  serializeJsonPretty(status, message);
  mqttClient.publish(Publish_Json, 0, true, message.c_str());
}
