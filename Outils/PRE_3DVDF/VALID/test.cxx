// genere par :./traduction_Qt el.trace 
//fichier genere par traduction_Qt
#include <stdio.h>
#include <config.h>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>

#include <BRepPrimAPI_MakePrism.hxx>

#include <TopoDS_Shape.hxx>

#include <BRepClass3d_SolidClassifier.hxx>
#include <TopAbs.hxx>


int main_2 (int argc, char* argv[] ) {
  
  TopoDS_Shape shape;
  BRep_Builder B;
  
  //  ifstream entree;
  float* xi;float* yi; float* zi;
  int nx,ny,nz;
 
  int cas=1;
   
  BRepTools::Read(shape,argv[cas],B);
  BRepClass3d_SolidClassifier classif(shape);
  int tot=0;
  float x =atof(argv[2]);
  float y=atof(argv[3]);
  float z=atof(argv[4]);
  
  gp_Pnt P(x,y,z);
  cout<<x << " "<<y<<" "<<z<<" ";
  classif.Perform( P,1e-6);
  switch(classif.State())
    {
    case  TopAbs_IN: 
      cout<<"IN"<<endl;
      break;
    case    TopAbs_OUT:
      cout<<"OUT"<<endl;
      break;
    case   TopAbs_ON:
      cout<<"ON"<<endl;
      break;
    case   TopAbs_UNKNOWN: 
      cout<<"???"<<endl;
      break;
    default: cout<<"gros_pb"<<endl;exit(-1);
    }
  return 0;

}

int main ( int argc, char* argv[] ) 
{
  return main_2(argc,argv);
  //return main_2();
}

