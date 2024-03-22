const { ipcRenderer } = require('electron');

ipcRenderer.on('canvas-clear', () => {
    const canvas = document.getElementById('main_canvas');
    if (canvas) {
        const ctx = canvas.getContext('2d');
        ctx.clearRect(0,0,canvas.width,canvas.height);
    }
});

ipcRenderer.on('canvas-manipulation', (event, data) => {
    // Handle canvas manipulation based on the received data
    if (data.action === 'drawRectangle') {
        const { position, width, height } = data;
        const canvas = document.getElementById('main_canvas');
        if (canvas) {
            const ctx = canvas.getContext('2d');
            const x = position.X * canvas.width;
            const y = position.Y * canvas.height;
            if (ctx) {
                ctx.fillRect(x, y, width * canvas.width, height * canvas.height);
            }
            console.log(`Draw Rectangle: x:${x} y:${y} width:${width} height:${height}`);
        }
    }

    if (data.action === 'drawCheckmark') {
        const { position, size } = data;
        const canvas = document.getElementById('main_canvas');
        if (canvas) {
            const ctx = canvas.getContext('2d');
            const x = position.X * canvas.width;
            const y = position.Y * canvas.height;
            if (ctx) {
                ctx.beginPath();
                ctx.strokeStyle = "red"
                ctx.moveTo(x, y + size * canvas.height / 2);
                ctx.lineTo(x + size * canvas.width / 3, y + size * canvas.height);
                ctx.lineTo(x + size * canvas.width, y);
                ctx.stroke();
            }
            console.log(`Draw Checkmark: x:${x} y:${y} size:${size}`);
        }
    }
});
