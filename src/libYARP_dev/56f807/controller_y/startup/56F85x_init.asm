
; -------------------------------------------------------
;
;	56836E_init.asm
; 	
;	Metrowerks, a Motorola Company
;	sample code
; 	
;	description:  main entry point to C code.
;                 setup runtime for C and call main
;
; -------------------------------------------------------

;===============================

; OMR mode bits

;===============================

NL_MODE                                                          EQU  $8000
CM_MODE                                                          EQU  $0100
XP_MODE                                                          EQU  $0080
R_MODE                                                           EQU  $0020
SA_MODE                                                          EQU  $0010



;===============================

; init

;===============================


        section startup

        XREF  F_stack_addr
        XREF  F_xROM_to_xRAM
        XREF  F_pROM_to_xRAM
        XREF  F_Ldata_size
        XREF  F_Ldata_ROM_addr
        XREF    F_Ldata_RAM_addr
        org   p:


        GLOBAL Finit_56800_

        SUBROUTINE "Finit_56800_",Finit_56800_,Finit_56800_END-Finit_56800_

Finit_56800_:

;
; setup the OMr with the values required by C
;
        bfset #NL_MODE,omr    ; ensure NL=1  (enables nsted DO loops)
        nop
        nop
        bfclr #(CM_MODE|XP_MODE|R_MODE|SA_MODE),omr               ; ensure CM=0  (optional for C)
                                                                            ; ensure XP=0 to enable harvard architecture
                                                                            ; ensure R=0  (required for C)
                                                                            ; ensure SA=0 (required for C)




; setup the m01 register for linear addressing

        move.w                                                   #-1,x0
        moveu.w                                                  x0,m01          ; set the m register to linear addressing

        moveu.w                                                  hws,la          ; clear the hardware stack
        moveu.w                                                  hws,la
        nop
        nop





CALLMAIN:                           ; initialize compiler environment

                                    ; initialize the Stack
        move.l #>>F_Lstack_addr,r0
        bftsth #$0001,r0
        bcc noinc
        adda #1,r0
noinc:
        tfra  r0,sp                                                     ; set stack pointer too
        move.w                                                   #0,r1
        nop
        move.w                                                   r1,x:(sp)
        adda  #1,sp


        jsr                                                      F__init_sections        ; clear BSS


; call main()

        move.w                                                   #0,y0         ; pass parameters to main()
        move.w                                                   #0,R2
        move.w                                                   #0,R3

        jsr                                                      Fmain         ; call the user program


;
;   The fflush calls where removed because they added code
;   growth in cases where the user is not using any debugger IO.
;   Users should now make these calls at the end of main if they use debugger IO
;
;       move.w                                                   #0,r2
;       jsr   Ffflush                                              ; flush file IO
;       jsr   Ffflush_console                                    ; flush console IO

;       end of program; halt CPU
        debughlt
        rts
Finit_56800_END:

        endsec
