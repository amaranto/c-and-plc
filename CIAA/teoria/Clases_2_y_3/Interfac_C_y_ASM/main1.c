/* Este ejemplo realiza la resta entre dos enteros empleando
complemento a dos.
El main() prepara las variables y se las pasa a una funci�n
en assembler que realiza la operaci�n */

extern int resta(int x, int y);

int	a,b;
int	resul_rest;

void SystemInit (void)
{
}

int main(void) {

a = 12;
b = 7;
	resul_rest = resta(a,b);
}




