import { db } from '../config/firebase.js';

export const updateSystemSettings = async (req, res) => {
    try {
        const { is_auto } = req.body;

        if (typeof is_auto !== 'boolean') {
            return res.status(400).json({ error: "Le champ 'is_auto' doit être un booléen." });
        }

        await db.ref('system_settings').update({ is_auto });
        
        console.log(`⚙️ Mode système changé : ${is_auto ? 'AUTOMATIQUE' : 'MANUEL'}`);
        res.json({ message: `Mode ${is_auto ? 'Auto' : 'Manuel'} activé` });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

export const getFullStatus = async (req, res) => {
    try {
        const snapshot = await db.ref().once('value');
        const data = snapshot.val();
        
        // On renvoie une version propre des données au front
        res.json({
            environment: data.environment,
            zones: data.zones,
            system_settings: data.system_settings,
            history: data.history
        });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};