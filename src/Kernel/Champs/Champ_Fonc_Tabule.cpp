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
// File:        Champ_Fonc_Tabule.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_Tabule.h>
#include <Champ_Uniforme.h>

Implemente_instanciable(Champ_Fonc_Tabule,"Champ_Fonc_Tabule",Champ_Fonc_base);


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Champ_Fonc_Tabule::printOn(Sortie& os) const
{
  return os;
}

// Description:
//     Lecture du Champ a partir d'un flot d'entree,
//     (On ne sait traiter que les champs scalaires.)
//     exemple:
//     Champ_Fonc_Tabule ch
//     Lire ch
//     1 (nombre de composantes)
//     champ  (ch est fonction d'un champ ch)
//     {
//     2
//     0 500 0 250  (ch(0)=0 && ch(500)=250
//     }
// Precondition: seul les champs scalaires sont traites
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade fermante attendue
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition:
Entree& Champ_Fonc_Tabule::readOn(Entree& is)
{
  Motcle motlu;
  int nb_val;
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  int nbcomp,i;
  nbcomp=lire_dimension(is,que_suis_je());
  if(nbcomp==1)
    {
      fixer_nb_comp(nbcomp);
      is >> nom_champ_parametre_;
      is >> motlu;
      if (motlu == accolade_ouverte)
        {
          nb_val=lire_dimension(is,que_suis_je());
          DoubleVect param(nb_val);
          DoubleVect tab_valeurs(nb_val);
          for (i=0; i<nb_val; i++)
            is >> param[i];
          for (i=0; i<nb_val; i++)
            is >> tab_valeurs[i];
          la_table.remplir(param,tab_valeurs);
          is >> motlu;
          if (motlu != accolade_fermee)
            {
              Cerr << "Error reading from an object of type Champ_Fonc_Tabule" << finl;
              Cerr << "We expected keyword } instead of " << motlu << finl;
              exit();
            }
        }
      else if (motlu=="fonction")
        {
          Cerr<<"The syntax has changed..." << finl;
          Cerr<<"The syntax is now Champ_Fonc_fonction 1 field_expression"<<finl;
          exit();
          Cerr<<"We read the analytic function "<<finl;
          la_table.lire_f(is,0);
        }
      else
        {
          Cerr << "Error reading from an object of type Champ_Fonc_Tabule" << finl;
          Cerr << "We expected keyword { or fonction instead of " << motlu << finl;
          exit();
        }
    }
  else
    {
      Cerr << "Error reading from an object of type Champ_Fonc_Tabule" << finl;
      Cerr << "We know treating only the scalar fields " << finl;
      exit();
    }
  return is;
}

Champ_base& Champ_Fonc_Tabule::affecter_(const Champ_base& ch)
{
  if(!le_champ_tabule_discretise().non_nul())
    Cerr << le_nom() << "type : " << que_suis_je()
         << " can not be assigned to " << ch.le_nom()
         << " because " << le_nom() << " is incomplete " << finl;
  else
    {
      if(sub_type(Champ_Uniforme, ch))
        {
          const DoubleTab& val=ch.valeurs();
          if(val.nb_dim()==1)
            valeurs()=ch.valeurs()[0];
          else
            valeurs()=ch.valeurs()(0,0);
        }
      else
        {
          valeurs()=ch.valeurs();
        }
    }
  return *this;
}
