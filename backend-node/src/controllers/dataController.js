import { db } from '../config/firebase.js';
import { updateHistory } from '../services/historyService.js';
import { evaluateIrrigation } from '../services/decisionEngine.js'; // 1. Import du moteur

const validateDHT = (value, min, max) => {
    const num = parseFloat(value);
    if (isNaN(num) || num < min || num > max) return null;
    return num;
};

const convertMoisture = (raw, zoneId) => {
    const id = zoneId.toUpperCase();
    const min = parseInt(process.env[`${id}_RAW_MIN`]) || 200;
    const max = parseInt(process.env[`${id}_RAW_MAX`]) || 1023;
    const reverse = process.env[`${id}_REVERSE_LOGIC`] === 'true';

    let percentage = ((raw - min) / (max - min)) * 100;
    if (!reverse) percentage = 100 - percentage;
    return Math.max(0, Math.min(100, Math.round(percentage)));
};

export const collectData = async (req, res) => {
    try {
        const { air_temp, air_hum, zones } = req.body;
        const timestamp = new Date().toISOString();

        const validTemp = validateDHT(air_temp, -10, 60);
        const validHum = validateDHT(air_hum, 0, 100);

        const updates = {};
        const dataForHistory = {
            air_temp: validTemp,
            air_hum: validHum,
            z1_soil: 0,
            z2_soil: 0
        };

        if (validTemp !== null) updates['environment/current/air_temp'] = validTemp;
        if (validHum !== null) updates['environment/current/air_hum'] = validHum;
        if (validTemp !== null || validHum !== null) {
            updates['environment/current/last_sync'] = timestamp;
        }

        if (zones && Array.isArray(zones)) {
            zones.forEach(zone => {
                const zoneId = zone.id.toLowerCase();
                const calibratedMoisture = convertMoisture(zone.raw, zoneId);

                updates[`zones/${zoneId}/state/soil_moisture`] = calibratedMoisture;
                updates[`zones/${zoneId}/state/last_sync`] = timestamp;

                if (zoneId === 'z1') dataForHistory.z1_soil = calibratedMoisture;
                if (zoneId === 'z2') dataForHistory.z2_soil = calibratedMoisture;

                console.log(`[DATA] Zone ${zoneId.toUpperCase()} : ${zone.raw} -> ${calibratedMoisture}%`);
            });
        }

        // 2. Mise à jour Firebase (Temps réel)
        if (Object.keys(updates).length > 0) {
            await db.ref().update(updates);
        }

        // 3. Mise à jour de l'historique horaire
        await updateHistory(dataForHistory);

        // 4. APPEL DU MOTEUR DE DÉCISION
        // On lui passe les données fraîches pour qu'il analyse la situation
        await evaluateIrrigation(); 

        return res.status(201).json({ 
            status: 'success', 
            message: 'Données traitées, historique mis à jour et moteur de décision exécuté',
            timestamp 
        });

    } catch (error) {
        console.error('🔥 Erreur dans collectData:', error.message);
        return res.status(500).json({ error: 'Erreur interne du serveur' });
    }
};