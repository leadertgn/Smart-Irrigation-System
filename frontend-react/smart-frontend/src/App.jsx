import React, { useState, useEffect } from "react";
import { onAuthStateChanged, signOut } from "firebase/auth";
import { auth } from "./config/firebase";

// Import des composants
import { useIrrigationData } from "./hooks/useIrrigationData";
import WeatherCard from "./components/WeatherCard";
import SoilCard from "./components/SoilCard";
import ControlPanel from "./components/ControlPanel";
import HistoryChart from "./components/HistoryChart";
import Login from "./pages/Login";

// Import des icônes
import { LogOut, Leaf, ShieldCheck } from "lucide-react";

function App() {
  const [user, setUser] = useState(null);
  const [authLoading, setAuthLoading] = useState(true);
  
  // Hook personnalisé pour les données Firebase
  const { data, loading } = useIrrigationData();

  // 1. Surveillance de l'état de connexion (Firebase Auth)
  useEffect(() => {
    const unsubscribe = onAuthStateChanged(auth, (currentUser) => {
      setUser(currentUser);
      setAuthLoading(false);
    });
    return () => unsubscribe(); // Nettoyage de l'abonnement
  }, []);

  // 2. Gestion des états de chargement (Auth + Données)
  if (authLoading || (user && loading)) {
    return (
      <div className="min-h-screen flex items-center justify-center bg-slate-50">
        <div className="flex flex-col items-center">
          <div className="animate-spin rounded-full h-12 w-12 border-b-2 border-blue-600 mb-4"></div>
          <p className="text-slate-500 font-medium animate-pulse">Sécurisation de la connexion...</p>
        </div>
      </div>
    );
  }

  // 3. Si l'utilisateur n'est pas connecté, on affiche uniquement le Login
  if (!user) {
    return <Login />;
  }

  // 4. Sécurité si les données Firebase sont corrompues (après connexion)
  if (!data || !data.zones) {
    return (
      <div className="min-h-screen flex items-center justify-center bg-slate-50 p-6 text-center">
        <div className="bg-white p-8 rounded-3xl shadow-sm border border-red-100 max-w-md">
          <h2 className="text-red-600 font-bold text-xl mb-2">Structure de données absente</h2>
          <p className="text-slate-600 mb-4">
            Connexion réussie, mais impossible de lire les zones d'irrigation.
          </p>
          <button onClick={() => signOut(auth)} className="text-blue-600 font-bold underline">
            Se déconnecter
          </button>
        </div>
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-slate-50 p-4 md:p-8 font-sans">
      <div className="max-w-6xl mx-auto space-y-6">
        
        {/* --- BARRE SUPÉRIEURE (HEADER) --- */}
        <header className="flex flex-col md:flex-row md:items-center justify-between gap-4">
          <div className="flex items-center gap-3">
            <div className="bg-blue-600 p-2 rounded-xl shadow-lg shadow-blue-200">
              <Leaf className="text-white" size={24} />
            </div>
            <div>
              <h1 className="text-2xl font-black text-slate-800 tracking-tight leading-none">
                SMART GROW <span className="text-blue-600">.</span>
              </h1>
              <div className="flex items-center text-[10px] text-slate-400 font-bold uppercase tracking-widest mt-1">
                <ShieldCheck size={12} className="mr-1 text-green-500" /> 
                Session Sécurisée : {user.email}
              </div>
            </div>
          </div>

          <button 
            onClick={() => signOut(auth)}
            className="flex items-center self-start md:self-center px-4 py-2 bg-white border border-slate-200 rounded-xl text-xs font-bold text-slate-500 hover:text-red-500 hover:border-red-100 transition-all shadow-sm"
          >
            <LogOut size={14} className="mr-2" /> DÉCONNEXION
          </button>
        </header>

        {/* --- GRILLE PRINCIPALE (MÉTÉO & CAPTEURS) --- */}
        <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
          {/* Météo */}
          <div className="lg:col-span-1">
            <WeatherCard data={data.environment} />
          </div>

          {/* Zones d'humidité */}
          <div className="lg:col-span-2 grid grid-cols-1 md:grid-cols-2 gap-6">
            <SoilCard
              zoneId="z1"
              label={data.zones.z1?.metadata?.name || "Zone Nord"}
              moisture={data.zones.z1?.state?.soil_moisture || 0}
              config={data.zones.z1?.config}
            />
            <SoilCard
              zoneId="z2"
              label={data.zones.z2?.metadata?.name || "Zone Sud"}
              moisture={data.zones.z2?.state?.soil_moisture || 0}
              config={data.zones.z2?.config}
            />
          </div>
        </div>

        {/* --- SECTION HISTORIQUE (GRAPHIQUES) --- */}
        <div className="w-full">
          <HistoryChart history={data.history} />
        </div>

        {/* --- SECTION PILOTAGE (PANNEAU DE COMMANDE) --- */}
        <div className="max-w-3xl mx-auto w-full pt-4">
          <ControlPanel
            systemSettings={data.system_settings}
            zones={data.zones}
          />
        </div>

        {/* --- FOOTER --- */}
        <footer className="text-center py-8 border-t border-slate-200/50">
          <p className="text-[10px] text-slate-400 uppercase tracking-[0.2em] font-semibold">
            Dernière mise à jour système : {data.system_settings?.last_decision_engine_run 
              ? new Date(data.system_settings.last_decision_engine_run).toLocaleString('fr-FR')
              : 'En attente...'}
          </p>
        </footer>

      </div>
    </div>
  );
}

export default App;