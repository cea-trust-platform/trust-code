////L=0.003;

////Point(1)={0,0,0,L};
////Point(2)={0.1,0,0,L};
////Point(3)={0,0.1,0,L};
////Point(4)={-0.1,0,0,L};
////Point(5)={0,-0.1,0,L};

////Circle(1)={2,1,3};
////Circle(2)={3,1,4};
////Circle(3)={4,1,5};
////Circle(4)={5,1,2};

////Line Loop(1) = {1, 2, 3, 4};
////Plane Surface(1) = {1};

//////Extrude {0,0,0.2} { Surface{1} ; 5 }
////zLayers=10;
////Extrude {0,0,0.2} { Surface{1} ; Layers{zLayers}; }
//////Extrude {0,0,0.2} { Surface{1} ; Layers{zLayers};Recombine; }
////// "RECOMBINE" COMBINES TRIANGLES INTO QUADRANGLES

////Physical Surface("Inlet")={1};
////Physical Surface("Outlet")={26};
////Physical Surface("Wall")={13,17,21,25};
////Physical Volume("dom") = {1};

L=0.01;

Point(1)={0,0,0,L};
Point(2)={0,0.1,0,L};
Point(3)={0,0,0.1,L};
Point(4)={0,-0.1,0,L};
Point(5)={0,0,-0.1,L};

Circle(1)={2,1,3};
Circle(2)={3,1,4};
Circle(3)={4,1,5};
Circle(4)={5,1,2};

Physical Line("Wall")={1,2,3,4};

Line Loop(1) = {1, 2, 3, 4};
Plane Surface(1) = {1};

Physical Surface("dom")={1};


