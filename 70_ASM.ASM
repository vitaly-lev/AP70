;	<C> <Lion>
;
;	70_asm.asm
;
;	Programm for communication IBM-PC <-> IBM
;
;	Varios ASM Functions
;
;	930423	V1.0
;
;
	.model	small
	dosseg
;
;--- include files
;
;
	public	_SetCurPos
	public	_GetCurPos
	public	_GetVidConfig
	public	_PutChar
;
	.data

	include	demo.inc
	include bios.inc

	.data

vconfig VIDCONFIG <>           ; Global video configuration structure
;
	.code
;
;
;* SetCurPos - Sets cursor position.
;*
;* Shows:   BIOS Interrupt - 10h, Function 2 (Set Cursor Position)
;*
;* Uses:    vconfig - Video configuration structure, declared in the
;*          DEMO.INC include file. The structure must first be
;*          initialized by calling the GetVidConfig procedure.
;*
;* Params:  row - Target row
;*          col - Target column
;*
;* Return:  None
_SetCurPos PROC c\
	 row:WORD, col:WORD

        mov     dx, col                 ; DL = column
        mov     dh, BYTE PTR row        ; DH = row
        mov     ah, 2                   ; Function 2
        mov     bh, vconfig.dpage       ; Current page
        int     10h                     ; Set Cursor Position
        ret

_SetCurPos ENDP


;* GetCurPos - Gets current cursor position.
;*
;* Shows:   BIOS Interrupt - 10h, Function 3 (Get Cursor Position)
;*
;* Uses:    vconfig - Video configuration structure, declared in the
;*          DEMO.INC include file. The structure must first be
;*          initialized by calling the GetVidConfig procedure.
;*
;* Params:  None
;*
;* Return:  Short integer with high byte = row, low byte = column

_GetCurPos PROC c

        mov     ah, 3                   ; Function 3
        mov     bh, vconfig.dpage
        int     10h                     ; Get Cursor Position
        mov     ax, dx                  ; Return cursor row/column
        ret

_GetCurPos ENDP

;* GetVidConfig - Determines current video configuration and initializes
;* the vconfig structure.
;*
;* Shows:   BIOS Interrupt - 10h, Function 0 (Set Video Mode)
;*                           10h, Function 0Fh (Get Video Mode)
;*                           10h, Function 1Ah (Get or Set Display
;*                                Combination Code)
;*
;* Uses:    vconfig - Video configuration structure, declared in the
;*          DEMO.INC include file.
;*
;* Params:  None
;*
;* Return:  None

_GetVidConfig PROC c

        mov     ax, 1A00h               ; Get video info for VGA
        int     10h
chkVGA:
        cmp     al, 1Ah                 ; Is VGA or MCGA present?
        jne     chkEGA                  ; No?  Then check for EGA

        cmp     bl, 2                   ; If VGA exists as secondary adapter,
        je      isCGA                   ;   check for CGA and mono as primary
        jb      isMONO
        cmp     bl, 5                   ; If EGA is primary, do normal
        jbe     chkEGA                  ;   EGA checking
chkMCGA:
        mov     vconfig.adapter, MCGA   ; Yes?  Assume MCGA
        mov     vconfig.display, COLOR
        cmp     bl, 8                   ; Correct assumption?
        ja      gotmode                 ; Yes?  Continue
isVGA:
        mov     vconfig.adapter, VGA    ; Assume it's VGA color
        je      gotmode                 ; Yes?  Continue
        mov     vconfig.display, MONO   ; No?  Must be VGA mono
        jmp     gotmode                 ; Finished with VGA, so jump
chkEGA:
        mov     ah, 12h                 ; Call EGA status function
        mov     bl, 10h
        sub     cx, cx                  ; Clear status bits
        int     10h
        jcxz    chkCGA                  ; If CX is unchanged, not EGA
isEGA:
        mov     vconfig.adapter, EGA    ; Set structure fields for EGA
        mov     vconfig.display, MONO   ; Assume EGA mono
        or      bh, bh                  ; Correct assumption?
        jnz     gotmode                 ; Yes?  Continue
        mov     vconfig.display, COLOR  ; No?  Must be EGA color
        jmp     gotmode                 ; Finished with EGA, so jump
chkCGA:
        int     11h                     ; Get equipment list
        and     al, 30h                 ; If bits 4-5 set, monochrome
        cmp     al, 30h                 ; Monochrome text mode?
        je      isMONO                  ; Yes?  Continue
isCGA:
        mov     vconfig.adapter, CGA    ; No?  Must be CGA
        mov     vconfig.display, COLOR
        jmp     gotmode
isMONO:
        mov     vconfig.adapter, MDA    ; Set MONO
        mov     vconfig.display, MONO
gotmode:
        mov     ah, 0Fh
        int     10h                     ; Get current mode
        mov     vconfig.mode, al        ; Record mode
        mov     vconfig.dpage, bh       ;   and current page
        mov     al, vconfig.display     ; Multiply display value
        cbw                             ;   (which is either 0 or 1)
        mov     bx, 800h                ;   by 800h, then add to 0B000h
        mul     bx                      ;   for segment address of
        add     ax, 0B000h              ;   video buffer
        add     ah, vconfig.dpage       ; Adding display page gives
        mov     vconfig.sgmnt, ax       ;   address of current page

        sub     ax, ax
        mov     es, ax
        mov     al, es:[44Ah]           ; Get number of display cols
        mov     vconfig.cols, al        ; Store in structure
        mov     vconfig.rows, 24        ; Assume bottom row # = 24
        cmp     vconfig.adapter, EGA    ; EGA or VGA?
        jl      exit                    ; No?  Exit
        mov     ax, 1130h               ; Yes?  Request character info
        sub     bh, bh                  ; Set BH to valid value
        push    bp                      ; BP will change, so save it
        int     10h                     ; Get number of rows/screen
        mov     vconfig.rows, dl        ; Keep in structure
        pop     bp                      ; Restore BP
exit:
        ret

_GetVidConfig ENDP

_PutChar proc c chr:byte

	@PutChar chr
		
	ret

_PutChar endp

	end


