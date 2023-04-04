MAIN:   mov r3 ,LENGTH
.entry GG
.extern IMAGINE
LOOP:   jmp L1(#-1,MONSTER)
        mcr m1 
            sub r1, r4
            bne KPOP
            mov r1, r2
            cmp r1, r3
        endmcr
.extern KPOP
        mcr pp
            bne GG
            inc r4
            dec r5
            jmp L1
        endmcr
        prn #-5
        pp
.extern MONSTER
        bne LOOP(r4,r3)
        m1
L1:     inc r3
        bne LOOP(K,STR)
GG:     bne IMAGINE 
END:    stop
STR:    .string "abcdef"
LENGTH: .data 6,-9,15
K:      .data 22
