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

  // On n'ajoute un point que si l'heure a changé
  if (
    historyCache.timestamps.length === 0 ||
    historyCache.timestamps[historyCache.timestamps.length - 1] !==
      currentHourKey
  ) {
    const limit = 24;

    pushAndLimit(historyCache.timestamps, currentHourKey, limit);
    pushAndLimit(historyCache.air_temp, currentData.air_temp, limit);
    pushAndLimit(historyCache.air_hum, currentData.air_hum, limit);
    pushAndLimit(historyCache.z1_soil, currentData.z1_soil, limit);
    pushAndLimit(historyCache.z2_soil, currentData.z2_soil, limit);

    await db.ref("history").set(historyCache);
    console.log(`📈 Historique synchronisé pour ${currentHourKey}`);
  }
};

function pushAndLimit(array, value, limit) {
  array.push(value);
  if (array.length > limit) array.shift();
}
