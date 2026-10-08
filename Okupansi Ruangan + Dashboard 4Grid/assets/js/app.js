/**
 * MAIN CONTROLLER WITH AUTOMATIC HISTORY ARCHIVING & CLEANUP
 */

document.addEventListener("DOMContentLoaded", () => {
    UI.appendLog("Connecting to Firebase Realtime Cloud...", "info");

    let currentInfoDosen = null;
    let currentStatusSensor = "KOSONG";

    // 1. Listen Data Input Form Dosen dari /Ruangan/J101/info
    database.ref('/Ruangan/J101/info').on('value', (snapshot) => {
        currentInfoDosen = snapshot.val();
        UI.renderStatus(currentStatusSensor, currentInfoDosen);
    });

    // 2. Listen Status Okupansi Hardware ESP32 dari /Ruangan/J101/status
    database.ref('/Ruangan/J101/status').on('value', (snapshot) => {
        currentStatusSensor = snapshot.val();

        // JIKA STATUS BERUBAH JADI KOSONG DAN SEBELUMNYA ADA DATA DOSEN
        if (currentStatusSensor === "KOSONG" && currentInfoDosen && currentInfoDosen.dosen) {
            
            // A. Pindahkan data dosen lama ke Node History Rekapitulasi
            const historyRef = database.ref('/History/J101').push();
            historyRef.set({
                dosen: currentInfoDosen.dosen,
                matkul: currentInfoDosen.matkul,
                jam: currentInfoDosen.jam,
                selesaiPada: new Date().toLocaleString('id-ID')
            });

            UI.appendLog(`AUTO-ARCHIVE: Data perkuliahan ${currentInfoDosen.dosen} dipindahkan ke History.`, 'info');

            // B. Reset Node Active Info agar bersih kembali untuk dosen berikutnya
            database.ref('/Ruangan/J101/info').remove();
            currentInfoDosen = null;
        }

        UI.renderStatus(currentStatusSensor, currentInfoDosen);
    });
});