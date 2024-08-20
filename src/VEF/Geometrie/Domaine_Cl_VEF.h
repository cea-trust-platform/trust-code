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

#ifndef Domaine_Cl_VEF_included
#define Domaine_Cl_VEF_included

/*! @brief class Domaine_Cl_VEF Cette classe porte les tableaux qui servent a mettre en oeuvre
 *
 *   les condition aux limites dans la formulation VEF
 *   volumes_entrelaces_Cl_ : volumes des polyedres entrelaces autour des faces
 *                            non standard
 *   normales_facettes_Cl_ : normales aux facettes situees dans des polyedres
 *                           non standard
 *   type_elem_Cl_: type des polyedres non standard. Le type_elem_Cl d'un element
 *                  non standard depend de son nombre de faces de Dirichlet et de
 *                  la position de ces faces dans l'element (voir Tri_VEF et Tetra_VEF)
 *
 *
 * @sa Domaine_Cl_dis_base
 */

#include <Domaine_Cl_dis_base.h>
#include <Champ_Inc.h>

class Domaine_VEF;

class Domaine_Cl_VEF: public Domaine_Cl_dis_base
{
  Declare_instanciable(Domaine_Cl_VEF);
public:
  void associer(const Domaine_VEF&);
  void completer(const Domaine_dis&) override;
  int initialiser(double temps) override;
  void imposer_cond_lim(Champ_Inc&, double) override;

  virtual void remplir_volumes_entrelaces_Cl(const Domaine_VEF&);
  void remplir_normales_facettes_Cl(const Domaine_VEF&);
  void remplir_vecteur_face_facette_Cl(const Domaine_VEF&);

  inline DoubleVect& volumes_entrelaces_Cl() { return volumes_entrelaces_Cl_; }
  inline const DoubleVect& volumes_entrelaces_Cl() const { return volumes_entrelaces_Cl_; }
  inline const double& volumes_entrelaces_Cl(int i) const { return volumes_entrelaces_Cl_[i]; }
  inline double& volumes_entrelaces_Cl(int i) { return volumes_entrelaces_Cl_[i]; }
  inline DoubleTab& normales_facettes_Cl() { return normales_facettes_Cl_; }
  inline const DoubleTab& normales_facettes_Cl() const { return normales_facettes_Cl_; }
  inline const double& normales_facettes_Cl(int num_poly, int num_fa7, int ncomp) const { return normales_facettes_Cl_(num_poly, num_fa7, ncomp); }
  inline double& normales_facettes_Cl(int num_poly, int num_fa7, int ncomp) { return normales_facettes_Cl_(num_poly, num_fa7, ncomp); }
  inline DoubleTab& vecteur_face_facette_Cl() { return vecteur_face_facette_Cl_; }
  inline const DoubleTab& vecteur_face_facette_Cl() const { return vecteur_face_facette_Cl_; }
  inline int type_elem_Cl(int i) const { return type_elem_Cl_[i]; }
  inline const IntVect& type_elem_Cl() const { return type_elem_Cl_; }

  int nb_faces_sortie_libre() const;
  Domaine_VEF& domaine_vef();
  const Domaine_VEF& domaine_vef() const;

  int nb_bord_periodicite() const;

protected:
  DoubleVect volumes_entrelaces_Cl_;
  DoubleTab normales_facettes_Cl_, vecteur_face_facette_Cl_;
  IntVect type_elem_Cl_;
  int modif_perio_fait_ = 0;

  void remplir_type_elem_Cl(const Domaine_VEF&);
};

#endif /* Domaine_Cl_VEF_included */
