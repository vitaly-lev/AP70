;
;
;	Vitaly Lev

;	<C> <Lion>	
;
;	70_async.c
;
;	Programm for communication IBM-XT <-> IBM
;
;	Timer service Functions
;
;	930608	V1.0
;
;!!! _timer : channel timer's number = TNUMBER (70_def.h)
;
	.model	small
	dosseg
;
	.data

      	public	_timer_no
	public	_timerins,_timerrem
	public	cdsg
	public 	_timer_int
	public	_timer
	public	_dostime
	public	_dosdate
;
old_t_vct	label	dword		; old timer vector contents
old_bx		dw	0		; base
old_es		dw	0		; segment
timer_active	db	0		; timer active flag
tick		db	18		; clock tick count
_timer_no  	dw	0		; number of timers in table
_timer		dd	00		; pointer to first timer
		dd	00		; number of timer = NTIMER*channel (70_def.h)
		dd	00
;
	.code
;
; Data in code segment
;
cdsg	dw	0

;
;---
;
_timerins proc near

	push	bp
	push	es			; save es and ds
	push	ds

	cmp	timer_active,1		; already installed?
	je	tin9			; yes-go return

; Initialize
	mov	timer_active,1		; set timer active flag
	mov	cdsg,ds			; save data segment

; Save current interrupt vector contents then install our own handler.

	mov	ah,35h			; get vector request
	mov	al,1ch			; we want the timer one
	int	21h			; int to DOS
	mov	old_bx,bx		; save vector contents
	mov	old_es,es
	mov	dx,offset _timer_int	; point to our handler
	mov	ax,cs			; be sure ds is right
	mov	ds,ax
	mov	ah,25h			; set vector request
	mov	al,1ch			; we want the timer one
	int	21h			; int to DOS

; Return

tin9:
	pop    ds			; restore data segment
	pop    es			; restore es
	pop    bp
	ret

_timerins endp

;
;---
;

_timerrem proc near

	push	bp
	cmp	timer_active,1		; installed?
	jne	tir9	 		; no-go return

; Recover old vector contents 
; and put them back.

	push	ds	 		; save ds register
	mov	ah,25h			; set vector request
	mov	al,1ch			; we want the timer one
	mov	dx,old_bx		; recover old vector
	mov	ds,old_es
	int	21h	 		; int to DOS
	pop	ds	 		; restore ds register
	mov	timer_active,0		; reset timer active flag

; Return

tir9:
	pop	bp
	ret

_timerrem endp

;
;---
;

_timer_int proc far

; Initialize
	push	ds	       		; save registers
	mov	ds,cdsg	       		; enable data addressability

; Re-enable interrupts
;####	sti

; When a clock interrupt occurs, decrement the second ticker.  If the
; ticker goes to zero, count down the second-based timers.

t1:
	dec	tick	     		; decrement ticker
	jnz	tiret			; go on if not zero

; Pass through second-based timers and decrement any that are not already
; zero.  Zero can mean either 'elapsed' or 'inactive'.

ti1:
	mov	tick,18      		; reset ticker

	push	bx	     		; save registers
	push	cx
	mov	bx,offset _timer	; point to list of timers
	mov	cx,_timer_no		; do for each timer

ti3:
	cmp	word ptr [bx],0 	; active?
	je	ti33				; no-go on
	dec	word ptr [bx]		; yes-count it down
	jmp	ti4

ti33:
	cmp	word ptr [bx+2],0
	je	ti4
	dec	word ptr [bx+2]
	dec	word ptr [bx]
ti4:
	add	bx,4			; advance to next timer
	loop	ti3			; and try again

	pop	cx			; restore registers
	pop	bx			; and go on

tiret:
	cli				;  disable interrupts before call
	pushf				; this is to simulate an int instruction
	call	old_t_vct		; inter-segment indirect call

	pop	ds
	sti				; re-enable interrupts
	iret				; and return from interrupt

_timer_int endp

;        --------------------------------------------------------------------
;        void dostime(&hour,&min,&sec,&ths);
;
;        int hour;         0-23  military time
;        int min;	   0-59
;        int sec;          0-59
;        int ths;          0-99
;        --------------------------------------------------------------------
;cProc    dostime,<PUBLIC>,<di>
;
;         parmW    Hour
;         parmW    Minutes
;         parmW    Seconds
;         parmW    Thousandths
;
;cBegin

_dostime proc near
	push	bp
	mov	bp,sp
	push	di

	mov	ah,2cH			;DOS 'Get Time' function
	int	21H			;Call DOS

	mov	al,ch
	xor	ah,ah
	mov	di, word ptr [bp+6]	;Address of Hour
	mov	[di],ax

	mov	bl,cl
	xor	bh,bh
	mov	di, word ptr [bp+8]	;Address of Minutes
	mov	[di],bx

	mov	cl,dh
	xor	ch,ch
	mov	di, word ptr [bp+10]	;Address of Seconds
	mov	[di],cx

	xor	dh,dh
	mov	di, word ptr [bp+12]	;Address of Thousandths
	mov	[di],dx

;cEnd
	pop	di
	pop	bp
	ret
_dostime endp



;
;        --------------------------------------------------------------------
;        void dosdate(&month,&mday,&year,&wday);
;
;        int month;	   1-12
;        int mday;	   1-31	 day of month
;        int year;	   1980-2099
;        int wday;	   0-6   day of week (0=Sun,6=Sat)
;        --------------------------------------------------------------------
;cProc    dosdate,<PUBLIC>,<di>
;
;         parmW    Month
;         parmW    Mday
;         parmW    Year
;         parmW    Wday
;
;cBegin
_dosdate proc near
	push	bp
	mov	bp,sp
	push	di

	mov	ah,2aH			;DOS 'Get Date' function
	int	21H			;Call DOS

	mov	bl,dh
	xor	bh,bh
	mov	di, word ptr [bp+6]	;Address of Month
	mov	[di],bx

	xor	dh,dh
	mov	di, word ptr [bp+8]	;Address of Day-of-Month
	mov	[di],dx

	mov	di, word ptr [bp+10]	;Address of Year
	mov	[di],cx

	xor	ah,ah
	mov	di, word ptr [bp+12]	;Address of Day-of-Week
	mov	[di],ax
;cEnd
	pop	di
	pop	bp
	ret
_dosdate endp

	end
