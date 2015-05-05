/****************************************************************************
* Copyright (c) 2015, CEA
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
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Dift_VDF_Face_base.h>
#include <Eval_VDF_Face.h>
#include <Eval_Diff_VDF.h>
#include <SFichier.h>

Implemente_base(Op_Dift_VDF_Face_base,"Op_Dift_VDF_Face_base",Op_Dift_VDF_base);

//// printOn
//



Sortie& Op_Dift_VDF_Face_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//


Entree& Op_Dift_VDF_Face_base::readOn(Entree& s )
{
  return s ;
}


/////////////////////////////////////////////////////////
//
// Fonctions de la classe Op_Dift_VDF_Face_base
//
////////////////////////////////////////////////////////

// Description:
// complete l'iterateur et l'evaluateur
void Op_Dift_VDF_Face_base::associer(const Zone_dis& zone_dis,
                                     const Zone_Cl_dis& zone_cl_dis,
                                     const Champ_Inc& ch_diffuse)
{
  const Champ_Face& inco = ref_cast(Champ_Face,ch_diffuse.valeur());
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  iter.associer(zvdf, zclvdf, *this);
  Evaluateur_VDF& eval_diff =  iter.evaluateur();

  eval_diff.associer_zones(zvdf, zclvdf );
  //(dynamic_cast<Eval_VDF_Face&>(eval_diff)).associer_inconnue(inco );
  get_eval_face().associer_inconnue(inco );
}


// Description:
// associe le champ de diffusivite a l'evaluateur
void Op_Dift_VDF_Face_base::associer_diffusivite(const Champ_base& ch_diff)
{
  Eval_Diff_VDF& eval_diff_turb = (Eval_Diff_VDF&) iter.evaluateur();
  eval_diff_turb.associer(ch_diff);
}

const Champ_base& Op_Dift_VDF_Face_base::diffusivite() const
{
  const Eval_Diff_VDF& eval_diff_turb =
    (Eval_Diff_VDF&) iter.evaluateur();
  return eval_diff_turb.get_diffusivite();
}


void Op_Dift_VDF_Face_base::associer_loipar(const Turbulence_paroi& loi_paroi)
{
  //loipar = loi_paroi;
}


double Op_Dift_VDF_Face_base::calculer_dt_stab() const
{

  const Zone_VDF& zone_VDF = iter.zone();
  return calculer_dt_stab(zone_VDF);
}
double Op_Dift_VDF_Face_base::calculer_dt_stab(const Zone_VDF& zone_VDF) const
{
  double dt_stab;
  double coef;
  //  const IntTab& elem_faces = zone_VDF.elem_faces();
  const DoubleVect& diffu = diffusivite().valeurs();
  const DoubleVect& diffu_turb = la_diffusivite_turbulente->valeurs();

  // Calcul du pas de temps de stabilite :
  //
  //
  //  - La diffusivite est non constante donc:
  //
  //     dt_stab = Min (1/(2*(diffu_lam(i)+diffu_turb(i))*coeff(i))
  //
  //     avec :
  //            coeff =  1/(dx*dx) + 1/(dy*dy) + 1/(dz*dz)
  //
  //            i decrivant l'ensemble des elements du maillage
  //
  //  Rq: en hydraulique on cherche le Max sur les elements du maillage
  //      initial (comme en thermique) et non le Max sur les volumes de Qdm.

  coef= -1.e10;

  // B.Mat. 9/3/2005: pour traiter monophasique/qc/front-tracking de facon
  // generique. Mettre a jour le qc et l'ancien ft pour utiliser ce mecanisme
  int elem;
  const int nb_elem = zone_VDF.nb_elem();
  const int dim     = Objet_U::dimension;

  if (has_champ_masse_volumique())
    {
      const DoubleTab& valeurs_rho = get_champ_masse_volumique().valeurs();
      for (elem = 0; elem < nb_elem; elem++)
        {
          int i;
          double diflo = 0.;

          for (i = 0; i < dim; i++)
            {
              const double h = zone_VDF.dim_elem(elem, i);
              diflo += 1. / (h * h);
            }
          const double mu_physique  = diffu(elem);
          const double mu_turbulent = diffu_turb(elem);
          const double inv_rho = 1./valeurs_rho(elem) ;
          diflo *= (mu_physique + mu_turbulent) * inv_rho;
          coef = max(coef, diflo);
        }

    }
  else
    {

      const Champ_base& champ_diffu = diffusivite_pour_pas_de_temps();
      const DoubleVect& diffu_dt = champ_diffu.valeurs();
      const int diffu_dt_variable = (diffu_dt.size() == 1) ? 0 : 1;
      const int diffu_variable = (diffu.size() == 1) ? 0 : 1;
      for (elem = 0; elem < nb_elem; elem++)
        {
          int i;
          double diflo = 0.;

          for (i = 0; i < dim; i++)
            {
              const double h = zone_VDF.dim_elem(elem, i);
              diflo += 1. / (h * h);
            }
          const double mu_physique  = (diffu_variable ? diffu(elem):diffu(0));
          const double mu_turbulent = diffu_turb(elem);
          const double diffu_dt_l = (diffu_dt_variable ? diffu_dt(elem) : diffu_dt(0));
          // si on a associe mu au lieu de nu , on a nu sans diffu_dt
          // le pas de temps de stab est nu+nu_t, on calcule (mu+mu_t)*(nu/mu)=(mu+mu_t)/rho=nu+nu_t (avantage par rapport a la division par rho ca marche aussi pour alpha et lambda et en VEF
          diflo *= (mu_physique + mu_turbulent)*(diffu_dt_l)/mu_physique ;
          coef = max(coef, diflo);
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
  const int diffu_variable = (diffu.size() == 1) ? 0 : 1;
  const double diffu_constante = (diffu_variable ? 0. : diffu(0));
  int nb_elem = zone_VDF.nb_elem();
  for (int elem=0; elem<nb_elem; elem++)
    {
      double h_inv=0;
      for (int dir=0; dir<dimension; dir++)
        {
          double h=zone_VDF.dim_elem(elem,dir);
          h_inv+=1/(h*h);
        }
      double diffu_l = diffu_variable ? diffu(elem) : diffu_constante;
      double coef = 1/(2*(dt+DMINFLOAT)*h_inv*dt_diff_sur_dt_conv) - diffu_l;

      if (coef>0 && coef<borne_visco_turb(elem))
        borne_visco_turb(elem) = coef;
    }
}

// Description:
// on dimensionne notre matrice.
void Op_Dift_VDF_Face_base::dimensionner(Matrice_Morse& matrice) const
{
  /* GF Si avec les solveurs implicite et simple
     if (sub_type(Schema_Euler_Implicite,equation().schema_temps()))
     {
     Cerr << "Les schemas implicites ne sont pas disponibles en VDF avec un modele de turbulence." << finl;
     exit();
     }
  */
  Op_VDF_Face::dimensionner(iter.zone(), iter.zone_Cl(), matrice);
}

