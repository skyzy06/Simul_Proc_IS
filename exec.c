/*!
 * \file exec.c
 * \brief Exécution d'une instruction.
 */

#include <stdio.h>
#include "machine.h"
#include "error.h"


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
        case NOP: return true;
        case LOAD: return load(pmach, instr, addr);
        case STORE: return store(pmach, instr, addr);
        case ADD: return add(pmach, instr, addr);
        case SUB: return sub(pmach, instr, addr);
        case BRANCH: return branch(pmach, instr, addr);
        case CALL: return call(pmach, instr, addr);
        case RET: return ret(pmach, instr, addr);
        case PUSH: return push(pmach, instr, addr);
        case POP: return pop(pmach, instr, addr);
        case HALT: return false;
        default: error(ERR_UNKNOWN, addr);
    }
}

/**
 * @param pmach la machine en cours
 * @param instr l'instruction en cours
 * @return l'adresse réelle d'une adresse indexée ou absolue
 */
unsigned int get_addr(Machine *pmach, Instruction instr) {
    if (instr.instr_indexed._indexed) { // si indexée
        return (pmach->_registers[instr.instr_indexed._rindex] + instr.instr_indexed._offset); // Addr = (RX) + Offset
    } else { // si absolue
        return instr.instr_absolute._address; // Addr = Abs
    }
}

/**
 * Met à jour le code condition selon la valeur de registre
 * @param pmach la machine en cours
 * @param reg le numéro de registre
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

/**
 * Décodage et éxecution de l'instruction LOAD
 * Accepte adressage immédiat, absolu et indexé
 * 
 * @param pmach machine en cours d'éxecution
 * @param instr instruction en cours
 * @param addr addresse de l'instruction en cours
 * @return true
 */
bool load(Machine *pmach, Instruction instr, unsigned addr) {
    if (instr.instr_generic._immediate) { // si I = 1, immédiat
        pmach->_registers[instr.instr_generic._regcond] = instr.instr_immediate._value; // R <- Val
    } else { // sinon I = 0, absolu ou indexé
        unsigned int adresse = get_addr(pmach, instr); // on récupère l'adresse réelle
        if (adresse > pmach->_datasize) { // on contrôle qu'on ne dépasse pas la taille de la pile
            error(ERR_SEGDATA, addr);
        }
        pmach->_registers[instr.instr_generic._regcond] = pmach->_data[adresse]; // R <- Data[Addr]
    }
    //on met à jour le code condition
    refresh_code_cond(pmach, pmach->_registers[instr.instr_generic._regcond]);
    return true;
}

/**
 * Décodage et éxecution de l'instruction STORE
 * Accepte adressage absolu et indexé
 * 
 * @param pmach machine en cours d'éxecution
 * @param instr instruction en cours
 * @param addr addresse de l'instruction en cours
 * @return true
 */
bool store(Machine *pmach, Instruction instr, unsigned addr) {
    return true;
}

/**
 * Décodage et éxecution de l'instruction ADD
 * Accepte l'adressage immédiat, absolu et indexé
 * 
 * @param pmach machine en cours d'éxecution
 * @param instr instruction en cours
 * @param addr addresse de l'instruction en cours
 * @return true
 */
bool add(Machine *pmach, Instruction instr, unsigned addr) {
    return true;
}

/**
 * Décodage et éxecution de l'instruction SUB
 * Accepte l'addressage immédiat, absolu et indexé
 * 
 * @param pmach machine en cours d'éxecution
 * @param instr instruction en cours
 * @param addr addresse de l'instruction en cours
 * @return true
 */
bool sub(Machine *pmach, Instruction instr, unsigned addr) {
    return true;
}

/**
 * Décodage et éxecution de l'instruction BRANCH
 * Accepte l'adressage absolu et indexé
 * 
 * @param pmach machine en cours d'éxecution
 * @param instr instruction en cours
 * @param addr addresse de l'instruction en cours
 * @return true
 */
bool branch(Machine *pmach, Instruction instr, unsigned addr) {
    return true;
}

/**
 * Décodage et éxecution de l'instruction CALL
 * Accepté l'adressage absolu et indexé
 * 
 * @param pmach machine en cours d'éxecution
 * @param instr instruction en cours
 * @param addr addresse de l'instruction en cours
 * @return true
 */
bool call(Machine *pmach, Instruction instr, unsigned addr) {
    return true;
}

/**
 * Décodage et éxecution de l'instruction RET
 * 
 * @param pmach machine en cours d'éxecution
 * @param instr instruction en cours
 * @param addr addresse de l'instruction en cours
 * @return true
 */
bool ret(Machine *pmach, Instruction instr, unsigned addr) {
    return true;
}

/**
 * Décodage et éxecution de l'instruction PUSH
 * Accepte l'adressage immédiat, absolu et indexé
 * 
 * @param pmach machine en cours d'éxecution
 * @param instr instruction en cours
 * @param addr addresse de l'instruction en cours
 * @return true
 */
bool push(Machine *pmach, Instruction instr, unsigned addr) {
    return true;
}

/**
 * Décodage et éxecution de l'instruction POP
 * Accepte adressage absolu et indexé
 * 
 * @param pmach machine en cours d'éxecution
 * @param instr instruction en cours
 * @param addr addresse de l'instruction en cours
 * @return true
 */
bool pop(Machine *pmach, Instruction instr, unsigned addr) {
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