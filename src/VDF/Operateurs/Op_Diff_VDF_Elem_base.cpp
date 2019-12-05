/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Op_Diff_VDF_Elem_base.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_VDF_Elem_base.h>
#include <Champ_P0_VDF.h>

#include <Eval_Diff_VDF.h>
#include <Eval_VDF_Elem.h>
#include <Echange_contact_VDF.h>
#include <Array_tools.h>
#include <Matrix_tools.h>

Implemente_base_sans_constructeur(Op_Diff_VDF_Elem_base,"Op_Diff_VDF_Elem_base",Op_Diff_VDF_base);



//// printOn
//

Sortie& Op_Diff_VDF_Elem_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Op_Diff_VDF_Elem_base::readOn(Entree& s )
{
  return s ;
}

double Op_Diff_VDF_Elem_base::calculer_dt_stab() const
{
  // Calcul du pas de temps de stabilite :
  //
  //  - La  diffusivite est uniforme donc :
  //
  //     dt_stab = 1/(2*diffusivite*Max(coef(elem)))
  //
  //     avec:
  //           coef = 1/(dx*dx) + 1/(dy*dy) + 1/(dz*dz)
  //
  //           i decrivant l'ensemble des elements du maillage
  //
  // Rq : On ne balaie pas l'ensemble des elements puisque
  //      le max de coeff est atteint sur l'element qui realise
  //      a la fois le min de dx le min de dy et le min de dz
  double dt_stab = DMAXFLOAT;
  const Zone_VDF& zone_VDF = iter.zone();
  const DoubleTab& diffu = has_champ_masse_volumique() ? diffusivite().valeurs() : diffusivite_pour_pas_de_temps().valeurs();


  if (sub_type(Champ_Uniforme,diffusivite_pour_pas_de_temps()) && !has_champ_masse_volumique())
    {
      // GF le max permet de traiter le multi_inco
      double alpha=max_array(diffu);

      double coef = 1/(zone_VDF.h_x()*zone_VDF.h_x())
                    + 1/(zone_VDF.h_y()*zone_VDF.h_y());

      if (dimension == 3)
        coef += 1/(zone_VDF.h_z()*zone_VDF.h_z());
      if (alpha==0)
        dt_stab = DMAXFLOAT;
      else
        dt_stab = 0.5/(alpha*coef);
    }
  else
    {
      double alpha_loc;
      int nbdim=diffu.nb_dim();
      int nb_elem=diffu.dimension(0);
      for (int elem=0 ; elem<nb_elem; elem++)
        {
          double h = 0;
          for (int i=0 ; i<dimension; i++)
            {
              double l = zone_VDF.dim_elem(elem,i);
              h += 1./(l*l);
            }
          if (nbdim==1)
            alpha_loc=diffu(elem);
          else
            {
              alpha_loc = diffu(elem,0);
              for (int ncomp=1; ncomp<diffu.dimension(1); ncomp++)
                alpha_loc = max(alpha_loc,diffu(elem,ncomp));
            }
          if (has_champ_masse_volumique())
            {
              const DoubleTab& rho = get_champ_masse_volumique().valeurs();
              alpha_loc/= rho(elem);
            }
          double dt_loc = 0.5/((alpha_loc+DMINFLOAT)*h);
          if (dt_loc<dt_stab)
            dt_stab = dt_loc;
        }
    }

  return Process::mp_min(dt_stab);
}


// Description:
// complete l'iterateur et l'evaluateur
void Op_Diff_VDF_Elem_base::associer(const Zone_dis& zone_dis,
                                     const Zone_Cl_dis& zone_cl_dis,
                                     const Champ_Inc& ch_diffuse)
{
  const Champ_P0_VDF& inco = ref_cast(Champ_P0_VDF,ch_diffuse.valeur());
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  iter.associer(zvdf, zclvdf, *this);

  Evaluateur_VDF& iter_vdf=iter.evaluateur();
  iter_vdf.associer_zones(zvdf, zclvdf );          // Evaluateur_VDF::associer_zones
  // ((Eval_VDF_Elem&) iter.evaluateur()).associer_inconnue(inco );        // Eval_VDF_Elem::associer_inconnue
  // GF sans dynamic_cast il FAUT rajouter une methode associer_inconnue
  // la ligne du dessu fait core_dump (j'ai eu le meme pb avec ocnd_lim_rayo)
  //(dynamic_cast<Eval_VDF_Elem&> (iter_vdf)).associer_inconnue(inco );
  get_eval_elem().associer_inconnue(inco );
}


// Description:
// associe le champ de diffusivite a l'evaluateur
void Op_Diff_VDF_Elem_base::associer_diffusivite(const Champ_base& ch_diff)
{
  Eval_Diff_VDF& eval_diff = (Eval_Diff_VDF&) iter.evaluateur();
  ////Eval_Diff_VDF& eval_diff = (Eval_Diff_VDF&) dynamic_cast<Eval_Diff_VDF&>(iter.evaluateur());

  eval_diff.associer(ch_diff);                // Eval_Diff_VDF::associer
}

const Champ_base& Op_Diff_VDF_Elem_base::diffusivite() const
{
  const Eval_Diff_VDF& eval_diff = (const Eval_Diff_VDF&) iter.evaluateur();
  ////const Eval_Diff_VDF& eval_diff = (const Eval_Diff_VDF&) dynamic_cast<const Eval_Diff_VDF&>(iter.evaluateur());
  return eval_diff.get_diffusivite();
}

void Op_Diff_VDF_Elem_base::contribuer_termes_croises(const DoubleTab& inco, Matrice_Morse& matrice, const Probleme_base& autre_pb) const
{

  const Zone_VDF& zone = iter.zone();
  const Zone_Cl_VDF& zcl = iter.zone_Cl();

  // boucle sur les cl pour trouver un paroi_contact
  for (int i = 0; i < zone.nb_front_Cl(); i++)
    {
      const Cond_lim& la_cl = zcl.les_conditions_limites(i);
      if (la_cl.valeur().que_suis_je().debute_par("Paroi_Echange_contact"))
        {
          const Echange_contact_VDF& cl = ref_cast(Echange_contact_VDF, la_cl.valeur());
          if (cl.nom_autre_pb() == autre_pb.le_nom())
            {
              const IntTab& elem_dist = cl.get_size_bloc_and_distant_elems();
              std::map<int, std::pair<int, int>> f2e;
              // elem_dist(0, :) contient la dimension du bloc rectangulaire
              for (int j = 1; j < elem_dist.dimension(0); j++)
                f2e[elem_dist(j, 2)] = std::make_pair(elem_dist(j, 0), elem_dist(j, 1));
              iter.ajouter_contribution_autre_pb(inco, matrice, la_cl, f2e);
            }
        }
    }

}

void Op_Diff_VDF_Elem_base::dimensionner_termes_croises(Matrice_Morse& matrice, const Probleme_base& autre_pb) const
{
  const Zone_VDF& zone = iter.zone();
  const Zone_Cl_VDF& zcl = iter.zone_Cl();
  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  int nl = -1, nc = -1;

  // boucle sur les cl pour trouver un paroi_contact
  for (int i = 0; i < zone.nb_front_Cl(); i++)
    {
      const Cond_lim& la_cl = zcl.les_conditions_limites(i);
      if (la_cl.valeur().que_suis_je().debute_par("Paroi_Echange_contact"))
        {
          const Echange_contact_VDF& cl = ref_cast(Echange_contact_VDF, la_cl.valeur());
          if (cl.nom_autre_pb() == autre_pb.le_nom())
            {
              const IntTab& elem_dist = cl.get_size_bloc_and_distant_elems();
              nl = elem_dist(0, 0), nc = elem_dist(0, 1);
              for (int j = 1; j < elem_dist.dimension(0); j++)
                stencil.append_line(elem_dist(j, 0), elem_dist(j, 1));
            }
        }
    }

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(nl, nc, stencil, matrice);
}
