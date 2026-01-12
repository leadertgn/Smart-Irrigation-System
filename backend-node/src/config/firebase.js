import admin from "firebase-admin";

const serviceAccount = {
  projectId: process.env.FIREBASE_PROJECT_ID?.trim(),
  clientEmail: process.env.FIREBASE_CLIENT_EMAIL?.trim(),
  privateKey: process.env.FIREBASE_PRIVATE_KEY?.replace(/\\n/g, '\n').trim()?.replace(/"/g, "")
};

try {
  admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
    databaseURL: process.env.FIREBASE_DB_URL
  });
  console.log("✅ SDK Firebase initialisé sans erreur");
} catch (error) {
  console.error("❌ Erreur fatale Firebase:", error.message);
}

const db = admin.database();
const auth = admin.auth();

export { db, auth };