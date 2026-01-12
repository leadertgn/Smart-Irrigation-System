#ifndef CONFIG_EXAMPLE_H
#define CONFIG_EXAMPLE_H

// COPIE CE FICHIER EN config.h ET REMPLIS TES VALEURS
// ⚠️ NE JAMAIS COMMIT config.h ! ⚠️

// WiFi (laissé vide pour WiFiManager)
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

// Firebase - À récupérer sur Firebase Console
#define FIREBASE_HOST "TON_PROJET.firebaseio.com"
#define FIREBASE_API_KEY "AIzaSyXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#define FIREBASE_DATABASE_URL "https://TON_PROJET.firebaseio.com"

// Pins ESP32 (à vérifier avec Eckyon)
#define RELAY_PIN_1 23  // Pompe 1
#define RELAY_PIN_2 22  // Pompe 2
#define LED_BUILTIN 2   // LED interne ESP32

#endif