//-----------------
// Instructions
//-----------------
        TEXT 30

        // Programme principal
main    EQU *
        PUSH @op1
        PUSH @op2
        POP @op2
        POP @op1
        HALT 
        NOP 
        NOP 
        NOP 
        NOP
        
        END
        
//-----------------
// Données et pile
//-----------------
        DATA 30
        
        WORD 0
result  WORD 0
op1     WORD 20
op2     WORD 5
        
        END
