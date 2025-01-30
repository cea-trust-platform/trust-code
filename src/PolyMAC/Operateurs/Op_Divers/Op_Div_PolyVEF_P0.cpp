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

#include <Assembleur_P_PolyVEF_P0.h>
#include <Champ_Face_PolyVEF_P0.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Op_Div_PolyVEF_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Matrice_Morse.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <SFichier.h>


Implemente_instanciable(Op_Div_PolyVEF_P0,"Op_Div_PolyVEF_P0",Operateur_Div_base);


//// printOn
//

Sortie& Op_Div_PolyVEF_P0::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Div_PolyVEF_P0::readOn(Entree& s)
{
  return s ;
}



/*! @brief
 *
 */
void Op_Div_PolyVEF_P0::associer(const Domaine_dis_base& dom_dis,
                                 const Domaine_Cl_dis_base& dcl_dis,
                                 const Champ_Inc_base&)
{
  le_dom_PolyVEF_P0 = ref_cast(Domaine_PolyVEF_P0, dom_dis);
  le_dcl_PolyVEF_P0 = ref_cast(Domaine_Cl_PolyMAC, dcl_dis);
}

void Op_Div_PolyVEF_P0::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_PolyVEF_P0& dom = le_dom_PolyVEF_P0.valeur();
  const Champ_Face_PolyVEF_P0& ch = ref_cast(Champ_Face_PolyVEF_P0, equation().inconnue());
  const DoubleTab& inco = ch.valeurs(), &press = ref_cast(Navier_Stokes_std, equation()).pression().valeurs();
  const IntTab& f_e = dom.face_voisins();
  int i, e, f, ne_tot = dom.nb_elem_tot(), d, D = dimension, n, N = inco.line_size() / D;

  Matrice_Morse *matv = matrices.count("vitesse") ? matrices["vitesse"] : nullptr, *matp = matrices.count("pression") ? matrices["pression"] : nullptr, matv2, matp2;
  IntTrav sten_v(0,2), sten_p(0, 2); //stencil des deux matrices

  for (f = 0; f < dom.nb_faces(); f++) /* sten_v : divergence "classique" */
    {
      for (i = 0; i < 2; i++)
        if ((e = f_e(f, i)) < dom.nb_elem()) //elements
          for (d = 0; d < D; d++)
            for (n = 0; n < N; n++)
              sten_v.append_line(N * (e >= 0 ? e : ne_tot + f) + n, N * (D * f + d) + n);
    }

  for (e = 0; e < dom.nb_elem(); e++) sten_p.append_line(e, e); /* sten_p : diagonale du vide */
  for (f = 0; f < dom.nb_faces_bord() ; f++) sten_p.append_line(ne_tot + f, ne_tot + f);

  if (matv) tableau_trier_retirer_doublons(sten_v), Matrix_tools::allocate_morse_matrix(press.size_totale(), inco.size_totale(), sten_v, matv2);
  if (matp) tableau_trier_retirer_doublons(sten_p), Matrix_tools::allocate_morse_matrix(press.size_totale(), press.size_totale(), sten_p, matp2);
  if (matv) matv->nb_colonnes() ? *matv += matv2 : *matv = matv2;
  if (matp) matp->nb_colonnes() ? *matp += matp2 : *matp = matp2;
}

void Op_Div_PolyVEF_P0::ajouter_blocs_ext(const DoubleTab& vit, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_PolyVEF_P0& dom = le_dom_PolyVEF_P0.valeur();
  const Champ_Face_PolyVEF_P0& ch = ref_cast(Champ_Face_PolyVEF_P0, equation().inconnue());
  const Conds_lim& cls = le_dcl_PolyVEF_P0->les_conditions_limites();
  const DoubleTab& nf = dom.face_normales();
  const IntTab& f_e = dom.face_voisins(), &fcl = ch.fcl();
  const DoubleVect& pf = equation().milieu().porosite_face();
  int i, e, f, ne_tot = dom.nb_elem_tot(), d, D = dimension, n, N = vit.line_size() / D, has_P_ref = 0, has_fb = secmem.dimension_tot(0) > ne_tot;
  Matrice_Morse *matv = matrices.count("vitesse") ? matrices["vitesse"] : nullptr, *matp = matrices.count("pression") ? matrices["pression"] : nullptr, matv2, matp2;

  for (i = 0; i < cls.size(); i++)
    if (sub_type(Neumann_sortie_libre,cls[i].valeur())) has_P_ref = 1;
  if (matp && !has_P_ref && Process::me() == Process::mp_min(dom.nb_elem() ? Process::me() : 1e8)) /* 1er proc possedant un element reel */
    (*matp)(0, 0) += 1;

  flux_bords_.resize(dom.nb_faces_bord(),N), flux_bords_ = 0;
  for (f = 0; f < dom.nb_faces(); f++)
    {
      /* contributions amont / aval */
      for (i = 0; i < 2; i++)
        if ((e = f_e(f, i)) < 0 && !has_fb) //faces de bord non demandees
          continue;
        else if (e < 0 && fcl(f, 0) == 1) //CL de Neumann -> p = p_imp
          for (n = 0; n < N; n++)
            {
              secmem(ne_tot + f, n) += ref_cast(Neumann, cls[fcl(f, 1)].valeur()).flux_impose(fcl(f, 2), n);
              if (matv) (*matv)(N * (ne_tot + f) + n, N * (ne_tot + f) + n)--;
            }
        else if (e < dom.nb_elem()) //elem reel ou bord non Neumann
          for (d = 0; d < D; d++)
            for (n = 0; n < N; n++)
              {
                secmem(e >= 0 ? e : ne_tot + f, n) += (i ? -1 : 1) * pf(f) * nf(f, d) * vit(f, N * d + n);
                if (matv) (*matv)(N * (e >= 0 ? e : ne_tot + f) + n, N * (D * f + d) + n) -= (i ? -1 : 1) * pf(f) * nf(f, d);
              }
      /* aux bords : flux_bords_ + contrib des CLs de Dirichlet a la divergence */
      if (f < dom.premiere_face_int())
        for (d = 0; d < D; d++)
          for (n = 0; n < N; n++)
            flux_bords_(f, n) += nf(f, d) * pf(f) * vit(f, N * d + n);
      if (has_fb && fcl(f, 0) == 3)
        for (d = 0; d < D; d++)
          for (n = 0; n < N; n++)
            secmem(ne_tot + f, n) += nf(f, d) * pf(f) * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + n);
    }
}

DoubleTab& Op_Div_PolyVEF_P0::ajouter(const DoubleTab& vit, DoubleTab& div) const
{
  ajouter_blocs_ext(equation().inconnue().valeurs(), {}, div);
  return div;
}

DoubleTab& Op_Div_PolyVEF_P0::calculer(const DoubleTab& vit, DoubleTab& div) const
{
  div = 0;
  return ajouter(vit,div);

}

int Op_Div_PolyVEF_P0::impr(Sortie& os) const
{

  const int impr_bord=(le_dom_PolyVEF_P0->domaine().bords_a_imprimer().est_vide() ? 0:1);
  ouvrir_fichier(Flux_div,"",je_suis_maitre());
  EcrFicPartage Flux_face;
  ouvrir_fichier_partage(Flux_face,"",impr_bord);
  const Schema_Temps_base& sch = equation().probleme().schema_temps();
  double temps = sch.temps_courant();
  if (je_suis_maitre()) Flux_div.add_col(temps);

  int nb_compo=flux_bords_.dimension(1);
  // On parcours les frontieres pour sommer les flux par frontiere dans le tableau flux_bord
  DoubleVect flux_bord(nb_compo);
  DoubleVect bilan(nb_compo);
  bilan = 0;
  for (int num_cl=0; num_cl<le_dom_PolyVEF_P0->nb_front_Cl(); num_cl++)
    {
      flux_bord=0;
      const Cond_lim& la_cl = le_dcl_PolyVEF_P0->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl->frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      for (int face=ndeb; face<nfin; face++)
        for(int k=0; k<nb_compo; k++)
          flux_bord(k)+=flux_bords_(face, k);
      for(int k=0; k<nb_compo; k++)
        flux_bord(k)=Process::mp_sum(flux_bord(k));

      if(je_suis_maitre())
        {
          for(int k=0; k<nb_compo; k++)
            {
              //Ajout pour impression sur fichiers separes
              Flux_div.add_col(flux_bord(k));
              bilan(k)+=flux_bord(k);
            }
        }
    }

  if(je_suis_maitre())
    {
      for(int k=0; k<nb_compo; k++)
        {
          Flux_div.add_col(bilan(k));
        }
      Flux_div << finl;
    }

  for (int num_cl=0; num_cl<le_dom_PolyVEF_P0->nb_front_Cl(); num_cl++)
    {
      const Frontiere_dis_base& la_fr = le_dcl_PolyVEF_P0->les_conditions_limites(num_cl)->frontiere_dis();
      const Cond_lim& la_cl = le_dcl_PolyVEF_P0->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl->frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      if (le_dom_PolyVEF_P0->domaine().bords_a_imprimer().contient(la_fr.le_nom()))
        {
          Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps " << temps << " : " << finl;
          for (int face=ndeb; face<nfin; face++)
            {
              if (dimension==2)
                Flux_face << "# Face a x= " << le_dom_PolyVEF_P0->xv(face,0) << " y= " << le_dom_PolyVEF_P0->xv(face,1) << " flux=" ;
              else if (dimension==3)
                Flux_face << "# Face a x= " << le_dom_PolyVEF_P0->xv(face,0) << " y= " << le_dom_PolyVEF_P0->xv(face,1) << " z= " << le_dom_PolyVEF_P0->xv(face,2) << " flux=" ;
              for(int k=0; k<nb_compo; k++)
                Flux_face << " " << flux_bords_(face, k);
              Flux_face << finl;
            }
          Flux_face.syncfile();
        }
    }

  return 1;
}


void Op_Div_PolyVEF_P0::volumique(DoubleTab& div) const
{
  const Domaine_PolyVEF_P0& dom = le_dom_PolyVEF_P0.valeur();
  const DoubleVect& vol = dom.volumes();
  int nb_elem=dom.nb_elem_tot();
  int num_elem;

  for(num_elem=0; num_elem<nb_elem; num_elem++)
    div(num_elem)/=vol(num_elem);
}
