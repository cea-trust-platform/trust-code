/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Frontiere_included
#define Frontiere_included

#include <Faces.h>

/*! @brief Classe Frontiere.
 *
 * Une Frontiere decrit une partie de la frontiere d'un Domaine,
 *     elle possede un Nom, elle porte des Faces et a un Domaine associe.
 *     Une Frontiere peut etre specialisee en un Bord, un Joint un
 *     Raccord ou une Face Interne.
 *
 * @sa Domaine Bord Joint Raccord Faces_Interne
 */
class Frontiere : public Objet_U
{
  Declare_base(Frontiere);

public:

  // Entree& lire(Entree&);
  // Sortie& ecrire(Sortie&) const;
  void associer_domaine(const Domaine&);
  const Domaine& domaine() const;
  Domaine& domaine();
  void nommer(const Nom&) override;
  void ajouter_faces(const IntTab&);
  void typer_faces(const Motcle&);
  void typer_faces(const Type_Face&);
  inline const Faces& faces() const;
  inline Faces& faces();
  IntTab& les_sommets_des_faces();
  const IntTab& les_sommets_des_faces() const;
  inline int nb_faces() const;
  inline void dimensionner(int);
  inline const Nom& le_nom() const override;
  void renum(const IntVect&);
  void add(const Frontiere& );
  inline int nb_faces_virt() const;
  inline int face_virt(int i) const;

  inline int num_premiere_face() const;
  inline void fixer_num_premiere_face(int );
  inline const ArrOfInt& get_faces_virt() const;
  inline ArrOfInt& get_faces_virt();
  inline const double& get_aire() const { return aire_; }
  inline void set_aire(double& aire) { aire_ = aire; }

  virtual void creer_tableau_faces(Array_base&, RESIZE_OPTIONS opt = RESIZE_OPTIONS::COPY_INIT) const;
  virtual void trace_elem_local(const DoubleTab&, DoubleTab&) const;
  virtual void trace_face_local(const DoubleTab&, DoubleTab&) const;
  virtual void trace_face_local(const DoubleVect&, DoubleVect&) const;
  virtual void trace_elem_distant(const DoubleTab&, DoubleTab&) const;
  virtual void trace_som_distant(const DoubleTab&, DoubleTab&) const;
  virtual void trace_face_distant(const DoubleTab&, DoubleTab&) const;
  virtual void trace_face_distant(const DoubleVect&, DoubleVect&) const;
  virtual void trace_som_local(const DoubleTab& y, DoubleTab& x) const;
private :

  Nom nom;
  Faces les_faces;
  REF(Domaine) le_dom;
  ArrOfInt faces_virt;
  int num_premiere_face_ = -100;
  double aire_ = -100.;
};


/*! @brief Renvoie le nombre de faces de la frontiere.
 *
 * @return (int) le nombre de faces de la frontiere
 */
inline int Frontiere::nb_faces() const
{
  return les_faces.nb_faces();
}


/*! @brief Dimensionne la frontiere, i.
 *
 * e. fixe son nombre de faces.
 *
 * @param (int i) le nombre de faces a donner a la frontiere
 */
inline void Frontiere::dimensionner(int i)
{
  les_faces.dimensionner(i);
}


/*! @brief Renvoie le nom de la frontiere.
 *
 * @return (Nom&) le nom de la frontiere
 */
inline const Nom& Frontiere::le_nom() const
{
  return nom;
}

/*! @brief Renvoie les faces de la frontiere (version const)
 *
 * @return (Faces&) les faces de la frontiere
 */
inline const Faces& Frontiere::faces() const
{
  return les_faces;
}

/*! @brief Renvoie les faces de la frontiere
 *
 * @return (Faces&) les faces de la frontiere
 */
inline Faces& Frontiere::faces()
{
  return les_faces;
}

inline int Frontiere::nb_faces_virt() const
{
  return faces_virt.size_array();
}
inline int Frontiere::face_virt(int i) const
{
  return faces_virt[i];
}

inline int Frontiere::num_premiere_face() const
{
  return num_premiere_face_ ;
}
inline void Frontiere::fixer_num_premiere_face(int i)
{
  num_premiere_face_ = i;
}

inline const ArrOfInt& Frontiere::get_faces_virt() const
{
  return faces_virt;
}

inline ArrOfInt& Frontiere::get_faces_virt()
{
  return faces_virt;
}
#endif
