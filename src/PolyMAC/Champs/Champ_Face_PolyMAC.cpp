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
// File:        Champ_Face_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Linear_algebra_tools_impl.h>
#include <Connectivite_som_elem.h>
#include <Dirichlet_homogene.h>
#include <Champ_Fonc_reprise.h>
#include <Champ_Face_PolyMAC.h>
#include <Schema_Temps_base.h>
#include <Zone_Cl_PolyMAC.h>
#include <Champ_Uniforme.h>
#include <TRUSTTab_parts.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Matrix_tools.h>
#include <Zone_PolyMAC.h>
#include <Zone_Cl_dis.h>
#include <Dirichlet.h>
#include <Symetrie.h>
#include <EChaine.h>
#include <Domaine.h>
#include <Zone_VF.h>
#include <array>
#include <cmath>
#include <Operateur.h>
#include <Op_Diff_PolyMAC_base.h>
#include <MD_Vector_base.h>

Implemente_instanciable(Champ_Face_PolyMAC,"Champ_Face_PolyMAC",Champ_Inc_base) ;

Sortie& Champ_Face_PolyMAC::printOn(Sortie& os) const
{
  os << que_suis_je() << " " << le_nom();
  return os;
}

Entree& Champ_Face_PolyMAC::readOn(Entree& is)
{
  return is;
}

Champ_base& Champ_Face_PolyMAC::le_champ(void)
{
  return *this;
}

const Champ_base& Champ_Face_PolyMAC::le_champ(void) const
{
  return *this;
}

void Champ_Face_PolyMAC::associer_zone_dis_base(const Zone_dis_base& z_dis)
{
  ref_zone_vf_=ref_cast(Zone_VF, z_dis);
}


int Champ_Face_PolyMAC::fixer_nb_valeurs_nodales(int n)
{

  // j'utilise le meme genre de code que dans Champ_Fonc_P0_base
  // sauf que je recupere le nombre de faces au lieu du nombre d'elements
  //
  // je suis tout de meme etonne du code utilise dans
  // Champ_Fonc_P0_base::fixer_nb_valeurs_nodales()
  // pour recuperer la zone discrete...

  const Champ_Inc_base& self = ref_cast(Champ_Inc_base, *this);
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,self.zone_dis_base());

  assert(n == zone.nb_faces());

  // Probleme: nb_comp vaut 2 mais on ne veut qu'une dimension !!!
  // HACK :
  nb_compo_ /= dimension;
  creer_tableau_distribue(zone.md_vector_faces());
  nb_compo_ *= dimension;
  return n;

}

void Champ_Face_PolyMAC::init_auxiliary_variables()
{
  const Zone_PolyMAC& zone = ref_cast( Zone_PolyMAC,ref_zone_vf_.valeur());
  for (int n = 0; n < nb_valeurs_temporelles(); n++)
    {
      DoubleTab &vals = futur(n);
      vals.set_md_vector(MD_Vector()); //on enleve le MD_Vector...
      vals.resize_dim0(zone.mdv_faces_aretes.valeur().get_nb_items_tot()); //...on dimensionne a la bonne taille...
      vals.set_md_vector(zone.mdv_faces_aretes); //...et on remet le bon MD_Vector
    }
}

Champ_base& Champ_Face_PolyMAC::affecter_(const Champ_base& ch)
{
  const DoubleTab& v = ch.valeurs();
  DoubleTab_parts parts(valeurs());
  DoubleTab& val = parts[0]; //partie vitesses
  const Zone_PolyMAC& zone = ref_cast( Zone_PolyMAC,ref_zone_vf_.valeur());
  int nb_faces = zone.nb_faces();
  const DoubleVect& surface = zone.face_surfaces();
  const DoubleTab& normales = zone.face_normales();

  if (sub_type(Champ_Uniforme,ch))
    {
      for (int num_face=0; num_face<nb_faces; num_face++)
        {
          double vn=0;
          for (int dir=0; dir<dimension; dir++)
            vn+=v(0,dir)*normales(num_face,dir);

          vn/=surface(num_face);
          val(num_face) = vn;
        }
    }
  else if (sub_type(Champ_Fonc_reprise, ch))
    {
      for (int num_face=0; num_face<nb_faces; num_face++)
        val(num_face) = ch.valeurs()[num_face];
    }
  else
    {
      //      int ndeb_int = zone.premiere_face_int();
      //      const IntTab& face_voisins = zone.face_voisins();
      const DoubleTab& xv=zone.xv();
      DoubleTab eval(val.dimension_tot(0),dimension);
      ch.valeur_aux(xv,eval);
      for (int num_face=0; num_face<nb_faces; num_face++)
        {
          double vn=0;
          for (int dir=0; dir<dimension; dir++)
            vn+=eval(num_face,dir)*normales(num_face,dir);


          vn/=surface(num_face);
          val(num_face) = vn;
        }
    }
  return *this;
}


DoubleVect& Champ_Face_PolyMAC::valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const
{
  throw;
  // return Champ_implementation_RT0::valeur_a_elem(position,result,poly);
}

double Champ_Face_PolyMAC::valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const
{
  throw;
  //return Champ_implementation_RT0::valeur_a_elem_compo(position,poly,ncomp);
}

//tableaux de correspondance pour les CLs
void Champ_Face_PolyMAC::init_fcl() const
{
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  const Conds_lim& cls = zone_Cl_dis().les_conditions_limites();
  int i, f, n;

  fcl_.resize(zone.nb_faces_tot(), 3);
  for (n = 0; n < cls.size(); n++)
    {
      const Front_VF& fvf = ref_cast(Front_VF, cls[n].frontiere_dis());
      int idx = sub_type(Neumann, cls[n].valeur())
                + 2 * sub_type(Navier, cls[n].valeur())
                + 3 * sub_type(Dirichlet, cls[n].valeur()) + 4 * sub_type(Dirichlet_homogene, cls[n].valeur());
      if (!idx) Cerr << "Champ_Face_PolyMAC : CL non codee rencontree!" << finl, Process::exit();
      for (i = 0; i < fvf.nb_faces_tot(); i++)
        f = fvf.num_face(i), fcl_(f, 0) = idx, fcl_(f, 1) = n, fcl_(f, 2) = i;
    }
  fcl_init_ = 1;
}

/* vitesse aux elements */
void Champ_Face_PolyMAC::interp_ve(const DoubleTab& inco, DoubleTab& val, bool is_vit) const
{
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  const DoubleTab& xv = zone.xv(), &xp = zone.xp();
  const DoubleVect& fs = zone.face_surfaces(), &pf = zone.porosite_face(), &pe = zone.porosite_elem(), &ve = zone.volumes();
  const IntTab &e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  int e, f, j, r;

  val = 0;
  for (e = 0; e < val.dimension(0); e++) for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
        {
          const double coef = is_vit ? pf(f) / pe(e) : 1.0;
          for (r = 0; r < dimension; r++) val(e, r) += fs(f) / ve(e) * (xv(f, r) - xp(e, r)) * (e == f_e(f, 0) ? 1 : -1) * inco(f) * coef;
        }
}

/* vitesse aux elements sur une liste d'elements */
void Champ_Face_PolyMAC::interp_ve(const DoubleTab& inco, const IntVect& les_polys, DoubleTab& val, bool is_vit) const
{
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  const DoubleTab& xv = zone.xv(), &xp = zone.xp();
  const DoubleVect& fs = zone.face_surfaces(), &pf = zone.porosite_face(), &pe = zone.porosite_elem(), &ve = zone.volumes();
  const IntTab &e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  int e, f, j, r;

  for (int poly = 0; poly < les_polys.size(); poly++)
    {
      e = les_polys(poly);
      if (e!=-1)
        {
          for (r = 0; r < dimension; r++) val(e, r) = 0;
          for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
            {
              const double coef = is_vit ? pf(f) / pe(e) : 1.0;
              for (r = 0; r < dimension; r++) val(e, r) += fs(f) / ve(e) * (xv(f, r) - xp(e, r)) * (e == f_e(f, 0) ? 1 : -1) * inco(f) * coef;
            }
        }
    }
}

DoubleTab& Champ_Face_PolyMAC::valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  int nb_compo=cha.nb_comp();
  assert(val.line_size() == nb_compo);
  // XXX : TODO Check this assert (positions and not val)
  assert((positions.dimension(0) == les_polys.size())||(positions.dimension_tot(0) == les_polys.size()));
  // assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));


  if (val.nb_dim() > 2)
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
  if (!ma_zone_cl_dis.non_nul()) return val;//on ne peut rien faire tant qu'on ne connait pas les CLs

  //on interpole ve sur tous les elements, puis on se restreint a les_polys
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  DoubleTrav ve(0, dimension);
  zone.zone().domaine().creer_tableau_elements(ve);
  bool is_vit = cha.le_nom().debute_par("vitesse");
  interp_ve(cha.valeurs(), ve, is_vit);
  for (int p = 0; p < les_polys.size(); p++) for (int r = 0, e = les_polys(p); e < zone.nb_elem() && r < dimension; r++) val(p, r) = (e==-1) ? 0. : ve(e, r);
  return val;
}

DoubleVect& Champ_Face_PolyMAC::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& val, int ncomp) const
{
  const Champ_base& cha=le_champ();
  assert(val.size() == polys.size());

  if (!ma_zone_cl_dis.non_nul()) return val;//on ne peut rien faire tant qu'on ne connait pas les CLs

  //on interpole ve sur tous les elements, puis on se restreint a les_polys
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  DoubleTrav ve(0, dimension);
  zone.zone().domaine().creer_tableau_elements(ve);
  interp_ve(cha.valeurs(), ve);

  for (int p = 0; p < polys.size(); p++) val(p) = (polys(p) == -1) ? 0. : ve(polys(p), ncomp);

  return val;
}

DoubleTab& Champ_Face_PolyMAC::remplir_coord_noeuds(DoubleTab& positions) const
{

  throw;
  // return Champ_implementation_RT0::remplir_coord_noeuds(positions);
}

int Champ_Face_PolyMAC::remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const
{

  throw;
  //  return Champ_implementation_RT0::remplir_coord_noeuds_et_polys(positions,polys);
}

DoubleTab& Champ_Face_PolyMAC::valeur_aux_faces(DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  int nb_compo=cha.nb_comp();

  if (nb_compo == 1)
    {
      Cerr<<"Champ_Face_PolyMAC::valeur_aux_faces"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }

  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
  val.resize(zone.nb_faces(), dimension), val = 0;

  for (int f = 0; f < zone.nb_faces(); f++)
    for (int r = 0; r < dimension; r++) val(f, r) = cha.valeurs()(f) * zone.face_normales(f, r) / zone.face_surfaces(f);
  return val;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//Methode qui renvoie SMA_barre aux elements a partir de la vitesse aux faces
//SMA_barre = Sij*Sij (sommation sur les indices i et j)
////////////////////////////////////////////////////////////////////////////////////////////////////

DoubleTab& Champ_Face_PolyMAC::trace(const Frontiere_dis_base& fr, DoubleTab& x, double t, int distant) const
{
  assert(distant==0);
  const bool vectoriel = (le_champ().nb_comp() > 1);
  const int dim = vectoriel ? dimension : 1;
  const Front_VF& fr_vf = ref_cast(Front_VF, fr);
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,zone_vf());
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
              for (int d = 0; d < dim; d++) x(i, d) = vectoriel ? ve(elem, d) : val[face];
            }
        }
    }

  return x;
}

int Champ_Face_PolyMAC::nb_valeurs_nodales() const
{
  ConstDoubleTab_parts val_part(valeurs());
  return val_part[0].dimension(0) + val_part[1].dimension(0);
}
