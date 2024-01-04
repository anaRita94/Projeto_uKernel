#include <Arduino.h>
uint16_t push_data_to_stack(int array[], int size) {
    int i;
    uint16_t endereco_SP;

    // Guardar dados na stack
    for (i = 0; i < size; i++) {
        asm volatile (
            "push %[value]"
            :
            : [value] "r" (array[i])
        );
    }
    endereco_SP = SP;
	return endereco_SP;
}


uint16_t pull_data_from_stack(int array[], int size, uint16_t endereco_SP) {
    int value, i;
    SP = endereco_SP;

    // Retirar os dados da stack pela ordem inversa
    for (i = size - 1; i >= 0; i--) {
        
        asm volatile (
            "pop %[value]"
            : [value] "=r" (value)
        );
	array[i] = value;
    }

    endereco_SP = SP;
    return endereco_SP;
}

typedef struct {
  uint16_t endereco_stack;
  int tamanho;
  int variaveis[];
} processo;

processo um, dois;


void setup() {
  // put your setup code here, to run once:

um.variaveis[0] = 2;
um.variaveis[1] = 3;

dois.variaveis[0] = 10;
dois.variaveis[1] = 20;

Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
um.endereco_stack = push_data_to_stack(um.variaveis, um.tamanho);

pull_data_from_stack(dois.variaveis,dois.tamanho, um.endereco_stack);

if(dois.variaveis[0] == 2)
Serial.print("OK ||||");

else
Serial.println("Not OK ||||");

if(dois.variaveis[1] == 3)
Serial.println("OK ");

else
Serial.println("Not OK :(");
}
s