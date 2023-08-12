STACKLEN EQU 216
ACKTIMEOUTCOUNTER EQU 4100               ; ~32,800 cycle delay to account for delayed 7th memcard ACK

org 0x0BFC00000
.bios

.import PSX-BIOS.ROM,0,0xBF4            ; import original BIOS up until injection point

jal 0BFC65CB8h                          ; Replace nop in parseConfig with payload entrypoint

.import PSX-BIOS.ROM,0xBF8,0x650C0      ; import up until garbage memory in charset region

sendMemCardData:
        ; Preserve stack pointer, allocate stack space
        addiu   r29,r29,-STACKLEN       ; Jump by increments of 8 according to gcc compiler?
                                        ;       (152 from gcc + 64 from register saving)
        sw      r31,STACKLEN-4(r29)     ; Always 4 off of max stack size allocation
        sw      r30,STACKLEN-8(r29)
        move    r30,r29

        ; Save temp registers since the compiler didn't expect them to be modified with our payload
        ; r2-15, r24, r25
        sw      r2,STACKLEN-12(r30)
        sw      r3,STACKLEN-16(r30)
        sw      r4,STACKLEN-20(r30)
        sw      r5,STACKLEN-24(r30)
        sw      r6,STACKLEN-28(r30)
        sw      r7,STACKLEN-32(r30)
        sw      r8,STACKLEN-36(r30)
        sw      r9,STACKLEN-40(r30)
        sw      r10,STACKLEN-44(r30)
        sw      r11,STACKLEN-48(r30)
        sw      r12,STACKLEN-52(r30)
        sw      r13,STACKLEN-56(r30)
        sw      r14,STACKLEN-60(r30)
        sw      r15,STACKLEN-64(r30)
        sw      r24,STACKLEN-68(r30)
        sw      r25,STACKLEN-72(r30)

        jal     stopPad
        nop

        ; Load I/O Address constants on upper temp registers
        li      r25,0x1F801040          ; 0x1F801040 - JOY_DATA
                                        ; 0x1F801044 - JOY_STAT
                                        ; 0x1F80104A - JOY_CTRL
                                        ; 0x1F801048 - JOY_MODE
                                        ; 0x1F80104E - JOY_BAUD
        li      r12,0x0020b075          ; 0x0020B075 - bootStringAddress

        ; i = [r30 + 24]
        ; gameIDBuf = [r30 + 28] (grows upwards from there)

        ; Game ID Command bytes:
        ;     0  0x81 - select memory card
        ;     1  0x21 - command to begin Game ID transfer
        ;     2  0x00 - reserved
        ;     3  strlen of Game ID string
        ;     4+ Game ID string with null terminator byte at the end (0x00)

        li      r8,0x81
        sb      r8,28(r30)      ; gameIDBuf[0] = 0x81   (801FFCD4 on stack)
        li      r8,0x21
        sb      r8,29(r30)      ; gameIDBuf[1] = 0x21
        sb      zero,30(r30)    ; gameIDBuf[2] = 0x00

        ; Move bootStringAddress forward if there are spaces in SYSTEM.CNF file
        li      r10,10
BOOTSTRINGSPACECORRECTION:
        lbu     r9,0(r12)       ; r9 = bootStringAddress[0]
        li      r8,0x63         ; r8 = 'c'
        beq     r8,r9,BOOTSTRINGCORRECT         ; if bootStringAddres[0] != 'c'...
        nop
        subi    r10,r10,1
        bgtz    r10,BOOTSTRINGSPACECORRECTION
        addiu   r12,r12,1                       ; ... move it forward a byte

BOOTSTRINGCORRECT:
        sw      zero,24(r30)    ; i = 0                 (801FFCD0)
        li      r8,0            ; r8 = i (avoiding multiple loads by only using r8 for i, be careful!)
        b       WHILENOTSEMICOLON
        nop

IDSTRINGBUILDER:
        ; append next character in bootstring to gameIDBuf[4] and on

        addu    r10,r12,r8      ; 
        lbu     r10,0(r10)      ; r10 = bootStringAddress[i]
        addiu   r9,r8,32        ; r9 = i+4+28
        addu    r11,r30,r9      ; r11 = address of r30+i+4+28 (gameIDBuf[i+4] = [r30+28+i+4])
        sb      r10,0(r11)      ; gameIDBuf[i+4] = bootStringAddress[i]

        addiu   r8,r8,1         ;
        sw      r8,24(r30)      ; i++

WHILENOTSEMICOLON:
        ; while (bootStringAddress[i] != ';') conditional

        addu    r9,r12,r8                       ; r12 = 0x0020B077 (bootStringAddress)
        lbu     r9,0(r9)                        ; r9 = bootStringAddress[i]
        li      r10,0x3b                        ; r10 = ';'
        bne     r9,r10,IDSTRINGBUILDER          ; jump to IDSTRINGBUILDER if bootStringAddress[i] != ';'
        nop

        ; gameIDBuf[3] = i+3 (strlen for Game ID string) (3 = ';' + final num + 0x00 terminator)
        addiu   r9,r8,3
        sb      r9,31(r30)

        ; add ';' that stopped the while loop
        addiu   r9,r8,32        ; r9 = i+4+28
        addu    r9,r30,r9       ; r9 = address of r30+28 + i+4
        sb      r10,0(r9)       ; gameIDBuf[i+4] = ';'

        ; read number following ';'
        addu    r11,r12,r8      ; 
        lbu     r10,1(r11)      ; r10 = bootStringAddress[i+1]
        nop
        sb      r10,1(r9)       ; gameIDBuf[i+5] = bootStringAddress[i+1]

        ; add 0x00 null terminator to end of string
        li      r10,0           ; r10 = 0
        sb      r10,2(r9)       ; gameIDBuf[i+6] = 0x00

        ; store final command length in mem - i + 7 (4 header bytes + ';', final number, and 0x00 terminator)
        addiu   r9,r8,7
        sw      r9,STACKLEN-76(r30)


        ; call InitCard() and StartCard() to ensure memcard is active
        ; save r26 and r27 (kernel reserved; destroyed by initCard) and r25 (global destroyed by both)
        sw      r26,STACKLEN-80(r30)
        sw      r27,STACKLEN-84(r30)
        sw      r25,STACKLEN-132(r30)
        li      r4,1
        jal     initCard
        nop
        ; restore r26 and r27 in case they're needed before the next call
        lw      r26,STACKLEN-80(r30)
        lw      r27,STACKLEN-84(r30)
        jal     startCard
        ; ^ seems to destroy r26 as well so reload it one more time
        nop
        lw      r26,STACKLEN-80(r30)
        lw      r25,STACKLEN-132(r30)

        ; Throw in one more stopPad to improve changes of ID string not being mangled in transit
        ; Destroys r26 (and some temps we don't care about right now)
        jal     stopPad
        nop
        lw      r26,STACKLEN-80(r30)


        ; Set up registers for Joypad/Memory ports
        li      r8,0x000D               ;
        sh      r8,0x8(r25)             ; JOY_MODE = 0x000D (8-bit, no parity, 1x baud multiplier)
        li      r8,0x0088               ;
        sh      r8,0xE(r25)             ; JOY_BAUD = 0x0088 (~250kHz with 1x multiplier)
        li      r8,0x1003               ;
        sh      r8,0xA(r25)             ; JOY_CTRL = 0x1003 (TX Enabled, JOY1 Select, ACK Interrupt enabled)
        ; Delay after JOY1 Select as per documentation
        li      r8,170                  ; 510 cycle delay assuming each instruction takes 1 cycle
DELAYWHILE:
        subi    r8,r8,1
        bgtz    r8,DELAYWHILE
        nop


        ; Send Game ID command
        sw      zero,24(r30)            ; i = 0
        li      r8, 0                   ; r8 = i
        lw      r13,STACKLEN-76(r30)    ; r13 = commandLength
        nop
        subiu   r12,r13,1               ; Used for checking final byte to send
        b       CHECKCOMMANDLENGTH
        nop

; for (int i = 0; i < commandLength; i++)
SENDCOMMANDBYTE:
        ; Send command byte to JOY_DATA bus address
        addiu   r9,r30,28               ; r9 = r30+28 (gameIDBuf)
        addu    r9,r8,r9                ; r9 = r30+28 + i
        lbu     r4,0(r9)                ; r4 = gameIDBuf[i]

        ; Check if this is the last byte and set r5 appropriately
        beq     r8,r12,ISLASTBYTE       ; if i = commandLength-1
        nop
        li      r5,0
        b       NOTLASTBYTE
        nop
ISLASTBYTE:
        li      r5,1
NOTLASTBYTE:
        jal     sendByte                ; JOY_DATA[0] = r4
        nop

        ; continue loop to send next byte
        addiu   r8,r8,1                 ;
        sw      r8,24(r30)              ; i++

CHECKCOMMANDLENGTH:
        ; i < commandLength conditional in for loop
        .nocash                         ; work-around for no$psx assembler bug for setlt instruction
        setlt   r9,r8,r13
        .native
        bne     r9,zero,SENDCOMMANDBYTE ; send another byte if i < commandLength
        nop

FINALBITTIMEOUT:
ACKTIMEOUT:
        jal     startPad
        nop

        ; restore temp registers before returning
        lw      r2,STACKLEN-12(r30)
        lw      r3,STACKLEN-16(r30)
        lw      r4,STACKLEN-20(r30)
        lw      r5,STACKLEN-24(r30)
        lw      r6,STACKLEN-28(r30)
        lw      r7,STACKLEN-32(r30)
        lw      r8,STACKLEN-36(r30)
        lw      r9,STACKLEN-40(r30)
        lw      r10,STACKLEN-44(r30)
        lw      r11,STACKLEN-48(r30)
        lw      r12,STACKLEN-52(r30)
        lw      r13,STACKLEN-56(r30)
        lw      r14,STACKLEN-60(r30)
        lw      r15,STACKLEN-64(r30)
        lw      r24,STACKLEN-68(r30)
        lw      r25,STACKLEN-72(r30)

        ; return after restoring stack and frame pointers
        move    r29,r30
        lw      r31,STACKLEN-4(r29)
        lw      r30,STACKLEN-8(r29)
        addiu   r29,r29,STACKLEN
        jr      r31
        nop

; in: r4 - byte to send, r5 - isFinalByte boolean ---- destroys: r9, r10, r11
sendByte:
        sb      r4,0(r25)               ; JOY_DATA[0] = r4

        ; exitCriticalSection()
        ;li      r4,0x02
        ;syscall
        ;lw      r26,STACKLEN-80(r30)

        ; Skip timeout check if isFinalByte = 1
        bne     r5,zero,ACKEND
        nop

        ; start while loop to wait for an ACK interrupt
        li      r9,512                  ; for bitwise comparison to get the 10th bit (interrupt request)
        li      r11,ACKTIMEOUTCOUNTER
ACKWHILE:
        ; timeout if ACK is not received in a reasonable timeframe (will happen on no Memcard Pro in slot 1)
        subi    r11,r11,1
        beq     r11,zero,ACKTIMEOUT
        nop

        lw      r10,0x4(r25)              ; r10 = JOY_STAT value
        nop
        andi    r10,r10,512             ; r10 AND 0010 0000 0000 (512) to get 10th bit overall

        ; if not timed out, continue looping until ACK received
        bne     r10,r9,ACKWHILE
        nop


        ; handle ACK received
        ; reset for next ACK by setting JOY_CTRL bit 5
        lh      r9,0xA(r25)               ; r9 = JOY_CTRL value
        nop
        ori     r9,r9,16                ; r9 = JOY_CTRL OR 0001 0000 to only change required bit
        sh      r9,0xA(r25)               ; Write Acknowledge (5th bit) to JOY_CTRL to reset JOY_STAT bits
        jr      ra                      ; Return from SENDBYTE
        nop
ACKEND:
        ; wait for final data bit to be available
        li      r10,4
        li      r11,50000
WHILEFINALBIT:
        subi    r11,r11,1
        beq     r11,zero,FINALBITTIMEOUT
        nop
        lb      r9,0x4(r25)               ; r9 = JOY_STAT value
        nop
        andi    r9,r9,4                 ; Get 3rd bit, 0100 (Transfer received/finished)
        bne     r9,r10,WHILEFINALBIT
        nop

        jr      ra                      ; Return from SENDBYTE
        nop


initCard:
        addiu   r10,zero,0x00b0
        jr      r10
        addiu   r9,zero,0x004a

startCard:
        addiu   r10,zero,0x00b0
        jr      r10
        addiu   r9,zero,0x004b

startPad:
        addiu   r10,zero,0x00b0
        jr      r10
        addiu   r9,zero,0x0013

; Destroys r2,r3,r4,r5,r6,r8,r9,r10,r14,r15,r26
stopPad:
        addiu   r10,zero,0x00b0
        jr      r10
        addiu   r9,zero,0x0014

.align 800h                             ; zerofill to ..7FFh to align with original garbage mem's length
.import PSX-BIOS.ROM,0x66000,-1         ; Import remainder of original BIOS file