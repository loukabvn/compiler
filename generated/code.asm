; Programme assembleur ASIPRO

	const ax,debut
	jmp ax

; Déclaration des constantes
:div_by_zero
@string "Error : division by zero\n"
:newline
@string "\n"
:true
@string "true"
:false
@string "false"

; Gestions des erreurs
:err_div_zero
	const ax,div_by_zero
	callprintfs ax
	end

:main
	const ax,0
	push ax

	pop ax
	const bx,var:a
	storew ax,bx

	const ax,10
	push ax

	cp ax,sp
	callprintfd ax
	pop ax
	const ax,newline
	callprintfs ax
	ret

:test
	const bx,var:b
	loadw ax,bx
	push ax

	cp ax,sp
	callprintfd ax
	pop ax
	ret

:debut

; Préparation de la pile
	const bp,pile
	const sp,pile
	const ax,2
	sub sp,ax
	const ax,main
	call ax

	end

; Zone des variables :
; La zone de pile
:pile
@int 0
