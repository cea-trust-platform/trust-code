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

#include <Op_Diff_VDF_Elem_base.h>
#include <Echange_contact_VDF.h>
#include <Champ_P0_VDF.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Statistiques.h>

extern Stat_Counter_Id diffusion_counter_;

Implemente_base_sans_constructeur(Op_Diff_VDF_Elem_base,"Op_Diff_VDF_Elem_base",Op_Diff_VDF_base);

Sortie& Op_Diff_VDF_Elem_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Diff_VDF_Elem_base::readOn(Entree& s ) { return s ; }

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
  const Domaine_VDF& domaine_VDF = iter->domaine();
  const DoubleTab& diffu = has_champ_masse_volumique() ? diffusivite().valeurs() : diffusivite_pour_pas_de_temps().valeurs();


  if (sub_type(Champ_Uniforme,diffusivite_pour_pas_de_temps()) && !has_champ_masse_volumique())
    {
      // GF le max permet de traiter le multi_inco
      double alpha=max_array(diffu);

      double coef = 1/(domaine_VDF.h_x()*domaine_VDF.h_x()) + 1/(domaine_VDF.h_y()*domaine_VDF.h_y());

      if (dimension == 3) coef += 1/(domaine_VDF.h_z()*domaine_VDF.h_z());

      if (alpha==0) dt_stab = DMAXFLOAT;
      else dt_stab = 0.5/(alpha*coef);
    }
  else
    {
      double alpha_loc;
      const int nb_elem = diffu.dimension(0);
      for (int elem = 0 ; elem < nb_elem; elem++)
        {
          double h = 0;
          for (int i=0 ; i<dimension; i++)
            {
              double l = domaine_VDF.dim_elem(elem,i);
              h += 1./(l*l);
            }
          alpha_loc = diffu(elem,0);
          for (int ncomp=1; ncomp<diffu.dimension(1); ncomp++) alpha_loc = std::max(alpha_loc,diffu(elem,ncomp));
          if (has_champ_masse_volumique())
            {
              const DoubleTab& rho = get_champ_masse_volumique().valeurs();
              alpha_loc/= rho(elem);
            }
          double dt_loc = 0.5/((alpha_loc+DMINFLOAT)*h);
          if (dt_loc<dt_stab) dt_stab = dt_loc;
        }
    }

  return Process::mp_min(dt_stab);
}

void Op_Diff_VDF_Elem_base::contribuer_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, Matrice_Morse& matrice) const
{
  const Domaine_VDF& domaine = iter->domaine();
  const IntTab& f_e = domaine.face_voisins();
  const Domaine_Cl_VDF& zcl = iter->domaine_Cl();
  int l;

  // boucle sur les cl pour trouver un paroi_contact
  for (int i = 0; i < domaine.nb_front_Cl(); i++)
    {
      const Cond_lim& la_cl = zcl.les_conditions_limites(i);
      if (!la_cl.valeur().que_suis_je().debute_par("Paroi_Echange_contact")) continue; //pas un Echange_contact
      const Echange_contact_VDF& cl = ref_cast(Echange_contact_VDF, la_cl.valeur());
      if (cl.nom_autre_pb() != autre_pb.le_nom()) continue; //not our problem

      std::map<int, std::pair<int, int>> f2e;
      const Front_VF& fvf = ref_cast(Front_VF, cl.frontiere_dis());
      for (int j = 0; j < cl.item.dimension(0); j++)
        if ((l = cl.item(j)) >= 0)
          {
            int f = fvf.num_face(j);
            int e = f_e(f, 0) == -1 ? f_e(f, 1) : f_e(f, 0);
            f2e[f] = std::make_pair(e, l);
          }
      iter->ajouter_contribution_autre_pb(inco, matrice, la_cl, f2e);
    }
}

void Op_Diff_VDF_Elem_base::dimensionner_termes_croises(Matrice_Morse& matrice, const Probleme_base& autre_pb, int nl, int nc) const
{
  const Champ_P0_VDF& ch = ref_cast(Champ_P0_VDF, equation().inconnue().valeur());
  const Domaine_VDF& domaine = iter->domaine();
  const IntTab& f_e = domaine.face_voisins();
  const Conds_lim& cls = iter->domaine_Cl().les_conditions_limites();
  int i, j, l, f, n, N = ch.valeurs().line_size();

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  for (i = 0; i < cls.size(); i++)
    if (sub_type(Echange_contact_VDF, cls[i].valeur()))
      {
        const Echange_contact_VDF& cl = ref_cast(Echange_contact_VDF, cls[i].valeur());
        if (cl.nom_autre_pb() != autre_pb.le_nom()) continue; //not our problem

        /* stencil */
        const Front_VF& fvf = ref_cast(Front_VF, cl.frontiere_dis());
        for (j = 0; j < cl.item.dimension(0); j++)
          if ((l = cl.item(j)) >= 0)
            {
              f = fvf.num_face(j);
              int e = f_e(f, 0) == -1 ? f_e(f, 1) : f_e(f, 0);
              for (n = 0; n < N; n++) stencil.append_line(N * e + n, N * l + n);
            }
      }

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(nl, nc, stencil, matrice);
}

void Op_Diff_VDF_Elem_base::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string nom_inco = equation().inconnue().le_nom().getString();
  if (semi_impl.count(nom_inco)) return; //semi-implicite -> rien a dimensionner

  if (!op_ext_init_) init_op_ext();
  int n_ext = (int)op_ext.size(); //pour la thermique monolithique

  std::vector<Matrice_Morse *> mat(n_ext);
  std::vector<int> N(n_ext); //nombre de composantes par probleme de op_ext
  for (int i = 0; i < n_ext; i++)
    {
      N[i] = op_ext[i]->equation().inconnue().valeurs().line_size();

      std::string nom_mat = i ? nom_inco + "/" + op_ext[i]->equation().probleme().le_nom().getString() : nom_inco;
      mat[i] = matrices.count(nom_mat) ? matrices.at(nom_mat) : NULL;
      if(!mat[i]) continue;
      Matrice_Morse mat2;
      if(i==0) Op_VDF_Elem::dimensionner(iter->domaine(), iter->domaine_Cl(), mat2);
      else
        {
          int nl = N[0] * iter->domaine().nb_elem_tot();
          int nc = N[i] * op_ext[i]->equation().domaine_dis()->nb_elem_tot();
          dimensionner_termes_croises(mat2, op_ext[i]->equation().probleme(),nl, nc);
        }
      mat[i]->nb_colonnes() ? *mat[i] += mat2 : *mat[i] = mat2;
    }
}

void Op_Diff_VDF_Elem_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(diffusion_counter_);
  if (!op_ext_init_) init_op_ext();

  // On commence par l'operateur locale; i.e. *this !
  iter->ajouter_blocs(matrices, secmem, semi_impl);

  // On ajoute des termes si axi ...
  Op_Diff_VDF_base::ajoute_terme_pour_axi(matrices, secmem, semi_impl);

  // On ajoute contribution si monolithique
  if ((int) op_ext.size() > 1) ajouter_blocs_pour_monolithique(matrices, secmem, semi_impl);

  statistiques().end_count(diffusion_counter_);
}

void Op_Diff_VDF_Elem_base::ajouter_blocs_pour_monolithique(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  int n_ext = (int)op_ext.size() - 1; // pour la thermique monolithique, -1 car 1er (i.e. *this) est deja fait via ajouter_blocs
  std::vector<Matrice_Morse *> mat(n_ext);
  std::vector<const DoubleTab *> inco(n_ext); //inconnues

  for (int i = 0; i < n_ext; i++)
    {
      std::string nom_mat = nom_inco + "/" + op_ext[i + 1]->equation().probleme().le_nom().getString();
      mat[i] = matrices.count(nom_mat) ? matrices.at(nom_mat) : NULL;
      if(mat[i])
        {
          inco[i] = semi_impl.count(nom_mat) ? &semi_impl.at(nom_mat) : &op_ext[i + 1]->equation().inconnue()->valeurs();
          contribuer_termes_croises(*inco[i], op_ext[i + 1]->equation().probleme(), op_ext[i + 1]->equation().inconnue()->valeurs(), *mat[i]);
        }
    }
}
