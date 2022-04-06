/****************************************************************************
* Copyright (c) 2022, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
// File:        UserUnaryFunction.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////
#include <UserUnaryFunction.h>
// il faut include LecFicDiffuse.h pour la definition du label AVEC_PARSER
#include <LecFicDiffuse.h>

Implemente_instanciable(UserUnaryFunction,"UserUnaryFunction",UnaryFunction);




// Description:
//    Ecriture sur un flot de sortie
//    Ecrit la valeur de la constante
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& UserUnaryFunction::printOn(Sortie& os) const
{

  return os;
}


// Description:
//    Lit la valeur de la constante
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& UserUnaryFunction::readOn(Entree& is)
{
#ifdef AVEC_PARSER
  Nom sfunc, nom, inco;

  is >> nom;
  is >> inco;
  is >> sfunc;


  setFunction(nom,inco, sfunc);

  LecFicDiffuse& f = verif_cast(LecFicDiffuse&, is);
  {
    f.parser().addFunc(*this);
    p.setPere(f.parser());
  }
  p.parseString();
  Cerr << "Interpretation of the function " << sfunc << " OK" << finl;


#else
  Cerr << " UserUnaryFunction : TRUST must be re-compiled with accounting for the PARSER mode" << finl;
  Cerr <<" See file Lec_Fic_Dif.h" << finl;
  exit();
#endif
  return is;
}

void UserUnaryFunction::setFunction(Nom& name, Nom& inco, Nom& func)
{
  nom_ = name;
  inconnue = inco;

  std::string sfunc(func.getString());
  for (auto & c: sfunc) c = toupper(c);

  p.setString(sfunc);
  p.addVar(inco.getChar());
}

double UserUnaryFunction::eval(double x)
{
  p.setVar(0,x);
  return p.eval();
}
