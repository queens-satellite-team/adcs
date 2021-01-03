#include <Arduino_FreeRTOS.h>

// define two tasks for Blink & AnalogRead
void TaskBlink( void *pvParameters );
void Comms( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {

// initialize serial communication at 9600 bits per second:
Serial.begin(9600);
Serial1.begin(9600);

xTaskCreate(TaskBlink,"Blink",128,NULL,1,NULL);
xTaskCreate(Comms,"Comms",128,NULL,2,NULL);

vTaskStartScheduler();

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskBlink(void *pvParameters)  // This is a task.
{

  (void) pvParameters;
  
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);

  while(1) // A Task shall never return or exit.
  {
    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 10 ); // wait for one second
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay( 10 ); // wait for one second
  }
}

void Comms(void *pvParameters)  // This is a task.
{

  (void) pvParameters;

  pinMode(7,INPUT);
  int va = 0;

  while(1){
    vTaskSuspendAll();
    Serial.print(va);
    va = digitalRead(7);
    if(va == HIGH){
      Serial1.write('a');
      Serial.write('a');    }
    else{
      Serial1.write('x');
      Serial.write('x');
    }
    Serial.print(va);
    xTaskResumeAll();  
    vTaskDelay(10);  // one tick delay (15ms) in between reads for stability
  }
}
