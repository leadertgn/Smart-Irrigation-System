import { db } from '../config/firebase.js';
import { updateForecast } from './weatherService.js';

export const evaluateIrrigation = async () => {
    try {
        const settingsSnap = await db.ref('system_settings').once('value');
        const { is_auto } = settingsSnap.val();
        if (!is_auto) return;

        const now = new Date();
        const hour = now.getHours();
       // const hour = 13 // tests
        const activeHours = [8, 13, 18];

        // 1. Si on est dans une heure d'arrosage, on rafraîchit la météo d'abord
        if (activeHours.includes(hour)) {
           await updateForecast();
        }

        // 2. Récupérer les données fraîches (météo + zones)
        const forecastSnap = await db.ref('environment/forecast').once('value');
        const zonesSnap = await db.ref('zones').once('value');
        
        const { rain_prob } = forecastSnap.val();
        const zones = zonesSnap.val();
        const updates = {};

        for (const [id, zone] of Object.entries(zones)) {
            const { soil_moisture } = zone.state;
            const { threshold_normal, threshold_critical, rain_threshold_limit, watering_duration_sec } = zone.config;

            const isCritical = soil_moisture <= threshold_critical;
            const isTimeSlot = activeHours.includes(hour);
            const isDry = soil_moisture <= threshold_normal;
            const willNotRain = rain_prob < rain_threshold_limit;

            // Logique : Arrosage si URGENCE ou (HEURE + SEC + PAS DE PLUIE)
            if (isCritical || (isTimeSlot && isDry && willNotRain)) {
                
                // On ne déclenche que si la pompe n'est pas déjà ON
                if (!zone.state.pump_status) {
                    console.log(`🌊 [ACTION] Activation pompe ${id}`);
                    updates[`zones/${id}/state/pump_status`] = true;

                    setTimeout(async () => {
                        await db.ref(`zones/${id}/state/pump_status`).set(false);
                        await db.ref(`zones/${id}/state/last_irrigation`).set(new Date().toISOString());
                        console.log(`🛑 [ACTION] Arrêt pompe ${id}`);
                    }, watering_duration_sec * 1000);
                }
            }
        }

        if (Object.keys(updates).length > 0) {
            await db.ref().update(updates);
        }

    } catch (error) {
        console.error("❌ Erreur Decision Engine:", error);
    }
};