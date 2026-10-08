const UI = {
    statusHero: document.getElementById('statusHeroJ101'),
    statusText: document.getElementById('statusTextJ101'),
    statusSubtitle: document.getElementById('statusSubtitleJ101'),
    statusDesc: document.getElementById('statusDescJ101'),
    lastUpdateText: document.getElementById('lastUpdateJ101'),
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
        if (this.consoleLog) this.consoleLog.prepend(entry);
    },

    renderStatus(status, infoDosen = null) {
        const timeNow = new Date().toLocaleTimeString('id-ID', { hour: '2-digit', minute: '2-digit', second: '2-digit' }) + " WIB";
        if (this.lastUpdateText) this.lastUpdateText.textContent = timeNow;

        if (status === "TERISI") {
            if (this.statusHero) this.statusHero.className = "status-hero terisi animate__animated animate__pulse";
            if (this.statusText) this.statusText.textContent = "TERISI";
            if (this.statusSubtitle) this.statusSubtitle.innerHTML = '<i class="fa-solid fa-user-group me-1"></i>Perkuliahan / Aktivitas Sedang Berlangsung';
            
            if (infoDosen && infoDosen.dosen) {
                if (this.statusDesc) this.statusDesc.innerHTML = `
                    <div class="p-3 rounded-3 bg-dark bg-opacity-50 border border-danger border-opacity-25">
                        <div class="row g-1 small">
                            <div class="col-12"><i class="fa-solid fa-user-tie text-danger me-2"></i><strong>Dosen:</strong> ${infoDosen.dosen}</div>
                            <div class="col-12"><i class="fa-solid fa-book text-danger me-2"></i><strong>Matkul:</strong> ${infoDosen.matkul}</div>
                            <div class="col-12"><i class="fa-solid fa-clock text-danger me-2"></i><strong>Sesi:</strong> ${infoDosen.jam}</div>
                        </div>
                    </div>
                `;
            } else {
                if (this.statusDesc) this.statusDesc.innerHTML = `<p class="small text-warning mb-0"><i class="fa-solid fa-triangle-exclamation me-1"></i>Aktivitas terdeteksi, Form Dosen belum diisi.</p>`;
            }
            
            if (this.livePulse) this.livePulse.className = "pulse-indicator red pulse-red";
            this.playBeep(880, 200);
            this.appendLog("EVENT J.101: Sensor Membaca Aktivitas -> Status TERISI", 'terisi');

        } else if (status === "KOSONG") {
            if (this.statusHero) this.statusHero.className = "status-hero kosong animate__animated animate__fadeIn";
            if (this.statusText) this.statusText.textContent = "KOSONG";
            if (this.statusSubtitle) this.statusSubtitle.innerHTML = '<i class="fa-solid fa-shield-halved me-1"></i>Ruangan Siap Digunakan';
            
            if (infoDosen && infoDosen.dosen) {
                if (this.statusDesc) this.statusDesc.innerHTML = `
                    <p class="small text-info mb-0"><i class="fa-solid fa-info-circle me-1"></i>Form Dosen (${infoDosen.dosen}) Terisi, Menunggu Sensor...</p>
                `;
            } else {
                if (this.statusDesc) this.statusDesc.innerHTML = '<p class="small text-muted mb-0">Tidak ada aktivitas pergerakan terdeteksi di dalam ruangan.</p>';
            }
            
            if (this.livePulse) this.livePulse.className = "pulse-indicator green pulse-green";
            this.playBeep(440, 150);
            this.appendLog("EVENT J.101: Standby / Sepi -> Status KOSONG", 'kosong');
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