/**
 * =================================================================
 * FIREBASE CONFIGURATION & INITIALIZATION
 * Project: IoT Room Occupancy Monitoring System
 * Author: Alif Adji Shaputra (FTIK UPS Tegal)
 * =================================================================
 */

const firebaseConfig = {
    databaseURL: "https://okupansi-ruangan-default-rtdb.asia-southeast1.firebasedatabase.app"
};

// Inisialisasi Firebase App
firebase.initializeApp(firebaseConfig);

// Export Reference Realtime Database
const database = firebase.database();