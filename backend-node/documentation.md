# 📘 Decision Engine – Documentation Technique

Le **Decision Engine** est le cerveau du système d’arrosage intelligent.  
Il est déclenché **à chaque réception de nouvelles données** envoyées par l’ESP8266 et décide, pour chaque zone de culture, s’il faut arroser ou non.

---

##  Objectif du moteur de décision

- Garantir la survie des plantes
- Optimiser la consommation d’eau
- Exploiter les créneaux horaires autorisés
- Anticiper les précipitations météo

---

##  Fonctionnement global

À la réception des données environnementales (température, humidité de l’air, humidité du sol), le moteur :

1. Analyse chaque **zone de culture**
2. Évalue 4 critères de décision
3. Applique la matrice de décision
4. Déclenche ou non l’arrosage

---

##  Les 4 piliers de la décision

Pour chaque zone, l’algorithme calcule les variables suivantes :

### 1️ Urgence — `isCritical`
> L’humidité du sol est-elle **sous le seuil de survie** ?

- `true` → La plante est en danger
- `false` → Pas de risque immédiat

---

### 2️ Créneau horaire — `isTimeSlot`
> Sommes-nous dans un créneau autorisé d’arrosage ?

Créneaux autorisés :
- **08h00**
- **13h00**
- **18h00**

---

### 3️ Besoin hydrique — `isDry`
> L’humidité du sol est-elle **inférieure au seuil de confort** ?

- `true` → Le sol est sec
- `false` → Le sol est suffisamment humide

---

### 4️ Économie d’eau — `willNotRain`
> La probabilité de pluie est-elle **suffisamment faible** ?

- `true` → Pas de pluie significative prévue
- `false` → Pluie probable, arrosage inutile

---

##  Matrice de décision

Cette matrice résume **tous les scénarios possibles** et la décision associée.

| Scénario | Humidité du sol | Heure  | Pluie prévue | Action     | Justification |
|--------|----------------|--------|--------------|------------|---------------|
| **A : Urgence** | 15% (Critique) | 10h | 90% | **ARROSAGE** | La plante est en danger immédiat, on n’attend pas la pluie |
| **B : Routine** | 35% (Sec) | 13h | 10% | **ARROSAGE** | Créneau valide, sol sec, météo favorable |
| **C : Économie** | 35% (Sec) | 13h | 80% | **RIEN** | Pluie imminente, on économise l’eau |
| **D : Repos** | 60% (OK) | 08h | 5% | **RIEN** | Le sol est déjà suffisamment humide |
| **E : Hors créneau** | 40% (Moyen) | 15h | 0% | **RIEN** | Ce n’est pas l’heure d’arroser et ce n’est pas critique |

---

##  Règles de priorité

Les décisions suivent l’ordre de priorité suivant :

1. **Urgence vitale (`isCritical`)**
2. Créneau horaire (`isTimeSlot`)
3. Besoin hydrique (`isDry`)
4. Prévision météo (`willNotRain`)

>  En cas d’urgence, **l’arrosage est déclenché immédiatement**, indépendamment de l’heure ou de la météo.

---

##  Cycle d’exécution

1. Réception des données depuis l’ESP8266
2. Évaluation des 4 piliers pour chaque zone
3. Application de la matrice de décision
4. Mise à jour de l’état dans Firebase
5. Déclenchement ou non de l’arrosage

---

##  Avantages du moteur

- Décisions cohérentes et explicables
- Économie d’eau intelligente
- Tolérance aux conditions extrêmes
- Architecture évolutive (ajout de nouveaux critères possible)

---

##  Évolutions possibles

- Pondération dynamique des critères
- Apprentissage automatique (historique des données)
- Gestion saisonnière
- Personnalisation par type de culture

---

 *Ce moteur constitue le cœur logique du Smart Irrigation System.*
