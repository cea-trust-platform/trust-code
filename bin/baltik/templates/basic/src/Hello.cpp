
#include <Hello.h>

Implemente_instanciable(Hello,"Hello",Interprete);


// printOn et readOn 

Sortie& Hello::printOn(Sortie& s ) const 
{
  return s << que_suis_je() << "\n";
}

Entree& Hello::readOn(Entree& is ) 
{
  //
  // VERIFIER ICI QU'ON A BIEN TOUT LU;  
  //
  return is;
}

Entree& Hello::interpreter(Entree& is)
{
Cout <<"Hello world" <<finl;
return is;
}
