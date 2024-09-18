/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Fluide_Weakly_Compressible.h>
#include <Loi_Etat_Multi_GP_WC.h>
#include <Neumann_sortie_libre.h>
#include <Domaine_Cl_dis_base.h>
#include <Champ_Fonc_Fonction.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>

#include <Probleme_base.h>
#include <Equation_base.h>
#include <Domaine_VF.h>
#include <Param.h>

Implemente_instanciable(Fluide_Weakly_Compressible,"Fluide_Weakly_Compressible",Fluide_Dilatable_base);
// XD fluide_weakly_compressible fluide_dilatable_base fluide_weakly_compressible -1 Weakly-compressible flow with a low mach number assumption; this means that the thermo-dynamic pressure (used in state law) can vary in space.
// XD attr loi_etat loi_etat_base loi_etat 1 The state law that will be associated to the Weakly-compressible fluid.
// XD attr sutherland bloc_sutherland sutherland 1 Sutherland law for viscosity and for conductivity.
// XD attr traitement_pth chaine(into=["constant"]) traitement_pth 1 Particular treatment for the thermodynamic pressure Pth ; there is currently one possibility: NL2 1) the keyword \'constant\' makes it possible to have a constant Pth but not uniform in space ; it\'s the good choice when the flow is open (e.g. with pressure boundary conditions).
// XD attr lambda field_base lambda_u 1 Conductivity (W.m-1.K-1).
// XD attr mu field_base mu 1 Dynamic viscosity (kg.m-1.s-1).

Sortie& Fluide_Weakly_Compressible::printOn(Sortie& os) const { return Fluide_Dilatable_base::printOn(os); }

Entree& Fluide_Weakly_Compressible::readOn(Entree& is)
{
  traitement_PTh = 2; /* par default */
  return Fluide_Dilatable_base::readOn(is);
}

void Fluide_Weakly_Compressible::set_param(Param& param)
{
  Fluide_Dilatable_base::set_param(param);
  param.ajouter("pression_thermo", &Pth_); // XD_ADD_P double Initial thermo-dynamic pressure used in the assosciated state law.
  param.ajouter("pression_xyz", &Pth_xyz_); // XD_ADD_P field_base Initial thermo-dynamic pressure used in the assosciated state law. It should be defined with as a Champ_Fonc_xyz.
  param.ajouter("use_total_pressure", &use_total_pressure_); // XD_ADD_P entier Flag (0 or 1) used to activate and use the total pressure in the assosciated state law. The default value of this Flag is 0.
  param.ajouter("use_hydrostatic_pressure", &use_hydrostatic_pressure_); // XD_ADD_P entier Flag (0 or 1) used to activate and use the hydro-static pressure in the assosciated state law. The default value of this Flag is 0.
  param.ajouter("use_grad_pression_eos", &use_grad_pression_eos_); // XD_ADD_P entier Flag (0 or 1) used to specify whether or not the gradient of the thermo-dynamic pressure will be taken into account in the source term of the temperature equation (case of a non-uniform pressure). The default value of this Flag is 1 which means that the gradient is used in the source.
  param.ajouter("time_activate_ptot", &time_activate_ptot_); // XD_ADD_P double Time (in seconds) at which the total pressure will be used in the assosciated state law.
}

/*! @brief Complete le fluide avec les champs inconnus associes au probleme
 *
 * @param (Pb_Thermohydraulique& pb) le probleme a resoudre
 */
void Fluide_Weakly_Compressible::completer(const Probleme_base& pb)
{
  Cerr << "Fluide_Weakly_Compressible::completer";

  if (Pth_ > -1.)
    Cerr << " Pth = " << Pth_ << finl;
  else
    Cerr << finl;

  if (use_total_hydro_pressure())
    checkTraitementPth(pb.equation(0).domaine_Cl_dis());

  if (Pth_xyz_.non_nul())
    {
      if (Pth_xyz_->que_suis_je() != "Champ_Fonc_xyz") // TODO : check if it is generic
        {
          Cerr << "Error in your data file : This is not allowed !" << finl;
          Cerr << "Pression_xyz should be defined with Champ_Fonc_xyz !" << finl;
          Process::exit();
        }
      if (Pth_ > -1. || Pth_n > -1.)
        {
          Cerr << "Error in your data file : This is not allowed !" << finl;
          Cerr << "You can not specify both pression_xyz and pression_thermo !" << finl;
          Process::exit();
        }
      if (use_total_hydro_pressure())
        {
          Cerr << "Error in your data file : This is not allowed !" << finl;
          Cerr << "You need to define pression_thermo when using the flags use_total_pressure or use_hydrostatic_pressure !" << finl;
          Process::exit();
        }
    }
  else // pression_xyz not specified in data file
    {
      if (Pth_ > -1.)
        Pth_n = Pth_;
      else
        {
          Cerr << "Error in your data file : This is not allowed !" << finl;
          Cerr << "You need to specify either pression_xyz or pression_thermo !" << finl;
          Process::exit();
        }

      if (use_total_pressure_ && time_activate_ptot_ == -1.)
        {
          Cerr << "Error in your data file : This is not allowed !" << finl;
          Cerr << "You should define time_activate_ptot when using the flag use_total_pressure !" << finl;
          Process::exit();
        }

      if (use_total_pressure_ && use_hydrostatic_pressure_)
        {
          Cerr << "Error in your data file : This is not allowed !" << finl;
          Cerr << "Not possible to activate the two flags use_total_pressure and use_hydrostatic_pressure at same time !" << finl;
          Process::exit();
        }

      if (use_hydrostatic_pressure_ && !a_gravite())
        {
          Cerr << "Error in your data file : This is not allowed !" << finl;
          Cerr << "Not possible to activate the flag use_hydrostatic_pressure without defining a gravity vector !" << finl;
          Process::exit();
        }
    }

  // XXX : On l'a besoin pour initialiser rho ...
  if (!use_saved_data())
    {
      if (use_pth_xyz())
        {
          Cerr << "Initializing the thermo-dynamic pressure Pth_xyz ..." << finl;
          int isVDF = 0; // VDF or VEF ??
          if (pb.equation(0).discretisation().que_suis_je() == "VDF")
            isVDF = 1;

          // We know that mu is always stored on elems and that Champ_Don rho_xyz_ is evaluated on elements
          assert(Pth_xyz_->valeurs().size() == viscosite_dynamique()->valeurs().size());

          if (isVDF) // Disc VDF => Pth_xyz_ on elems => we do nothing
            Pth_tab_ = Pth_n_tab_ = Pth_xyz_->valeurs();
          else
            {
              // we use rho for affecter because the field is on the faces in VEF (as rho)
              Champ_base& ch_rho = masse_volumique().valeur();
              ch_rho.affecter_(Pth_xyz_);
              Pth_tab_ = Pth_n_tab_ = ch_rho.valeurs();
            }
        }
      else
        {
          Cerr << "Initializing the thermo-dynamic pressure ..." << finl;
          // Pth_tab_ doit avoir meme dimensions que rho (elem en VDF et faces en VEF)
          Pth_tab_ = masse_volumique()->valeurs();
          const int n = Pth_tab_.dimension_tot(0);
          for (int i = 0; i < n; i++)
            Pth_tab_(i) = Pth_;
          Pth_n_tab_ = Pth_tab_;
        }
    }

  pression_eos_->valeurs() = Pth_tab_;

  Fluide_Dilatable_base::completer(pb);

  // le bon temps
  assert(pression_eos_.non_nul());
  pression_eos_->mettre_a_jour(le_probleme_->schema_temps().temps_courant());

  if (pression_hydro_.non_nul())
    pression_hydro_->mettre_a_jour(le_probleme_->schema_temps().temps_courant());

  if (unsolved_species_.non_nul())
    unsolved_species_->mettre_a_jour(le_probleme_->schema_temps().temps_courant());
}

void Fluide_Weakly_Compressible::checkTraitementPth(const Domaine_Cl_dis_base& domaine_cl)
{
  if (traitement_PTh == 0)
    {
      Cerr << "The EDO option for the pressure treatment is not yet supported for the Weakly_Compressible module !" << finl;
      Cerr << "Please use either conservation_masse or constant options !" << finl;
      Process::exit();
    }
  // appel a la classe mere
  Fluide_Dilatable_base::checkTraitementPth(domaine_cl);
}

void Fluide_Weakly_Compressible::discretiser(const Probleme_base& pb, const Discretisation_base& dis)
{
  const Domaine_dis_base& domaine_dis = pb.equation(0).domaine_dis();
  double temps = pb.schema_temps().temps_courant();

  // In *_Melange_Binaire_WC we do not even have a temperature variable ...
  // it is the species mass fraction Y1... Although named ch_temperature
  Champ_Don& ch_TK = ch_temperature();
  if (pb.que_suis_je() == "Pb_Hydraulique_Melange_Binaire_WC" || pb.que_suis_je() == "Pb_Hydraulique_Melange_Binaire_Turbulent_WC")
    dis.discretiser_champ("temperature", domaine_dis, "fraction_massique", "neant", 1, temps, ch_TK);
  else
    dis.discretiser_champ("temperature", domaine_dis, "temperature", "K", 1, temps, ch_TK);

  if (type_fluide() != "Gaz_Parfait")
    loi_etat()->champs_compris().ajoute_champ(ch_TK);

  Fluide_Dilatable_base::discretiser(pb, dis);

  // XXX XXX : Champs pour WC : comme la temperature car elem en VDF et faces en VEF
  Champ_Don& phydro = pression_hydro();
  dis.discretiser_champ("temperature", domaine_dis, "pression_hydro", "Pa", 1, temps, phydro);
  champs_compris_.ajoute_champ(phydro);

  Champ_Don& peos = pression_eos();
  dis.discretiser_champ("temperature", domaine_dis, "pression_eos", "Pa", 1, temps, peos);
  champs_compris_.ajoute_champ(peos);

  // Seulement pour multi-especes
  if (pb.que_suis_je() == "Pb_Thermohydraulique_Especes_WC")
    {
      Champ_Don& yn = fraction_massique_nonresolue();
      dis.discretiser_champ("temperature", domaine_dis, "fraction_massique_nonresolue", "neant", 1, temps, yn);
      champs_compris_.ajoute_champ(yn);
    }
}

void Fluide_Weakly_Compressible::abortTimeStep()
{
  Fluide_Dilatable_base::abortTimeStep();
  Pth_tab_ = Pth_n_tab_;
  Pth_ = Pth_n;
}

void Fluide_Weakly_Compressible::update_pressure_fields(double temps)
{
  Fluide_Dilatable_base::update_pressure_fields(temps);
  if (a_gravite())
    {
      calculer_pression_hydro();
      pression_hydro_->mettre_a_jour(temps);
    }
  // for post-processing
  pression_eos_->valeurs() = Pth_tab_;
  pression_eos_->mettre_a_jour(temps);
}

/*! @brief Calcule la pression totale : pression thermodynamique + pression hydrodynamique
 *
 */
void Fluide_Weakly_Compressible::calculer_pression_tot()
{
  Fluide_Dilatable_base::calculer_pression_tot();
  if (use_total_hydro_pressure())
    remplir_champ_pression_for_EOS();
}

void Fluide_Weakly_Compressible::remplir_champ_pression_tot(int n, const DoubleTab& PHydro, DoubleTab& PTot)
{
  P_NS_elem_ = PHydro; // pressure on elem for vdf & vef ...
  if (use_pth_xyz())
    for (int i = 0; i < n; i++)
      PTot(i, 0) = PHydro(i, 0) + Pth_tab_(i, 0);
  else
    for (int i = 0; i < n; i++)
      PTot(i, 0) = PHydro(i, 0) + Pth_;
}

/*! @brief Calcule la pression hydrostatique = rho*g*z
 *
 */
void Fluide_Weakly_Compressible::calculer_pression_hydro()
{
  DoubleTab& tab_Phydro = pression_hydro_->valeurs();
  const Domaine_dis_base& domaine_dis= pression_->domaine_dis_base();
  const Domaine_VF& domaine = ref_cast(Domaine_VF, domaine_dis);
  int is_VDF = domaine_dis.que_suis_je() == "Domaine_VDF" ? 1 : 0;
  const DoubleTab& centres_de_gravites = is_VDF ? domaine.xp() : domaine.xv();
  const DoubleTab& tab_rho = rho_np1(), grav = gravite().valeurs();
  const int n = tab_Phydro.dimension_tot(0);
  assert(n == tab_rho.dimension_tot(0) && a_gravite());
  for (int i = 0; i < n; i++)
    {
      double gz = 0.;
      for (int dir = 0; dir < dimension; dir++)
        gz += centres_de_gravites(i, dir) * grav(0, dir);

      tab_Phydro(i) = tab_rho(i) * gz;
    }
  if (use_total_hydro_pressure())
    remplir_champ_pression_for_EOS();
}

/*! @brief Calcule la pression utilisee dans les lois d'etat WC
 *
 */
void Fluide_Weakly_Compressible::remplir_champ_pression_for_EOS()
{
  if (use_total_pressure())
    {
      double t0 = le_probleme_->schema_temps().temps_courant(), t1 = le_probleme_->schema_temps().temps_futur(1);
      // time to activate ptot and change the eos pressure
      if (t0 < time_activate_ptot_ && t1 > time_activate_ptot_)
        {
          if (Pth_tab_.dimension_tot(0) == P_NS_elem_.dimension_tot(0)) // VDF
            for (int i = 0; i < Pth_tab_.dimension_tot(0); i++)
              Pth_tab_(i, 0) = P_NS_elem_(i, 0) + Pth_;
          else // VEF : on verra le jour ou on fait du PolyMAC_P0P1NC
            {
              // on a P_NS_elem_ aux elems et Pth_tab_ comme rho, i.e aux faces
              const Domaine_VF& zvf = ref_cast(Domaine_VF, inco_chaleur().domaine_dis_base());
              assert(Pth_tab_.dimension_tot(0) == zvf.nb_faces_tot());

              const IntTab& elem_faces = zvf.elem_faces();
              const DoubleVect& volumes = zvf.volumes();
              const int nb_face_elem = elem_faces.dimension(1), nb_elem_tot = zvf.nb_elem_tot();

              Pth_tab_ = 0.;
              DoubleTab vol_tot(Pth_tab_);
              for (int ele = 0; ele < nb_elem_tot; ele++)
                for (int s = 0; s < nb_face_elem; s++)
                  {
                    Pth_tab_(elem_faces(ele, s)) += P_NS_elem_(ele, 0) * volumes(ele);
                    vol_tot(elem_faces(ele, s)) += volumes(ele);
                  }
              for (int f = 0; f < zvf.nb_faces(); f++)
                Pth_tab_(f) /= vol_tot(f);

              // enfin ajoute Pth_ !
              Pth_tab_ += Pth_;
            }
        }
    }
  else if (use_hydrostatic_pressure())
    {
      if (le_probleme_->schema_temps().nb_pas_dt() == 0 && use_saved_data())
        { /* do nothing &  use saved data */ }
      else
        {
          Pth_tab_ = pression_hydro_->valeurs();
          const int n = Pth_tab_.dimension_tot(0);
          for (int i = 0; i < n; i++)
            Pth_tab_(i) += Pth_; // present dt
        }
    }
  else
    {
      Cerr << "The method Fluide_Weakly_Compressible::remplir_champ_pression_for_EOS() should not be called !" << finl;
      Process::exit();
    }
}

void Fluide_Weakly_Compressible::write_header_edo()
{
  if (je_suis_maitre())
    {
      SFichier fic(output_file_);
      if (Pth_ > -1. && !use_total_pressure() && !use_hydrostatic_pressure())
        fic << "# Time sum(T*dv)/sum(dv)[K] sum(rho*dv)/sum(dv)[kg/m3] Pth[Pa]" << finl;
      else
        fic << "# Time sum(T*dv)/sum(dv)[K] sum(rho*dv)/sum(dv)[kg/m3] Mean_Pth[Pa]" << finl;
    }
}

void Fluide_Weakly_Compressible::write_mean_edo(double temps)
{
  const double Ch_m = eos_tools_->moyenne_vol(inco_chaleur_->valeurs());
  const double rhom = eos_tools_->moyenne_vol(rho->valeurs());

  if (je_suis_maitre() && traitement_PTh != 2)
    {
      SFichier fic(output_file_, ios::app);
      if (Pth_ > -1. && !use_total_pressure() && !use_hydrostatic_pressure())
        fic << temps << " " << Ch_m << " " << rhom << " " << Pth_ << finl;
      else
        fic << temps << " " << Ch_m << " " << rhom << " " << eos_tools_->moyenne_vol(Pth_tab_) << finl;
    }
}
