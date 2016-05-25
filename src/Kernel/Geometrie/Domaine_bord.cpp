/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Domaine_bord.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////
#include <Domaine_bord.h>
#include <Hexaedre.h>

Implemente_instanciable(Domaine_bord,"Domaine_bord",Domaine);

// Description: pour l'instant exit()
Entree& Domaine_bord::readOn(Entree& is)
{
  exit();
  return is;
}

// Description: pour l'instant exit()
Sortie& Domaine_bord::printOn(Sortie& os) const
{
  exit();
  return os;
}

// Description: construit le domaine en appelant extraire_domaine_bord()
// Precondition: le domaine doit etre vide.
void Domaine_bord::construire_domaine_bord(const Domaine& source, const Nom& nom_bord)
{
  domaine_source_ = source;
  bord_source_ = nom_bord;
  extraire_domaine_bord(source, nom_bord, *this, renum_som_);
}

// Description: renvoie une reference au domaine source
const Domaine& Domaine_bord::get_domaine_source() const
{
  return domaine_source_;
}

// Description: renvoie le nom du bord source
const Nom& Domaine_bord::get_nom_bord_source() const
{
  return bord_source_;
}

// Description: renvoie renum_som (pour chaque sommet du domaine_bord, indice
//  du meme sommet dans le domaine)
const ArrOfInt& Domaine_bord::get_renum_som() const
{
  return renum_som_;
}

// Description: methode pour convertir un type de face en type d'element
//  (a deplacer dans la classe Faces ?)
void type_face_to_type_elem(const Elem_geom_base& type_elem, const Type_Face& type_face, Motcle& type_elem_face)
{
  switch(type_face)
    {
    case vide_0D:
      type_elem_face = "??";
      break;
    case point_1D:
      type_elem_face = "??";
      break;
    case segment_2D:
      type_elem_face = "segment";
      break;
    case segment_2D_axi:
      type_elem_face = "segment";
      break;
    case triangle_3D:
      type_elem_face = "triangle";
      break;
    case quadrilatere_2D_axi:
      type_elem_face = "quadrangle_VEF";
      break;
    case quadrangle_3D:
      type_elem_face = (sub_type(Hexaedre,type_elem)?"rectangle":"quadrangle_VEF");
      break;
    case quadrangle_3D_axi:
      type_elem_face = "quadrangle_VEF";
      break;
    default:
      type_elem_face = "??";
    }
}

// Description: remplit le domaine "dest" avec les sommets et les faces du bord "nom_bord"
//  du domaine "src". Les sommets du domaine dest sont uniquement les sommets qui sont sur
//  une face du bord. Le tableau renum_som est dimensionne a dest.nb_som() et rempli comme
//  suit: renum_som[i] est l'indice dans le domaine "src" du sommet i du domaine "dest".
// Precondition: le domaine dest doit etre vide (ne pas contenir de zone).
void Domaine_bord::extraire_domaine_bord(const Domaine& src,
                                         const Nom& nom_bord,
                                         Domaine& dest,
                                         ArrOfInt& renum_som)
{
  if (Process::nproc() > 1)
    {
      Cerr << "extraire_domaine_bord in parallel: the domain created will not have a distributed structure\n"
           << " (this will be done one day... ask to B.Mathieu)" << finl;
    }

  const Zone& zone_src = src.zone(0);
  // Le domaine destination doit etre vide:
  assert(dest.nb_zones() == 0);
  // Initialisation du domaine et d'une zone:
  // On choisit un nom pour le domaine
  dest.nommer(src.le_nom() + Nom("_") + nom_bord);
  // On cree une zone
  Zone z;
  dest.add(z);
  Zone& zone_dest = dest.zone(0);
  zone_dest.associer_domaine(dest);
  // Type des elements de la zone:
  Motcle type_elem;
  type_face_to_type_elem(zone_src.type_elem().valeur(), zone_src.type_elem().valeur().type_face(), type_elem);
  zone_dest.type_elem().typer(type_elem);
  zone_dest.type_elem().valeur().associer_zone(zone_dest);

  const Frontiere& front = zone_src.frontiere(nom_bord);
  const int nb_faces = front.faces().nb_faces();
  const int nb_som_face = front.faces().nb_som_faces();
  const IntTab& faces_src = front.faces().les_sommets();
  IntTab& elem_dest = zone_dest.les_elems();
  elem_dest.resize(nb_faces, nb_som_face);
  renum_som.reset();
  renum_som.set_smart_resize(1);
  // renum_inverse: pour chaque sommet du domaine source, son indice dans le domaine destination:
  ArrOfInt renum_inverse(src.nb_som());
  renum_inverse= -1;
  int nb_som_dest = 0;
  int i, j;
  for (i = 0; i < nb_faces; i++)
    {
      for (j = 0; j < nb_som_face; j++)
        {
          const int som = faces_src(i, j);
          // Si le sommet n'a pas encore ete rencontre, lui donner un indice dans le domaine dest:
          if (renum_inverse[som] < 0)
            {
              renum_som.append_array(som);
              renum_inverse[som] = nb_som_dest++;
            }
          elem_dest(i, j) = renum_inverse[som];
        }
    }
  // Copie des sommets utilises dans le domaine destination
  DoubleTab& som_dest = dest.les_sommets();
  const DoubleTab& som_src = src.les_sommets();
  const int dim = som_src.dimension(1);
  som_dest.resize(nb_som_dest, dim);
  for (i = 0; i < nb_som_dest; i++)
    {
      const int som = renum_som[i];
      for (j = 0; j < dim; j++)
        som_dest(i, j) = som_src(som, j);
    }

  // A faire si besoin: initialiser le joint des sommets pour avoir les items communs,
  //  et autres si necessaire.
}
