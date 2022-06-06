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

#include <Paroi_Knudsen_non_negligeable.h>
#include <Motcle.h>
#include <Champ_front_fonc_gradient.h>
#include <Discretisation_base.h>


Implemente_instanciable(Paroi_Knudsen_non_negligeable,"Paroi_Knudsen_non_negligeable",Dirichlet_paroi_defilante);


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
Sortie& Paroi_Knudsen_non_negligeable::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
// Lecture des champs vitesse_paroi et k
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
Entree& Paroi_Knudsen_non_negligeable::readOn(Entree& s )
{
  Cerr << "Paroi_Knudsen_non_negligeable::readOn" << finl;
  Motcle mot;
  Motcles les_motcles(3);
  {
    les_motcles[0] = "vitesse_paroi";
    les_motcles[1] = "k";
  }
  int nb_champs_lus=0;
  while (nb_champs_lus<2)
    {
      s >> mot ;
      int rang = les_motcles.search(mot);
      if (rang == 0)
        s >> vitesse_paroi_ ;
      if (rang == 1)
        s >> k_ ;
      nb_champs_lus++;
    }

  le_champ_front.typer("Champ_front_fonc_gradient");
  le_champ_front->fixer_nb_comp(dimension);
  return s;
}

void Paroi_Knudsen_non_negligeable::completer()
{
  Cerr << "Paroi_Knudsen_non_negligeable::completer" << finl;
  Nom type="Champ_front_fonc_gradient_";
  type += zone_Cl_dis().equation().discretisation().que_suis_je();
  // Typage definitif en fonction de la discretisation
  Frontiere_dis_base& fr=le_champ_front.frontiere_dis();
  le_champ_front.typer(type);
  le_champ_front.associer_fr_dis_base(fr);
  // Paroi defilante : le champ_front est la vitesse de nombre
  // de composantes la dimension du pb ... Est ce utile de dimensionner
  // maintenant cela ?
  le_champ_front->fixer_nb_comp(dimension);
  // On associe l'inconnue:
  Champ_front_fonc_gradient& ch=ref_cast(Champ_front_fonc_gradient,le_champ_front.valeur());
  ch.associer_ch_inc_base(zone_Cl_dis().equation().inconnue().valeur());
  Cerr << "Paroi_Knudsen_non_negligeable::completer OK" << finl;
}

// Description:
//    Renvoie un booleen indiquant la compatibilite des conditions
//    aux limites avec l'equation specifiee en parametre.
//    Des CL de type Dirichlet_paroi_defilante sont compatibles
//    avec une equation dont le domaine est l'hydraulique (Navier_Stokes)
//    ou bien indetermine.
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation avec laquelle il faut verifier la compatibilite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: valeur booleenne,
//                   1 si les CL sont compatibles avec l'equation
//                   0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Paroi_Knudsen_non_negligeable::compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app=eqn.domaine_application();
  Motcle Hydraulique="Hydraulique";
  Motcle indetermine="indetermine";
  if ( (dom_app==Hydraulique) || (dom_app==indetermine) )
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}
