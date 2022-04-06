#include <OneWire.h> //library OneWire
#include <DallasTemperature.h> //library DS18B20
#include <Adafruit_BMP085.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
// pin data DS18B0 konek ke pin 9 Arduino UNO
#define ONE_WIRE_BUS 9
#define utara 10
#define tl 3
#define timur 4
#define tenggara 5
#define selatan 6
#define bd 7
#define barat 8
#define bl 9

//koneksi pin LCD 16x2 ke Arduino UNO
//(RS,E,D4,D5,D6,D7)
LiquidCrystal_I2C lcd(0x27, 20, 4);
static const int RXPin = 12, TXPin = 13;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
Adafruit_BMP085 bmp;
// Setup komunikasi one wire
OneWire oneWire(ONE_WIRE_BUS);  

// mereferensikan sensor ke library one wire
DallasTemperature sensors(&oneWire);
float Celsius = 0;

volatile byte half_revolutions; //variabel tipe data byte
unsigned int rpmku; //variabel tipe data integer
unsigned long timeold; //variabel tipe data long

int kalibrasi; //variabel tipe data integer
void setup(void)
{
  if (!bmp.begin()) {
  lcd.print("ERROR, check wiring!");
  while (1) {}
  }
  lcd.backlight();
  lcd.clear();
  lcd.begin(16, 2);//jenis LCD 16x2
  lcd.setCursor(5,1);
  lcd.print("MATHERION");//tampilan baris 1
  lcd.setCursor(2,2); //set ke baris 2 
  lcd.print("BY : HANDOKO 2.0");//tampilan baris 2  
  sensors.begin();  // Start sensor
  delay(4000);//tunggu 2 detik
  lcd.clear();//hapus tampilan
  lcd.print("Sensor DS18B20");//tampilan baris 1
  lcd.setCursor(0,1);//set ke baris 2
  lcd.print("Temp=");//tampilan baris 2
  lcd.backlight();
   attachInterrupt(0, rpm_fun, RISING); //mengambil sinyal high pada pin 2
   half_revolutions = 0; //memberikan nilai 0 pada viariabel
   rpmku = 0;
   timeold = 0;
   kalibrasi = 0;

  Serial.begin(9600);
  
  lcd.clear();
  lcd.noCursor();
  lcd.init();
  lcd.init();

  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  //Serial.begin(115200);
  ss.begin(GPSBaud);
  pinMode(utara,INPUT_PULLUP);
  pinMode(timur,INPUT_PULLUP);
  pinMode(selatan,INPUT_PULLUP);
  pinMode(barat,INPUT_PULLUP);
}
void loop(void)
{ 
  
  rpmku = 30*1000/(millis() - timeold)*half_revolutions; //mengaktifkan counter millis
  timeold = millis(); //hasil counter dimasukkan ke variabel timeold
  half_revolutions = 0; //reset variabel

  kalibrasi = (rpmku - 150)/109;  //rumus kalibrasi
 
  if((kalibrasi > 590)&&(kalibrasi < 605)){
    kalibrasi = 0;
  }
  
  lcd.setCursor(0, 0);
  lcd.print("Angin=");
  lcd.print(kalibrasi);
  lcd.print(" m/s ");

  if(kalibrasi>11){
   
    lcd.setCursor(15,0);
    lcd.print("Buruk");
  }
  if(kalibrasi<11){
    lcd.setCursor(14,0);
    lcd.print("Normal");
  }
  {
   lcd.setCursor(0, 2);
    lcd.setCursor(0, 2);
    lcd.print("P: ");
    lcd.print(bmp.readPressure()/100);
    lcd.print(" hPa");
    if(bmp.readPressure()>1008){
   
    lcd.setCursor(14,2);
    lcd.print("Normal");
  }
    if(bmp.readPressure()<1008){
   
    lcd.setCursor(15,2);
    lcd.print("Buruk");
  }
    //read altitude
    lcd.setCursor(0,1);
    lcd.print("T: ");
    
  //kirim perintah ke sensor
  sensors.requestTemperatures(); 
  Celsius = sensors.getTempCByIndex(0);
  lcd.setCursor(3,1);//set ke baris 2 kolom 4
  //ambil data suhu dan tampilkan hasilnya ke LCD
  lcd.print(Celsius); 
  lcd.print(" C ");//Celcius  
  if(Celsius>25){
   
    lcd.setCursor(14,1);
    lcd.print("Normal");
  }
  if(Celsius<25){
    lcd.setCursor(15,1);
    lcd.print("Buruk");
  }
  }

  {
    if (digitalRead(timur) == LOW){
  lcd.setCursor(12,0);
   lcd.print("S");

}
if (digitalRead(tenggara) == LOW){
  lcd.setCursor(12,0);
  lcd.print("T");
}
if (digitalRead(bd) == LOW){
  lcd.setCursor(12,0);
  lcd.print("U");
}
if (digitalRead(utara) == LOW){
  lcd.setCursor(12,0);
  lcd.print("B");
  
} 

  }
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    //lcd.setCursor(0,0);
    //lcd.print(F("No GPS detected"));
    while(true);
  }
 delay(100);
}

void rpm_fun(){
   half_revolutions++; //counter interupt
}
void displayInfo()
{
  if (gps.location.isValid())
  {
    lcd.setCursor(0,3);
    lcd.print(gps.location.lat(), 6);
    lcd.print(F(","));
    lcd.setCursor(6,3);
    lcd.print(gps.location.lng(), 6);
  }
}
