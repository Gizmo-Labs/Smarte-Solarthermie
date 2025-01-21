# Smarte-Solarthermie
Solarthermie-Regelung mit ESP32S3 und Nextion-Touch-Display + MQTT-Interface

## Hintergrund
Dieses Repository enthält die Hard- und Softwareinhalte zum Bau einer smarten  
Solarthermie-Regelung. Solarthermie nutzt die Energie der Sonne zur Warmwassererzeugung.  

Die zugrunde liegende Anlage umfasst zwei Solarkollektoren auf dem Dach.  
Dabei ist einer auf der Westseite, der andere auf der Ostseite verortet.  

In jedem Kollektor ist ein Temperaturfühler vom Typ **PT1000** angebracht.  
Im Technikraum befindet sich ein Pufferspeicher, der mit **5** Temperatursensoren des Typs **DS18B20** überwacht wird.  

Jeder Solarkreis verfügt über eine zugehörige Umwälzpumpe, die über Relais angesteuert wird.  
Zudem ist eine weitere Pumpe im System installiert, die das Warmwasser bei Erzeugung vom  
angebundenen Heizkessel in den Speicher transportiert.  

Für diese spezielle Anlageninstallation ist zudem ein Umschaltventil vorhanden, dass zur Rücklaufanhebung dient.  

Zur Energiemessung ist ein Smart-Meter in der Steuerung integriert.

## Systembeschreibung

📌 Bedienung über 10" Touch-Display von Nextion  
📌 Digitale Temperaturmessung am Pufferspeicher mit DS18B20  
📌 Analoge Temperaturmessung auf dem Dach mit PT1000  
📌 Integriertes Smart-Meter zur Messung von Spannung, Strom, Leistung und Energie  
📌 Wärmemengenzähler to be Done...  
📌 Anlagenschutz durch verschiedene Überwachungsparameter  
📌 Sämtliche Parameter am Display einstellbar  
📌 Speicherung aller Werte bei Stromausfall  
📌 Separates Web-Interface für das Anlagenmonitoring  
📌 Übertragung sämtlicher Messungen per MQTT-Schnittstelle an externes System  
📌 Automatische Dimmfunktion  
📌 Einstellbare Displayhelligkeit  
📌 Manuelle Schaltfunktion für alle Ausgänge zu Testzwecken  

