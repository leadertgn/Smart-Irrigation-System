import { db } from "../config/firebase.js";
import { updateForecast } from "./weatherService.js";

export const evaluateIrrigation = async () => {
  try {
    // 1. Vérifier si le mode automatique est activé
    const settingsSnap = await db.ref("system_settings").once("value");
    const settings = settingsSnap.val();

    // Sécurité si les settings n'existent pas encore
    if (!settings || !settings.is_auto) {
      console.log("ℹ️ Mode Auto désactivé. Aucune action entreprise.");
      return;
    }

    const isDemoMode = process.env.APP_MODE === "DEMO";
    const now = new Date();
    const hour = now.getHours();
    const activeHours = [8, 13, 18];

    // En mode DEMO, on considère qu'on est toujours dans un créneau horaire (TimeSlot)
    const isTimeSlot = isDemoMode || activeHours.includes(hour);

    // 2. Mise à jour de la météo (seulement aux heures clés ou en mode démo)
    if (isTimeSlot) {
      await updateForecast();
    }

    // 3. Récupération des données nécessaires
    const [forecastSnap, zonesSnap] = await Promise.all([
      db.ref("environment/forecast").once("value"),
      db.ref("zones").once("value"),
    ]);

    const forecast = forecastSnap.val() || { rain_prob: 0 };
    const zones = zonesSnap.val();

    if (!zones) return;

    const { rain_prob } = forecast;
    const updates = {};

    // 4. Analyse de chaque zone
    for (const [id, zone] of Object.entries(zones)) {
      const { soil_moisture, pump_status } = zone.state;
      const {
        threshold_normal,
        threshold_critical,
        rain_threshold_limit,
        watering_duration_sec,
      } = zone.config;

      const isCritical = soil_moisture <= threshold_critical;
      const isDry = soil_moisture <= threshold_normal;
      const willNotRain = rain_prob < rain_threshold_limit;

      // LOGIQUE DE DÉCISION
      // On arrose si : URGENCE CRITIQUE OR (C'est l'heure AND c'est sec AND il ne va pas pleuvoir)
      if (isCritical || (isTimeSlot && isDry && willNotRain)) {
        // On ne déclenche l'arrosage que si la pompe est actuellement éteinte
        if (!pump_status) {
          console.log(
            `🌊 [DÉCISION] Activation pompe ${id} (${isDemoMode ? "MODE DEMO" : "MODE PROD"})`
          );

          updates[`zones/${id}/state/pump_status`] = true;

          // Planification de l'arrêt automatique après X secondes
          setTimeout(async () => {
            try {
              const stopUpdates = {};
              stopUpdates[`zones/${id}/state/pump_status`] = false;
              stopUpdates[`zones/${id}/state/last_irrigation`] =
                new Date().toISOString();

              await db.ref().update(stopUpdates);
              console.log(`🛑 [ACTION] Arrêt automatique pompe ${id} terminé.`);
            } catch (err) {
              console.error(
                `❌ Erreur lors de l'arrêt de la pompe ${id}:`,
                err
              );
            }
          }, watering_duration_sec * 1000);
        }
      }
    }

    // 5. Appliquer toutes les activations de pompes d'un coup
    if (Object.keys(updates).length > 0) {
      await db.ref().update(updates);
    }
  } catch (error) {
    console.error("❌ Erreur critique dans le Decision Engine:", error.message);
  }
};
