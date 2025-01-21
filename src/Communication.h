/********************************************************
  Bibliotheken
 ********************************************************/
#pragma once
#include <WiFi.h>
#include <ArduinoJson.h>
#include "esp_heap_caps.h"

/********************************************************
  Funktions-Prototypen WiFi
********************************************************/
void WiFiStart();
void connectToWifi();

/********************************************************
  Funktions-Prototypen Kommunikation
********************************************************/
void initMqtt();
void sendMeasurements();
void sendStorage();
void sendStatusStorage();
void sendStatusChip();
void sendLogic();
void sendMaxLimits();
void sendMinLimits();
void sendDiffValues();
void sendValveValues();
void sendLoadPumpValues();
void sendOverTempValues();
void sendFreezeProtectValues();
void sendTimerEast();
void sendTimerWest();
void buildStatusJson(String text);

/********************************************************
  Funktions-Prototypen Allgemein
********************************************************/
String ConvertResetReasonToString(esp_reset_reason_t reason);

/********************************************************
  Globale Konstanten Kommunikation
********************************************************/
#define DEBUG_COMMUNICATION false
#define MQTT_HOST IPAddress(192, 168, 178, 200)
#define MQTT_PORT 1883

/********************************************************
  Deklaration Globale Variablen Kommunikation
********************************************************/
extern const char *host;


/********************************************************
  Memory Allocator für Arduino JSON auf externem PSRAM
********************************************************/
struct SpiRamAllocator : ArduinoJson::Allocator
{
  void *allocate(size_t size) override
  {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }

  void deallocate(void *pointer) override
  {
    heap_caps_free(pointer);
  }

  void *reallocate(void *ptr, size_t new_size) override
  {
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
  }
};
