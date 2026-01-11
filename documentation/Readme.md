
#  Documentation API – Développeur IoT (ESP8266)

Cette documentation décrit le **format d’envoi des données** depuis un objet IoT (ESP8266) vers le serveur backend.

---

##  Endpoint

```
POST /api/data/collect
```

---

##  Authentification

L’authentification se fait via un **header HTTP obligatoire**.

### Headers requis

| Clé            | Valeur             | Description                                    |
| -------------- | ------------------ | ---------------------------------------------- |
| Content-Type   | application/json   | Format des données envoyées                    |
| x-device-token | PROJET_KONNON_2026 | Token défini dans le fichier `.env` du serveur |

---

##  Corps de la requête

L’ESP doit envoyer **un seul objet JSON** contenant :

* les données environnementales globales
* la liste des zones surveillées

### Exemple de requête JSON

```json
{
  "air_temp": 25.4,
  "air_hum": 65,
  "zones": [
    {
      "id": "z1",
      "raw": 750
    },
    {
      "id": "z2",
      "raw": 420
    }
  ]
}
```

---

##  Détails des champs

### Champs racine

| Champ    | Type  | Description                                      |
| -------- | ----- | ------------------------------------------------ |
| air_temp | Float | Température mesurée par le capteur DHT22 (en °C) |
| air_hum  | Float | Humidité relative mesurée par le DHT22 (en %)    |
| zones    | Array | Liste des zones actives                          |

### Objet `zone`

| Champ | Type   | Description                                                                             |
| ----- | ------ | --------------------------------------------------------------------------------------- |
| id    | String | Identifiant unique de la zone (doit correspondre à la base Firebase : `z1`, `z2`, etc.) |
| raw   | Int    | Valeur brute lue sur l’entrée analogique (entre 0 et 1023)                              |

---

##  Réponses du serveur

| Code HTTP            | Signification                              |
| -------------------- | ------------------------------------------ |
| **201 Created**      | Données reçues et traitées avec succès     |
| **400 Bad Request**  | Format JSON invalide ou données manquantes |
| **401 Unauthorized** | Token manquant ou invalide                 |

---

##  Bonnes pratiques côté ESP8266

* Toujours envoyer un JSON valide
* Vérifier la présence du header `x-device-token`
* Respecter les plages de valeurs (`raw` entre 0 et 1023)
* Envoyer uniquement les zones actives

