	.MODEL TINY
	.CODE
BootAddr	EQU 07c00h
RelocOffs	EQU 0h
RelocSeg	EQU 0600h
RelocSize	EQU 100h
Stage15Seg	EQU 2000h
Stage15Offs	EQU 100h

_TEXT	SEGMENT PUBLIC USE16
	ASSUME CS:_TEXT,DS:_TEXT
	ORG 0
Start:  cli
	xor AX, AX
	mov SS, AX
	mov SP, BootAddr
	push AX
	pop DS
	push AX
	pop ES
	sti
; Relcating myself
	mov SI, BootAddr
	mov AX, RelocSeg
	mov ES, AX
	mov DI, RelocOffs
	mov CX, RelocSize
	cld
	repne
	movsw
	xor AX, AX
	mov ES, AX
; Continuing execution of relocated code
	DB 0EAh ; jmp far
	DW OFFSET ReStart, RelocSeg

ReStart:
	cli
	mov AX, RelocSeg
	mov SS, AX
	mov DS, AX
	mov ES, AX
	xor AX, AX
	mov SP, AX
	sti

	; Saving the number of boot drive
	mov [Drive], DL

	; Greeting
	push BP
	mov BP, OFFSET Msg
	call Print
	pop BP

	; Loading Stage 1.5
	call LoadStage15

	; Print "Starting..."
	push BP
	mov BP, OFFSET Starting
	call Print
	pop BP

	; Starting Stage 1.5
	cli
	mov AX, Stage15Seg
	push AX
	pop SS
	mov SP, 0FFFFh
	push AX
	pop DS
	push AX
	pop ES
	sti
; TODO Ensure cheksum is ok
	DB 0EAh	; jmp far
	DW Stage15Offs, Stage15Seg

; **************************************************************
;
; Prints lint pointed by ES:BP
;
; **************************************************************
Print	PROC
	push AX
	push BX
	push SI
	mov BH, 0
	mov SI, 0
	mov AH, 0Eh
PrnChr:	mov AL, BYTE PTR [BP+SI]
	cmp AL, 0
	jz PrintEOS
	int 10h
	inc SI
	jmp PrnChr
PrintEOS:
	pop SI
	pop BX
	pop AX
	ret
Print	ENDP


; **************************************************************
;
; Loads the rest of code
;
; **************************************************************
LoadStage15	PROC
	push BP
	mov BP, SP

	; Print "Loading..."
	push BP
	mov BP, OFFSET Loading
	call Print
	pop BP

	; Loading 62 sectors to 0140:0000
	push ES
	mov AX, Stage15Seg;
	mov ES, AX
	mov BX, 100h
	mov DL, [Drive]
	mov DH, 0
	mov CH, 0
	mov CL, 2
	mov AH, 2
	mov AL, 48
	int 13h
	pop ES

	; Saving error code
	push AX

	; Error handling
	push BP
	jc LoadFail

	; Successful loading
	mov BP, OFFSET Loaded
	jmp PrintResult

	; Failed loading
LoadFail:
	mov BP, OFFSET Failed
PrintResult:
	call Print
	pop BP

	; Returning error code to AX
	pop AX

	mov SP, BP
	pop BP
	ret
LoadStage15	ENDP

; **************************************************************
;
; Data
;
; **************************************************************
Drive	DB 0
Msg	DB "RetroBoot v", VERSION, 0Dh, 0Ah, 0
Loading	DB "Loading stage 1.5... ", 0
Starting	DB "Starting stage 1.5... ", 0
Loaded	DB "ok", 0Dh, 0Ah, 0
Failed  DB "failed", 0Dh, 0Ah, 0
_TEXT	ENDS
	END
