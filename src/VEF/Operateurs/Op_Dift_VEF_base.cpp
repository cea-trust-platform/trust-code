/****************************************************************************
* Copyright (c) 2020, CEA
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

#include <Op_Dift_VEF_base.h>
#include <Champ_Fonc.h>
#include <Mod_turb_hyd_base.h>
#include <Modele_turbulence_scal.h>
#include <Navier_Stokes_std.h>
#include <Zone_Cl_VEF.h>

Implemente_base(Op_Dift_VEF_base,"Op_Dift_VEF_base",Op_Diff_VEF_base);

Sortie& Op_Dift_VEF_base::printOn(Sortie& is) const
{
  return Op_Diff_VEF_base::printOn(is);
}

Entree& Op_Dift_VEF_base::readOn(Entree& is)
{
  Op_Diff_VEF_base::readOn(is);
  return is;
}

void Op_Dift_VEF_base::associer_modele_turbulence(const Mod_turb_hyd_base& mod)
{
  le_modele_turbulence    = mod;
}

//

void Op_Dift_VEF_base::mettre_a_jour(double )
{
  if (sub_type(Navier_Stokes_std,equation())) // on traite l'hydraulique
    {
      if ( le_modele_turbulence->loi_paroi().non_nul())
        if (le_modele_turbulence->loi_paroi()->use_shear())
          {
            // Modif BM: on ne prend la ref que si le tableau a ete initialise, sinon ca bloque
            // l'initialisation
            const DoubleTab& tab = le_modele_turbulence->loi_paroi().valeur().Cisaillement_paroi();
            if (tab.size_array() > 0)
              tau_tan_.ref(tab);
          }
    }
}

void Op_Dift_VEF_base::associer(const Zone_dis& zone_dis,
                                const Zone_Cl_dis& zone_cl_dis,
                                const Champ_Inc& ch_transporte)
{
  const Zone_VEF& zvef = ref_cast(Zone_VEF,zone_dis.valeur());
  const Zone_Cl_VEF& zclvef = ref_cast(Zone_Cl_VEF,zone_cl_dis.valeur());
  la_zone_vef = zvef;
  la_zcl_vef = zclvef;
  inconnue_ = ch_transporte;
}

void Op_Dift_VEF_base::completer()
{
  Operateur_base::completer();

  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  if (modele_turbulence.non_nul() && sub_type(Mod_turb_hyd_base,modele_turbulence.valeur()))
    {
      const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
      const Champ_Fonc& viscosite_turbulente = mod_turb.viscosite_turbulente();
      associer_diffusivite_turbulente(viscosite_turbulente);
      associer_modele_turbulence(mod_turb);
    }
  else if (sub_type(Modele_turbulence_scal_base,modele_turbulence.valeur()))
    {
      const Modele_turbulence_scal_base& modele_turbulence_scalaire = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
      const Champ_Fonc& conductivite_turbulente = modele_turbulence_scalaire.conductivite_turbulente();
      associer_diffusivite_turbulente(conductivite_turbulente);
    }
  else
    {
      Cerr << "Error in Op_Dift_VEF_Face::completer() " << finl;
      Cerr <<  que_suis_je() <<" operator is presently associated to "<<equation().que_suis_je()<<finl;
      Cerr << "instead of being associated to an equation dedicated to a turbulent flow."<<finl;
      exit();
    }
}

void Op_Dift_VEF_base::calculer_borne_locale(DoubleVect& borne_visco_turb, double dt_conv, double dt_diff_sur_dt_conv) const
{
  const Zone_VEF& la_zone_VEF = la_zone_vef.valeur();
  int nb_elem = la_zone_VEF.nb_elem();
  int flag = diffusivite().valeurs().dimension(0)>1 ? 1 : 0;
  for (int elem=0; elem<nb_elem; elem++)
    {
      double h_inv = 1./la_zone_VEF.carre_pas_maille(elem);
      // C'est pas tres propre pour recuperer diffu mais ca evite de coder cette methode dans plusieurs classes:
      double diffu = (flag ? diffusivite().valeurs()(elem) : diffusivite().valeurs()(0,0));
      //
      double coef = 1./(2*(dt_conv+DMINFLOAT)*dimension*h_inv*dt_diff_sur_dt_conv) - diffu;
      if (coef>0 && coef<borne_visco_turb(elem))
        borne_visco_turb(elem) = coef;
    }
}
