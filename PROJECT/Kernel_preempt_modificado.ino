

//Definir os Pins dos LEDs
#define d1 13
#define d2 12
#define d3 11
#define d4 10
#include <Arduino.h>
#include <limits.h>

/* 4 Tasks:
 *     T1 - T = 100ms   -> Led d1 toggle
 *     T2 - T = 200ms   -> Led d2 toggle
 *     T3 - T = 600ms   -> Led d3 toggle
 *     T4 - T = 100ms   -> Led d4 copied from button A1
 */

int Sched_AddT(void (*f)(void), int d, int deadline); //Adicionar uma nova tarefa

/*void toggle(void) {digitalWrite(d4, !digitalRead(d4));}

/*void t0(void) {
  static int A1_old;
  int A1_new = digitalRead(A1); 
  
  if (A1_new != A1_old)
    Sched_AddT(toggle, 2000 /* delay */, 0 /* deadline */);
    
  /*A1_old = A1_new;
}*/

void t1(void) {digitalWrite(d1, !digitalRead(d1)); delay(700);}
void t2(void) {digitalWrite(d2, !digitalRead(d2));}
void t3(void) {digitalWrite(d3, !digitalRead(d3));}
void t4(void) {digitalWrite(d4, !digitalRead(d4));}


//Estrutra da struct onde tem as características da tarefa
typedef struct {
  /* period in ticks */
  int period;
  /* ticks until next activation */
  int delay;
  /* function pointer */
  void (*func)(void);
  /* activation counter */
  int exec;
  /* remaining deadline -> usado quando temos EDF*/
  int remaining_deadline; 
} Sched_Task_t;

#define NT 20 //variável número de tarefas total
Sched_Task_t Tasks[NT];
int cur_task = NT;

//Iniciar o agendamento das tarefas 
int Sched_Init(void){
  for(int x=0; x<NT; x++)
    Tasks[x].func = 0;
  /* Also configures interrupt that periodically calls Sched_Schedule(). */
  noInterrupts(); // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
 
  OCR1A = 6250; // set the time quantum to 10ms (16MHz/256/10Hz) OCR1A register is set to 6250, which corresponds to a time quantum of 10ms  // OCR1A = 16000; // 16MHz / 1000Hz?
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS12); // 256 prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  interrupts(); // enable all interrupts  
}

//Adicionar Tarefas
int Sched_AddT(void (*f)(void), int d, int deadline) {
  for (int x = 0; x < NT; x++) {
    if (!Tasks[x].func) {
      Tasks[x].period = d; // Set the task's period
      Tasks[x].delay = 0;
      Tasks[x].exec = 0;
      Tasks[x].func = f;
      Tasks[x].remaining_deadline = deadline; // Set the task's remaining deadline
      return x;
    }
  }
  return -1; // Return -1 if no empty slot is found
}

//Agendar agendamento
void Sched_Schedule() {
  int min_deadline = INT_MAX; //INT_MAX e uma varivel que dá o número máximo de bits e que esta presente em <Arduino.h>
  int min_deadline_task = -1;

  for (int x = 0; x < NT; x++) {
    if (Tasks[x].func && Tasks[x].delay) {
      Tasks[x].delay--;
    } else {
      // Update Task
      Tasks[x].exec++;
      Tasks[x].delay = Tasks[x].period - 1;
      Tasks[x].remaining_deadline = Tasks[x].period; // Update the task's remaining deadline

      // Find Task with Earliest Deadline
      if (Tasks[x].remaining_deadline < min_deadline) {
        min_deadline = Tasks[x].remaining_deadline;
        min_deadline_task = x;
      }
    }
  }

  // Update the cur_task variable to execute the task in the ISR
  cur_task = min_deadline_task;
}


//Executar tarefas
/* 1. Iterate through the task set to find the task with the earliest remaining deadline.
   2. If a task with a non-zero remaining deadline is found, mark the task as completed if its remaining deadline reaches zero 
   and run the next task with the earliest remaining deadline.*/ 

void Sched_Dispatch(){ //-> Function iterates through the task set to find the task with the earliest remaining deadline. 
  int earliest_deadline_task = -1;
  int earliest_deadline = INT_MAX;

  // Find the task with the earliest remaining deadline
  for(int x=0; x<NT; x++) {
    if(Tasks[x].exec && Tasks[x].period > 0 && Tasks[x].remaining_deadline > 0) {
      if(Tasks[x].remaining_deadline < earliest_deadline) {
        earliest_deadline = Tasks[x].remaining_deadline;
        earliest_deadline_task = x;
      }
    }
  }

  // If a task with a non-zero remaining deadline is found, mark it as completed if its remaining deadline reaches zero
  if(earliest_deadline_task >= 0) {
    Tasks[earliest_deadline_task].remaining_deadline--;

    if(Tasks[earliest_deadline_task].remaining_deadline == 0) {
      Tasks[earliest_deadline_task].exec = 0;
      Tasks[earliest_deadline_task].activation_count++;
    }
  }
}

ISR(TIMER1_COMPA_vect) {
  Sched_Schedule();
  Sched_Dispatch();

  // Find the task with the highest deadline among the tasks that are ready to run
  int max_deadline = 0;
  int max_task_index = 0;
  for (int i = 0; i < NT; i++) {
    if (Tasks[i].exec && Tasks[i].period > 0 && Tasks[i].remaining_deadline > 0) {
      if (Tasks[i].remaining_deadline > max_deadline) {
        max_deadline = Tasks[i].remaining_deadline;
        max_task_index = i;
      }
    }
  }

  // Update the remaining deadline of the selected task
  Tasks[max_task_index].remaining_deadline--;

  // If the selected task's remaining deadline reaches zero, mark it as completed and update the task's activation counter
  if (Tasks[max_task_index].remaining_deadline == 0) {
    Tasks[max_task_index].exec = 0;
    Tasks[max_task_index].activation_count++;
  }

  // Execute the task with the earliest deadline
  if (cur_task >= 0) {
    Tasks[cur_task].exec = 0;
    Tasks[cur_task].func();
    /* Delete task if one-shot */
    if (!Tasks[cur_task].period) Tasks[cur_task].func = 0;
  }
}

//Exemplo com Leds das tarefas e a sua sequência a seguir
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(d4, OUTPUT);
  pinMode(d3, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d1, OUTPUT);

  //Agregar o ISR a um botão
  //attachInterrupt(digitalPinToInterrupt(2), ISR, RISING)). //Arduino Uno tem dois pinos que podem ser usados como interrupção externa, o pino 2 e 3 -> Interrupções externas

  //Iniciar agendamento
  Sched_Init();

  Sched_AddT(t0, 1 /* delay */,   10 /* period */);
  Sched_AddT(t3, 1 /* delay */,  200 /* period */);
  Sched_AddT(t2, 1 /* delay */,  500 /* period */);

  /* add a bunch of one-shot tasks, that will temporarily take up space in the TCB array */
  /* This forces task t1 to have a lower priority, and leave empty TCB entries for the 
   *  toggle task added by t0.
   */
  Sched_AddT(toggle, 10 /* delay */,  0 /* period */);
  Sched_AddT(toggle, 20 /* delay */,  0 /* period */);
  Sched_AddT(toggle, 30 /* delay */,  0 /* period */);
  Sched_AddT(toggle, 40 /* delay */,  0 /* period */);
  Sched_AddT(toggle, 50 /* delay */,  0 /* period */);
  Sched_AddT(toggle, 60 /* delay */,  0 /* period */);
  Sched_AddT(toggle, 70 /* delay */,  0 /* period */);
  Sched_AddT(toggle, 80 /* delay */,  0 /* period */);
  
  Sched_AddT(t1, 1 /* delay */, 1000 /* period */);

}


// the loop function runs over and over again forever
void loop() {
  /* nothing to do */
}
