#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"

//array for the labels 
struct label* labels;
int length_of_labels;
//array for .word
int dot_word[SIZE_OF_DATA];

// opens the files to write/read and calls the functions :"first_scan(fptr)" and "second_scan(fptr)" 
// to create the output files 
// argv1 - fib.asm (or something.asm)
// argv2 - imemin.txt
// argv3 - dmemin.txt
int main(int argc, char* argv[]) {

	errno_t err = NULL, err1 = NULL, err2 = NULL;
	FILE *fptr = NULL, *iptr = NULL, *dptr = NULL;

	// meanwhile we open the file via fopen_s instead of command line argument 
	err = fopen_s(&fptr, argv[1], "r");

	if (err != 0)
	{
		printf("cannot open file");
		exit(1);
	}

	first_scan(fptr);
	fclose(fptr);

	err = fopen_s(&fptr, argv[1], "r");
	if (err != 0)
	{
		printf("cannot open file");
		exit(1);
	}
	err1 = fopen_s(&iptr, argv[2], "w+");
	if (err1 != 0)
	{
		printf("cannot open file");
		exit(1);
	}
	err2 = fopen_s(&dptr, argv[3], "w+");
	if (err2 != 0)
	{
		printf("cannot open file");
		exit(1);
	}

	second_scan(fptr, iptr, dptr);

	fclose(fptr);
	fclose(iptr);
	fclose(dptr);

	return 0;
}

// input arguments:
// arg 1 - input argument, pointer to the assembly file  
// read the file char by char, calculate the PC and save the labels addresses to be used in the second scan 
// in addition, the function checks the syntax, and will exit the program if a mistake occurs. 
void first_scan(FILE *fptr) {

	int type = ERROR, pc = 0, is_imm = 0;
	char* buffer = NULL;
	length_of_labels = MAX_LABEL_LENGTH;
	
	// prepare the array of the lables we're about to collect 
	init_labels(length_of_labels);

	do {

		is_imm = 0;

		// get a word
		buffer = get_word(fptr);
		type = classify_word(buffer);

		//instruction 
		if (type == INSTRUCTION) {
			//registers and commas 
			for (int i = 0; i < 3; i++) {
				buffer = get_word(fptr);
				if (classify_word(buffer) == REGISTER) {
					// if we use the imm register - the pc will increase in 2 
					if (!strcmp(buffer, "$imm"))
						is_imm = 1;
					buffer = get_word(fptr);
					if (classify_word(buffer) != COMMA) {
						wrong_syntax_error();
					}
				}
				else {
					wrong_syntax_error();
				}
			}
			//immediate
			buffer = get_word(fptr);
			//label
			if (classify_word(buffer) == LABEL) {
				// try to add the label to the array - if we ran out of place
				// allocate some new memory, and then add the label 
				if (!add_label(buffer, length_of_labels,-1)) {
					increase_labels(length_of_labels);
					length_of_labels *= 2;
					add_label(buffer, length_of_labels,-1);
				}
			}
			else {
				if (classify_word(buffer) != IMMEDIATE)
					wrong_syntax_error();
			}
			if (is_imm)
				pc += 2;
			else
				pc += 1;
		}

		//label
		if (type == LABEL) {
			if (!add_label(buffer, length_of_labels, pc)) {
				increase_labels(length_of_labels);
				length_of_labels *= 2;
				add_label(buffer, length_of_labels, pc);
			}
			//colon
			buffer = get_word(fptr);
			type = classify_word(buffer);
			if (type != COLON)
				wrong_syntax_error();
		}

		//.word 
		if (type == DOT_WORD) {
			//expect 2 immediates 
			for (int i = 0; i < 2; i++) {
				buffer = get_word(fptr);
				if (classify_word(buffer) != IMMEDIATE)
					wrong_syntax_error();
			}
			pc += 1;
		}

	} while(buffer[0] != EOF);
	free(buffer);
}

// input arguments:
// arg 1 - input argument, pointer to the assembly file  
// arg 2 - result argument, pointer to the instruction file  
// arg 3 - result argument, pointer to the data file  
// the function fill the imem.txt file and the dmem.txt file. note that we assume a correct input (assembly file)
// since it was examine in the first scan. 
void second_scan(FILE *fptr, FILE *iptr, FILE *dptr) {
	int type = ERROR, is_imm = 0;
	char* buffer = NULL;
	// init thw data array (will be written to the dmem file)
	for (int i = 0; i < SIZE_OF_DATA; i++) {
		dot_word[i] = -1;
	}
	do {

		is_imm = 0;

		// get a word
		buffer = get_word(fptr);
		type = classify_word(buffer);

		//label
		if (type == LABEL) {
			buffer = get_word(fptr); //colon 
		}

		//.word 
		if (type == DOT_WORD) {
			//imm
			buffer = get_word(fptr);
			dot_word[get_num(buffer) % SIZE_OF_DATA] = get_num(get_word(fptr));
		}

		//instruction 
		if (type == INSTRUCTION) {
			fprintf(iptr, "%02X", get_instruction_num(buffer));
			//registers and commas 
			for (int i = 0; i < 3; i++) {
				buffer = get_word(fptr);
				if (!strcmp(buffer, "$imm"))
					is_imm = 1;
				fprintf(iptr, "%01X", get_register_num(buffer));
				buffer = get_word(fptr); //comma
			}
			fprintf(iptr, "\n");
			//immediate
			buffer = get_word(fptr);
			type = classify_word(buffer);
			//label
			if (is_imm) {
				if (type == LABEL) 
					fprintf(iptr, "%05X\n", get_label_pc(buffer));
				
				if(type==IMMEDIATE)
					fprintf(iptr, "%05X\n", get_num(buffer) & 0xfffff);
			}
		}
	} while (buffer[0] != EOF);
	write_to_dmem(dptr);
	free_labels(length_of_labels);
}


// input arguments:
// arg1 - input argument, pointer to a string 
// the function returns a number that indicates the type of the word :
// -2 - error
// 1 - instruction 
// 2 - registers
// 3 - label
// 4 - immediate (hexa or decimal)
// 5 - .word  
// 6 - comma 
// 7 - colon
int classify_word(char* word) { 

	// instruction - 1 
	for (int i = 0; i < NUM_INST; i++) {
		if (!strcmp(word, inst[i].name))
			return INSTRUCTION; 
	}

	// register - 2
	for (int i = 0; i < NUM_INST; i++) {
		if (!strcmp(word, regi[i].name))
			return REGISTER;
	}
	
	// label - 3
	if ((word[0] >= 'A' && word[0] <= 'Z') || (word[0] >= 'a' && word[0] <= 'z')) { // fisrt char is a letter
		int flag = 0, i=1; 
		while (flag == 0 && word[i] != '\0') {
			// not a capital letter, small letter or a number
			if (!(word[i] >= 'A' && word[i] <= 'Z') && !(word[i] >= 'a' && word[i] <= 'z') && !(word[i] >= '0' && word[i] <= '9')) {
				flag = 1;
			}
			i++;
		}
		if (flag == 0)
			return LABEL;
	}

	// immediate - 4
	// decimal 
	if ((word[0] >= '0' && word[0] <= '9') || (word[0] == '-')) { // fisrt char is a number, or a '-' 
		int flag = 0, i = 1;
		while (flag == 0 && word[i] != '\0') {
			// not a number
			if (!(word[i] >= '0' && word[i] <= '9')) {
				flag = 1;
				break;
			}
			i++;
		}
		if (flag == 0)
			return IMMEDIATE;
	}
	// hexa 
	if (word[0] == '0') { // fisrt char is a letter
		int flag = 0, i = 2;
		if ((word[1] != '\0') && (word[1] == 'x')) {
			while (flag == 0 && word[i] != '\0') {
				// not a capital letter, small letter or a number
				if (!(word[i] >= 'A' && word[i] <= 'F') && !(word[0] >= 'a' && word[0] <= 'f') && !(word[i] >= '0' && word[i] <= '9')) {
					flag = 1;
				}
				i++;
			}
		}
		if (flag == 0)
			return IMMEDIATE;
	}

	// .word - 5
	if (!strcmp(word, ".word"))
		return DOT_WORD;

	// comma - 6 
	if (!strcmp(word, ","))
		return COMMA;

	// colon - 7 
	if (!strcmp(word, ":"))
		return COLON;

	// error - -2
	return ERROR;

}

// input arguments:
// arg 1 - input argument, pointer to the assembly file  
// the function returns a pointer to a single word from the file (and drops out comments)
char* get_word(FILE *fptr) {
	char *buffer = malloc(MAX_LABEL_LENGTH), ch = '0';
	int i = 1, flag = 0;
	
	// preceed to the actual word (pass any gaps and comments)
	do {
		//peeking the next char 
		ch = fpeek(fptr);
		//EOF
		if (ch == EOF) {
			buffer[0] = EOF;
			buffer[1] = '\0';
			return buffer;
		}
		// gaps
		if ((ch == '\n') || (ch == '\t') || (ch == ' '))
			fgetc(fptr); //pass any gaps
		// comments
		if (ch == '#') {
			do {
				ch = fgetc(fptr);
			} while (ch!='\n'); // when # appears - skip to the end of the line 
		}
	} while ((ch == '\n') || (ch == '\t') || (ch == ' ') || (ch == '#'));

	// we stopped when we reached to the beggining of the word 
	ch = fgetc(fptr);
	buffer[0] = ch;

	if ((ch!= ',') && (ch!=':')) { 
		//copy the word to the buufer untill a different one appears
		do {
			ch = fpeek(fptr);
			if ((ch != ',') && (ch != '$') && (ch != '#') && (ch != '\n') && (ch != '\t') && (ch != ' ') && (ch != EOF) && (ch != ':')) {
				buffer[i] = ch;
				fgetc(fptr);
				i++;
			}
			else
				flag = 1;
		} while (!flag);
	}
	buffer[i] = '\0'; // determinate the string 
	
	return buffer; 
}

// input arguments:
// arg 1 - input argument, pointer to the assembly file  
// result argument:
// the next char in the file 
// peeking in the next char without moving the pointer 
char fpeek(FILE* fptr)
{
    char c = fgetc(fptr);
	return c == EOF ? EOF : ungetc(c, fptr);
}

//input arguments:
// arg1 - a string represents a label from the assembly file 
// arg2 - the length of the labels array 
// arg3 - current pc, if the label appears in instruction it would be -1
// the function return 1 if the label allready appears in the label array, or if the label doesn't appear but it was successfully added 
// otherwise, it returns 0 
int add_label(char *label, int len, int pc) {
	int i = 0, found = 0;
	while ((i < len) && (strcmp(labels[i].name,"")) && (found==0)) {
		if (!strcmp(labels[i].name, label)) {
			found = 1;
			if(pc!=-1)
				labels[i].pc = pc;
			break;
		}
		i++; 
	}
	// label allready exists  
	if (found == 1) {
		return 1;
	}
	// there is place for another label and this label is new! (not found)
	if ((i < len) && (found == 0)) {
		labels[i].name = malloc(strlen(label) + 1);
		labels[i].pc = pc;
		strcpy(labels[i].name, label);
		return 1;
	}
	//the new label needed to be added but we're out of place - return "false"
	else {
		return 0;
	}
}

// input arguments:
// arg1 - the length of the labels array 
// initialize the labels array 
void init_labels(int length) {
	labels = malloc(length * sizeof(struct label));
	for (int i = 0; i < length; i++) {
		labels[i].name = "";
		labels[i].pc = -1;
	}
}

// input arguments:
// arg1 - the length of the labels array
// double the size of the labels array 
void increase_labels(int length_of_labels) {
	int len = length_of_labels * 2;
	struct label* realloced_labels = realloc(labels, len * sizeof(struct label));
	if (realloced_labels) {
		labels = realloced_labels;
	}
	else {
		printf("cannot allocate memory");
		exit(2);
	}
	for (int i = length_of_labels; i < len; i++) {
		labels[i].name = "";
		labels[i].pc = -1;
	}
}

// print a messege and exit the assembler if the syntax is wrong 
void wrong_syntax_error() {
	printf("wrong syntax");
	exit(2);
}

// input arguments:
// arg1 - a label 
// arg2 - the length of the labels array
// arg3 - the vaule of the pc (currently)
// the function add the "position" (pc value) of a label to the labels' array 
int add_label_position(char *label, int length_of_labels, int pc) {
	// if the label allready exists, just update the pc 
	for (int i = 0; i < length_of_labels; i++) {
		if (!strcmp(labels[i].name, label)) {
			labels[i].pc = pc;
			return 1;
		}
	}
	// else we need to allocate more memory
	return 0;
}

// input arguments:
// arg1 - the length of the labels array
// the function free the allocated memory (includes the one of each label)
void free_labels(int len) {
	for (int i = 0; i < len; i++) {
		if (labels[i].name != "") {
			free(labels[i].name);
		}
	}
	free(labels);
}

// input arguments:
// arg1 - a string represents a register name
// the function returns the number of the register whose name was passed
int get_register_num(char *regis) {
	int i = 0;
	// by the time we use this function, we know for sure that the input is one of the registers 
	while (strcmp(regis, regi[i].name))
		i++;
	return regi[i].id;
}

// input arguments:
// arg1 - a string represents an instruction name
// the function returns the number of the instruction whose name was passed
int get_instruction_num(char *instruct) {
	int i = 0;
	// by the time we use this function, we know for sure that the input is one of the instructions 
	while (strcmp(instruct, inst[i].name))
		i++;
	return inst[i].id;
}

// input arguments:
// arg1 - a string represents a label
// the function returns matched pc of the label as it was calculated in the first scan (in labels array) 
int get_label_pc(char *label) {
	int i = 0;
	// by the time we use this function, we know for sure that the input is one of the instructions 
	while (strcmp(label, labels[i].name))
		i++;
	return labels[i].pc;
}

// input argumants:
// arg1 - a string represents a number (decimal or hexa)
// the function converts the string to a number, whether it contains a number in hexa or decimal
// returns the number in decimal 
int get_num(char *imm) {
	int num;
	if ((imm[0] == '0') && (imm[1] != '\0') && ((imm[1] == 'x') || (imm[1] == 'X')))
	{
		num = strtol(imm, NULL, 16);
	}
	else
		num = atoi(imm);
	return num;
}

// input arguments:
// arg1 - a pointer to dmem file
// the function takes the data in the array data and copy it to the dmemin file 
void write_to_dmem(FILE* dptr) {
	for (int i = 0; i < SIZE_OF_DATA; i++) {
		if (dot_word[i] != -1) {
			fprintf(dptr, "%08X\n", dot_word[i] & 0xfffff);
		}
		else {
			fprintf(dptr, "00000000\n");
		}
	}
}