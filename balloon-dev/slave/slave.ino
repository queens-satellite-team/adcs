#include <Arduino_FreeRTOS.h>

//function definition
void flash(int pin){
    digitalWrite(pin, HIGH);
    vTaskDelay( 10 );
    digitalWrite(pin, LOW);
    vTaskDelay( 10 );
}

// define two tasks for Blink & AnalogRead
void TaskBlink( void *pvParameters );
void Comms( void *pvParameters );

void setup() {
// initialize serial communication at 9600 bits per second:
Serial.begin(9600);

xTaskCreate(TaskBlink,"Blink",128,NULL,1,NULL);
xTaskCreate(Comms,"Comms",128,NULL,2,NULL);

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
  pinMode(13, OUTPUT);
//flash pin continually while working
  while(1){
    flash(13);
//indicates functional RTOS
  }
}

void Comms(void *pvParameters){
  (void) pvParameters;
//2 is output, init variables
  pinMode(2, OUTPUT);
  String msg;
//read serial and react
  while(1){
//check availability
    if (Serial.available()){
//pause RTOS for communication
      vTaskSuspendAll();
      msg = Serial.readStringUntil('\n');
      switch(msg[1]){
        case 'n':
          digitalWrite(2,HIGH);
          break;
        case 'f':
          digitalWrite(2,LOW);
          break;
      }
//resume RTOS
      xTaskResumeAll();
    }
//delay for RTOS
    vTaskDelay(10);
  }
}
