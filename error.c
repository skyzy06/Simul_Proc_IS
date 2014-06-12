#include <stdlib.h>
#include <stdio.h>

#include "error.h"

//! Affichage d'une erreur et fin du simulateur
/*!
 * \note Toutes les erreurs étant fatales on ne revient jamais de cette
 * fonction. L'attribut \a noreturn est une extension (non standard) de GNU C
 * qui indique ce fait.
 * 
 * \param err code de l'erreur
 * \param addr adresse de l'erreur
 */


void error(Error err, unsigned addr)
{
	switch (err){
		case ERR_NOERROR:
			printf("Pas d'erreur at 0x%x\n",addr);
			exit(EXIT_FAILURE);
		case ERR_UNKNOWN:
			printf("Instruction inconnue at 0x%x\n",addr);
			exit(EXIT_FAILURE);
		case ERR_ILLEGAL:
			printf("Condition illégale at 0x%x\n",addr);
			exit(EXIT_FAILURE);
		case ERR_IMMEDIATE:
			printf("Valeur immédiate interdite 0x%x\n",addr);
			exit(EXIT_FAILURE);
		case ERR_SEGTEXT:
			printf("Violation de taille du segment de texte at 0x%x\n",addr);
			exit(EXIT_FAILURE);
		case ERR_SEGDATA:
			printf("Violation de taille du segment de données at 0x%x\n",addr);
			exit(EXIT_FAILURE);
		case ERR_SEGSTACK:
			printf("Violation de taille du segment de pile at 0x%x\n",addr);
			exit(EXIT_FAILURE);
		default:
			printf("Condition illégale at 0x%x\n", addr);
			exit(EXIT_FAILURE);
	}
}

//! Affichage d'un avertissement
/*!
 * \param warn code de l'avertissement
 * \param addr adresse de l'erreur
 */
void warning(Warning warn, unsigned addr)
{
	printf("WARNING: HALT reached at address 0x%x\n",addr);
}
