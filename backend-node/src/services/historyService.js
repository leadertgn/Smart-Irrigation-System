import { db } from "../config/firebase.js";

let historyCache = null;

export const initHistoryService = async () => {
  try {
    const snapshot = await db.ref("history").once("value");
    historyCache = snapshot.val() || {
      timestamps: [],
      air_temp: [],
      air_hum: [],
      z1_soil: [],
      z2_soil: [],
    };
    console.log("📦 Cache historique centralisé initialisé");
  } catch (error) {
    console.error("❌ Erreur initialisation historique:", error);
  }
};

export const updateHistory = async (currentData) => {
  if (!historyCache) await initHistoryService();

  const now = new Date();
  const currentHourKey = now.toISOString().substring(0, 13) + ":00Z";
  const isDemoMode = process.env.APP_MODE === 'DEMO';

  // Condition : On ajoute si c'est une nouvelle heure OU si on est en mode DEMO
  const shouldUpdate = 
    isDemoMode || 
    historyCache.timestamps.length === 0 ||
    historyCache.timestamps[historyCache.timestamps.length - 1] !== currentHourKey;

  if (shouldUpdate) {
    // En mode DEMO, on met un timestamp précis (ISO complet) pour voir les points se suivre
    // En mode PROD, on garde la clé de l'heure
    const timeToSave = isDemoMode ? now.toISOString() : currentHourKey;
    const limit = 24; // Tu peux augmenter à 50 en mode DEMO pour voir plus de points

    pushAndLimit(historyCache.timestamps, timeToSave, limit);
    pushAndLimit(historyCache.air_temp, currentData.air_temp, limit);
    pushAndLimit(historyCache.air_hum, currentData.air_hum, limit);
    pushAndLimit(historyCache.z1_soil, currentData.z1_soil, limit);
    pushAndLimit(historyCache.z2_soil, currentData.z2_soil, limit);

    await db.ref("history").set(historyCache);
    console.log(`📈 Historique mis à jour (${isDemoMode ? 'MODE DEMO' : 'MODE PROD'})`);
  }
};

function pushAndLimit(array, value, limit) {
  array.push(value);
  if (array.length > limit) array.shift();
}
