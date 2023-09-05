#include <DHT.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <PIDController.h>
#include <AntoIO.h>

// กำหนดชนิดของเซนเซอร์ DHT11
#define DHTTYPE DHT11

// กำหนดขาของเซนเซอร์ DHT
int DHTPIN_IN[3] = {5, 4, 0}; // D1, D2, D3
int DHTPIN_OUT = 2; // D4

// กำหนดขาของเซนเซอร์ DHT
int heaterOutputPin[3] = {14, 12, 13}; //D5, D6, D7

// สร้างอินสแตนซ์ของเซนเซอร์ DHT สำหรับขาเข้าและขาออก
DHT dht_in[3] = {DHT(DHTPIN_IN[0], DHTTYPE), DHT(DHTPIN_IN[1], DHTTYPE), DHT(DHTPIN_IN[2], DHTTYPE)};
DHT dht_out = DHT(DHTPIN_OUT, DHTTYPE);

// กำหนดข้อมูล WiFi
const char *ssid = "หมก";
const char *password = "csos1478";
const char *user = "SmartLegSplint";
const char *token = "KKLdLloWwarKm1bsNgFAmwJix49eCoPbJ14ywF3H";
const char *thing = "Smart_Leg_Splint";

// สร้างอินสแตนซ์ของ AntoIO ด้วยข้อมูลผู้ใช้งานและเครื่อง Anto ที่เซ็ตไว้
AntoIO anto(user, token, thing);

// สร้างอินสแตนซ์ของ PIDController
PIDController pid[3];

// สร้างอินสแตนซ์ของ ESP8266WebServer
ESP8266WebServer server(80);

// กำหนดสถานะของเครื่องทำความร้อน ค่า PWM และอื่น ๆ
bool heaterStatus[3] = {false, false, false};
float heater[3] = {0.0, 0.0, 0.0};
float pwmOutput[3] = {0.0, 0.0, 0.0};

// กำหนดค่าคงที่ของ PID
float kp[3] = {350.0, 350.0, 200.0};
float ki[3] = {0.5, 0.5, 0.2};
float kd[3] = {5.0, 5.0, 10.0};

// กำหนดค่าเป้าหมายสำหรับแต่ละเครื่องทำความร้อน
float setpoints[3] = {37.0, 37.0, 37.0};

// กำหนดเวลาตั้งต้นสำหรับการนับถอยหลัง
bool checkSetpoints[3] = {false, false, false};
unsigned long previousMillis_loop = 0;
unsigned long previousMillis[3] = {0, 0, 0};
int minute[3] = {0, 0, 0};
int second[3] = {0, 0, 0};

void setup() {
  Serial.begin(115200);
  
  pinMode(heaterOutputPin[0], OUTPUT);
  pinMode(heaterOutputPin[1], OUTPUT);
  pinMode(heaterOutputPin[2], OUTPUT);
  
  analogWrite(heaterOutputPin[0], LOW);
  analogWrite(heaterOutputPin[1], LOW);
  analogWrite(heaterOutputPin[2], LOW);
  
  for (int i = 0; i < 3; i++) {
    // เริ่มต้นเซนเซอร์ DHT และ PID controller
    dht_in[i].begin();
    dht_out.begin();
    pid[i].begin();
    pid[i].setpoint(0);
    pid[i].tune(kp[i], ki[i], kd[i]);
    pid[i].limit(0, 255);
  }
  /*
  // ตั้งค่าค่า IP address, Gateway, Subnet Mask, และ DNS server ของ ESP8266
  IPAddress ip(192, 168, 1, 100);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(8, 8, 8, 8);
  
  // กำหนดค่า IP address และการกำหนดค่าเครือข่ายของ ESP8266
  WiFi.config(ip, gateway, subnet, dns);
  */
  // ตั้งค่าการเชื่อมต่อ Wi-Fi และ Anto
  setupWiFiandAnto();
  
  // เริ่มต้น SPIFFS, และเซิร์ฟเวอร์
  LittleFS.begin();
  server.begin();
  
  // ตั้งค่า Route สำหรับ HTTP requests
  setupRoutes();
}

void loop() {
  // อัพเดตสถานะของเว็บเซิร์ฟเวอร์
  server.handleClient();
  
  // อัพเดตค่าจาก DHT เซ็นเซอร์และส่งข้อมูลไปยัง Anto
  updateDHTDataAndSendToAnto();
  
  //ใช้ฟังก์ชัน `loop()` ของ Anto MQTT ในการรับข้อมูลจาก Anto
  anto.mqtt.loop();
  
  // ตรวจสอบและควบคุมการทำงานของเครื่องทำความร้อน
  controlHeaters();
}

// ฟังก์ชันในการตั้งค่าเป็น Station (เชื่อมต่อเครือข่าย Wi-Fi)
void setupWiFiandAnto() {
  Serial.print("Anto library version: ");
  Serial.println(anto.getVersion());

  Serial.print("Trying to connect to ");
  Serial.println(ssid);

  anto.begin(ssid, password, messageReceived);

  Serial.println("Connected to Anto");

  anto.sub("start_heater_1");
  anto.sub("start_heater_2");
  anto.sub("start_heater_3");
  
}

// ฟังก์ชันในการรับข้อมูลที่ถูกส่งมาจาก Anto MQTT
void messageReceived(String thing, String channel, String payload) {
  Serial.print("Received: ");
  Serial.print(thing);
  Serial.print("/");
  Serial.print(channel);
  Serial.print("-> ");
  Serial.println(payload);

  // ตรวจสอบว่ารับข้อมูลจากช่องที่เราสนใจหรือไม่
  if (thing == "Smart_Leg_Splint") {
    if (channel == "start_heater_1") {
      // ถ้ารับค่า "1" ให้เปิดเครื่องทำความร้อนที่ตำแหน่ง 1
      if (payload == "1") {
        heaterStatus[0] = true;
      } else {
        heaterStatus[0] = false;
      }
    } else if (channel == "start_heater_2") {
      // ถ้ารับค่า "1" ให้เปิดเครื่องทำความร้อนที่ตำแหน่ง 2
      if (payload == "1") {
        heaterStatus[1] = true;
      } else {
        heaterStatus[1] = false;
      }
    } else if (channel == "start_heater_3") {
      // ถ้ารับค่า "1" ให้เปิดเครื่องทำความร้อนที่ตำแหน่ง 3
      if (payload == "1") {
        heaterStatus[2] = true;
      } else {
        heaterStatus[2] = false;
      }
    }
  }
}

// ฟังก์ชันอัพเดตค่าจาก DHT เซ็นเซอร์และส่งข้อมูลไปยัง Anto
void updateDHTDataAndSendToAnto() {
  // อ่านค่าอุณหภูมิและความชื้นจากเซ็นเซอร์ DHT11
  float temp_in[3];
  float humid_in[3];
  float temp_out;
  
  temp_in[0] = dht_in[0].readTemperature();
  temp_in[1] = dht_in[1].readTemperature();
  temp_in[2] = dht_in[2].readTemperature();
  
  humid_in[0] = dht_in[0].readHumidity();
  humid_in[1] = dht_in[1].readHumidity();
  humid_in[2] = dht_in[2].readHumidity();
  
  temp_out = dht_out.readTemperature();
  
  // ส่งค่าอุณหภูมิและความชื้นไปยัง Anto MQTT
  anto.pub("temp_in_1", temp_in[0]);
  anto.pub("temp_in_2", temp_in[1]);
  anto.pub("temp_in_3", temp_in[2]);
  
  anto.pub("humidity_in_1", humid_in[0]);
  anto.pub("humidity_in_2", humid_in[1]);
  anto.pub("humidity_in_3", humid_in[2]);
  
  anto.pub("temp_out_1", temp_out);
  anto.pub("temp_out_2", temp_out);
  anto.pub("temp_out_3", temp_out);
  
  // ส่งค่าเปอร์เซ็นต์ของเครื่องทำความร้อนไปยัง Anto MQTT
  anto.pub("heater_1", heater[0]);
  anto.pub("heater_2", heater[1]);
  anto.pub("heater_3", heater[2]);
  
  // ส่งค่า Setpoint ไปยัง Anto MQTT
  anto.pub("setpoint_1", setpoints[0]);
  anto.pub("setpoint_2", setpoints[1]);
  anto.pub("setpoint_3", setpoints[2]);
  
  // ส่งค่านาทีและวินาทีไปยัง Anto MQTT
  anto.pub("minute_1", minute[0]);
  anto.pub("minute_2", minute[1]);
  anto.pub("minute_3", minute[2]);
  
  anto.pub("second_1", second[0]);
  anto.pub("second_2", second[1]);
  anto.pub("second_3", second[2]);

  anto.pub("timer_1", minute[0]);
  anto.pub("timer_2", minute[1]);
  anto.pub("timer_3", minute[2]);
}

// ฟังก์ชันในการควบคุมการทำงานของเครื่องทำความร้อน
void controlHeaters() {
  for (int i = 0; i < 3; i++) {
    // อ่านอุณหภูมิจากเซนเซอร์ DHT เข้ามา
    float temp_in = dht_in[i].readTemperature();
    // กำหนดค่าเป้าหมายสำหรับ PID
    pid[i].setpoint(setpoints[i]);
    // ควบคุมการผลิตความร้อน
    if (heaterStatus[i]) {
      pwmOutput[i] = pid[i].compute(temp_in);
      analogWrite(heaterOutputPin[i], pwmOutput[i]);
      // ตรวจสอบเงื่อนไขเวลา
      if (temp_in >= setpoints[i]) {
        checkSetpoints[i] = true;
      }
      if (checkSetpoints[i]) {
        handleTimer(i);
      }
    } else {
      pwmOutput[i] = 0;
      analogWrite(heaterOutputPin[i], pwmOutput[i]);
      checkSetpoints[i] = false;
    }
    // คำนวณเปอร์เซ็นต์ของเครื่องทำความร้อน
    heater[i] = (pwmOutput[i] / 255) * 100;
  }
}

// ฟังก์ชันในการนับเวลาถอยหลังสำหรับแต่ละเครื่องทำความร้อน
void handleTimer(int heaterIndex) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis[heaterIndex] >= 1000) {
    if (second[heaterIndex] > 0) {
      second[heaterIndex]--;
    } else {
      if (minute[heaterIndex] > 0) {
        minute[heaterIndex]--;
        second[heaterIndex] = 59;
      } else {
        minute[heaterIndex] = 0;
        second[heaterIndex] = 0;
      }
    }
    previousMillis[heaterIndex] = currentMillis;
  }
}

// ฟังก์ชันในการตั้งค่า Route
void setupRoutes() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/index.html", HTTP_GET, handleRoot);
  server.on("/about.html", HTTP_GET, []() {
    handleFile("/about.html");
  });
  server.on("/setting_shin.html", HTTP_GET, []() {
    handleFile("/setting_shin.html");
  });
  server.on("/setting_sole.html", HTTP_GET, []() {
    handleFile("/setting_sole.html");
  });
  server.on("/setting_instep.html", HTTP_GET, []() {
    handleFile("/setting_instep.html");
  });
  server.on("/css/bootstrap.min.css", HTTP_GET, []() {
    handleFile("/css/bootstrap.min.css");
  });
  server.on("/js/bootstrap.bundle.min.js", HTTP_GET, []() {
    handleFile("/js/bootstrap.bundle.min.js");
  });
  server.on("/js/bootstrap.min.js", HTTP_GET, []() {
    handleFile("/js/bootstrap.min.js");
  });
  server.on("/js/chart.min.js", HTTP_GET, []() {
    handleFile("/js/chart.min.js");
  });
  server.on("/js/jquery.min.js", HTTP_GET, []() {
    handleFile("/js/jquery.min.js");
  });
}

// ฟังก์ชันในการจัดการหน้า Root
void handleRoot() {
  handleFile("/index.html");
}

// ฟังก์ชันในการให้ Content Type ของไฟล์
String getContentType(const String &fileName) {
  if (fileName.endsWith(".html")) return "text/html";
  if (fileName.endsWith(".css")) return "text/css";
  if (fileName.endsWith(".js")) return "application/javascript";
  if (fileName.endsWith(".jpg") || fileName.endsWith(".jpeg")) return "image/jpeg";
  if (fileName.endsWith(".png")) return "image/png";
  if (fileName.endsWith(".gif")) return "image/gif";
  if (fileName.endsWith(".ico")) return "image/x-icon";
  if (fileName.endsWith(".xml")) return "text/xml";
  if (fileName.endsWith(".pdf")) return "application/pdf";
  if (fileName.endsWith(".zip")) return "application/zip";
  return "text/plain";
}

// ฟังก์ชันในการจัดเก็บไฟล์ด้วย SPIFFS
void handleFile(const String& fileName) {
  File file = LittleFS.open(fileName, "r");
  if (!file) {
    server.send(404, "text/plain", "File not found");
    return;
  }
  server.streamFile(file, getContentType(fileName));
  file.close();
}