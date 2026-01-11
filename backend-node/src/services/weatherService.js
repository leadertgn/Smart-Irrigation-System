import axios from 'axios';
import { db } from '../config/firebase.js';

export const updateForecast = async () => {
    try {
        const apiKey = process.env.WEATHER_API_KEY;
        const city = process.env.CITY_NAME || 'Cotonou';
        // API "Current Weather" pour la description et "Forecast" pour la probabilité de pluie (pop)
        // Note: On utilise l'API 3-hour forecast car elle donne le champ 'pop' (Probability of Precipitation)
        const url = `https://api.openweathermap.org/data/2.5/forecast?q=${city}&appid=${apiKey}&units=metric&cnt=1&lang=fr`;

        const response = await axios.get(url);
        const nextPoint = response.data.list[0];

        const forecastData = {
            // pop est un float entre 0 et 1 (0.2 = 20%)
            rain_prob: Math.round((nextPoint.pop || 0) * 100),
            description: nextPoint.weather[0].description,
            last_updated: new Date().toISOString()
        };

        await db.ref('environment/forecast').set(forecastData);
        console.log(`🌤️ Météo actualisée : ${forecastData.description} (${forecastData.rain_prob}% pluie)`);
        return forecastData;
    } catch (error) {
        console.error("❌ Erreur API Météo:", error.response?.data?.message || error.message);
        return null;
    }
};