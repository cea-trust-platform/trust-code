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
#include <Discretisation_base.h>
#include <Champ_val_tot_sur_vol_base.h>

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
  la_puissance_lu.typer(type);
  Champ_Don_base& ch_puissance_lu = ref_cast(Champ_Don_base,la_puissance_lu.valeur());
  is >> ch_puissance_lu;
  const int nb_comp = ch_puissance_lu.nb_comp();

  eqn.probleme().discretisation().discretiser_champ("CHAMP_ELEM", eqn.zone_dis(), "pp", "1",nb_comp,0., la_puissance);

  if (la_puissance_lu.le_nom()=="anonyme")
    for (int n = 0; n < nb_comp; n++) la_puissance_lu->fixer_nom_compo(n, Nom("Puissance_volumique") + (nb_comp ? Nom(n) :""));
  else
    {
      // on met a jour le nom des compos
      Cerr << "The field 'Puissance_volumique' has been renamed as '" << la_puissance_lu.le_nom() << "'." << finl;
      la_puissance_lu->fixer_nom_compo(la_puissance_lu.le_nom());
    }

  for (int n = 0; n < nb_comp; n++) la_puissance->fixer_nom_compo(n, la_puissance_lu.le_nom() + (nb_comp ? Nom(n) :""));
  // PL: Il faut faire nommer_completer_champ_physique les 2 champs (plantage sinon pour une puissance de type Champ_fonc_tabule)
  eqn.discretisation().nommer_completer_champ_physique(eqn.zone_dis(),la_puissance_lu.le_nom(),"W/m3",la_puissance_lu,eqn.probleme());
  eqn.discretisation().nommer_completer_champ_physique(eqn.zone_dis(),la_puissance_lu.le_nom(),"W/m3",la_puissance,eqn.probleme());
  la_puissance.valeur().valeurs() = 0;
  la_puissance.valeur().affecter(ch_puissance_lu);
}

void Terme_Puissance_Thermique::initialiser_champ_puissance(const Equation_base& eqn)
{
  if (sub_type(Champ_val_tot_sur_vol_base,la_puissance_lu.valeur()))
    {
      const Zone_dis_base& zdis = eqn.zone_dis().valeur();
      const Zone_Cl_dis_base& zcldis = eqn.zone_Cl_dis().valeur();
      Champ_val_tot_sur_vol_base& champ_puis = ref_cast(Champ_val_tot_sur_vol_base,la_puissance_lu.valeur());
      champ_puis.evaluer(zdis,zcldis);
    }
}

void Terme_Puissance_Thermique::mettre_a_jour(double temps)
{
  la_puissance_lu.mettre_a_jour(temps);
  la_puissance.valeur().affecter(la_puissance_lu.valeur());
}

void Terme_Puissance_Thermique::preparer_source(const Probleme_base& pb)
{
  if (!pb.is_QC())
    {
      if (!pb.milieu().capacite_calorifique().non_nul())
        {
          Cerr << "La capacite calorifique Cp n'est pas definie..." << finl;
          Cerr << "Elle est necessaire pour la definition du terme source puissance thermique." << finl;
          Process::exit();
        }
      const Champ_Don& le_Cp = pb.milieu().capacite_calorifique();
      const Champ_base& rho = pb.milieu().masse_volumique();
      associer_champs(rho, le_Cp);
    }
}

void Terme_Puissance_Thermique::modify_name_file(Nom& fichier) const
{
  if (la_puissance.le_nom()!="Puissance_volumique")
    {
      fichier+="_";
      fichier+=la_puissance.le_nom();
    }
}
