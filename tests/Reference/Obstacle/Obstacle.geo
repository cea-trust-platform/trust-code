Mailler dom 
{
	Pave Entree
		{
		Origine 0. 0.
		Nombre_de_Noeuds 6 23
		Longueurs 0.1 0.22
		}
		{
		bord Inlet 	X = 0. 0. <= Y <= 0.22 
		bord Upper 	Y = 0.22 0. <= X <= 0.1 
		bord Lower 	Y = 0. 0. <= X <= 0.1 
		bord Square 	X = 0.1 0.1 <= Y <= 0.12
		} ,

	Pave Haut
		{
		Origine 0.1 0.12
		Nombre_de_Noeuds 2 11
		Longueurs 0.02 0.1
		}
		{
		bord Upper 	Y = 0.22 0.1 <= X <= 0.12 
		bord Square 	Y = 0.12 0.1 <= X <= 0.12
		} ,

	Pave Bas
		{
		Origine 0.1 0.
		Nombre_de_Noeuds 2 11
		Longueurs 0.02 0.1
		}
		{
		bord Lower 	Y = 0. 0.1 <= X <= 0.12 
		bord Square 	Y = 0.1 0.1 <= X <= 0.12
		} ,

	Pave Sortie
		{
		Origine 0.12 0.
		Nombre_de_Noeuds 41 23
		Longueurs 0.8 0.22
		}
		{
		bord Outlet 	X = 0.92 0. <= Y <= 0.22 
		bord Lower 	Y = 0. 0.12 <= X <= 0.92 
		bord Upper 	Y = 0.22 0.12 <= X <= 0.92 
		bord Square 	X = 0.12 0.1 <= Y <= 0.12
		}
}
