import { db } from '../config/firebase.js';

// Mettre à jour les seuils et durées d'une zone
export const updateZoneConfig = async (req, res) => {
    try {
        const { id } = req.params; 
        const { threshold_normal, threshold_critical, rain_threshold_limit, watering_duration_sec } = req.body;

        // Validation logique
        if (threshold_normal <= threshold_critical) {
            return res.status(400).json({ error: "Le seuil normal doit être supérieur au seuil critique." });
        }

        await db.ref(`zones/${id}/config`).update({
            threshold_normal,
            threshold_critical,
            rain_threshold_limit,
            watering_duration_sec
        });
        
        res.json({ status: "success", message: `Configuration zone ${id} mise à jour` });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

// Activer/Désactiver la pompe manuellement
export const toggleManualPump = async (req, res) => {
    try {
        const { id } = req.params;
        const { action } = req.body; // attendu: "ON" ou "OFF"

        const zoneRef = db.ref(`zones/${id}`);
        const snap = await zoneRef.once('value');
        const zoneData = snap.val();

        if (action === "ON") {
            await zoneRef.child('state').update({ pump_status: true });
            
            // Sécurité : extinction auto basée sur la config de la zone
            setTimeout(async () => {
                await zoneRef.child('state').update({ 
                    pump_status: false,
                    last_irrigation: new Date().toISOString()
                });
                console.log(`🛑 Arrêt auto pompe ${id} (Cycle manuel terminé)`);
            }, zoneData.config.watering_duration_sec * 1000);

            res.json({ status: "success", message: `Pompe ${id} activée` });
        } else {
            await zoneRef.child('state').update({ pump_status: false });
            res.json({ status: "success", message: `Pompe ${id} arrêtée` });
        }
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};