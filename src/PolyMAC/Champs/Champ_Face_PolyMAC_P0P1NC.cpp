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

#include <Linear_algebra_tools_impl.h>
#include <Connectivite_som_elem.h>
#include <Champ_Fonc_reprise.h>
#include <Champ_Face_PolyMAC_P0P1NC.h>
#include <Schema_Temps_base.h>
#include <Champ_Uniforme.h>
#include <TRUSTTab_parts.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Matrix_tools.h>
#include <Domaine_PolyMAC_P0P1NC.h>
#include <Symetrie.h>
#include <EChaine.h>
#include <Domaine.h>
#include <Domaine_VF.h>
#include <array>
#include <cmath>
#include <Operateur.h>
#include <Op_Diff_PolyMAC_P0P1NC_base.h>
#include <MD_Vector_base.h>

Implemente_instanciable(Champ_Face_PolyMAC_P0P1NC,"Champ_Face_PolyMAC_P0P1NC", Champ_Face_PolyMAC) ;

Sortie& Champ_Face_PolyMAC_P0P1NC::printOn(Sortie& os) const { return os << que_suis_je() << " " << le_nom(); }

Entree& Champ_Face_PolyMAC_P0P1NC::readOn(Entree& is) { return is; }

int Champ_Face_PolyMAC_P0P1NC::fixer_nb_valeurs_nodales(int n)
{
  // j'utilise le meme genre de code que dans Champ_Fonc_P0_base sauf que je recupere le nombre de faces au lieu du nombre d'elements
  // je suis tout de meme etonne du code utilise dans Champ_Fonc_P0_base::fixer_nb_valeurs_nodales() pour recuperer le domaine discrete...

  assert(n == domaine_PolyMAC_P0P1NC().nb_faces() || n < 0); //on accepte a la fois les conventions VEF et VDF

  // Probleme: nb_comp vaut 2 mais on ne veut qu'une dimension !!!
  // HACK :
  const int old_nb_compo = nb_compo_;
  if(nb_compo_ >= dimension) nb_compo_ /= dimension;

  creer_tableau_distribue(domaine_PolyMAC_P0P1NC().md_vector_faces());
  nb_compo_ = old_nb_compo;
  return n;
}

void Champ_Face_PolyMAC_P0P1NC::init_auxiliary_variables()
{
  for (int n = 0; n < nb_valeurs_temporelles(); n++)
    if (futur(n).size_reelle_ok())
      {
        DoubleTab& vals = futur(n);
        vals.set_md_vector(MD_Vector()); //on enleve le MD_Vector...
        vals.resize_dim0(domaine_PolyMAC_P0P1NC().mdv_faces_aretes.valeur().get_nb_items_tot()); //...on dimensionne a la bonne taille...
        vals.set_md_vector(domaine_PolyMAC_P0P1NC().mdv_faces_aretes); //...et on remet le bon MD_Vector
      }
}

int Champ_Face_PolyMAC_P0P1NC::reprendre(Entree& fich)
{
  const Domaine_PolyMAC_P0P1NC* domaine = le_dom_VF.non_nul() ? &ref_cast( Domaine_PolyMAC_P0P1NC,le_dom_VF.valeur()) : NULL;
  valeurs().set_md_vector(MD_Vector()); //on enleve le MD_Vector...
  valeurs().resize(0);
  int ret = Champ_Inc_base::reprendre(fich);
  //et on met le bon
  if (domaine) valeurs().set_md_vector(valeurs().dimension_tot(0) > domaine->nb_faces_tot() ? domaine->mdv_faces_aretes : domaine->md_vector_faces());
  return ret;
}

/* vitesse aux elements */
void Champ_Face_PolyMAC_P0P1NC::interp_ve(const DoubleTab& inco, DoubleTab& val, bool is_vit) const
{
  const Domaine_PolyMAC_P0P1NC& domaine = domaine_PolyMAC_P0P1NC();
  const DoubleTab& xv = domaine.xv(), &xp = domaine.xp();
  const DoubleVect& fs = domaine.face_surfaces(), *ppf = mon_equation_non_nul() ? &equation().milieu().porosite_face() : NULL, *ppe = ppf ? &equation().milieu().porosite_elem() : NULL, &ve = domaine.volumes();
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  int e, f, j, r;

  val = 0;
  for (e = 0; e < val.dimension(0); e++)
    for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
      {
        const double coef = is_vit && ppf ? (*ppf)(f) / (*ppe)(e) : 1.0;
        for (r = 0; r < dimension; r++) val(e, r) += fs(f) / ve(e) * (xv(f, r) - xp(e, r)) * (e == f_e(f, 0) ? 1 : -1) * inco(f) * coef;
      }
}

/* vitesse aux elements sur une liste d'elements */
void Champ_Face_PolyMAC_P0P1NC::interp_ve(const DoubleTab& inco, const IntVect& les_polys, DoubleTab& val, bool is_vit) const
{
  const Domaine_PolyMAC_P0P1NC& domaine = domaine_PolyMAC_P0P1NC();
  const DoubleTab& xv = domaine.xv(), &xp = domaine.xp();
  const DoubleVect& fs = domaine.face_surfaces(), *ppf = mon_equation_non_nul() ? &equation().milieu().porosite_face() : NULL, *ppe = ppf ? &equation().milieu().porosite_elem() : NULL, &ve = domaine.volumes();
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  int e, f, j, d, D = dimension, n, N = inco.line_size();
  assert(ve.line_size() == N * D);

  for (int poly = 0; poly < les_polys.size(); poly++)
    if ((e = les_polys(poly)) != -1)
      {
        for (n = 0; n < N * D; n++) val(e, n) = 0;
        for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
          {
            const double coef = is_vit && ppf ? (*ppf)(f) / (*ppe)(e) : 1.0;
            for (d = 0; d < D; d++)
              for (n = 0; n < N; n++) val(e, N * d + n) += fs(f) / ve(e) * (xv(f, d) - xp(e, d)) * (e == f_e(f, 0) ? 1 : -1) * inco(f, n) * coef;
          }
      }
}

DoubleTab& Champ_Face_PolyMAC_P0P1NC::valeur_aux_faces(DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  int nb_compo=cha.nb_comp(), n, N = cha.valeurs().line_size(), d, D = dimension;

  if (nb_compo == 1)
    {
      Cerr<<"Champ_Face_PolyMAC_P0P1NC::valeur_aux_faces"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }

  const Domaine_PolyMAC_P0P1NC& domaine = domaine_PolyMAC_P0P1NC();
  val.resize(domaine.nb_faces(), N * D);

  for (int f = 0; f < domaine.nb_faces(); f++)
    for (d = 0; d < D; d++)
      for (n = 0; n < N; n++) val(f, N * d + n) = cha.valeurs()(f, n) * domaine.face_normales(f, d) / domaine.face_surfaces(f);
  return val;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//Methode qui renvoie SMA_barre aux elements a partir de la vitesse aux faces
//SMA_barre = Sij*Sij (sommation sur les indices i et j)
////////////////////////////////////////////////////////////////////////////////////////////////////

DoubleTab& Champ_Face_PolyMAC_P0P1NC::trace(const Frontiere_dis_base& fr, DoubleTab& x, double t, int distant) const
{
  assert(distant==0);
  const bool vectoriel = (le_champ().nb_comp() > 1);
  const DoubleTab& val = valeurs(t);
  int n, N = val.line_size(), d, D = dimension, dim = vectoriel ? D : 1;
  const Front_VF& fr_vf = ref_cast(Front_VF, fr);
  const IntTab& face_voisins = domaine_PolyMAC_P0P1NC().face_voisins();
  DoubleTrav ve(0, N * D);
  if (vectoriel)
    {
      domaine_PolyMAC_P0P1NC().domaine().creer_tableau_elements(ve);
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
              for (d = 0; d < dim; d++)
                for (n = 0; n < N; n++) x(i, N * d + n) = vectoriel ? ve(elem, N * d + n) : val(face, n);
            }
        }
    }

  return x;
}

int Champ_Face_PolyMAC_P0P1NC::nb_valeurs_nodales() const
{
  return domaine_PolyMAC_P0P1NC().nb_faces(); //on ignore les variables auxiliaires
}
