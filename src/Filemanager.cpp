/********************************************************
  Bibliotheken
********************************************************/
#include "Prototypes.h"

/********************************************************
  Lokal deklarierte Instanzen
********************************************************/
File file;

/********************************************************
  Initialisiere Dateisystem FFAT
********************************************************/
void initFileSystem()
{
  if (!FFat.begin(FORMAT_FFAT_IF_FAILED, "/ffat", 10, "ffat"))
  {
    Serial.println("Dateisystem FFat konnte nicht erstellt werden!");
    return;
  }
  else
  {
    Serial.println("Dateisystem erfolgreich erstellt!");
  }
}