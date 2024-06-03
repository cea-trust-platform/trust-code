/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Echange_contact_PolyMAC_P0.h>
#include <Champ_front_calc.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>
#include <Schema_Euler_Implicite.h>
#include <Milieu_base.h>
#include <Domaine_PolyMAC_P0.h>
#include <Equation_base.h>
#include <Champ_Elem_PolyMAC_P0.h>
#include <Operateur.h>
#include <Op_Diff_PolyMAC_P0_Elem.h>
#include <Front_VF.h>


Implemente_instanciable(Echange_contact_PolyMAC_P0,"Paroi_Echange_contact_PolyMAC_P0",Echange_externe_impose);
using namespace MEDCoupling;

Sortie& Echange_contact_PolyMAC_P0::printOn(Sortie& s ) const
{
  return s << que_suis_je() << finl;
}

Entree& Echange_contact_PolyMAC_P0::readOn(Entree& s )
{
  if (app_domains.size() == 0) app_domains = { Motcle("Thermique"), Motcle("Neutronique"), Motcle("fraction_massique"), Motcle("indetermine") };

  //Nom nom_bord;
  //Motcle nom_champ;
  s >> nom_autre_pb_ >> nom_bord_ >> nom_champ_ >> invh_paroi;
  invh_paroi = invh_paroi > 1e8 ? 0 : 1. / invh_paroi;
  T_ext().typer("Ch_front_var_instationnaire_dep");
  T_ext()->fixer_nb_comp(1);
  return s;
}

void Echange_contact_PolyMAC_P0::init_op() const
{
  if (o_diff.non_nul()) return;
  Champ_front_calc ch;
  ch.creer(nom_autre_pb_, nom_bord_, nom_champ_);
  fvf = ref_cast(Front_VF, frontiere_dis()), o_fvf = ref_cast(Front_VF, ch.front_dis()); //frontieres
  const Equation_base& eqn = domaine_Cl_dis().equation(), &o_eqn = ch.equation(); //equations
  i_fvf = eqn.domaine_dis()->rang_frontiere(fvf->le_nom()), i_o_fvf = o_eqn.domaine_dis()->rang_frontiere(nom_bord_);

  int i_op = -1, o_i_op = -1, i; //indice de l'operateur de diffusion dans l'autre equation
  for (i = 0; i < eqn.nombre_d_operateurs(); i++)
    if (sub_type(Op_Diff_PolyMAC_P0_Elem, eqn.operateur(i).l_op_base())) i_op = i;
  if (i_op < 0) Process::exit(le_nom() + " : no diffusion operator found in " + eqn.probleme().le_nom() + " !");
  for (i = 0; i < o_eqn.nombre_d_operateurs(); i++)
    if (sub_type(Op_Diff_PolyMAC_P0_Elem, o_eqn.operateur(i).l_op_base())) o_i_op = i;
  if (o_i_op < 0) Process::exit(le_nom() + " : no diffusion operator found in " + o_eqn.probleme().le_nom() + " !");
  diff = ref_cast(Op_Diff_PolyMAC_P0_Elem, eqn.operateur(i_op).l_op_base());
  o_diff = ref_cast(Op_Diff_PolyMAC_P0_Elem, o_eqn.operateur(o_i_op).l_op_base());

}

/* identification des elements / faces de l'autre cote de la frontiere, avec offsets */
void Echange_contact_PolyMAC_P0::init_fs_dist() const
{
  if (fs_dist_init_) return; //deja fait
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, fvf->domaine_dis()), &o_domaine = ref_cast(Domaine_PolyMAC_P0, o_fvf->domaine_dis());
  const DoubleTab& xv = domaine.xv(), &o_xv = o_domaine.xv(), &xs = domaine.domaine().coord_sommets(), &o_xs = o_domaine.domaine().coord_sommets();
  const IntTab& f_s = domaine.face_sommets(), &o_f_s = o_domaine.face_sommets();

  int i, j, f, o_f, s, o_s, nf_tot = fvf->nb_faces_tot(), o_nf_tot = o_fvf->nb_faces_tot(), d, D = dimension;
  f_dist.resize(nf_tot);

  std::set<int> s_som, s_o_som; //sommets de chaque cote
  for (i = 0; i <   nf_tot; i++)
    for (f =   fvf->num_face(i), j = 0; j <   f_s.dimension(1) && (s =   f_s(f, j)) >= 0; j++)   s_som.insert(s);
  for (i = 0; i < o_nf_tot; i++)
    for (f = o_fvf->num_face(i), j = 0; j < o_f_s.dimension(1) && (s = o_f_s(f, j)) >= 0; j++) s_o_som.insert(s);
  std::vector<int> som(s_som.begin(), s_som.end()), o_som(s_o_som.begin(), s_o_som.end()); //en vecteur
  int ns_tot = (int)som.size(), o_ns_tot = (int)o_som.size();

  DoubleTrav xvf(nf_tot, D), o_xvf(o_nf_tot, D), xsf(ns_tot, D), o_xsf(o_ns_tot, D); //positions locales/distantes -> pour calcul de correspondance
  for (i = 0; i <   nf_tot; i++)
    for (d = 0; d < D; d++)   xvf(i, d) =   xv(  fvf->num_face(i), d);
  for (i = 0; i < o_nf_tot; i++)
    for (d = 0; d < D; d++) o_xvf(i, d) = o_xv(o_fvf->num_face(i), d);
  for (i = 0; i <   ns_tot; i++)
    for (d = 0; d < D; d++)   xsf(i, d) =   xs(  som[i], d);
  for (i = 0; i < o_ns_tot; i++)
    for (d = 0; d < D; d++) o_xsf(i, d) = o_xs(o_som[i], d);

#ifdef MEDCOUPLING_
  MCAuto<DataArrayDouble> fdad(DataArrayDouble::New()), o_fdad(DataArrayDouble::New()), sdad(DataArrayDouble::New()), o_sdad(DataArrayDouble::New());
  fdad->useExternalArrayWithRWAccess(xvf.addr(), nf_tot, D), o_fdad->useExternalArrayWithRWAccess(o_xvf.addr(), o_nf_tot, D);
  sdad->useExternalArrayWithRWAccess(xsf.addr(), ns_tot, D), o_sdad->useExternalArrayWithRWAccess(o_xsf.addr(), o_ns_tot, D);
  //point de o_{f,s}dad le plus proche de chaque point de {f,s}dad
  MCAuto<DataArrayInt> f_idx(nf_tot && o_nf_tot ? o_fdad->findClosestTupleId(fdad) : NULL), s_idx(ns_tot && o_ns_tot ? o_sdad->findClosestTupleId(sdad) : NULL);

  for (i = 0; i < nf_tot; i++) //remplissage de f_dist : face distante si coincidence, -1 sinon
    {
      f = fvf->num_face(i), o_f = o_nf_tot ? o_fvf->num_face(f_idx->getIJ(i, 0)) : -1;
      double d2 = o_f >= 0 ? domaine.dot(&xv(f, 0), &xv(f, 0), &o_xv(o_f, 0), &o_xv(o_f, 0)) : 1e8;
      if (d2 < 1e-12) f_dist(i) = o_f;
      else f_dist(i) = -1;
      if (i < fvf->nb_faces() && d2 >= 1e-12)
        Process::exit(Nom("Echange_contact_PolyMAC_P0: missing opposite faces detected between ") + fvf->le_nom() + " and " + o_fvf->le_nom()
                      + " ! Have you used Decouper_multi?");
    }
  for (i = 0; i < ns_tot; i++) //remplissage de s_dist
    {
      s = som[i], o_s = o_ns_tot ? o_som[s_idx->getIJ(i, 0)] : -1;
      if (o_s >= 0 && domaine.dot(&xs(s, 0), &xs(s, 0), &o_xs(o_s, 0), &o_xs(o_s, 0)) < 1e-12) s_dist[s] = o_s;
    }

#else
  Process::exit("Echange_contact_PolyMAC_P0 : MEDCoupling is required!");
#endif
  fs_dist_init_ = 1;
}
