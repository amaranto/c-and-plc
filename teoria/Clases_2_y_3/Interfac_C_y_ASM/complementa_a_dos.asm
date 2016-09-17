; Ejemplo de una suma entre registros
	
	AREA 	Code,CODE, READONLY	; Dar nombre a esta area de codigo

	EXPORT 	resta

resta

	mvn		r1,r1			;Luego de la operación, se debe devolver el contenido en R0
	add		r1,r1,#1
	add		r0,r0,r1
	
	BX		LR

	END
