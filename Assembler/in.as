MAIN:   mov r3 ,LENGTH
.entry GG
.extern ADHD
LOOP:   jmp L1(#-1,r6)
        mcr m1 
            sub r1, r4
            bne END
        endmcr
        mcr pp
            bne GG
            jmp L1
        endmcr
        prn #-5
        bne LOOP(r4,r3)
        pp
        m1
L1:     inc r3
        bne LOOP(K,STR)
GG:     bne ADHD 
END:    stop
STR:    .string "abcdef"
LENGTH: .data 6,-9,15
K:      .data 22
