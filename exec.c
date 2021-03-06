/*!
 * \file exec.c
 * \brief Exécution d'une instruction.
 */

#include <stdio.h>
#include "machine.h"
#include "error.h"

//! Ensemble des instructions avec opérations

bool load(Machine *pmach, Instruction instr, unsigned addr);
bool store(Machine *pmach, Instruction instr, unsigned addr);
bool add(Machine *pmach, Instruction instr, unsigned addr);
bool sub(Machine *pmach, Instruction instr, unsigned addr);
bool branch(Machine *pmach, Instruction instr, unsigned addr);
bool call(Machine *pmach, Instruction instr, unsigned addr);
bool ret(Machine *pmach, Instruction instr, unsigned addr);
bool push(Machine *pmach, Instruction instr, unsigned addr);
bool pop(Machine *pmach, Instruction instr, unsigned addr);

//! Décodage et exécution d'une instruction

/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return faux après l'exécution de \c HALT ; vrai sinon
 */
bool decode_execute(Machine *pmach, Instruction instr) {
    unsigned addr = pmach->_pc;
    Code_Op op = instr.instr_generic._cop;
    switch (op) {
        case ILLOP: error(ERR_ILLEGAL, addr);
        case NOP: return true; // ne fait rien
        case LOAD: return load(pmach, instr, addr);
        case STORE: return store(pmach, instr, addr);
        case ADD: return add(pmach, instr, addr);
        case SUB: return sub(pmach, instr, addr);
        case BRANCH: return branch(pmach, instr, addr);
        case CALL: return call(pmach, instr, addr);
        case RET: return ret(pmach, instr, addr);
        case PUSH: return push(pmach, instr, addr);
        case POP: return pop(pmach, instr, addr);
        case HALT: return false; // arrêt normal de l'exécution
        default: error(ERR_UNKNOWN, addr);
    }
}

/*!
 * \param pmach la machine en cours
 * \param instr l'instruction en cours
 * \return l'adresse réelle d'une adresse indexée ou absolue
 */
unsigned int get_addr(Machine *pmach, Instruction instr) {
    if (instr.instr_indexed._indexed) { // si indexée
        return (pmach->_registers[instr.instr_indexed._rindex] + instr.instr_indexed._offset); // Addr = (RX) + Offset
    } else { // si absolue
        return instr.instr_absolute._address; // Addr = Abs
    }
}

//! Met à jour le code condition selon la valeur de registre

/*!
 * \param pmach la machine en cours
 * \param reg le numéro de registre
 */
void refresh_code_cond(Machine *pmach, unsigned int reg) {
    if (reg < 0) {
        pmach->_cc = CC_N; // cc négatif
    } else if (reg > 0) {
        pmach->_cc = CC_P; // cc positif
    } else {
        pmach->_cc = CC_Z; // cc nul
    }
}

//! Vérifie qu'on n'a pas d'erreur de segmentation dans la pile de donnée

/*!
 * \param pmach la machine en cours
 * \param adresse adresse (absolue ou indexée) de l'instruction en cours
 * \param addr adresse réelle
 */
void check_data_addr(Machine *pmach, unsigned int adresse, unsigned addr) {
    if (adresse > pmach->_datasize)
        error(ERR_SEGDATA, addr);
}

//! Contrôle que l'instruction n'est pas immédiate

/*!
 * \param instr l'instruction en cours
 * \param addr l'adresse de l'instruction
 */
void check_not_immediate(Instruction instr, unsigned addr) {
    if (instr.instr_generic._immediate) {
        error(ERR_IMMEDIATE, addr);
    }
}

//! Contrôle si la condition de branchement C est respectée

/*!
 * \param pmach la machime en cours
 * \param instr l'instruction courante
 * \param addr l'adresse de l'instruction
 * \return si la condition est respectée
 */
bool condition_respected(Machine *pmach, Instruction instr, unsigned addr) {
    switch (instr.instr_generic._regcond) {
        case NC: return true; // Pas de condition, toujours vraie
        case EQ: return (pmach->_cc == CC_Z); // Egal à 0, CC == Z
        case NE: return (pmach->_cc != CC_Z); // Différent de z, CC != Z
        case GT: return (pmach->_cc == CC_P); // Strictement positif, CC > Z
        case GE: return (pmach->_cc == CC_P || pmach->_cc == CC_Z); // Positif ou nul, CC >= Z
        case LT: return (pmach->_cc == CC_N); // Strictement négatif, CC < Z
        case LE: return (pmach->_cc == CC_N || pmach->_cc == CC_Z); // Négatif ou nul, cc <= Z
        default: error(ERR_CONDITION, addr); // condition illégale
    }
}

//! Contrôle que le sommet de pile est valide

/*!
 * \param pmach la machine en cours
 * \param addr adresse de l'instruction
 */
void check_stack_pointer(Machine *pmach, unsigned addr) {
    if (pmach->_sp < pmach->_dataend || pmach->_sp >= pmach->_datasize) { // dataend>SP>datasize
        error(ERR_SEGSTACK, addr);
    }
}

//! Décodage et éxecution de l'instruction LOAD

/*!
 * Accepte adressage immédiat, absolu et indexé
 * 
 * \param pmach machine en cours d'éxecution
 * \param instr instruction en cours
 * \param addr addresse de l'instruction en cours
 * \return true
 */
bool load(Machine *pmach, Instruction instr, unsigned addr) {
    if (instr.instr_generic._immediate) { // si I = 1, immédiat
        pmach->_registers[instr.instr_generic._regcond] = instr.instr_immediate._value; // R <- Val
    } else { // sinon I = 0, absolu ou indexé
        unsigned int adresse = get_addr(pmach, instr); // on récupère l'adresse réelle
        check_data_addr(pmach, adresse, addr); // on contrôle qu'on ne dépasse pas la taille de la pile
        pmach->_registers[instr.instr_generic._regcond] = pmach->_data[adresse]; // R <- Data[Addr]
    }
    //on met à jour le code condition
    refresh_code_cond(pmach, pmach->_registers[instr.instr_generic._regcond]);
    return true;
}

//! Décodage et éxecution de l'instruction STORE

/*!
 * Accepte adressage absolu et indexé
 * 
 * \param pmach machine en cours d'éxecution
 * \param instr instruction en cours
 * \param addr addresse de l'instruction en cours
 * \return true
 */
bool store(Machine *pmach, Instruction instr, unsigned addr) {
    check_not_immediate(instr, addr); // on contrôle que l'instruction n'est pas immédiate
    unsigned int adresse = get_addr(pmach, instr); // on récupére l'adresse réelle
    check_data_addr(pmach, adresse, addr); // on contrôle qu'on est dans la pile
    pmach->_data[adresse] = pmach->_registers[instr.instr_generic._regcond]; // Data[Addr] <- R
    return true;
}

//! Décodage et éxecution de l'instruction ADD

/*!
 * Accepte adressage immédiat, absolu et indexé
 * 
 * \param pmach machine en cours d'éxecution
 * \param instr instruction en cours
 * \param addr addresse de l'instruction en cours
 * \return true
 */
bool add(Machine *pmach, Instruction instr, unsigned addr) {
    if (instr.instr_generic._immediate) { // si I = 1, immédiat
        pmach->_registers[instr.instr_generic._regcond] += instr.instr_immediate._value; // R <- R + Value
    } else { // sinon I = 0, absolue ou indexée
        unsigned int adresse = get_addr(pmach, instr); // on récupére l'adresse réelle
        check_data_addr(pmach, adresse, addr); // on contrôle qu'on est dans la pile
        pmach->_registers[instr.instr_generic._regcond] += pmach->_data[adresse]; // R <- R + Data[Addr]
    }
    refresh_code_cond(pmach, pmach->_registers[instr.instr_generic._regcond]); // on met à jour le code condition
    return true;
}

//! Décodage et éxecution de l'instruction SUB

/*!
 * Accepte addressage immédiat, absolu et indexé
 * 
 * \param pmach machine en cours d'éxecution
 * \param instr instruction en cours
 * \param addr addresse de l'instruction en cours
 * \return true
 */
bool sub(Machine *pmach, Instruction instr, unsigned addr) {
    if (instr.instr_generic._immediate) { // si I = 1, immédiat
        pmach->_registers[instr.instr_generic._regcond] -= instr.instr_immediate._value; // R <- R + Value
    } else { // sinon I = 0, absolue ou indexée
        unsigned int adresse = get_addr(pmach, instr); // on récupére l'adresse réelle
        check_data_addr(pmach, adresse, addr); // on contrôle qu'on est dans la pile
        pmach->_registers[instr.instr_generic._regcond] -= pmach->_data[adresse]; // R <- R + Data[Addr]
    }
    refresh_code_cond(pmach, pmach->_registers[instr.instr_generic._regcond]); // on met à jour le code condition
    return true;
}

//! Décodage et éxecution de l'instruction BRANCH

/*!
 * Accepte adressage absolu et indexé
 * 
 * \param pmach machine en cours d'éxecution
 * \param instr instruction en cours
 * \param addr addresse de l'instruction en cours
 * \return true
 */
bool branch(Machine *pmach, Instruction instr, unsigned addr) {
    check_not_immediate(instr, addr); // on contrôle que l'adresse n'est pas immédiate
    if (condition_respected(pmach, instr, addr)) { // on vérifie que la condition de branchement est vraie
        unsigned int adresse = get_addr(pmach, instr); // on récupère l'adresse de l'instruction
        pmach->_pc = adresse; // PC <- Addr
    }
    return true;
}

//! Décodage et éxecution de l'instruction CALL

/*!
 * Accepté adressage absolu et indexé
 * 
 * \param pmach machine en cours d'éxecution
 * \param instr instruction en cours
 * \param addr addresse de l'instruction en cours
 * \return true
 */
bool call(Machine *pmach, Instruction instr, unsigned addr) {
    check_not_immediate(instr, addr); // on contrôle que l'adresse n'est pas immédiate
    check_stack_pointer(pmach, addr); // on contrôle que le SP est valide (dataend<SP<=datasize-1)
    if (condition_respected(pmach, instr, addr)) { // on vérifie que la condition de branchement est vraie
        pmach->_data[pmach->_sp--] = pmach->_pc; // Data[SP] <- PC puis SP <- SP -1
        unsigned int adresse = get_addr(pmach, instr); // on récupère l'adresse de l'instruction
        pmach->_pc = adresse; // PC <- Addr
    }
    return true;
}

//! Décodage et éxecution de l'instruction RET

/*!
 * Accepte adressage immédiat, absolu et indexé
 * 
 * \param pmach machine en cours d'éxecution
 * \param instr instruction en cours
 * \param addr addresse de l'instruction en cours
 * \return true
 */
bool ret(Machine *pmach, Instruction instr, unsigned addr) {
    check_stack_pointer(pmach, addr); // on contrôle que le SP est valide (dataend<SP<=datasize-1)
    pmach->_pc = pmach->_data[++pmach->_sp]; // SP <- SP +1 puis PC <- Data[SP]
    return true;
}

//! Décodage et éxecution de l'instruction PUSH

/*!
 * Accepte adressage immédiat, absolu et indexé
 * 
 * \param pmach machine en cours d'éxecution
 * \param instr instruction en cours
 * \param addr addresse de l'instruction en cours
 * \return true
 */
bool push(Machine *pmach, Instruction instr, unsigned addr) {
    check_stack_pointer(pmach, addr); // on contrôle que le SP est valide (dataend<SP<=datasize-1)
    if (instr.instr_generic._immediate) { // si I = 1, instruction immédiate
        pmach->_data[pmach->_sp--] = instr.instr_immediate._value; // Data[SP] <- Value puis SP <- SP -1
    } else { // si I = 0, instruction absolue ou indexée
        unsigned int adresse = get_addr(pmach, instr); // on récupère l'adresse de l'instruction
        check_data_addr(pmach, adresse, addr); // on contrôle qu'on reste dans la pile
        pmach->_data[pmach->_sp--] = pmach->_data[adresse]; // Data[SP] <- Data[Addr] puis SP <- SP -1
    }
    return true;
}

//! Décodage et éxecution de l'instruction POP

/*! 
 * Accepte adressage absolu et indexé
 * 
 * \param pmach machine en cours d'éxecution
 * \param instr instruction en cours
 * \param addr addresse de l'instruction en cours
 * \return true
 */
bool pop(Machine *pmach, Instruction instr, unsigned addr) {
    check_not_immediate(instr, addr); // on contrôle que l'adresse n'est pas immédiate
    unsigned int adresse = get_addr(pmach, instr); // on récupère l'adresse de l'instruction
    check_data_addr(pmach, adresse, addr); // on contrôle qu'on reste dans la pile
    check_stack_pointer(pmach, addr); // on contrôle que le SP est valide (dataend<SP<=datasize-1)
    pmach->_data[adresse] = pmach->_data[++pmach->_sp]; // SP <- SP +1 puis Data[Addr] <- Data[SP]
    return true;
}

//! Trace de l'exécution

/*!
 * On écrit l'adresse et l'instruction sous forme lisible.
 *
 * \param msg le message de trace
 * \param pmach la machine en cours d'exécution
 * \param instr l'instruction à exécuter
 * \param addr son adresse
 */
void trace(const char *msg, Machine *pmach, Instruction instr, unsigned addr) {
    printf("TRACE: %s: 0x%04x: ", msg, addr);
    print_instruction(instr, addr);
    printf("\n");
}