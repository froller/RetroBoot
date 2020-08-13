	.MODEL TINY
	.CODE
BootAddr	EQU 07c00h
RelocAddr	EQU 00600h
RelocSeg	EQU 0060h
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
; Релокация
	mov SI, BootAddr
	mov DI, RelocAddr
	mov CX, RelocSize
	cld
	repne
	movsw
; Старт на новом месте
	DB 0EAh ; jmp far
	DW OFFSET ReStart, RelocSeg

ReStart:
	cli
	mov AX, 0060h
	mov SS, AX
	mov DS, AX
	mov ES, AX
	xor AX, AX
	mov SP, AX
	sti

	; Сохранение загрузочного диска
	mov [Drive], DL

	; Приветствие
	push BP
	mov BP, OFFSET Msg
	call Print
	pop BP

	; Загрузка Stage 1.5
	call LoadStage15

	; Печатаем Starting...
	push BP
	mov BP, OFFSET Starting
	call Print
	pop BP

	; Запуск Stage 1.5
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
; TODO проверить контрольную сумму
	DB 0EAh	; jmp far
	DW 0100h, Stage15Seg

; **************************************************************
;
; Печатает строку из ES:BP
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
; Загружает оставшийся код
;
; **************************************************************
LoadStage15	PROC
	push BP
	mov BP, SP

	; Печатаем Loading...
	push BP
	mov BP, OFFSET Loading
	call Print
	pop BP

	; Грузим 62 сектора по адресу 0140:0000
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

	; Сохраняем результат для потомков
	push AX

	; Обработка ошибок загрузки
	push BP
	jc LoadFail

	; Удачная загрузка
	mov BP, OFFSET Loaded
	jmp PrintResult

	; Неудачная загрузка
LoadFail:
	mov BP, OFFSET Failed
PrintResult:
	call Print
	pop BP

	; Возвращаем ошибку загрузки в AX
	pop AX

	mov SP, BP
	pop BP
	ret
LoadStage15	ENDP

; **************************************************************
;
; Данные
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
