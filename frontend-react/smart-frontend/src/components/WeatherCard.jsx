import React from 'react';
import { CloudRain, Thermometer, Droplets, Wind } from 'lucide-react';

const WeatherCard = ({ data }) => {
  if (!data) return null;

  const { current, forecast } = data;

  return (
    <div className="bg-gradient-to-br from-blue-600 to-blue-700 rounded-2xl shadow-lg p-6 text-white">
      <div className="flex justify-between items-start mb-6">
        <div>
          <h3 className="text-blue-100 text-sm font-medium uppercase tracking-wider">Météo Locale</h3>
          <p className="text-2xl font-bold">Lokossa</p>
        </div>
        <CloudRain size={32} className="text-blue-200 animate-pulse" />
      </div>

      <div className="grid grid-cols-2 gap-4">
        {/* Température */}
        <div className="bg-white/10 rounded-xl p-3 backdrop-blur-md">
          <div className="flex items-center text-blue-100 text-xs mb-1">
            <Thermometer size={14} className="mr-1" /> Température
          </div>
          <div className="text-xl font-bold">{current.air_temp} °C</div>
        </div>

        {/* Humidité Air */}
        <div className="bg-white/10 rounded-xl p-3 backdrop-blur-md">
          <div className="flex items-center text-blue-100 text-xs mb-1">
            <Droplets size={14} className="mr-1" /> Humidité Air
          </div>
          <div className="text-xl font-bold">{current.air_hum} %</div>
        </div>
      </div>

      <div className="mt-6 flex items-center justify-between bg-white/20 rounded-xl p-4">
        <div className="text-sm">
          <span className="block text-blue-100 text-xs uppercase">Prévision Pluie</span>
          <span className="font-bold text-lg">{forecast.rain_prob}% de probabilité</span>
        </div>
        <div className={`px-3 py-1 rounded-full text-xs font-bold ${forecast.is_raining_soon ? 'bg-red-500 text-white' : 'bg-green-500 text-white'}`}>
          {forecast.is_raining_soon ? 'ARROSAGE SUSPENDU' : 'CIEL DÉGAGÉ'}
        </div>
      </div>
    </div>
  );
};

export default WeatherCard;