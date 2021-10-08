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
// File:        Neumann_paroi.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Cond_Lim
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Neumann_paroi.h>
#include <Motcle.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Convection_Diffusion_Concentration.h>

Implemente_instanciable(Neumann_paroi,"Neumann_paroi",Neumann);

Sortie& Neumann_paroi::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Neumann_paroi::readOn(Entree& s )
{
  return Cond_lim_base::readOn(s) ;
}

// Description:
//    Renvoie un booleen indiquant la compatibilite des conditions
//    aux limites avec l'equation specifiee en parametre.
//    Des CL de type Neumann_paroi sont compatibles
//    avec une equation dont le domaine est la Thermique
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
int Neumann_paroi::compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app=eqn.domaine_application();
  Motcle Thermique="Thermique", Thermique_H ="Thermique_H",Diphasique="diphasique_moyenne";
  Motcle indetermine="indetermine",Concentration="Concentration",FracMass="fraction_massique";

  if ( (dom_app==Thermique) || (dom_app==Thermique_H) || (dom_app==Diphasique) || (dom_app==indetermine)
       || (dom_app==Concentration) || (dom_app==FracMass))
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}

void Neumann_paroi::verifie_ch_init_nb_comp()
{
  if (le_champ_front.non_nul())
    {
      const Equation_base& eq = zone_Cl_dis().equation();
      const int nb_comp = le_champ_front.valeur().nb_comp();
      eq.verifie_ch_init_nb_comp(eq.inconnue(),nb_comp);
    }
}

double Neumann_paroi::flux_impose(int i) const
{
  if (le_champ_front.valeurs().size()==1)
    return le_champ_front(0,0);
  else if (le_champ_front.valeurs().dimension(1)==1)
    return le_champ_front(i,0);
  else
    Cerr << "Neumann_paroi::flux_impose erreur" << finl;
  Process::exit();
  return 0.;
}

double Neumann_paroi::flux_impose(int i,int j) const
{
  if (le_champ_front.valeurs().dimension(0)==1)
    return le_champ_front(0,j);
  else
    return le_champ_front(i,j);

}

void Neumann_paroi::mettre_a_jour(double temps)
{
  Cond_lim_base::mettre_a_jour(temps);
  le_champ_front.mettre_a_jour(temps);
}
