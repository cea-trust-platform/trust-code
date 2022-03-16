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
// File:        Op_Diff_VDF_Elem_base.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Op_Diff_Dift/Op_Diff_Dift_base
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_VDF_Elem_base.h>
#include <Echange_contact_VDF.h>
#include <Champ_P0_VDF.h>
#include <Matrix_tools.h>
#include <Array_tools.h>

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
  const Zone_VDF& zone_VDF = iter.zone();
  const DoubleTab& diffu = has_champ_masse_volumique() ? diffusivite().valeurs() : diffusivite_pour_pas_de_temps().valeurs();


  if (sub_type(Champ_Uniforme,diffusivite_pour_pas_de_temps()) && !has_champ_masse_volumique())
    {
      // GF le max permet de traiter le multi_inco
      double alpha=max_array(diffu);

      double coef = 1/(zone_VDF.h_x()*zone_VDF.h_x()) + 1/(zone_VDF.h_y()*zone_VDF.h_y());

      if (dimension == 3) coef += 1/(zone_VDF.h_z()*zone_VDF.h_z());

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
              double l = zone_VDF.dim_elem(elem,i);
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
  const Zone_VDF& zone = iter.zone();
  const IntTab& f_e = zone.face_voisins();
  const Zone_Cl_VDF& zcl = iter.zone_Cl();
  int l;

  // boucle sur les cl pour trouver un paroi_contact
  for (int i = 0; i < zone.nb_front_Cl(); i++)
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
      iter.ajouter_contribution_autre_pb(inco, matrice, la_cl, f2e);
    }
}

void Op_Diff_VDF_Elem_base::dimensionner_termes_croises(Matrice_Morse& matrice, const Probleme_base& autre_pb, int nl, int nc) const
{
  const Champ_P0_VDF& ch = ref_cast(Champ_P0_VDF, equation().inconnue().valeur());
  const Zone_VDF& zone = iter.zone();
  const IntTab& f_e = zone.face_voisins();
  const Conds_lim& cls = iter.zone_Cl().les_conditions_limites();
  int i, j, l, f, n, N = ch.valeurs().line_size();

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  for (i = 0; i < cls.size(); i++) if (sub_type(Echange_contact_VDF, cls[i].valeur()))
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
  if (!op_ext_init_) init_op_ext();
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  int n_ext = op_ext.size(); //pour la thermique monolithique

  std::vector<Matrice_Morse *> mat(n_ext);
  std::vector<int> N(n_ext); //nombre de composantes par probleme de op_ext
  for (int i = 0; i < n_ext; i++)
    {
      N[i] = op_ext[i]->equation().inconnue().valeurs().line_size();

      std::string nom_mat = i ? nom_inco + "_" + op_ext[i]->equation().probleme().le_nom().getString() : nom_inco;
      mat[i] = matrices.count(nom_mat) ? matrices.at(nom_mat) : NULL;
      if(!mat[i]) continue;
      Matrice_Morse mat2;
      if(i==0) Op_VDF_Elem::dimensionner(iter.zone(), iter.zone_Cl(), mat2);
      else
        {
          int nl = N[0] * iter.zone().nb_elem_tot();
          int nc = N[i] * op_ext[i]->equation().zone_dis()->nb_elem_tot();
          dimensionner_termes_croises(mat2, op_ext[i]->equation().probleme(),nl, nc);
        }
      mat[i]->nb_colonnes() ? *mat[i] += mat2 : *mat[i] = mat2;
    }
}

void Op_Diff_VDF_Elem_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  if (!op_ext_init_) init_op_ext();

  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  int n_ext = op_ext.size(); //pour la thermique monolithique
  std::vector<Matrice_Morse *> mat(n_ext);
  std::vector<const DoubleTab *> inco(n_ext); //inconnues

  mat[0] = matrices.count(nom_inco) ? matrices.at(nom_inco) : NULL;
  inco[0] = semi_impl.count(nom_inco) ? &semi_impl.at(nom_inco) : &op_ext[0]->equation().inconnue().valeur().valeurs();

  if(mat[0]) iter.ajouter_contribution(*inco[0], *mat[0]);
  iter.ajouter(*inco[0],secmem);

  if (equation().domaine_application() == Motcle("Hydraulique"))
    // On est dans le cas des equations de Navier_Stokes
    {
      // Ajout du terme supplementaire en V/(R*R) dans le cas des coordonnees axisymetriques
      if(Objet_U::bidim_axi == 1)
        {
          const Zone_VDF& zvdf=iter.zone();
          const DoubleTab& xv=zvdf.xv();
          const IntVect& ori=zvdf.orientation();
          const IntTab& face_voisins=zvdf.face_voisins();
          const DoubleVect& volumes_entrelaces=zvdf.volumes_entrelaces();
          int face, nb_faces=zvdf.nb_faces();//, cst;
          DoubleTrav diffu_tot(zvdf.nb_elem_tot());
          double db_diffusivite;
          Nom nom_eq=equation().que_suis_je();
          if ((nom_eq == "Navier_Stokes_standard")||(nom_eq == "Navier_Stokes_QC")||(nom_eq == "Navier_Stokes_FT_Disc"))
            {
              const Eval_Diff_VDF& eval=dynamic_cast<const Eval_Diff_VDF&> (iter.evaluateur());
              const Champ_base& ch_diff=eval.get_diffusivite();
              const DoubleTab& tab_diffusivite=ch_diff.valeurs();
              if (tab_diffusivite.size() == 1)
                diffu_tot = tab_diffusivite(0,0);
              else
                diffu_tot = tab_diffusivite;

              for(face=0; face<nb_faces; face++)
                if(ori(face)==0)
                  {
                    int elem1=face_voisins(face,0);
                    int elem2=face_voisins(face,1);
                    if(elem1==-1)
                      db_diffusivite=diffu_tot(elem2);
                    else if (elem2==-1)
                      db_diffusivite=diffu_tot(elem1);
                    else
                      db_diffusivite=0.5*(diffu_tot(elem2)+diffu_tot(elem1));
                    double r= xv(face,0);
                    if(r>=1.e-24)
                      {
                        if(mat[0]) (*mat[0])(face,face)+=db_diffusivite*volumes_entrelaces(face)/(r*r);
                        secmem(face) -=(*inco[0])(face)*db_diffusivite*volumes_entrelaces(face)/(r*r);

                      }
                  }
            }
          else if (equation().que_suis_je() == "Navier_Stokes_Interface_avec_trans_masse" ||
                   equation().que_suis_je() == "Navier_Stokes_Interface_sans_trans_masse" ||
                   equation().que_suis_je() == "Navier_Stokes_Front_Tracking" ||
                   equation().que_suis_je() == "Navier_Stokes_Front_Tracking_BMOL")
            {
              // Voir le terme source axi dans Interfaces/VDF
            }
          else
            {
              Cerr << "Probleme dans Op_Diff_VDF_base::contribuer_a_avec  avec le type de l'equation" << finl;
              Cerr << "on n'a pas prevu d'autre cas que Navier_Stokes_std" << finl;
              exit();
            }
        }
    }

  for (int i = 1; i < n_ext; i++)
    {
      std::string nom_mat = nom_inco + "_" + op_ext[i]->equation().probleme().le_nom().getString();
      mat[i] = matrices.count(nom_mat) ? matrices.at(nom_mat) : NULL;
      if(mat[i])
        {
          inco[i] = semi_impl.count(nom_mat) ? &semi_impl.at(nom_mat) : &op_ext[i]->equation().inconnue().valeur().valeurs();
          contribuer_termes_croises(*inco[i], op_ext[i]->equation().probleme(), op_ext[i]->equation().inconnue().valeurs(), *mat[i]);
        }
    }
}

