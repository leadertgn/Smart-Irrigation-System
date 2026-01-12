import React, { useState } from 'react';
import api from '../api/axios';
import { Power, Settings } from 'lucide-react';

const ControlPanel = ({ systemSettings, zones }) => {
  const [loading, setLoading] = useState(false);

  // Fonction pour changer le mode (Auto/Manuel)
  const toggleSystemMode = async () => {
    setLoading(true);
    try {
      // On inverse l'état actuel de is_auto
      await api.patch('/system/settings', { is_auto: !systemSettings?.is_auto });
    } catch (error) {
      console.error("Erreur mode:", error);
      alert("Impossible de changer le mode système.");
    } finally {
      setLoading(false);
    }
  };

  // Fonction pour forcer une pompe
  const togglePump = async (zoneId, currentStatus) => {
    try {
      const action = currentStatus ? "OFF" : "ON";
      await api.post(`/zones/${zoneId}/pump`, { action });
    } catch (error) {
      console.error("Erreur pompe:", error);
      alert(`Erreur lors de la commande de la pompe ${zoneId}`);
    }
  };

  return (
    <div className="bg-white rounded-2xl shadow-sm border border-slate-100 p-6">
      {/* Header du panneau */}
      <div className="flex flex-col sm:flex-row items-center justify-between mb-6 gap-4">
        <h3 className="text-lg font-bold text-slate-800 flex items-center">
          <Settings className="mr-2 text-slate-400" size={20} /> 
          Configuration & Commandes
        </h3>
        <button 
          onClick={toggleSystemMode}
          disabled={loading}
          className={`w-full sm:w-auto px-4 py-2 rounded-xl font-bold transition-all shadow-sm ${
            systemSettings?.is_auto 
            ? 'bg-green-100 text-green-700 hover:bg-green-200 border border-green-200' 
            : 'bg-orange-100 text-orange-700 hover:bg-orange-200 border border-orange-200'
          }`}
        >
          {systemSettings?.is_auto ? '🤖 MODE AUTO ACTIVÉ' : '👤 MODE MANUEL ACTIVÉ'}
        </button>
      </div>

      {/* Liste des pompes par zone */}
      <div className="space-y-4">
        {zones && Object.keys(zones).map((id) => (
          <div key={id} className="flex items-center justify-between p-4 bg-slate-50 rounded-xl border border-slate-100">
            <div>
              <span className="text-xs font-bold text-slate-400 uppercase tracking-widest">{id}</span>
              <p className="font-semibold text-slate-700">Pompe d'arrosage</p>
            </div>
            
            <button
              // Sécurité : on utilise le chaînage optionnel ?. pour éviter le crash
              onClick={() => togglePump(id, zones[id]?.state?.pump_status)}
              // On désactive le bouton si le système est en mode Automatique
              disabled={systemSettings?.is_auto} 
              className={`flex items-center px-6 py-2 rounded-lg font-bold transition-all ${
                zones[id]?.state?.pump_status
                ? 'bg-blue-600 text-white shadow-lg shadow-blue-200'
                : 'bg-slate-200 text-slate-400'
              } ${systemSettings?.is_auto ? 'opacity-50 cursor-not-allowed' : 'hover:scale-105 active:scale-95'}`}
            >
              <Power size={18} className="mr-2" />
              {zones[id]?.state?.pump_status ? 'EN MARCHE' : 'ARRÊTÉE'}
            </button>
          </div>
        ))}
      </div>
      
      {/* Message d'information si mode AUTO */}
      {systemSettings?.is_auto && (
        <div className="mt-4 p-3 bg-blue-50 rounded-lg border border-blue-100">
          <p className="text-xs text-center text-blue-600 font-medium">
            ℹ️ Le mode Automatique gère les pompes selon vos seuils. Passez en mode Manuel pour un contrôle direct.
          </p>
        </div>
      )}
    </div>
  );
};

export default ControlPanel;