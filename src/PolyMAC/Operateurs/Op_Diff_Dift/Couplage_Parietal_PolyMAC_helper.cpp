/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Couplage_Parietal_PolyMAC_helper.h>
#include <Convection_Diffusion_Temperature.h>
#include <Flux_interfacial_PolyMAC_P0P1NC.h>
#include <Echange_contact_PolyMAC_P0.h>
#include <Op_Diff_PolyMAC_P0_Elem.h>
#include <Echange_externe_impose.h>
#include <Champ_Elem_PolyMAC_P0.h>
#include <Flux_parietal_base.h>
#include <Domaine_PolyMAC_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Milieu_composite.h>
#include <Option_PolyMAC.h>
#include <Neumann_paroi.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Statistiques.h>
#include <Array_tools.h>
#include <functional>
#include <cmath>

void Couplage_Parietal_PolyMAC_helper::associer(const Op_Diff_PolyMAC_P0_Elem& op)
{
  op_elem_ = op;
}

void Couplage_Parietal_PolyMAC_helper::completer_d_nuc()
{
  const Champ_Elem_PolyMAC_P0& ch = ref_cast(Champ_Elem_PolyMAC_P0, op_elem_->equation().inconnue());
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, op_elem_->domaine_poly());
  d_nuc_.resize(0, ch.valeurs().line_size());
  domaine.domaine().creer_tableau_elements(d_nuc_);
}

/* construction de s_dist : sommets du porbleme coincidant avec des sommets de problemes distants */
void Couplage_Parietal_PolyMAC_helper::init_s_dist() const
{
  if (s_dist_init_)
    return; //deja fait

  if (op_elem_->has_echange_contact())
    {
      const Conds_lim& cls = op_elem_->domaine_cl_poly().les_conditions_limites();
      for (const auto &itr : cls)
        if (sub_type(Echange_contact_PolyMAC_P0, itr.valeur()))
          {
            const Echange_contact_PolyMAC_P0& cl = ref_cast(Echange_contact_PolyMAC_P0, itr.valeur());
            cl.init_op();

            const Op_Diff_PolyMAC_P0_Elem *o_diff = &cl.o_diff.valeur();
            cl.init_fs_dist();

            for (auto &s_sb : cl.s_dist)
              s_dist[s_sb.first][o_diff] = s_sb.second;
          }
    }

  s_dist_init_ = 1;
}

void Couplage_Parietal_PolyMAC_helper::init_op_ext() const
{
  if (som_ext_init_)
    return; //deja fait

  /* remplissage de op_ext : de proche en proche */
  op_elem_->op_ext = { &(op_elem_.valeur()) };
  init_s_dist();

  /* construction de som_ext_{d, e, f} */
  som_ext_d.resize(0, 2);
  som_ext_d.append_line(0, 0);
  som_ext_pe.resize(0, 2);
  som_ext_pf.resize(0, 4);

  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, op_elem_->domaine_poly());
  auto s_dist_full = s_dist;

  for (std::set<const Op_Diff_PolyMAC_P0_Elem*> op_ext_tbd = { &(op_elem_.valeur()) }; op_ext_tbd.size();)
    {
      const Op_Diff_PolyMAC_P0_Elem *op = *op_ext_tbd.begin();
      op_ext_tbd.erase(op_ext_tbd.begin());

      //elargissement de op_ext
      if (op_elem_->has_echange_contact())
        {
          const Conds_lim& cls = op->equation().domaine_Cl_dis().les_conditions_limites();
          for (const auto &itr : cls)
            if (sub_type(Echange_contact_PolyMAC_P0, itr.valeur()))
              {
                const Echange_contact_PolyMAC_P0& cl = ref_cast(Echange_contact_PolyMAC_P0, itr.valeur());

                cl.init_op();

                const Op_Diff_PolyMAC_P0_Elem *o_diff = &cl.o_diff.valeur();
                if (std::find(op_elem_->op_ext.begin(), op_elem_->op_ext.end(), o_diff) == op_elem_->op_ext.end())
                  {
                    op_elem_->op_ext.push_back(o_diff);
                    op_ext_tbd.insert(o_diff);
                  }
              }
        }

      //elargissement de s_dist_full : aargh...
      op->couplage_parietal_helper().init_s_dist();
      if (op != &(op_elem_.valeur()))
        for (auto &&s_op_sb : s_dist_full)
          if (s_op_sb.second.count(op))
            for (auto &&op_sc : op->couplage_parietal_helper().s_dist[s_op_sb.second[op]])
              if (!s_op_sb.second.count(op_sc.first))
                s_op_sb.second[op_sc.first] = op_sc.second;
    }


  std::set<std::array<int, 2>> s_pe; // (pb, el)
  std::set<std::array<int, 4>> s_pf; // (pb1, f1, pb2, f2)

  /*
   * XXX NO WORRIES : c'est std::vector<ref_array>
   */
  std::vector<std::reference_wrapper<const Domaine_PolyMAC_P0>> domaines;
  std::vector<std::reference_wrapper<const IntTab>> fcl, e_f, f_s;
  std::vector<std::reference_wrapper<const Static_Int_Lists>> som_elem;

  for (auto &&op : op_elem_->op_ext)
    domaines.push_back(std::ref(ref_cast(Domaine_PolyMAC_P0, op->equation().domaine_dis())));

  for (auto &&op : op_elem_->op_ext)
    fcl.push_back(std::ref(ref_cast(Champ_Elem_PolyMAC_P0, op->equation().inconnue()).fcl()));

  for (auto &&zo : domaines)
    {
      e_f.push_back(std::ref(zo.get().elem_faces()));
      f_s.push_back(std::ref(zo.get().face_sommets()));
      som_elem.push_back(std::ref(zo.get().som_elem()));
    }

  /* autres CLs (hors Echange_contact) devant etre traitees par som_ext : Echange_impose_base, tout si Pb_Multiphase avec Flux_parietal_base */
  const Conds_lim& cls = op_elem_->equation().domaine_Cl_dis().les_conditions_limites();
  int has_flux = (sub_type(Energie_Multiphase, op_elem_->equation()) || sub_type(Convection_Diffusion_Temperature, op_elem_->equation())) &&
                 op_elem_->equation().probleme().has_correlation("flux_parietal");

  for (int i = 0; i < cls.size(); i++)
    if (has_flux || sub_type(Echange_contact_PolyMAC_P0, cls[i].valeur()))
      for (int j = 0; j < ref_cast(Front_VF, cls[i]->frontiere_dis()).nb_faces(); j++)
        {
          const int f = ref_cast(Front_VF, cls[i]->frontiere_dis()).num_face(j);
          for (int k = 0; k < f_s[0].get().dimension(1); k++)
            {
              const int s = f_s[0](f, k);
              if (s < 0) continue;

              if (!s_dist_full.count(s))
                s_dist_full[s] = { }; //dans le std::map, mais pas d'operateurs distants!
            }
        }

  for (const auto &s_op_sb : s_dist_full)
    {
      const int s = s_op_sb.first;

      if (s < domaine.nb_som())
        {
          //elements
          for (int i = 0; i < som_elem[0].get().get_list_size(s); i++)
            {
              std::array<int, 2> arr = { 0, som_elem[0](s, i) };
              s_pe.insert( { arr }); //cote local
            }

          for (const auto &op_sb : s_op_sb.second) //cotes distants
            {
              const int iop = (int) (std::find(op_elem_->op_ext.begin(), op_elem_->op_ext.end(), op_sb.first) - op_elem_->op_ext.begin());

              for (int i = 0; i < som_elem[iop].get().get_list_size(op_sb.second); i++)
                {
                  std::array<int, 2> arr = { iop, som_elem[iop](op_sb.second, i) };
                  s_pe.insert( { arr });
                }
            }

          //faces : celles des elements de s_pe
          for (const auto &iop_e : s_pe)
            {
              const int iop = iop_e[0];
              const int e = iop_e[1];
              const int s_l = iop ? s_op_sb.second.at(op_elem_->op_ext[iop]) : s;

              for (int i = 0; i < e_f[iop].get().dimension(1); i++)
                {
                  const int f = e_f[iop](e, i);
                  if (f < 0) continue;

                  int ok = 0;

                  for (int j = 0; !ok && j < f_s[iop].get().dimension(1); j++)
                    {
                      const int sb = f_s[iop](f, j);
                      if (sb < 0) continue;

                      ok |= sb == s_l;
                    }

                  if (!ok || fcl[iop](f, 0) != 3)
                    continue; //face ne touchant pas le sommet ou non Echange_contact

                  const Echange_contact_PolyMAC_P0& cl = ref_cast(Echange_contact_PolyMAC_P0, op_elem_->op_ext[iop]->equation().domaine_Cl_dis().les_conditions_limites()[fcl[iop](f, 1)].valeur());

                  //operateur / face de l'autre cote
                  int o_iop = (int) (std::find(op_elem_->op_ext.begin(), op_elem_->op_ext.end(), &cl.o_diff.valeur()) - op_elem_->op_ext.begin());
                  int o_f = cl.f_dist(fcl[iop](f, 2));

                  std::array<int, 4> arr = { iop < o_iop ? iop : o_iop, iop < o_iop ? f : o_f, iop < o_iop ? o_iop : iop, iop < o_iop ? o_f : f };
                  s_pf.insert( { arr }); //stocke dans l'ordre
                }
            }

          int mix = 0; //melange-t-on les composantes autour de ce sommet? oui si une equation Energie_Multiphase avec correlation de flux parietal est presente autour
          for (const auto &pe : s_pe)
            {
              som_ext_pe.append_line(pe[0], pe[1]);
              mix |= (   sub_type(Energie_Multiphase, op_elem_->op_ext[pe[0]]->equation()) ||
                         sub_type(Convection_Diffusion_Temperature, op_elem_->op_ext[pe[0]]->equation()) )
                     && op_elem_->op_ext[pe[0]]->equation().probleme().has_correlation("flux_parietal");
            }

          for (auto &&pf : s_pf)
            som_ext_pf.append_line(pf[0], pf[1], pf[2], pf[3]);

          ref_cast_non_const(IntTab, op_elem_->tab_som_ext()).append_line(s);
          som_mix.append_line(mix);
          som_ext_d.append_line(som_ext_pe.dimension(0), som_ext_pf.dimension(0));

          s_pe.clear();
          s_pf.clear();
        }
    }

  som_ext_init_ = 1;
}

void Couplage_Parietal_PolyMAC_helper::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string nom_inco = op_elem_->equation().inconnue().le_nom().getString();
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, op_elem_->domaine_poly());
  const IntTab& f_e = domaine.face_voisins();

  std::vector<Matrice_Morse*> mat(op_elem_->op_ext.size());

  //une matrice potentielle a remplir par operateur de op_ext
  for (int i = 0; i < (int) op_elem_->op_ext.size(); i++)
    {
      const auto& nom_inco_mat = i ? nom_inco + "/" + op_elem_->op_ext[i]->equation().probleme().le_nom().getString() : nom_inco;

      mat[i] = matrices.count(nom_inco_mat) ? matrices.at(nom_inco_mat) : nullptr;
    }

  std::vector<int> N(op_elem_->op_ext.size()); //nombre de composantes par probleme de op_ext

  std::vector<IntTab> stencil(op_elem_->op_ext.size()); //stencils par matrice

  for (int i = 0; i < (int) op_elem_->op_ext.size(); i++)
    {
      stencil[i].resize(0, 2);
      N[i] = op_elem_->op_ext[i]->equation().inconnue().valeurs().line_size();
    }

  IntTrav tpfa(0, N[0]); //pour suivre quels flux sont a deux points

  domaine.creer_tableau_faces(tpfa);
  tpfa = 1;

  Cerr << "Op_Diff_PolyMAC_P0_Elem::dimensionner() : ";

  //avec fgrad : parties hors Echange_contact (ne melange ni les problemes, ni les composantes)
  for (int f = 0; f < domaine.nb_faces(); f++)
    for (int i = 0; i < 2; i++)
      {
        const int e = f_e(f, i);
        if (e < 0) continue;

        if (e < domaine.nb_elem()) //stencil a l'element e
          for (int j = op_elem_->tab_phif_d()(f); j < op_elem_->tab_phif_d()(f + 1); j++)
            {
              const int e_s = op_elem_->tab_phif_e()(j);

              if (e_s < domaine.nb_elem_tot())
                for (int n = 0; n < N[0]; n++)
                  {
                    stencil[0].append_line(N[0] * e + n, N[0] * e_s + n);

                    const int tmp = (e_s == f_e(f, 0)) || (e_s == f_e(f, 1)) || (op_elem_->tab_phif_c()(j, n) == 0);
                    tpfa(f, n) &= tmp;
                  }
            }
      }

  //avec som_ext : partie Echange_contact -> melange toutes les composantes si som_mix = 1
  for (int i = 0; i < op_elem_->tab_som_ext().dimension(0); i++)
    for (int j = som_ext_d(i, 0); j < som_ext_d(i + 1, 0); j++)
      {
        const int e = som_ext_pe(j, 1);

        if (!som_ext_pe(j, 0) && e < domaine.nb_elem())
          for (int k = som_ext_d(i, 0); k < som_ext_d(i + 1, 0); k++)
            {
              const int p_s = som_ext_pe(k, 0);
              const int e_s = som_ext_pe(k, 1);

              for (int n = 0; n < N[0]; n++)
                for (int m = (som_mix(i) ? 0 : n); m < (som_mix(i) ? N[p_s] : n + 1); m++)
                  stencil[p_s].append_line(N[0] * e + n, N[p_s] * e_s + m);
            }
      }

  for (int i = 0; i < (int) op_elem_->op_ext.size(); i++)
    if (mat[i])
      {
        tableau_trier_retirer_doublons(stencil[i]);
        Matrice_Morse mat2;
        Matrix_tools::allocate_morse_matrix(N[0] * domaine.nb_elem_tot(), N[i] * op_elem_->op_ext[i]->equation().domaine_dis().nb_elem_tot(), stencil[i], mat2);

        if (mat[i]->nb_colonnes())
          *mat[i] += mat2;
        else
          *mat[i] = mat2;
      }

  int n_sten = 0;
  for (const auto &st : stencil)
    n_sten += st.dimension(0); //n_sten : nombre total de points du stencil de l'operateur

  const double elem_t = static_cast<double>(domaine.domaine().md_vector_elements()->nb_items_seq_tot()),
               face_t = static_cast<double>(domaine.md_vector_faces()->nb_items_seq_tot());
  Cerr << "width " << Process::mp_sum_as_double(n_sten) / (N[0] * elem_t) << " "
       << mp_somme_vect_as_double(tpfa) * 100. / (N[0] * face_t) << "% TPFA " << finl;
}

void Couplage_Parietal_PolyMAC_helper::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const std::string& nom_inco = op_elem_->equation().inconnue().le_nom().getString();

  const int n_ext = (int) op_elem_->op_ext.size(), D = Objet_U::dimension, un = 1;

  std::vector<Matrice_Morse*> mat(n_ext); //matrices

  std::vector<int> N; //composantes

  /*
   * XXX NO WORRIES : c'est std::vector<ref_array>
   */
  std::vector<std::reference_wrapper<const Domaine_PolyMAC_P0>> domaine; //domaines
  std::vector<std::reference_wrapper<const Conds_lim>> cls; //conditions aux limites
  std::vector<std::reference_wrapper<const IntTab>> fcl, e_f, f_e, f_s; //tableaux "fcl", "elem_faces", "faces_voisins"
  std::vector<std::reference_wrapper<const DoubleVect>> fs; //surfaces
  std::vector<std::reference_wrapper<const DoubleTab>> inco, nf, xp, xs, xv, diffu; //inconnues, normales aux faces, positions elems / faces / sommets

  int M = 0;
  for (int i = 0; i < n_ext; M = std::max(M, N[i]), i++)
    {
      std::string nom_mat = i ? nom_inco + "/" + op_elem_->op_ext[i]->equation().probleme().le_nom().getString() : nom_inco;

      mat[i] = !semi_impl.count(nom_inco) && matrices.count(nom_mat) ? matrices.at(nom_mat) : nullptr;

      domaine.push_back(std::ref(ref_cast(Domaine_PolyMAC_P0, op_elem_->op_ext[i]->equation().domaine_dis())));

      f_e.push_back(std::ref(domaine[i].get().face_voisins()));
      e_f.push_back(std::ref(domaine[i].get().elem_faces()));
      f_s.push_back(std::ref(domaine[i].get().face_sommets()));

      fs.push_back(std::ref(domaine[i].get().face_surfaces()));
      nf.push_back(std::ref(domaine[i].get().face_normales()));

      xp.push_back(std::ref(domaine[i].get().xp()));
      xv.push_back(std::ref(domaine[i].get().xv()));
      xs.push_back(std::ref(domaine[i].get().domaine().coord_sommets()));

      cls.push_back(std::ref(op_elem_->op_ext[i]->equation().domaine_Cl_dis().les_conditions_limites()));

      diffu.push_back(ref_cast(Op_Diff_PolyMAC_P0_Elem, *(op_elem_->op_ext[i])).nu());

      const Champ_Inc_base& ch_inc = op_elem_->op_ext[i]->has_champ_inco() ? op_elem_->op_ext[i]->mon_inconnue() : op_elem_->op_ext[i]->equation().inconnue();
      const Champ_Elem_PolyMAC_P0& ch = ref_cast(Champ_Elem_PolyMAC_P0, ch_inc);

      inco.push_back(std::ref(semi_impl.count(nom_mat) ? semi_impl.at(nom_mat) : ch.valeurs()));

      N.push_back(inco[i].get().line_size());
      fcl.push_back(std::ref(ch.fcl()));
    }

  const Domaine_PolyMAC_P0& domaine0 = domaine[0];

  /* avec phif : flux hors Echange_contact -> mat[0] seulement */
  DoubleTrav flux(N[0]);

  for (int f = 0; f < domaine0.nb_faces(); f++)
    {
      flux = 0.;

      for (int i = op_elem_->tab_phif_d()(f); i < op_elem_->tab_phif_d()(f + 1); i++)
        {
          const int eb = op_elem_->tab_phif_e()(i);
          const int fb = eb - domaine0.nb_elem_tot();

          if (fb < 0) //element
            {
              for (int n = 0; n < N[0]; n++)
                flux(n) += op_elem_->tab_phif_c()(i, n) * fs[0](f) * inco[0](eb, n);

              if (mat[0])
                for (int j = 0; j < 2; j++)
                  {
                    const int e = f_e[0](f, j);
                    if (e < 0) continue;

                    if (e < domaine[0].get().nb_elem())
                      for (int n = 0; n < N[0]; n++) //derivees
                        (*mat[0])(N[0] * e + n, N[0] * eb + n) += (j ? 1 : -1) * op_elem_->tab_phif_c()(i, n) * fs[0](f);
                  }

            }
          else if (fcl[0](fb, 0) == 1 || fcl[0](fb, 0) == 2)
            {
              for (int n = 0; n < N[0]; n++) //Echange_impose_base
                flux(n) += (op_elem_->tab_phif_c()(i, n) ? op_elem_->tab_phif_c()(i, n) * fs[0](f) * ref_cast(Echange_impose_base, cls[0].get()[fcl[0](fb, 1)].valeur()).T_ext(fcl[0](fb, 2), n) : 0);
            }
          else if (fcl[0](fb, 0) == 4)
            {
              for (int n = 0; n < N[0]; n++) //Neumann non homogene
                flux(n) += (op_elem_->tab_phif_c()(i, n) ? op_elem_->tab_phif_c()(i, n) * fs[0](f) * ref_cast(Neumann_paroi, cls[0].get()[fcl[0](fb, 1)].valeur()).flux_impose(fcl[0](fb, 2), n) : 0);
            }
          else if (fcl[0](fb, 0) == 6)
            {
              for (int n = 0; n < N[0]; n++) //Dirichlet
                flux(n) += (op_elem_->tab_phif_c()(i, n) ? op_elem_->tab_phif_c()(i, n) * fs[0](f) * ref_cast(Dirichlet, cls[0].get()[fcl[0](fb, 1)].valeur()).val_imp(fcl[0](fb, 2), n) : 0);
            }
        }

      for (int j = 0; j < 2; j++)
        {
          const int e = f_e[0](f, j);
          if (e < 0) continue;

          if (e < domaine[0].get().nb_elem())
            for (int n = 0; n < N[0]; n++) //second membre -> amont/aval
              secmem(e, n) += (j ? -1 : 1) * flux(n);
        }

      if (f < domaine0.premiere_face_int())
        for (int n = 0; n < N[0]; n++)
          op_elem_->flux_bords()(f, n) = flux(n); //flux aux bords
    }

  d_nuc_ = 0.; //remise a zero du diametre de nucleation

  // remplir les tabs ... mais seulement si besoin !
  DoubleTab *pqpi = op_elem_->equation().sources().size() &&
                    sub_type(Flux_interfacial_PolyMAC_P0P1NC, op_elem_->equation().sources().dernier().valeur()) ?
                    &ref_cast(Flux_interfacial_PolyMAC_P0P1NC, op_elem_->equation().sources().dernier().valeur()).qpi() : nullptr;


  /* avec som_ext : flux autour des sommets affectes par des Echange_contact */
  double i3[3][3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } }, eps = 1e-8, eps_g = 1e-6, fac[3];

  std::vector<std::array<int, 2>> s_pe, s_pf; //listes { probleme, elements/bord}, { probleme, face } autour du sommet
  std::map<std::array<int, 2>, std::array<int, 2>> m_pf; //connections Echange_contact : m_pf[{pb1, f1}] = { pb2, f2 }
  std::vector<double> surf_fs, vol_es; //surfaces partielles des faces connectees au sommet (meme ordre que s_f)
  std::vector<std::array<std::array<double, 3>, 2>> vec_fs; //pour chaque facette, base de (D-1) vecteurs permettant de la parcourir
  std::vector<std::vector<int>> se_f; /* se_f[i][.] : faces connectees au i-eme element connecte au sommet s */
  std::vector<int> type_f; //type_f[i] : type de la face i (numerotation du tableau fcl)

  IntTrav i_efs, i_e, i_eq_flux, i_eq_cont, i_eq_pbm, piv(1); //indices dans les matrices : i_efs(i, j, n) -> composante n de la face j de l'elem i dans s_pe, i_e(i, n) -> indice de la phase n de l'elem i de s_pe

  //i_eq_{flux,cont}(i, n) -> n-ieme equation de flux/de continuite a la face i de s_pf

  //i_eq_pbm(i_efs(i, j, n)) -> n-ieme equation "flux = correlation" a la face j de l'elem i de s_pe (seulement si Pb_Multiphase)

  DoubleTrav A, B, Ff, Fec, Qf, Qec, Tefs, C, X, Y, W(1), S, x_fs;

  // Et pour les methodes span de la classe Saturation pour le flux parietal
  std::vector<DoubleTrav> Ts_tab(n_ext), Sigma_tab(n_ext), Lvap_tab(n_ext);

  for (int i = 0; i < n_ext; i++)
    if (sub_type(Milieu_composite, op_elem_->op_ext[i]->equation().milieu()))
      {
        const Milieu_composite& milc = ref_cast(Milieu_composite, op_elem_->op_ext[i]->equation().milieu());

        const int nbelem_tot = domaine[i].get().nb_elem_tot(), nb_max_sat = N[i] * (N[i] - 1) / 2; // oui !! suite arithmetique !!

        if (op_elem_->op_ext[i]->equation().probleme().has_correlation("flux_parietal"))
          {
            Ts_tab[i].resize(nbelem_tot, nb_max_sat);
            Sigma_tab[i].resize(nbelem_tot, nb_max_sat);
            Lvap_tab[i].resize(nbelem_tot, nb_max_sat);

            const DoubleTab& press = ref_cast(QDM_Multiphase, ref_cast(Pb_Multiphase, op_elem_->op_ext[i]->equation().probleme()).equation_qdm()).pression().passe();

            for (int k = 0; k < N[i]; k++)
              for (int l = k + 1; l < N[i]; l++)
                if (milc.has_saturation(k, l))
                  {
                    Saturation_base& z_sat = milc.get_saturation(k, l);
                    const int ind_trav = (k * (N[i] - 1) - (k - 1) * (k) / 2) + (l - k - 1); // Et oui ! matrice triang sup !

                    // XXX XXX XXX
                    // Attention c'est dangereux ! on suppose pour le moment que le champ de pression a 1 comp. Par contre la taille de res est nb_max_sat*nbelem !!
                    // Aussi, on passe le Span le nbelem pour le champ de pression et pas nbelem_tot ....
                    assert(press.line_size() == 1);

                    // recuperer Tsat et sigma ...
                    const DoubleTab& sig = z_sat.get_sigma_tab(), &tsat = z_sat.get_Tsat_tab();

                    // fill in the good case
                    for (int ii = 0; ii < nbelem_tot; ii++)
                      {
                        Ts_tab[i](ii, ind_trav) = tsat(ii);
                        Sigma_tab[i](ii, ind_trav) = sig(ii);
                      }

                    z_sat.Lvap(press.get_span_tot() /* elem reel */, Lvap_tab[i].get_span_tot(), nb_max_sat, ind_trav);
                  }
          }
      }

  for (int i_s = 0; i_s < op_elem_->tab_som_ext().dimension(0); i_s++)
    {
      const int s = op_elem_->tab_som_ext()(i_s);

      if (s < domaine0.nb_som())
        {
          /* (pb, elem) connectes a s -> avec som_ext_pe (deja classes) */
          s_pe.clear();
          int n_e = 0;

          for (int i = som_ext_d(i_s, 0); i < som_ext_d(i_s + 1, 0); i++, n_e++)
            s_pe.push_back( { { som_ext_pe(i, 0), som_ext_pe(i, 1) } });

          /* m_pf : correspondances par parois contact */
          m_pf.clear();
          for (int i = som_ext_d(i_s, 1); i < som_ext_d(i_s + 1, 1); i++)
            for (int j = 0; j < 2; j++)
              m_pf[ { { som_ext_pf(i, j ? 2 : 0), som_ext_pf(i, j ? 3 : 1) } }] = { { som_ext_pf(i, j ? 0 : 2), som_ext_pf(i, j ? 1 : 3) } };

          /* faces, leurs surfaces partielles */
          s_pf.clear();
          surf_fs.clear();
          vec_fs.clear();
          se_f.resize(std::max(int(se_f.size()), n_e));

          for (int i = 0; i < n_e; i++)
            {
              se_f[i].clear();
              int p = s_pe[i][0];
              int e = s_pe[i][1];
              int sp = p ? s_dist[s].at(op_elem_->op_ext[p]) : s;

              for (int j = 0; j < e_f[p].get().dimension(1); j++)
                {
                  const int f = e_f[p](e, j);
                  if (f < 0) continue;

                  int sb = 0;
                  int k = 0;

                  for ( ; k < f_s[p].get().dimension(1); k++)
                    {
                      sb = f_s[p](f, k);
                      if (sb < 0) continue;

                      if (sb == sp)
                        break;
                    }

                  if (sb != sp)
                    continue; /* face de e non connectee a s -> on saute */

                  se_f[i].push_back(f); //faces connectees a (p, e)

                  //couple (p, f) de la face : si la face est un Echange_contact, alors on choisit le couple du pb d'indice le plus bas
                  std::array<int, 2> pf0 = { { p, f } };
                  std::array<int, 2> pf = m_pf.count(pf0) && m_pf[pf0] < pf0 ? m_pf[pf0] : pf0;

                  int l = (int) (std::lower_bound(s_pf.begin(), s_pf.end(), pf) - s_pf.begin());

                  if (l == (int) s_pf.size() || s_pf[l] != pf) /* si (p, f) n'est pas dans s_pf, on l'ajoute */
                    {
                      s_pf.insert(s_pf.begin() + l, pf); //(pb, face) -> dans s_pf
                      if (D < 3)
                        {
                          surf_fs.insert(surf_fs.begin() + l, fs[p](f) / 2);
                          vec_fs.insert(vec_fs.begin() + l, { { { xs[p](sp, 0) - xv[p](f, 0), xs[p](sp, 1) - xv[p](f, 1), 0 }, { 0, 0, 0 } } }); //2D -> facile
                        }
                      else
                        {
                          surf_fs.insert(surf_fs.begin() + l, 0);
                          vec_fs.insert(vec_fs.begin() + l, { { { 0, 0, 0 }, { 0, 0, 0 } } });

                          for (int m = 0; m < 2; m++) //3D -> deux sous-triangles
                            {
                              if (m == 1 || k > 0)
                                {
                                  sb = f_s[p](f, m ? (k + 1 < f_s[p].get().dimension(1) && f_s[p](f, k + 1) >= 0 ? k + 1 : 0) : k - 1); //sommet suivant (m = 1) ou precedent avec k > 0 -> facile
                                }
                              else
                                {
                                  for (int n = f_s[p].get().dimension(1) - 1; (sb = f_s[p](f, n)) == -1;)
                                    n--; //sommet precedent avec k = 0 -> on cherche a partir de la fin
                                }

                              auto v = domaine0.cross(D, D, &xs[p](sp, 0), &xs[p](sb, 0), &xv[p](f, 0), &xv[p](f, 0)); //produit vectoriel (xs - xf)x(xsb - xf)

                              surf_fs[l] += std::fabs(domaine0.dot(&v[0], &nf[p](f, 0))) / fs[p](f) / 4; //surface a ajouter

                              for (int d = 0; d < D; d++)
                                vec_fs[l][m][d] = (xs[p](sp, d) + xs[p](sb, d)) / 2 - xv[p](f, d); //vecteur face -> arete
                            }
                        }
                    }
                }
            }

          int n_f = (int) s_pf.size(); //nombre de faces

          /* conversion de se_f en indices dans s_f */
          for (int i = 0; i < n_e; i++)
            {
              int p = s_pe[i][0];
              for (int j = 0; j < (int) se_f[i].size(); j++)
                {
                  std::array<int, 2> pf0 = { p, se_f[i][j] };
                  std::array<int, 2> pf = m_pf.count(pf0) && m_pf[pf0] < pf0 ? m_pf[pf0] : pf0;

                  se_f[i][j] = (int) (std::lower_bound(s_pf.begin(), s_pf.end(), pf) - s_pf.begin());
                }
            }

          /* volumes */
          double vol_s = 0.;
          vol_es.resize(n_e);

          for (int i = 0; i < n_e; vol_s += vol_es[i], i++)
            {
              int p = s_pe[i][0];
              int e = s_pe[i][1];
              vol_es[i] = 0.;

              for (int j = 0; j < (int) se_f[i].size(); j++)
                {
                  int k = se_f[i][j];
                  int pb = s_pf[k][0];
                  int f = s_pf[k][1];
                  vol_es[i] += surf_fs[k] * std::fabs(domaine0.dot(&xp[p](e, 0), &nf[pb](f, 0), &xv[pb](f, 0))) / fs[pb](f) / D;
                }
            }

          /* inconnues en paroi (i_efs), aux elements (i_e). On alloues toutes les composantes si som_mix = 1, une seule sinon */
          int mix = som_mix(i_s), Nm = mix ? 1 : N[s_pe[0][0]]; //nombre total d'equations/variables aux faces, nombre total de variables aux elements, t_ec = t_e + 1, nombres divises par Nl

          int n_ef = 0;
          for (int i = 0; i < n_e; i++)
            n_ef = std::max(n_ef, int(se_f[i].size())); //nombre max de faces par elem

          i_efs.resize(n_e, n_ef, 1 + M * mix);
          i_efs = -1;
          int t_eq = 0;

          for (int i = 0; i < n_e; i++)
            {
              int p = s_pe[i][0];
              int e = s_pe[i][1];

              for (int j = 0; j < (int) se_f[i].size(); j++)
                {
                  int k = se_f[i][j];
                  for (int n = 0; n < (mix ? N[p] : 1); n++, t_eq++)
                    i_efs(i, j, n) = t_eq; //une temperature de paroi par phase

                  //si face de bord d'un Pb_Multiphase (hors frontiere ouverte), une inconnue supplementaire : la Tparoi (dans ce cas, mix = 1)
                  int f = s_pf[k][0] == p && (e == f_e[p](s_pf[k][1], 0) || e == f_e[p](s_pf[k][1], 1)) ? s_pf[k][1] : m_pf.at(s_pf[k])[1]; //numero de face cote e

                  if (( sub_type(Energie_Multiphase, op_elem_->op_ext[p]->equation())
                        || sub_type(Convection_Diffusion_Temperature, op_elem_->op_ext[p]->equation()) )
                      && op_elem_->op_ext[p]->equation().probleme().has_correlation("flux_parietal")
                      && fcl[p](f, 0) && fcl[p](f, 0) != 5)
                    {
                      i_efs(i, j, M) = t_eq++;
                    }
                }
            }

          i_e.resize(n_e, mix ? M : 1);
          i_e = -1;
          int t_e = 0;
          int t_ec = 1; // t_ec = t_e + 1, nombres divises par Nl

          for (int i = 0; i < n_e; i++)
            {
              int p = s_pe[i][0];

              for (int n = 0; n < (mix ? N[p] : 1); n++, t_e++, t_ec++)
                i_e(i, n) = t_e;
            }

          /* equations */
          type_f.resize(n_f);
          i_eq_flux.resize(n_f, mix ? M : 1);
          i_eq_flux = -1;
          i_eq_cont.resize(n_f, mix ? M : 1);
          i_eq_cont = -1;
          int k = 0;

          for (int i = 0; i < n_f; i++)
            {
              int p1 = s_pf[i][0];
              int p2 = m_pf.count(s_pf[i]) ? m_pf[s_pf[i]][0] : p1;
              int p12[2] = { p1, p2 };
              int n12[2] = { N[p1], N[p2] }; //nombres de composantes de chaque cote

              int f = s_pf[i][1];
              type_f[i] = fcl[p1](f, 0); //type de face

              if (type_f[i] && type_f[i] != 5)
                for (int j = 0; j < 2; j++)
                  if ( (sub_type(Energie_Multiphase, op_elem_->op_ext[p12[j]]->equation())
                        || sub_type(Convection_Diffusion_Temperature, op_elem_->op_ext[p12[j]]->equation())) //si flux parietal et CL non Neumann, il n'y a qu'une valeur en paroi
                       && op_elem_->op_ext[p12[j]]->equation().probleme().has_correlation("flux_parietal"))
                    {
                      n12[j] = 1;
                    }

              if (n12[0] != n12[1])
                {
                  Cerr << "Op_Diff_PolyMAC_P0_Elem : incompatibility between " << op_elem_->op_ext[p1]->equation().probleme().le_nom() <<
                       " and " << op_elem_->op_ext[p2]->equation().probleme().le_nom() << " ( " << n12[0]
                       << " vs " << n12[1] << " components)!" << finl;

                  Process::exit();
                }
              //equations de flux : tous types sauf Dirichlet et Echange_impose_base
              if (type_f[i] != 6 && type_f[i] != 7 && type_f[i] != 1 && type_f[i] != 2)
                for (int n = 0; n < (mix ? n12[0] : 1); n++)
                  {
                    i_eq_flux(i, n) = k;
                    k++;
                  }

              //equations de continuite : tous types sauf Neumann
              if (type_f[i] != 4 && type_f[i] != 5)
                for (int n = 0; n < (mix ? n12[0] : 1); n++)
                  {
                    i_eq_cont(i, n) = k;
                    k++;
                  }
            }

          //si inconnues de paroi de Pb_Multiphase : equations dues aux correlations de flux (dans ce cas mix = 1)
          if (mix)
            {
              i_eq_pbm.resize(t_eq);
              i_eq_pbm = -1;

              for (int i = 0; i < n_e; i++)
                {
                  int p = s_pe[i][0];

                  for (int j = 0; j < (int) se_f[i].size(); j++)
                    if (i_efs(i, j, M) >= 0)
                      for (int n = 0; n < N[p]; n++)
                        {
                          i_eq_pbm(i_efs(i, j, n)) = k;
                          k++;
                        }
                }
            }

          assert(k == t_eq); //a-ton bien autant d'equations que d'inconnues?

          for (int essai = 0; essai < 3; essai++) /* essai 0 : MPFA O -> essai 1 : MPFA O avec x_fs mobiles -> essai 2 : MPFA symetrique (corecive, mais pas tres consistante) */
            {
              if (essai == 1) /* essai 1 : tentative de symmetrisation en deplacant les x_fs. Si mix = 1, on ne peut pas les deplacer independamment */
                {
                  /* systeme lineaire */
                  const int nc = (D - 1) * n_f;
                  const int nl = D * (D - 1) / 2 * t_e;
                  const int n_m = std::max(nc, nl);

                  C.resize(Nm, nc, nl);
                  Y.resize(Nm, n_m);
                  C = 0.;
                  Y = 0.;

                  int il = 0;

                  for (int i = 0; i < n_e; i++)
                    {
                      int p = s_pe[i][0];
                      int e = s_pe[i][1];

                      for (int d = 0; d < D; d++)
                        for (int db = 0; db < d; db++, il += !mix)
                          for (int n = 0; n < N[p]; n++, il += mix)
                            for (int j = 0; j < (int) se_f[i].size(); j++)
                              {
                                k = se_f[i][j];
                                const int f = (s_pf[k][0] == p) ? s_pf[k][1] : m_pf[s_pf[k]][1]; //indice de face, num dans le probleme courant, amont/aval
                                const int sgn = e == f_e[p](f, 0) ? 1 : -1;

                                for (int l = 0; l < D; l++)
                                  fac[l] = sgn * domaine0.nu_dot(&diffu[p].get(), e, n, &nf[p](f, 0), i3[l]) * surf_fs[k] / fs[p](f) / vol_es[i]; //vecteur lambda_e nf sortant * facteur commun

                                Y(!mix * n, il) += fac[d] * (xv[p](f, db) - xp[p](e, db)) - fac[db] * (xv[p](f, d) - xp[p](e, d)); //second membre

                                for (int l = 0; l < D - 1; l++)
                                  C(!mix * n, (D - 1) * k + l, il) += fac[db] * vec_fs[k][l][d] - fac[d] * vec_fs[k][l][db]; //matrice
                              }
                    }


                  /* resolution -> DEGLSY */
                  int nw = -1, rk, infoo;
                  F77NAME(dgelsy)(&nl, &nc, &un, &C(0, 0, 0), &nl, &Y(0, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);

                  W.resize(nw = (int) (std::lrint(W(0))));
                  piv.resize(nc);

                  for (int n = 0; n < Nm; n++)
                    piv = 0, F77NAME(dgelsy)(&nl, &nc, &un, &C(n, 0, 0), &nl, &Y(n, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);

                  /* x_fs = xf + corrections */
                  x_fs.resize(Nm, n_f, D);
                  for (int n = 0; n < Nm; n++)
                    for (int i = 0; i < n_f; i++)
                      {
                        int p = s_pf[i][0];
                        int f = s_pf[i][1];

                        for (int d = 0; d < D; d++)
                          for (x_fs(n, i, d) = xv[p](f, d), k = 0; k < D - 1; k++)
                            x_fs(n, i, d) += std::min(std::max(Y(n, (D - 1) * i + k), 0.), 0.5) * vec_fs[i][k][d];
                      }
                }

              A.resize(Nm, t_eq, t_eq);
              B.resize(Nm, t_ec = t_e + 1, t_eq);
              Ff.resize(Nm, t_eq, t_eq);
              Fec.resize(Nm, t_eq, t_ec); //systeme A.dT_efs = B.{dT_eb, 1}, flux sortant a chaque face

              if (mix)
                {
                  Qf.resize(n_e, t_eq, M, M);
                  Qec.resize(n_e, t_ec, M, M); //si Pb_Multiphase : flux paroi-interface
                }

              /* debut du Newton sur les T_efs : initialisation aux temperatures de mailles */
              Tefs.resize(Nm, t_eq);

              for (int i = 0; i < n_e; i++)
                {
                  int p = s_pe[i][0];
                  int e = s_pe[i][1];

                  for (int j = 0; j < (int) se_f[i].size(); j++)
                    {
                      for (int n = 0; n < N[p]; n++)
                        Tefs(!mix * n, i_efs(i, j, mix * n)) = inco[p](e, n); //Tefs de chaque phase

                      if (mix && i_efs(i, j, M) >= 0)
                        Tefs(0, i_efs(i, j, M)) = inco[p](e, 0); //Tparoi : on prend la temperature de la phase 0 faute de mieux
                    }
                }

              int nonlinear = 0;
              int cv = 0;

              for (int it = 0; !cv && it < 100; it++) //Newton sur les Tefs. Si mix = 0 (pas de Pb_Multi), une seule iteration suffit
                {
                  A = 0.;
                  B = 0.;
                  Ff = 0.;
                  Fec = 0.;
                  Qf = 0.;
                  Qec = 0.;

                  for (int i = 0; i < n_e; i++)
                    {
                      int p = s_pe[i][0];
                      int e = s_pe[i][1];
                      n_ef = (int) se_f[i].size();

                      C.resize(Nm, n_ef, D);
                      const int n_m = std::max(D, n_ef);

                      Y.resize(Nm, D, n_m);
                      X.resize(Nm, n_ef, D);

                      /* gradient dans e */
                      if (essai < 2) /* essais 0 et 1 : gradient consistant */
                        {
                          /* gradient dans (e, s) -> matrice / second membre M.x = Y du systeme (grad u)_i = sum_f b_{fi} (x_fs_i - x_e), avec x_fs le pt de continuite de u_fs */
                          for (int n = 0; n < Nm; n++)
                            for (int j = 0; j < n_ef; j++)
                              {
                                k = se_f[i][j];
                                const int pb = s_pf[k][0];
                                const int f = s_pf[k][1];

                                for (int d = 0; d < D; d++)
                                  C(n, j, d) = (essai ? x_fs(n, k, d) : xv[pb](f, d)) - xp[p](e, d);
                              }

                          Y = 0.;

                          for (int n = 0; n < Nm; n++)
                            for (int d = 0; d < D; d++)
                              Y(n, d, d) = 1.;

                          int nw = -1, rk, infoo;

                          F77NAME(dgelsy)(&D, &n_ef, &D, &C(0, 0, 0), &D, &Y(0, 0, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);

                          nw = (int) (std::lrint(W(0)));

                          W.resize(nw);
                          piv.resize(n_m);

                          for (int n = 0; n < Nm; n++)
                            {
                              piv = 0;
                              F77NAME(dgelsy)(&D, &n_ef, &D, &C(n, 0, 0), &D, &Y(n, 0, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);
                            }

                          for (int n = 0; n < Nm; n++)
                            for (int j = 0; j < n_ef; j++)
                              for (int d = 0; d < D; d++)
                                X(n, j, d) = Y(n, d, j); /* pour pouvoir utiliser nu_dot */
                        }
                      else /* essai 2 : gradient non consistant */
                        {
                          for (int n = 0; n < Nm; n++)
                            for (int j = 0; j < n_ef; j++)
                              {
                                k = se_f[i][j];
                                const int pb = s_pf[k][0];
                                const int f = s_pf[k][1];
                                const int sgn = (pb == p) && (e == f_e[p](f, 0)) ? 1 : -1;

                                for (int d = 0; d < D; d++)
                                  X(n, j, d) = surf_fs[k] / vol_es[i] * sgn * nf[pb](f, d) / fs[pb](f); /* essai 2 : gradient non consistant */
                              }
                        }

                      /* remplissage de A, B, Ff, Fec */
                      for (int j = 0; j < n_ef; j++)
                        {
                          k = se_f[i][j];
                          const int f = (s_pf[k][0] == p) &&
                                        (e == f_e[p](s_pf[k][1], 0) || e == f_e[p](s_pf[k][1], 1)) ? s_pf[k][1] : m_pf[s_pf[k]][1]; //indice de face, numero de face local

                          const int sgn_l = (e == f_e[p](f, 0)) ? 1 : -1;
                          const int sgn = (p == s_pf[k][0]) && (f == s_pf[k][1]) ?
                                          sgn_l : -1; //orientation de la face locale, de la face globale

                          /* flux : remplit Ff / Fec (format standard) dans tous les cas, et les equations de A/B donnees par i_eq_flux / i_eq_cont (format LAPACK) */
                          for (int l = 0; l < n_ef; l++)
                            {
                              for (int n = 0; n < N[p]; n++)
                                {
                                  const double x = sgn_l * domaine0.nu_dot(&diffu[p].get(), e, n, &nf[p](f, 0), &X(!mix * n, l, 0)) / fs[p](f);
                                  const double y = x * surf_fs[k];

                                  /* stockage du flux sortant dans Ff / Fec */
                                  Fec(!mix * n, i_efs(i, j, mix * n), t_e) += y * (Tefs(!mix * n, i_efs(i, l, mix * n)) - inco[p](e, n));

                                  Ff(!mix * n, i_efs(i, j, mix * n), i_efs(i, l, mix * n)) += y;

                                  Fec(!mix * n, i_efs(i, j, mix * n), i_e(i, mix * n)) -= y;

                                  /* equations de flux : on contribue a celle de la phase (si elle existe), ou a defaut a celle de la phase 0 */
                                  int i_eq = i_eq_flux(k, mix * n);

                                  if (i_eq < 0)
                                    i_eq = i_eq_flux(k, 0); /* ou a defaut a celle de la phase 0 */

                                  if (i_eq >= 0)
                                    {
                                      B(!mix * n, t_e, i_eq) += x * (Tefs(!mix * n, i_efs(i, l, mix * n)) - inco[p](e, n));

                                      A(!mix * n, i_efs(i, l, mix * n), i_eq) -= x;

                                      B(!mix * n, i_e(i, mix * n), i_eq) -= x;
                                    }

                                  /* equations de continuite : on y contribue si on est l'amont d'un Echange_contact pour prendre en compte son coeff d'echange */
                                  i_eq = i_eq_cont(k, mix * n);
                                  if (i_eq < 0)
                                    i_eq = i_eq_cont(k, 0); /* ou a defaut a celle de la phase 0 */

                                  if (sgn > 0 && (type_f[k] && type_f[k] < 4) && (i_eq >= 0) )
                                    {

                                      const double h = (type_f[k] == 3) ? -1 /* inutile */:
                                                       ref_cast(Echange_impose_base, cls[p].get()[fcl[p](f, 1)].valeur()).h_imp(fcl[p](f, 1), !mix || (i_eq == i_eq_cont(k, n)) ? n : 0);

                                      const double invh = type_f[k] == 3 ? ref_cast(Echange_contact_PolyMAC_P0, cls[p].get()[fcl[p](f, 1)].valeur()).invh_paroi :
                                                          1. / std::max(h, 1e-10);

                                      B(!mix * n, t_e, i_eq) += invh * x * (Tefs(!mix * n, i_efs(i, l, mix * n)) - inco[p](e, n));

                                      A(!mix * n, i_efs(i, l, mix * n), i_eq) -= invh * x;

                                      B(!mix * n, i_e(i, mix * n), i_eq) -= invh * x;
                                    }

                                  /* si Pb_Multiphase : partie "flux" de l'equation sur la correlation */

                                  if (mix)
                                    {
                                      i_eq = i_eq_pbm(i_efs(i, j, mix * n));
                                      if (i_eq >= 0)
                                        {
                                          B(!mix * n, t_e, i_eq) += x * (Tefs(!mix * n, i_efs(i, l, mix * n)) - inco[p](e, n));

                                          A(!mix * n, i_efs(i, l, mix * n), i_eq) -= x;

                                          B(!mix * n, i_e(i, mix * n), i_eq) -= x;
                                        }
                                    }
                                }
                            }

                          /* autres equations : continuite, correlations */
                          if (mix && i_efs(i, j, M) >= 0) /* inconnue de Tparoi -> Pb_Multiphase */
                            {
                              //equation de continuite : avec la Tparoi
                              int i_eq = i_eq_cont(k, 0);

                              if (i_eq >= 0)
                                {
                                  B(0, t_e, i_eq) += sgn * Tefs(0, i_efs(i, j, M));

                                  A(0, i_efs(i, j, M), i_eq) -= sgn;
                                }

                              //equations sur les correlations
                              const Probleme_base& pbp = op_elem_->op_ext[p]->equation().probleme();
                              const Flux_parietal_base& corr = ref_cast(Flux_parietal_base, pbp.get_correlation("Flux_parietal"));

                              const DoubleTab *alpha = sub_type(Pb_Multiphase, pbp) ? &ref_cast(Pb_Multiphase, pbp).equation_masse().inconnue().passe() : nullptr;

                              const DoubleTab& dh = pbp.milieu().diametre_hydraulique_elem(),
                                               &press = ref_cast(Navier_Stokes_std, pbp.equation(0)).pression().passe(),
                                                &vit = ref_cast(Navier_Stokes_std, pbp.equation(0)).inconnue().passe(),
                                                 &lambda = pbp.milieu().conductivite().passe(),
                                                  &mu = ref_cast(Fluide_base, pbp.milieu()).viscosite_dynamique().passe(),
                                                   &rho = pbp.milieu().masse_volumique().passe(),
                                                    &Cp = pbp.milieu().capacite_calorifique().passe();

                              /* uniforme fields ??? */
                              const int Clambda = (lambda.dimension(0) == 1), Cmu = (mu.dimension(0) == 1),
                                        Crho = (rho.dimension(0) == 1), Ccp = (Cp.dimension(0) == 1);

                              Flux_parietal_base::input_t in;
                              Flux_parietal_base::output_t out;

                              DoubleTrav Tf(N[p]), qpk(N[p]), dTf_qpk(N[p], N[p]),
                                         dTp_qpk(N[p]), qpi(N[p], N[p]), dTf_qpi(N[p], N[p], N[p]),
                                         dTp_qpi(N[p], N[p]), nv(N[p]), d_nuc(N[p]);


                              /* fill inputs */
                              in.N = N[p];
                              in.f = f;
                              in.y = (e == f_e[p](f, 0)) ? domaine[p].get().dist_face_elem0(f, e) : domaine[p].get().dist_face_elem1(f, e);
                              in.D_h = dh(e);
                              in.D_ch = dh(e);
                              in.alpha = alpha ? &(*alpha)(e, 0) : nullptr;
                              in.T = &Tf(0);
                              in.p = press(e);
                              in.v = nv.addr();
                              in.Tp = Tefs(0, i_efs(i, j, M));
                              in.lambda = &lambda(!Clambda * e, 0);
                              in.mu = &mu(!Cmu * e, 0);
                              in.rho = &rho(!Crho * e, 0);
                              in.Cp = &Cp(!Ccp * e, 0);

                              if (corr.needs_saturation())
                                {
                                  in.Lvap = &Lvap_tab[p](e, 0);
                                  in.Sigma = &Sigma_tab[p](e, 0);
                                  in.Tsat = &Ts_tab[p](e, 0);
                                }
                              else
                                {
                                  in.Lvap = nullptr;
                                  in.Sigma = nullptr;
                                  in.Tsat = nullptr;
                                }

                              /* init outputs */
                              out.qpk = &qpk;
                              out.dTf_qpk = &dTf_qpk;
                              out.dTp_qpk = &dTp_qpk;
                              out.qpi = &qpi;
                              out.dTp_qpi = &dTp_qpi;
                              out.dTf_qpi = &dTf_qpi;
                              out.nonlinear = &nonlinear;
                              out.d_nuc = &d_nuc;

                              for (int d = 0; d < D; d++)
                                for (int n = 0; n < N[p]; n++)
                                  nv(n) += std::pow(vit(domaine[p].get().nb_faces_tot() + D * e + d, n), 2);

                              for (int n = 0; n < N[p]; n++)
                                {
                                  nv(n) = sqrt(nv(n));
                                  Tf(n) = corr.T_at_wall() ? Tefs(0, i_efs(i, j, n)) : inco[p](e, n);
                                }

                              // appel : on n'est implicite qu'en les temperatures
                              corr.qp(in, out);

                              /* qpk : contributions aux equations sur les Tkp */
                              for (int n = 0; n < N[p]; n++)
                                {
                                  i_eq = i_eq_pbm(i_efs(i, j, n));

                                  B(0, t_e, i_eq) -= qpk(n);

                                  A(0, i_efs(i, j, M), i_eq) += dTp_qpk(n);

                                  for (int m = 0; corr.T_at_wall() && m < N[p]; m++)
                                    A(0, i_efs(i, j, m), i_eq) += dTf_qpk(n, m);
                                }

                              /* qpi : contribution a l'equation de flux a la face (si elle existe), contributions au tableau de flux paroi-interface */
                              i_eq = i_eq_flux(k, 0);
                              if (i_eq >= 0)
                                for (int k1 = 0; k1 < N[p]; k1++)
                                  for (int k2 = k1 + 1; k2 < N[p]; k2++) //partie constante, derivee en Tp
                                    {
                                      B(0, t_e, i_eq) += qpi(k1, k2);

                                      A(0, i_efs(i, j, M), i_eq) -= dTp_qpi(k1, k2);

                                      for (int n = 0; corr.T_at_wall() && n < N[p]; n++)
                                        A(0, i_efs(i, j, n), i_eq) -= dTf_qpi(k1, k2, n);
                                    }

                              for (int k1 = 0; k1 < N[p]; k1++)
                                for (int k2 = k1 + 1; k2 < N[p]; k2++) //partie constante, derivee en Tp
                                  {
                                    Qec(i, t_e, k1, k2) += surf_fs[k] * qpi(k1, k2);

                                    Qf(i, i_efs(i, j, M), k1, k2) += surf_fs[k] * dTp_qpi(k1, k2);

                                    for (int m = 0; corr.T_at_wall() && m < N[p]; m++)
                                      Qf(i, i_efs(i, j, m), k1, k2) += surf_fs[k] * dTf_qpi(k1, k2, m); //derivees en Tf
                                  }

                              if (d_nuc_.dimension(0) && !d_nuc_a_jour_)
                                for (int k1 = 0; k1 < N[p]; k1++) // d_nuc depends on the temperature so must only be updated once when the temperature input of the wall flux correlation is the old temperature
                                  d_nuc_(e, k1) += d_nuc(k1), d_nuc_a_jour_ = 1;
                            }
                          else /* pas d'inconnue de Tparoi -> continuite composante par composante */
                            {
                              for (int n = 0; n < N[p]; n++)
                                {
                                  const int i_eq = i_eq_cont(k, mix * n);
                                  if (i_eq >= 0) /* pas d'inconnue de Tparoi -> continuite composante par composante */
                                    {
                                      B(!mix * n, t_e, i_eq) += sgn * Tefs(!mix * n, i_efs(i, j, mix * n));

                                      A(!mix * n, i_efs(i, j, mix * n), i_eq) -= sgn;
                                    }
                                }
                            }

                          /* contributions CLs : aux equations de flux si Neumann, a celles de continuite si Dirichlet ou Echange_impose */
                          if (type_f[k] == 1 || type_f[k] == 2)
                            for (int n = 0; n < N[p]; n++)
                              {
                                const int i_eq = i_eq_cont(k, mix * n);
                                if (i_eq >= 0) //Echange_impose_base
                                  B(!mix * n, t_e, i_eq) -= ref_cast(Echange_impose_base, cls[p].get()[fcl[p](f, 1)].valeur()).T_ext(fcl[p](f, 2), n);
                              }

                          if (type_f[k] == 6)
                            for (int n = 0; n < N[p]; n++)
                              {
                                const int i_eq = i_eq_cont(k, mix * n);
                                if (i_eq >= 0) //Dirichlet (non homogene)
                                  B(!mix * n, t_e, i_eq) -= ref_cast(Dirichlet, cls[p].get()[fcl[p](f, 1)].valeur()).val_imp(fcl[p](f, 2), n);
                              }

                          if (type_f[k] == 4)
                            for (int n = 0; n < N[p]; n++)
                              {
                                const int i_eq = i_eq_flux(k, mix * n);
                                if (i_eq >= 0) //Neumann
                                  B(!mix * n, t_e, i_eq) -= ref_cast(Neumann, cls[p].get()[fcl[p](f, 1)].valeur()).flux_impose(fcl[p](f, 2), n);
                              }
                        }
                    }
                  /* resolution(s) -> DGELSY */
                  int nw = -1, rk, infoo;

                  F77NAME(dgelsy)(&t_eq, &t_eq, &t_ec, &A(0, 0, 0), &t_eq, &B(0, 0, 0), &t_eq, &piv(0), &eps, &rk, &W(0), &nw, &infoo);

                  piv.resize(t_eq);

                  nw = (int) (std::lrint(W(0)));
                  W.resize(nw);

                  for (int n = 0; n < Nm; n++)
                    {
                      piv = 0;
                      F77NAME(dgelsy)(&t_eq, &t_eq, &t_ec, &A(n, 0, 0), &t_eq, &B(n, 0, 0), &t_eq, &piv(0), &eps, &rk, &W(0), &nw, &infoo);
                    }

                  /* mise a jour des Tefs et convergence. Si nonlinear = 0, tout est lineaire -> pas besoin d'autres iterations */
                  for (int n = 0; n < Nm; n++)
                    {
                      cv = 1;
                      for (int i = 0; i < t_eq; i++)
                        {
                          Tefs(n, i) += B(n, t_e, i);
                          cv &= !nonlinear || std::fabs(B(n, t_e, i)) < 1e-3;
                        }
                    }
                }

              if (!cv && essai < 2)
                continue; //T_efs pas converge avec flux non coercif -> on essaie de stabiliser
              else if (!cv)
                {
                  Cerr << "non-convergence des T_efs!" << finl;
                  Process::exit();
                }

              /* subbstitution dans des u_efs^n dans Fec et Qec */
              for (int n = 0; n < Nm; n++)
                for (int i = 0; i < t_eq; i++)
                  for (int j = 0; j < t_ec; j++)
                    for (k = 0; k < t_eq; k++)
                      Fec(n, i, j) += Ff(n, i, k) * B(n, j, k);

              if (mix)
                for (int i = 0; i < n_e; i++)
                  for (int j = 0; j < t_ec; j++)
                    for (k = 0; k < t_eq; k++)
                      for (int k1 = 0; k1 < M; k1++)
                        for (int k2 = k1 + 1; k2 < M; k2++)
                          Qec(i, j, k1, k2) += Qf(i, k, k1, k2) * B(0, j, k);

              /* A : forme(s) bilineaire */
              if (essai == 2)
                break; //pas la peine pour VFSYM

              A.resize(Nm, t_e, t_e);
              A = 0.;

              for (int m = 0; m < Nm; m++)
                for (int i = 0; i < n_e; i++)
                  {
                    const int p = s_pe[i][0];

                    for (int j = 0; j < (int) se_f[i].size(); j++)
                      for (int n = 0; n < (mix ? N[p] : 1); n++)
                        for (int l = 0; l < t_e; l++)
                          A(m, i_e(i, n), l) -= Fec(m, i_efs(i, j, n), l);
                  }


              /* symmetrisation */
              for (int n = 0; n < Nm; n++)
                for (int i = 0; i < t_e; i++)
                  for (int j = 0; j <= i; j++)
                    {
                      A(n, i, j) = A(n, j, i) = (A(n, i, j) + A(n, j, i)) / 2.;
                    }

              /* v.p. la plus petite : DSYEV */
              int nw = -1, infoo;
              F77NAME(DSYEV)("N", "U", &t_e, &A(0, 0, 0), &t_e, S.addr(), &W(0), &nw, &infoo);

              nw = (int) (std::lrint(W(0)));
              W.resize(nw);
              S.resize(t_e);
              cv = 1;

              for (int n = 0; n < Nm; n++)
                F77NAME(DSYEV)("N", "U", &t_e, &A(n, 0, 0), &t_e, &S(0), &W(0), &nw, &infoo), cv &= S(0) > -1e-8 * vol_s;

              if (cv)
                break;
            }

          /* contributions aux flux et aux matrices */
          for (int i = 0; i < n_e; i++)
            {
              const int e = s_pe[i][1];

              if ((s_pe[i][0] == 0) && (e < domaine[0].get().nb_elem()))
                for (int j = 0; j < (int) se_f[i].size(); j++)
                  {
                    k = se_f[i][j];
                    const int f = s_pf[k][0] ? m_pf[s_pf[k]][1] : s_pf[k][1];

                    for (int n = 0; n < N[0]; n++) //seulement celles du probleme courant
                      {
                        secmem(e, n) += Fec(!mix * n, i_efs(i, j, mix * n), t_e); //partie constante

                        if (f < domaine0.premiere_face_int())
                          op_elem_->flux_bords()(f, n) += Fec(!mix * n, i_efs(i, j, mix * n), t_e);

                        for (k = 0; k < n_e; k++)
                          {
                            const int p = s_pe[k][0];

                            if (mat[p])
                              {
                                const int eb = s_pe[k][1];

                                for (int m = (mix ? 0 : n); m < (mix ? N[p] : n + 1); m++) // derivees : si on a la matrice
                                  (*mat[p])(N[0] * e + n, N[p] * eb + m) -= Fec(!mix * n, i_efs(i, j, mix * n), i_e(k, mix * m));
                              }
                          }
                      }
                  }
            }

          /* contributions a qpi : partie constante seulement pour le moment */
          if (pqpi)
            for (int i = 0; i < n_e; i++)
              if (s_pe[i][0] == 0)
                {
                  const int e = s_pe[i][1];

                  for (int k1 = 0; k1 < N[0]; k1++)
                    for (int k2 = k1 + 1; k2 < N[0]; k2++)
                      (*pqpi)(e, k1, k2) += Qec(i, t_e, k1, k2);
                }
        }
    }

  if (pqpi)
    (*pqpi).echange_espace_virtuel();
}
