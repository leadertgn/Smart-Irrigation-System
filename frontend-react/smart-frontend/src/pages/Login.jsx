import React, { useState } from 'react';
import { auth } from '../config/firebase';
import { signInWithEmailAndPassword } from 'firebase/auth';
import { Leaf } from 'lucide-react';

const Login = () => {
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const [error, setError] = useState('');

  const handleLogin = async (e) => {
    e.preventDefault();
    try {
      await signInWithEmailAndPassword(auth, email, password);
    } catch (err) {
      setError("Identifiants incorrects ou accès refusé.");
      console.log('err : ',err);
    }
  };

  return (
    <div className="min-h-screen flex items-center justify-center bg-slate-100 p-4">
      <div className="bg-white p-8 rounded-3xl shadow-xl w-full max-w-md border border-slate-200">
        <div className="flex flex-col items-center mb-8">
          <div className="bg-blue-600 p-3 rounded-2xl mb-4 shadow-lg shadow-blue-200">
            <Leaf className="text-white" size={32} />
          </div>
          <h2 className="text-2xl font-black text-slate-800">SMART GROW</h2>
          <p className="text-slate-400 text-sm">Connectez-vous pour piloter le système</p>
        </div>

        <form onSubmit={handleLogin} className="space-y-4">
          <div>
            <label className="text-xs font-bold text-slate-400 uppercase ml-1">Email</label>
            <input 
              type="email" 
              className="w-full p-3 bg-slate-50 border border-slate-200 rounded-xl mt-1 focus:ring-2 focus:ring-blue-500 outline-none transition-all"
              placeholder="admin@smartgrow.com"
              onChange={(e) => setEmail(e.target.value)}
              required
            />
          </div>
          <div>
            <label className="text-xs font-bold text-slate-400 uppercase ml-1">Mot de passe</label>
            <input 
              type="password" 
              className="w-full p-3 bg-slate-50 border border-slate-200 rounded-xl mt-1 focus:ring-2 focus:ring-blue-500 outline-none transition-all"
              placeholder="••••••••"
              onChange={(e) => setPassword(e.target.value)}
              required
            />
          </div>
          {error && <p className="text-red-500 text-xs font-bold text-center">{error}</p>}
          <button 
            type="submit"
            className="w-full bg-blue-600 text-white font-bold p-4 rounded-xl shadow-lg shadow-blue-100 hover:bg-blue-700 transition-all active:scale-95"
          >
            SE CONNECTER
          </button>
        </form>
      </div>
    </div>
  );
};

export default Login;