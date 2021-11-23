/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Dirichlet.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Cond_Lim
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Dirichlet.h>
#include <Equation_base.h>

Implemente_base(Dirichlet,"Dirichlet",Cond_lim_base);


// Description:
//    Ecrit le type de l'objet sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Dirichlet::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}



// Description:
//    Simple appel a: Cond_lim_base::readOn(Entree& )
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& s
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Dirichlet::readOn(Entree& s )
{
  return Cond_lim_base::readOn(s);
}


// Description:
//    Renvoie la valeur imposee sur la i-eme composante
//    du champ a la frontiere au temps par defaut du champ_front.
// Precondition:
// Parametre: int i
//    Signification: indice suivant la premiere dimension du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: double
//    Signification: la valeur imposee sur la composante du champ specifiee
//    Contraintes:
// Exception: deuxieme dimension du champ de frontiere superieur a 1
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Dirichlet::val_imp(int i) const
{
  return val_imp_au_temps(le_champ_front->get_temps_defaut(),i);
}


// Description:
//    Renvoie la valeur imposee sur la (i,j)-eme composante
//    du champ a la frontiere au temps par defaut du champ_front.
// Precondition:
// Parametre: int i
//    Signification: indice suivant la premiere dimension du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: int j
//    Signification: indice suivant la deuxieme dimension du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: double
//    Signification: la valeur imposee sur la composante du champ specifiee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Dirichlet::val_imp(int i, int j) const
{
  return val_imp_au_temps(le_champ_front->get_temps_defaut(),i,j);
}

// Description:
//    Renvoie la valeur imposee sur la i-eme composante
//    du champ a la frontiere au temps precise.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double Dirichlet::val_imp_au_temps(double temps, int i) const
{
  const DoubleTab& tab=le_champ_front->valeurs_au_temps(temps);
  if (tab.size()==1)
    return tab(0,0);
  if (tab.line_size()==1)
    return tab(i,0);
  else
    Cerr << "Dirichlet::val_imp error" << finl;
  exit();
  return 0.;
}


// Description:
//    Renvoie la valeur imposee sur la (i,j)-eme composante
//    du champ a la frontiere au temps precise.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double Dirichlet::val_imp_au_temps(double temps, int i, int j) const
{
  const DoubleTab& tab=le_champ_front->valeurs_au_temps(temps);
  if (tab.dimension(0)==1)
    return tab(0,j);
  else
    return tab(i,j);
}

void Dirichlet::verifie_ch_init_nb_comp()
{
  if (le_champ_front.non_nul())
    {
      const Equation_base& eq = zone_Cl_dis().equation();
      const int nb_comp = le_champ_front.valeur().nb_comp();
      eq.verifie_ch_init_nb_comp(eq.inconnue(),nb_comp, this);
    }
}
