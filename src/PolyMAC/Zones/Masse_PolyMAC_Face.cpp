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
// File:        Masse_PolyMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Zones
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Masse_PolyMAC_Face.h>
#include <Zone_Cl_PolyMAC.h>
#include <Zone_PolyMAC.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Debog.h>
#include <Equation_base.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Champ_Face_PolyMAC.h>
#include <Operateur.h>
#include <Op_Diff_negligeable.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <ConstDoubleTab_parts.h>

Implemente_instanciable(Masse_PolyMAC_Face,"Masse_PolyMAC_Face",Solveur_Masse_base);


//     printOn()
/////

Sortie& Masse_PolyMAC_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Masse_PolyMAC_Face::readOn(Entree& s)
{
  return s ;
}


///////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Masse_PolyMAC_Face
//
//////////////////////////////////////////////////////////////


DoubleTab& Masse_PolyMAC_Face::appliquer_impl(DoubleTab& sm) const
{
  //hors faces de bord, on ne fait rien et on passe secmem a corriger_derivee_* (car PolyMAC a une matrice de masse)
  assert(la_zone_PolyMAC.non_nul());
  assert(la_zone_Cl_PolyMAC.non_nul());
  const Zone_PolyMAC& zone_PolyMAC = la_zone_PolyMAC.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  ch.init_cl();

  assert(sm.nb_dim() <= 2); // sinon on ne fait pas ce qu'il faut
  int nb_faces_tot = zone_PolyMAC.nb_faces_tot();
  int nb_aretes_tot = (dimension < 3 ? zone_PolyMAC.nb_som_tot() : zone_PolyMAC.zone().nb_aretes_tot()), nc = sm.nb_dim() == 2 ? sm.dimension(1) : 1;

  if (sm.dimension_tot(0) != nb_faces_tot && sm.dimension_tot(0) != nb_faces_tot + nb_aretes_tot)
    {
      Cerr << "Masse_PolyMAC_Face::appliquer :  erreur dans la taille de sm" << finl;
      exit();
    }

  //mise a zero de la partie vitesse de sm sur les faces a vitesse imposee
  for (int f = 0; f < zone_PolyMAC.nb_faces(); f++) if (ch.icl(f, 0) > 1)
      for (int k = 0; k < nc; k++) sm.addr()[nc * f + k]= 0;

  return sm;
}

//
void Masse_PolyMAC_Face::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  la_zone_PolyMAC = ref_cast(Zone_PolyMAC, la_zone_dis_base);
}

void Masse_PolyMAC_Face::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_zone_Cl_PolyMAC = ref_cast(Zone_Cl_PolyMAC, la_zone_Cl_dis_base);
}

void Masse_PolyMAC_Face::completer()
{
  no_diff_ = true;
  for(int i = 0; i < equation().nombre_d_operateurs(); i++)
    if (sub_type(Operateur_Diff_base, equation().operateur(i).l_op_base()))
      if (!sub_type(Op_Diff_negligeable, equation().operateur(i).l_op_base())) no_diff_ = false;
}

void Masse_PolyMAC_Face::dimensionner(Matrice_Morse& matrix) const
{
  const Zone_PolyMAC& zone = la_zone_PolyMAC;
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  int i, a, f, fb, nf_tot = zone.nb_faces_tot(), na_tot = dimension < 3 ? zone.zone().nb_som_tot() : zone.zone().nb_aretes_tot();
  const bool only_m2 = (matrix.nb_lignes() == nf_tot);

  zone.init_m1(), zone.init_m2(), ch.init_ra();
  IntTab indice(0,2);
  indice.set_smart_resize(1);
  //partie vitesses : matrice de masse des vitesses si la face n'est pas a vitesse imposee, diagonale sinon
  for (f = 0; f < zone.nb_faces(); f++)
    if (ch.icl(f, 0) > 1) indice.append_line(f, f);
    else for (i = zone.m2deb(f); i < zone.m2deb(f + 1); i++) if (ch.icl(fb = zone.m2ji(i, 0), 0) < 2) indice.append_line(f, fb);


  //partie vorticites : diagonale si pas de diffusion
  if (!only_m2) for (a = 0; no_diff_ && a < (dimension < 3 ? zone.nb_som() : zone.zone().nb_aretes()); a++)
      indice.append_line(nf_tot + a, nf_tot + a);

  tableau_trier_retirer_doublons(indice);
  Matrix_tools::allocate_morse_matrix(nf_tot + !only_m2 * na_tot, nf_tot + !only_m2 * na_tot, indice, matrix);
}

DoubleTab& Masse_PolyMAC_Face::ajouter_masse(double dt, DoubleTab& secmem, const DoubleTab& inco, int penalisation) const
{
  const Zone_PolyMAC& zone = la_zone_PolyMAC;
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zone_Cl_PolyMAC->les_conditions_limites();
  const DoubleTab& nf = zone.face_normales();
  const DoubleVect& fs = zone.face_surfaces(), &pf = zone.porosite_face();
  DoubleVect coef(zone.porosite_face());
  coef = 1.;
  int i, k, f, fb;

  if (has_coefficient_temporel_) appliquer_coef(coef);
  zone.init_m1(), zone.init_m2(), ch.init_ra();

  //partie vitesses : contribution de m2 / dt
  for (f = 0; f < zone.nb_faces(); f++)
    if (ch.icl(f, 0) == 3) for (k = 0, secmem(f) = 0; k < dimension; k++)//valeur imposee par une CL de type Dirichlet
        secmem(f) += nf(f, k) * ref_cast(Dirichlet, cls[ch.icl(f, 1)].valeur()).val_imp(ch.icl(f, 2), k) / fs(f);
    else if (ch.icl(f, 0) > 1) secmem(f) = 0; //Dirichlet homogene ou Symetrie
    else for (i = zone.m2deb(f); i < zone.m2deb(f + 1); i++)
        if (ch.icl(fb = zone.m2ji(i, 0), 0) < 2) //vf calculee
          secmem(f) += zone.m2ci(i) * pf(fb) * coef(f) * inco(fb) / dt;
        else if (ch.icl(fb, 0) == 3) for (k = 0; k < dimension; k++) //Dirichlet
            secmem(f) += zone.m2ci(i) * pf(fb) * coef(f) * ref_cast(Dirichlet, cls[ch.icl(fb, 1)].valeur()).val_imp(ch.icl(fb, 2), k) * nf(fb, k) / (fs(fb) * dt);

  return secmem;
}

Matrice_Base& Masse_PolyMAC_Face::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  const Zone_PolyMAC& zone = la_zone_PolyMAC;
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const DoubleVect& pf = zone.porosite_face();
  DoubleVect coef(zone.porosite_face());
  coef = 1.;
  int i, a, f, fb, nf_tot = zone.nb_faces_tot();
  Matrice_Morse& mat = ref_cast(Matrice_Morse, matrice);

  if (has_coefficient_temporel_) appliquer_coef(coef);
  zone.init_m1(), zone.init_m2(), ch.init_ra();

  //partie vitesses : contribution de m2 / dt
  for (f = 0; f < zone.nb_faces(); f++) //vf imposee par CL
    if (ch.icl(f, 0) > 1) mat(f, f) = 1;
    else for (i = zone.m2deb(f); i < zone.m2deb(f + 1); i++) if (ch.icl(fb = zone.m2ji(i, 0), 0) < 2) //v(fb) calculee
          mat(f, fb) += zone.m2ci(i) * coef(f) * pf(fb) / dt;

  //partie vorticites : diagonale si Op_Diff_negligeable
  if (mat.nb_lignes() > nf_tot) for (a = 0; no_diff_ && a < (dimension < 3 ? zone.nb_som() : zone.zone().nb_aretes()); a++)
      mat(nf_tot + a, nf_tot + a) = 1;

  return matrice;
}

void Masse_PolyMAC_Face::appliquer_coef(DoubleVect& coef) const
{
  if (has_coefficient_temporel_)
    {
      REF(Champ_base) ref_coeff;
      ref_coeff = equation().get_champ(name_of_coefficient_temporel_);

      DoubleTab values;
      if (sub_type(Champ_Inc_base,ref_coeff.valeur()))
        {
          const Champ_Inc_base& coeff = ref_cast(Champ_Inc_base,ref_coeff.valeur());
          ConstDoubleTab_parts val_parts(coeff.valeurs());
          values.ref(val_parts[0]);

        }
      else if (sub_type(Champ_Fonc_base,ref_coeff.valeur()))
        {
          const Champ_Fonc_base& coeff = ref_cast(Champ_Fonc_base,ref_coeff.valeur());
          values.ref(coeff.valeurs());

        }
      else if (sub_type(Champ_Don_base,ref_coeff.valeur()))
        {
          DoubleTab nodes;
          equation().inconnue().valeur().remplir_coord_noeuds(nodes);
          ref_coeff.valeur().valeur_aux(nodes,values);
        }
      tab_multiply_any_shape(coef, values, VECT_REAL_ITEMS);
    }
}
