export const evaluateIrrigation = async () => {
    try {
        const settingsSnap = await db.ref('system_settings').once('value');
        const { is_auto } = settingsSnap.val();
        if (!is_auto) return;

        const isDemoMode = process.env.APP_MODE === 'DEMO'; // Lecture du mode
        const now = new Date();
        const hour = now.getHours();
        const activeHours = [8, 13, 18];

        // En mode DEMO, on considère qu'on est toujours dans un "TimeSlot"
        const isTimeSlot = isDemoMode || activeHours.includes(hour);

        if (activeHours.includes(hour) || isDemoMode) {
           await updateForecast();
        }

        const forecastSnap = await db.ref('environment/forecast').once('value');
        const zonesSnap = await db.ref('zones').once('value');
        
        const { rain_prob } = forecastSnap.val();
        const zones = zonesSnap.val();
        const updates = {};

        for (const [id, zone] of Object.entries(zones)) {
            const { soil_moisture } = zone.state;
            const { threshold_normal, threshold_critical, rain_threshold_limit, watering_duration_sec } = zone.config;

            const isCritical = soil_moisture <= threshold_critical;
            const isDry = soil_moisture <= threshold_normal;
            const willNotRain = rain_prob < rain_threshold_limit;

            // Logique augmentée : En mode DEMO, l'arrosage Dry + NoRain se déclenche tout le temps
            if (isCritical || (isTimeSlot && isDry && willNotRain)) {
                
                if (!zone.state.pump_status) {
                    console.log(`🌊 [ACTION] Activation pompe ${id} (${isDemoMode ? 'MODE DEMO' : 'MODE PROD'})`);
                    updates[`zones/${id}/state/pump_status`] = true;

                    setTimeout(async () => {
                        await db.ref(`zones/${id}/state/pump_status`).set(false);
                        await db.ref(`zones/${id}/state/last_irrigation`).set(new Date().toISOString());
                        console.log(`🛑 [ACTION] Arrêt automatique pompe ${id}`);
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