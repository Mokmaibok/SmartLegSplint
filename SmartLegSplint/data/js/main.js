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
    'timer_1',
    'timer_2',
    'timer_3'
];
const apiUrl_get = "https://api.anto.io/channel/get/" + apiKey + "/" + thing + "/";
const apiUrl_set = "https://api.anto.io/channel/set/" + apiKey + "/" + thing + "/";

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
        fetchData(apiUrl_get + channel[i], channel[i]);
    }
}, 1000);

// ส่งคำสั่งเปิด/ปิดเครื่องทำความร้อน
function sendHeaterCommand(heaterNumber, command) {
    const url = `${apiUrl_set}start_heater_${heaterNumber}/${command}`;
    fetch(url)
        .then(response => response.json())
        .then(message => console.log(message))
        .catch(error => console.error('Error:', error));
}

// ตัวอย่างการเรียกใช้ฟังก์ชันสำหรับเครื่องทำความร้อนที่ 1
function startHeater_1() {
    sendHeaterCommand(1, "1");
}

function stopHeater_1() {
    sendHeaterCommand(1, "0");
}

// ตัวอย่างการเรียกใช้ฟังก์ชันสำหรับเครื่องทำความร้อนที่ 2
function startHeater_2() {
    sendHeaterCommand(2, "1");
}

function stopHeater_2() {
    sendHeaterCommand(2, "0");
}

// ตัวอย่างการเรียกใช้ฟังก์ชันสำหรับเครื่องทำความร้อนที่ 3
function startHeater_3() {
    sendHeaterCommand(3, "1");
}

function stopHeater_3() {
    sendHeaterCommand(3, "0");
}

fetch('https://api.anto.io/channel/get/KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H/Smart_Leg_Splint/start_heater_1')
    .then(response => response.json())
    .then(data => console.log(data))
    .catch(error => console.error('Error:', error));