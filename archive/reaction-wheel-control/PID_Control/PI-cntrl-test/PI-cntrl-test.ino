#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

//initialize i2c comm with orient sensor
Adafruit_BNO055 bno = Adafruit_BNO055(55);

#define VEL_ADDR 0x60
#define ACC_ADDR 0x61

int DES_EUL = 90;
int VEL_PREV = 0;
//integration constants
float K = 0.1;

// define all tasks
void WHEEL_CNTRL( void *pvParameters );
//void MAG_CNTRL( void *pvParameters );

// define all functions
void I2C_WRITE(int value, int ADDR) {

  Wire.beginTransmission(ADDR);
  Wire.write(64);                     // cmd to update the DAC
  Wire.write(value >> 4);        // the 8 most significant bits...
  Wire.write((value & 15) << 4); // the 4 least significant bits...
  Wire.endTransmission();

}

// the setup function runs once
void setup() {

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  //initialize i2c library
  Wire.begin();

  //settings for orient sensor
  bno.begin();
  bno.setExtCrystalUse(true);

  //setup read pins for motor output (this will need a multiplexor for multiple wheels)
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, OUTPUT);

  //initialize all tasks
  xTaskCreate(WHEEL_CNTRL, "WHEEL_CNTRL", 128, NULL, 1, NULL); //add handle for RTOS priority manipulation

  // Now the task scheduler takes control
  vTaskStartScheduler();

}

void loop()
{
  // EMPTY. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void WHEEL_CNTRL(void *pvParameters) {
  //apply analog voltages based on position and velocity from bno055

  while (1) {

    //read bno055
    sensors_event_t event;
    bno.getEvent(&event);
    int SAT_EUL = event.orientation.x;
    int SAT_VEL = event.gyro.x;

    //distance and direction to turn
    int DIST = DES_EUL - SAT_EUL;
    if (abs(DIST) > 200) {

      int TEMP;
      if (DIST < 0){
        TEMP = 360 - abs(DIST);
        //Serial.print("case 1\n");//troubleshooting
      }
      if (DIST > 0){
        TEMP = -360 + abs(DIST);
        //Serial.print("case 2\n");//troubleshooting
        }
      DIST = TEMP;
      
    }

    //account for velocity
    float DIFF = (float)DIST - ((float)SAT_VEL) * K;
    if(abs(DIST) < 2){
        DIFF = 0;
    }

//troubleshooting
//    Serial.print("DIST: ");
//    Serial.print(DIST);
//    Serial.print("\n");
//    Serial.print("DIFF: ");
//    Serial.print(DIFF);
//    Serial.print("\n");
//    Serial.print(SAT_VEL);
//    Serial.print("\n");

    //apply value
    int ACC = DIFF;
    long ACC_WRITE = ((long)ACC*2000)/200;
    if(abs(ACC_WRITE) > 2000)
      ACC_WRITE = 2000*ACC_WRITE/abs(ACC_WRITE);
    int VEL = VEL_PREV + DIFF*0.1;
    if(abs(VEL) > 4000)
      VEL = 4000*VEL/abs(VEL);
    VEL_PREV = VEL;

//troubleshooting
//    Serial.print("EUL: ");
//    Serial.print(SAT_EUL);
//    Serial.print("\n");
//    Serial.print("VEL: ");
//    Serial.print(VEL);
//    Serial.print("\n");
//    Serial.print("ACC: ");
//    Serial.print(ACC);
//    Serial.print("\n\n");
//    Serial.print("VEL: ");
//    Serial.print(VEL);
//    Serial.print("\n");
//    Serial.print("ACC: ");
//    Serial.print(ACC_WRITE);
//    Serial.print("\n\n");

    if(VEL > 0){
      analogWrite(A2, 0);
      Serial.print("LOW");
    }
    else if(VEL < 0){
      analogWrite(A2, 4095);
      Serial.print("HIGH");
    }
    
    if(abs(DIFF) > 30){
      I2C_WRITE(abs((int)4000), VEL_ADDR);
    }
    else{
      I2C_WRITE(abs((int)VEL), VEL_ADDR);
    }

    I2C_WRITE(abs((int)ACC_WRITE), ACC_ADDR);

    vTaskDelay(5);

  }

  vTaskDelay(1);  // one tick delay (15ms) in between reads for stability

}
