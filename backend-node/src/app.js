import express from "express";
import cors from "cors";
import dataRoutes from "./routes/dataRoutes.js";
import zoneRoutes from "./routes/zoneRoutes.js"; 
import systemRoutes from "./routes/systemRoutes.js";

const app = express();

app.use(cors());
app.use(express.json());

// Routes pour l'ESP8266
app.use("/api/data", dataRoutes);

// Routes pour le Dashboard React
app.use("/api/zones", zoneRoutes);
app.use("/api/system", systemRoutes);

app.get("/ping", (req, res) => {
  res.status(200).json({ status: "OK", message: "Backend opérationnel" });
});

app.use((req, res) => {
  res.status(404).json({ error: "Route non trouvée" });
});

export default app;