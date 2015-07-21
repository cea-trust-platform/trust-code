/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Champ_Front_Fonction.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Front_Fonction.h>
#include <EChaine.h>
#include <Champ_Inc_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>

Implemente_instanciable(Champ_Front_Fonction,"Champ_Front_Fonction",Ch_front_var_instationnaire_dep);


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
Sortie& Champ_Front_Fonction::printOn(Sortie& os) const
{
  return os;
}

// Description:
//     Lecture du Champ a partir d'un flot d'entree,
//     (On ne sait traiter que les champs scalaires.)
//     exemple:
//     Champ_Front_Fonction
//     1 (nombre de composantes)
//     temperature  (ch est fonction de la temperature)
//     val*6 (expression du parser)
//
// Precondition: seul les champs scalaires sont traites
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
// Effets de bord:
// Postcondition:
Entree& Champ_Front_Fonction::readOn(Entree& is)
{


  int nbcomp;
  nbcomp=lire_dimension(is,que_suis_je());
  if(nbcomp==1)
    {
      fixer_nb_comp(nbcomp);
      is >> nom_champ_parametre_;

      Cerr<<"The analytical fonction is read... "<<finl;
      la_table.lire_f(is);
    }
  else
    {
      Cerr << "Error when reading Champ_Front_Fonction:" << finl;
      Cerr << "We can deal only with scalar fields." << finl;
      exit();
    }
  return is;
}
void Champ_Front_Fonction::mettre_a_jour(double temps)
{


  const Champ_base& ch=ref_pb.valeur().get_champ(nom_champ_parametre_);

  DoubleTab& tab=valeurs_au_temps(temps);
  DoubleTab tab_ch(tab);
  ch.trace(frontiere_dis(),tab_ch,temps);
  assert(tab.dimension(0)==tab_ch.dimension(0));
  int nb_cas=tab.dimension_tot(0);
  for (int comp=0; comp<tab.dimension(1); comp++)
    for (int c=0; c<nb_cas; c++)
      {
        tab(c,comp)=la_table.val(tab_ch(c,comp));
      }

}

int Champ_Front_Fonction::initialiser(double temps, const Champ_Inc_base& inco)
{
  if (!Champ_front_var_instationnaire::initialiser(temps,inco))
    return 0;
  ref_pb=inco.equation().probleme();
  mettre_a_jour(temps);
  return 1;
}
Champ_front_base& Champ_Front_Fonction::affecter_(const Champ_front_base& ch)
{
  Cerr<<__FILE__<<__LINE__<<" not coded" <<finl;
  exit();
  return (*this);
}



