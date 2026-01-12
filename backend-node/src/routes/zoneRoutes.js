import { Router } from 'express';
import * as zoneController from '../controllers/zoneController.js';
import { verifyToken } from '../middlewares/authMiddleware.js'; // Import du middleware
const router = Router();

// Modifier la config : PATCH /api/zones/z1/config
router.patch('/:id/config', verifyToken,zoneController.updateZoneConfig);

// Forçage pompe : POST /api/zones/z1/pump
router.post('/:id/pump',verifyToken, zoneController.toggleManualPump);

export default router;