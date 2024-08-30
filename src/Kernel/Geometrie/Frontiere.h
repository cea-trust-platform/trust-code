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
template <typename _SIZE_>
class Frontiere_32_64 : public Objet_U
{
  Declare_base_32_64(Frontiere_32_64);

public:
  using int_t = _SIZE_;
  using ArrOfInt_t = ArrOfInt_T<_SIZE_>;
  using IntVect_t = IntVect_T<_SIZE_>;
  using IntTab_t = IntTab_T<_SIZE_>;
  using DoubleVect_t = DoubleVect_T<_SIZE_>;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;
  using Domaine_t = Domaine_32_64<_SIZE_>;
  using Faces_t = Faces_32_64<_SIZE_>;


  void associer_domaine(const Domaine_t&);
  const Domaine_t& domaine() const;
  Domaine_t& domaine();
  inline const Nom& le_nom() const override {  return nom; }
  void nommer(const Nom&) override;
  void ajouter_faces(const IntTab_t&);
  void typer_faces(const Motcle&);
  void typer_faces(const Type_Face&);
  inline const Faces_t& faces() const { return les_faces; }
  inline Faces_t& faces() {  return les_faces; }
  IntTab_t& les_sommets_des_faces();
  const IntTab_t& les_sommets_des_faces() const;
  /// Renvoie le nombre de faces de la frontiere.
  inline int_t nb_faces() const  {  return les_faces.nb_faces(); }
  /// Dimensionne la frontiere, i.e. fixe son nombre de faces.
  inline void dimensionner(int_t i) {  les_faces.dimensionner(i); }
  void renum(const IntVect_t&);
  void add(const Frontiere_32_64& );
  inline int_t nb_faces_virt() const  { return faces_virt.size_array(); }
  inline int_t face_virt(int_t i) const { return faces_virt[i]; }

  inline int_t num_premiere_face() const { return num_premiere_face_ ;  }
  inline void fixer_num_premiere_face(int_t i) { num_premiere_face_ = i; }
  inline const ArrOfInt_t& get_faces_virt() const { return faces_virt; }
  inline ArrOfInt_t& get_faces_virt() {  return faces_virt; }

  inline const double& get_aire() const { return aire_; }
  inline void set_aire(double& aire) { aire_ = aire; }

  virtual void creer_tableau_faces(Array_base&, RESIZE_OPTIONS opt = RESIZE_OPTIONS::COPY_INIT) const;

  // Various trace methods - only used by the Champ_xx classes - no need for a 64b version, so stick with DoubleTab, not DoubleTab_t:
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
  Faces_t les_faces;
  REF(Domaine_t) le_dom;
  ArrOfInt_t faces_virt;
  int_t num_premiere_face_ = -100;
  double aire_ = -100.;
};


using Frontiere = Frontiere_32_64<int>;
using Frontiere_64 = Frontiere_32_64<trustIdType>;

#endif
