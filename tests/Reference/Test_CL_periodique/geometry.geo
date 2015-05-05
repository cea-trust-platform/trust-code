
Point(1) = {1.,0.0,0.0,0.2};
Point(2) = {2.,0.0,0.0,0.2};

Line(1) = {1,2};

zLayers=3;
Extrude {0,0,0.5} 
	{ Line{1}; Layers{zLayers};Recombine; }

// "RECOMBINE" COMBINES TRIANGLES INTO QUADRANGLES

thetaLayers=4;
Extrude {{0,0,1}, {0,0,0}, Pi/2} 
	{ Surface{5}; Layers{thetaLayers};Recombine; }
Extrude {{0,0,1}, {0,0,0}, Pi/2} 
	{ Surface{27}; Layers{thetaLayers};Recombine; }
Extrude {{0,0,1}, {0,0,0}, Pi/2} 
	{ Surface{49}; Layers{thetaLayers};Recombine; }
Extrude {{0,0,1}, {0,0,0}, Pi/2} 
	{ Surface{71}; Layers{thetaLayers};Recombine; }

Physical Surface("INLET") = {14,36,58,80};
Physical Surface("OUTLET") = {22,44,66,88};
Physical Surface("INNER_WALL") = {26,48,70,92};
Physical Surface("OUTER_WALL") = {18,40,62,84};

Physical Volume("dom") = {1, 2, 3, 4};

