// Import the functions you need from the SDKs you need
import { initializeApp } from "https://www.gstatic.com/firebasejs/11.1.0/firebase-app.js";
import { getAnalytics } from "https://www.gstatic.com/firebasejs/11.1.0/firebase-analytics.js";
import { getDatabase, ref, child, get } from "https://www.gstatic.com/firebasejs/11.1.0/firebase-database.js";

// Firebase configurations
const firebaseConfig = {
    apiKey: "AIzaSyDuY9TcLA0t3jk4-v6ulEmi6n4wl_ERyG0",
    authDomain: "medical-wearable.firebaseapp.com",
    databaseURL: "https://medical-wearable-default-rtdb.firebaseio.com",
    projectId: "medical-wearable",
    storageBucket: "medical-wearable.firebasestorage.app",
    messagingSenderId: "443567306370",
    appId: "1:443567306370:web:e46a5adbb2b5bb07cd269b",
    measurementId: "G-C8497DDRRW"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const analytics = getAnalytics(app);
const database = getDatabase(app);

// Read date from the realtime database function
export async function readData() {
    const databaseRef = ref(database);
    let measurements = {
        heart_rate: undefined,
        spo2: undefined,
        temperature: undefined
    };

    await get(child(databaseRef, 'Measurements/')).then((snapshot) => {
        measurements.heart_rate = snapshot.val().Heart_rate;
        measurements.spo2 = snapshot.val().SPO2;
        measurements.temperature = (snapshot.val().Temperature).toFixed(1);

        console.log(measurements);
    })

    return measurements;
}