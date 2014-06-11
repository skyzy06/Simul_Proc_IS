        TEXT 30

main	EQU *
		ADD R00, #12345
		CALL NC, 6[R00]
		NOP
		NOP

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
        