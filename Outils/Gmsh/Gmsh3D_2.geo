lc = 0.02;
Point(1) = {0,0,0,lc};
Point(2) = {0.1,0,0,lc};
Point(3) = {0.1,0,0.1,lc};
Point(4) = {0,0,0.1,lc};
Point(5) = {0,0.1,0,lc};
Point(6) = {0.1,0.1,0,lc};
Point(7) = {0.1,0.1,0.1,lc};
Point(8) = {0,0.1,0.1,lc};
Line(1) = {1,2};
Line(2) = {2,3};
Line(3) = {3,4};
Line(4) = {4,1};
Line(5) = {2,2};
Line(6) = {2,6};
Line(7) = {6,5};
Line(8) = {5,1};
Line(9) = {3,7};
Line(10) = {7,8};
Line(11) = {8,5};
Line(12) = {4,8};
Line(13) = {6,7};
Line Loop(14) = {1,2,3,4};
Plane Surface(15) = {14};
Line Loop(16) = {-13,-6,2,9};
Plane Surface(17) = {16};
Line Loop(18) = {-11,-10,-13,7};
Plane Surface(19) = {18};
Line Loop(20) = {12,-10,-9,3};
Plane Surface(21) = {20};
Line Loop(22) = {1,6,7,8};
Plane Surface(23) = {22};
Line Loop(24) = {-4,12,11,8};
Plane Surface(25) = {24};
Surface Loop(26) = {23,15,17,19,25,21};
Volume(27) = {26};
Physical Surface("bord1") = {15};
Physical Surface("bord2") = {23};
Physical Surface("bord3") = {25};
Physical Surface("bord4") = {21};
Physical Surface("bord5") = {19};
Physical Surface("bord6") = {17};
Physical Volume("domaine") = {27};



