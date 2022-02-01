#include <Wire.h> 

#include "Adafruit_CCS811.h"
#include <SoftwareSerial.h>
SoftwareSerial uno(7,8); // Rx, Tx bersilangan


#define mq7 A1
#define mq135 A0
#define RL1 10 //nilai RL =10 pada sensor
#define m -0.417 //hasil perhitungan gradien
#define b 0.425 //hasil perhitungan perpotongan
#define RO 18 //hasil pengukuran RO 19


Adafruit_CCS811 ccs;
String kirim;
float data;
float data1;
int data2;
int data3;
float kosong=0;
//mq135
const int numReadings = 5;//nilai penambilan sample pembacaan sebesar 5 kali
float readings[numReadings];      
int readIndex = 0;              
float total = 0;                  
float average = 0;

float CO;
int val;
int co = HIGH;
String a;


void setup() {
// put your setup code here, to run once:

  Serial.begin(9600);
  uno.begin(9600);
  pinMode(mq7, INPUT);
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
     readings[thisReading] = 0;
  }
  Serial.println("CCS811 test");
   
  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }

  // Wait for the sensor to be ready
  while(!ccs.available());
}
//mq7
long RL = 1000; // 1000 Ohm
long Ro = 830; // 830 ohm ( SILAHKAN DISESUAIKAN)

void loop() {
 MQ7();
 MQ135();
 cs811();
 kirimMcu();
 data=average; // tipe data yg dioutputkan float
 data1=CO;
 //data2=val;
 data3=ccs.geteCO2();
}
void MQ7(){
 int sensorvalue = analogRead(mq7); // membaca nilai ADC dari sensor
 float VRL= sensorvalue*5.00/1024;  // mengubah nilai ADC ( 0 - 1023 ) menjadi nilai voltase ( 0 - 5.00 volt )
 float Rs = ( 5.00 * RL / VRL ) - RL;
 CO = 100 * pow(Rs / Ro,-1.53); // ppm = 100 * ((rs/ro)^-1.53);

  co = digitalRead(mq7);
  if (co== LOW)
    {
      a = "TRUE";
    }
  else if (co == HIGH)
    {
      a = "FALSE";
    }
  Serial.println();  
  Serial.print("CO : ");
  Serial.print(CO);
  Serial.println(" ppm");  
  delay(1000);
}

void MQ135() {

  float VRL; 
  float RS;  
  float ratio; 
   
  VRL = analogRead(mq135)*(5/1023.0); //konversi analog ke tegangan
   RS =(5.0 /VRL-1)*10 ;//rumus untuk RS
  ratio = RS/RO;  // rumus mencari ratio
  float ppm = pow(10, ((log10(ratio)-b)/m));//rumus mencari ppm
  
 total = total - readings[readIndex];

  readings[readIndex] = ppm;

  total = total + readings[readIndex];

  readIndex = readIndex + 1;


  if (readIndex >= numReadings) {

    readIndex = 0;
  }

  average = total / numReadings;
  Serial.print("NH3= ");
 //menampilkan nilai rata-rata ppm setelah 5 kali pembacaan
 Serial.println(average);
 //Serial.print(" PPM");
}
void cs811() {
  if(ccs.available()){
    if(!ccs.readData()){
      Serial.print("CO2: ");
      Serial.print(ccs.geteCO2());
      Serial.print("ppm");
      //Serial.println(ccs.getTVOC());
    }
    else{
      Serial.println("ERROR!");
      while(1);
    }
  }
  delay(500);
}
void kirimMcu() {
  // put your main code here, to run repeatedly:
  kirim= String(kosong) + "#" + String(data) + "#" + String(data1) + "#" + String(data2) + "#" + String(data3);
uno.println(kirim);  
}
