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
// File:        Terme_Puissance_Thermique.cpp
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Puissance_Thermique.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Champ.h>
#include <Equation_base.h>
#include <Champ_Uniforme.h>
#include <Champ_Fonc_Tabule.h>


// Description:
//    Lit le terme de puissance thermique a partir
//    d'un flot d'entree et d une equation pour eventuellement
//    acceder a la discretisation afin de typer la_puissance.
//    Lit uniquement un champ donne representant la_puissance.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Terme_Puissance_Thermique::lire_donnees(Entree& is,const Equation_base& eqn)
{
  Cerr << "Lecture du Champ Puissance" << finl;

  Motcle type;
  is >> type;
  if (type=="Valeur_totale_sur_volume")
    {
      const Zone_dis_base& zdis = eqn.zone_dis().valeur();
      const Nom& type_zdis = zdis.que_suis_je();
      if (type_zdis.debute_par("Zone_VDF"))
        type += "_VDF";
      else  if (type_zdis.debute_par("Zone_VEF"))
        type += "_VEF";
    }
  la_puissance.typer(type);
  Champ_Don_base& ch_puissance = ref_cast(Champ_Don_base,la_puissance.valeur());
  is >> ch_puissance;

  if ((!sub_type(Champ_Uniforme,ch_puissance))&&(!sub_type(Champ_Fonc_Tabule,ch_puissance)))
    {
      const DoubleTab& p=ch_puissance.valeurs();
      if (p.get_md_vector()!=eqn.zone_dis().valeur().zone().md_vector_elements())
        {
          for (int i=0; i<1000; i++)
            {
              Cerr<<" Warning !!!!!! The field associate to the power seems to not have the good support !!!!!!!" <<finl;
              Cerr<<"If you use champ_fonc_med try use_exisiting_domain"<<finl;
            }
          Process::exit();
        }
    }
  if (la_puissance.le_nom()=="anonyme")
    la_puissance->fixer_nom_compo("Puissance_volumique");
  else
    {
// on met a jour le nom des compos
      la_puissance->fixer_nom_compo(la_puissance.le_nom());
    }
}

void Terme_Puissance_Thermique::preparer_source(const Probleme_base& pb)
{
  const Champ_Don& le_Cp = pb.milieu().capacite_calorifique();
  const Champ_Don& rho = pb.milieu().masse_volumique();
  associer_champs(rho,le_Cp);
}

void   Terme_Puissance_Thermique::modify_name_file(Nom& fichier) const
{
  if (la_puissance.le_nom()!="Puissance_volumique")
    {
      fichier+="_";
      fichier+=la_puissance.le_nom();
    }
}
