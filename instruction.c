/*!
 * \file instruction.c
 * \print d'instruction.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "instruction.h"

 //! Forme imprimable des codes opérations
const char *cop_names[] = {"ILLOP",	"NOP", "LOAD", "STORE",	"ADD", "SUB", "BRANCH", "CALL", "RET", "PUSH", "POP", "HALT"};

//! Forme imprimable des conditions
const char *condition_names[] = {"NC", "EQ", "NE", "GT", "GE", "LT","LE"};

//! Impression d'une instruction avec 2 arguments
/*!
 * \param instr l'instruction à imprimer
 */
void print_two(Instruction instr){
	if(instr.instr_generic._immediate == 0){
		if(instr.instr_generic._indexed == 0 || (instr.instr_generic._indexed == 1 && instr.instr_indexed._offset == 0)){
			printf("R%02d, @0x%04x", instr.instr_absolute._regcond, instr.instr_absolute._address);
		}
		else{
			printf("R%02d, %d[R%02d]", instr.instr_indexed._regcond, instr.instr_indexed._offset, instr.instr_indexed._rindex);
		}
	}
	else{
		printf("R%02d, #%d", instr.instr_immediate._regcond, instr.instr_immediate._value);
	}
}

//! Impression d'une instruction avec 1 arguments
/*!
 * \param instr l'instruction à imprimer
 */
 void print_onenimm(Instruction instr){
 	if(instr.instr_generic._immediate == 0){
		if(instr.instr_generic._indexed == 0){
			printf("@0x%04x", instr.instr_absolute._address);
		}
		else{
			printf("%d[R%02d]", instr.instr_indexed._offset, instr.instr_indexed._rindex);
		}
	}
 }

//! Impression d'une instruction sous forme lisible (désassemblage)
/*!
 * \param instr l'instruction à imprimer
 * \param addr son adresse
 */
void print_instruction(Instruction instr, unsigned addr){
	switch(instr.instr_generic._cop){
		case ILLOP: 
			printf("%s", cop_names[0]);
			break;
		case NOP:
			printf("%s", cop_names[1]);
			break;
		case LOAD:
			printf("%s ", cop_names[2]);
			print_two(instr);
			break;
		case STORE:
			printf("%s ", cop_names[3]);
			if(instr.instr_generic._immediate == 0){
				if(instr.instr_generic._indexed == 0){
					printf("R%02d, @0x%04x", instr.instr_absolute._regcond, instr.instr_absolute._address);
				}
				else{
					printf("R%02d, %d[R%02d]", instr.instr_indexed._regcond, instr.instr_indexed._offset, instr.instr_indexed._rindex);
				}
			}
			break;
		case ADD:
			printf("%s ", cop_names[4]);
			print_two(instr);
			break;
		case SUB:
			printf("%s ", cop_names[5]);
			print_two(instr);
			break;
		case BRANCH:
			printf("%s ", cop_names[6]);
			printf("%s, ", condition_names[instr.instr_generic._regcond]);
			print_onenimm(instr);
			break;
		case CALL:
			printf("%s ", cop_names[7]);
			printf("%s, ", condition_names[instr.instr_generic._regcond]);
			print_onenimm(instr);
			break;
		case RET:
			printf("%s", cop_names[8]);
			break;
		case PUSH:
			printf("%s ", cop_names[9]);
			if(instr.instr_generic._immediate == 0){
				if(instr.instr_generic._indexed == 0){
					printf("@0x%04x", instr.instr_absolute._address);
				}
				else{
					printf("%d[R%02d]", instr.instr_indexed._offset, instr.instr_indexed._rindex);
				}
			}
			else{
				printf("#%d", instr.instr_immediate._value);
			}
			break;
		case POP:
			printf("%s ", cop_names[10]);
			print_onenimm(instr);
			break;
		case HALT:
			printf("%s", cop_names[11]);
			break;
	}
}