/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Convection_Diffusion_Chaleur_Turbulent_QC.h>
#include <Navier_Stokes_Turbulent_QC.h>
#include <Fluide_Quasi_Compressible.h>
#include <Assembleur_base.h>
#include <Probleme_base.h>
#include <Discret_Thyd.h>
#include <Param.h>

Implemente_instanciable(Navier_Stokes_Turbulent_QC, "Navier_Stokes_Turbulent_QC", Navier_Stokes_Turbulent);
// XD navier_stokes_turbulent_qc navier_stokes_turbulent navier_stokes_turbulent_qc -1 Navier-Stokes equations under low Mach number as well as the associated turbulence model equations.

Sortie& Navier_Stokes_Turbulent_QC::printOn(Sortie& is) const { return Equation_base::printOn(is); }

Entree& Navier_Stokes_Turbulent_QC::readOn(Entree& is)
{
  assert(le_fluide.non_nul());
  if (!sub_type(Fluide_Quasi_Compressible, le_fluide.valeur()))
    {
      Cerr << "ERROR: the equation " << this->que_suis_je() << " can be associated only to a quasi-compressible fluid." << finl;
      exit();
    }
  Navier_Stokes_Turbulent::readOn(is);
  divergence.set_description((Nom) "Mass flow rate=Integral(rho*u*ndS) [kg.s-1]");
  return is;
}

const Champ_Don& Navier_Stokes_Turbulent_QC::diffusivite_pour_transport() const
{
  return fluide().viscosite_dynamique();
}

/*! @brief appel Navier_Stokes_Turbulent::mettre_a_jour et Convection_Diffusion_Chaleur_Turbulent_QC::mettre_a_jour_modele
 *
 */
void Navier_Stokes_Turbulent_QC::mettre_a_jour(double temps)
{
  Navier_Stokes_Turbulent::mettre_a_jour(temps);
  // Elie Saikali
  // for Pb_Hydraulique_Melange_Binaire_Turbulent_QC we do all in mettre_a_jour(temps) !
  if (probleme().que_suis_je() != "Pb_Hydraulique_Melange_Binaire_Turbulent_QC")
    ref_cast(Convection_Diffusion_Chaleur_Turbulent_QC,probleme().equation(1)).mettre_a_jour_modele(temps);
}

int Navier_Stokes_Turbulent_QC::impr(Sortie& os) const
{
  Navier_Stokes_Fluide_Dilatable_Proto::impr_impl(*this, os);
  return Navier_Stokes_Turbulent::impr(os);
}

void Navier_Stokes_Turbulent_QC::imprimer(Sortie& os) const
{
  Navier_Stokes_Turbulent::imprimer(os);
  return;
}

void Navier_Stokes_Turbulent_QC::discretiser()
{
  Navier_Stokes_Turbulent::discretiser();
  const Discret_Thyd& dis = ref_cast(Discret_Thyd, discretisation());
  dis.vitesse(schema_temps(), domaine_dis(), rho_la_vitesse_);
  rho_la_vitesse_.nommer("rho_u");
  rho_la_vitesse_.valeur().nommer("rho_u");
}

/*! @brief Appels successifs a: Navier_Stokes_std::completer()
 *
 *       Mod_Turb_Hyd::completer() [sur le membre concerne]
 *
 */
void Navier_Stokes_Turbulent_QC::completer()
{
  if (le_fluide->a_gravite())
    {
      Cerr << "Gravity=" << le_fluide->gravite() << finl;
      //l'equation de NS en quasi compressible peut contenir un terme source de gravite
      if (le_fluide->beta_t().non_nul())    //pour debogage    A ENLEVER APRES
        {
          Cerr << "Beta_t !=0  -> Boussinesq is currently used :" << finl;
        }
      else
        {
          Cerr << "Creation of the buoyancy source term for the Navier_Stokes_Turbulent_QC equation :" << finl;
          Source t;
          Source& so = les_sources.add(t);
          Nom type_so = "Source_QC_Gravite_";
          Nom disc = discretisation().que_suis_je();
          if (disc == "VEFPreP1B")
            disc = "VEF";
          type_so += disc;
          so.typer_direct(type_so);
          so->associer_eqn(*this);
          Cerr << so->que_suis_je() << finl;
        }
    }

  Navier_Stokes_Turbulent::completer();
}

/*! @brief cf Equation_base::preparer_calcul() Assemblage du solveur pression et
 *
 *      initialisation de la pression.
 *
 * @return (int) renvoie toujours 1
 */
int Navier_Stokes_Turbulent_QC::preparer_calcul()
{
  return Navier_Stokes_Turbulent::preparer_calcul();
  /*
   // Cerr << "Navier_Stokes_Fluide_Dilatable_Proto::preparer_calcul()" << finl;
   Equation_base::preparer_calcul();
   //solveur_pression->assembler_QC(le_fluide->masse_volumique().valeurs());
   assembleur_pression_->assembler_QC(le_fluide->masse_volumique().valeurs(),matrice_pression_);


   if (le_traitement_particulier.non_nul())
   le_traitement_particulier.preparer_calcul_particulier();


   la_pression.valeurs()=0.;
   la_pression.changer_temps(schema_temps().temps_courant());
   Cerr << "Projection of initial and boundaries conditions" << finl;

   projeter();
   le_modele_turbulence.preparer_calcul();

   return 1;*/
}

const Champ_base& Navier_Stokes_Turbulent_QC::get_champ(const Motcle& nom) const
{

  if (nom == "rho_u")
    return rho_la_vitesse().valeur();
  try
    {
      return Navier_Stokes_Turbulent::get_champ(nom);
    }
  catch (Champs_compris_erreur&)
    {
    }

  try

    {
      return milieu().get_champ(nom);
    }
  catch (Champs_compris_erreur&)
    {
    }
  throw Champs_compris_erreur();

}

/*! @brief Calcule la derivee en temps de l'inconnue vitesse, i.
 *
 * e. l'acceleration dU/dt et la renvoie.
 *     Appelle Equation_base::derivee_en_temps_inco(DoubleTab& )
 *     Calcule egalement la pression.
 *
 * @param (DoubleTab& vpoint) le tableau des valeurs de l'acceleration dU/dt
 * @return (DoubleTab&) le tableau des valeurs de l'acceleration (derivee de la vitesse)
 */
DoubleTab& Navier_Stokes_Turbulent_QC::derivee_en_temps_inco(DoubleTab& vpoint)
{
  return Navier_Stokes_Fluide_Dilatable_Proto::derivee_en_temps_inco_impl(*this, vpoint);
}

void Navier_Stokes_Turbulent_QC::assembler(Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem)
{
  return Navier_Stokes_Fluide_Dilatable_Proto::assembler_impl(mat_morse, present, secmem);
}
void Navier_Stokes_Turbulent_QC::assembler_avec_inertie(Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem)
{
  return Navier_Stokes_Fluide_Dilatable_Proto::assembler_avec_inertie_impl(*this, mat_morse, present, secmem);
}

bool Navier_Stokes_Turbulent_QC::initTimeStep(double dt)
{

  DoubleTab& tab_vitesse = inconnue().valeurs();

  Fluide_Quasi_Compressible& fluide_QC = ref_cast(Fluide_Quasi_Compressible, le_fluide.valeur());

  const DoubleTab& tab_rho = fluide_QC.rho_discvit();

  DoubleTab& rhovitesse = rho_la_vitesse_.valeurs();
  Navier_Stokes_Fluide_Dilatable_Proto::rho_vitesse_impl(tab_rho, tab_vitesse, rhovitesse);

  return Navier_Stokes_Turbulent::initTimeStep(dt);
}
