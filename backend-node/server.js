import 'dotenv/config'; // Charge les variables du .env dès le départ
import http from 'http';
import app from './src/app.js';
import { initHistoryService } from './src/services/historyService.js';
import { updateForecast } from './src/services/weatherService.js';
const PORT = process.env.PORT || 3000;

const server = http.createServer(app);

server.on('error', (error) => {
  console.error('Erreur serveur :', error);
});

const isDev = process.env.NODE_ENV === "development";
const APP_NAME = "Smart Irrigation API";
server.listen(PORT, async () => {
  console.log(
    isDev
      ? `🚀 Serveur en ligne (DEV) → http://localhost:${PORT}`
      : `🚀 Serveur démarré en PRODUCTION (PORT ${PORT})`
  );
  try {
    await initHistoryService();
    console.log("📦 Cache historique centralisé initialisé depuis au démarrage du serveur");

    await updateForecast();
    console.log("🌤️ Météo initialisée avec succès au démarrage du serveur ");
    console.log(`🚀 ${APP_NAME} démarrée (${isDev ? "DEV" : "PROD"})`);
    console.log("📡 En attente de données IoT...");
  } catch (error) {
    console.error("❌ Erreur au démarrage du serveur :", error);
    process.exit(1); // fail fast en prod
  }
});
