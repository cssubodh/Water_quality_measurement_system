//* Code by Subodh ****
//*
//*************************************"**********************//
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x20,16,2);
#include <TinyGPS.h>
#include <SoftwareSerial.h>
SoftwareSerial SIM900(7, 8);

TinyGPS gps;  //Creates a new instance of the TinyGPS object
#define DS18B20_PIN   10
int raw_temp;
float temp;
char txt[] = " C ";
void setup()
{
  Serial.begin(9600);
  SIM900.begin(9600);  
   lcd.begin(16,2);
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  txt[0] = 223;
}

void loop()
{
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      //Serial.print(c);
      if (gps.encode(c)) 
        newData = true;  
    }
  }

  if (newData)      //If newData is true
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);   
    SIM900.print("AT+CMGF=1\r"); 
    delay(400);
    SIM900.println("AT + CMGS = \"+91xxxxxxxxxx\"");// recipient's mobile number with country code
    delay(300);
    SIM900.print("Latitude = ");
    SIM900.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    SIM900.print(" Longitude = ");
    SIM900.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    delay(200);
    
    SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
    delay(200);
    SIM900.println();
     lcd.setCursor(0,0);
     lcd.print("Lat: ");
    lcd.print(flat,5);
    lcd.setCursor(0,1);
    lcd.print("Long: ");
    lcd.print(flon,5);
    delay(1000);
    lcd.clear();
    if(ds18b20_read(&raw_temp)) {
    Serial.print("Temperature = ");
    temp = (float)raw_temp / 16;     // Convert temperature raw value into degree Celsius (temp in °C = raw/16)
    Serial.print(temp);              // Print temperature value in degree Celsius
    Serial.println("°C");            // Print '°C'
    // Display temperature on LCD
    lcd.setCursor(2, 0);
    lcd.print("Temperature:");
    lcd.setCursor(4, 1);
    lcd.print(temp);
    lcd.print(txt);
    delay(1000);
    lcd.clear();
    para();
  }
  else {
    Serial.println("Communication Error!");
    lcd.setCursor(4, 1);
    lcd.print(" Error! ");
  }
  delay(1000);
    

  }
 
  Serial.println(failed);
 // if (chars == 0)
   // Serial.println("** No characters received from GPS: check wiring **");
}

bool ds18b20_start(){
  bool ret = 0;
  digitalWrite(DS18B20_PIN, LOW);     // Send reset pulse to the DS18B20 sensor
  pinMode(DS18B20_PIN, OUTPUT);
  delayMicroseconds(500);             // Wait 500 us
  pinMode(DS18B20_PIN, INPUT);
  delayMicroseconds(100);             //wait to read the DS18B20 sensor response
  if (!digitalRead(DS18B20_PIN)) {
    ret = 1;                          // DS18B20 sensor is present
    delayMicroseconds(400);           // Wait 400 us
  }
  return(ret);
}

void ds18b20_write_bit(bool value){
  digitalWrite(DS18B20_PIN, LOW);
  pinMode(DS18B20_PIN, OUTPUT);
  delayMicroseconds(2);
  digitalWrite(DS18B20_PIN, value);
  delayMicroseconds(80);
  pinMode(DS18B20_PIN, INPUT);
  delayMicroseconds(2);
}

void ds18b20_write_byte(byte value){
  byte i;
  for(i = 0; i < 8; i++)
    ds18b20_write_bit(bitRead(value, i));
}

bool ds18b20_read_bit(void) {
  bool value;
  digitalWrite(DS18B20_PIN, LOW);
  pinMode(DS18B20_PIN, OUTPUT);
  delayMicroseconds(2);
  pinMode(DS18B20_PIN, INPUT);
  delayMicroseconds(5);
  value = digitalRead(DS18B20_PIN);
  delayMicroseconds(100);
  return value;
}

byte ds18b20_read_byte(void) {
  byte i, value;
  for(i = 0; i  <8; i++)
    bitWrite(value, i, ds18b20_read_bit());
  return value;
}

bool ds18b20_read(int *raw_temp_value) {
  if (!ds18b20_start())                     // Send start pulse
    return(0);                              // Return 0 if error
  ds18b20_write_byte(0xCC);                 // Send skip ROM command
  ds18b20_write_byte(0x44);                 // Send start conversion command
  while(ds18b20_read_byte() == 0);          // Wait for conversion complete
  if (!ds18b20_start())                     // Send start pulse
    return(0);                              // Return 0 if error
  ds18b20_write_byte(0xCC);                 // Send skip ROM command
  ds18b20_write_byte(0xBE);                 // Send read command
  *raw_temp_value = ds18b20_read_byte();    // Read temperature LSB byte and store it on raw_temp_value LSB byte
  *raw_temp_value |= (unsigned int)(ds18b20_read_byte() << 8);     // Read temperature MSB byte and store it on raw_temp_value MSB byte
  return(1);                                // OK --> return 1
}


void para(){
    int phvalue=analogRead(A0);
  float voltage = phvalue *(14.0 / 1023.0);
    lcd.setCursor(6, 0);
    lcd.print("Ph:");
    lcd.setCursor(4, 1);
    lcd.print(voltage);
    delay(1000);
    lcd.clear();
    int turbidity =analogRead(A1);
    float voltage1=turbidity * (6.0/1023.0);
    lcd.setCursor(2, 0);
    lcd.print("Turbidity:");
    lcd.setCursor(6, 1);
    lcd.print(voltage1);
    delay(1000);
    lcd.clear();
    int conductivity =analogRead(A2);
    float voltage2 = conductivity/1024.0*500;
    
    lcd.setCursor(2, 0);
    lcd.print("Conductivity:");
    lcd.setCursor(6, 1);
    lcd.print(voltage2);
    delay(1000);
    lcd.clear();
    
  
}
