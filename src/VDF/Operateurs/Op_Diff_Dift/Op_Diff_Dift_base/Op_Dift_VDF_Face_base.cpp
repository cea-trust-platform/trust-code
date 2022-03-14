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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Op_Dift_VDF_Face_base.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Op_Diff_Dift/Op_Diff_Dift_base
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Dift_VDF_Face_base.h>

Implemente_base(Op_Dift_VDF_Face_base,"Op_Dift_VDF_Face_base",Op_Dift_VDF_base);

Sortie& Op_Dift_VDF_Face_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Dift_VDF_Face_base::readOn(Entree& s ) { return s ; }

double Op_Dift_VDF_Face_base::calculer_dt_stab() const
{
  const Zone_VDF& zone_VDF = iter.zone();
  return calculer_dt_stab(zone_VDF);
}

/*
 * Calcul du pas de temps de stabilite : Pour const/var Face
 * La diffusivite laminaire est constante ou variable, La diffusivite turbulente est variable
 *    dt_stab = Min (1/(2*(diff_lam(i)+diff_turb(i))*coeff(elem))
 *
 *  avec coeff =  1/(dx*dx) + 1/(dy*dy) + 1/(dz*dz) et i decrivant l'ensemble des elements du maillage
 *  Rq: en hydraulique on cherche le Max sur les elements du maillage initial (comme en thermique) et non le Max sur les volumes de Qdm.
 */
double Op_Dift_VDF_Face_base::calculer_dt_stab(const Zone_VDF& zone_VDF) const
{
  double dt_stab, coef = -1.e10;
  const DoubleTab& diffu = diffusivite().valeurs(), &diffu_turb = diffusivite_turbulente()->valeurs();

  // B.Mat. 9/3/2005: pour traiter monophasique/qc/front-tracking de facon generique. Mettre a jour le qc et l'ancien ft pour utiliser ce mecanisme
  const int nb_elem = zone_VDF.nb_elem(), dim = Objet_U::dimension;
  if (has_champ_masse_volumique())
    {
      const DoubleTab& valeurs_rho = get_champ_masse_volumique().valeurs();
      for (int elem = 0; elem < nb_elem; elem++)
        {
          double diflo = 0.;
          for (int i = 0; i < dim; i++)
            {
              const double h = zone_VDF.dim_elem(elem, i);
              diflo += 1. / (h * h);
            }
          double mu_physique = diffu(elem, 0),  mu_turbulent = diffu_turb(elem, 0);

          for (int ncomp = 1; ncomp < diffu.line_size(); ncomp++) mu_physique = std::max(mu_physique, diffu(elem, ncomp));
          for (int ncomp = 1; ncomp < diffu_turb.line_size(); ncomp++) mu_turbulent = std::max(mu_turbulent, diffu_turb(elem, ncomp));

          const double inv_rho = 1./valeurs_rho(elem) ;
          diflo *= (mu_physique + mu_turbulent) * inv_rho;
          coef = std::max(coef, diflo);
        }
    }
  else
    {
      const Champ_base& champ_diffu = diffusivite_pour_pas_de_temps();
      const DoubleTab& diffu_dt = champ_diffu.valeurs();
      const int diffu_dt_variable = (diffu_dt.dimension(0) == 1) ? 0 : 1, diffu_variable = (diffu.dimension(0) == 1) ? 0 : 1;
      for (int elem = 0; elem < nb_elem; elem++)
        {
          double diflo = 0.;
          for (int i = 0; i < dim; i++)
            {
              const double h = zone_VDF.dim_elem(elem, i);
              diflo += 1. / (h * h);
            }

          int item = (diffu_variable ? elem : 0);
          double mu_physique = diffu(item, 0), mu_turbulent = diffu_turb(elem, 0);

          for (int ncomp = 1; ncomp < diffu.line_size(); ncomp++) mu_physique = std::max(mu_physique, diffu(item, ncomp));
          for (int ncomp = 1; ncomp < diffu_turb.line_size(); ncomp++) mu_turbulent = std::max(mu_turbulent, diffu_turb(elem, ncomp));

          item = (diffu_dt_variable ? elem : 0);
          double diffu_dt_l = diffu_dt(item, 0);

          for (int ncomp = 1; ncomp < diffu_dt.line_size(); ncomp++) diffu_dt_l = std::max(diffu_dt_l, diffu_dt(item, ncomp));

          // si on a associe mu au lieu de nu , on a nu sans diffu_dt
          // le pas de temps de stab est nu+nu_t, on calcule (mu+mu_t)*(nu/mu)=(mu+mu_t)/rho=nu+nu_t (avantage par rapport a la division par rho ca marche aussi pour alpha et lambda et en VEF
          diflo *= (mu_physique + mu_turbulent)*(diffu_dt_l)/mu_physique ;
          coef = std::max(coef, diflo);
        }
    }
  coef = Process::mp_max(coef);
  dt_stab = 0.5 / (coef+DMINFLOAT);

  return dt_stab;
}

void Op_Dift_VDF_Face_base::calculer_borne_locale(DoubleVect& borne_visco_turb,double dt,double dt_diff_sur_dt_conv) const
{
  const Zone_VDF& zone_VDF = iter.zone();
  const Champ_base& champ_diffu = diffusivite();
  const DoubleVect& diffu = champ_diffu.valeurs();
  const int diffu_variable = (diffu.size() == 1) ? 0 : 1, nb_elem = zone_VDF.nb_elem();
  const double diffu_constante = (diffu_variable ? 0. : diffu(0));
  for (int elem=0; elem<nb_elem; elem++)
    {
      double h_inv = 0;
      for (int dir = 0; dir < dimension; dir++)
        {
          double h = zone_VDF.dim_elem(elem,dir);
          h_inv += 1/(h*h);
        }
      double diffu_l = diffu_variable ? diffu(elem) : diffu_constante;
      double coef = 1/(2*(dt+DMINFLOAT)*h_inv*dt_diff_sur_dt_conv) - diffu_l;

      if (coef>0 && coef<borne_visco_turb(elem)) borne_visco_turb(elem) = coef;
    }
}

void Op_Dift_VDF_Face_base::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : NULL, mat2;
  Op_VDF_Face::dimensionner(iter.zone(), iter.zone_Cl(), mat2);
  mat->nb_colonnes() ? *mat += mat2 : *mat = mat2;
}
