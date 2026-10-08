/**
 * UI Module & DOM Renderer
 */
const UI = {
    statusHero: document.getElementById('statusHero'),
    statusText: document.getElementById('statusText'),
    statusSubtitle: document.getElementById('statusSubtitle'),
    statusDesc: document.getElementById('statusDesc'),
    lastUpdateText: document.getElementById('lastUpdateText'),
    consoleLog: document.getElementById('consoleLog'),
    livePulse: document.getElementById('livePulse'),

    playBeep(freq = 600, duration = 150) {
        try {
            const ctx = new (window.AudioContext || window.webkitAudioContext)();
            const osc = ctx.createOscillator();
            const gain = ctx.createGain();
            osc.type = "sine";
            osc.frequency.value = freq;
            gain.gain.setValueAtTime(0.05, ctx.currentTime);
            osc.connect(gain);
            gain.connect(ctx.destination);
            osc.start();
            osc.stop(ctx.currentTime + (duration / 1000));
        } catch (e) {}
    },

    appendLog(message, type = 'info') {
        const time = new Date().toLocaleTimeString('id-ID');
        const entry = document.createElement('div');
        entry.className = 'log-entry log-animated';
        
        let colorClass = 'text-info';
        if (type === 'terisi') colorClass = 'text-danger fw-bold';
        if (type === 'kosong') colorClass = 'text-success fw-bold';

        entry.innerHTML = `<span class="text-muted">[${time}]</span> <span class="${colorClass}">${message}</span>`;
        this.consoleLog.prepend(entry);
    },

    renderStatus(status, infoDosen = null) {
        const timeNow = new Date().toLocaleTimeString('id-ID', { hour: '2-digit', minute: '2-digit', second: '2-digit' }) + " WIB";
        this.lastUpdateText.textContent = timeNow;

        // CASE 1: SENSOR MENDETEKSI GERAKAN (STATUS TERISI)
        if (status === "TERISI") {
            this.statusHero.className = "status-hero terisi animate__animated animate__pulse";
            this.statusText.textContent = "TERISI";
            this.statusSubtitle.innerHTML = '<i class="fa-solid fa-user-group me-2"></i>Perkuliahan / Aktivitas Sedang Berlangsung';
            
            // Cek apakah Dosen sudah pernah submit form
            if (infoDosen && infoDosen.dosen) {
                this.statusDesc.innerHTML = `
                    <div class="mt-3 p-3 rounded-4 bg-dark bg-opacity-50 border border-danger border-opacity-25 text-start">
                        <div class="row g-2">
                            <div class="col-12"><i class="fa-solid fa-user-tie text-danger me-2"></i><strong>Dosen Pengampu:</strong> ${infoDosen.dosen}</div>
                            <div class="col-12"><i class="fa-solid fa-book text-danger me-2"></i><strong>Mata Kuliah:</strong> ${infoDosen.matkul}</div>
                            <div class="col-12"><i class="fa-solid fa-clock text-danger me-2"></i><strong>Estimasi Jam:</strong> ${infoDosen.jam}</div>
                        </div>
                    </div>
                `;
            } else {
                this.statusDesc.innerHTML = `<p class="mt-2 text-warning"><i class="fa-solid fa-triangle-exclamation me-2"></i>Aktivitas terdeteksi oleh sensor, namun Form Presensi Dosen belum diisi.</p>`;
            }
            
            this.livePulse.className = "pulse-indicator red pulse-red";
            this.playBeep(880, 200);
            this.appendLog("EVENT: Sensor Membaca Aktivitas -> Status TERISI", 'terisi');

        } 
        // CASE 2: TIDAK ADA GERAKAN (STATUS KOSONG)
        else if (status === "KOSONG") {
            this.statusHero.className = "status-hero kosong animate__animated animate__fadeIn";
            this.statusText.textContent = "KOSONG";
            this.statusSubtitle.innerHTML = '<i class="fa-solid fa-shield-halved me-2"></i>Ruangan Siap Digunakan / Standby';
            
            if (infoDosen && infoDosen.dosen) {
                this.statusDesc.innerHTML = `
                    <p class="mt-2 text-info"><i class="fa-solid fa-info-circle me-2"></i>Form Dosen (${infoDosen.dosen}) Sudah Diisi, Menunggu Sensor Membaca Pergerakan di Ruangan...</p>
                `;
            } else {
                this.statusDesc.innerHTML = '<p class="mt-2 text-muted">Tidak ada aktivitas pergerakan terdeteksi di dalam ruangan.</p>';
            }
            
            this.livePulse.className = "pulse-indicator green pulse-green";
            this.playBeep(440, 150);
            this.appendLog("EVENT: Tidak Ada Gerakan / Hold Timer Habis -> Status KOSONG", 'kosong');
        }
    },

    startLiveClock() {
        const clockElement = document.getElementById('digitalClock');
        if (clockElement) {
            setInterval(() => {
                const now = new Date();
                clockElement.textContent = now.toLocaleTimeString('id-ID', { hour: '2-digit', minute: '2-digit', second: '2-digit' }) + " WIB";
            }, 1000);
        }
    }
};

document.addEventListener("DOMContentLoaded", () => {
    UI.startLiveClock();
});