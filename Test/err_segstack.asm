TEXT    30

main    EQU *
        CALL NC, @subprog

//sous porgramme
subprog EQU *
loop    BRANCH LE, @return
        PUSH #0
        BRANCH NC, @loop
return  RET
        END

//----------------
// Données et pile
//----------------
        DATA 30
        
        WORD 0
result  WORD 0
op1     WORD 20
op2     WORD 5
        
        END
  