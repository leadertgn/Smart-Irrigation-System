import React, { useState } from 'react';
import { Droplets, Settings2, Save, X } from 'lucide-react';
import api from '../api/axios';

const SoilCard = ({ zoneId, label, moisture, config }) => {
  const [isEditing, setIsEditing] = useState(false);
  const [formData, setFormData] = useState(config);
  const [loading, setLoading] = useState(false);

  const handleSave = async () => {
    setLoading(true);
    try {
      await api.patch(`/zones/${zoneId}/config`, formData);
      setIsEditing(false);
    } catch (error) {
      alert(`Erreur lors de la mise à jour de la configuration : ${error}`);
    } finally {
      setLoading(false);
    }
  };

  const getColor = (val) => {
    if (val < config.threshold_critical) return 'text-red-500';
    if (val < config.threshold_normal) return 'text-orange-500';
    return 'text-blue-500';
  };

  return (
    <div className="bg-white rounded-2xl shadow-sm p-6 border border-slate-100 transition-all">
      <div className="flex justify-between items-center mb-4">
        <h3 className="font-semibold text-slate-700">{label}</h3>
        <button 
          onClick={() => setIsEditing(!isEditing)}
          className="p-2 hover:bg-slate-50 rounded-full text-slate-400"
        >
          {isEditing ? <X size={20} /> : <Settings2 size={20} />}
        </button>
      </div>
      
      {!isEditing ? (
        // --- VUE AFFICHAGE ---
        <>
          <div className="flex flex-col items-center justify-center py-4">
            <div className={`text-5xl font-bold mb-2 ${getColor(moisture)}`}>
              {moisture}%
            </div>
            <div className="flex items-center text-slate-400 text-sm italic">
              <Droplets size={16} className="mr-1" /> Humidité du sol
            </div>
          </div>
          <div className="mt-4 grid grid-cols-2 gap-2 text-[10px] uppercase font-bold text-slate-400">
             <div>Seuil Normal: <span className="text-slate-600">{config.threshold_normal}%</span></div>
             <div>Seuil Critique: <span className="text-slate-600">{config.threshold_critical}%</span></div>
          </div>
        </>
      ) : (
        // --- VUE FORMULAIRE ---
        <div className="space-y-3 py-2">
          <div className="grid grid-cols-2 gap-3">
            <div>
              <label className="text-[10px] font-bold text-slate-400 uppercase">Seuil Normal (%)</label>
              <input 
                type="number" 
                className="w-full bg-slate-50 border border-slate-200 rounded-lg p-2 text-sm"
                value={formData.threshold_normal}
                onChange={(e) => setFormData({...formData, threshold_normal: parseInt(e.target.value)})}
              />
            </div>
            <div>
              <label className="text-[10px] font-bold text-slate-400 uppercase">Seuil Critique (%)</label>
              <input 
                type="number" 
                className="w-full bg-slate-50 border border-slate-200 rounded-lg p-2 text-sm"
                value={formData.threshold_critical}
                onChange={(e) => setFormData({...formData, threshold_critical: parseInt(e.target.value)})}
              />
            </div>
            <div>
              <label className="text-[10px] font-bold text-slate-400 uppercase">Limite Pluie (%)</label>
              <input 
                type="number" 
                className="w-full bg-slate-50 border border-slate-200 rounded-lg p-2 text-sm"
                value={formData.rain_threshold_limit}
                onChange={(e) => setFormData({...formData, rain_threshold_limit: parseInt(e.target.value)})}
              />
            </div>
            <div>
              <label className="text-[10px] font-bold text-slate-400 uppercase">Durée (Sec)</label>
              <input 
                type="number" 
                className="w-full bg-slate-50 border border-slate-200 rounded-lg p-2 text-sm"
                value={formData.watering_duration_sec}
                onChange={(e) => setFormData({...formData, watering_duration_sec: parseInt(e.target.value)})}
              />
            </div>
          </div>
          <button 
            onClick={handleSave}
            disabled={loading}
            className="w-full bg-blue-600 text-white rounded-lg py-2 text-sm font-bold flex items-center justify-center hover:bg-blue-700 transition-colors"
          >
            {loading ? "Enregistrement..." : <><Save size={16} className="mr-2"/> Sauvegarder</>}
          </button>
        </div>
      )}

      <div className="mt-4 pt-4 border-t border-slate-50">
        <div className="w-full bg-slate-100 rounded-full h-2">
          <div 
            className={`h-2 rounded-full transition-all duration-500 ${getColor(moisture).replace('text', 'bg')}`}
            style={{ width: `${moisture}%` }}
          ></div>
        </div>
      </div>
    </div>
  );
};

export default SoilCard;