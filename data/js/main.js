// URLs ของ API
const api_temp_in_1 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/temp_in_1';
const api_temp_in_2 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/temp_in_2';
const api_temp_in_3 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/temp_in_3';
const api_temp_out = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/temp_out';
const api_humidity_in_1 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/humidity_in_1';
const api_humidity_in_2 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/humidity_in_2';
const api_humidity_in_3 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/humidity_in_3';
const api_heater_1 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/heater_1';
const api_heater_2 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/heater_2';
const api_heater_3 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/heater_3';
const api_setpoint_1 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/setpoint_1';
const api_setpoint_2 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/setpoint_2';
const api_setpoint_3 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/setpoint_3';
const api_minute_1 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/minute_1';
const api_minute_2 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/minute_2';
const api_minute_3 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/minute_3';
const api_second_1 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/second_1';
const api_second_2 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/second_2';
const api_second_3 = 'https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/second_3';

// ดึงข้อมูลจาก API และแสดงผล
function fetchData(apiUrl, elementId) {
    fetch(apiUrl)
        .then(response => response.json())
        .then(data => {
            document.getElementById(elementId).textContent = `${data.value}`;
        })
        .catch(error => {
            console.error(`เกิดข้อผิดพลาดในการดึงข้อมูล ${elementId}:`, error);
        });
}

// อัพเดตข้อมูลทุก 1 วินาที
setInterval(() => {
    fetchData(api_temp_in_1, 'temp_in_1');
    fetchData(api_temp_in_2, 'temp_in_2');
    fetchData(api_temp_in_3, 'temp_in_3');
    fetchData(api_temp_out, 'temp_out_1');
    fetchData(api_temp_out, 'temp_out_2');
    fetchData(api_temp_out, 'temp_out_3');
    fetchData(api_humidity_in_1, 'humidity_in_1');
    fetchData(api_humidity_in_2, 'humidity_in_2');
    fetchData(api_humidity_in_3, 'humidity_in_3');
    fetchData(api_heater_1, 'heater_1');
    fetchData(api_heater_2, 'heater_2');
    fetchData(api_heater_3, 'heater_3');
    fetchData(api_setpoint_1, 'setpoint_1');
    fetchData(api_setpoint_2, 'setpoint_2');
    fetchData(api_setpoint_3, 'setpoint_3');
    fetchData(api_minute_1, 'minute_1');
    fetchData(api_minute_2, 'minute_2');
    fetchData(api_minute_3, 'minute_3');
    fetchData(api_second_1, 'second_1');
    fetchData(api_second_2, 'second_2');
    fetchData(api_second_3, 'second_3');
}, 1000);