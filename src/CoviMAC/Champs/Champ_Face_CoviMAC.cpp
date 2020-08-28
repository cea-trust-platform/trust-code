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
// File:        Champ_Face_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Face_CoviMAC.h>
#include <Domaine.h>
#include <Zone_VF.h>
#include <Champ_Uniforme.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_dis.h>
#include <Zone_Cl_CoviMAC.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Connectivite_som_elem.h>
#include <Matrix_tools.h>
#include <EChaine.h>
#include <ConstDoubleTab_parts.h>
#include <Linear_algebra_tools_impl.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Schema_Temps_base.h>
#include <array>
#include <cmath>

Implemente_instanciable(Champ_Face_CoviMAC,"Champ_Face_CoviMAC",Champ_Inc_base) ;

Sortie& Champ_Face_CoviMAC::printOn(Sortie& os) const
{
  os << que_suis_je() << " " << le_nom();
  return os;
}

Entree& Champ_Face_CoviMAC::readOn(Entree& is)
{
  return is;
}

Champ_base& Champ_Face_CoviMAC::le_champ(void)
{
  return *this;
}

const Champ_base& Champ_Face_CoviMAC::le_champ(void) const
{
  return *this;
}

void Champ_Face_CoviMAC::associer_zone_dis_base(const Zone_dis_base& z_dis)
{
  ref_zone_vf_=ref_cast(Zone_VF, z_dis);
}

int Champ_Face_CoviMAC::fixer_nb_valeurs_nodales(int n)
{

  // j'utilise le meme genre de code que dans Champ_Fonc_P0_base
  // sauf que je recupere le nombre de faces au lieu du nombre d'elements
  //
  // je suis tout de meme etonne du code utilise dans
  // Champ_Fonc_P0_base::fixer_nb_valeurs_nodales()
  // pour recuperer la zone discrete...

  const Champ_Inc_base& self = ref_cast(Champ_Inc_base, *this);
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,self.zone_dis_base());

  assert(n == zone.nb_faces() + dimension * zone.nb_elem());

  // Probleme: nb_comp vaut dimension mais on ne veut qu'une dimension !!!
  // HACK :
  int old_nb_compo = nb_compo_;
  nb_compo_ /= dimension;

  /* variables : valeurs normales aux faces, puis valeurs aux elements par blocs -> pour que line_size() marche */
  creer_tableau_distribue(zone.mdv_ch_face);
  nb_compo_ = old_nb_compo;
  return n;
}


Champ_base& Champ_Face_CoviMAC::affecter_(const Champ_base& ch)
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,ref_zone_vf_.valeur());
  const DoubleVect& fs = zone.face_surfaces();
  const DoubleTab& nf = zone.face_normales(), &xv = zone.xv();
  DoubleTab& val = valeurs(), eval;
  if (sub_type(Champ_Uniforme, ch)) eval = ch.valeurs();
  else eval.resize(val.dimension_tot(0), dimension), ch.valeur_aux(xv,eval);

  for (int f = 0, unif = sub_type(Champ_Uniforme, ch); f < zone.nb_faces_tot(); f++) for (int r = 0; r < dimension; r++)
      val(f) += eval(unif ? 0 : f, r) * nf(f, r) / fs(f);
  return *this;
}

DoubleVect& Champ_Face_CoviMAC::valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const
{
  throw;
  // return Champ_implementation_RT0::valeur_a_elem(position,result,poly);
}

double Champ_Face_CoviMAC::valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const
{
  throw;
  //return Champ_implementation_RT0::valeur_a_elem_compo(position,poly,ncomp);
}

//tableaux de correspondance pour les CLs
void Champ_Face_CoviMAC::init_cl() const
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  const Conds_lim& cls = zone_Cl_dis().les_conditions_limites();
  int i, f, n;

  if (fcl.dimension(0)) return;
  fcl.resize(zone.nb_faces_tot(), 3);
  for (n = 0; n < cls.size(); n++)
    {
      const Front_VF& fvf = ref_cast(Front_VF, cls[n].frontiere_dis());
      int idx = sub_type(Neumann, cls[n].valeur()) + sub_type(Neumann_homogene, cls[n].valeur())
                + 2 * sub_type(Symetrie, cls[n].valeur())
                + 3 * sub_type(Dirichlet, cls[n].valeur()) + 4 * sub_type(Dirichlet_homogene, cls[n].valeur());
      if (!idx) Cerr << "Champ_Face_CoviMAC : CL non codee rencontree!" << finl, Process::exit();
      for (i = 0; i < fvf.nb_faces_tot(); i++)
        f = fvf.num_face(i), fcl(f, 0) = idx, fcl(f, 1) = n, fcl(f, 2) = i;
    }
  CRIMP(fcl);
}

void Champ_Face_CoviMAC::init_ve() const
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  zone.init_ve(), init_cl();
  IntTrav egrad_d, egrad_j;
  DoubleTrav egrad_c;
  /* (grad v) aux elements */
  zone.egrad(fcl, { 0, 1, 1, 0, 0}, NULL, 1, egrad_d, egrad_j, egrad_c); //1 compo
  /* matrice et second membre de M.ve^(2) = ve^(1) + b */
  zone.init_ve2(fcl, { 0, 1, 1, 0, 0}, egrad_d, egrad_j, egrad_c, ve_mat, ve_bd, ve_bj, ve_bc, valeurs().line_size()); //N compo
  /* solveur direct pour M */
  char lu[] = "Petsc Cholesky { quiet }";
  EChaine ch(lu);
  ch >> ve_solv;
}

/* met en coherence les composantes aux elements avec les vitesses aux faces : interpole sur phi * v */
void Champ_Face_CoviMAC::update_ve(DoubleTab& val) const
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  const Conds_lim& cls = zone_Cl_dis().les_conditions_limites();
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem();
  int e, f, j, k, d, D = dimension, n, N = val.line_size();
  DoubleTab_parts part(val); //part[0] -> aux faces, part[1] -> aux elems
  DoubleTrav b(part[1]); //membre de droite du systeme ve_mat.part[1] = b
  init_ve();

  for (e = 0; e < zone.nb_elem(); e++)
    {
      /* interpolation d'ordre 1 de v * phi */
      for (j = zone.ved(e); j < zone.ved(e + 1); j++) for (f = zone.vej(j), d = 0; d < D; d++) for (n = 0; n < N; n++)
            b.addr()[N * (D * e + d) + n] += zone.vec(j, d) * part[0].addr()[N * f + n] * pf(f);
      /* partie "CLs de Dirichlet" de grad v_e */
      for (j = N * D * e, d = 0; d < D; d++) for (n = 0; n < N; n++, j++) for (k = ve_bd(j); k < ve_bd(j + 1); k++) if (fcl(f = ve_bj(k, 0), 0) == 3)
              b.addr()[j] -= ve_bc(k) * pf(f) * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), ve_bj(k, 1));
    }

  /* resolution -> phi * ve d'ordre 2 */
  ve_solv.resoudre_systeme(ve_mat, b, part[1]);
  /* retour a ve */
  for (e = 0; e < zone.nb_elem(); e++) for (d = 0; d < D; d++) for (n = 0; n < N; n++) part[1].addr()[N * (D * e + d) + n] /= pe(e);
  part[1].echange_espace_virtuel();
}

/* gradient d_j v_i aux elements : interp_ve -> grad -> interp_ve */
void Champ_Face_CoviMAC::interp_gve(const DoubleTab& inco, DoubleTab& vals) const
{
  abort();
}

DoubleTab& Champ_Face_CoviMAC::valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val) const
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  const Champ_base& cha=le_champ();
  int nb_compo=cha.nb_comp(), nf_tot = zone.nb_faces_tot(), d, D = dimension, n, N = valeurs().line_size();
  if (val.nb_dim() == 1)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(nb_compo == 1);
    }
  else if (val.nb_dim() == 2)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(val.dimension(1) == nb_compo);
    }
  else
    {
      Cerr << "Erreur TRUST dans Champ_Face_implementation::valeur_aux_elems()" << finl;
      Cerr << "Le DoubleTab val a plus de 2 entrees" << finl;
      Process::exit();
    }

  if (nb_compo == 1)
    {
      Cerr<<"Champ_Face_implementation::valeur_aux_elems"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }

  for (int p = 0, e; p < les_polys.size(); p++) for (e = les_polys(p), d = 0; e < zone.nb_elem() && d < D; d++) for (n = 0; n < N; n++)
        val(p, N * d + n) = valeurs()(N * (nf_tot + D * e + d) + n);
  return val;
}

DoubleVect& Champ_Face_CoviMAC::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& val, int ncomp) const
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  int nf_tot = zone.nb_faces_tot(), D = dimension, N = valeurs().line_size();
  assert(val.size() == polys.size());

  for (int p = 0, e; p < polys.size(); p++) val(p) = (e = polys(p)) < 0 ? 0. : valeurs().addr()[N * (nf_tot + D * e) + ncomp];

  return val;
}

DoubleTab& Champ_Face_CoviMAC::remplir_coord_noeuds(DoubleTab& positions) const
{

  throw;
  // return Champ_implementation_RT0::remplir_coord_noeuds(positions);
}

int Champ_Face_CoviMAC::remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const
{

  throw;
  //  return Champ_implementation_RT0::remplir_coord_noeuds_et_polys(positions,polys);
}

DoubleTab& Champ_Face_CoviMAC::valeur_aux_faces(DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  int nb_compo=cha.nb_comp();

  if (nb_compo == 1)
    {
      Cerr<<"Champ_Face_CoviMAC::valeur_aux_faces"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }

  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,ref_zone_vf_.valeur());
  const DoubleTab& nf = zone.face_normales();
  const DoubleVect& fs = zone.face_surfaces();

  val.resize(zone.nb_faces(), dimension), val = 0;

  for (int f = 0; f < zone.nb_faces(); f++)
    for (int r = 0; r < dimension; r++)
      val(f, r) = cha.valeurs()(f) * nf(f, r) / fs(f);
  return val;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//Methode qui renvoie SMA_barre aux elements a partir de la vitesse aux faces
//SMA_barre = Sij*Sij (sommation sur les indices i et j)
////////////////////////////////////////////////////////////////////////////////////////////////////

DoubleVect& Champ_Face_CoviMAC::calcul_S_barre_sans_contrib_paroi(const DoubleTab& vitesse, DoubleVect& SMA_barre) const
{
  // avec contrib au bord pour l'instant...
  abort();
  return calcul_S_barre(vitesse, SMA_barre);
}

DoubleVect& Champ_Face_CoviMAC::calcul_S_barre(const DoubleTab& vitesse, DoubleVect& SMA_barre) const
{
  abort();
  return SMA_barre;
}

DoubleTab& Champ_Face_CoviMAC::trace(const Frontiere_dis_base& fr, DoubleTab& x, double t, int distant) const
{
  assert(distant==0);
  init_cl();
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  const bool vectoriel = (le_champ().nb_comp() > 1);
  const int dim = vectoriel ? dimension : 1, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();
  const Front_VF& fr_vf = ref_cast(Front_VF, fr);
  const IntTab& face_voisins = zone.face_voisins();
  const DoubleTab& val = valeurs(t);

  for (int i = 0; i < fr_vf.nb_faces(); i++)
    {
      const int face = fr_vf.num_premiere_face() + i, elem = face_voisins(face, 0);
      for (int d = 0; d < dim; d++) x(i, d) = val(vectoriel ? nf_tot + ne_tot * d + elem : face);
    }

  return x;
}
