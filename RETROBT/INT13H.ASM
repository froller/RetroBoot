	P286
	.MODEL TINY

_TEXT	SEGMENT PUBLIC 'CODE'
	PUBLIC _biosInt13h, _fakeMBR, _fakePartition, _fakeDrive

_biosInt13h:	dq 0
_fakePartition:	db 0
_fakeMBR:	db "This is fake MBR stub. If you see this the actual MBR was not loaded yet."
		db 439 dup(0)
_fakeDrive:	db 80h
_callCount:	db 1

	PUBLIC _int13h
_int13h	PROC FAR
	pusha
	; cafe
	cmp	AX, 0CAFEh
	jz	@cafe
	; disarm
	cmp	AX, 0DEADh
	jz	@disarm
	; mbr
	cmp	AH, 02h	; READ SECTORS
	jz	@mbr
@chain:
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
	cmp	DL, byte ptr _fakeDrive	; drive
	jnz	@chain
	cmp	AL, 1	; one sector
	jnz	@chain
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
	xor	AX, AX
	push	AX
	pop	ES
	push	CS
	pop	DS
	mov	DI, 4Ch
	mov	SI, OFFSET _biosInt13h
	mov	CX, 2
	rep
	movsw

	popa
	xor	AH, AH
	clc
	iret


_int13h	ENDP
_TEXT	ENDS
	END
