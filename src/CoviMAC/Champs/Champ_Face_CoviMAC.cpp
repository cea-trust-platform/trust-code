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

int Champ_Face_CoviMAC::fixer_nb_valeurs_temporelles(int i)
{
  valeurs_normales_->fixer_nb_cases(i), valeurs_normales_a_jour_->fixer_nb_cases(i);
  return Champ_Inc_base::fixer_nb_valeurs_temporelles(i);
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

  assert(n == zone.nb_faces());

  // Probleme: nb_comp vaut dimension mais on ne veut qu'une dimension !!!
  // HACK :
  int old_nb_compo = nb_compo_;
  nb_compo_ /= dimension;
  creer_tableau_distribue(zone.md_vector_faces());
  nb_compo_ = old_nb_compo;
  return n;
}

void Champ_Face_CoviMAC::creer_tableau_distribue(const MD_Vector& md, Array_base::Resize_Options opt)
{
  /* pour les valeurs aux faces duales */
  Champ_Inc_base::creer_tableau_distribue(md, opt);
  if (valeurs_normales_->valeurs().size_array() == 0 && !valeurs_normales_->valeurs().get_md_vector().non_nul())
    for (int i = 0; i < nb_valeurs_temporelles(); i++)
      {
        if (nb_compo_ > 1) valeurs_normales_->futur(i).valeurs().resize(0, nb_compo_);
        MD_Vector_tools::creer_tableau_distribue(md, valeurs_normales_->futur(i).valeurs(), opt);
        valeurs_normales_a_jour_->futur(i).valeurs().resize(1); //une valeur par case : 0 (pas a jour) / 1 (a jour)
      }
}

DoubleTab& Champ_Face_CoviMAC::valeurs()
{
  valeurs_normales_a_jour_->valeurs() = 0;
  return Champ_Inc_base::valeurs();
}

DoubleTab& Champ_Face_CoviMAC::valeurs(double tps)
{
  DoubleTab& resu = Champ_Inc_base::valeurs(tps); //pour planter si on ne trouve pas le temps
  Roue& vaj = valeurs_normales_a_jour_.valeur();
  if (tps == temps()) vaj.valeurs() = 0;
  else if (temps() < tps)
    {
      for(int i = 0; i < nb_valeurs_temporelles() && vaj.futur(i).temps() <= temps(); i++)
        if (vaj.futur(i).temps() == temps()) vaj.futur(i).valeurs() = 0;
    }
  else if (temps() > tps)
    {
      for(int i = 0; i < nb_valeurs_temporelles() && vaj.passe(i).temps() >= temps(); i++)
        if (vaj.passe(i).temps() == temps()) vaj.passe(i).valeurs() = 0;
    }
  return resu;
}

DoubleTab& Champ_Face_CoviMAC::futur(int i)
{
  valeurs_normales_a_jour_->futur(i).valeurs() = 0;
  return Champ_Inc_base::futur(i);
}

DoubleTab& Champ_Face_CoviMAC::passe(int i)
{
  valeurs_normales_a_jour_->passe(i).valeurs() = 0;
  return Champ_Inc_base::passe(i);
}

Champ_Inc_base& Champ_Face_CoviMAC::avancer(int i)
{
  for (int j = 0; j < i; j++) valeurs_normales_->avancer(valeurs_normales_), valeurs_normales_a_jour_->avancer(valeurs_normales_a_jour_);
  return Champ_Inc_base::avancer(i);
}

Champ_Inc_base& Champ_Face_CoviMAC::reculer(int i)
{
  for (int j = 0; j < i; j++) valeurs_normales_->reculer(valeurs_normales_), valeurs_normales_a_jour_->reculer(valeurs_normales_a_jour_);
  return Champ_Inc_base::reculer(i);
}

double Champ_Face_CoviMAC::changer_temps_futur(const double& t, int i)
{
  valeurs_normales_->futur(i).changer_temps(t), valeurs_normales_a_jour_->futur(i).changer_temps(t);
  return Champ_Inc_base::changer_temps_futur(t, i);
}

double Champ_Face_CoviMAC::changer_temps_passe(const double& t, int i)
{
  valeurs_normales_->passe(i).changer_temps(t), valeurs_normales_a_jour_->passe(i).changer_temps(t);
  return Champ_Inc_base::changer_temps_passe(t, i);
}

Champ_Face_CoviMAC::operator DoubleTab& ()
{
  valeurs_normales_a_jour_->valeurs() = 0;
  return les_valeurs->valeurs();
}

double Champ_Face_CoviMAC::changer_temps(const double& t)
{
  valeurs_normales_->changer_temps(t), valeurs_normales_a_jour_->changer_temps(t);
  return Champ_Inc_base::changer_temps(t);
}

const DoubleTab& Champ_Face_CoviMAC::valeurs_normales() const
{
  if (!valeurs_normales_a_jour_->valeurs()(0)) interp_vfn(valeurs(), valeurs_normales_->valeurs()), valeurs_normales_a_jour_->valeurs() = 1;
  return valeurs_normales_->valeurs();
}

const DoubleTab& Champ_Face_CoviMAC::valeurs_normales(double tps)
{
  int idx = 0;
  if (temps() < tps) while (les_valeurs->futur( idx).temps() < tps && idx  < nb_valeurs_temporelles()) idx++;
  if (temps() > tps) while (les_valeurs->passe(-idx).temps() > tps && -idx < nb_valeurs_temporelles()) idx--;
  assert((idx ? (idx > 0 ? les_valeurs->futur(idx) : les_valeurs->passe(-idx)).temps() : temps()) == tps);
  const DoubleTab& val = idx ? (idx > 0 ? les_valeurs->futur(idx).valeurs() : les_valeurs->passe(-idx).valeurs()) : valeurs();
  DoubleTab& vn = idx ? (idx > 0 ? valeurs_normales_->futur(idx).valeurs() : valeurs_normales_->passe(-idx).valeurs()) : valeurs_normales_->valeurs(),
               &vn_a_j = idx ? (idx > 0 ? valeurs_normales_a_jour_->futur(idx).valeurs() : valeurs_normales_a_jour_->passe(-idx).valeurs()) : valeurs_normales_a_jour_->valeurs();
  if (!vn_a_j(0)) interp_vfn(val, vn), vn_a_j(0) = 1;
  return vn;
}

Champ_base& Champ_Face_CoviMAC::affecter_(const Champ_base& ch)
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,ref_zone_vf_.valeur());
  const IntTab& e_f = zone.face_voisins();
  const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces();
  const DoubleTab& nf = zone.face_normales(), &xp = zone.xp(), &xv = zone.xv();
  DoubleTab& val = valeurs(), eval;
  if (sub_type(Champ_Uniforme, ch)) eval = ch.valeurs();
  else eval.resize(val.dimension_tot(0), dimension), ch.valeur_aux(xv,eval);

  for (int f = 0, unif = sub_type(Champ_Uniforme, ch); f < zone.nb_faces_tot(); f++)
    for (int e0 = e_f(f, 0), e1 = e_f(f, 1), r = 0; r < dimension; r++)
      val(f) += eval(unif ? 0 : f, r) * (e1 >= 0 ? (xp(e1, r) - xp(e0, r)) * fs(f) / vf(f) : nf(f, r) / fs(f));
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

  if (icl.dimension(0)) return;
  icl.resize(zone.nb_faces_tot(), 3);
  for (n = 0; n < cls.size(); n++)
    {
      const Front_VF& fvf = ref_cast(Front_VF, cls[n].frontiere_dis());
      int idx = sub_type(Neumann, cls[n].valeur()) + sub_type(Neumann_homogene, cls[n].valeur())
                + 2 * sub_type(Symetrie, cls[n].valeur())
                + 3 * sub_type(Dirichlet, cls[n].valeur()) + 4 * sub_type(Dirichlet_homogene, cls[n].valeur());
      if (!idx) Cerr << "Champ_Face_CoviMAC : CL non codee rencontree!" << finl, Process::exit();
      for (i = 0; i < fvf.nb_faces_tot(); i++)
        f = fvf.num_face(i), icl(f, 0) = idx, icl(f, 1) = n, icl(f, 2) = i;
    }
  CRIMP(icl);
}

/* vitesse aux elements (mise en composante mineure) : interpole sur le champ phi * v */
void Champ_Face_CoviMAC::interp_ve(const DoubleTab& inco, DoubleTab& val) const
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem();
  int e, f, j, r, n, N = inco.line_size(), Nb = val.line_size();
  assert(Nb == N * dimension);

  zone.init_ve();
  for (e = 0, val = 0; e < val.dimension(0); e++) for (j = zone.ved(e); j < zone.ved(e + 1); j++)
      for (n = 0, f = zone.vej(j); n < N; n++) for (r = 0; r < dimension; r++)
          val.addr()[Nb * e + dimension * n + r] += zone.vec(j, r) * inco.addr()[N * f + n] * pf(f) / pe(e);
}

/* passage 'un champ aux faces duales a un champ aux faces : interpole le champ phi * v */
void Champ_Face_CoviMAC::interp_vfn(const DoubleTab& src, DoubleTab& dst) const
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  const DoubleVect& pf = zone.porosite_face();
  double coeff;
  int i, n, f, fb, N = src.line_size();
  assert(N == dst.line_size());

  zone.init_w1();
  for (f = 0, dst = 0; f < zone.nb_faces(); f++) for (i = zone.w1d(f); i < zone.w1d(f + 1); i++)
      for (n = 0, fb = zone.w1j(i), coeff = zone.w1c(i) * pf(fb) / pf(f); n < N; n++) dst.addr()[N * f + n] += coeff * src.addr()[N * fb + n];
}

/* gradient d_j v_i aux elements : interp_ve -> grad -> interp_ve */
void Champ_Face_CoviMAC::interp_gve(const DoubleTab& inco, DoubleTab& vals) const
{
  abort();
}

DoubleTab& Champ_Face_CoviMAC::valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  int nb_compo=cha.nb_comp();
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

  //on interpole ve sur tous les elements, puis on se restreint a les_polys
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  DoubleTrav ve(0, dimension);
  zone.zone().domaine().creer_tableau_elements(ve);
  interp_ve(cha.valeurs(), ve);
  for (int p = 0; p < les_polys.size(); p++) for (int r = 0, e = les_polys(p); e < zone.nb_elem() && r < dimension; r++) val(p, r) = ve(e, r);
  return val;
}

DoubleVect& Champ_Face_CoviMAC::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& val, int ncomp) const
{
  init_cl();
  const Champ_base& cha=le_champ();
  assert(val.size() == polys.size());

  //on interpole ve sur tous les elements, puis on se restreint a les_polys
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  DoubleTrav ve(0, dimension);
  zone.zone().domaine().creer_tableau_elements(ve);
  interp_ve(cha.valeurs(), ve);

  for (int p = 0; p < polys.size(); p++) val(p) = (polys(p) == -1) ? 0. : ve(polys(p), ncomp);

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
  const DoubleTab& tf = zone.face_tangentes();

  val.resize(zone.nb_faces(), dimension), val = 0;

  for (int f = 0; f < zone.nb_faces(); f++)
    for (int r = 0; r < dimension; r++)
      val(f, r) = cha.valeurs()(f) * tf(f, r);
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
  const bool vectoriel = (le_champ().nb_comp() > 1);
  const int dim = vectoriel ? dimension : 1;
  const Front_VF& fr_vf = ref_cast(Front_VF, fr);
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  const IntTab& face_voisins = zone.face_voisins();
  const DoubleTab& val = valeurs(t);
  DoubleTrav ve(0, dimension);
  if (vectoriel)
    {
      zone.zone().domaine().creer_tableau_elements(ve);
      interp_ve(val, ve);
    }

  for (int i = 0; i < fr_vf.nb_faces(); i++)
    {
      const int face = fr_vf.num_premiere_face() + i;
      for (int dir = 0; dir < 2; dir++)
        {
          const int elem = face_voisins(face, dir);
          if (elem != -1)
            {
              for (int d = 0; d < dim; d++) x(i, d) = vectoriel ? ve(elem, d) : val[elem];
            }
        }
    }

  return x;
}
