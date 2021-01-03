#include <Arduino_FreeRTOS.h>

// define two tasks for Blink & AnalogRead
void TaskBlink( void *pvParameters );
void Comms( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {

// initialize serial communication at 9600 bits per second:
Serial.begin(9600);

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

  pinMode(2, OUTPUT);

  while(1)
  {
    
  if (Serial.available())
  {
  // Read one value from the Serial buffer and store it in the variable com
  int com = Serial.read();
  // Act according to the value received
  if (com == 'x')
  {
  // stop the led
  digitalWrite(2, LOW);
  }
  else if (com == 'a')
  {
  // Start the LED
  digitalWrite(2, HIGH);
  }
  }
    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}
