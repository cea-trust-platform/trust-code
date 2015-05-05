// Choix de la forme dans:
// http://www.aerospaceweb.org/question/aerodynamics/q0231.shtml
// 0 : Carre  		(Figure 22)	Cx 2.05
// 1 : Triangle1	(Figure 17)	Cx 1.55
// 2 : Plaque 		(Figure 18)	Cx 1.98
// 3 : Triangle2	(Figure 19)	Cx 2.00
// 4 : Cercle 		(Figure 12)	Cx 1.17
// 5 : DemiCercle	(Figure 14)	Cx 1.16
Forme = 5;

lc = 0.02;
// Pas du maillage loin
lc1 = lc * 8;
// Pas du maillage pres
lc2 = lc / 2;

// Diametre de la forme
D = 0.14 ;
E = D ;
param = 1;
H = param * 10 * D ;
X = param * 5 * D ;
L = param * 10 * D + X + E; 
Point(1) = {0,0,0,lc1};
Point(2) = {L,0,0,lc1};
Point(3) = {L,H,0,lc1};
Point(4) = {0,H,0,lc1};

Point(5) = {X,0,0,lc2};
Point(8) = {X+E,0,0,lc2};

Line(2) = {1,5};
Line(5) = {8,2};
Line(6) = {2,3};
Line(7) = {3,4};
Line(8) = {4,1};

// Carre 
If(Forme==0)
   Point(6) = {X,D/2,0,lc2};
   Point(7) = {X+E,D/2,0,lc2};
   Line(1) = {5,6};
   Line(3) = {6,7};
   Line(4) = {7,8};
   Line Loop(5) = {2,1,3,4,5,6,7,8};
   Physical Line("Forme") = {1,3,4};
   Physical Line("Axe") = {2,5};
EndIf

// Prisme 17
If(Forme==1)
   Point(6) = {X+E/2,D/2,0,lc2};
   Point(7) = {X+E/2,0,0,lc2};
   Line(1) = {5,6};
   Line(3) = {6,7};
   Line(4) = {7,8};
   Line Loop(5) = {2,1,3,4,5,6,7,8};
   Physical Line("Forme") = {1,3};
   Physical Line("Axe") = {2,4,5};
EndIf

// Plaque
If(Forme==2)
   eps=E*0.01;
   Point(6) = {X+E/2,0,0,lc2};
   Point(7) = {X+E/2,D/2,0,lc2};
   Point(9) = {X+E/2+eps,D/2,0,lc2};
   Point(10) = {X+E/2+eps,0,0,lc2};
   Line(3) = {5,6};
   Line(1) = {6,7};
   Line(9) = {7,9};
   Line(4) = {9,10};
   Line(10) = {10,8};
   Line Loop(5) = {2,1,3,9,4,10,5,6,7,8};
   Physical Line("Forme") = {1,9,4};
   Physical Line("Axe") = {2,3,10,5};
EndIf

// Prisme 19
If(Forme==3)
   Point(6) = {X,D/2,0,lc2};
   Point(7) = {X+E/2,0,0,lc2};
   Line(1) = {5,6};
   Line(3) = {6,7};
   Line(4) = {7,8};
   Line Loop(5) = {2,1,3,4,5,6,7,8};
   Physical Line("Forme") = {1,3};
   Physical Line("Axe") = {2,4,5};
EndIf

// Cercle
If(Forme==4)
   Point(6) = {X+D/2,0,0,lc2};
   Point(7) = {X+D/2,D/2,0,lc2};
   Circle(1) = {5,6,7};
   Circle(3) = {7,6,8};
   Line Loop(5) = {2,1,3,5,6,7,8};
   Physical Line("Forme") = {1,3};
   Physical Line("Axe") = {2,5};
EndIf

// 1/2 Cercle
If (Forme==5)
   Point(6) = {X+D/2,0,0,lc2};
   Point(7) = {X+D/2,D/2,0,lc2};
   Circle(1) = {5,6,7};
   Line(3) = {7,6};
   Line(4) = {6,8};
   Line Loop(5) = {2,1,3,4,5,6,7,8};
   Physical Line("Forme") = {1,3};
   Physical Line("Axe") = {2,4,5};
EndIf

Plane Surface(9) = {5};

// Frontieres
Physical Line("Sortie") = {6};
Physical Line("Haut") = {7};
Physical Line("Entree") = {8};

// Domaine
Physical Surface("domaine") = {9};
