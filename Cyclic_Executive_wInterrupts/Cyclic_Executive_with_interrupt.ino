



#define d1 13
#define d2 12
#define d3 11
#define d4 10

/* 4 Tasks:
 *     T1 - T = 100ms   -> Led d1 toggle
 *     T2 - T = 200ms   -> Led d2 toggle
 *     T3 - T = 600ms   -> Led d3 toggle
 *     T4 - T = 100ms   -> Led d4 copied from button A1
 */

void t1() {digitalWrite(d1, !digitalRead(d1));}
void t2() {digitalWrite(d2, !digitalRead(d2));}
void t3() {digitalWrite(d3, !digitalRead(d3));}
void t4() {digitalWrite(d4,  digitalRead(A1));}

void m1() {
 t1();
 t2();
 t3();
 t4();
}

void m2() {
 t3();
 t4();
}

void m3() {
 t2();
 t3();
 t4();
}

void m4() {
 t3();
 t4();
}

void m5() {
 t2();
 t3();
 t4();
}

void m6() {
 t3();
 t4();
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(d4, OUTPUT);
  pinMode(d3, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d1, OUTPUT);

  noInterrupts(); // function is used to disable all interrupts before configuring the timer
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
 
  //No exemplo timer1 usa o 31250
  //use the Compare Match A interrupt with a frequency of 10 Hz (compare match register set to 6250 for a 16MHz clock and 256 prescaler).
  OCR1A = 6250; // compare match register 16MHz/256/10Hz
  //OCR1A = 31250; // compare match register 16MHz/256/2Hz
  //OCR1A = 31;    // compare match register 16MHz/256/2kHz
  TCCR1B |= (1 << WGM12); // CTC mode  Sets the Waveform Generation Mode to Clear Timer on Compare Match (CTC) mode.
  TCCR1B |= (1 << CS12); // 256 prescaler, Sets the prescaler to 256
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt, Enables the Timer/Counter1 Compare Match A interrupt.
  interrupts(); // enable all interrupts  
}

volatile int go = 0;
// indicating that it may be changed at any time outside the control of the code. 
//******!!!It serves as a flag to signal the main loop when to proceed.

//interrupt service routine (ISR) named TIMER1_COMPA_vect
ISR(TIMER1_COMPA_vect){//timer1 interrupt
  go = 1; //The ISR sets the go variable to 1, indicating that the main loop can proceed with the next set of tasks.
  //This interrupt is generated when the Timer 1 counter matches the value set in the OCR1A register.
}


/*
The wait function is introduced to pause the execution of the main loop until the go flag is set by the interrupt service routine. This ensures synchronization between the 
periodic interrupt and the main loop execution.
*/
void wait() {
  while (go == 0);
  go = 0;
}

// the loop function runs over and over again forever
//This allows for a more precise timing control and responsiveness in the execution of tasks. 
//uses interrupts to signal when the next set of tasks should be executed.
void loop() {
  m1();
  wait();
  m2();
  wait();
  m3();
  wait();
  m4();
  wait();
  m5();
  wait();
  m6();
  wait();
}
