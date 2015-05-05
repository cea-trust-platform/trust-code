
#ifndef Hello_inclus
#define Hello_inclus

#include <Interprete.h>


class Hello : public Interprete
{

  Declare_instanciable(Hello);

public :

  Entree& interpreter(Entree&);
 
};


#endif



