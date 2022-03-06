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
// File:        Op_Div_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Div_PolyMAC.h>
#include <Zone_Cl_PolyMAC.h>
#include <Champ_Face_PolyMAC.h>
//#include <Les_Cl.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Check_espace_virtuel.h>
#include <EcrFicPartage.h>
#include <SFichier.h>
#include <Matrice_Morse.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Debog.h>

Implemente_instanciable(Op_Div_PolyMAC,"Op_Div_PolyMAC",Operateur_Div_base);


//// printOn
//

Sortie& Op_Div_PolyMAC::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Div_PolyMAC::readOn(Entree& s)
{
  return s ;
}



// Description:
void Op_Div_PolyMAC::associer(const Zone_dis& zone_dis,
                              const Zone_Cl_dis& zone_Cl_dis,
                              const Champ_Inc&)
{
  const Zone_PolyMAC& zPolyMAC = ref_cast(Zone_PolyMAC, zone_dis.valeur());
  const Zone_Cl_PolyMAC& zclPolyMAC = ref_cast(Zone_Cl_PolyMAC, zone_Cl_dis.valeur());
  la_zone_PolyMAC = zPolyMAC;
  la_zcl_PolyMAC = zclPolyMAC;
}

void Op_Div_PolyMAC::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Zone_PolyMAC& zone = la_zone_PolyMAC.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const DoubleTab& inco = ch.valeurs(), &press = ref_cast(Navier_Stokes_std, equation()).pression().valeurs();
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins(), &fcl = ch.fcl();
  int i, j, e, f, ne_tot = zone.nb_elem_tot();

  Matrice_Morse *matv = matrices.count("vitesse") ? matrices["vitesse"] : NULL, *matp = matrices.count("pression") ? matrices["pression"] : NULL, matv2, matp2;
  IntTab sten_v(0,2), sten_p(0, 2);
  DoubleTab w2; //matrice w2 aux elements (la meme que dans Op_Grad et Assembleur_P)
  sten_v.set_smart_resize(1), sten_p.set_smart_resize(1), w2.set_smart_resize(1);

  for (f = 0; matv && f < zone.nb_faces(); f++) /* dependance en v : divergence par elem + v = v_imp aux faces de Dirichlet */
    if (fcl(f, 0) > 1) sten_v.append_line(ne_tot + f, f); /* v impose par CLs : contribution a l'equation v = v_imp a la paroi */
    else for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) sten_v.append_line(e, f); /* v calcule : contribution a la divergence aux elems */

  for (e = 0; matp && e < zone.nb_elem_tot(); e++) for (zone.W2(NULL, e, w2), i = 0; i < w2.dimension(0); i++) /* dependance en p : equation sur p_f */
      if (fcl(f = e_f(e, i), 0) == 1) sten_p.append_line(ne_tot + f, ne_tot + f); /* aux faces de Neumann : p_f = p_imp */
      else if (!fcl(f, 0)) for (sten_p.append_line(ne_tot + f, e), j = 0; j < w2.dimension(1); j++) /* aux faces internes : egalite des deux gradients */
          if (w2(i, j, 0)) sten_p.append_line(ne_tot + f, ne_tot + e_f(e, j));
  if (matp) for (e = 0; e < zone.nb_elem(); e++) sten_p.append_line(e, e); //diagonale du vide!

  if (matv) tableau_trier_retirer_doublons(sten_v), Matrix_tools::allocate_morse_matrix(press.size_totale(), inco.size_totale(), sten_v, matv2);
  if (matp) tableau_trier_retirer_doublons(sten_p), Matrix_tools::allocate_morse_matrix(press.size_totale(), press.size_totale(), sten_p, matp2);
  if (matv) matv->nb_colonnes() ? *matv += matv2 : *matv = matv2;
  if (matp) matp->nb_colonnes() ? *matp += matp2 : *matp = matp2;
}

void Op_Div_PolyMAC::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Zone_PolyMAC& zone = la_zone_PolyMAC.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zcl_PolyMAC->les_conditions_limites();
  const DoubleTab& inco = ch.valeurs(), &press = ref_cast(Navier_Stokes_std, equation()).pression().valeurs(), &nf = zone.face_normales();
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins(), &fcl = ch.fcl();
  const DoubleVect& fs = zone.face_surfaces(), &pf = zone.porosite_face();
  int i, j, e, f, fb, ne_tot = zone.nb_elem_tot(), d, D = dimension;
  Matrice_Morse *matv = matrices.count("vitesse") ? matrices["vitesse"] : NULL, *matp = matrices.count("pression") ? matrices["pression"] : NULL, matv2, matp2;

  DoubleTrav w2; //matrice w2 aux elements (la meme que dans Op_Grad et Assembleur_P)
  w2.set_smart_resize(1);

  for (f = 0; f < zone.nb_faces(); f++) /* divergence aux elements + equations aux bords */
    {
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) /* divergence aux elems */
          {
            secmem(e) -= (i ? 1 : -1) * fs(f) * pf(f) * inco(f);
            if (fcl(f, 0) < 2 && matv) (*matv)(e, f) += (i ? 1 : -1) * fs(f) * pf(f);
          }
      /* equations v = v_imp ou p = p_imp aux faces de bord */
      if (fcl(f, 0) > 1)
        {
          if (fcl(f, 0) == 3) for (d = 0; d < D; d++) secmem(ne_tot + f) += nf(f, d) * pf(f) * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), d);
          secmem(ne_tot + f) -= fs(f) * pf(f) * inco(f);
          if (matv) (*matv)(ne_tot + f, f) += fs(f) * pf(f);
        }
      else if (fcl(f, 0) == 1)
        {
          secmem(ne_tot + f) += ref_cast(Neumann, cls[fcl(f, 1)].valeur()).flux_impose(fcl(f, 2), 0) - press(ne_tot + f);
          if (matp) (*matp)(ne_tot + f, ne_tot + f) += 1;
        }
    }

  /* equations aux faces internes : egalite des gradients */
  for (e = 0; e < zone.nb_elem_tot(); e++) for (zone.W2(NULL, e, w2), i = 0; i < w2.dimension(0); i++) if ((f = e_f(e, i)) < zone.nb_faces() && !fcl(f, 0))
        {
          double coeff_e = 0;
          for (j = 0; j < w2.dimension(1); j++) if (w2(i, j, 0))
              {
                fb = e_f(e, j);
                secmem(ne_tot + f) -= pf(f) * w2(i, j, 0) * (press(ne_tot + fb) - press(e));
                if (matp) (*matp)(ne_tot + f, ne_tot + fb) += pf(f) * w2(i, j, 0), coeff_e += pf(f) * w2(i, j, 0);
              }
          if (matp) (*matp)(ne_tot + f, e) -= coeff_e;
        }
}

DoubleTab& Op_Div_PolyMAC::ajouter(const DoubleTab& vit, DoubleTab& div) const
{
  const Zone_PolyMAC& zone = la_zone_PolyMAC.valeur();
  const DoubleVect& fs = zone.face_surfaces(), &pf = zone.porosite_face();
  const DoubleTab& nf = zone.face_normales();
  const Conds_lim& cls = la_zcl_PolyMAC->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins(), &fcl = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur()).fcl();
  int i, e, f, ne_tot = zone.nb_elem_tot(), d, D = dimension, has_f = div.dimension_tot(0) > ne_tot;

  DoubleTab& tab_flux_bords = flux_bords_;
  tab_flux_bords.resize(zone.nb_faces_bord(),1);
  tab_flux_bords=0;

  for (f = 0; f < zone.nb_faces(); f++)
    {
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) /* aux elements */
          div(e) += (i ? -1 : 1) * fs(f) * pf(f) * vit(f);
      if (f >= zone.premiere_face_int()) continue;
      /* si "div" a des valeurs aux faces : bilan de masse avec la CL imposee aux faces de bord de Dirichlet */
      if (has_f && fcl(f, 0) != 1) div(ne_tot + f) -= fs(f) * pf(f) * vit(f);
      if (has_f && fcl(f, 0) == 3) for (d = 0; d < D; d++) div(ne_tot + f) += nf(f, d) * pf(f) * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), d);
      /* a toutes les faces de bord : contribution a tab_flux_bords */
      tab_flux_bords(f) = fs(f) * pf(f) * vit(f); //flux aux bords
    }

  div.echange_espace_virtuel();

  return div;
}

DoubleTab& Op_Div_PolyMAC::calculer(const DoubleTab& vit, DoubleTab& div) const
{
  div = 0;
  return ajouter(vit,div);

}

int Op_Div_PolyMAC::impr(Sortie& os) const
{

  const int impr_bord=(la_zone_PolyMAC->zone().Bords_a_imprimer().est_vide() ? 0:1);
  SFichier Flux_div;
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
  for (int num_cl=0; num_cl<la_zone_PolyMAC->nb_front_Cl(); num_cl++)
    {
      flux_bord=0;
      const Cond_lim& la_cl = la_zcl_PolyMAC->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
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

  for (int num_cl=0; num_cl<la_zone_PolyMAC->nb_front_Cl(); num_cl++)
    {
      const Frontiere_dis_base& la_fr = la_zcl_PolyMAC->les_conditions_limites(num_cl).frontiere_dis();
      const Cond_lim& la_cl = la_zcl_PolyMAC->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      if (la_zone_PolyMAC->zone().Bords_a_imprimer().contient(la_fr.le_nom()))
        {
          Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps " << temps << " : " << finl;
          for (int face=ndeb; face<nfin; face++)
            {
              if (dimension==2)
                Flux_face << "# Face a x= " << la_zone_PolyMAC->xv(face,0) << " y= " << la_zone_PolyMAC->xv(face,1) << " flux=" ;
              else if (dimension==3)
                Flux_face << "# Face a x= " << la_zone_PolyMAC->xv(face,0) << " y= " << la_zone_PolyMAC->xv(face,1) << " z= " << la_zone_PolyMAC->xv(face,2) << " flux=" ;
              for(int k=0; k<nb_compo; k++)
                Flux_face << " " << flux_bords_(face, k);
              Flux_face << finl;
            }
          Flux_face.syncfile();
        }
    }

  return 1;
}


void Op_Div_PolyMAC::volumique(DoubleTab& div) const
{
  const Zone_PolyMAC& zone_PolyMAC = la_zone_PolyMAC.valeur();
  const DoubleVect& vol = zone_PolyMAC.volumes();
  int nb_elem=zone_PolyMAC.zone().nb_elem_tot();
  int num_elem;

  for(num_elem=0; num_elem<nb_elem; num_elem++)
    div(num_elem)/=vol(num_elem);
}
