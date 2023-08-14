MAIN:   mov @r3 ,LENGTH
LOOP:   jmp L1
        mcro .data
        ;
                sub @r1, @r4
                bne END
        endmcro
        prn -5
        bne LOOP
        data
L1:     inc K
        bne LOOP
END:    stop
srulik
STR:    .string “abcdef”
LENGTH: .data 6,-9,15 
K:      .data 22