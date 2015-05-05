//////////////////////////////////////////////////////////////////////////////
//
// File:        Testeur2.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include <Testeur2.h>

Implemente_instanciable(Testeur2,"Testeur2",Interprete);


// printOn et readOn 

Sortie& Testeur2::printOn(Sortie& s ) const 
{
  return s << que_suis_je() << "\n";
}

Entree& Testeur2::readOn(Entree& is ) 
{
  return is;
}


Entree& Testeur2::interpreter(Entree& is)
{ 

	// on cree un tableau que l'on ne detruite pas a fin de verifier que valgrind voit l'erreur
  double * p =new double[142];
  cerr<<"adresse p"<<p<<endl;
  return is;
}
