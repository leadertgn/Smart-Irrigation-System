import { Router } from 'express';
import * as dataController from '../controllers/dataController.js';

const router = Router();

// Middleware simple pour vérifier le token de l'ESP
const verifyIotToken = (req, res, next) => {
    const token = req.headers['x-device-token'];
    if (token !== process.env.IOT_SECRET_TOKEN) {
        return res.status(401).json({ error: 'Token IoT invalide' });
    }
    next();
};

router.post('/collect', verifyIotToken, dataController.collectData);

export default router;