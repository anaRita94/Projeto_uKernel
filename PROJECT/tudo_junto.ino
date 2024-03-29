#include <Arduino_FreeRTOS.h>
#define portCLEAR_COUNTER_ON_MATCH (0x08)
#define portPRESCALE_256 (0x04)
#define portCLOCK_PRESCALER (256)
#define portCOMPARE_MATCH_A_INTERRUPT_ENABLE (0x10)

#define F_CPU 16000000 
#define configTICK_RATE_HZ 1000



static void prvSetupTimerInterrupt(void){
  TCNT1 = 65535 - (F_CPU / 2048);

  TCCR1B = (1 << CS10) | (1 << CS12);

  TCCR1A = 0x00;

  TIMSK1 = (1 << TOIE1);

  sei();

}



/*static void prvSetupTimerInterrupt(void)
{
  unsigned long ulCompareMatch;
  unsigned char ucHighByte, ucLowByte;

  // Generate the compare match value for our required tick frequency.
  ulCompareMatch = F_CPU / configTICK_RATE_HZ;

  // We only have 16 bits so have to scale to get our required tick rate.
  ulCompareMatch /= portCLOCK_PRESCALER;
  // Setup compare match value for compare match A.
  // Interrupts are disabled before calling this function, so we need not bother here.
  ucLowByte = ulCompareMatch & 0xff;
  ulCompareMatch >>= 8;
  ucHighByte = ulCompareMatch & 0xff;

  // Disable Timer1 interrupts
  TIMSK1 &= ~(1 << OCIE1A);

  // Setup clock source and compare match behavior.
  TCCR1A = 0;
  TCCR1B = 0;

  // Set compare match value
  OCR1AH = ucHighByte;
  OCR1AL = ucLowByte;

  // Setup clock source and compare match behavior.
  TCCR1B |= (1 << WGM12);  // CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); // 1024 prescaler

  // Enable the interrupt - this is okay as interrupts are currently globally disabled.
  TIMSK1 |= (1 << OCIE1A);
}*/


void SIG_OUTPUT_COMPARE1A( void ) __attribute__ ( ( signal,
naked ) );
void vPortYieldFromTick( void ) __attribute__ ( ( naked ) );

#define portSAVE_CONTEXT() \
asm volatile ( \
 "push r0 \n\t" \ 
 "in r0, __SREG__ \n\t" \
 "cli \n\t" \
 "push r0 \n\t" \
 "push r1 \n\t" \ 
 "clr r1 \n\t" \ 
 "push r2 \n\t" \ 
 "push r3 \n\t" \
 "push r4 \n\t" \
 "push r5 \n\t" \
 "push r6 \n\t" \
 "push r7 \n\t" \
 "push r8 \n\t" \
 "push r9 \n\t" \
 "push r10 \n\t" \
 "push r11 \n\t" \
 "push r12 \n\t" \
 "push r13 \n\t" \
 "push r14 \n\t" \
 "push r15 \n\t" \
 "push r16 \n\t" \
 "push r17 \n\t" \
 "push r18 \n\t" \
 "push r19 \n\t" \
 "push r20 \n\t" \
 "push r21 \n\t" \
 "push r22 \n\t" \
 "push r23 \n\t" \
 "push r24 \n\t" \
 "push r25 \n\t" \
 "push r26 \n\t" \
 "push r27 \n\t" \
 "push r28 \n\t" \
 "push r29 \n\t" \
 "push r30 \n\t" \
 "push r31 \n\t" \
 "lds r26, pxCurrentTCB \n\t" \ 
 "lds r27, pxCurrentTCB + 1 \n\t" \ 
 "in r0, __SP_L__ \n\t" \ 
 "st x+, r0 \n\t" \ 
 "in r0, __SP_H__ \n\t" \ 
 "st x+, r0 \n\t" \ 
);



#define portRESTORE_CONTEXT() \
asm volatile ( \
 "lds r26, pxCurrentTCB \n\t" \
 "lds r27, pxCurrentTCB + 1 \n\t" \
 "ld r28, x+ \n\t" \
 "out __SP_L__, r28 \n\t" \
 "ld r29, x+ \n\t" \
 "out __SP_H__, r29 \n\t" \
 "pop r31 \n\t" \
 "pop r30 \n\t" \
 "pop r29 \n\t" \
 "pop r28 \n\t" \
 "pop r27 \n\t" \
 "pop r26 \n\t" \
 "pop r25 \n\t" \
 "pop r24 \n\t" \
 "pop r23 \n\t" \
 "pop r22 \n\t" \
 "pop r21 \n\t" \
 "pop r19 \n\t" \
 "pop r18 \n\t" \
 "pop r17 \n\t" \
 "pop r16 \n\t" \
 "pop r15 \n\t" \
 "pop r14 \n\t" \
 "pop r13 \n\t" \
 "pop r12 \n\t" \
 "pop r11 \n\t" \
 "pop r10 \n\t" \
 "pop r9 \n\t" \
 "pop r8 \n\t" \
 "pop r7 \n\t" \
 "pop r6 \n\t" \
 "pop r5 \n\t" \
 "pop r4 \n\t" \
 "pop r3 \n\t" \
 "pop r2 \n\t" \ 
 "pop r1 \n\t" \
 "pop r0 \n\t" \ 
 "out __SREG__, r0 \n\t" \
 "pop r0 \n\t" \
);


void vPortYieldFromTick( void )
{

 /* This is a naked function so the context
 is saved. */


 portSAVE_CONTEXT();

 /* Increment the tick count and check to see
 if the new tick value has caused a delay
 period to expire. This function call can
 cause a task to become ready to run. */
 xTaskIncrementTick(); 


 /* See if a context switch is required.
 Switch to the context of a task made ready
 to run by vTaskIncrementTick() if it has a
 priority higher than the interrupted task. */
 vTaskSwitchContext();

 /* Restore the context. If a context switch
 has occurred this will restore the context of
 the task being resumed. */
 portRESTORE_CONTEXT();


 /* Return from this naked function. */
 asm volatile ( "ret" );
}


/* Interrupt service routine for the RTOS tick. */
void SIG_OUTPUT_COMPARE1A( void )
{ 
 /* Call the tick function. */
 vPortYieldFromTick();
 /* Return from the interrupt. If a context
 switch has occurred this will return to a
 different task. */
 asm volatile ( "reti" );
}


TaskHandle_t Task1Handle;
TaskHandle_t Task2Handle;
TaskHandle_t Task3Handle;

int tickCount = 0;

void setup() {
  Serial.begin(9600);

  xTaskCreate(task1, "Task1", 128, NULL, 3, &Task1Handle);
  xTaskCreate(task2, "Task2", 128, NULL, 2, &Task2Handle);
  xTaskCreate(task3, "Task3", 128, NULL, 1, &Task3Handle);

  // Set up timer interrupt for RTOS tick
 /* noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 15624; // 1 second at 16 MHz
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10); // 1024 prescaler
  TIMSK1 |= (1 << OCIE1A);
  interrupts();*/
  prvSetupTimerInterrupt();
  vTaskStartScheduler();
}

void loop() {
  // Empty loop, tasks are scheduled by the FreeRTOS scheduler
}

ISR(TIMER1_OVF_vect) {
  Serial.println("teste");
  TCNT1 = 65535 - (F_CPU / 2048);
  //portYIELD_FROM_ISR(); // Trigger the RTOS tick from the timer interrupt
  SIG_OUTPUT_COMPARE1A(); 
}

void task1(void *pvParameters) {
  while (1) {
    Serial.println("Task1");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
void task2(void *pvParameters) {
  while (1) {
    int countt = 100 + countt;
    Serial.print("Task 2 ");
    Serial.println(countt);
    //Serial.println("Task 2");
    vTaskDelay(1500 / portTICK_PERIOD_MS);
  }
}

void task3(void *pvParameters) {
  while (1) {
    int count = count + 1; 
    Serial.print("Task 3 ");
    Serial.println(count);
    //Serial.println("Task 3");
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}
