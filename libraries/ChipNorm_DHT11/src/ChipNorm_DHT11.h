// FILE: ChipNorm_DHT11.h
// VERSION: 1.0.0
// PURPOSE: DHT11 Temperature & Humidity Sensor library
//
// ChipNorm invests time and resources providing this open code,
// please support ChipNorm by purchasing products from ChipNorm.
//
// Written by Enrique Fernandez for ChipNorm by FMH.
//
// Copyright   Copyright (c) 2025 Enrique Fernandez - ChipNorm by FMH
// See the LICENSE file for details.

#ifndef CHIPNORM_DHT11_H
#define CHIPNORM_DHT11_H

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

// --- Fehlercodes definieren ---
#define DHTLIB_OK          0
#define DHTLIB_ERROR_CHECKSUM -1
#define DHTLIB_ERROR_TIMEOUT  -2

// --- Sensor-Datenstruktur ---
struct DHT11_Data {
  float humidity;    // Luftfeuchtigkeit in %
  float temperature; // Temperatur in Celsius
};

// --- Klasse definieren ---
class DHT11_Bibliothek {
public:
  // Konstruktor: Initialisiert die Klasse mit der PIN-Nummer, an die der Sensor angeschlossen ist
  DHT11_Bibliothek(uint8_t pin);

  // Methode zum Auslesen der Sensor-Daten
  // Gibt einen der Fehlercodes (DHTLIB_OK, etc.) zurück
  int read();

  // Öffentliche Member-Variablen zur Speicherung der zuletzt gelesenen Daten
  DHT11_Data data;

private:
  uint8_t _pin; // Die PIN-Nummer des Sensors
  uint8_t _readData[5]; // Internes Array zum Speichern der 5 Byte Daten (Feucht. Int/Dez, Temp. Int/Dez, Checksumme)

  // Interne Hilfsfunktion zum Lesen der rohen Datenbits (implementiert in .cpp)
  int _readRawData();
};

#endif