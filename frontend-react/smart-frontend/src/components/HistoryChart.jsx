import React from 'react';
import { 
  AreaChart, Area, LineChart, Line, XAxis, YAxis, CartesianGrid, 
  Tooltip, ResponsiveContainer, Legend, ReferenceLine 
} from 'recharts';
import { Thermometer, Droplets, Activity } from 'lucide-react';

const HistoryChart = ({ history }) => {
  if (!history || !history.timestamps || !Array.isArray(history.timestamps)) {
    return (
      <div className="bg-white rounded-2xl p-6 border border-slate-100 text-center text-slate-400">
        Aucune donnée historique disponible.
      </div>
    );
  }

  // Préparation des données combinées
  const data = history.timestamps.map((t, index) => ({
    time: new Date(t).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }),
    z1: history.z1_soil ? history.z1_soil[index] : 0,
    z2: history.z2_soil ? history.z2_soil[index] : 0,
    airTemp: history.air_temp ? history.air_temp[index] : 0,
    airHum: history.air_hum ? history.air_hum[index] : 0,
  }));

  return (
    <div className="space-y-6">
      {/* --- GRAPHIQUE 1 : HUMIDITÉ DU SOL --- */}
      <div className="bg-white rounded-2xl shadow-sm border border-slate-100 p-6">
        <h3 className="text-lg font-bold text-slate-800 mb-6 flex items-center gap-2">
          <Activity className="text-blue-500" size={20} />
          Analyse des seuils d'humidité du sol
        </h3>
        
        <div className="h-[300px] w-full"> 
          <ResponsiveContainer width="100%" height="100%">
            <AreaChart data={data}>
              <defs>
                <linearGradient id="colorZ1" x1="0" y1="0" x2="0" y2="1">
                  <stop offset="5%" stopColor="#3b82f6" stopOpacity={0.1}/>
                  <stop offset="95%" stopColor="#3b82f6" stopOpacity={0}/>
                </linearGradient>
                <linearGradient id="colorZ2" x1="0" y1="0" x2="0" y2="1">
                  <stop offset="5%" stopColor="#10b981" stopOpacity={0.1}/>
                  <stop offset="95%" stopColor="#10b981" stopOpacity={0}/>
                </linearGradient>
              </defs>
              <CartesianGrid strokeDasharray="3 3" vertical={false} stroke="#f1f5f9" />
              <XAxis dataKey="time" axisLine={false} tickLine={false} tick={{fill: '#94a3b8', fontSize: 10}} />
              <YAxis domain={[0, 100]} axisLine={false} tickLine={false} tick={{fill: '#94a3b8', fontSize: 12}} unit="%" />
              <Tooltip contentStyle={{ borderRadius: '12px', border: 'none', boxShadow: '0 4px 12px rgba(0,0,0,0.1)' }} />
              <Legend verticalAlign="top" align="right" iconType="circle" wrapperStyle={{paddingBottom: '20px'}} />

              <ReferenceLine y={45} stroke="#94a3b8" strokeDasharray="5 5" label={{ position: 'right', value: 'Optimal', fill: '#94a3b8', fontSize: 10 }} />
              <ReferenceLine y={25} stroke="#ef4444" strokeDasharray="3 3" label={{ position: 'right', value: 'CRITIQUE', fill: '#ef4444', fontSize: 10, fontWeight: 'bold' }} />

              <Area name="Zone Nord (Z1)" type="monotone" dataKey="z1" stroke="#3b82f6" strokeWidth={3} fillOpacity={1} fill="url(#colorZ1)" />
              <Area name="Zone Sud (Z2)" type="monotone" dataKey="z2" stroke="#10b981" strokeWidth={3} fillOpacity={1} fill="url(#colorZ2)" />
            </AreaChart>
          </ResponsiveContainer>
        </div>
      </div>

      {/* --- GRAPHIQUE 2 : CONDITIONS ATMOSPHÉRIQUES --- */}
      <div className="bg-white rounded-2xl shadow-sm border border-slate-100 p-6">
        <h3 className="text-lg font-bold text-slate-800 mb-6 flex items-center gap-2">
          <Droplets className="text-cyan-500" size={20} />
          Conditions Air & Température
        </h3>
        
        <div className="h-[250px] w-full"> 
          <ResponsiveContainer width="100%" height="100%">
            <LineChart data={data}>
              <CartesianGrid strokeDasharray="3 3" vertical={false} stroke="#f1f5f9" />
              <XAxis dataKey="time" axisLine={false} tickLine={false} tick={{fill: '#94a3b8', fontSize: 10}} />
              
              {/* Double axe Y : Gauche pour Temp (°C), Droite pour Hum (%) */}
              <YAxis yAxisId="left" axisLine={false} tickLine={false} tick={{fill: '#f59e0b', fontSize: 12}} unit="°C" />
              <YAxis yAxisId="right" orientation="right" axisLine={false} tickLine={false} tick={{fill: '#06b6d4', fontSize: 12}} unit="%" />
              
              <Tooltip contentStyle={{ borderRadius: '12px', border: 'none', boxShadow: '0 4px 12px rgba(0,0,0,0.1)' }} />
              <Legend verticalAlign="top" align="right" iconType="circle" />

              <Line yAxisId="left" name="Température Air" type="monotone" dataKey="airTemp" stroke="#f59e0b" strokeWidth={2} dot={{ r: 4 }} activeDot={{ r: 6 }} />
              <Line yAxisId="right" name="Humidité Air" type="monotone" dataKey="airHum" stroke="#06b6d4" strokeWidth={2} dot={{ r: 4 }} activeDot={{ r: 6 }} />
            </LineChart>
          </ResponsiveContainer>
        </div>
      </div>
    </div>
  );
};

export default HistoryChart;