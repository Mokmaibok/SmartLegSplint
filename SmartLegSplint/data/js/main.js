// URLs ของ API
const apiKey = "KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H";
const thing = "Smart_Leg_Splint";
const channel = [
    'temp_in_1',
    'temp_in_2',
    'temp_in_3',
    'temp_out_1',
    'temp_out_2',
    'temp_out_3',
    'humidity_in_1',
    'humidity_in_2',
    'humidity_in_3',
    'heater_1',
    'heater_2',
    'heater_3',
    'setpoint_1',
    'setpoint_2',
    'setpoint_3',
    'minute_1',
    'minute_2',
    'minute_3',
    'second_1',
    'second_2',
    'second_3',
    'start_heater_1',
    'start_heater_2',
    'start_heater_3'
];
const apiUrl = "https://api.anto.io/channel/get/" + apiKey + "/" + thing + "/";

// ดึงข้อมูลจาก API และแสดงผล
function fetchData(apiUrl, channel) {
    fetch(apiUrl)
        .then(response => response.json())
        .then(data => {
            const element = document.getElementById(channel);
            if (element) {
                element.textContent = data.value;
            }
        })
        .catch(error => {
            console.error(`เกิดข้อผิดพลาดในการดึงข้อมูล ${channel}:`, error);
        });
}

// อัพเดตข้อมูลทุก 1 วินาที
setInterval(() => {
    for (let i = 0; i < channel.length; i++) {
        fetchData(apiUrl + channel[i], channel[i]);
    }
}, 1000);