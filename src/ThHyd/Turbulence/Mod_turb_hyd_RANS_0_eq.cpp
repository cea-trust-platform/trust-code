/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Mod_turb_hyd_RANS_0_eq.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Mod_turb_hyd_RANS_0_eq.h>
#include <Discretisation_base.h>
#include <Modifier_nut_pour_QC.h>
#include <Motcle.h>
#include <Equation_base.h>
#include <EcrMED.h>
#include <Domaine.h>
#include <Schema_Temps_base.h>
#include <stat_counters.h>
#include <Param.h>

Implemente_base_sans_constructeur(Mod_turb_hyd_RANS_0_eq,"Mod_turb_hyd_RANS_0_eq",Mod_turb_hyd_base);

Mod_turb_hyd_RANS_0_eq::Mod_turb_hyd_RANS_0_eq()
{
  /*
    Noms& nom=champs_compris_.liste_noms_compris();
    nom.dimensionner(1);
    // L energie cinetique ne peut etre postraitee pour le modele longueur de melange
    //A coder
    nom[0]="k";
  */
}

// Description:
//    Simple appel a Mod_turb_hyd_base::printOn(Sortie&)
// Precondition:
// Parametre: Sortie& is
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
Sortie& Mod_turb_hyd_RANS_0_eq::printOn(Sortie& is) const
{
  return Mod_turb_hyd_base::printOn(is);
}


// Description:
//    Simple appel a Mod_turb_hyd_base::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Mod_turb_hyd_RANS_0_eq::readOn(Entree& is)
{
  Mod_turb_hyd_base::readOn(is);
  return is;
}

void Mod_turb_hyd_RANS_0_eq::set_param(Param& param)
{
  Mod_turb_hyd_base::set_param(param);
  param.ajouter("fichier_ecriture_K_eps",&fichier_K_eps_sortie_);
}

void Mod_turb_hyd_RANS_0_eq::discretiser()
{
  Mod_turb_hyd_base::discretiser();
  discretiser_K(mon_equation->schema_temps(),mon_equation->zone_dis(),energie_cinetique_turb_);
  champs_compris_.ajoute_champ(energie_cinetique_turb_);
}

int Mod_turb_hyd_RANS_0_eq::a_pour_Champ_Fonc(const Motcle& mot,
                                              REF(Champ_base)& ch_ref ) const
{
  Motcles les_motcles(3);
  {
    les_motcles[0] ="viscosite_turbulente";
    les_motcles[1] ="k";
    les_motcles[2] ="distance_paroi";
  }
  int rang = les_motcles.search(mot);
  switch(rang)
    {
    case 0:
      {
        ch_ref = la_viscosite_turbulente.valeur();
        return 1;
      }
    case 1:
      {
        ////ch_ref = energie_cinetique_turb_.valeur();
        ////return 1;

        Cerr<<"The kinetic energy cannot be post-processed for the mixing length model."<<finl;
        //A coder
        return 0;
      }
    case 2:
      {

        exit();
        return 0;
      }
    default:
      return 0;
    }
}
int Mod_turb_hyd_RANS_0_eq::comprend_mot(const Motcle& mot) const
{
  return comprend_champ(mot);
}

int Mod_turb_hyd_RANS_0_eq::comprend_champ(const Motcle& mot) const
{
  Motcles les_motcles(3);
  {
    les_motcles[0] = "k";
    les_motcles[1] = "viscosite_turbulente";
    les_motcles[2] = "distance_paroi";
  }

  int rang = les_motcles.search(mot);

  if (rang == 0)
    {
      Cerr<<"The kinetic energy is not known for the mixing length model."<<finl;
      //A coder
      return 0;
    }
  else if (rang == 1)
    return 1;
  else if (rang == 2)
    {

      exit();
      return 0;
    }
  else return 0;
}


int Mod_turb_hyd_RANS_0_eq::reprendre(Entree& is)
{
  return 1;
}

void Mod_turb_hyd_RANS_0_eq::completer()
{
  // creation de K_eps_sortie et du fichier med si on a demande ecrire_K_eps
  if (fichier_K_eps_sortie_!=Nom())
    {
      // 1) on cree le fichier med et on postraite le domaine
      EcrMED ecr_med;
      const Domaine& dom=mon_equation->zone_dis().zone().domaine();
      ecr_med.ecrire_domaine(fichier_K_eps_sortie_.nom_me(me()),dom,dom.le_nom(),-1);
      //2 on discretise le champ K_eps_pour_la_sortie
      const Discretisation_base& dis = mon_equation->discretisation();
      Noms noms(2);
      Noms unit(2);
      noms[0]="K";
      noms[1]="eps";
      unit[0]="m2/s2";
      unit[1]="m2/s3";
      int nb_case_tempo=1;
      double temps=mon_equation->schema_temps().temps_courant();
      dis.discretiser_champ("CHAMP_ELEM",mon_equation->zone_dis().valeur(),scalaire ,noms,unit,2,nb_case_tempo,temps,K_eps_sortie_);
      K_eps_sortie_.nommer("K_eps_from_nut");
      K_eps_sortie_.valeur().nommer("K_eps_from_nut");
      K_eps_sortie_->fixer_unites(unit);
      K_eps_sortie_->fixer_noms_compo(noms);
    }
}

void Mod_turb_hyd_RANS_0_eq::mettre_a_jour(double )
{
  statistiques().begin_count(nut_counter_);
  calculer_viscosite_turbulente();
  calculer_energie_cinetique_turb();
  loipar.calculer_hyd(la_viscosite_turbulente,energie_cinetique_turbulente());
  limiter_viscosite_turbulente();
  Correction_nut_et_cisaillement_paroi_si_qc(*this);
  energie_cinetique_turb_.valeurs().echange_espace_virtuel();
  la_viscosite_turbulente.valeurs().echange_espace_virtuel();
  statistiques().end_count(nut_counter_);
}


void Mod_turb_hyd_RANS_0_eq::imprimer (Sortie& os )  const
{
  const Schema_Temps_base& sch = mon_equation->schema_temps();
  double temps_courant = sch.temps_courant();
  double dt= sch.pas_de_temps() ;
  if ( limpr_ustar(temps_courant,sch.temps_precedent(),dt,dt_impr_ustar) || limpr_ustar(temps_courant,sch.temps_precedent(),dt,dt_impr_ustar_mean_only) )
    if ( K_eps_sortie_.non_nul())
      {
        double temps=mon_equation->schema_temps().temps_courant();
        K_eps_sortie_->mettre_a_jour(temps);

        //  calcul de K_eps

        DoubleTab& K_Eps = K_eps_sortie_.valeurs();
        const DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
        const DoubleTab& wall_length = wall_length_.valeurs();
        const int nb_elem   = K_Eps.dimension(0);

        const double Kappa = 0.415;
        double Cmu = CMU;
        // PQ : 27/06/07 : expressions de k et eps basees sur :
        //
        //   nu_t = C_mu.K.L.k^(1/2)  et nu_t = C_mu.k^2/eps

        for(int elem=0; elem<nb_elem; elem++)
          {
            K_Eps(elem,0)=pow(visco_turb(elem)/(Cmu*Kappa*wall_length(elem)),2);

            if (visco_turb(elem)==0.)
              K_Eps(elem,1)=0.;
            else
              K_Eps(elem,1)=Cmu*K_Eps(elem,0)*K_Eps(elem,0)/visco_turb(elem);
          }

        // PQ : recalibrage de k et eps d'apres resultats
        //        sur Canal plan a Re = 100 000 et Re = 1 000 000
        for(int elem=0; elem<nb_elem; elem++)
          {
            K_Eps(elem,0)/=47.;
            K_Eps(elem,1)/=1000.;
            //Cerr<<elem <<" "<< K_Eps(elem,0)<<" "<<K_Eps(elem,1)<<" ";
            //Cerr<<visco_turb(elem)<<" "<<wall_length(elem)<<finl;
          }

        // enfin ecriture du champ aux elems (il y est deja)
        const Domaine& dom=mon_equation->zone_dis().zone().domaine();
        Nom fic=fichier_K_eps_sortie_.nom_me(me());

        EcrMED ecr_med;
        Nom nom_post=K_eps_sortie_.le_nom();
        Nom nom_dom=dom.le_nom();
        Nom nom_dom_inc= dom.le_nom();
        Nom type_elem=dom.zone(0).type_elem()->que_suis_je();
        assert(K_eps_sortie_.valeurs().dimension(0)==dom.zone(0).nb_elem());
        ecr_med.ecrire_champ("CHAMPMAILLE",fic,dom,nom_post,K_eps_sortie_.valeurs(),K_eps_sortie_->unites(),K_eps_sortie_->noms_compo(),type_elem,temps,0);
      }
  return Mod_turb_hyd_base::imprimer(os);
}

