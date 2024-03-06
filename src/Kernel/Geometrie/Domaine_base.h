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

#ifndef Domaine_base_included
#define Domaine_base_included

#include <Sous_Domaine.h>
#include <TRUSTArrays.h>
#include <TRUST_Deriv.h>
#include <TRUSTList.h>
#include <Elem_geom.h>
#include <TRUSTTabs.h>
#include <TRUST_Ref.h>
#include <Raccords.h>
#include <Joints.h>
#include <Bords.h>
#include <Noms.h>

#include <Bords_Internes.h>
#include <Groupes_Faces.h>

class Domaine_dis;
class Probleme_base;

/*! @brief Base class for domains description.
 * This class holds all the data shared by all domains **and** not sensitive to the 32/64 bits configuration.
 *
 * @sa Domaine class which inherits from Domaine_base and is templatized on entity index size (32 / 64b)
 */
class Domaine_base : public Objet_U
{
  Declare_base_sans_constructeur(Domaine_base);

public:
  //
  // General
  //
  inline const Nom& le_nom() const override   {   return nom_; }
  inline void nommer(const Nom& nom) override {  nom_ = nom; }
  inline double epsilon() const { return epsilon_; }

  ///
  /// Joints
  ///
  inline int nb_joints() const { return mes_faces_joint_.nb_joints(); }

  inline Joint& joint(int i) {   return mes_faces_joint_(i); }
  inline const Joint& joint(int i) const { return mes_faces_joint_(i); }
  inline Joint& joint(const Nom& nom) {  return mes_faces_joint_(nom); }
  inline const Joint& joint(const Nom& nom) const {   return mes_faces_joint_(nom); }
  inline Joints& faces_joint() { return mes_faces_joint_; }
  inline const Joints& faces_joint() const {  return mes_faces_joint_; }

  inline Joint& joint_of_pe(int);
  inline const Joint& joint_of_pe(int) const;
  int comprimer_joints();

  void renum_joint_common_items(const IntVect& nums, const int elem_offset);

  //
  // Time-dependency
  //
  virtual void initialiser (double temps, Domaine_dis&, Probleme_base&) {}
  virtual void set_dt(double& dt_) {}
  virtual void mettre_a_jour(double temps, Domaine_dis&, Probleme_base&) {}
  virtual void update_after_post(double temps) {}

  //
  // Printing/export stuff
  //
  inline void associer_bords_a_imprimer(LIST(Nom) liste)     { bords_a_imprimer_=liste; }
  inline void associer_bords_a_imprimer_sum(LIST(Nom) liste) { bords_a_imprimer_sum_=liste; }
  Entree& lire_bords_a_imprimer(Entree& s) ;
  Entree& lire_bords_a_imprimer_sum(Entree& s) ;
  inline const LIST(Nom)& bords_a_imprimer() const { return bords_a_imprimer_; }
  inline const LIST(Nom)& bords_a_imprimer_sum() const { return bords_a_imprimer_sum_; }
  inline int  moments_a_imprimer() const  {  return moments_a_imprimer_;  }
  inline int& moments_a_imprimer() {  return moments_a_imprimer_;  }


  //
  // Various
  //
  inline bool axi1d() const {  return axi1d_;  }
  inline void fixer_epsilon(double eps)  { epsilon_=eps; }
  inline bool deformable() const  {   return deformable_;  }
  inline bool& deformable() {   return deformable_;  }
  inline void set_fichier_lu(Nom& nom)  {    fichier_lu_=nom;   }
  inline const Nom& get_fichier_lu() const  {   return fichier_lu_;  }

protected:
  /// Domaine name
  Nom nom_;

  // Les faces de joint sont les faces communes avec les autres processeurs (bords
  //  du domaine locale a ce processeur qui se raccordent a un processeur voisin)
  Joints mes_faces_joint_;

  int moments_a_imprimer_;
  LIST(Nom) bords_a_imprimer_;
  LIST(Nom) bords_a_imprimer_sum_;

  int axi1d_;
  double epsilon_;
  bool deformable_;
  Nom fichier_lu_;

  void duplique_bords_internes();

  /// Volume total du domaine (somme sur tous les processeurs)
  double volume_total_;
};


inline const Joint& Domaine_base::joint_of_pe(int pe) const
{
  int i;
  for(i=0; i<nb_joints(); i++)
    if(mes_faces_joint_(i).PEvoisin()==pe)
      break;
  return mes_faces_joint_(i);
}

inline Joint& Domaine_base::joint_of_pe(int pe)
{
  int i;
  for(i=0; i<nb_joints(); i++)
    if(mes_faces_joint_(i).PEvoisin()==pe)
      break;
  return mes_faces_joint_(i);
}


#endif
