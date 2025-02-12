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

#include <Op_Grad_VEF_P1B_Face.h>
#include <Check_espace_virtuel.h>
#include <Neumann_sortie_libre.h>
#include <Dirichlet_homogene.h>
#include <Schema_Temps_base.h>
#include <Navier_Stokes_std.h>
#include <communications.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Periodique.h>
#include <Dirichlet.h>
#include <TRUSTTrav.h>
#include <Symetrie.h>
#include <Domaine.h>
#include <Device.h>
#include <Debog.h>

Implemente_instanciable(Op_Grad_VEF_P1B_Face, "Op_Grad_VEFPreP1B_P1NC|Op_Grad_VEF_P1NC", Operateur_Grad_base);

Sortie& Op_Grad_VEF_P1B_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Grad_VEF_P1B_Face::readOn(Entree& s) { return s; }

const Domaine_VEF& Op_Grad_VEF_P1B_Face::domaine_vef() const
{
  return ref_cast(Domaine_VEF, le_dom_vef.valeur());
}

void Op_Grad_VEF_P1B_Face::associer(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_Cl_dis, const Champ_Inc_base&)
{
  le_dom_vef = ref_cast(Domaine_VEF, domaine_dis);
  la_zcl_vef = ref_cast(Domaine_Cl_VEF, domaine_Cl_dis);
}

static int chercher_arete(int elem, int somi, int somj, const IntTab& elem_aretes, const IntTab& aretes_som)
{
  if (somi > somj)
    {
      int k = somi;
      somi = somj;
      somj = k;
    }
  for (int i_arete = 0; i_arete < 6; i_arete++)
    {
      int arete = elem_aretes(elem, i_arete);
      int som1 = aretes_som(arete, 0);
      if (somi == som1)
        {
          int som2 = aretes_som(arete, 1);
          if (somj == som2)
            return arete;
        }
    }
  return -1;
}
static void verifier(const Op_Grad_VEF_P1B_Face& op, int& init, const Domaine_VEF& domaine_VEF, const DoubleTab& pre, DoubleTab& grad)

{
  // Methode verifier ne marche que pour P0+P1 en 2D et P0+P1+Pa en 3D
  if (Objet_U::dimension == 2 && domaine_VEF.get_alphaE() + domaine_VEF.get_alphaS() != 2)
    {
      Cerr << "Methode verifier de Op_Grad_VEF_P1B_Face non valable pour cette discretisation." << finl;
      Process::exit();
    }
  if (Objet_U::dimension == 3 && domaine_VEF.get_alphaE() + domaine_VEF.get_alphaS() + domaine_VEF.get_alphaA() != 3)
    {
      Cerr << "Methode verifier de Op_Grad_VEF_P1B_Face non valable pour cette discretisation." << finl;
      Process::exit();
    }
  const Domaine& domaine = domaine_VEF.domaine();
  const Domaine& dom = domaine;
  int nb_elem_tot = domaine.nb_elem_tot();
  int nb_som_tot = dom.nb_som_tot();
  int nb_elem = domaine.nb_elem();
  init = 1;
  const DoubleVect& volumes_entrelaces = domaine_VEF.volumes_entrelaces();
  const DoubleTab& xp = domaine_VEF.xp();
  const DoubleTab& coord_sommets = dom.coord_sommets();

  // Verification de la pression arete
  DoubleTab tab(pre);
  exemple_champ_non_homogene(domaine_VEF, tab);

  DoubleTab r(grad);
  r = 0;
  op.ajouter(tab, r);

  // Pression mise a 1 sur tous les noeuds
  DoubleTab p(pre);
  p = 1;
  r = 0;
  op.ajouter(p, r);
  if (nb_elem < 30)
    {
      Cerr << Process::me() << " grad(1) som = " << finl;
      r.ecrit(Cerr);
    }
  // Pression mise a 1 sur les elements seulement
  p = 0;
  int i = 0;
  for (; i < nb_elem_tot; i++)
    p(i) = 0;
  r = 0;
  op.ajouter(p, r);
  Cerr << "[" << Process::me() << "] p(1) elem = " << finl;
  p.ecrit(Cerr);
  Cerr << "[" << Process::me() << "] grad(1) elem = " << finl;
  r.ecrit(Cerr);

  // Pression mise a 1 sur les sommets seulement
  p = 0;
  for (; i < nb_elem_tot + nb_som_tot; i++)
    p(i) = 1;
  r = 0;
  op.ajouter(p, r);
  Cerr << Process::me() << " grad(1) som = " << finl;
  r.ecrit(Cerr);

  // Pression mise a 1 sur les aretes seulement
  p = 0;
  int sz = p.size_totale();
  for (; i < sz; i++)
    p(i) = 1;
  r = 0;
  op.ajouter(p, r);
  Cerr << Process::me() << " grad(1) aretes = " << finl;
  r.ecrit(Cerr);
  // Pression variable sur les elements seulement
  p = 0;
  for (i = 0; i < nb_elem_tot; i++)
    p(i) = xp(i, 0) - 1;
  r = 0;
  op.ajouter(p, r);
  int nbf = volumes_entrelaces.size();
  for (i = 0; i < nbf; i++)
    for (int comp = 0; comp < Objet_U::dimension; comp++)
      r(i, comp) /= volumes_entrelaces(i);
  Cerr << Process::me() << " grad(x-1) elem = " << finl;
  r.ecrit(Cerr);

  // Pression variable sur les sommets seulement
  p = 0;
  for (i = nb_elem_tot; i < nb_elem_tot + nb_som_tot; i++)
    {
      p(i) = coord_sommets(i - nb_elem_tot, 0) - 1;
    }
  r = 0;
  op.ajouter(p, r);
  for (i = 0; i < nbf; i++)
    for (int comp = 0; comp < Objet_U::dimension; comp++)
      r(i, comp) /= volumes_entrelaces(i);
  Cerr << Process::me() << " grad(x-1) som = " << finl;
  r.ecrit(Cerr);
}

DoubleTab& Op_Grad_VEF_P1B_Face::modifier_grad_pour_Cl(DoubleTab& tab_grad) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Conds_lim& les_cl = domaine_Cl_VEF.les_conditions_limites();
  int nb_bords = les_cl.size();
  int dim = Objet_U::dimension;
  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      {
        const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
        int num1 = le_bord.num_premiere_face();
        int num2 = num1 + le_bord.nb_faces();
        if (sub_type(Periodique, la_cl.valeur()))
          {
            const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
            CIntArrView face_associee = la_cl_perio.face_associee().view_ro();
            DoubleTabView grad = tab_grad.view_rw();
            Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(num1, num1+le_bord.nb_faces()/2), KOKKOS_LAMBDA(const int face)
            {
              int face_ass = num1 + face_associee(face - num1);
              for (int comp = 0; comp < dim; comp++)
                {
                  grad(face, comp) += grad(face_ass, comp);
                  grad(face_ass, comp) = grad(face, comp);
                }
            });
            end_gpu_timer(__KERNEL_NAME__);
          }
        else
          {
            if (sub_type(Symetrie, la_cl.valeur()))
              {
                CDoubleTabView face_normales = domaine_VEF.face_normales().view_ro();
                DoubleTabView grad = tab_grad.view_rw();
                Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(num1, num2), KOKKOS_LAMBDA(const int face)
                {
                  double psc = 0;
                  double norm = 0;
                  for (int comp = 0; comp < dim; comp++)
                    {
                      psc += grad(face, comp) * face_normales(face, comp);
                      norm += face_normales(face, comp) * face_normales(face, comp);
                    }
                  // psc/=norm; // Fixed bug: Arithmetic exception
                  if (std::fabs(norm) >= DMINFLOAT)
                    psc /= norm;
                  for (int comp = 0; comp < dim; comp++)
                    grad(face, comp) -= psc * face_normales(face, comp);
                });
                end_gpu_timer(__KERNEL_NAME__);
              }
            else if (sub_type(Dirichlet,la_cl.valeur()) || sub_type(Dirichlet_homogene, la_cl.valeur()))
              {
                DoubleTabView grad = tab_grad.view_wo();
                Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::MDRangePolicy<Kokkos::Rank<2>>({num1,0}, {num2,dim}), KOKKOS_LAMBDA(const int face, const int comp)
                {
                  grad(face, comp) = 0;
                });
                end_gpu_timer(__KERNEL_NAME__);
              }
          }
      }
    }
  return tab_grad;
}

DoubleTab& Op_Grad_VEF_P1B_Face::ajouter_elem(const DoubleTab& pre, DoubleTab& tab_grad) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  assert(domaine_VEF.get_alphaE());
  const Domaine& domaine = domaine_VEF.domaine();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  int nfe = domaine.nb_faces_elem();
  int nb_elem_tot = domaine.nb_elem_tot();
  CDoubleArrView porosite_face = equation().milieu().porosite_face().view_ro();
  CDoubleTabView face_normales = domaine_VEF.face_normales().view_ro();
  // Si pas de support P1, on impose Neumann sur P0
  if (domaine_VEF.get_alphaS() == 0)
    {
      const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
      const Conds_lim& les_cl = domaine_Cl_VEF.les_conditions_limites();
      int nb_bords = les_cl.size();
      int dim = Objet_U::dimension;
      for (int n_bord = 0; n_bord < nb_bords; n_bord++)
        {
          const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
          if (sub_type(Neumann_sortie_libre, la_cl.valeur()))
            {
              const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre, la_cl.valeur());
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
              int num1 = le_bord.num_premiere_face();
              int num2 = num1 + le_bord.nb_faces();
              CDoubleTabView flux_impose = la_sortie_libre.flux_impose().view_ro();
              DoubleTabView grad = tab_grad.view_rw();
              Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(num1, num2), KOKKOS_LAMBDA(const int face)
              {
                double P_imp = flux_impose(face - num1, 0);
                double diff = P_imp;
                for (int comp = 0; comp < dim; comp++)
                  grad(face, comp) += diff * face_normales(face, comp) * porosite_face(face);
              });
              end_gpu_timer(__KERNEL_NAME__);
            }
        }
    }

  CIntTabView face_voisins = domaine_VEF.face_voisins().view_ro();
  CIntTabView elem_faces_v = elem_faces.view_ro();
  CDoubleTabView pre_v = pre.view_ro();
  DoubleTabView grad = tab_grad.view_rw();
  int dim = Objet_U::dimension;

  auto kern_elem = KOKKOS_LAMBDA(int
                                 elem)
  {
    for (int indice = 0; indice < nfe; indice++)
      {
        double pe = pre_v(elem, 0);
        int face = elem_faces_v(elem, indice);
        double signe = 1;
        if (elem != face_voisins(face, 0)) signe = -1;
        for (int comp = 0; comp < dim; comp++)
          {
            double val = pe * signe * face_normales(face, comp) * porosite_face(face);
            Kokkos::atomic_sub(&grad(face, comp), val);
          }
      }
  };

  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_elem_tot, kern_elem);
  end_gpu_timer(__KERNEL_NAME__);

  return tab_grad;
}

DoubleTab& Op_Grad_VEF_P1B_Face::ajouter_som(const DoubleTab& tab_pre, DoubleTab& tab_grad) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  assert(domaine_VEF.get_alphaS());
  const Domaine& domaine = domaine_VEF.domaine();
  const Domaine& dom = domaine;
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();

  const DoubleTab& tab_face_normales = domaine_VEF.face_normales();
  const DoubleVect& tab_porosite_face = equation().milieu().porosite_face();
  const IntTab& tab_som_elem = domaine.les_elems();
  const IntTab& tab_elem_faces = domaine_VEF.elem_faces();
  const IntTab& tab_face_voisins = domaine_VEF.face_voisins();

  int nfe = domaine.nb_faces_elem();
  int nps = domaine_VEF.numero_premier_sommet();
  int nb_elem_tot = domaine.nb_elem_tot();

  if (som_.size_array() == 0)
    {
      // Tableaux constants
      som_.resize(nb_elem_tot, nfe);
      coeff_som_.resize(nb_elem_tot);
      for (int el = 0; el < nb_elem_tot; el++)
        {
          coeff_som_(el) = calculer_coef_som(el, domaine_Cl_VEF, domaine_VEF);
          for (int indice = 0; indice < nfe; indice++)
            som_(el, indice) = nps + dom.get_renum_som_perio(tab_som_elem(el, indice));
        }
    }

  CIntTabView elem_faces = tab_elem_faces.view_ro();
  CIntTabView face_voisins = tab_face_voisins.view_ro();
  CDoubleTabView face_normales = tab_face_normales.view_ro();
  CDoubleArrView porosite_face = tab_porosite_face.view_ro();
  CDoubleArrView coeff_som = coeff_som_.view_ro();
  CIntTabView som_v = som_.view_ro();
  CDoubleArrView pre = static_cast<const ArrOfDouble&>(tab_pre).view_ro();
  DoubleTabView grad = tab_grad.view_rw();
  int dim = Objet_U::dimension;

  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                       Kokkos::MDRangePolicy<Kokkos::Rank<2>>({0, 0}, {nb_elem_tot, nfe}),
                       KOKKOS_LAMBDA (int elem, int indice)
  {
    int face = elem_faces(elem,indice);

    double signe = 1;
    if (elem != face_voisins(face,0))
      signe = -1;

    double sigma[3];
    for (int comp = 0; comp < dim; comp++)
      sigma[comp] = face_normales(face,comp) * signe;

    for (int indice2 = 0; indice2 < nfe; indice2++)
      {
        int face2 = elem_faces(elem,indice2);
        for (int comp = 0; comp < dim; comp++)
          {
            Kokkos::atomic_add(&grad(face2,comp), -(coeff_som(elem) * pre(som_v(elem,indice)) * sigma[comp] * porosite_face(face2)));
          }
      }
  });
  end_gpu_timer(__KERNEL_NAME__);

  bool has_sortie_libre = false;
  const Conds_lim& les_cl = domaine_Cl_VEF.les_conditions_limites();
  int nb_bords = les_cl.size();
  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Neumann_sortie_libre, la_cl.valeur())) has_sortie_libre = true;
    }

  if (has_sortie_libre)
    {
      for (int n_bord = 0; n_bord < nb_bords; n_bord++)
        {
          const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
          if (sub_type(Neumann_sortie_libre, la_cl.valeur()))
            {
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
              const Neumann_sortie_libre& sortie_libre = ref_cast(Neumann_sortie_libre, la_cl.valeur());
              int num1 = le_bord.num_premiere_face();
              int num2 = num1 + le_bord.nb_faces();
              CDoubleTabView flux_impose = sortie_libre.flux_impose().view_ro();
              CIntTabView face_sommets = domaine_VEF.face_sommets().view_ro();
              CIntArrView renum_som_perio = dom.get_renum_som_perio().view_ro();
              Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(num1, num2), KOKKOS_LAMBDA(const int face)
              {
                const double P_imp = flux_impose(face - num1, 0);
                for (int indice = 0; indice < (nfe - 1); indice++)
                  {
                    int som = nps + renum_som_perio(face_sommets(face, indice));
                    for (int comp = 0; comp < dim; comp++)
                      grad(face, comp) -= 1. / dim * face_normales(face, comp) * (pre(som) - P_imp) *
                                          porosite_face(face);
                  }
              });
              end_gpu_timer(__KERNEL_NAME__);
            }
        }
    }

  return tab_grad;
}

DoubleTab& Op_Grad_VEF_P1B_Face::ajouter_aretes(const DoubleTab& pre, DoubleTab& grad) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  assert(domaine_VEF.get_alphaA());
  const Domaine& domaine = domaine_VEF.domaine();
  //const Domaine& dom=domaine;
  const DoubleTab& face_normales = domaine_VEF.face_normales();
  const DoubleVect& porosite_face = equation().milieu().porosite_face();
  const IntTab& som_elem = domaine.les_elems();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const IntTab& aretes_som = domaine_VEF.domaine().aretes_som();
  const IntTab& elem_aretes = domaine_VEF.domaine().elem_aretes();
  const ArrOfInt& renum_arete_perio = domaine_VEF.get_renum_arete_perio();
  const ArrOfInt& ok_arete = domaine_VEF.get_ok_arete();
  int nfe = domaine.nb_faces_elem();
  int nb_elem_tot = domaine.nb_elem_tot();
  //  int nps=domaine_VEF.numero_premier_sommet();
  int npa = domaine_VEF.numero_premiere_arete();
  int indice;
  ArrOfDouble sigma(dimension);
  for (int elem = 0; elem < nb_elem_tot; elem++)
    {
      for (indice = 0; indice < nfe; indice++)
        {
          //som = nps+dom.get_renum_som_perio(som_elem(elem,indice));
          int face = elem_faces(elem, indice);
          double signe = 1;
          if (elem != face_voisins(face, 0))
            signe = -1;
          for (int comp = 0; comp < dimension; comp++)
            sigma[comp] = signe * face_normales(face, comp);
        }
      for (int isom = 0; isom < 3; isom++)
        {
          int somi = som_elem(elem, isom);
          int facei = elem_faces(elem, isom);
          double signei = 1.;
          if (face_voisins(facei, 0) != elem)
            signei = -1.;
          for (int jsom = isom + 1; jsom < 4; jsom++)
            {
              int somj = som_elem(elem, jsom);
              int facej = elem_faces(elem, jsom);
              double signej = 1.;
              if (face_voisins(facej, 0) != elem)
                signej = -1.;
              int arete = chercher_arete(elem, somi, somj, elem_aretes, aretes_som);
              arete = renum_arete_perio[arete];
              if (ok_arete[arete])
                {
                  double pa = pre(npa + arete);
                  int niinij = 0;
                  for (int ksom = 0; ksom < 4; ksom++)
                    {
                      int face2 = elem_faces(elem, ksom);
                      if ((ksom == isom) || (ksom == jsom))
                        {
                          if (niinij == ksom)
                            niinij++;
                          for (int comp = 0; comp < 3; comp++)
                            grad(face2, comp) += porosite_face(face2) * 1. / 15. * (signei * face_normales(facei, comp) + signej * face_normales(facej, comp)) * pa;
                        }
                      else
                        {
                          if (niinij == ksom)
                            niinij++;
                          while ((niinij == jsom) || (niinij == isom))
                            niinij++;
                          assert(niinij < 4);
                          int facek = elem_faces(elem, niinij);
                          double signek = 1.;
                          if (face_voisins(facek, 0) != elem)
                            signek = -1.;
                          for (int comp = 0; comp < 3; comp++)
                            grad(face2, comp) += porosite_face(face2) * 2. / 15. * signek * face_normales(facek, comp) * pa;
                          niinij = ksom;
                        }
                    }
                }
            }
        }
    }
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Conds_lim& les_cl = domaine_Cl_VEF.les_conditions_limites();
  const IntTab& face_sommets = domaine_VEF.face_sommets();
  int nb_bords = les_cl.size();
  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      {
        const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
        int num1 = le_bord.num_premiere_face();
        int num2 = num1 + le_bord.nb_faces();
        if (sub_type(Neumann_sortie_libre, la_cl.valeur()))
          {
            for (int face = num1; face < num2; face++)
              {
                int elem = face_voisins(face, 0);
                for (int i = 0; i < 2; i++)
                  {
                    int somi = face_sommets(face, i);
                    for (int j = i + 1; j < 3; j++)
                      {
                        int somj = face_sommets(face, j);
                        int arete = chercher_arete(elem, somi, somj, elem_aretes, aretes_som);
                        arete = renum_arete_perio[arete];
                        for (int comp = 0; comp < dimension; comp++)
                          grad(face, comp) -= porosite_face(face) * 0 * face_normales(face, comp) * pre(npa + arete);
                      }
                  }
              }
          }
      }
    }
  return grad;
}

double Op_Grad_VEF_P1B_Face::calculer_coef_som(int elem, const Domaine_Cl_VEF& zcl, const Domaine_VEF& domaine_VEF)
{
  double coef_std = 1. / (Objet_U::dimension * (Objet_U::dimension + 1));
  if (domaine_VEF.get_modif_div_face_dirichlet() == 0)
    return coef_std;
  int type_elem = 0;
  int rang_elem = domaine_VEF.rang_elem_non_std()(elem);
  if (rang_elem != -1)
    type_elem = zcl.type_elem_Cl(rang_elem);
  double coef;
  if (type_elem == 0)
    return coef_std;
  if (Objet_U::dimension == 2)
    {
      switch(type_elem)
        {
        case 0:
          coef = 3;
          break;
        case 1:
        case 2:
        case 4:
          coef = 2;
          break;
        case 3:
        case 5:
        case 6:
          coef = 1;
          break;
        default:
          coef = -1;
          Process::exit();
          break;
        }
    }
  else if (Objet_U::dimension == 3)
    {
      switch(type_elem)
        {
        case 0:
          coef = 4;
          break;
        case 1:
        case 2:
        case 4:
        case 8:
          coef = 3;
          break;
        case 3:
        case 5:
        case 6:
        case 9:
        case 10:
        case 12:
          coef = 2;
          break;
        case 7:
        case 11:
        case 13:
        case 14:
          coef = 1;
          break;
        default:
          coef = -1;
          Process::exit();
          break;
        }
    }
  else
    {
      coef = -1;
      Process::exit();
    }
  return 1. / (Objet_U::dimension * coef);
}

DoubleTab& Op_Grad_VEF_P1B_Face::ajouter(const DoubleTab& pre, DoubleTab& grad) const
{
  // pre doit avoir son espace virtuel a jour
  assert_espace_virtuel_vect(pre);
  // on va faire += sur l'espace virtuel, mais sans utiliser les valeurs
  assert_invalide_items_non_calcules(grad);
  //Debog::verifier("Op_Grad_VEF_P1B_Face::ajouter pre", pre);
  const Domaine_VEF& domaine_VEF = ref_cast(Domaine_VEF, le_dom_vef.valeur());
  static int init = 1;
  if (!init)
    verifier(*this, init, domaine_VEF, pre, grad);
  if (domaine_VEF.get_alphaE())
    ajouter_elem(pre, grad);
  if (domaine_VEF.get_alphaS())
    ajouter_som(pre, grad);
  if (domaine_VEF.get_alphaA())
    ajouter_aretes(pre, grad);
  modifier_grad_pour_Cl(grad);
  //calculer_flux_bords();
  //Optimisation car pas necessaire:
  //grad.echange_espace_virtuel();
  //Debog::verifier("Op_Grad_VEF_P1B_Face::ajouter grad en sortie:", grad);
  return grad;
}

void Op_Grad_VEF_P1B_Face::calculer_flux_bords() const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  if (flux_bords_.size_array() == 0)
    flux_bords_.resize(domaine_VEF.nb_faces_bord(), dimension);
  flux_bords_ = 0.;

  //int nse=domaine_VEF.domaine().nb_som_elem();
  int nb_faces_bord = domaine_VEF.premiere_face_int();
  int nps = domaine_VEF.numero_premier_sommet();
  const IntTab& sommets = domaine_VEF.face_sommets();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  //const IntTab& som_elem=le_dom_vef->domaine().les_elems();
  const DoubleTab& face_normales = domaine_VEF.face_normales();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std, equation());
  const Champ_P1_isoP1Bulle& la_pression_P1B = ref_cast(Champ_P1_isoP1Bulle, eqn_hydr.pression_pa());
  // Si on filtre:
  la_pression_P1B.filtrage(domaine_VEF, la_pression_P1B);
  const DoubleVect& pression_P1B = la_pression_P1B.champ_filtre();

  double coeff_P1 = 1. / dimension;
  bool alphaE = domaine_VEF.get_alphaE();
  bool alphaS = domaine_VEF.get_alphaS();
  int nb_som_par_face = sommets.dimension(1);
  CIntTabView face_voisins_v = face_voisins.view_ro();
  CIntTabView sommets_v = sommets.view_ro();
  CDoubleTabView face_normales_v = face_normales.view_ro();
  CDoubleArrView pression_P1B_v = pression_P1B.view_ro();
  DoubleTabView flux_bords_v = flux_bords_.view_wo();
  int dim = Objet_U::dimension;

  auto kern_flux_bords = KOKKOS_LAMBDA(int
                                       face)
  {
    int elem = face_voisins_v(face, 0);
    double pres_tot = 0.;
    // Contribution de la pression P0
    if (alphaE) pres_tot = pression_P1B_v(elem);
    // Contribution de la pression P1
    if (alphaS)
      {
        double pres_som = 0.;
        for (int som = 0; som < nb_som_par_face; som++)
          pres_som += pression_P1B_v(nps + sommets_v(face, som));
        pres_tot += coeff_P1 * pres_som;
      }
    // Calcul de la resultante et du couple de pression
    for (int i = 0; i < dim; i++)
      flux_bords_v(face, i) = pres_tot * face_normales_v(face, i);
  };

  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_faces_bord, kern_flux_bords);
  end_gpu_timer(__KERNEL_NAME__);
}

int Op_Grad_VEF_P1B_Face::impr(Sortie& os) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const int impr_mom = domaine_VEF.domaine().moments_a_imprimer();
  const int impr_sum = (domaine_VEF.domaine().bords_a_imprimer_sum().est_vide() ? 0 : 1);
  const int impr_bord = (domaine_VEF.domaine().bords_a_imprimer().est_vide() ? 0 : 1);
  const Schema_Temps_base& sch = equation().probleme().schema_temps();
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  DoubleTab xgr;
  if (impr_mom)
    xgr = domaine_VEF.calculer_xgr();

  DoubleTrav tab_flux_bords(3, domaine_VEF.nb_front_Cl(), dimension);
  tab_flux_bords = 0.;
  /*
   fluxx_s       ->   flux_bords2(0,num_cl,0)
   fluxy_s       ->   flux_bords2(0,num_cl,1)
   fluxz_s       ->   flux_bords2(0,num_cl,2)
   fluxx_sum_s   ->   flux_bords2(1,num_cl,0)
   fluxy_sum_s   ->   flux_bords2(1,num_cl,1)
   fluxz_sum_s   ->   flux_bords2(1,num_cl,2)
   moment(0)     ->   flux_bords2(2,num_cl,0)
   moment(1)     ->   flux_bords2(2,num_cl,1)
   moment(2)     ->   flux_bords2(2,num_cl,2)
   */
  int nb_bord = domaine_VEF.nb_front_Cl();
  for (int n_bord = 0; n_bord < nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
      int impr_boundary = (domaine_VEF.domaine().bords_a_imprimer_sum().contient(le_bord.le_nom()) ? 1 : 0);
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      for (int face = ndeb; face < nfin; face++)
        {
          // Calcul de la resultante et du couple de pression
          if (dimension == 2)
            {
              tab_flux_bords(0, n_bord, 0) += flux_bords_(face, 0);
              tab_flux_bords(0, n_bord, 1) += flux_bords_(face, 1);

              // Calcul du moment exerce par le fluide sur le bord (OM/\F)
              if (impr_mom)
                tab_flux_bords(2, n_bord, 0) += flux_bords_(face, 1) * xgr(face, 0) - flux_bords_(face, 0) * xgr(face, 1);
              if (impr_boundary)
                {
                  tab_flux_bords(1, n_bord, 0) += flux_bords_(face, 0);
                  tab_flux_bords(1, n_bord, 1) += flux_bords_(face, 1);
                }
            }
          else if (dimension == 3)
            {
              tab_flux_bords(0, n_bord, 0) += flux_bords_(face, 0);
              tab_flux_bords(0, n_bord, 1) += flux_bords_(face, 1);
              tab_flux_bords(0, n_bord, 2) += flux_bords_(face, 2);

              if (impr_mom)
                {
                  // Calcul du moment exerce par le fluide sur le bord (OM/\F)
                  tab_flux_bords(2, n_bord, 0) += flux_bords_(face, 2) * xgr(face, 1) - flux_bords_(face, 1) * xgr(face, 2);
                  tab_flux_bords(2, n_bord, 1) += flux_bords_(face, 0) * xgr(face, 2) - flux_bords_(face, 2) * xgr(face, 0);
                  tab_flux_bords(2, n_bord, 2) += flux_bords_(face, 1) * xgr(face, 0) - flux_bords_(face, 0) * xgr(face, 1);
                }
              if (impr_boundary)
                {
                  tab_flux_bords(1, n_bord, 0) += flux_bords_(face, 0);
                  tab_flux_bords(1, n_bord, 1) += flux_bords_(face, 1);
                  tab_flux_bords(1, n_bord, 2) += flux_bords_(face, 2);
                }
            }
        }
    } // fin for n_bord

  // On somme les contributions de chaque processeur
  mp_sum_for_each_item(tab_flux_bords);

  if (je_suis_maitre())
    {
      ouvrir_fichier(Flux_grad, "", 1);
      ouvrir_fichier(Flux_grad_moment, "moment", impr_mom);
      ouvrir_fichier(Flux_grad_sum, "sum", impr_sum);

      // Write time
      Flux_grad.add_col(sch.temps_courant());
      if (impr_sum)
        Flux_grad_sum.add_col(sch.temps_courant());
      if (impr_mom)
        Flux_grad_moment.add_col(sch.temps_courant());

      // Write flux on boundaries
      for (int n_bord = 0; n_bord < nb_bord; n_bord++)
        {
          if (dimension == 2)
            {
              Flux_grad.add_col(tab_flux_bords(0, n_bord, 0));
              Flux_grad.add_col(tab_flux_bords(0, n_bord, 1));
              if (impr_sum)
                {
                  Flux_grad_sum.add_col(tab_flux_bords(1, n_bord, 0));
                  Flux_grad_sum.add_col(tab_flux_bords(1, n_bord, 1));
                }
              if (impr_mom)
                Flux_grad_moment.add_col(tab_flux_bords(2, n_bord, 0));
            }
          else if (dimension == 3)
            {
              Flux_grad.add_col(tab_flux_bords(0, n_bord, 0));
              Flux_grad.add_col(tab_flux_bords(0, n_bord, 1));
              Flux_grad.add_col(tab_flux_bords(0, n_bord, 2));
              if (impr_sum)
                {
                  Flux_grad_sum.add_col(tab_flux_bords(1, n_bord, 0));
                  Flux_grad_sum.add_col(tab_flux_bords(1, n_bord, 1));
                  Flux_grad_sum.add_col(tab_flux_bords(1, n_bord, 2));
                }
              if (impr_mom)
                {
                  Flux_grad_moment.add_col(tab_flux_bords(2, n_bord, 0));
                  Flux_grad_moment.add_col(tab_flux_bords(2, n_bord, 1));
                  Flux_grad_moment.add_col(tab_flux_bords(2, n_bord, 2));
                }
            }
        }
      Flux_grad << finl;
      if (impr_sum)
        Flux_grad_sum << finl;
      if (impr_mom)
        Flux_grad_moment << finl;
    }

  const LIST(Nom) &Liste_bords_a_imprimer = domaine_VEF.domaine().bords_a_imprimer();
  if (!Liste_bords_a_imprimer.est_vide())
    {
      EcrFicPartage Flux_grad_face;
      ouvrir_fichier_partage(Flux_grad_face, "", impr_bord);
      for (int n_bord = 0; n_bord < nb_bord; n_bord++)
        {
          const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          if (domaine_VEF.domaine().bords_a_imprimer().contient(le_bord.le_nom()))
            {
              if (je_suis_maitre())
                {
                  Flux_grad_face << "# Force par face sur " << le_bord.le_nom() << " au temps ";
                  sch.imprimer_temps_courant(Flux_grad_face);
                  Flux_grad_face << " : " << finl;
                }
              for (int face = ndeb; face < nfin; face++)
                {
                  Flux_grad_face << "# Face a x= " << domaine_VEF.xv(face, 0) << " y= " << domaine_VEF.xv(face, 1);
                  if (dimension == 3)
                    Flux_grad_face << " z= " << domaine_VEF.xv(face, 2);
                  Flux_grad_face << " : Fx= " << flux_bords_(face, 0) << " Fy= " << flux_bords_(face, 1);
                  if (dimension == 3)
                    Flux_grad_face << " Fz= " << flux_bords_(face, 2);
                  Flux_grad_face << finl;
                }
              Flux_grad_face.syncfile();
            }
        }
    }
  return 1;
}
