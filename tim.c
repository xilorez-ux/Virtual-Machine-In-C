#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_STACK_SIZE 1024




typedef enum {
	INST_PUSH, //add value to the stack
	INST_POP,
	INST_DUP, //duplicate a value
	INST_SWAP, //swap values a and b's positions in the stack (a --> b, b-->a)
	INST_ADD, //add a and b
	INST_SUB, //sub a and b
	INST_MUL, //multiplying a and b
	INST_DIV, //dividing a by b
	INST_CMPE, //compare if given values are equal
	INST_CMPNE, //compare if given values are not equal
	INST_CMPG, //compare if given value a is greater than value b
	INST_CMPL, //compare if given value a is lower than value b
	INST_CJMP, 
	INST_JMP, //go to specified value index in the stack
	INST_PRINT, //print corresponding value in the stack, if not specified then first in the stack
} Inst_Set;

typedef struct{
	Inst_Set type;
	int value;
} Inst;

typedef struct{
	int stack[MAX_STACK_SIZE];
	int stack_size;
	size_t program_size;
	Inst *instructions;
} Machine;

#define DEF_INST_PUSH(x) {.type = INST_PUSH, .value = x}
#define DEF_INST_POP() {.type = INST_POP}
#define DEF_INST_DUP() {.type = INST_DUP}
#define DEF_INST_SWAP() {.type = INST_SWAP}
#define DEF_INST_ADD() {.type = INST_ADD}
#define DEF_INST_SUB() {.type = INST_SUB}
#define DEF_INST_MUL() {.type = INST_MUL}
#define DEF_INST_DIV() {.type = INST_DIV}
#define DEF_INST_CMPE() {.type = INST_CMPE}
#define DEF_INST_CMPNE() {.type = INST_CMPNE}
#define DEF_INST_CMPG() {.type = INST_CMPG}
#define DEF_INST_CMPL() {.type = INST_CMPL}
#define DEF_INST_CJMP(x) {.type = INST_CJMP, .value = x}
#define DEF_INST_JMP(x) {.type = INST_JMP, .value = x}
#define DEF_INST_PRINT() {.type = INST_PRINT}


Inst program[] = {
	DEF_INST_PUSH(1),
	DEF_INST_PUSH(1),
	DEF_INST_CMPE(),
	DEF_INST_CJMP(7),
	DEF_INST_PUSH(2),
	DEF_INST_ADD(),
	DEF_INST_PUSH(4),
	DEF_INST_PRINT(),
	};

#define PROGRAM_SIZE (sizeof(program)/sizeof(program[0]))


void push(Machine *machine, int value){
	if(machine->stack_size >= MAX_STACK_SIZE){
		fprintf(stderr, "ERROR: Stack Overflow\n");
		exit(1);
	}
	machine->stack[machine->stack_size] = value;
	machine->stack_size++;
}

int pop(Machine *machine){
	if(machine->stack_size <=0){
		fprintf(stderr, "ERROR : Stack Underflow\n");
		exit(1);
	}
	machine->stack_size--;
	return machine->stack[machine->stack_size];
}


void write_program_to_file(Machine *machine, char *file_path){
	FILE *file = fopen(file_path, "wb");

	if(file == NULL){
		fprintf(stderr, "ERROR: Could not write to file %s\n", file_path);
		exit(1);
	}
	
	fwrite(machine->instructions,sizeof(machine->instructions[0]), PROGRAM_SIZE, file);

	fclose(file);
}

Machine *read_program_from_file(Machine *machine, char *file_path){
	FILE *file = fopen(file_path, "rb");
	if(file == NULL){
		fprintf(stderr, "ERROR: Could not read from file %s\n", file_path);
		exit(1);
	}
	Inst *instructions = malloc(sizeof(Inst) * MAX_STACK_SIZE);
	
	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	fread(instructions, sizeof(instructions[0]), length / 8, file);

	machine->program_size = length / 8;
	machine->instructions = instructions;

	fclose(file);
	return machine;

}



int main(void){
	int a, b;
	Machine *loaded_machine = malloc(sizeof(Machine) + MAX_STACK_SIZE);
	loaded_machine->instructions = program;
	write_program_to_file(loaded_machine, "test.tim");
	loaded_machine = read_program_from_file(loaded_machine, "test.tim");
	for(size_t ip = 0; ip < loaded_machine->program_size; ip++){
		switch(loaded_machine->instructions[ip].type){
			case INST_PUSH:
				push(loaded_machine, loaded_machine->instructions[ip].value);
				break;
			case INST_POP:
				pop(loaded_machine);
				break;
			case INST_DUP:
				a = pop(loaded_machine);
				push(loaded_machine, a);
				push(loaded_machine, a);
				break;
			case INST_SWAP:
				a = pop(loaded_machine);
				b = pop(loaded_machine);
				push(loaded_machine, a);
				push(loaded_machine, b);
				break;
			case INST_ADD:
				a = pop(loaded_machine);
				b = pop(loaded_machine);
				push(loaded_machine, a + b);
				break;
			case INST_SUB:
				a = pop(loaded_machine);
				b = pop(loaded_machine);
				push(loaded_machine, a - b);
				break;
			case INST_MUL:
				a = pop(loaded_machine);
				b = pop(loaded_machine);
				push(loaded_machine, a * b);
				break;
			case INST_DIV:
				a = pop(loaded_machine);
				b = pop(loaded_machine);
				if(b == 0){
					fprintf(stderr, "ERROR: Cannot divide by 0\n");
					exit(1);
				}
				push(loaded_machine, a / b);
				break;
			case INST_CMPE:
				a = pop(loaded_machine);
				b = pop(loaded_machine);
				push(loaded_machine, b);
				push(loaded_machine, a);
				if(a == b){
					push(loaded_machine, 1);
				} else {
					push(loaded_machine, 0);
				}
				break;
			case INST_CMPNE:
				a = pop(loaded_machine);
				b = pop(loaded_machine);
				push(loaded_machine, b);
				push(loaded_machine, a);
				if(a != b){
					push(loaded_machine,1);
				} else {
					push(loaded_machine,0);
				}
				break;
			case INST_CMPG:
				a = pop(loaded_machine);
				b = pop(loaded_machine);
				push(loaded_machine, a);
				push(loaded_machine, b);
				if(a > b){
					push(loaded_machine, 1);
				}
				else if(a ==b){
					fprintf(stderr, "Error: wrong method called, a == b");
				} else {
					push(loaded_machine, 0);
				}
				break;
			case INST_CMPL:
				a = pop(loaded_machine);
				b = pop(loaded_machine);
				push(loaded_machine, a);
				push(loaded_machine, b);
				if(a < b){
					push(loaded_machine, 1);
				} else if(a == b){
				       fprintf(stderr, "Error: wrong method called, a == b");
				} else {
					push(loaded_machine, 0);
				}
				break;
			case INST_CJMP:
				if(pop(loaded_machine) == 1){
					ip = loaded_machine->instructions[ip].value - 1;
					if(ip + 1 >= loaded_machine->program_size){
						fprintf(stderr, "ERROR: Cannot jump out of bounds\n");
						exit(1);
					}
				} else {
					continue;
				}
				break;
			case INST_JMP:
				ip = loaded_machine->instructions[ip].value - 1;
				if(ip + 1 >=loaded_machine->program_size){
					fprintf(stderr, "ERROR: Cannot jump out of bounds\n");
					exit(1);
				}
				break;
			case INST_PRINT:
				printf("%d\n", pop(loaded_machine));
				break;
		}
	}

	return 0;
}

