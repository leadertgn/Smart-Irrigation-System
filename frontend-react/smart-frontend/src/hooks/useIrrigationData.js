import { useState, useEffect } from 'react';
import { db } from '../config/firebase';
import { ref, onValue } from 'firebase/database';

export const useIrrigationData = () => {
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const dbRef = ref(db, '/'); // On écoute la racine pour tout avoir d'un coup
    
    // onValue crée un abonnement temps réel
    const unsubscribe = onValue(dbRef, (snapshot) => {
      if (snapshot.exists()) {
        setData(snapshot.val());
      }
      setLoading(false);
    });

    return () => unsubscribe(); // Nettoyage de l'abonnement
  }, []);

  return { data, loading };
};