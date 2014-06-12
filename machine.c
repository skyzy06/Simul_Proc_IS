#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "machine.h"
#include "exec.h"
#include "debug.h"

void perror_exit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

//! Chargement d'un programme
/*!
 * La machine est réinitialisée et ses segments de texte et de données sont
 * remplacés par ceux fournis en paramètre.
 *
 * \param pmach la machine en cours d'exécution
 * \param textsize taille utile du segment de texte
 * \param text le contenu du segment de texte
 * \param datasize taille utile du segment de données
 * \param data le contenu initial du segment de texte
 */
void load_program(Machine *pmach,
                  unsigned textsize, Instruction text[textsize],
                  unsigned datasize, Word data[datasize],  unsigned dataend)
{
    // textsize
    pmach->_textsize = textsize;

    // text
    pmach->_text = (Instruction *) malloc(textsize * sizeof(Instruction)); // + free ?
    for (int i = 0; i < textsize; ++i)
        pmach->_text[i] = text[i]; // + HALT ?

    // dataend
    pmach->_dataend = dataend;

    // datasize
    if(datasize - dataend >= MINSTACKSIZE)
        pmach->_datasize = datasize;
    else pmach->_datasize = datasize + MINSTACKSIZE;

    // data
    pmach->_data = (Word *) malloc(pmach->_datasize * sizeof(Word));
    for (int i = 0; i < datasize; ++i)
        pmach->_data[i] = data[i];

    // initialisation des registres
    for(int i = 0; i < NREGISTERS - 1; i++)
        pmach->_registers[i] = 0;

    // pc
    pmach->_pc = 0;

    // cc
    pmach->_cc = CC_U;

    // sp
    pmach->_sp = pmach->_datasize - 1;
}

//! Lecture d'un programme depuis un fichier binaire
/*!
 * Le fichier binaire a le format suivant :
 * 
 *    - 3 entiers non signés, la taille du segment de texte (\c textsize),
 *    celle du segment de données (\c datasize) et la première adresse libre de
 *    données (\c dataend) ;
 *
 *    - une suite de \c textsize entiers non signés représentant le contenu du
 *    segment de texte (les instructions) ;
 *
 *    - une suite de \c datasize entiers non signés représentant le contenu initial du
 *    segment de données.
 *
 * Tous les entiers font 32 bits et les adresses de chaque segment commencent à
 * 0. La fonction initialise complétement la machine.
 *
 * \param pmach la machine à simuler
 * \param programfile le nom du fichier binaire
 *
 */
void read_program(Machine *mach, const char *programfile)
{
    int fd = open(programfile, O_RDONLY);
    uint32_t textsize, datasize, dataend, *text, *data;

    // ouverture du fichier
    if(fd == -1)
        perror_exit("read_program.open");

    // textsize, datasize, dataend
    if(read(fd, &textsize, sizeof(uint32_t)) < sizeof(uint32_t)
        || read(fd, &datasize, sizeof(uint32_t)) < sizeof(uint32_t)
        || read(fd, &dataend, sizeof(uint32_t)) < sizeof(uint32_t))
    {
        fprintf(stderr, "could not read textsize, datasize or dataend from %s\n", programfile);
        exit(EXIT_FAILURE);
    }

    // text
    text = (uint32_t *) malloc(textsize * sizeof(uint32_t));
    if(read(fd, text, textsize * sizeof(uint32_t)) < textsize * sizeof(uint32_t))
    {
        fprintf(stderr, "could not read text from %s\n", programfile);
        exit(EXIT_FAILURE);
    }

    // data
    data = (uint32_t *) malloc(datasize * sizeof(uint32_t));
    if(read(fd, data, datasize * sizeof(uint32_t)) < datasize * sizeof(uint32_t))
    {
        fprintf(stderr, "could not read data from %s\n", programfile);
        exit(EXIT_FAILURE);
    }

    load_program(mach, textsize, (Instruction *) text, datasize, data, dataend); // casts ?
    free(text);
    free(data);
    close(fd);
}
 
//! Affichage du programme et des données
/*!
 * On affiche les instruction et les données en format hexadécimal, sous une
 * forme prête à être coupée-collée dans le simulateur.
 *
 * Pendant qu'on y est, on produit aussi un dump binaire dans le fichier
 * dump.bin. Le format de ce fichier est compatible avec l'option -b de
 * test_simul.
 *
 * \param pmach la machine en cours d'exécution
 */
void dump_memory(Machine *pmach)
{
    int fd = open("dump.bin", O_WRONLY|O_TRUNC|O_CREAT, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR);

    // ouverture du fichier
    if(fd == -1)
        perror_exit("dump_memory.open");

    // écriture des sizes
    if(write(fd, &pmach->_textsize, sizeof(uint32_t)) < sizeof(uint32_t)
        || write(fd, &pmach->_datasize, sizeof(uint32_t)) < sizeof(uint32_t)
        || write(fd, &pmach->_dataend, sizeof(uint32_t)) < sizeof(uint32_t))
    {
        fprintf(stderr, "could not write textsize, datasize or dataend in dump.bin\n");
        exit(EXIT_FAILURE);
    }

    // affichage / écriture des instructions
    puts("Instruction text[] = {");
    for (int i = 0; i < pmach->_textsize; ++i)
    {
        if(i % 4 == 0)
            putchar('\t');
        if(write(fd, &pmach->_text[i]._raw, sizeof(uint32_t)) < sizeof(uint32_t)
            || printf("0x%08x, ", pmach->_text[i]._raw) < 0)
        {
            fprintf(stderr, "could not write text in dump.bin\n");
            exit(EXIT_FAILURE);
        }
        if(i % 4 == 3)
            putchar('\n');
    }

    // affichage textsize
    printf("\n};\nunsigned textsize = %u\n\n", pmach->_textsize);

    // affichage / écriture des datas
    puts("Word data[] = {");
    for (int i = 0; i < pmach->_datasize; ++i)
    {
        if(i % 4 == 0)
            putchar('\t');
        if(write(fd, &pmach->_data[i], sizeof(uint32_t)) < sizeof(uint32_t)
            || printf("0x%08x, ", pmach->_data[i]) < 0)
        {
            fprintf(stderr, "could not write text in dump.bin\n");
            exit(EXIT_FAILURE);
        }
        if(i % 4 == 3)
            putchar('\n');
    }

    // affichage datasize, dataend
    printf("\n};\nunsigned datasize = %u\n", pmach->_datasize);
    printf("unsigned dataend = %u\n\n", pmach->_dataend);

    close(fd);
}

//! Affichage des instructions du programme
/*!
 * Les instructions sont affichées sous forme symbolique, précédées de leur adresse.
.* 
 * \param pmach la machine en cours d'exécution
 */
void print_program(Machine *pmach)
{
    printf("\n*** PROGRAM (size: %u) ***", pmach->_textsize);
    for(int i = 0; i < pmach->_textsize; i++)
    {
        printf("\n0x%04x: 0x%08x\t", i, pmach->_text[i]._raw);
        print_instruction(pmach->_text[i], 0);
    }
    puts("\n");
}

//! Affichage des données du programme
/*!
 * Les valeurs sont affichées en format hexadécimal et décimal.
 *
 * \param pmach la machine en cours d'exécution
 */
void print_data(Machine *pmach)
{
    printf("*** DATA (size %u, end = Ox%08x (%u)) ***", pmach->_datasize, pmach->_dataend, pmach->_dataend);
    for(int i = 0; i < pmach->_datasize; i++)
    {
        if(i % 3 == 0)
            putchar('\n');
        printf("0x%04x: 0x%08x %u\t", i, pmach->_data[i], pmach->_data[i]);
    }
    puts("\n");
}

//! Affichage des registres du CPU
/*!
 * Les registres généraux sont affichées en format hexadécimal et décimal.
 *
 * \param pmach la machine en cours d'exécution
 */
void print_cpu(Machine *pmach)
{
    char c;

    switch(pmach->_cc)
    {
        case 0:
            c = 'U';
            break;
        case 1:
            c = 'Z';
            break;
        case 2:
            c = 'P';
            break;
        case 3:
            c = 'N';
            break;
    }
    printf("\n*** CPU ***\nPC:  Ox%08x   CC: %c\n", pmach->_pc, c);
    for(int i = 0; i < NREGISTERS; i++)
    {
        if(i % 3 == 0)
            putchar('\n');
        printf("R%02i: 0x%08x %u\t", i, pmach->_registers[i], pmach->_registers[i]);
    }
    puts("\n");
}

//! Simulation
/*!
 * La boucle de simulation est très simple : recherche de l'instruction
 * suivante (pointée par le compteur ordinal \c _pc) puis décodage et exécution
 * de l'instruction.
 *
 * \param pmach la machine en cours d'exécution
 * \param debug mode de mise au point (pas à apas) ?
 */
void simul(Machine *pmach, bool debug)
{
    do
    {
        trace("Executing", pmach, pmach->_text[pmach->_pc], pmach->_pc);
        if(debug)
            debug = debug_ask(pmach);
    }
    while(decode_execute(pmach, pmach->_text[pmach->_pc++]));
}
