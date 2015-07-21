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
// File:        Op_Diff_EF_base.cpp
// Directory:   $TRUST_ROOT/src/EF/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_EF_base.h>
#include <Champ_Uniforme.h>
#include <Milieu_base.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Zone_EF.h>
#include <Zone_Cl_EF.h>
#include <Champ_Fonc_P0_base.h>
#include <Domaine.h>

Implemente_base_sans_constructeur(Op_Diff_EF_base,"Op_Diff_EF_base",Operateur_Diff_base);

Op_Diff_EF_base::Op_Diff_EF_base()
{
  declare_support_masse_volumique(1);
}

//// printOn
//

Sortie& Op_Diff_EF_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Op_Diff_EF_base::readOn(Entree& s )
{
  return s ;
}

int Op_Diff_EF_base::impr(Sortie& os) const
{
  return Op_EF_base::impr(os, *this);
}

void Op_Diff_EF_base::associer(const Zone_dis& zone_dis,
                               const Zone_Cl_dis& zone_cl_dis,
                               const Champ_Inc& ch_transporte)
{

  const Zone_EF& zEF = ref_cast(Zone_EF,zone_dis.valeur());
  const Zone_Cl_EF& zclEF = ref_cast(Zone_Cl_EF,zone_cl_dis.valeur());

  /*  if (sub_type(Champ_P1_VEF,ch_transporte.valeur()))
    {
      const Champ_P1_VEF& inco = ref_cast(Champ_P1_VEF,ch_transporte.valeur());
      REF(Champ_P1_VEF) inconnue;
      inconnue = inco;
    }
  if (sub_type(Champ_Q1_EF,ch_transporte.valeur()))
    {
      const Champ_Q1_EF& inco = ref_cast(Champ_Q1_EF,ch_transporte.valeur());
      REF(Champ_Q1_EF) inconnue;
      inconnue = inco;
    }
  */
  la_zone_EF = zEF;
  la_zcl_EF = zclEF;


  const Zone_EF& zone_EF = la_zone_EF.valeur();

  int nb_comp = 1;
  //  int nb_dim = resu.nb_dim();
  //  if(nb_dim==2)
  //    nb_comp=resu.dimension(1);

  (ref_cast(DoubleTab,flux_bords_)).resize(zone_EF.nb_faces_bord(),nb_comp);
  flux_bords_=0.;
}
double Op_Diff_EF_base::calculer_dt_stab() const
{
  remplir_nu(nu_);
  // La diffusivite est constante dans le domaine donc
  //
  //          dt_diff = h*h/diffusivite

  double dt_stab = DMAXFLOAT;
  const Zone_EF& zone_EF = la_zone_EF.valeur();
  if (! has_champ_masse_volumique())
    {
      if (1)
        {
          // Methode "standard" de calcul du pas de temps
          // Ce calcul est tres conservatif: si le max de la diffusivite
          // n'est pas atteint a l'endroit ou le min de delta_h_carre est atteint,
          // le pas de temps est sous-estime.
          const Champ_base& champ_diffusivite = diffusivite_pour_pas_de_temps();
          const DoubleVect&      valeurs_diffusivite = champ_diffusivite.valeurs();
          double alpha_max = local_max_vect(valeurs_diffusivite);

          if ((alpha_max == 0.)||(zone_EF.nb_elem()==0))
            dt_stab = DMAXFLOAT;
          else
            {
              const double min_delta_h_carre = zone_EF.carre_pas_du_maillage();
              dt_stab = min_delta_h_carre / (2. * dimension * alpha_max);
            }
        }
      else
        {
          const Zone_EF& zone_ef=ref_cast(Zone_EF,equation().zone_dis().valeur());
          const DoubleTab& coord=zone_ef.zone().domaine().les_sommets();
          const IntTab& elems=zone_ef.zone().les_elems() ;
          const Champ_base& champ_diffusivite = diffusivite_pour_pas_de_temps();
          const DoubleVect&      valeurs_diffusivite = champ_diffusivite.valeurs();
          const int nb_elem = zone_EF.nb_elem();
          int elem;
          // Champ de masse volumique variable.
          for (elem = 0; elem < nb_elem; elem++)
            {

              const double diffu   = valeurs_diffusivite(elem)+DMINFLOAT;
              double dx2=0,dl;
              for (int d=0; d<3; d++)
                {
                  dl=coord(elems(elem,0),d)-coord(elems(elem,7),d);
                  dx2+=(dl*dl);
                }
              const double dt      = dx2 /  diffu;
              if (dt_stab > dt)
                {
                  dt_stab = dt;
                  // Cerr<<" ii "<<elem<<" "<<diffu<<" "<<dx2<<finl;
                }
            }
        }
    }
  else
    {
      const double           deux_dim      = 2. * Objet_U::dimension;
      const Champ_base& champ_diffu   = diffusivite();
      const DoubleTab&       valeurs_diffu = champ_diffu.valeurs();
      const Champ_base&      champ_rho     = get_champ_masse_volumique();
      const DoubleTab&       valeurs_rho   = champ_rho.valeurs();
      assert(sub_type(Champ_Fonc_P0_base, champ_rho));
      assert(sub_type(Champ_Fonc_P0_base, champ_diffu));
      const int nb_elem = zone_EF.nb_elem();
      int elem;
      // Champ de masse volumique variable.
      for (elem = 0; elem < nb_elem; elem++)
        {
          const double h_carre = zone_EF.carre_pas_maille(elem);
          const double diffu   = valeurs_diffu(elem);
          const double rho     = valeurs_rho(elem);
          const double dt      = h_carre * rho / (deux_dim * diffu);
          if (dt_stab > dt)
            dt_stab = dt;
        }
    }
  dt_stab = Process::mp_min(dt_stab);
  return dt_stab;
}
