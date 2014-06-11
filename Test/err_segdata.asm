        TEXT 30

main EQU *
        LOAD R00, #1234
        LOAD R01, 3[R00]

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
        