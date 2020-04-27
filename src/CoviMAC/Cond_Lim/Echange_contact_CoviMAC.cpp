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
// File:        Echange_contact_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Cond_Lim
// Version:     /main/32
//
//////////////////////////////////////////////////////////////////////////////

#include <Echange_contact_CoviMAC.h>
#include <Champ_front_calc.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>
#include <Schema_Euler_Implicite.h>
#include <Milieu_base.h>

#include <Zone_CoviMAC.h>
#include <Equation_base.h>
#include <Champ_P0_CoviMAC.h>
#include <Operateur.h>
#include <Op_Diff_CoviMAC_Elem.h>

#include <cmath>

Implemente_instanciable(Echange_contact_CoviMAC,"Paroi_Echange_contact_CoviMAC",Echange_externe_impose);

int meme_point2(const DoubleVect& a,const DoubleVect& b);


Sortie& Echange_contact_CoviMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Echange_contact_CoviMAC::readOn(Entree& s )
{
  Nom nom_bord;
  Motcle nom_champ;
  s >> nom_autre_pb_ >> nom_bord >> nom_champ >> h_paroi;
  T_autre_pb().typer("Champ_front_calc");
  Champ_front_calc& ch=ref_cast(Champ_front_calc, T_autre_pb().valeur());
  ch.creer(nom_autre_pb_, nom_bord, nom_champ);
  T_ext().typer("Ch_front_var_instationnaire_dep");
  T_ext()->fixer_nb_comp(1);
  return s ;
}

void Echange_contact_CoviMAC::completer()
{
  Echange_externe_impose::completer();
  T_autre_pb().associer_fr_dis_base(T_ext().frontiere_dis());
  T_autre_pb()->completer();
}

int Echange_contact_CoviMAC::initialiser(double temps)
{
  if (!Echange_externe_impose::initialiser(temps))
    return 0;

  Champ_front_calc& ch=ref_cast(Champ_front_calc, T_autre_pb().valeur());
  const Equation_base&    o_eqn  = ch.equation();
  const Front_VF& fvf = ref_cast(Front_VF, frontiere_dis()), o_fvf = ref_cast(Front_VF, ch.front_dis());
  const Zone_CoviMAC& o_zone = ref_cast(Zone_CoviMAC, ch.zone_dis());
  const IntTab& o_f_e = o_zone.face_voisins(), &o_e_f = o_zone.elem_faces();
  int i, j, k, l, e, f, N = ch.nb_comp(), o_n_f = o_e_f.dimension(1);
  Nom nom_racc1=frontiere_dis().frontiere().le_nom();

  h_imp_.typer("Champ_front_fonc");
  h_imp_->fixer_nb_comp(N);
  h_imp_.valeurs().resize(0, N);
  fvf.frontiere().creer_tableau_faces(h_imp_.valeurs());

  ch.initialiser(temps,zone_Cl_dis().equation().inconnue());

  monolithic = sub_type(Schema_Euler_Implicite, o_eqn.schema_temps()) ?
               ref_cast(Schema_Euler_Implicite, o_eqn.schema_temps()).thermique_monolithique() : 0;
  if (!monolithic) return 1; //pas besoin du reste
  o_zone.init_m2(), o_zone.init_virt_ef_map();

  /* src(i) = (proc, j) : source de l'item i de mdv_elem_faces */
  IntTab src(0, 2);
  MD_Vector_tools::creer_tableau_distribue(o_zone.mdv_elems_faces, src);
  for (i = 0; i < src.dimension_tot(0); i++) src(i, 0) = Process::me(), src(i, 1) = i;
  src.echange_espace_virtuel();

  /* o_proc, o_item -> processeur/item de l'element, puis des autres faces utilisees dans W2(f,.) pour chaque face de la frontiere */
  DoubleTrav o_proc(0, o_n_f), o_item(0, o_n_f), proc(0, o_n_f), l_item(0, o_n_f);
  o_zone.creer_tableau_faces(o_proc), o_zone.creer_tableau_faces(o_item);
  fvf.frontiere().creer_tableau_faces(proc), fvf.frontiere().creer_tableau_faces(l_item);
  int c_max = 1; //nombre max d'items/coeffs a echanger par face
  for (i = 0; i < o_fvf.nb_faces(); i++)
    {
      f = o_fvf.num_face(i), e = o_f_e(f, 0);
      for (j = 0; j < o_n_f && o_e_f(e, j) != f; ) j++; // f = o_e_f(e, j)
      o_proc(f, 0) = src(e, 0), o_item(f, 0) = src(e, 1); //element
      for (k = o_zone.w2i(o_zone.m2d(e) + j) + 1, l = 1; k < o_zone.w2i(o_zone.m2d(e) + j + 1); k++, l++) //W2 hors diag
        {
          int idx = o_zone.nb_elem_tot() + o_e_f(e, o_zone.w2j(k));
          o_proc(f, l) = src(idx, 0), o_item(f, l) = src(idx, 1);
        }
      for (c_max = max(c_max, l); l < o_n_f; l++) o_proc(f, l) = o_item(f, l) = -1; //on finit avec des -1
    }
  c_max = Process::mp_max(c_max);

  //projection sur la frontiere locale
  if (o_fvf.frontiere().que_suis_je() == "Raccord_distant_homogene")
    o_fvf.frontiere().trace_face_distant(o_proc, proc), o_fvf.frontiere().trace_face_distant(o_item, l_item);
  else o_fvf.frontiere().trace_face_local(o_proc, proc), o_fvf.frontiere().trace_face_local(o_item, l_item);

  //remplissage
  item.resize(fvf.nb_faces(), c_max), item = -1;
  for (i = 0; i < fvf.nb_faces(); i++) for (j = 0; j < c_max && l_item(i, j) >= 0; j++)
      if (proc(i, j) == Process::me()) item(i, j) = l_item(i, j);                     //item local (reel)
      else
        {
          if (o_zone.virt_ef_map.count({{ (int) proc(i, j), (int) l_item(i, j) }}))   //item local (virtuel)
          {
            item(i, j) = o_zone.virt_ef_map.at({{ (int) proc(i, j),  (int) l_item(i, j) }});
          }
          else extra_items[ {{ (int) proc(i, j), (int) l_item(i, j) }}].push_back({{ i, j }}), item(i, j) = -2; //item manquant
        }

  //coeff : 1 coeff de plus que item -> celui de la face elle-meme (mis au debut)
  coeff.resize(0, 1 + item.dimension(1), N), delta_int.resize(0, N, 2),
               fvf.frontiere().creer_tableau_faces(coeff);

  //tableaux lies a la stabilisation de Le Potier / Mahamane (cf. Op_Diff_CoviMAC_Elem)
  stab_ = ref_cast(Op_Diff_CoviMAC_Elem, o_eqn.operateur(0).l_op_base()).stab_;
  if (stab_)
    {
      delta.resize(0, item.dimension(1), N);
      fvf.frontiere().creer_tableau_faces(delta_int), fvf.frontiere().creer_tableau_faces(delta);
    }
  coeffs_a_jour_ = 0, delta_a_jour_ = (stab_ ? 1 : 0);
  return 1;
}

void Echange_contact_CoviMAC::update_coeffs()
{
  if (coeffs_a_jour_) return;
  //objets de l'autre cote : equation, zone, inconnue (prefix o_), frontiere (pour faire trace_face_distant)
  Champ_front_calc& ch=ref_cast(Champ_front_calc, T_autre_pb().valeur());
  const Zone_CoviMAC& o_zone = ref_cast(Zone_CoviMAC, ch.zone_dis());
  const Equation_base&    o_eqn  = ch.equation();
  const Op_Diff_CoviMAC_Elem& o_op_diff = ref_cast(Op_Diff_CoviMAC_Elem, o_eqn.operateur(0).l_op_base());
  const Champ_P0_CoviMAC& o_inc  = ref_cast(Champ_P0_CoviMAC, ch.inconnue());
  const Front_VF&         o_fvf  = ref_cast(Front_VF, ch.front_dis());
  const IntTab& e_f = o_zone.elem_faces();
  const DoubleVect& fs = o_zone.face_surfaces(), &ve = o_zone.volumes();

  //tableaux "nu_faces" utilises par les operateurs de diffusion de chaque cote
  o_op_diff.update_nu(), o_op_diff.update_delta_int();
  int ne_tot = o_zone.nb_elem_tot(), N = ch.nb_comp();

  //tableaux aux faces de l'autre cote, a transmettre par o_fr.trace_face_{distant,local} : on ne les remplit que sur les faces de o_fr
  o_zone.init_m2();
  DoubleTrav o_Text, o_Himp, o_coeff, o_delta_int, nu_ef(e_f.dimension(1), N);
  if (monolithic) o_coeff.resize(o_zone.nb_faces(), 1 + item.dimension(1), N), o_delta_int.resize(o_zone.nb_faces(), N, 2);
  else o_Text.resize(o_zone.nb_faces(), N), o_Himp.resize(o_zone.nb_faces(), N);
  for (int i = 0; i < o_fvf.nb_faces(); i++)
    {
      int f = o_fvf.num_face(i), e = o_zone.face_voisins(f, 0), fb, j, k, n, i_f = 0, idx;
      for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++) if (fb == f) i_f = j; //numero de la face f dans l'element e
      o_op_diff.remplir_nu_ef(e, nu_ef);

      /* construction du flux de chaleur sortant, en mettant la partie en T_f dans H_imp et le reste dans T_ext */
      for (j = o_zone.w2i(o_zone.m2d(e) + i_f), idx = 0; j < o_zone.w2i(o_zone.m2d(e) + i_f + 1); j++, idx++)
        for (fb = e_f(e, o_zone.w2j(j)), n = 0; n < N; n++)
          {
            double fac = fs(fb) / ve(e) * o_zone.w2c(j) * nu_ef(o_zone.w2j(j), n);
            if (monolithic)
              o_coeff(f, idx ? idx + 1 : 0, n) += fs(f) * fac, o_coeff(f, 1, n) -= fs(f) * fac; //cote face, cote element
            else
              {
                if (f == fb) o_Himp(f, n) += fac;
                else o_Text(f, n) -= fac * o_inc.valeurs().addr()[N * (ne_tot + fb) + n];
                o_Text(f, n) += fac * o_inc.valeurs().addr()[N * e + n];
              }
          }
      for (n = 0; stab_ && monolithic && n < N; n++) for (k = 0; k < 2; k++) o_delta_int(f, n, k) = o_op_diff.delta_f_int(f, n, k);
      for (n = 0; !monolithic && n < N; n++) if (o_Himp(f, n) > 1e-10) o_Text(f, n) /= o_Himp(f, n); //passage au vrai T_ext
      for (n = 0; h_paroi < 1e9 && n < N; n++) //prise en compte de la resistance de la paroi
        {
          double fac = h_paroi / (h_paroi + (monolithic ? o_coeff(f, 0, n) / fs(f) : o_Himp(f, n)));
          if (monolithic)
            {
              if (stab_) o_delta_int(f, n, 0) *= fac;
              for (j = 0; j < o_coeff.dimension(1); j++) o_coeff(f, j, n) *= fac;
            }
          else o_Himp(f, n) *= fac;
        }
    }

  //transmission : soit par Raccord_distant_homogene, soit copie simple
  if (o_fvf.frontiere().que_suis_je() == "Raccord_distant_homogene")
    {
      if (monolithic) o_fvf.frontiere().trace_face_distant(o_coeff, coeff);
      else o_fvf.frontiere().trace_face_distant(o_Text, T_ext()->valeurs()), o_fvf.frontiere().trace_face_distant(o_Himp, h_imp_->valeurs());
      if (monolithic && stab_) o_fvf.frontiere().trace_face_distant(o_delta_int, delta_int);
    }
  else
    {
      if (monolithic) o_fvf.frontiere().trace_face_local(o_coeff, coeff);
      else o_fvf.frontiere().trace_face_local(o_Text, T_ext()->valeurs()), o_fvf.frontiere().trace_face_local(o_Himp, h_imp_->valeurs());
      if (monolithic && stab_) o_fvf.frontiere().trace_face_local(o_delta_int, delta_int);
    }

  if (monolithic) coeff.echange_espace_virtuel();
  else T_ext()->valeurs().echange_espace_virtuel(), h_imp_->valeurs().echange_espace_virtuel();
  if (monolithic && stab_) delta_int.echange_espace_virtuel();
  coeffs_a_jour_ = 1;
}

void Echange_contact_CoviMAC::update_delta() const
{
  if (!monolithic || delta_a_jour_) return; //deja fait
  const Champ_front_calc& ch=ref_cast(Champ_front_calc, T_autre_pb().valeur());
  const Front_VF& o_fvf = ref_cast(Front_VF, ch.front_dis());
  const Zone_CoviMAC& o_zone = ref_cast(Zone_CoviMAC, ch.zone_dis());
  const Equation_base&    o_eqn  = ch.equation();
  const Op_Diff_CoviMAC_Elem& o_op_diff = ref_cast(Op_Diff_CoviMAC_Elem, o_eqn.operateur(0).l_op_base());
  const IntTab& e_f = o_zone.elem_faces();

  //remplissage
  o_op_diff.update_delta();
  int i, j, n, N = ch.nb_comp();
  DoubleTrav o_delta(o_zone.nb_faces(), item.dimension(1), N);
  for (i = 0; i < o_fvf.nb_faces(); i++)
    {
      int f = o_fvf.num_face(i), fb, e = o_zone.face_voisins(f, 0), i_f = 0, idx;
      for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++) if (fb == f) i_f = j; //numero de la face f dans l'element e
      for (n = 0; n < N; n++) o_delta(f, 0, n) = o_op_diff.delta_e(e, n);
      for (j = o_zone.w2i(o_zone.m2d(e) + i_f) + 1, idx = 1; j < o_zone.w2i(o_zone.m2d(e) + i_f + 1); j++, idx++) //on saute le premier coeff (diagonale)
        for (n = 0; n < N; n++) o_delta(f, idx, n) = o_op_diff.delta_f(e_f(e, o_zone.w2j(j)), n);
    }

  //transmission
  if (o_fvf.frontiere().que_suis_je() == "Raccord_distant_homogene") o_fvf.frontiere().trace_face_distant(o_delta, delta);
  else o_fvf.frontiere().trace_face_local(o_delta, delta);
  delta.echange_espace_virtuel();
  delta_a_jour_ = 1;
}

void Echange_contact_CoviMAC::mettre_a_jour(double temps)
{
  Echange_externe_impose::mettre_a_jour(temps);
  coeffs_a_jour_ = 0, delta_a_jour_ = (stab_ ? 0 : 1);
}
