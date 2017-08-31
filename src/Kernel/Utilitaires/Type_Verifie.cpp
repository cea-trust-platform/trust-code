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
// File:        Type_Verifie.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/48
//
//////////////////////////////////////////////////////////////////////////////
#include <Type_Verifie.h>
#include <Motcle.h>
#include <Noms.h>

static Motcles motcle_obsolete;

// Parcours du jeu de donnees pour verification des mots cles obsoletes
void verifie(const Nom& type)
{
  int n=57;
  if( motcle_obsolete.size()!=n)
    {
      int i=0;
      // Definition des anciens mots cles desormais plus reconnus
      motcle_obsolete.dimensionner(n);
      motcle_obsolete[i++] = "Solv_GCP_SSOR";
      motcle_obsolete[i++] = "Paroi_flux_impose_Rayo_transp_VDF";
      motcle_obsolete[i++] = "Paroi_flux_impose_Rayo_transp_VEF";
      motcle_obsolete[i++] = "Frontiere_Ouverte_Rayo_transp_VDF";
      motcle_obsolete[i++] = "Frontiere_Ouverte_Rayo_transp_VEF";
      motcle_obsolete[i++] = "EchgItemsComm";
      motcle_obsolete[i++] = "LireMED";
      motcle_obsolete[i++] = "EcrMED";
      motcle_obsolete[i++] = "Canal_thermo";
      motcle_obsolete[i++] = "Prandtl_sous_maille";
      motcle_obsolete[i++] = "Decentre";
      motcle_obsolete[i++] = "Distance_perio";
      motcle_obsolete[i++] = "T_paroi";
      motcle_obsolete[i++] = "Champ_front_lineaire";
      motcle_obsolete[i++] = "Champ_front_analytique";
      motcle_obsolete[i++] = "Tourbillon";
      motcle_obsolete[i++] = "Puissance_Thermique_Uniforme";
      motcle_obsolete[i++] = "Source_Constituant_Uniforme";
      motcle_obsolete[i++] = "Seuil_convg_implicite";
      motcle_obsolete[i++] = "Seuil_resol_implicite";
      motcle_obsolete[i++] = "Seuil_convg_variable";
      motcle_obsolete[i++] = "Schema_Crank_Nicholson";
      motcle_obsolete[i++] = "Resoudre_ALE";
      motcle_obsolete[i++] = "Resoudre_QComp";
      motcle_obsolete[i++] = "Decoupebord";
      motcle_obsolete[i++] = "Periodique_1Proc";
      motcle_obsolete[i++] = "Corriger_coord_perio";
      motcle_obsolete[i++] = "Echange_domcut";
      motcle_obsolete[i++] = "Extruder_en3_perio";
      motcle_obsolete[i++] = "Extruder_perio";
      motcle_obsolete[i++] = "ScatterAscii";
      motcle_obsolete[i++] = "frontiere_ouverte_gradient_pression_impose_VEF";
      motcle_obsolete[i++] = "Champ_front_calc_intern";
      motcle_obsolete[i++] = "Champ_front_calc_recycl_fluct_pbperio";
      motcle_obsolete[i++] = "Champ_front_calc_recycl_champ";
      motcle_obsolete[i++] = "Champ_front_calc_intern_2pbs";
      motcle_obsolete[i++] = "Champ_front_calc_recycl_fluct";
      motcle_obsolete[i++] = "direction_periodicite";
      motcle_obsolete[i++] = "NPHypre";
      motcle_obsolete[i++] = "NPPetsc";
      motcle_obsolete[i++] = "Latatomed";
      motcle_obsolete[i++] = "Latatoother";
      motcle_obsolete[i++] = "MG";
      motcle_obsolete[i++] = "precond_local";
      motcle_obsolete[i++] = "quick_period";
      motcle_obsolete[i++] = "diffusivite";
      motcle_obsolete[i++] = "Ecrire_fic_meshtv";
      motcle_obsolete[i++] = "meshtv";
      motcle_obsolete[i++] = "paroi_couple";
      motcle_obsolete[i++] = "corriger_coordonnees";
      motcle_obsolete[i++] = "Champ_front_debit_VEF";
      motcle_obsolete[i++] = "Schema_MacCormack";
      motcle_obsolete[i++] = "Alternant";
      motcle_obsolete[i++] = "Alter_interp";
      motcle_obsolete[i++] = "Boussinesq";
      motcle_obsolete[i++] = "Temperature_paroi_NOPASPOURGENEPI";
      motcle_obsolete[i++] = "Read_MED_For_Testing_No_Verify_Option";
      assert(i==n);
    }
  // GF on ne constuit les noms de la nouvelle syntaxe que si necessaire
  // cela optimise un peu...
  int rang=motcle_obsolete.search((Motcle)type);
  if (rang!=-1)
    {

      // Definition de la nouvelle syntaxe
      Noms nouvelle_syntaxe(n);
      int i=0;
      nouvelle_syntaxe[i++] = "1.2, le format du jeu de donnees pour le solveur de pression a change:\nsolveur_pression GCP_ssor { omega 1.5 seuil 1.e-8 impr }\nDevient, pour tenir compte du preconditionneur:\nsolveur_pression GCP { precond ssor { omega 1.5 } seuil 1.e-8 impr }";
      nouvelle_syntaxe[i++] = "1.4.9, il devient Paroi_flux_impose_Rayo_transp";
      nouvelle_syntaxe[i++] = "1.4.9, il devient Paroi_flux_impose_Rayo_transp";
      nouvelle_syntaxe[i++] = "1.4.9, il devient Frontiere_Ouverte_Rayo_transp";
      nouvelle_syntaxe[i++] = "1.4.9, il devient Frontiere_Ouverte_Rayo_transp";
      nouvelle_syntaxe[i++] = "1.5.";
      nouvelle_syntaxe[i++] = "1.5, il devient Lire_MED";
      nouvelle_syntaxe[i++] = "1.5, il devient Ecrire_MED";
      nouvelle_syntaxe[i++] = "1.5, il devient Canal";
      nouvelle_syntaxe[i++] = "1.5, il suffit de mettre Prandtl";
      nouvelle_syntaxe[i++] = "1.5.1, il est a remplacer par generic";
      nouvelle_syntaxe[i++] = "1.5.1, il n'est plus necessaire dans la definition de la loi de paroi Shifted";
      nouvelle_syntaxe[i++] = "1.5.1, il devient Temperature_paroi";
      nouvelle_syntaxe[i++] = "1.5.1, on doit utiliser Champ_front_fonc_XYZ a la place";
      nouvelle_syntaxe[i++] = "1.5.1, on doit utiliser Champ_front_fonc_XYZ a la place";
      nouvelle_syntaxe[i++] = "1.5.1, on doit utiliser Champ_fonc_XYZ a la place";
      nouvelle_syntaxe[i++] = "1.5.1, on doit utiliser Puissance_Thermique a la place";
      nouvelle_syntaxe[i++] = "1.5.1, on doit utiliser Source_Constituant a la place";
      nouvelle_syntaxe[i++] = "1.5.1, on doit utiliser Seuil_convergence_implicite a la place";
      nouvelle_syntaxe[i++] = "1.5.1, on doit utiliser Seuil_convergence_solveur a la place";
      nouvelle_syntaxe[i++] = "1.5.1, on doit utiliser Seuil_convergence_variable a la place";
      nouvelle_syntaxe[i++] = "1.5.2, on doit utiliser Sch_CN_EX_iteratif ou Sch_CN_iteratif a la place";
      nouvelle_syntaxe[i++] = "1.5.2, on doit utiliser Resoudre a la place";
      nouvelle_syntaxe[i++] = "1.5.2, on doit utiliser Resoudre a la place";
      nouvelle_syntaxe[i++] = "1.5.3, on doit utiliser Decoupebord_pour_rayonnement a la place. Voir la nouvelle syntaxe.";
      nouvelle_syntaxe[i++] = "1.5.3, on doit utiliser le mot cle Periodique dans le decoupeur pour specifier les bords periodiques.";
      nouvelle_syntaxe[i++] = "1.5.5, on doit utiliser Corriger_frontiere_periodique { ... corriger_coordonnees }";
      nouvelle_syntaxe[i++] = "1.5.7, il n'est plus necessaire pour paralleliser certaines conditions limites.";
      nouvelle_syntaxe[i++] = "1.5.7, you should use Extruder_en3 keyword and after RegroupeBord keyword to create periodic boundary.";
      nouvelle_syntaxe[i++] = "1.6.0, you should use Extruder keyword and after RegroupeBord keyword to create periodic boundary.";
      nouvelle_syntaxe[i++] = "1.6.0, you should use ScatterFormatte.";
      nouvelle_syntaxe[i++] = "1.6.0, you should use frontiere_ouverte_gradient_pression_impose_VEFPREP1B.";
      nouvelle_syntaxe[i++] = "1.6.1, you should use Champ_front_recyclage instead.";
      nouvelle_syntaxe[i++] = "1.6.1, you should use Champ_front_recyclage instead.";
      nouvelle_syntaxe[i++] = "1.6.1, you should use Champ_front_recyclage instead.";
      nouvelle_syntaxe[i++] = "1.6.1, you should use Champ_front_recyclage instead.";
      nouvelle_syntaxe[i++] = "1.6.1, you should use Champ_front_recyclage instead.";
      nouvelle_syntaxe[i++] = "1.6.1, it is no longer necessary for a periodic boundary condition.";
      nouvelle_syntaxe[i++] = "1.6.1, you should use Lata_to_med with new syntax.";
      nouvelle_syntaxe[i++] = "1.6.1, you should use Lata_to_other with new syntax.";
      nouvelle_syntaxe[i++] = "1.6.1, is no longer available as solver.";
      nouvelle_syntaxe[i++] = "1.6.1, is no longer available as precond.";
      nouvelle_syntaxe[i++] = "1.6.1, Numerical Platon library is not included anymore. Look at the new keyword Petsc.";
      nouvelle_syntaxe[i++] = "1.6.1, Numerical Platon library is not included anymore. Look at the new keyword Petsc.";
      nouvelle_syntaxe[i++] = "1.6.4, you should use quick instead (periodic boundary condition is supported with the last keyword).";
      nouvelle_syntaxe[i++] = "1.6.4, you should use coefficient_diffusion instead.";
      nouvelle_syntaxe[i++] = "1.6.4, you should use Postraiter_domaine keyword instead with LATA format.";
      nouvelle_syntaxe[i++] = "1.6.4, you should use LATA format.";
      nouvelle_syntaxe[i++] = "1.6.6, you should use paroi_contact instead.";
      nouvelle_syntaxe[i++] = "1.6.7, because is activated now by default.";
      nouvelle_syntaxe[i++] = "1.6.7, you should use Champ_front_debit instead.";
      nouvelle_syntaxe[i++] = "1.6.7, you should use another time scheme.";
      nouvelle_syntaxe[i++] = "1.6.7, you should use another convection scheme.";
      nouvelle_syntaxe[i++] = "1.6.7, you should use another convection scheme.";
      nouvelle_syntaxe[i++] = "1.6.8, you should use now: Boussinesq_temperature { ... } , Boussinesq_concentration { ... }";
      nouvelle_syntaxe[i++] = "1.6.9.";
      nouvelle_syntaxe[i++] = "never use that keyword!!!";
      assert(i==n);
      Cerr << "The keyword " << motcle_obsolete[rang] << " is now obsolete," << finl;
      Cerr << "since the version " << nouvelle_syntaxe[rang] << finl << finl;
      Cerr << "Look if necessary at the reference manual and change your data file." << finl;
      Process::exit();
    }
  // Parcours les releases notes pour afficher des infos en relation avec le mot cle type
  cherche_dans_les_releases_notes(type);
}
