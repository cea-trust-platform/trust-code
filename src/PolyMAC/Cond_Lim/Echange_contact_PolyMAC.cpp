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
// File:        Echange_contact_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Cond_Lim
// Version:     /main/32
//
//////////////////////////////////////////////////////////////////////////////

#include <Echange_contact_PolyMAC.h>
#include <Champ_front_calc.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>
#include <Schema_Euler_Implicite.h>
#include <Milieu_base.h>

#include <Zone_PolyMAC.h>
#include <Equation_base.h>
#include <Champ_P0_PolyMAC.h>
#include <Operateur.h>
#include <Op_Diff_PolyMAC_base.h>

#include <cmath>

Implemente_instanciable(Echange_contact_PolyMAC,"Paroi_Echange_contact_PolyMAC",Echange_externe_impose);

int meme_point2(const DoubleVect& a,const DoubleVect& b);


Sortie& Echange_contact_PolyMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Echange_contact_PolyMAC::readOn(Entree& s )
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

void Echange_contact_PolyMAC::completer()
{
  Echange_externe_impose::completer();
  T_autre_pb().associer_fr_dis_base(T_ext().frontiere_dis());
  T_autre_pb()->completer();
}

int Echange_contact_PolyMAC::initialiser(double temps)
{
  if (!Echange_externe_impose::initialiser(temps))
    return 0;

  Champ_front_calc& ch=ref_cast(Champ_front_calc, T_autre_pb().valeur());
  const Equation_base&    o_eqn  = ch.equation();
  const Front_VF& fvf = ref_cast(Front_VF, frontiere_dis()), o_fvf = ref_cast(Front_VF, ch.front_dis());
  const Zone_PolyMAC& o_zone = ref_cast(Zone_PolyMAC, ch.zone_dis());
  const Milieu_base& le_milieu=ch.milieu();
  const IntTab& o_f_e = o_zone.face_voisins(), &o_e_f = o_zone.elem_faces();
  int i, j, k, l, e, f, nb_comp = le_milieu.conductivite()->nb_comp(), o_n_f = o_e_f.dimension(1);
  Nom nom_racc1=frontiere_dis().frontiere().le_nom();

  h_imp_.typer("Champ_front_fonc");
  h_imp_->fixer_nb_comp(nb_comp);
  h_imp_.valeurs().resize(0, nb_comp);
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
  DoubleTrav o_proc(0, o_n_f), o_item(0, o_n_f), proc(0, o_n_f), item(0, o_n_f);
  o_zone.creer_tableau_faces(o_proc), o_zone.creer_tableau_faces(o_item);
  fvf.frontiere().creer_tableau_faces(proc), fvf.frontiere().creer_tableau_faces(item);
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
    o_fvf.frontiere().trace_face_distant(o_proc, proc), o_fvf.frontiere().trace_face_distant(o_item, item);
  else o_fvf.frontiere().trace_face_local(o_proc, proc), o_fvf.frontiere().trace_face_local(o_item, item);

  //remplissage
  remote_item.resize(fvf.nb_faces(), c_max), remote_item = -1;
  for (i = 0; i < fvf.nb_faces(); i++) for (j = 0; j < c_max && item(i, j) >= 0; j++)
      if (proc(i, j) == Process::me()) remote_item(i, j) = item(i, j);                 //item local (reel)
      else if (o_zone.virt_ef_map.count({{ (int) proc(i, j), (int) item(i, j) }}))     //item local (virtuel)
        remote_item(i, j) = o_zone.virt_ef_map.at({{ (int) proc(i, j),  (int) item(i, j) }});
  else extra_items[ {{ (int) proc(i, j), (int) item(i, j) }}] = {{ i, j }};            //item manquant

  //remote_coeff : 1 coeff de plus que remote_item -> celui de la diagonale de W2 (mis au debut)
  remote_coeff.resize(0, 1 + remote_item.dimension(1)), remote_contrib.resize(0, 1);
  fvf.frontiere().creer_tableau_faces(remote_coeff), fvf.frontiere().creer_tableau_faces(remote_contrib);

  return 1;
}

void Echange_contact_PolyMAC::mettre_a_jour(double temps)
{
  //objets de l'autre cote : equation, zone, inconnue (prefix o_), frontiere (pour faire trace_face_distant)
  Champ_front_calc& ch=ref_cast(Champ_front_calc, T_autre_pb().valeur());
  const Zone_PolyMAC& o_zone = ref_cast(Zone_PolyMAC, ch.zone_dis());
  const Equation_base&    o_eqn  = ch.equation(), &eqn = zone_Cl_dis().equation();
  const Champ_P0_PolyMAC& o_inc  = ref_cast(Champ_P0_PolyMAC, ch.inconnue());
  const Front_VF&         o_fvf  = ref_cast(Front_VF, ch.front_dis());
  const Milieu_base&      o_mil  = ch.milieu(), &mil = eqn.milieu();
  const IntTab& e_f = o_zone.elem_faces();
  const DoubleTab& xp = o_zone.xp(), &xv = o_zone.xv();
  const DoubleVect& fs = o_zone.face_surfaces(), &ve = o_zone.volumes();

  //tableaux "nu_faces" utilises par les operateurs de diffusion de chaque cote
  const DoubleTab& o_nu = ref_cast(Op_Diff_PolyMAC_base, o_eqn.operateur(0).l_op_base()).get_nu(),
                   &o_nu_fac = ref_cast(Op_Diff_PolyMAC_base, o_eqn.operateur(0).l_op_base()).get_nu_fac();
  int N_nu = o_nu.line_size(), ne_tot = o_zone.nb_elem_tot();

  //facteur par lequel on doit multiplier nu pour obtenir lambda
  double o_rCp = o_eqn.que_suis_je() == "Conduction"
                 || o_eqn.que_suis_je() == "Convection_Diffusion_Temperature"
                 || o_eqn.que_suis_je() == "Convection_Diffusion_Temperature_Turbulent"
                 || o_eqn.que_suis_je().debute_par("Transport") ? o_mil.masse_volumique()(0, 0) * o_mil.capacite_calorifique()(0, 0) : 1,
                 rCp = eqn.que_suis_je() == "Conduction"
                       || eqn.que_suis_je() == "Convection_Diffusion_Temperature"
                       || eqn.que_suis_je() == "Convection_Diffusion_Temperature_Turbulent"
                       || eqn.que_suis_je().debute_par("Transport") ? mil.masse_volumique()(0, 0) * mil.capacite_calorifique()(0, 0) : 1;

  //tableaux aux faces de l'autre cote, a transmettre par o_fr.trace_face_{distant,local} : on ne les remplit que sur les faces de o_fr
  o_zone.init_m2();
  DoubleTrav o_Text, o_Himp, o_coeff, o_contrib;
  if (monolithic) o_coeff.resize(o_zone.nb_faces(), 1 + remote_item.dimension(1)), o_contrib.resize(o_zone.nb_faces());
  else o_Text.resize(o_zone.nb_faces()), o_Himp.resize(o_zone.nb_faces());
  for (int i = 0; i < o_fvf.nb_faces(); i++)
    {
      int f = o_fvf.num_face(i), fb, j, k, l, e = o_zone.face_voisins(f, 0), i_f = 0, idx;
      for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++) if (fb == f) i_f = j; //numero de la face f dans l'element e

      /* construction du flux de chaleur sortant, en mettant la partie en T_f dans H_imp et le reste dans T_ext */
      for (j = o_zone.w2i(o_zone.m2d(e) + i_f), idx = 0; j < o_zone.w2i(o_zone.m2d(e) + i_f + 1); j++, idx++)
        {
          fb = e_f(e, o_zone.w2j(j));
          double nu_ef = 0;
          if (N_nu == 1) nu_ef = o_nu.addr()[e]; //isotrope
          else if (N_nu == dimension) for (k = 0; k < dimension; k++) //anisotrope diagonal
              nu_ef += o_nu.addr()[dimension * e + k] * std::pow(xv(fb, k) - xp(e, k), 2);
          else if (N_nu == dimension * dimension) for (k = 0; k < dimension; k++) for (l = 0; l < dimension; l++)
                nu_ef += o_nu.addr()[dimension * (dimension * e + k) + l] * (xv(fb, k) - xp(e, k)) * (xv(fb, l) - xp(e, l)); //anisotrope complet
          else abort();
          nu_ef *= o_rCp * o_nu_fac.addr()[f] * (N_nu > 1 ? 1. / o_zone.dot(&xv(f, 0), &xv(f, 0), &xp(e, 0), &xp(e, 0)) : 1);
          double fac = fs(fb) / ve(e) * o_zone.w2c(j) * nu_ef;
          if (monolithic)
            {
              o_coeff(f, idx ? idx + 1 : 0) += fs(f) * fac / rCp, o_coeff(f, 1) -= fs(f) * fac / rCp;
              o_contrib(f) += fs(f) * fac / rCp * ((idx ? o_inc.valeurs()(ne_tot + fb) : 0) - o_inc.valeurs()(e));
            }
          else
            {
              if (f == fb) o_Himp(f) += fac;
              else o_Text(f) -= fac * o_inc.valeurs()(ne_tot + fb);
              o_Text(f) += fac * o_inc.valeurs()(e);
            }
        }
      if (!monolithic && o_Himp(f) > 1e-10) o_Text(f) /= o_Himp(f); //passage au vrai T_ext
      if (h_paroi < 1e9) //prise en compte de la resistance de la paroi
        {
          double fac = h_paroi / (h_paroi + (monolithic ? o_coeff(f, 0) * rCp / fs(f) : o_Himp(f)));
          if (monolithic) for (j = 0, o_contrib(f) *= fac; j < o_coeff.dimension(1); j++) o_coeff(f, j) *= fac;
          else o_Himp(f) *= fac;
        }
    }

  //transmission : soit par Raccord_distant_homogene, soit copie simple
  if (o_fvf.frontiere().que_suis_je() == "Raccord_distant_homogene")
    {
      if (monolithic) o_fvf.frontiere().trace_face_distant(o_coeff, remote_coeff), o_fvf.frontiere().trace_face_distant(o_contrib, remote_contrib);
      else o_fvf.frontiere().trace_face_distant(o_Text, T_ext()->valeurs()), o_fvf.frontiere().trace_face_distant(o_Himp, h_imp_->valeurs());
    }
  else
    {
      if (monolithic) o_fvf.frontiere().trace_face_local(o_coeff, remote_coeff), o_fvf.frontiere().trace_face_local(o_contrib, remote_contrib);
      else o_fvf.frontiere().trace_face_local(o_Text, T_ext()->valeurs()), o_fvf.frontiere().trace_face_local(o_Himp, h_imp_->valeurs());
    }

  if (monolithic) remote_coeff.echange_espace_virtuel(), remote_contrib.echange_espace_virtuel();
  else T_ext()->valeurs().echange_espace_virtuel(), h_imp_->valeurs().echange_espace_virtuel();
  Echange_externe_impose::mettre_a_jour(temps);
}
