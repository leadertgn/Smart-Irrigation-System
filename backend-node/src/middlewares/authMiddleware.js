import { admin } from '../config/firebase.js'; // Assure-toi d'exporter l'admin depuis ta config

export const verifyToken = async (req, res, next) => {
    const authHeader = req.headers.authorization;

    if (!authHeader || !authHeader.startsWith('Bearer ')) {
        return res.status(401).json({ error: 'Accès non autorisé : Token manquant' });
    }

    const idToken = authHeader.split('Bearer ')[1];

    try {
        // Le serveur vérifie le jeton directement auprès de Firebase
        const decodedToken = await admin.auth().verifyIdToken(idToken);
        req.user = decodedToken; // On stocke les infos utilisateur si besoin
        next(); // C'est bon, on passe à la suite (le contrôleur)
    } catch (error) {
        console.error('Erreur vérification token:', error.message);
        return res.status(403).json({ error: 'Token invalide ou expiré' });
    }
};