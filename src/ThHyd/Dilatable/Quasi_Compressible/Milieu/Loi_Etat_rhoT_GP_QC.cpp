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

#include <Fluide_Dilatable_base.h>
#include <Loi_Etat_rhoT_GP_QC.h>
#include <TRUSTTab.h>
#include <Param.h>
#include <ParserView.h>

Implemente_instanciable_sans_constructeur( Loi_Etat_rhoT_GP_QC, "Loi_Etat_rhoT_Gaz_Parfait_QC", Loi_Etat_GP_base ) ;
// XD rhoT_gaz_parfait_QC loi_etat_gaz_parfait_base rhoT_gaz_parfait_QC -1 Class for perfect gas used with a quasi-compressible fluid where the state equation is defined as rho = f(T).

Loi_Etat_rhoT_GP_QC::Loi_Etat_rhoT_GP_QC() : is_exp_(false) { }

Sortie& Loi_Etat_rhoT_GP_QC::printOn( Sortie& os ) const
{
  Loi_Etat_GP_base::printOn( os );
  return os;
}

Entree& Loi_Etat_rhoT_GP_QC::readOn( Entree& is )
{
  Nom expression_;

  Param param(que_suis_je());
  param.ajouter("Cp",&Cp_,Param::REQUIRED);// XD_ADD_P double Specific heat at constant pressure of the gas Cp.
  param.ajouter("Prandtl",&Pr_); // XD_ADD_P double Prandtl number of the gas Pr=mu*Cp/lambda
  param.ajouter("rho_xyz",&rho_xyz_); // XD_ADD_P field_base Defined with a Champ_Fonc_xyz to define a constant rho with time (space dependent)
  param.ajouter("rho_t",&expression_); // XD_ADD_P chaine Expression of T used to calculate rho. This can lead to a variable rho, both in space and in time.
  param.ajouter("T_min",&TMIN_); // XD_ADD_P double Temperature may, in some cases, locally and temporarily be very small (and negative) even though computation converges. T_min keyword allows to set a lower limit of temperature (in Kelvin, -1000 by default). WARNING: DO NOT USE THIS KEYWORD WITHOUT CHECKING CAREFULY YOUR RESULTS!
  param.lire_avec_accolades(is);

  if (expression_ == "??" && rho_xyz_.est_nul())
    {
      Cerr << "Error in Loi_Etat_rhoT_GP_QC::readOn !" << finl;
      Cerr << "The closure equation of rho is not read in your data file !" << finl;
      Cerr << "Either use rho_t followed by an expression of T," << finl;
      Cerr << "or use rho_xyz followed by a Champ_Fonc_xyz !" << finl;
      Process::exit();
    }

  if (expression_ != "??")
    {
      is_exp_ = true;
      Cerr << "Parsing the expression " << expression_ << finl;
      parser_.setNbVar(1);
      parser_.setString(expression_);
      parser_.addVar("T");
      parser_.parseString();
    }
  else
    assert(rho_xyz_->que_suis_je() == "Champ_Fonc_xyz" );

  return is;
}

// Method used to initialize rho from a Champ_Fonc_xyz
// In this case the DoubleTab rho_ is initialized and remains constant with time
void Loi_Etat_rhoT_GP_QC::initialiser_rho()
{
  int isVDF = 0;
  if (le_fluide->masse_volumique().que_suis_je()=="Champ_Fonc_P0_VDF") isVDF = 1;
  // We know that mu is always stored on elems
  int nb_elems = le_fluide->viscosite_dynamique().valeurs().size();
  // The OWN_PTR(Champ_Don_base) rho_xyz_ is evaluated on elements
  assert (rho_xyz_->valeurs().size() == nb_elems);

  if (isVDF)
    {
      // Disc VDF => rho_ & rho_xyz_ on elems => we do nothing
      rho_.resize(nb_elems, 1);
      DoubleTab& fld = rho_xyz_->valeurs();
      for (int i=0; i<nb_elems; i++) rho_(i,0)=fld(i,0);
    }
  else
    {
      // Disc VEF => rho on faces ...
      int nb_faces =  le_fluide->masse_volumique().valeurs().size(); // rho on faces in VEF
      rho_.resize(nb_faces, 1);

      Champ_base& ch_rho = le_fluide->masse_volumique();
      ch_rho.affecter_(rho_xyz_);
      DoubleTab& fld = ch_rho.valeurs();
      for (int i = 0; i < nb_faces; i++) rho_(i,0)= fld(i,0);
    }
}

void Loi_Etat_rhoT_GP_QC::initialiser_inco_ch()
{
  // required here for xyz !
  if ( !is_exp_ ) initialiser_rho();
  Loi_Etat_base::initialiser_inco_ch();
}

// Override
double Loi_Etat_rhoT_GP_QC::calculer_masse_volumique(double P, double T) const
{
  /* Not useful for this state law */
  return -3000.;
}

// Overload
double Loi_Etat_rhoT_GP_QC::calculer_masse_volumique(double P, double T, int ind) const
{
  if (inf_ou_egal(T,TMIN_))
    {
      Cerr << finl << "Error, we find a temperature of " << T << " !" << finl;
      Cerr << "The minium of temperature is definied to " << TMIN_ << " !" << finl;
      Cerr << "Either your calculation has diverged or you don't define" << finl;
      Cerr << "temperature in Kelvin somewhere in your data file." << finl;
      Cerr << "It is mandatory for Quasi compressible model." << finl;
      Cerr << "Check your data file." << finl;
      Process::exit();
    }

  if (is_exp_)
    {
      parser_.setVar(0,T);
      return parser_.eval();
    }
  else
    {
      // Initialized from Champ_Fonc_xyz
      // dont change, return what is saved in rho_
      return rho_(ind,0);
    }
}

double Loi_Etat_rhoT_GP_QC::inverser_Pth(double T, double rho)
{
  abort();
  throw;
}

/*! @brief Recalcule la masse volumique
 *
 */
void Loi_Etat_rhoT_GP_QC::calculer_masse_volumique()
{
  const DoubleTab& tab_ICh = le_fluide->inco_chaleur().valeurs();
  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  int n=tab_rho.size();
  if (is_exp_)
    {
      double TMIN = TMIN_;
      ParserView parser(parser_);
      parser.parseString();
      CDoubleTabView ICh = tab_ICh.view_ro();
      CDoubleArrView rho_n = static_cast<const DoubleVect&>(tab_rho_n).view_ro();
      DoubleArrView rho_np1 = static_cast<DoubleVect&>(tab_rho_np1).view_wo();
      DoubleTabView rho = tab_rho.view_wo();
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), n, KOKKOS_LAMBDA(const int i)
      {
        double T = ICh(i, 0);
        if (T<=TMIN) Process::Kokkos_exit("Dumb temperature in Loi_Etat_rhoT_GP_QC::calculer_masse_volumique !");
        int threadId = parser.acquire();
        parser.setVar(0, T, threadId);
        rho_np1(i) = parser.eval(threadId);
        rho(i, 0) = 0.5 * (rho_n(i) + rho_np1(i));
        parser.release(threadId);
      });
      end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
    }
  else
    {
      CDoubleTabView rho = rho_.view_ro();
      CDoubleArrView rho_n = static_cast<const DoubleVect&>(tab_rho_n).view_ro();
      DoubleArrView rho_np1 = static_cast<DoubleVect&>(tab_rho_np1).view_wo();
      DoubleTabView masse_volumique = tab_rho.view_rw();
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), n, KOKKOS_LAMBDA(const int i)
      {
        rho_np1(i) = rho(i,0);
        masse_volumique(i, 0) = 0.5 * (rho_n(i) + rho_np1(i));
      });
      end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
    }
  tab_rho.echange_espace_virtuel();
  tab_rho_np1.echange_espace_virtuel();
  le_fluide->calculer_rho_face(tab_rho_np1);
}
