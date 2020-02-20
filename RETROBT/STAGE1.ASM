	.MODEL TINY
	.CODE
BootAddr	EQU 07c00h
RelocAddr	EQU 01000h
RelocSeg	EQU 0100h
RelocSize	EQU 100h
Stage15Seg	EQU 0140h

_Text	SEGMENT PUBLIC USE16
	ASSUME CS:_Text,DS:_Text
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
	cld
; ��������
	mov SI, BootAddr
	mov DI, RelocAddr
	mov CX, RelocSize
	repne
	movsw
; ���� �� ����� ����
	DB 0EAh ; jmp far
	DW OFFSET ReStart, RelocSeg

ReStart:
	push CS
	pop DS
	push CS
	pop ES

	; ���࠭���� ����㧮筮�� ��᪠
	mov [Drive], DL

	; �ਢ���⢨�
	push BP
	mov BP, OFFSET Msg
	call Print
	pop BP

	; ����㧪� Stage 1.5
	call LoadStage15

	; ���⠥� Starting...
	push BP
	mov BP, OFFSET Starting
	call Print
	pop BP

	; ����� Stage 1.5
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
	DB 0EAh	; jmp far
	DW 0100h, Stage15Seg

	cli
	hlt

; **************************************************************
;
; ���⠥� ��ப� �� ES:BP
;
; **************************************************************
Print	PROC
	push AX
	push BX
	push SI
	mov BH, 0
	mov SI, 0
	mov AH, 0Eh
PrnChr:	mov AL, BYTE PTR [ES:BP+SI]
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
; ����㦠�� ��⠢訩�� ���
;
; **************************************************************
LoadStage15	PROC
	push BP
	mov BP, SP

	; ���⠥� Loading...
	push BP
	mov BP, OFFSET Loading
	call Print
	pop BP

	; ��㧨� 62 ᥪ�� �� ����� 0140:0000
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

	; ���࠭塞 १���� ��� ��⮬���
	push AX

	; ��ࠡ�⪠ �訡�� ����㧪�
	push BP
	jc LoadFail

	; ���筠� ����㧪�
	mov BP, OFFSET Loaded
	jmp PrintResult

	; ��㤠筠� ����㧪�
LoadFail:
	mov BP, OFFSET Failed
PrintResult:
	call Print
	pop BP

	; �����頥� �訡�� ����㧪� � AX
	pop AX

	mov SP, BP
	pop BP
	ret
LoadStage15	ENDP

; **************************************************************
;
; �����
;
; **************************************************************

Drive	DB 0
Msg	DB "RetroBoot v0.0", 0Dh, 0Ah, 0
Loading	DB "Loading stage 1.5... ", 0
Starting	DB "Starting stage 1.5... ", 0
Loaded	DB "ok", 0Dh, 0Ah, 0
Failed  DB "failed", 0Dh, 0Ah, 0
_Text	ENDS
	END