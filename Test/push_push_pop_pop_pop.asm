//-----------------
// Instructions
//-----------------
        TEXT 30

        // Programme principal
main    EQU *
        PUSH @op1
        PUSH @op2
        CALL NC, @subprog
        HALT 
        NOP 
        NOP 
        NOP 
        NOP

        // Sous-programme
subprog EQU *
loop    BRANCH LE, @return
        POP @op2
        BRANCH NC, @loop
return  RET
        
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
