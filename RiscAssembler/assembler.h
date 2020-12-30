#pragma once
#include <stdio.h>
#include <stdlib.h>
#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#define MAX_LINE_LENGTH 500
#define MAX_LABEL_LENGTH 50
#define SIZE_OF_DATA 4096
#define NUM_INST 22
#define NUM_REGI 16
#define ERROR -2 // since EOF is -1 
#define INSTRUCTION 1
#define REGISTER 2
#define LABEL 3
#define IMMEDIATE 4
#define DOT_WORD 5
#define COMMA 6
#define COLON 7

struct instructions {
	char* name;
	int id;
};

struct label {
	char* name;
	int pc;
};

struct registers {
	char* name;
	int id;
};

struct dot_word {
	int line;
	int data;
};

//array for the labels 
extern struct label* labels;
extern int length_of_labels;
//array for .word
extern int dot_word[SIZE_OF_DATA];

//array for the registers 
struct registers regi[] = {
	   {"$zero",0},
	   {"$imm",1},
	   {"$v0",2},
	   {"$a0",3},
	   {"$a1",4},
	   {"$t0",5},
	   {"$t1",6},
	   {"$t2",7},
	   {"$t3",8},
	   {"$s0",9},
	   {"$s1",10},
	   {"$s2",11},
	   {"$gp",12},
	   {"$sp",13},
	   {"$fp",14},
	   {"$ra",15},
};
//array for the instructions 
struct instructions inst[] = {
   {"add",0},
   {"sub",1},
   {"and",2},
   {"or",3},
   {"xor",4},
   {"mul",5},
   {"sll",6},
   {"sra",7},
   {"srl",8},
   {"beq",9},
   {"bne",10},
   {"blt",11},
   {"bgt",12},
   {"ble",13},
   {"bge",14},
   {"jal",15},
   {"lw",16},
   {"sw",17},
   {"reti",18},
   {"in",19},
   {"out",20},
   {"halt",21}
};

// functions 
void first_scan(FILE *fptr);
void second_scan(FILE *fptr, FILE *iptr, FILE *dptr);
int classify_word(char *word);
char* get_word(FILE *fptr);
char fpeek(FILE *fptr);
int add_label(char *label, int length_of_labels, int pc);
void init_labels(int length);
void increase_labels(int length_of_labels);
void wrong_syntax_error();
int add_label_position(char *label, int length_of_labels, int pc);
void free_labels(int len);
int get_register_num(char *regis);
int get_instruction_num(char *instruct);
int get_label_pc(char *label);
int get_num(char *imm);
void write_to_dmem(FILE* dptr);

#endif // !ASSEMBLER_H
