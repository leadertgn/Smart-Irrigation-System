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

server.listen(PORT, async () => {
  console.log(`🚀 Serveur en ligne sur http://localhost:${PORT}`);
  await initHistoryService(); // Remplit le tableau persistant au démarrage
  await updateForecast(); // <--- Ici pour avoir la météo réelle dès le début
  console.log(`📡 En attente de données IoT...`);
});