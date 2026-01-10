Le dossier iot-collecte

# Module IoT Collecte - ESP8266

## Développeur
Ozias

## Description
Collecte des données environnementales pour le système d'irrigation intelligent.

## Cahier des charges respecté
- [x] Programmation POO (classes SoilSensor, EnvSensor)
- [x] WiFiManager pour configuration SSID/URL
- [x] Lecture capteurs (A0 + DHT11/22)
- [x] Envoi HTTP POST toutes les 10 minutes
- [x] Format JSON conforme au contrat

## Configuration
### Fichier `config.h` (non versionné)
```cpp
#define SOIL_SENSOR_PIN A0
#define DHT_SENSOR_PIN D4
#define DHT_TYPE DHT22
#define DEFAULT_BACKEND_URL "http://192.168.x.x:3000/api/data/collect"
#define DEVICE_TOKEN "IOT_SECRET_2026_PROD"
#define NODE_ID "ESP8266_ZONE_1"