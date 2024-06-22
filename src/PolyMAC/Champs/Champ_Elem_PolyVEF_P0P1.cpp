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

#include <Champ_Elem_PolyVEF_P0P1.h>
#include <Connectivite_som_elem.h>
#include <Domaine_PolyVEF.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Champ_Face_PolyVEF.h>
#include <Assembleur_P_PolyVEF_P0P1.h>
#include <Navier_Stokes_std.h>

Implemente_instanciable(Champ_Elem_PolyVEF_P0P1,"Champ_Elem_PolyVEF_P0P1",Champ_Elem_PolyMAC_P0);

Sortie& Champ_Elem_PolyVEF_P0P1::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Elem_PolyVEF_P0P1::readOn(Entree& s) { return Champ_Elem_PolyMAC_P0::readOn(s) ; }

const Domaine_PolyVEF& Champ_Elem_PolyVEF_P0P1::domaine_PolyVEF() const
{
  return ref_cast(Domaine_PolyVEF, le_dom_VF.valeur());
}

void Champ_Elem_PolyVEF_P0P1::init_auxiliary_variables()
{
  const Domaine_PolyVEF_P0P1& dom = ref_cast(Domaine_PolyVEF_P0P1,le_dom_VF.valeur());
  const Static_Int_Lists& s_e = dom.som_elem();
  for (int n = 0; n < nb_valeurs_temporelles(); n++)
    if (futur(n).size_reelle_ok())
      {
        DoubleTab& vals = futur(n);
        vals.set_md_vector(MD_Vector()); //on enleve le MD_Vector...
        vals.resize_dim0(dom.mdv_elems_soms.valeur().get_nb_items_tot()); //...on dimensionne a la bonne taille...
        vals.set_md_vector(dom.mdv_elems_soms); //...et on remet le bon MD_Vector
        /* initialisation des variables aux sommets : moyennes de celles aux elems */
        for (int s = 0, ne_tot = dom.nb_elem_tot(); s < dom.nb_som(); s++)
          for (int i = 0, ns = s_e.get_list_size(s); i < ns; i++)
            for (int e = s_e(s, i), m = 0; m < vals.dimension(1); m++)
              vals(ne_tot + s, m) += vals(e, m) / ns;
        vals.echange_espace_virtuel();
      }
}

const DoubleTab& Champ_Elem_PolyVEF_P0P1::alpha_es() const
{
  if (alpha_es_.nb_dim() > 1) return alpha_es_;
  const Domaine_PolyMAC& dom = ref_cast(Domaine_PolyVEF_P0P1,le_dom_VF.valeur());
  const IntTab& e_s = dom.domaine().les_elems(), &scld = scl_d(1), &ps_ref = ref_cast(Assembleur_P_PolyVEF_P0P1, ref_cast(Navier_Stokes_std, equation()).assembleur_pression().valeur()).ps_ref();;
  const DoubleTab& xp = dom.xp(), &xs = dom.domaine().coord_sommets();
  int e, s, i, j, D = dimension, nl = D + 1, nc, nm, un = 1, infoo = 0, rank, nw;
  double eps = 1e-8;
  std::vector<int> v_s;//sommets qu'on peut utiliser
  DoubleTrav A, B, W(1);
  IntTrav pvt;
  alpha_es_.resize(e_s.dimension_tot(0), e_s.dimension_tot(1)), alpha_es_ = 0;

  for (e = 0; e < dom.nb_elem_tot(); e++)
    {
      for (v_s.clear(), i = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++)
        if (ps_ref(s) < 0 || scld(s) < scld(s + 1))
          v_s.push_back(s);
      nc = (int) v_s.size(), nm = std::max(nc, nl), A.resize(nc, nl), B.resize(nm), pvt.resize(nm);
      if (nc < D + 1) //pas assez de points pour une interpolation lineaire -> on fait comme on peut...
        {
          for (i = 0, j = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++)
            if (ps_ref(s) < 0 || scld(s) < scld(s + 1))
              j++;
          for (i = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++)
            if (ps_ref(s) < 0 || scld(s) < scld(s + 1))
              alpha_es_(e, i) = 1. / j;
          continue;
        }

      /* systeme lineaire : interpolation exacte sur les fonctions affines */
      for (A = 0, B = 0, B(D) = 1, i = 0; i < nc; i++)
        for (s = v_s[i], j = 0; j < nl; j++)
          A(i, j) = j < D ? xs(s, j) - xp(e, j) : 1;
      /* resolution */
      nw = -1, pvt = 0,                     F77NAME(dgelsy)(&nl, &nc, &un, &A(0, 0), &nl, &B(0), &nc, &pvt(0), &eps, &rank, &W(0), &nw, &infoo);
      W.resize(nw = (int)std::lrint(W(0))), F77NAME(dgelsy)(&nl, &nc, &un, &A(0, 0), &nl, &B(0), &nc, &pvt(0), &eps, &rank, &W(0), &nw, &infoo);
      /* stockage */
      for (i = 0, j = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++)
        if (ps_ref(s) < 0 || scld(s) < scld(s + 1))
          alpha_es_(e, i) = B(j), j++;
    }
  return alpha_es_;
}

DoubleTab& Champ_Elem_PolyVEF_P0P1::valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const
{
  const Domaine_PolyMAC& dom = ref_cast(Domaine_PolyVEF_P0P1,le_dom_VF.valeur());
  if (!sub_type(Navier_Stokes_std, equation()) || (this != &ref_cast(Navier_Stokes_std, equation()).pression() && this != &ref_cast(Navier_Stokes_std, equation()).pression_pa()) || valeurs().dimension_tot(0) == dom.nb_elem_tot())
    return Champ_Elem_PolyMAC_P0::valeur_aux_elems(positions, polys, result); //ne s'applique qu'a la pression
  const IntTab& e_s = dom.domaine().les_elems();
  const DoubleTab& vals = valeurs(), &a_es = alpha_es();
  int e, s, i, j, n, N = valeurs().line_size(), ne_tot = dom.nb_elem_tot();
  for (i = 0; i < polys.size(); i++)
    if ((e = polys(i)) >= 0 && e < ne_tot)
    {
      for (n = 0; n < N; n++) result(i, n) = 0.5 * vals(e, n);
      for (j = 0; j < e_s.dimension(1) && (s = e_s(e, j)) >= 0; j++)
        for (n = 0; n < N; n++)
          result(i, n) += 0.5 * a_es(e, j) * vals(ne_tot + s, n);
    }
  return result;
}


DoubleTab& Champ_Elem_PolyVEF_P0P1::valeur_aux_sommets(const Domaine& domain, DoubleTab& result) const
{
  if (&domain == &le_dom_VF->domaine() && valeurs().dimension_tot(0) > le_dom_VF->nb_elem_tot())
    {
      ConstDoubleTab_parts part(valeurs());
      result = part[1];
      return result;
    }
  else return Champ_Elem_PolyMAC_P0::valeur_aux_sommets(domain, result);
}

DoubleVect& Champ_Elem_PolyVEF_P0P1::valeur_aux_sommets_compo(const Domaine& domain, DoubleVect& result, int ncomp) const
{
  if (domain == le_dom_VF->domaine() && valeurs().dimension_tot(0) > le_dom_VF->nb_elem_tot())
    {
      ConstDoubleTab_parts part(valeurs());
      for (int i = 0; i < std::min(result.size_totale(), part[1].dimension_tot(0)); i++)
        result(i) = part[1](i, ncomp);
      return result;
    }
  else return Champ_Elem_PolyMAC_P0::valeur_aux_sommets_compo(domain, result, ncomp);
}
