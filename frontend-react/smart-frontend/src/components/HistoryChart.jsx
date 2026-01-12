import React from 'react';
import { 
  AreaChart, Area, XAxis, YAxis, CartesianGrid, 
  Tooltip, ResponsiveContainer, Legend, ReferenceLine 
} from 'recharts';

const HistoryChart = ({ history }) => {
  if (!history || !history.timestamps || !Array.isArray(history.timestamps)) {
    return (
      <div className="bg-white rounded-2xl p-6 border border-slate-100 text-center text-slate-400">
        Aucune donnée historique disponible.
      </div>
    );
  }

  const data = history.timestamps.map((t, index) => ({
    time: new Date(t).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }),
    z1: history.z1_soil ? history.z1_soil[index] : 0,
    z2: history.z2_soil ? history.z2_soil[index] : 0,
  }));

  return (
    <div className="bg-white rounded-2xl shadow-sm border border-slate-100 p-6">
      <h3 className="text-lg font-bold text-slate-800 mb-6 flex items-center">
        📈 Analyse des seuils d'humidité
      </h3>
      
      <div className="h-[350px] w-full min-h-[350px]"> 
        <ResponsiveContainer width="100%" height="100%" minWidth={0}>
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
            
            <XAxis 
              dataKey="time" 
              axisLine={false} 
              tickLine={false} 
              tick={{fill: '#94a3b8', fontSize: 10}}
            />
            
            <YAxis 
              domain={[0, 100]} 
              axisLine={false} 
              tickLine={false} 
              tick={{fill: '#94a3b8', fontSize: 12}}
              unit="%"
            />

            <Tooltip 
              contentStyle={{ borderRadius: '12px', border: 'none', boxShadow: '0 4px 12px rgba(0,0,0,0.1)' }}
            />
            
            <Legend verticalAlign="top" align="right" iconType="circle" wrapperStyle={{paddingBottom: '20px'}} />

            {/* --- LIGNES DE RÉFÉRENCE (SEUILS) --- */}
            <ReferenceLine 
              y={45} 
              label={{ position: 'right', value: 'Seuil Optimal', fill: '#94a3b8', fontSize: 10 }} 
              stroke="#94a3b8" 
              strokeDasharray="5 5" 
            />
            <ReferenceLine 
              y={25} 
              label={{ position: 'right', value: 'CRITIQUE', fill: '#ef4444', fontSize: 10, fontWeight: 'bold' }} 
              stroke="#ef4444" 
              strokeDasharray="3 3" 
            />

            <Area 
              name="Zone Nord (Z1)"
              type="monotone" 
              dataKey="z1" 
              stroke="#3b82f6" 
              strokeWidth={3}
              fillOpacity={1} 
              fill="url(#colorZ1)" 
            />
            
            <Area 
              name="Zone Sud (Z2)"
              type="monotone" 
              dataKey="z2" 
              stroke="#10b981" 
              strokeWidth={3}
              fillOpacity={1} 
              fill="url(#colorZ2)" 
            />
          </AreaChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
};

export default HistoryChart;