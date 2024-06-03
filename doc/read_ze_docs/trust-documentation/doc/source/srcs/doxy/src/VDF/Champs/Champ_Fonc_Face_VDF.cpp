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

#include <Champ_Uniforme_Morceaux.h>
#include <Champ_Fonc_Face_VDF.h>
#include <Champ_Uniforme.h>
#include <Champ_Don_lu.h>

Implemente_instanciable(Champ_Fonc_Face_VDF, "Champ_Fonc_Face|Champ_Fonc_Face_VDF", Champ_Fonc_base);

Sortie& Champ_Fonc_Face_VDF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Fonc_Face_VDF::readOn(Entree& s) { return s; }

int Champ_Fonc_Face_VDF::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  assert(nb_noeuds == domaine_vdf().nb_faces());
  const MD_Vector& md = domaine_vdf().md_vector_faces();
  // HACK (le meme que dans Champ_Face_VDF.cpp)
  int old_nb_comp = nb_compo_;
  nb_compo_ = 1;
  creer_tableau_distribue(md);
  nb_compo_ = old_nb_comp;
  return nb_noeuds;
}

void Champ_Fonc_Face_VDF::mettre_a_jour(double t)
{
  Champ_Fonc_base::mettre_a_jour(t);
}

DoubleTab& Champ_Fonc_Face_VDF::trace(const Frontiere_dis_base& fr, DoubleTab& x, double tps, int distant) const
{
  return Champ_Face_VDF_implementation::trace(fr, valeurs(), x, distant);
}

int Champ_Fonc_Face_VDF::imprime(Sortie& os, int ncomp) const
{
  imprime_Face(os, ncomp);
  return 1;
}

Champ_base& Champ_Fonc_Face_VDF::affecter_(const Champ_base& ch)
{
  const DoubleTab& v = ch.valeurs();
  DoubleTab& val = valeurs();
  const Domaine_VDF& domaine_VDF = domaine_vdf();
  int nb_faces = domaine_VDF.nb_faces();
  const IntVect& orientation = domaine_VDF.orientation();
  int ori, n0, n1;

  if (sub_type(Champ_Uniforme, ch))
    {
      for (int num_face = 0; num_face < nb_faces; num_face++)
        val(num_face) = v(0, orientation(num_face));
    }
  else if ((sub_type(Champ_Uniforme_Morceaux, ch)) || (sub_type(Champ_Don_lu, ch)))
    {
      int ndeb_int = domaine_VDF.premiere_face_int();
      const IntTab& face_voisins = domaine_VDF.face_voisins();
      int num_face;

      for (num_face = 0; num_face < ndeb_int; num_face++)
        {
          ori = orientation(num_face);
          n0 = face_voisins(num_face, 0);
          if (n0 != -1)
            val(num_face) = v(n0, ori);
          else
            val(num_face) = v(face_voisins(num_face, 1), ori);
        }

      for (num_face = ndeb_int; num_face < nb_faces; num_face++)
        {
          ori = orientation(num_face);
          n0 = face_voisins(num_face, 0);
          n1 = face_voisins(num_face, 1);
          val(num_face) = 0.5 * (v(n0, ori) + v(n1, ori));
        }
    }
  else
    {
      //      int ndeb_int = domaine_VDF.premiere_face_int();
      //      const IntTab& face_voisins = domaine_VDF.face_voisins();
      DoubleTab positionX(domaine_VDF.nb_faces_X(), dimension);
      DoubleVect U(domaine_VDF.nb_faces_X());
      DoubleTab positionY(domaine_VDF.nb_faces_Y(), dimension);
      DoubleVect V(domaine_VDF.nb_faces_Y());
      DoubleTab positionZ(domaine_VDF.nb_faces_Z(), dimension);
      DoubleVect W(domaine_VDF.nb_faces_Z());
      const DoubleTab& xv = domaine_VDF.xv();
      int nbx = 0;
      int nby = 0;
      int nbz = 0;
      int num_face, k;

      for (num_face = 0; num_face < nb_faces; num_face++)
        {
          ori = orientation(num_face);
          switch(ori)
            {
            case 0:
              for (k = 0; k < dimension; k++)
                positionX(nbx, k) = xv(num_face, k);
              nbx++;
              break;
            case 1:
              for (k = 0; k < dimension; k++)
                positionY(nby, k) = xv(num_face, k);
              nby++;
              break;
            case 2:
              for (k = 0; k < dimension; k++)
                positionZ(nbz, k) = xv(num_face, k);
              nbz++;
              break;
            }
        }

      ch.valeur_aux_compo(positionX, U, 0);
      ch.valeur_aux_compo(positionY, V, 1);
      if (dimension == 3)
        ch.valeur_aux_compo(positionZ, W, 2);
      nbx = nby = nbz = 0;
      for (num_face = 0; num_face < nb_faces; num_face++)
        {
          ori = orientation(num_face);
          switch(ori)
            {
            case 0:
              val(num_face) = U(nbx++);
              break;
            case 1:
              val(num_face) = V(nby++);
              break;
            case 2:
              val(num_face) = W(nbz++);
              break;
            }
        }
    }
  return *this;
}

DoubleVect& Champ_Fonc_Face_VDF::valeur_aux_compo(const DoubleTab& positions, DoubleVect& tab_valeurs, int ncomp) const
{
  const Domaine& madomaine = domaine_dis_base().domaine();
  IntVect les_polys(positions.dimension(0));
  madomaine.chercher_elements(positions, les_polys);
  return valeur_aux_elems_compo(positions, les_polys, tab_valeurs, ncomp);
}
