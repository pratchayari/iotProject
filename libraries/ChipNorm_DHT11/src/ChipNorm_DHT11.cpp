

#include "ChipNorm_DHT11.h"

// --- Private Methode: Lesen der rohen 5 Byte Daten vom Sensor ---
// Gibt DHTLIB_OK oder einen Fehlercode zurück.
int DHT11_Bibliothek::_readRawData() {
  uint8_t i, j;
  uint16_t loopCnt;
  uint8_t bitData = 0;
  
  // 1. Host (Arduino) sendet Startsignal
  
  // Pin auf OUTPUT schalten und LOW für 18ms (oder mehr)
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  delay(18);

  // Pin auf HIGH für 20-40µs (Sensor erkennt dies als Beendigung des Startsignals)
  digitalWrite(_pin, HIGH);
  delayMicroseconds(20); 

  // Pin auf INPUT schalten, um auf die Antwort des Sensors zu warten
  pinMode(_pin, INPUT);

  // 2. Sensor-Antwort (LOW-Impuls)

  // Warten auf den LOW-Impuls des Sensors (max. 80µs)
  loopCnt = 10000;
  while(digitalRead(_pin) == LOW) {
    if(loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;
  }
  
  // Warten auf den HIGH-Impuls des Sensors (max. 80µs)
  loopCnt = 10000;
  while(digitalRead(_pin) == HIGH) {
    if(loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;
  }
  
  // 3. Daten lesen (40 Bit = 5 Byte)

  // Lösche das Daten-Array
  for (i = 0; i < 5; i++) _readData[i] = 0;

  for (i = 0; i < 5; i++) { // Lese 5 Bytes
    for (j = 0; j < 8; j++) { // Lese 8 Bits pro Byte
      
      // Warten auf den Start des HIGH-Impulses (max. 50µs)
      loopCnt = 10000;
      while(digitalRead(_pin) == LOW) {
        if(loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;
      }
      
      // HIGH-Impuls-Dauer messen, um '0' oder '1' zu bestimmen
      unsigned long t = micros();
      
      // Warten auf das Ende des HIGH-Impulses (max. 70µs)
      loopCnt = 10000;
      while(digitalRead(_pin) == HIGH) {
        if(loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;
      }
      
      t = micros() - t; // HIGH-Dauer

      // Bit schieben und setzen
      // HIGH-Impuls < 50µs ist '0', > 50µs ist '1'
      _readData[i] <<= 1;
      if(t > 40) { // Ein HIGH-Impuls von ca. 70µs bedeutet eine '1'
        _readData[i] |= 1;
      }
    }
  }

  // Alles erfolgreich gelesen
  return DHTLIB_OK;
}

// --- Konstruktor: Initialisiert die PIN-Nummer ---
DHT11_Bibliothek::DHT11_Bibliothek(uint8_t pin) {
  _pin = pin;
}

// --- Öffentliche Methode: Liest Daten und prüft Checksumme ---
int DHT11_Bibliothek::read() {
  int result = _readRawData();

  if (result != DHTLIB_OK) {
    return result; // Fehler beim Lesen der Rohdaten
  }

  // 1. Prüfe die Checksumme
  // Checksumme ist die Summe der ersten 4 Bytes (ohne Überlauf)
  uint8_t checksum = _readData[0] + _readData[1] + _readData[2] + _readData[3];
  
  // Das 5. Byte ist die übertragene Prüfsumme
  if (checksum != _readData[4]) {
    return DHTLIB_ERROR_CHECKSUM; 
  }

  // 2. Daten aufbereiten und in der öffentlichen Struktur speichern
  
  // DHT11 sendet Feuchtigkeit in den ersten beiden Bytes (Integer und Dezimal) 
  // und Temperatur in den nächsten beiden Bytes.
  // Da der DHT11 nur ganzzahlige Werte liefert, sind die Dezimalbytes meistens 0.

  // Feuchtigkeit
  data.humidity = (float)_readData[0]; // Integer-Teil (Byte 0)

  // Temperatur (Celsius)
  data.temperature = (float)_readData[2]; // Integer-Teil (Byte 2)
  
  // DHTLIB_OK, wenn alles erfolgreich war
  return DHTLIB_OK;
}