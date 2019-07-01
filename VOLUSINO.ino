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
 
// Deklarasikan variable untuk data
float vref = 5; //tegangan referensi
float resolusi = vref*100/1023; //konstanta peubah
float dust;
String payload;

//Dust Sensor
int dustPin = A0; // dust sensor - Wemos A0 pin
int ledPin = D2; //led pin untuk sensor
int ledKUN = 16; //gpio pin led kuning
int ledBIR = 15; //gpio pin led biru
int ledHIJ = 12; //gpio pin led hijau
int ledMER = 14; //gpio pin led merah
 
float voltsMeasured = 0; //variable tampung pengukuran volt sensor
float calcVoltage = 0; //variable tampung voltase terkalkulasi
float dustDensity = 0; //variable tampung intensitas debu

// Ini adalah alamat script (URL) yang kita pasang di web server
// Silahkan sesuaikan alamat IP dengan ip komputer anda atau alamat domain (bila di web hosting)
// '?dust=' adalah adalah nama parameter yang akan dikirimkan ke script PHP 
 
String url = "http://192.168.43.16/VOLUSINO/Simpandata.php?dust=";
 
//===============================
// SETUP
//===============================
 
void setup() {
  
 //memulai komunikasi serial
  Serial.begin(57600);
 
 //inisialisai pin mode
  pinMode(ledPin,OUTPUT);

  //RGB
  pinMode(ledKUN,OUTPUT);
  pinMode(ledHIJ,OUTPUT);
  pinMode(ledMER,OUTPUT);
  pinMode(ledBIR,OUTPUT);
  
 //memulai komunikasi serial USE
    USE_SERIAL.begin(115200);
    USE_SERIAL.setDebugOutput(false);
 
 //indikator kedip Volusino nyala
 digitalWrite(ledBIR,HIGH);
    delay(300);
    digitalWrite(ledBIR,LOW);
    delay(300);
    digitalWrite(ledBIR,HIGH);
    delay(300);
    digitalWrite(ledBIR,LOW);
    delay(300);
    
 //waiting untuk flushing
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
 
 //set kondisi led low all
    digitalWrite(ledBIR,LOW);
    digitalWrite(ledMER,LOW);
    digitalWrite(ledKUN,LOW);
    digitalWrite(ledHIJ,LOW);
    
 
 //set mode wifi modul ke slave 
 WiFi.mode(WIFI_STA);
 
 //set konfigurasi SSID & PWD Hotspot yang akan dikonek
    WiFiMulti.addAP("hmsan", "krsbi2019"); // Sesuaikan SSID dan password ini
}
 
//===============================
// LOOP
//===============================
 
void loop() {
    //Baca Sensor Debu
    digitalWrite(ledPin,LOW); // power on the LED
    delayMicroseconds(280);
 
    voltsMeasured = analogRead(dustPin); // read the sensor volt value
 
    delayMicroseconds(40);
 
    digitalWrite(ledPin,HIGH); // turn the LED off
    delayMicroseconds(9680);
 
    //mengolah hasil data sensor ke bentuk satuan intensitas debu (mg/m3)
    calcVoltage = voltsMeasured * (5 / 1024.0);
    dustDensity = 0.17 * calcVoltage - 0.1;
    
  //serial menampilkan data sensor
    Serial.println("GP2Y1010AU0F readings"); 
    Serial.print("Raw Signal Value = ");
    Serial.println(voltsMeasured); 
    Serial.print("Voltage = ");
    Serial.println(calcVoltage);
    Serial.print("Dust Density = ");
    Serial.println(dustDensity); // mg/m3
    Serial.println("");
 
 //mengatur kondisi led indikator merah (bahaya), kuning (kurang sehat), hijau (bersih)
    if(dustDensity < 30)digitalWrite(ledHIJ,HIGH);
    else if(dustDensity < 60)digitalWrite(ledKUN,HIGH);
    else if(dustDensity > 60)digitalWrite(ledMER,HIGH);
    delay(1000);
 
    //menyimpan data ke variable lain
    dust = dustDensity;
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
           //jika gagal menampilkan eror
            USE_SERIAL.printf("[HTTP] GET gagal, error: %s\n", http.errorToString(httpCode).c_str());
        }
 
        http.end();
    }
 
    delay(5000);
 
 //set kondisi  led low all
    digitalWrite(ledBIR,LOW);
    digitalWrite(ledMER,LOW);
    digitalWrite(ledKUN,LOW);
    digitalWrite(ledHIJ,LOW);
}
