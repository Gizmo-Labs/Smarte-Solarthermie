/********************************************************
   Bibliotheken
********************************************************/
#include "Prototypes.h"

/********************************************************
   Lokal deklarierte Instanzen
********************************************************/
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

String http_username = "admin";
String http_password = "admin";

/********************************************************
  Externe Referenzen
********************************************************/
extern LOGIC *logic_data_t;
extern PARAMETER *parameter_data_t;
extern TIMER *timer_data_t;

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
  }
  else if (type == WS_EVT_ERROR)
  {
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
  }
  else if (type == WS_EVT_PONG)
  {
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
  }
  else if (type == WS_EVT_DATA)
  {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    String msg = "";
    if (info->final && info->index == 0 && info->len == len)
    {
      // the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

      if (info->opcode == WS_TEXT)
      {
        for (size_t i = 0; i < info->len; i++)
        {
          msg += (char)data[i];
        }
      }
      else
      {
        char buff[3];
        for (size_t i = 0; i < info->len; i++)
        {
          sprintf(buff, "%02x ", (uint8_t)data[i]);
          msg += buff;
        }
      }
      Serial.printf("%s\n", msg.c_str());

      if (info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    }
    else
    {
      // message is comprised of multiple frames or the frame is split into multiple packets
      if (info->index == 0)
      {
        if (info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

      if (info->opcode == WS_TEXT)
      {
        for (size_t i = 0; i < len; i++)
        {
          msg += (char)data[i];
        }
      }
      else
      {
        char buff[3];
        for (size_t i = 0; i < len; i++)
        {
          sprintf(buff, "%02x ", (uint8_t)data[i]);
          msg += buff;
        }
      }
      Serial.printf("%s\n", msg.c_str());

      if ((info->index + len) == info->len)
      {
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if (info->final)
        {
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
          if (info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}

void initWebserver()
{
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  events.onConnect([](AsyncEventSourceClient *client)
                   { client->send("hello!", NULL, millis(), 1000); });

  server.addHandler(&events);

  /**********************************************************************
      Server Handle für Webserver Status-Seite in Intervall aktualisieren
  **********************************************************************/
  server.on("/status/renew", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", "[\"" + runtime() + "\",\"" + WiFi.RSSI() + " dBm" + "\",\"" + formatBytes(ESP.getFreeHeap()) + "\",\"" + String(fTempChip) + " °C" + "\"]"); });

  /**********************************************************************
      Server Handle für Webserver Status-Seite in Intervall aktualisieren
  **********************************************************************/
  server.on("/status/solar", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", "[\"" + String(logic_data_t->FlagIsAutomatic) + "\",\"" + String(fTemp_Collector_East) + " °C" + "\",\"" + String(fTemp_Collector_West) + " °C" + "\",\"" + String(fTemp_Warmwasser) + " °C" + "\",\"" + String(fTemp_Puffer_Mitte) + " °C" + "\",\"" + String(fTemp_Puffer_Unten) + " °C" + "\",\"" + String(fTemp_Vorlauf) + " °C" + "\",\"" + String(fTemp_Ruecklauf) + " °C" + "\",\"" + String(logic_data_t->FlagPumpEast) + "\",\"" + String(logic_data_t->FlagPumpWest) + "\",\"" + String(logic_data_t->FlagLoadPump) + "\",\"" + String(logic_data_t->FlagValve) + "\"]"); });

  /**********************************************************************
      Server Handle für Webserver Status-Seite in Intervall aktualisieren
  **********************************************************************/
  server.on("/status/smart", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", "[\"" + String(Volt) + " Volt" + "\",\"" + String(Ampere) + " Ampere" + "\",\"" + String(Watt) + " Watt" + "\",\"" + String(Arbeit / 1000.0) + " kWh" + "\",\"" + String(Waerme) + " Joule" + "\"]"); });

  /**********************************************************************
      Server Handle für Webserver Parameter-Seite in Intervall aktualisieren
  **********************************************************************/
  server.on("/parameter/ost", HTTP_GET, [](AsyncWebServerRequest *request)
            { uint8_t iFreigabezeit_Ost_Stunden = timer_data_t->iReleaseHourTenEast * 10 + timer_data_t->iReleaseHourOneEast;
              uint8_t iFreigabezeit_Ost_Minuten = timer_data_t->iReleaseMinTenEast * 10 + timer_data_t->iReleaseMinOneEast;
              uint8_t iSperrzeit_Ost_Stunden = timer_data_t->iBlockHourTenEast * 10 + timer_data_t->iBlockHourOneEast;
              uint8_t iSperrzeit_Ost_Minuten = timer_data_t->iBlockMinTenEast * 10 + timer_data_t->iBlockMinOneEast;
              request->send(200, "application/json", "[\"" + String(parameter_data_t->iMaxLimitOnPumpEast) + "°C" + "\",\"" + String(parameter_data_t->iMaxLimitOffPumpEast) + "°C" + "\",\"" + String(parameter_data_t->iMinLimitOnPumpEast) + "°C" + "\",\"" + String(parameter_data_t->iMinLimitOffPumpEast) + "°C" + "\",\"" + String(parameter_data_t->iDiffOnPumpEast) + " K" + "\",\"" + String(parameter_data_t->iDiffOffPumpEast) + " K" + "\",\"" + String(parameter_data_t->iOverOnCollectorEast) + "°C" + "\",\"" + String(parameter_data_t->iOverOffCollectorEast) + "°C" + "\",\"" + String(parameter_data_t->iColdOnCollectorEast) + "°C" + "\",\"" + String(parameter_data_t->iColdOffCollectorEast) + "°C" + "\",\"" + String(parameter_data_t->iCoolingEast) + "°C" + "\",\"" + String(iFreigabezeit_Ost_Stunden) + ":" + String(iFreigabezeit_Ost_Minuten) + "\",\"" + String(iSperrzeit_Ost_Stunden) + ":" + String(iSperrzeit_Ost_Minuten) + "\"]"); });

  /**********************************************************************
      Server Handle für Webserver Parameter-Seite in Intervall aktualisieren
  **********************************************************************/
  server.on("/parameter/west", HTTP_GET, [](AsyncWebServerRequest *request)
            { uint8_t iFreigabezeit_West_Stunden = timer_data_t->iReleaseHourTenWest * 10 + timer_data_t->iReleaseHourOneWest;
              uint8_t iFreigabezeit_West_Minuten = timer_data_t->iReleaseMinTenWest * 10 + timer_data_t->iReleaseMinOneWest;
              uint8_t iSperrzeit_West_Stunden = timer_data_t->iBlockHourTenWest * 10 + timer_data_t->iBlockHourOneWest;
              uint8_t iSperrzeit_West_Minuten = timer_data_t->iBlockMinTenWest * 10 + timer_data_t->iBlockMinOneWest;
              request->send(200, "application/json", "[\"" + String(parameter_data_t->iMaxLimitOnPumpWest) + "°C" + "\",\"" + String(parameter_data_t->iMaxLimitOffPumpWest) + "°C" + "\",\"" + String(parameter_data_t->iMinLimitOnPumpWest) + "°C" + "\",\"" + String(parameter_data_t->iMinLimitOffPumpWest) + "°C" + "\",\"" + String(parameter_data_t->iDiffOnPumpWest) + " K" + "\",\"" + String(parameter_data_t->iDiffOffPumpWest) + " K" + "\",\"" + String(parameter_data_t->iOverOnCollectorWest) + "°C" + "\",\"" + String(parameter_data_t->iOverOffCollectorWest) + "°C" + "\",\"" + String(parameter_data_t->iColdOnCollectorWest) + "°C" + "\",\"" + String(parameter_data_t->iColdOffCollectorWest) + "°C" + "\",\"" + String(parameter_data_t->iCoolingWest) + "°C" + "\",\"" + String(iFreigabezeit_West_Stunden) + ":" + String(iFreigabezeit_West_Minuten) + "\",\"" + String(iSperrzeit_West_Stunden) + ":" + String(iSperrzeit_West_Minuten) + "\"]"); });

  /**********************************************************************
      Server Handle für Webserver Parameter-Seite in Intervall aktualisieren
  **********************************************************************/
  server.on("/parameter/general", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", "[\"" + String(parameter_data_t->iDiffOnValve) + " K" + "\",\"" + String(parameter_data_t->iDiffOffValve) + " K" + "\",\"" + String(parameter_data_t->iValveRelease) + "°C" + "\",\"" + String(parameter_data_t->iDiffOnLoadpump) + " K" + "\",\"" + String(parameter_data_t->iDiffOffLoadpump) + " K" + "\",\"" + String(parameter_data_t->iTempLoadpump) + "°C" + "\",\"" + String(parameter_data_t->iSelectUpLoadpump) + "\",\"" + String(parameter_data_t->iSelectDownLoadpump) + "\"]"); });

  /**********************************************************************
      Server Handle für Webserver Status-Seite einmalig aktualisieren
  **********************************************************************/
   server.on("/status/once", HTTP_GET, [](AsyncWebServerRequest *request)
          { request->send(200, "application/json", "{\"LocalIP\":\"" + WiFi.localIP().toString() + "\", \"Hostname\":\"" + WiFi.getHostname() + "\", \"SSID\":\"" + WiFi.SSID() + "\", \"GatewayIP\":\"" + WiFi.gatewayIP().toString() + "\", \"Channel\":\"" + WiFi.channel() + "\", \"MacAddress\":\"" + WiFi.macAddress() + "\", \"SubnetMask\":\"" + WiFi.subnetMask().toString() + "\", \"BSSID\":\"" + WiFi.BSSIDstr() + "\", \"ClientIP\":\"" + request->client()->remoteIP().toString() + "\", \"DnsIP\":\"" + WiFi.dnsIP().toString() + "\"}"); });
  /**********************************************************************
      Server Handle für Webserver Status-Seite Button "Neu verbinden"
  **********************************************************************/
  server.on("/reconnect", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    request->send(304, "message/http");
    WiFi.reconnect(); });

  /**********************************************************************
      Server Handle für Webserver Status-Seite Button "Neu starten"
  **********************************************************************/
  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    request->send(304, "message/http");   
    ESP.restart(); });

  /**********************************************************************
      Server Handle für Webserver Rootpfad
  **********************************************************************/
  server.serveStatic("/", FFat, "/").setDefaultFile("index.html");

  /**********************************************************************
      Server Handle für Webserver Seite nicht gefunden
  **********************************************************************/
  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(404, "text/plain", "Webseite nicht gefunden!"); });

  /**********************************************************************
      Server Handle für Webserver Dateimanager - Status Dateisystem
  **********************************************************************/
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
            {
      String json;
      json.reserve(128);

      json = "{\"type\":\"";
      json += "FFat";
      json += "\", \"isOk\":";
      json += F("\"true\", \"totalBytes\":\"");
      json += String(FFat.totalBytes());
      json += F("\", \"usedBytes\":\"");
      json += String(FFat.usedBytes());
      json += "\"";
      json += "}";
      request->send(200, "application/json", json); });

  /**********************************************************************
      Server Handle für Webserver Dateimanager - Dateisystem einlesen
  **********************************************************************/
  server.on("/list", HTTP_GET, [](AsyncWebServerRequest *request)
            {
        if (http_username.length() && http_password.length() && !request->authenticate(http_username.c_str(), http_password.c_str()))
         return request->requestAuthentication();

        if (request->hasParam("dir"))
        {
            String path = request->getParam("dir")->value();

            File dir = FFat.open(path);
            path = String();
            String output = "[";

            File entry = dir.openNextFile();
            
            while (entry)
            {
                // if (isExcluded(FFat, entry.name()))
                // {
                //     entry = dir.openNextFile();
                //     continue;
                // }
        
            if (output != "[")
            output += ',';
            output += "{\"type\":\"";
            output += "file";
            output += "\",\"name\":\"";
            output += String(entry.name());
            output += "\",\"size\":";
            output += String(entry.size());
            output += "}";

            entry = dir.openNextFile();

            }

            dir.close();

            output += "]";
            request->send(200, "application/json", output);
            output = String();
        } });

  /**********************************************************************
      Server Handle für Webserver Dateimanager - Datei Download
  **********************************************************************/
  server.on("/edit", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    
       String contentType;

       if (request->hasParam("download"))
       {
         contentType = F("application/octet-stream");
       }
       else
       {
         contentType = request->contentType().c_str();
       }

      const char * buildTime = __DATE__ " " __TIME__ " GMT";
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", edit_htm_gz, edit_htm_gz_len);
      response->addHeader("Content-Encoding", "gzip");
      response->addHeader("Last-Modified", buildTime);
      request->send(response); });

  /**********************************************************************
      Server Handle für Webserver Dateimanager - Datei Upload
  **********************************************************************/
  server.on("/edit", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    
    if (http_username.length() && http_password.length() && !request->authenticate(http_username.c_str(), http_password.c_str()))
         return request->requestAuthentication();

       if(request->hasParam("data", true, true) && FFat.exists(request->getParam("data", true, true)->value()))
      request->send(200, "", "UPLOADED: "+request->getParam("data", true, true)->value()); });

  /**********************************************************************
      Server Handle für Webserver Dateimanager - Datei erstellen
  **********************************************************************/
  server.on("/edit", HTTP_PUT, [](AsyncWebServerRequest *request)
            {
       if (http_username.length() && http_password.length() && !request->authenticate(http_username.c_str(), http_password.c_str()))
         return request->requestAuthentication();

    
       if (!request->hasParam("path", true))
       {
         Serial.println("PATH ARG MISSING");
         request->send(400, "text/plain", "PATH ARG MISSING \r\n");
       }

       String src = String();

       if (request->hasParam("src", true))
       {
         src = request->getParam("src", true)->value();
       }

       String path = request->getParam("path", true)->value();

       if (path.isEmpty())
       {
         Serial.println("BAD PATH");
         request->send(400, "text/plain", "BAD PATH \r\n");
       }

      if (path == "/")
      {
        Serial.println("BAD PATH");
        request->send(400, "text/plain", "BAD PATH \r\n");
      }

      if (FFat.exists(path))
      {
        Serial.println("PATH FILE STILL EXISTS");
        request->send(400, "text/plain", "PATH FILE STILL EXISTS \r\n");
      }

      if (src.isEmpty())
      {
        // No source specified: creation
        Serial.println(String("Datei wurde erstellt! --> ") + path);

        if (path.endsWith("/"))
        {
          // Create a folder
          path.remove(path.length() - 1);

          if (!FFat.mkdir(path))
          {
            Serial.println("MKDIR FAILED");
            request->send(500, "text/plain", "MKDIR FAILED \r\n");
          }
        }
        else
        {
          // Create a file
          File file = FFat.open(path, "w");
          if (file)
          {
            file.write((const char)0);
            file.close();
          }
          else
          {
            Serial.println("CREATE FAILED");
            request->send(500, "text/plain", "CREATE FAILED \r\n");
          }
        }

        if (path.lastIndexOf('/') > -1)
        {
          path = path.substring(0, path.lastIndexOf('/'));
        }
        request->send(200, "text/plain", path);
      }
      else
      {
        // Source specified: rename
        if (src == "/")
        {
          Serial.println("BAD SRC");
          request->send(400, "text/plain", "BAD SRC \r\n");
        }

        if (!FFat.exists(src))
        {
          Serial.println("SRC FILE NOT FOUND");
          request->send(400, "text/plain", "SRC FILE NOT FOUND \r\n");
        }
        else
        {
          Serial.println(String("Datei wurde umbenannt! --> ") + path + " von " + src);
        }

        if (path.endsWith("/"))
        {
          path.remove(path.length() - 1);
        }

        if (src.endsWith("/"))
        {
          src.remove(src.length() - 1);
        }

        if (!FFat.rename(src, path))
        {
          Serial.println("RENAME FAILED");
          request->send(500, "text/plain", "RENAME FAILED \r\n");
        }

        while (!src.isEmpty() && !FFat.exists(src))
        {
          if (src.lastIndexOf('/') > 0)
          {
            src = src.substring(0, src.lastIndexOf('/'));
          }
          else
          {
            src = String();  // No slash => the top folder does not exist
          }
        }
        request->send(200, "text/plain", src);
      } });

  /**********************************************************************
      Server Handle für Webserver Dateimanager - Datei löschen
  **********************************************************************/
  server.on("/edit", HTTP_DELETE, [](AsyncWebServerRequest *request)
            {
       if (http_username.length() && http_password.length() && !request->authenticate(http_username.c_str(), http_password.c_str()))
         return request->requestAuthentication();

    
       String path = request->getParam("path", true)->value();

       if (path.isEmpty() || path == "/")
       {
         Serial.println("BAD PATH");
         request->send(400, "text/plain", "BAD PATH \r\n");
       }

    //    if (!FFat.exists(path))
    //    {
    //      request->send(404, "text/plain", "REPLY NOT FOUND");
    //    }

       File file = FFat.open(path, "r");
       bool isDir = file.isDirectory();
       file.close();

       // If it's a plain file, delete it
       if (!isDir)
       {
         FFat.remove(path);
         Serial.println(String("Datei wurde gelöscht! --> ") + path);
       }
       else
       {
         // Otherwise delete its contents first
        File root = FFat.open(path, "r");

         // Then delete the folder itself
         FFat.rmdir(path);
       }

       while (!path.isEmpty() && !FFat.exists(path))
       {
         if (path.lastIndexOf('/') > 0)
         {
           path = path.substring(0, path.lastIndexOf('/'));
         }
         else
         {
           path = String();  // No slash => the top folder does not exist
         }
       }
       request->send(200, "text/plain", path); });

  /**********************************************************************
      Server Handle für Webserver Dateimanager - Datei Upload
  **********************************************************************/
  server.onFileUpload([](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
                      {
      if(!index)
        Serial.printf("UploadStart: %s\n", filename.c_str());
      Serial.printf("%s", (const char*)data);
      if(final)
        Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len); });

  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
                       {
      if(!index)
        Serial.printf("BodyStart: %u\n", total);
      Serial.printf("%s", (const char*)data);
      if(index + len == total)
        Serial.printf("BodyEnd: %u\n", total); });

  server.begin();

  Serial.println("HTTP server started");
}

/*********************************************************
  Formatiere Bytes
********************************************************/
const String formatBytes(size_t const &bytes)
{ // lesbare Anzeige der Speichergrößen
  return bytes < 1024 ? static_cast<String>(bytes) + " Byte" : bytes < 1048576 ? static_cast<String>(bytes / 1024.0) + " KB"
                                                                               : static_cast<String>(bytes / 1048576.0) + " MB";
}

/*********************************************************
  Laufzeit ermitteln
********************************************************/
String runtime()
{
  static uint8_t rolloverCounter{0};
  static uint32_t previousMillis{0};
  uint32_t currentMillis{millis()};
  if (currentMillis < previousMillis)
    rolloverCounter++; // prüft Millis Überlauf
  previousMillis = currentMillis;
  uint32_t sek{(0xFFFFFFFF / 1000) * rolloverCounter + (currentMillis / 1000)};
  char buf[20];
  snprintf(buf, sizeof(buf), "%d Tag%s %02d:%02d:%02d", sek / 86400, sek < 86400 || sek >= 172800 ? "e" : "", sek / 3600 % 24, sek / 60 % 60, sek % 60);
  return buf;
}
