/********************************************************
   Bibliotheken
********************************************************/
#pragma once
#include <../data/edit/edit_htm.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

/********************************************************
  Interne Instanzen
********************************************************/
extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern AsyncEventSource events;

/********************************************************
  Funktions-Prototypen Webserver
********************************************************/
void initOTA();
void initWebserver();
String runtime();
const String formatBytes(size_t const &bytes);

/********************************************************
   Deklaration Dateimanager-Login
********************************************************/
extern String http_username;
extern String http_password;