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

#include <MD_Vector_tools.h>
#include <Frontiere.h>
#include <TRUSTTab.h>

Implemente_base(Frontiere,"Frontiere",Objet_U);


/*! @brief Lit les specification d'une frontiere a partir d'un flot d'entree.
 *
 *     On lit:
 *        le nom
 *        les faces
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Frontiere::readOn(Entree& is)
{
  is >> nom;
  return is >> les_faces;
}


/*! @brief Ecrit la frontiere sur un flot de sortie.
 *
 * On ecrit:
 *       le nom de la frontiere
 *       les faces
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Frontiere::printOn(Sortie& os) const
{
  os << nom << finl;
  return os << les_faces;
}

/*! @brief Associe la frontiere au domaine dont elle depend.
 *
 * @param (Domaine& une_domaine) le domaine a associee a la frontiere
 */
void Frontiere::associer_domaine(const Domaine& une_domaine)
{
  le_dom=une_domaine;
  les_faces.associer_domaine(une_domaine);
}

/*! @brief Donne un nom a la frontiere
 *
 * @param (Nom& name) le nom a donner a la frontiere
 */
void Frontiere::nommer(const Nom& name)
{
  nom=name;
}

/*! @brief Ajoute une (ou plusieurs) face(s) a la frontiere, la (les) face(s) est (sont) specifiee(s) par un tableau
 *
 *     contenant les numeros des sommets.
 *
 * @param (IntTab& sommets) tableau contenant les numeros des sommets des face a ajouter
 */
void Frontiere::ajouter_faces(const IntTab& sommets)
{
  les_faces.ajouter(sommets);
}

/*! @brief Type les faces de la frontiere.
 *
 * @param (Motcle& typ) le type (geometrique) des faces
 */
void Frontiere::typer_faces(const Motcle& typ)
{
  les_faces.typer(typ);
}

/*! @brief Type les faces de la frontiere
 *
 * @param (Type_Face& typ) le type (geometrique) des faces
 */
void Frontiere::typer_faces(const Type_Face& typ)
{
  les_faces.typer(typ);
}

/*! @brief Renvoie les sommets des faces de la frontiere
 *
 * @return (IntTab&) le tableau contenant les numeros des sommets des faces de la frontiere
 */
IntTab& Frontiere::les_sommets_des_faces()
{
  return les_faces.les_sommets();
}
/*! @brief Renvoie les sommets des faces de la frontiere
 *
 * @return (IntTab&) le tableau contenant les numeros des sommets des faces de la frontiere
 */
const IntTab& Frontiere::les_sommets_des_faces() const
{
  return les_faces.les_sommets();
}

/*! @brief Renumerote les noeuds (sommets) des faces.
 *
 * Le noeud de numero k devient le noeud de numero Les_Nums[k]
 *
 * @param (IntVect& Les_Nums) le vecteur de renumerotation le_nouveau_sommet[i] = Les_Nums[ancien_sommet[i]]
 */
void Frontiere::renum(const IntVect& Les_Nums)
{
  IntTab& les_sommets=faces().les_sommets();
  for(int i=0; i<les_sommets.dimension(0); i++)
    for(int j=0; j<les_sommets.dimension(1); j++)
      les_sommets(i,j)=Les_Nums[les_sommets(i,j)];
}

/*! @brief Renvoie la Domaine associee a la frontiere.
 *
 * (version const)
 *
 * @return (Domaine&) le domaine associee a la frontiere
 */
const Domaine& Frontiere::domaine() const
{
  return le_dom.valeur();
}

/*! @brief Renvoie la Domaine associee a la frontiere.
 *
 * @return (Domaine&) le domaine associee a la frontiere
 */
Domaine& Frontiere::domaine()
{
  return le_dom.valeur();
}


/*! @brief Ajoute les sommets (et faces) de la frontiere passee en parametre a l'objet (Frontiere).
 *
 * @param (Frontiere& front) la frontiere a "ajouter" a l'objet
 */
void Frontiere::add(const Frontiere& front)
{
  const Faces& a_ajouter=front.faces();
  int nbf1=les_faces.nb_faces();
  int nbf2=a_ajouter.nb_faces();
  //max to treat the case where my front is empty
  int nbs=std::max(les_faces.nb_som_faces(), a_ajouter.nb_som_faces());
  int face;
  les_faces.les_sommets().resize(nbf1+nbf2, nbs);
  for(face=0; face<nbf2; face++)
    for(int som=0; som<nbs; som++)
      les_faces.sommet(nbf1+face, som)=a_ajouter.sommet(face, som);

  if(a_ajouter.voisins().nb_dim() == 1)
    return;

  int nb_voisins = a_ajouter.voisins().dimension(1);
  les_faces.voisins().resize(nbf1+nbf2, nb_voisins);
  for(face=0; face<nbf2; face++)
    for(int voisin=0; voisin<nb_voisins; voisin++)
      les_faces.voisin(nbf1+face, voisin)=a_ajouter.voisin(face, voisin);
}

/*! @brief Cree un tableau ayant une "ligne" par face de cette frontiere Voir MD_Vector_tools::creer_tableau_distribue()
 *
 */
void Frontiere::creer_tableau_faces(Array_base& v, Array_base::Resize_Options opt) const
{
  const MD_Vector& md = les_faces.les_sommets().get_md_vector();
  MD_Vector_tools::creer_tableau_distribue(md, v, opt);
}

/*! @brief Renvoie la trace sur la frontiere du tableau aux elements y
 *
 */
void Frontiere::trace_elem_local(const DoubleTab& y, DoubleTab& x) const
{
  const int size = nb_faces(), nb_compo_ = y.line_size();

  // On dimensionne x si ce n'est pas fait
  if (x.size_array() == 0 && size != 0)
    x.resize(size, nb_compo_);
  else if (x.dimension(0) != size || nb_compo_ != x.line_size())
    {
      Cerr << "Call to Frontiere::trace_elem with a DoubleTab x not located on boundary faces or don't have the same number of components" << finl;
      Process::exit();
    }
  for (int i = 0; i < size; i++)
    {
      int elem = faces().voisin(i, 0);
      if (elem == -1)
        elem = faces().voisin(i, 1);

      for(int j = 0; j < nb_compo_; j++)
        x(i, j) = y(elem, j);
    }
}

/*! @brief Renvoie la trace sur la frontiere du tableau aux noeuds y
 *
 */
void Frontiere::trace_som_local(const DoubleTab& y, DoubleTab& x) const
{
  const IntTab& som_face = les_sommets_des_faces();
  const int size = nb_faces(), nb_compo_ = y.line_size();
  const int nsomfa = som_face.dimension(1);

  // On dimensionne x si ce n'est pas fait
  if (x.size_array() == 0 && size != 0)
    x.resize(size, nb_compo_);
  else if (x.dimension(0) != size || nb_compo_ != x.line_size())
    {
      Cerr << "Call to Frontiere::trace_elem with a DoubleTab x not located on boundary faces or don't have the same number of components" << finl;
      Process::exit();
    }

  for (int i = 0; i < size; i++)
    for(int j=0; j<nb_compo_; j++)
      {
        double s = 0.;
        for (int isom = 0; isom < nsomfa; isom++)
          s += y(som_face(i, isom), j);

        s /= nsomfa;
        x(i, j) = s;
      }
}

/*! @brief Renvoie la trace sur la frontiere du tableau aux faces y
 *
 */
void Frontiere::trace_face_local(const DoubleVect& y, DoubleVect& x) const
{
  Cerr << "Frontiere::trace_face(const DoubleVect& y, DoubleVect& x) const" << finl;
  Cerr << "not coded yet." << finl;
  Process::exit();
}

/*! @brief Renvoie la trace sur la frontiere du tableau aux faces y
 *
 */
void Frontiere::trace_face_local(const DoubleTab& y, DoubleTab& x) const
{
  int size = nb_faces(), M = y.line_size(), N = x.line_size();
  assert(x.dimension(0)==size);
  for (int i = 0; i < size; i++)
    {
      int face = num_premiere_face() + i;
      for (int n = 0; n < N; n++)
        x.addr()[N * i + n] = y.addr()[M * face + n];
    }
}

void Frontiere::trace_som_distant(const DoubleTab&, DoubleTab&) const
{
  Cerr<<que_suis_je()<<"::trace_som_distant not implemented "<<finl;
  Process::exit();
}

void Frontiere::trace_elem_distant(const DoubleTab&, DoubleTab&) const
{
  Cerr<<que_suis_je()<<"::trace_elem_distant not implemented "<<finl;
  Process::exit();
}
void Frontiere::trace_face_distant(const DoubleTab&, DoubleTab&) const
{
  Cerr<<que_suis_je()<<"::trace_face_distant not implemented "<<finl;
  Process::exit();
}
void Frontiere::trace_face_distant(const DoubleVect&, DoubleVect&) const
{
  Cerr<<que_suis_je()<<"::trace_face_distant not implemented "<<finl;
  Process::exit();
}
