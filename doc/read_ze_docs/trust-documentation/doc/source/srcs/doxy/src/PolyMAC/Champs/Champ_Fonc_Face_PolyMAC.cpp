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

#include <Champ_Fonc_Face_PolyMAC.h>
#include <Domaine_PolyMAC.h>
#include <Champ_Uniforme.h>
#include <TRUSTTab.h>
#include <Domaine.h>

Implemente_instanciable(Champ_Fonc_Face_PolyMAC,"Champ_Fonc_Face_PolyMAC",Champ_Fonc_base);

Sortie& Champ_Fonc_Face_PolyMAC::printOn(Sortie& os) const { return os << que_suis_je() << " " << le_nom(); }

Entree& Champ_Fonc_Face_PolyMAC::readOn(Entree& is) { return is; }

int Champ_Fonc_Face_PolyMAC::fixer_nb_valeurs_nodales(int n)
{
  // j'utilise le meme genre de code que dans Champ_Fonc_P0_base sauf que je recupere le nombre de faces au lieu du nombre d'elements
  // je suis tout de meme etonne du code utilise dans Champ_Fonc_P0_base::fixer_nb_valeurs_nodales() pour recuperer le domaine discrete...
  const Champ_Fonc_base& self = ref_cast(Champ_Fonc_base, *this);
  const Domaine_VF& le_dom_vf = ref_cast(Domaine_VF, self.domaine_dis_base());

  assert(n == le_dom_vf.nb_faces());

  const MD_Vector& md = le_dom_vf.md_vector_faces();
  // Probleme: nb_comp vaut 2 mais on ne veut qu'une dimension !!!
  // HACK :
  int old_nb_compo = nb_compo_;
  if(nb_compo_ >= dimension) nb_compo_ /= dimension;

  creer_tableau_distribue(md);
  nb_compo_ = old_nb_compo;
  return n;

}
Champ_base& Champ_Fonc_Face_PolyMAC::affecter_(const Champ_base& ch)
{
  const DoubleTab& v = ch.valeurs();
  DoubleTab& val = valeurs();
  const Domaine_VF& domaine_PolyMAC = ref_cast(Domaine_VF, le_dom_VF.valeur());
  int nb_faces = domaine_PolyMAC.nb_faces();
  const DoubleVect& surface = domaine_PolyMAC.face_surfaces();
  const DoubleTab& normales = domaine_PolyMAC.face_normales();

  if (sub_type(Champ_Uniforme, ch))
    {
      for (int num_face = 0; num_face < nb_faces; num_face++)
        {
          double vn = 0;
          for (int dir = 0; dir < dimension; dir++)
            vn += v(0, dir) * normales(num_face, dir);

          vn /= surface(num_face);
          val(num_face) = vn;
        }
    }
  else if (nb_compo_ == dimension)
    {
      const DoubleTab& xv = domaine_PolyMAC.xv();
      DoubleTab eval(val.dimension_tot(0), dimension);
      ch.valeur_aux(xv, eval);
      for (int num_face = 0; num_face < nb_faces; num_face++)
        {
          double vn = 0;
          for (int dir = 0; dir < dimension; dir++)
            vn += eval(num_face, dir) * normales(num_face, dir);

          vn /= surface(num_face);
          val(num_face) = vn;
        }
    }
  else if (nb_compo_ == 1)
    {
      const DoubleTab& xv = domaine_PolyMAC.xv();
      ch.valeur_aux(xv, val);
    }
  else
    abort();
  return *this;
}

DoubleVect& Champ_Fonc_Face_PolyMAC::valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const
{
  Cerr << "Champ_Fonc_Face_PolyMAC::" <<__func__ << " is not coded !" << finl;
  throw;
}

double Champ_Fonc_Face_PolyMAC::valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const
{
  Cerr << "Champ_Fonc_Face_PolyMAC::" <<__func__ << " is not coded !" << finl;
  throw;
}


void Champ_Fonc_Face_PolyMAC::interp_valeurs_elem(const DoubleTab& inco, DoubleTab& val) const
{
  const Domaine_PolyMAC& domaine = ref_cast(Domaine_PolyMAC,domaine_vf());
  const DoubleTab& xv = domaine.xv(), &xp = domaine.xp();
  const DoubleVect& fs = domaine.face_surfaces(), &ve = domaine.volumes();
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  int e, f, j, d, D = dimension, n, N = inco.line_size();
  assert(val.line_size() == N * D);

  val = 0;
  for (e = 0; e < val.dimension(0); e++)
    {
      for (n = 0; n < N * D; n++) val(e, n) = 0;
      for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
        for (d = 0; d < D; d++)
          for (n = 0; n < N; n++)
            val(e, N * d + n) += fs(f) / ve(e) * (xv(f, d) - xp(e, d)) * (e == f_e(f, 0) ? 1 : -1) * (inco.nb_dim() == 1 ? inco(f) : inco(f, n));
    }
}

DoubleTab& Champ_Fonc_Face_PolyMAC::valeur_aux_elems(const DoubleTab& positions,
                                                     const IntVect& les_polys,
                                                     DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  const Domaine_PolyMAC& domaine_VF = ref_cast(Domaine_PolyMAC, domaine_vf());
  int nb_compo=cha.nb_comp(), N = cha.valeurs().line_size(), D = dimension, nf_tot = domaine_VF.nb_faces_tot();
  assert(val.line_size() == nb_compo );
  // XXX : TODO Check this assert (positions and not val)
  assert((positions.dimension(0) == les_polys.size())||(positions.dimension_tot(0) == les_polys.size()));
  assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
  assert((cha.valeurs().dimension_tot(0)==nf_tot+D*domaine_VF.nb_elem_tot())||(cha.valeurs().dimension_tot(0)==nf_tot));

  if (val.nb_dim() > 2) Process::exit(que_suis_je() + "Le DoubleTab val a plus de 2 entrees");
  if (nb_compo == 1) Process::exit("TRUST error in Champ_Fonc_Face_PolyMAC::valeur_aux_elems : A scalar field cannot be of Champ_Face type !");

  DoubleTab ve(0, N * D);

  if (cha.valeurs().dimension_tot(0)==nf_tot)
    {
      domaine_VF.domaine().creer_tableau_elements(ve);
      interp_valeurs_elem(cha.valeurs(), ve);
      for (int p = 0; p < les_polys.size(); p++)
        for (int r = 0, e = les_polys(p); e < domaine_VF.nb_elem() && r < N * D; r++)
          val(p, r) = (e==-1) ? 0. : ve(e, r);
    }
  else if (cha.valeurs().dimension_tot(0)==nf_tot+D*domaine_VF.nb_elem_tot())
    for (int p = 0; p < les_polys.size(); p++)
      for (int e = les_polys(p), d = 0; e < domaine_VF.nb_elem() && d < D; d++)
        for (int n = 0; n < N; n++)
          val(p, N * d + n) = cha.valeurs()(nf_tot + D * e + d, n);
  else Process::exit("TRUST error in Champ_Fonc_Face_PolyMAC::valeur_aux_elems : curious number of faces !");

  return val;
}

DoubleTab& Champ_Fonc_Face_PolyMAC::valeur_aux_faces(DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  int nb_compo=cha.nb_comp(), n, N = cha.valeurs().line_size(), d, D = dimension;

  if (nb_compo == 1) Process::exit("TRUST error in Champ_Fonc_Face_PolyMAC::valeur_aux_faces : A scalar field cannot be of Champ_Face type !");

  const Domaine_VF& domaine = domaine_vf();
  val.resize(domaine.nb_faces(), N * D);

  for (int f = 0; f < domaine.nb_faces(); f++)
    for (d = 0; d < D; d++)
      for (n = 0; n < N; n++) val(f, N * d + n) = cha.valeurs()(f, n) * domaine.face_normales(f, d) / domaine.face_surfaces(f);
  return val;
}

DoubleVect& Champ_Fonc_Face_PolyMAC::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const
{
  Cerr << "Champ_Fonc_Face_PolyMAC::" <<__func__ << " is not coded !" << finl;
  throw;
}

DoubleTab& Champ_Fonc_Face_PolyMAC::remplir_coord_noeuds(DoubleTab& positions) const
{
  Cerr << "Champ_Fonc_Face_PolyMAC::" <<__func__ << " is not coded !" << finl;
  throw;
}

int Champ_Fonc_Face_PolyMAC::remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const
{
  Cerr << "Champ_Fonc_Face_PolyMAC::" <<__func__ << " is not coded !" << finl;
  throw;
}

DoubleTab& Champ_Fonc_Face_PolyMAC::valeur_aux_sommets(const Domaine& domain, DoubleTab& result) const
{
  Cerr << "Champ_Fonc_Face_PolyMAC::" <<__func__ << " is not coded !" << finl;
  throw;
}

DoubleVect& Champ_Fonc_Face_PolyMAC::valeur_aux_sommets_compo(const Domaine& domain, DoubleVect& result, int ncomp) const
{
  Cerr << "Champ_Fonc_Face_PolyMAC::" <<__func__ << " is not coded !" << finl;
  throw;
}

void Champ_Fonc_Face_PolyMAC::mettre_a_jour(double un_temps)
{
  return Champ_Fonc_base::mettre_a_jour(un_temps);
}
