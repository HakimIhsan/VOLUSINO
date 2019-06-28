// Include library yang diperlukan
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
 
// Gunakan serial sebagai monitor
#define USE_SERIAL Serial
 
// Buat object Wifi
ESP8266WiFiMulti WiFiMulti;
 
// Buat object http
HTTPClient http;
 
// Deklarasikan variable untuk suhu
float vref = 5;
float resolusi = vref*100/1023;
float suhu;
String payload;

//Dust Sensor
int dustPin = A0; // dust sensor - Wemos A0 pin
int ledPin = D2;
int ledKUN = 16;
int ledBIR = 15;
int ledHIJ = 12;
int ledMER = 14;    
 
float voltsMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

// Ini adalah alamat script (URL) yang kita pasang di web server
// Silahkan sesuaikan alamat IP dengan ip komputer anda atau alamat domain (bila di web hosting)
// '?suhu=' adalah adalah nama parameter yang akan dikirimkan ke script PHP 
 
String url = "http://192.168.43.16/VOLUSINO/Simpandata.php?suhu=";
 
//===============================
// SETUP
//===============================
 
void setup() {
  
  Serial.begin(57600);
  pinMode(ledPin,OUTPUT);

  //RGB
  pinMode(ledKUN,OUTPUT);
  pinMode(ledHIJ,OUTPUT);
  pinMode(ledMER,OUTPUT);
  pinMode(ledBIR,OUTPUT);
  
    USE_SERIAL.begin(115200);
    USE_SERIAL.setDebugOutput(false);
    digitalWrite(ledBIR,HIGH);
    delay(300);
    digitalWrite(ledBIR,LOW);
    delay(300);
    digitalWrite(ledBIR,HIGH);
    delay(300);
    digitalWrite(ledBIR,LOW);
    delay(300);
    
    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] Tunggu %d...\n", t);
        USE_SERIAL.flush();

       digitalWrite(ledKUN,HIGH);
       delay(166);
       digitalWrite(ledKUN,LOW);
       delay(166);

//       
       digitalWrite(ledHIJ,HIGH);
       delay(166);
       digitalWrite(ledHIJ,LOW);
       delay(166);

//       
       digitalWrite(ledMER,HIGH);
       delay(166);
       digitalWrite(ledMER,LOW);
       delay(166);
    }
    digitalWrite(ledBIR,LOW);
    digitalWrite(ledMER,LOW);
    digitalWrite(ledKUN,LOW);
    digitalWrite(ledHIJ,LOW);
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("hmsan", "krsbi2019"); // Sesuaikan SSID dan password ini
}
 
//===============================
// LOOP
//===============================
 
void loop() {
    //Baca Sensor Debu
    digitalWrite(ledPin,LOW); // power on the LED
    delayMicroseconds(280);
 
    voltsMeasured = analogRead(dustPin); // read the dust value
 
    delayMicroseconds(40);
    digitalWrite(ledPin,HIGH); // turn the LED off
    delayMicroseconds(9680);
 
    //measure your 5v and change below
    calcVoltage = voltsMeasured * (5 / 1024.0);
   dustDensity = 0.17 * calcVoltage - 0.1;
   // dustDensity = (voltsMeasured-13)*100/13;
    
    Serial.println("GP2Y1010AU0F readings"); 
    Serial.print("Raw Signal Value = ");
    Serial.println(voltsMeasured); 
    Serial.print("Voltage = ");
    Serial.println(calcVoltage);
    Serial.print("Dust Density = ");
    Serial.println(dustDensity); // mg/m3
    Serial.println("");
    if(dustDensity < 30)digitalWrite(ledHIJ,HIGH);
    else if(dustDensity < 60)digitalWrite(ledKUN,HIGH);
    else if(dustDensity > 60)digitalWrite(ledMER,HIGH);
    delay(1000);
    // Baca suhu dari pin analog
    suhu = dustDensity;
    //----------------------
 
    // Cek apakah statusnya sudah terhubung
    if((WiFiMulti.run() == WL_CONNECTED)) {
  
        // Tambahkan nilai suhu pada URL yang sudah kita buat
        USE_SERIAL.print("[HTTP] Memulai...\n");
        http.begin( url + (String) suhu ); 
        
        // Mulai koneksi dengan metode GET
        USE_SERIAL.print("[HTTP] Melakukan GET ke server...\n");
        int httpCode = http.GET();
 
        // Periksa httpCode, akan bernilai negatif kalau error
        if(httpCode > 0) {
            
            // Tampilkan response http
            USE_SERIAL.printf("[HTTP] kode response GET: %d\n", httpCode);
 
            // Bila koneksi berhasil, baca data response dari server
            if(httpCode == HTTP_CODE_OK) {
                payload = http.getString();
                USE_SERIAL.println(payload);
            }
 
        } else {
 
            USE_SERIAL.printf("[HTTP] GET gagal, error: %s\n", http.errorToString(httpCode).c_str());
        }
 
        http.end();
    }
 
    delay(5000);
    digitalWrite(ledBIR,LOW);
    digitalWrite(ledMER,LOW);
    digitalWrite(ledKUN,LOW);
    digitalWrite(ledHIJ,LOW);
}
