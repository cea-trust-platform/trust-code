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
// File:        Zone_Cl_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Zones
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Zone_Cl_VEF_included
#define Zone_Cl_VEF_included



//
// .DESCRIPTION class Zone_Cl_VEF
//  Cette classe porte les tableaux qui servent a mettre en oeuvre
//  les condition aux limites dans la formulation VEF
//  volumes_entrelaces_Cl_ : volumes des polyedres entrelaces autour des faces
//                           non standard
//  normales_facettes_Cl_ : normales aux facettes situees dans des polyedres
//                          non standard
//  type_elem_Cl_: type des polyedres non standard. Le type_elem_Cl d'un element
//                 non standard depend de son nombre de faces de Dirichlet et de
//                 la position de ces faces dans l'element (voir Tri_VEF et Tetra_VEF)
//
// .SECTION voir aussi
// Zone_Cl_dis_base


#include <Zone_Cl_dis_base.h>

class Champ_Inc;
class Zone_VEF;

class Zone_Cl_VEF : public Zone_Cl_dis_base
{

  Declare_instanciable(Zone_Cl_VEF);

public :

  void associer(const Zone_VEF& );
  void completer(const Zone_dis& );
  //  void mettre_a_jour(double );
  virtual int initialiser(double temps);
  virtual void imposer_cond_lim(Champ_Inc&, double);

  virtual void remplir_volumes_entrelaces_Cl(const Zone_VEF& );
  void remplir_normales_facettes_Cl(const Zone_VEF& );
  void remplir_vecteur_face_facette_Cl(const Zone_VEF& );
  inline DoubleVect& volumes_entrelaces_Cl();
  inline const DoubleVect& volumes_entrelaces_Cl() const;
  inline const double& volumes_entrelaces_Cl(int ) const;
  inline double& volumes_entrelaces_Cl(int );

  inline DoubleTab& normales_facettes_Cl();
  inline const DoubleTab& normales_facettes_Cl() const;
  inline const double& normales_facettes_Cl(int ,int ,int ) const;
  inline double& normales_facettes_Cl(int ,int ,int );

  inline DoubleTab& vecteur_face_facette_Cl();
  inline const DoubleTab& vecteur_face_facette_Cl() const;


  inline int type_elem_Cl(int ) const;

  int nb_faces_sortie_libre() const;
  Zone_VEF& zone_VEF();
  const Zone_VEF& zone_VEF() const;

  int nb_bord_periodicite() const;

protected:

  // Attributs:

  DoubleVect volumes_entrelaces_Cl_;
  DoubleTab normales_facettes_Cl_;
  DoubleTab vecteur_face_facette_Cl_;
  IntVect type_elem_Cl_;
  int modif_perio_fait_;

  // Fonctions de creation des membres prives de la zone:

  //  void remplir_volumes_entrelaces_Cl(const Zone_VEF& );
  //  void remplir_normales_facettes_Cl(const Zone_VEF& );
  void remplir_type_elem_Cl(const Zone_VEF& );

};

//
// Fonctions inline de la classe Zone_Cl_VEF
//


inline DoubleVect& Zone_Cl_VEF::volumes_entrelaces_Cl()
{
  return volumes_entrelaces_Cl_;
}

inline const DoubleVect& Zone_Cl_VEF::volumes_entrelaces_Cl() const
{
  return volumes_entrelaces_Cl_;
}

inline const double& Zone_Cl_VEF::volumes_entrelaces_Cl(int i) const
{
  return volumes_entrelaces_Cl_[i];
}

inline double& Zone_Cl_VEF::volumes_entrelaces_Cl(int i)
{
  return volumes_entrelaces_Cl_[i];
}

inline DoubleTab& Zone_Cl_VEF::normales_facettes_Cl()
{
  return normales_facettes_Cl_;
}

inline const  DoubleTab& Zone_Cl_VEF::normales_facettes_Cl() const
{
  return normales_facettes_Cl_;
}

inline DoubleTab& Zone_Cl_VEF::vecteur_face_facette_Cl()
{
  return vecteur_face_facette_Cl_;
}

inline const  DoubleTab& Zone_Cl_VEF::vecteur_face_facette_Cl() const
{
  return vecteur_face_facette_Cl_;
}




inline const double& Zone_Cl_VEF::normales_facettes_Cl(int num_poly,
                                                       int num_fa7,
                                                       int ncomp) const
{
  return normales_facettes_Cl_(num_poly,num_fa7,ncomp);
}

inline double& Zone_Cl_VEF::normales_facettes_Cl(int num_poly,
                                                 int num_fa7,
                                                 int ncomp)
{
  return normales_facettes_Cl_(num_poly,num_fa7,ncomp);
}

inline int Zone_Cl_VEF::type_elem_Cl(int i) const
{
  return type_elem_Cl_[i];
}

#endif
