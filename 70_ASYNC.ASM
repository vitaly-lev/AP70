;
;	Vitaly Lev
;	<C> <Lion>	
;
;	70_async.c
;
;	Programm for communication IBM-XT <-> IBM
;
;	Interrupt service Functions
;
;	920228	V1.0 Alfa1
;
;	Functions:	
;
;!!! not switch stack for number channel > 1
;
ic_imr	equ	21h			;i/o address of interrupt mask reg
;
	.model	small
	dosseg
;
;
; Public declarations
;
	public	_drvr_ins
	public	_drvr_rem
	public	_clr_int
	public	_set_int
	public	_port_add
	public	_hard_int
	public	_chan_no
	public	_linestat
	public	_iir_add
	public	_baudrate
	public	_drvr_active1
	public	_drvr_active2
	public	_port1
	public	_modem_st
;
;
; Extern declarations
;
	extrn	cdsg:word
	extrn	_ascode:near
;
	.data
;
;
;
_baudrate	db	0
_hard_int	db	0			;hardware interrupt #
_hard_int1	db	0
_hard_int2	db	0
_port_add 	dw	0			;base address of sio hardware
_chan_no 	dw	0			;channel # being used
;
	db	100 dup(?)
stack	equ	$
;
_drvr_active1	db	0
_drvr_active2	db	0
_linestat	db	?
		db	?
old_vct1 	dw	?
		dw	?
old_vct2 	dw	?
		dw	?
_iir_add 	dw	?
ic_imr_sav	db	?
_port1		db	0
_port2		db	0
;
	.code
;
	page
;PUBLIC---------------------------------------------------------------------
;
;	_drvr_ins - install async interrupt handler and set up async card
;
;---------------------------------------------------------------------------

_drvr_ins	proc	near
		
	push	bp
	mov	bp,sp

	push	bx
	push	dx
	push	es
;
	mov	ax,_chan_no		; get channel number
	mov	_port_add,03f8h		; assume port 1
	mov	_hard_int,0ch
	mov	_hard_int1,0ch
	cmp	al,0			; is it 1?
	je	do1			; yes-go on
	mov	_port_add,02f8h		; no-setup for port 2
	mov	_hard_int,0bh
	mov	_hard_int2,0bh
do1:
	in	al,ic_imr
	jmp	short $+2
	mov	ic_imr_sav,al
	mov	al,0ffh
	out	ic_imr,al

	mov	ah,35h			;ask for current vector contents
	mov	al,_hard_int		;specify which vector
	int	21h

	mov	ax,_chan_no
	cmp	al,0
	je	vct0
;
	mov	_drvr_active2,1
	mov	old_vct2,bx		;save current vector values
	mov	old_vct2+2,es
      	mov	dx,offset _async2
	jmp	init1

vct0: 
	mov	_drvr_active1,1
	mov	old_vct1,bx		;save current vector values
	mov	old_vct1+2,es
	mov	dx,offset _async1

init1:	mov	ah,25h			;install new vector values
	mov	al,_hard_int
	push	ds				;save current ds
	push	cs
	pop	ds				;ds = cs
	int	21h
	pop	ds				;restore ds

	mov	ax,_port_add
	inc	ax
	inc	ax
	mov	_iir_add,ax
;
;      initialize rs-232 driver to character format and baud rate
;
act1:	mov	al,_baudrate
;	mov	cl,5	   		;adjust for format
;	shl	al,cl			;move  into correct field
;	or	al,00011010b		;set up 7 bits, parity even, 1 stop bit
	xor	ah,ah			;rom bios control code
	mov	dx,_chan_no		;point to port number
	int	14h	   		;rom bios interrupt
;
;	enable rcvr and xmtr interrupts
;
	mov	dx,_port_add
	add	dx,1			;point to interrupt enable register
	mov	al,00000011b
	out	dx,al
	jmp	$+2
	nop
;
;	clear pending interrupts
;
	inc	dx		;point to interrupt identification reg
	in	al,dx	       	;clear xmtr interrupt by reading iir
	jmp	$+2
	nop
	sub	dx,2	       	;point to receive data buffer
	in	al,dx	       	;clear rcvr interrupt by reading rxb
	jmp	$+2
	nop
;
;	setup dtr, rts & interrupt hardware enable
;
	add	dx,4	   	;point to modem control register
	mov	al,00001011b 	;enable dtr, rts and interrupt (out 2)
	out	dx,al
	jmp	$+2
	nop
;
;      enable interrupt in controller.
;      if the port address is 2f8, use interrupt 3; otherwise use int 4.
;
	mov	al,ic_imr_sav
	mov	ah,0ffh - 00010000b	;assume int 4
	cmp	_port_add,02f8h		;com2?
	jne	p1			;no-go on
	mov	ah,0ffh - 00001000b	;switch to int 3
p1:		
	and	al,ah			;enable irq
	out	ic_imr,al
	jmp	$+2
	nop
;
done:
	pop	es
	pop	dx
	pop	bx
	pop	bp
	ret
;
_drvr_ins endp

	page
;PUBLIC---------------------------------------------------------------------
;
;	_drvr_rem - remove async interrupt handler
;
;--------------------------------------------------------------------------

_drvr_rem	proc	near
		push	bp
		mov	bp,sp
;
		push	dx
;
		cmp	_drvr_active1,1
		je	rem1
		cmp	_drvr_active2,1
		je	rem2
		jmp	rdone
;
;	reinstall system interrupt handler
;
rem1:
		push	ds	
		mov	dx,old_vct1
		mov	ax,old_vct1+2
		push	ax
		mov	al,_hard_int1
		mov	_drvr_active1,0
		jmp	rd1
;
rem2:		push	ds
		mov	dx,old_vct2
		mov	ax,old_vct2+2
		push	ax
		mov	al,_hard_int2
		mov	_drvr_active2,0
rd1:
		mov	ah,25h
		pop	ds
		int	21h
;
		pop	ds		    ;restore ds

rdone:
		pop	dx
		pop	bp
		ret
;
_drvr_rem	endp

		page
;LOCAL----------------------------------------------------------------------
;
;	_async - async interrupt handler
;
;---------------------------------------------------------------------------

_async1	proc	far
		push	bp	; 14
		push	ax
		push	bx
		push	cx
		push	dx
		push	si
		push	di
		push	ds
		push	es	; 140
;
		mov	di,ss	;
		mov	si,sp	; 148
;
		mov	ax,cs:cdsg ;152
		mov	ss,ax
		mov	es,ax
		mov	ds,ax	   ;158
;
		mov	ax,offset dgroup:stack ;162
		mov	sp,ax
		mov	bp,ax		;166
;
		push	di		
		push	si	  	;194
;
		cld			;196
;
		mov	_port1,1
		call	_ascode		;216 + 100? + 170 = 486
		mov	_port1,0
;
		pop	si		;12
		pop	di		;24
		mov	ss,di		;
		mov	sp,si		;28
;
		pop	es		;
		pop	ds
		pop	di
		pop	si
		pop	dx
		pop	cx
		pop	bx
		pop	ax
		pop	bp		;136
		sti			;138
		iret			;170
_async1 endp

_async2 proc	far
		push	bp	; 14
		push	ax
		push	bx
		push	cx
		push	dx
		push	si
		push	di
		push	ds
		push	es	; 140
;
		mov	di,ss	;
		mov	si,sp	; 148
;
		mov	ax,cs:cdsg ;152
		mov	ss,ax
		mov	es,ax
		mov	ds,ax	   ;158
;
		mov	ax,offset dgroup:stack ;162
		mov	sp,ax
		mov	bp,ax		;166
;
		push	di		
		push	si	  	;194
;
		cld			;196
;
		mov	_port2,1
		call	_ascode		;216 + 100? + 170 = 486
		mov	_port2,0
;
		pop	si		;12
		pop	di		;24
		mov	ss,di		;
		mov	sp,si		;28
;
		pop	es		;
		pop	ds
		pop	di
		pop	si
		pop	dx
		pop	cx
		pop	bx
		pop	ax
		pop	bp		;136
		sti			;138
		iret			;170
_async2 endp


;	set current modem status
;
_modem_st proc	near USES ax dx 

	mov	ax,_chan_no		; get channel number
	mov	dx,03f8h		; assume port 1
	cmp	al,0			; is it 1?
	je	st1			; yes-go on
	mov	dx,02f8h		; no-setup for port 2
st1:
	add	dx,6
	in	al,dx	       	
	mov	_linestat,al

	dec	dx
	in	al,dx	       	
	mov	_linestat+1,al
	
	ret

_modem_st endp

		page
;PUBLIC---------------------------------------------------------------------
;
;	_clr_int - do a cli from C
;
;---------------------------------------------------------------------------

_clr_int 	proc	near
		cli
		ret
_clr_int 	endp

;PUBLIC---------------------------------------------------------------------
;
;	_set_int - do an sti from C
;
;---------------------------------------------------------------------------

_set_int 	proc	near
		sti
		ret
_set_int 	endp

	end
