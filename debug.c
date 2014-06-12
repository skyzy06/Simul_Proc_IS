/*!
 * \file debug.h
 * \brief Fonctions de mise au point interactive.
 */
#include <stdbool.h>
#include <stdio.h>

#include "debug.h"
#include "machine.h"

//! Dialogue de mise au point interactive pour l'instruction courante.
/*!
 * Cette fonction gère le dialogue pour l'option \c -d (debug). Dans ce mode,
 * elle est invoquée après l'exécution de chaque instruction.  Elle affiche le
 * menu de mise au point et on exécute le choix de l'utilisateur. Si cette
 * fonction retourne faux, on abandonne le mode de mise au point interactive
 * pour les instructions suivantes et jusqu'à la fin du programme.
 * 
 * \param mach la machine/programme en cours de simulation
 * \return vrai si l'on doit continuer en mode debug, faux sinon
 */
bool debug_ask(Machine *pmach)
{
	char c1, c2;

	while(1)
	{
		printf("DEBUG?");
		c2 = '\n';
		for(int i = 0; (c1 = getchar()) != '\n'; i++)
			if(i < 1)
				c2 = c1;
		switch (c2){
			case 'h':
				printf("Available commands:\n");
				printf("       h       help\n");
				printf("       c       continue(exit interactive debug mode)\n");
				printf("       s       step by step(next instruction)\n");
				printf("       RET     step by step(next instruction)\n");
				printf("       r       print registers\n");
				printf("       d       print data memory\n");
				printf("       t       print text (program) memory\n");
				printf("       p       print text (program) memory\n");
				printf("       m       print registers and data memory\n");
				break;
			case 'c':
				return false;
			case 's':
			case '\n':
				return true;
			case 'r':
				print_cpu(pmach);
				break;
			case 'd':
				print_data(pmach);
				break;
			case 't':
			case 'p':
				print_program(pmach);
				break;
			case 'm':
				print_cpu(pmach);
				print_data(pmach);
				break;
		}
	}
	return true;
}
