import { Router } from 'express';
import { updateSystemSettings, getFullStatus } from '../controllers/systemController.js';
import { verifyToken } from '../middlewares/authMiddleware.js';
const router = Router();

// Modifier le mode : PATCH /api/system/settings
router.patch('/settings',verifyToken, updateSystemSettings);
router.get('/status',verifyToken, getFullStatus )
export default router;