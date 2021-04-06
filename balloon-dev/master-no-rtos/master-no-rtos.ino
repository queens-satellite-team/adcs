#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_MPU6050.h>
#include <SparkFun_I2C_GPS_Arduino_Library.h>
#include <TinyGPS++.h>

//Booleans controlling what is being read
int GPS_get_time = 1;
int GPS_get_coords = 1;
int BNO_get_orient = 1;
int BNO_get_gyro = 1;
int BNO_get_acc = 1;
int BNO_get_mag = 1;
int BNO_get_temp = 1;
int MPU_get_gyro = 1;
int MPU_get_acc = 1;
int MPU_get_temp = 1;

//controlling read rate
int x_time = 1000;

//setup GPS,BNO055,MPU6050
TinyGPSPlus gps;
Adafruit_BNO055 bno = Adafruit_BNO055(55,0x28);
Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_temp, *mpu_accel, *mpu_gyro;

//setup SD writing
const int chipSelect = 53;
File sensorData;
String buffer;
unsigned long lastMillis = 0;

//write buffer to SD
void writeData(){
  //write any available data to SD
  unsigned int chunkSize = sensorData.availableForWrite();
  Serial.print(chunkSize);
  Serial.print("\t");
  Serial.print(buffer.length());
  Serial.print("\n");
//  Serial.print(buffer.c_str());
//  Serial.print("\n");
  if (chunkSize && buffer.length() >= chunkSize) {
    //write to file and blink LED
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.print(buffer.c_str());
    Serial.print("\n");
    sensorData.write(buffer.c_str(), chunkSize);
    digitalWrite(LED_BUILTIN, LOW);
    //remove written data from buffer
    buffer.remove(0, chunkSize);
    sensorData.close();
    sensorData = SD.open("data.csv",FILE_WRITE);
  }
}

//Serial communication to OBC
int relayData(String data){
  Serial1.write(data.c_str());
  Serial.write(data.c_str());
}

//GPS data read and bundling
String readGPS(){
  String data = "";
  String time_data = ",,";
  String coords_data = ",";
  String alt_data = "";
  //read gps
  while(myI2CGPS.available()){
    gps.encode(myI2CGPS.read());
  }
  if(GPS_get_time == 1){
    time_data = String(gps.time.hour())+","+String(gps.time.minute())+","+String(gps.time.second());
  }
  if(GPS_get_coords == 1){
    coords_data = String(gps.location.lat(),6)+","+String(gps.location.lng(),6);
  }
  if(GPS_get_alt == 1){
    alt_data = String(gps.altitude.meters());
  }
  //return bundled data
  data = time_data+","+coords_data+","+alt_data;
//  Serial.print(data);
//  Serial.print("\n");
  return data;
}

//MPU6050 data read and bundling
String readMPU(){
  String data = "";
  String gyro_data = ",,";
  String acc_data = ",,";
  String temp_data = "";
  //read mpu6050
  if(MPU_get_gyro == 1){
    sensors_event_t gyro;
    mpu_gyro->getEvent(&gyro);
    gyro_data = String(gyro.gyro.x)+","+String(gyro.gyro.y)+","+String(gyro.gyro.x);
  }
  if(MPU_get_acc == 1){
    sensors_event_t accel;
    mpu_accel->getEvent(&accel);
    acc_data = String(accel.acceleration.x)+","+String(accel.acceleration.y)+","+String(accel.acceleration.z);
  }
  if(MPU_get_temp == 1){
    sensors_event_t temp;
    mpu_temp->getEvent(&temp);
    temp_data = String(temp.temperature);
  }
  //return bundled data
  data = gyro_data+","+acc_data+","+temp_data;
//  Serial.print(data);
//  Serial.print("\n");
  return data;
}

//BNO055 data read and bundling
String readBNO(){
//  Serial.print("reading BNO");
//  Serial.print("\n");
  String data = "";
  String orient_data = ",,";
  String gyro_data = ",,";
  String acc_data = ",,";
  String mag_data = ",,";
  String temp_data = "";
  //read bno055
  sensors_event_t event;
  bno.getEvent(&event);
  if(BNO_get_orient == 1){
    orient_data = String(event.orientation.x)+","+String(event.orientation.y)+","+String(event.orientation.x);
//    Serial.print("BNO Orient: \t");
//    Serial.print(event.orientation.x);
//    Serial.print("\n");
//    Serial.print(orient_data);
//    Serial.print("\n");
  }
  if(BNO_get_gyro == 1){
    gyro_data = String(event.gyro.x)+","+String(event.gyro.y)+","+String(event.gyro.x);
  }
  if(BNO_get_acc == 1){
    acc_data = String(event.acceleration.x)+","+String(event.acceleration.y)+","+String(event.acceleration.z);
  }
  if(BNO_get_mag == 1){
    mag_data = String(event.magnetic.x)+","+String(event.magnetic.y)+","+String(event.magnetic.z);
  }
  if(BNO_get_temp == 1){
    temp_data = String(bno.getTemp());
  }
//  Serial.print(orient_data);
//  Serial.print("\n");
  data = orient_data+","+gyro_data+","+acc_data+","+mag_data+","+temp_data;
//  Serial.print(data);
//  Serial.print("\n");
  return data;
}

void setup() {
Serial.begin(115200);
//Serial1.begin(115200);
Serial.print("Begin\n");

pinMode(LED_BUILTIN,OUTPUT);
digitalWrite(LED_BUILTIN,LOW);

//init SD file and headers
String heading = String("MEGA_Time,GPS_Hour,GPS_Minute,GPS_Second,GPS_Lat,GPS_Long,GPS_Alt,")+
    String("BNO_X,BNO_Y,BNO_Z,BNO_GYRO_X,BNO_GYRO_Y,BNO_GYRO_Z,")+
    String("BNO_ACC_X,BNO_ACC_Y,BNO_ACC_Z,BNO_MAG_X,BNO_MAG_Y,BNO_MAG_Z,BNO_TEMP,")+
    String("MPU_GYRO_X,MPU_GYRO_Y,MPU_GYRO_Z,MPU_ACC_X,MPU_ACC_Y,MPU_ACC_Z,MPU_TEMP")+"\r\n";
SD.begin(chipSelect);
if(!SD.exists("data.txt")){
  sensorData = SD.open("data.csv",FILE_WRITE);
  sensorData.write(heading.c_str(), heading.length());
  Serial.print("File not found, initialized\n");
}
else{
  sensorData = SD.open("data.csv",FILE_WRITE);
  Serial.print("File found\n");
}

//Initialize GPS
while(!myI2CGPS.begin()){
  Serial.print("Waiting for GPS to initialize\n");
  delay(500);
}
Serial.print("GPS Initialized\n");

//Initialize BNO055
while(!bno.begin()){
  Serial.print("Waiting for BNO to initialize\n");
  delay(500);
}
Serial.print("BNO Initialized\n");

//Initialize MPU6050
while(!mpu.begin()){
  Serial.print("Waiting for MPU to initialize\n");
  delay(500);
}
mpu_temp = mpu.getTemperatureSensor();
mpu_temp->printSensorDetails();
mpu_accel = mpu.getAccelerometerSensor();
mpu_accel->printSensorDetails();
mpu_gyro = mpu.getGyroSensor();
mpu_gyro->printSensorDetails();
Serial.print("MPU Initialized\n");

}

void loop(){

  unsigned long now = millis();
//  Serial.print(now);
//  Serial.print("\n");
  if((now - lastMillis) >= x_time){
    lastMillis = now;
    // add a new line to the buffer
    String gps_data = readGPS();
    String bno_data = readBNO();
    String mpu_data = readMPU();
    buffer += String(now)+",";
    buffer += gps_data+",";
    buffer += bno_data+",";
    buffer += mpu_data+",";
    buffer += "\r\n";
    if(Serial1.available()){
      relayData(gps_data);
    } 
    else{
      Serial.print("Serial 1 Communication Failure\n");
    }
  }
  
  writeData();
  
}
