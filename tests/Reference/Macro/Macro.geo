// create a mesh using macros
#define LX 0.1
#define LY 0.22

#define NUMBER_OF_ELEMENTS_PER_METER 100
#define NX (INT(LX * NUMBER_OF_ELEMENTS_PER_METER))
#define NY (INT(LY * NUMBER_OF_ELEMENTS_PER_METER))

Mailler dom {
	Pave Entree {
		Origine 0. 0.
		Longueurs LX LY
		Nombre_de_Noeuds NX  NY
	}
	{
		bord Left   X = 0.   0. <= Y <= LY 
		bord Right  X = LX   0. <= Y <= LY 
		bord Bottom Y = 0.   0. <= X <= LX
		bord Top    Y = LY   0. <= X <= LX
	}
}
