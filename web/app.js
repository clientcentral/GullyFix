//
// THIS TOOL CODED BY TECNO (YIA) ---
//
let checkInterval = null;

// 1. Particle System
function createParticle(x, y) {
    const particle = document.createElement('div');
    particle.className = 'particle';
    document.body.appendChild(particle);

    particle.style.left = (x - 3) + 'px';
    particle.style.top = (y - 3) + 'px';

    const angle = Math.random() * Math.PI * 2;
    const velocity = 40 + Math.random() * 80;
    const moveX = Math.cos(angle) * velocity;
    const moveY = Math.sin(angle) * velocity;

    setTimeout(() => {
        particle.style.transform = `translate(${moveX}px, ${moveY}px) scale(0)`;
        particle.style.opacity = '0';
    }, 10);

    setTimeout(() => {
        particle.remove();
    }, 850);
}

document.addEventListener('DOMContentLoaded', () => {
    const btn = document.getElementById("launch-btn");
    const minBtn = document.getElementById('btn-min');
    const closeBtn = document.getElementById('btn-close');
    const statusElement = document.querySelector(".status");
    const dragArea = document.getElementById('drag-area'); // منطقة السحب

    // --- الحل الجديد للسحب ---
    if (dragArea) {
        dragArea.onmousedown = (e) => {
            if (e.button === 0) { // كليك شمال فقط
                window.chrome.webview.postMessage("start_drag");
            }
        };
    }
    // زر التشغيل الأساسي
    if (btn) {
        btn.onclick = (e) => {
            btn.disabled = true;
            for (let i = 0; i < 20; i++) {
                createParticle(e.clientX, e.clientY);
            }
            statusElement.innerText = "WORKING...";
            statusElement.style.color = "#3498db";
            window.chrome.webview.postMessage("start_silent_fix");

            if (!checkInterval) {
                checkInterval = setInterval(() => {
                    window.chrome.webview.postMessage("check_current_status");
                }, 1000);
            }
        };
    }

    //BUTOTTOTOTN
    if (minBtn) {
        minBtn.onclick = () => {
            console.log("Minimizing...");
            window.chrome.webview.postMessage("window_minimize");
        };
    }
    if (closeBtn) {
        closeBtn.onclick = () => {
            console.log("Closing...");
            window.chrome.webview.postMessage("window_close");
        };
    }
});

// 3. ayo c++ shit sex
window.chrome.webview.addEventListener('message', event => {
    const statusElement = document.querySelector(".status");
    const btn = document.getElementById("launch-btn");
    if (!statusElement || !btn) return;

    let data = event.data;
    if (typeof data === 'string' && data.startsWith('"') && data.endsWith('"')) {
        data = data.slice(1, -1);
    }

    if (data === "1") {
        statusElement.innerText = "INSTALLED ✔";
        statusElement.style.color = "#2ecc71";
        btn.disabled = false;
        if (checkInterval) { clearInterval(checkInterval); checkInterval = null; }
    }
    else if (data === "2") {
        statusElement.innerText = "WAITING FOR STORE...";
        statusElement.style.color = "#f1c40f";
        btn.disabled = true;
    }
    else if (data === "0") {
        statusElement.innerText = "NOT FOUND ✖";
        statusElement.style.color = "#e74c3c";
        btn.disabled = false;
        if (checkInterval) { clearInterval(checkInterval); checkInterval = null; }
    }
});