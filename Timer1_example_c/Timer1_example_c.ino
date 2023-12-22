//timer interrupts


//timer setup for timer1.

/*
 Demonstrate the use of Timer 1 interrupts
 generate interrupts at a specified frequency, and when the interrupt occurs, it toggles the state of an LED
 blinking effect on the LED with a frequency determined by the timer settings.
 
*/

#define ledPin 13


void setup(){ //It initializes the system and sets up the timer for generating interrupts.
  //set pin as output
  pinMode(ledPin, OUTPUT);

  noInterrupts(); // disable all interrupts, to ensure that the subsequent timer setup is not interrupted.
  //timer configurations
  TCCR1A = 0; //Clears the Timer/Counter Control Register A for Timer 1
  TCCR1B = 0; //Clears the Timer/Counter Control Register B for Timer 1.
  TCNT1 = 0; //Clears the Timer/Counter 1 register.
 
  OCR1A = 31250; // compare match register 16MHz/256/2Hz
  //Sets the compare match register for Timer 1, configuring it to generate an interrupt at a frequency 
  //of 2 Hz (16MHz/256/2Hz). You can uncomment the next line for a frequency of 2 kHz.

  //OCR1A = 31;    // compare match register 16MHz/256/2kHz
  TCCR1B |= (1 << WGM12); // CTC mode  Sets the Waveform Generation Mode to Clear Timer on Compare Match (CTC) mode.
  TCCR1B |= (1 << CS12); // 256 prescaler, Sets the prescaler to 256
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt, Enables the Timer/Counter1 Compare Match A interrupt.
  interrupts(); // enable all interrupts after the timer setup.
}//end setup

// This is an Interrupt Service Routine (ISR) for the Timer/Counter1 Compare Match A interrupt. It is executed when Timer 1 reaches the value specified in OCR1A.
ISR(TIMER1_COMPA_vect){//timer1 interrupt
digitalWrite(ledPin, digitalRead(ledPin) ^ 1); // toggle LED pin
}
  
//In summary, this code configures Timer 1 to generate interrupts at a specific frequency, and when the interrupt occurs, it toggles the state of an LED connected to pin 13. 
//The loop() function is empty as the primary action is driven by interrupts.

void loop(){
  //do other things here
  //This function is typically empty in this type of code, as the main functionality is implemented using interrupts. It gets executed repeatedly 
  //after the setup() function.
}
