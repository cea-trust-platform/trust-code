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
// File:        Champ_front_Tabule.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_front_Tabule.h>
#include <DoubleTrav.h>

Implemente_instanciable(Champ_front_Tabule,"Champ_front_Tabule",Champ_front_instationnaire_base);
// XD champ_front_tabule front_field_base champ_front_tabule 0 Constant field on the boundary, tabulated as a function of time.
// XD attr nb_comp entier nb_comp 0 Number of field components.
// XD attr bloc bloc_lecture bloc 0 {nt1 t2 t3 ....tn u1 [v1 w1 ...] u2 [v2 w2 ...] u3 [v3 w3 ...] ... un [vn wn ...] } NL2 Values are entered into a table based on n couples (ti, ui) if nb_comp value is 1. The value of a field at a given time is calculated by linear interpolation from this table.

// Description:
//    Imprime les valeurs du champ sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
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
Sortie& Champ_front_Tabule::printOn(Sortie& os) const
{
  return os << valeurs();
}


// Description:
//    Lit les valeurs du champ tabule a partir d'un flot d'entree.
//    Format:
//     Champ_front_Tabule nb_comp { nval tps_1...tps_nval vrel_1...vrel_nval }
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Exception: accolade fermante attendue
// Effets de bord:
// Postcondition:
Entree& Champ_front_Tabule::readOn(Entree& is)
{
  Motcle motlu;
  int nb_val;
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");

  int dim,i,j;
  dim=lire_dimension(is,que_suis_je());
  fixer_nb_comp(dim);

  is >> motlu;
  if (motlu == accolade_ouverte)
    {
      nb_val=lire_dimension(is,que_suis_je());
      DoubleVect param(nb_val);
      DoubleTab tab_valeurs(nb_val, dim);
      for (i=0; i<nb_val; i++)
        is >> param[i];
      for (i=0; i<nb_val; i++)
        for (j=0; j<dim; j++)
          is >> tab_valeurs(i,j);
      la_table.remplir(param,tab_valeurs);
      is >> motlu;
      if (motlu != accolade_fermee)
        {
          Cerr << "Error reading from an object of type Champ_front_Tabule" << finl;
          Cerr << "We expected keyword } instead of " << motlu << finl;
          exit();
        }
    }
  else
    {
      Cerr << "Error reading from an object of type Champ_front_Tabule" << finl;
      Cerr << "We expected keyword { instead of " << motlu << finl;
      exit();
    }
  return is;
}



// Description:
//    Renvoie l'objet Champ_front_Tabule upcaste en Champ_front_base
// Precondition:
// Parametre: Champ_front_base& ch
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: NON ACCEDE
// Retour: Champ_front_base&
//    Signification: (*this) upcaste en Champ_front_base
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_front_base& Champ_front_Tabule::affecter_(const Champ_front_base& )
{
  return *this;
}


int Champ_front_Tabule::initialiser(double temps, const Champ_Inc_base& inco)
{
  if (!Champ_front_instationnaire_base::initialiser(temps,inco))
    return 0;
  mettre_a_jour(temps);
  return 1;
}

// Description:
//    Mise a jour en temps du champ.
//    Calcul du gradient Gpoint.
// Precondition:
// Parametre: double tps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_front_Tabule::mettre_a_jour(double temps)
{
  la_table.valeurs(valeurs_au_temps(temps), temps);
}
