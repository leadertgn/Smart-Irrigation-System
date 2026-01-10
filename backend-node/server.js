import express from "express";
import { configDotenv } from "dotenv";
const app = express();
const PORT = process.env.PORT || 3000;


app.get('/', (req, res) => {
  res.json({ status: 'Backend is running 🚀' })
})

app.get('/health', (req, res) => {
  res.status(200).send('OK')
})

app.listen(PORT, () => {
  console.log(`Server running on port ${PORT} to http://localhost:${PORT}`);
})