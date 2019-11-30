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
#include <Schema_Temps_base.h>
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
  Nom nom_pb, nom_bord;
  Motcle nom_champ;
  s >> nom_pb >> nom_bord >> nom_champ >> h_paroi;
  T_autre_pb().typer("Champ_front_calc");
  Champ_front_calc& ch=ref_cast(Champ_front_calc, T_autre_pb().valeur());
  ch.creer(nom_pb, nom_bord, nom_champ);
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
  const Milieu_base& le_milieu=ch.milieu();
  int nb_comp = le_milieu.conductivite()->nb_comp();
  Nom nom_racc1=frontiere_dis().frontiere().le_nom();
  Zone_dis_base& zone_dis1 = zone_Cl_dis().zone_dis().valeur();
  int nb_faces_raccord1 = zone_dis1.zone().raccord(nom_racc1).valeur().nb_faces();


  h_imp_.typer("Champ_front_fonc");
  h_imp_->fixer_nb_comp(nb_comp);
  h_imp_.valeurs().resize(nb_faces_raccord1, nb_comp);
  frontiere_dis().frontiere().creer_tableau_faces(h_imp_.valeurs());

  return ch.initialiser(temps,zone_Cl_dis().equation().inconnue());
}

void Echange_contact_PolyMAC::mettre_a_jour(double temps)
{
  //objets de l'autre cote : equation, zone, inconnue (prefix o_), frontiere (pour faire trace_face_distant)
  Champ_front_calc& ch=ref_cast(Champ_front_calc, T_autre_pb().valeur());
  const Zone_PolyMAC&     zone = ref_cast(Zone_PolyMAC, zone_Cl_dis().zone_dis().valeur()), o_zone = ref_cast(Zone_PolyMAC, ch.zone_dis());
  const Equation_base&    o_eqn  = ch.equation();
  const Champ_P0_PolyMAC& o_inc  = ref_cast(Champ_P0_PolyMAC, ch.inconnue());
  const Front_VF&         o_fvf  = ref_cast(Front_VF, ch.front_dis()), &fvf = ref_cast(Front_VF, frontiere_dis());
  const Milieu_base&      o_mil  = ch.milieu();
  const IntTab &e_f = o_zone.elem_faces();
  const DoubleTab &xp = o_zone.xp(), &xv = o_zone.xv();
  const DoubleVect &fs = o_zone.face_surfaces(), &ve = o_zone.volumes();

  //tableaux "nu_faces" utilises par les operateurs de diffusion de chaque cote
  const DoubleTab& nu = ref_cast(Op_Diff_PolyMAC_base, zone_Cl_dis().equation().operateur(0).l_op_base()).get_nu(),
                   &o_nu = ref_cast(Op_Diff_PolyMAC_base, o_eqn.operateur(0).l_op_base()).get_nu(),
                    &o_nu_fac = ref_cast(Op_Diff_PolyMAC_base, o_eqn.operateur(0).l_op_base()).get_nu_fac();
  int N_nu = o_nu.line_size(), ne_tot = o_zone.nb_elem_tot();

  //facteur par lequel on doit multiplier nu pour obtenir lambda
  double o_rCp = o_eqn.que_suis_je() == "Conduction"
                 || o_eqn.que_suis_je() == "Convection_Diffusion_Temperature"
                 || o_eqn.que_suis_je() == "Convection_Diffusion_Temperature_Turbulent"
                 || o_eqn.que_suis_je().debute_par("Transport") ? o_mil.masse_volumique()(0, 0) * o_mil.capacite_calorifique()(0, 0) : 1;

  //tableaux aux faces de l'autre cote, a transmettre par o_fr.trace_face_{distant,local} : on ne les remplit que sur les faces de o_fr
  o_zone.init_m2();
  DoubleTrav o_Text(o_zone.nb_faces()), o_Himp(o_zone.nb_faces());
  for (int i = 0; i < o_fvf.nb_faces(); i++)
    {
      int f = o_fvf.num_face(i), fb, j, k, l, e = o_zone.face_voisins(f, 0), i_f = 0;
      for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++) if (fb == f) i_f = j; //numero de la face f dans l'element e

      /* construction du flux de chaleur sortant, en mettant la partie en T_f dans H_imp et le reste dans T_ext */
      for (j = o_zone.w2i(o_zone.m2d(e) + i_f); j < o_zone.w2i(o_zone.m2d(e) + i_f + 1); j++)
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
          if (f == fb) o_Himp(f) += fac;
          else o_Text(f) -= fac * o_inc.valeurs()(ne_tot + fb);
          o_Text(f) += fac * o_inc.valeurs()(e);
        }
      if (o_Himp(f) > 1e-10) o_Text(f) /= o_Himp(f); //passage au vrai T_ext
      if (o_Himp(f) > 1e-10 && h_paroi < 1e9) o_Himp(f) = 1. / (1. / h_paroi + 1. / o_Himp(f)); //prise en compte de la resistance de la paroi
    }

  //transmission : soit par Raccord_distant_homogene, soit copie simple
  if (o_fvf.frontiere().que_suis_je() == "Raccord_distant_homogene")
    o_fvf.frontiere().trace_face_distant(o_Text, T_ext()->valeurs()), o_fvf.frontiere().trace_face_distant(o_Himp, h_imp_->valeurs());
  else o_fvf.frontiere().trace_face_local(o_Text, T_ext()->valeurs()), o_fvf.frontiere().trace_face_local(o_Himp, h_imp_->valeurs());
  //si nu_f n'est pas encore connu de notre cote, alors on n'echange pas
  for (int i = 0; i < fvf.nb_faces(); i++) if (nu.addr()[zone.face_voisins(fvf.num_face(i), 0)] < 1e-12) h_imp_->valeurs().addr()[i] = 1e-8;
  T_ext()->valeurs().echange_espace_virtuel(), h_imp_->valeurs().echange_espace_virtuel();
  Echange_externe_impose::mettre_a_jour(temps);
}
