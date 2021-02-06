#include <Arduino_FreeRTOS.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

//setup BNO055 Reader
Adafruit_BNO055 bno = Adafruit_BNO055(55,0x28);

//setup SD writing
const int chipSelect = 53;
File sensorData;

//function definition
void flash(int pin){
  digitalWrite(pin, HIGH);
  vTaskDelay( 10 );
  digitalWrite(pin, LOW);
  vTaskDelay( 10 );
}

int saveData(String data){
  //sensorData = SD.open("data.txt", FILE_WRITE);
  if(sensorData){
    sensorData.write(data.c_str(), data.length());
    return 1;
  }
  else{
    return 0;
  }
}

// define two tasks
//void TaskBlink( void *pvParameters );
//void Comms( void *pvParameters );
void ReadWrite( void *pvParameters );

void setup() {
//initialize serial communication at 9600 bits per second:
Serial.begin(115200);
Serial1.begin(115200);

Serial.print("Begin\n");

pinMode(LED_BUILTIN,OUTPUT);
digitalWrite(LED_BUILTIN,LOW);

String heading = String("X")+","+String("Y")+","+String("Z");

//initialize file to write to in SD card
SD.begin(chipSelect);
if(!SD.exists("data.txt")){
  sensorData = SD.open("data.txt",FILE_WRITE);
  sensorData.write(heading.c_str(), heading.length());
  Serial.print("File not found, initialized\n");
}
else{Serial.print("File found\n");}

//settings for orient sensor
bno.begin();
//bno.setExtCrystalUse(true);

Serial.print("BNO Setup\n");

xTaskCreate(TaskBlink,"Blink",128,NULL,1,NULL);
xTaskCreate(Comms,"Comms",128,NULL,2,NULL);
xTaskCreate(ReadWrite,"ReadWrite",128,NULL,3,NULL);

Serial.print("Tasks Created\n");

//start scheduler
vTaskStartScheduler();
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskBlink(void *pvParameters){
  (void) pvParameters;
// initialize digital pin 13 as an output
  pinMode(12, OUTPUT);
//flash pin continually while working
  while(1){
    flash(12);
//indicates functional RTOS
  }
}

void Comms(void *pvParameters){
  (void) pvParameters;
//7 is input, init variables
  pinMode(7,INPUT);
  String msg;
  int count;
  int i;
//read input pin
  while(1){
//check message availability
    if(Serial.available()){
//pause RTOS for communication
      vTaskSuspendAll();
      msg = Serial.readStringUntil('\n');
      count = msg.length();
      for(i = 0; i < count; i++){
//send every character of message
          Serial1.write(msg[i]);
      }
      Serial1.write('\n');
//resume RTOS
      xTaskResumeAll();
    }
//delay for RTOS stability
    vTaskDelay(10);
  }
}

void ReadWrite(void *pvParameters){
  (void) pvParameters;

  while(1){

  vTaskSuspendAll();

  String data = "";
  int success = 0;
  
  //read bno055
  sensors_event_t event;
  bno.getEvent(&event);
  data = String(event.orientation.x) + "," + String(event.orientation.y) +
    "," + String(event.orientation.z) + ","; 
    
//  while(success = 0){
//    digitalWrite(LED_BUILTIN,HIGH);
//    Serial.print("Writing");
//    success = saveData(data);
//  }
//  digitalWrite(LED_BUILTIN,LOW);

  sensorData.write(data.c_str(), data.length());

  Serial.print("Successful Write");
  
  xTaskResumeAll();

  vTaskDelay(10);

  }
}
