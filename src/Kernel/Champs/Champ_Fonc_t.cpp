/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Champ_Fonc_t.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_t.h>

Implemente_instanciable(Champ_Fonc_t,"Champ_Fonc_t",Champ_Uniforme_inst);

// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Champ_Fonc_t::printOn(Sortie& os) const
{
  return os;
}

// Description:
//  Lecture des parametres dans le fichier data.
//  On attend
//    nb_composantes expression_composante1(t) expression_composante2(t) ...
//  Exemple:
//    3 SIN(t) t*t 1.+t
Entree& Champ_Fonc_t::readOn(Entree& is)
{
  int nb_compo;
  nb_compo=lire_dimension(is,que_suis_je());
  if (nb_compo <= 0)
    {
      Cerr << "Error in Champ_Fonc_t::readOn : nb_compo <= 0" << finl;
      assert(0);
      exit();
    }
  dimensionner(1, nb_compo);
  ft.dimensionner(nb_compo);
  for (int i_parser = 0; i_parser < nb_compo; i_parser++)
    {
      Nom expression;
      is >> expression;
      const char* s = expression.getChar();
      String2 ss(s);
      ft[i_parser].setNbVar(1);
      ft[i_parser].setString(ss);
      ft[i_parser].addVar("t");
      ft[i_parser].parseString();
    }

  me_calculer(0.);

  return is;
}

// Description:
//  Calcul du tableau "valeurs()" au temps t a partir des fonctions
//  analytiques lues dans le readOn
void Champ_Fonc_t::me_calculer(double t)
{
  int i = 0;
  DoubleTab& val = valeurs();
  for (i = 0; i < ft.size(); i++)
    {
      //Parser & p = parser_array_[i];
      //p.setVar(0, t);
      //const double v = p.eval();
      ft[i].setVar(0, t);
      const double v = ft[i].eval();
      val(0, i) = v;
    }
}
