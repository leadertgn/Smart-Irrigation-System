import express from "express";
import cors from "cors";
import dataRoutes from "./routes/dataRoutes.js";

const app = express();

// Middlewares globaux
app.use(cors());
app.use(express.json()); // Indispensable pour lire le JSON envoyé par l'ESP8266
app.use("/api/data", dataRoutes);
// Route de santé (Healthcheck)
app.get("/ping", (req, res) => {
  res.status(200).json({ status: "OK", message: "Backend opérationnel" });
});

// Montage des routes (exemple)
// app.use('/api/data', dataRoutes);

// Gestion des routes non trouvées
app.use((req, res) => {
  res.status(404).json({ error: "Route non trouvée" });
});

export default app;
