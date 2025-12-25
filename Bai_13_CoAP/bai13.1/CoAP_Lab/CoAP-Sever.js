const coap = require('coap');
const server = coap.createServer();

server.on('request', (req, res) => {
    console.log(`[${new Date().toLocaleTimeString()}] Nhận tin: ${req.payload.toString()}`);
    console.log(`Loại: ${req._packet.confirmable ? 'CON (Cần xác nhận)' : 'NON (Không cần xác nhận)'}`);
    console.log(`Message ID: ${req.messageId}`); // Quan trọng để soi Message ID

    /* --- TẠO TÌNH HUỐNG LỖI (BƯỚC 4 TRONG ĐỀ) --- */
    // Để test bản tin CON gửi lại (Retransmission), hãy comment dòng dưới lại.
    // Khi đó Server nhận được tin nhưng KHÔNG gửi ACK.
    
    // res.end('OK'); 
});

server.listen(() => {
    console.log('CoAP Server đang chạy (Port 5683)...');
});