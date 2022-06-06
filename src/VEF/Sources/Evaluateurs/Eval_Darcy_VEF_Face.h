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

#ifndef Eval_Darcy_VEF_Face_included
#define Eval_Darcy_VEF_Face_included

////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION class Eval_Darcy_VEF_Face
//
////////////////////////////////////////////////////////////////////////////

#include <Evaluateur_Source_VEF_Face.h>
#include <Modele_Permeabilite.h>
#include <Ref_Champ_Inc.h>
#include <Ref_Champ_Don_base.h>
#include <Champ_Don.h>

class Eval_Darcy_VEF_Face: public Evaluateur_Source_VEF_Face
{

public:

  inline Eval_Darcy_VEF_Face();
  inline Eval_Darcy_VEF_Face(const Eval_Darcy_VEF_Face& eval);
  inline void mettre_a_jour( ) override;
  inline double calculer_terme_source_standard(int ) const override;
  inline void calculer_terme_source_standard(int , DoubleVect& ) const override;
  inline double calculer_terme_source_non_standard(int ) const override;
  inline void calculer_terme_source_non_standard(int , DoubleVect& ) const override;
  inline void associer(const Champ_Don&);
  inline void associer(const Champ_Inc&);
  Modele_Permeabilite modK;
  inline double& getPorosite()
  {
    return porosite;
  }

protected:
  inline void calculer_terme_source(int , DoubleVect& , const DoubleVect&) const;
  REF(Champ_Inc) vitesse;
  REF(Champ_Don_base) diffusivite_;
  DoubleTab db_diffusivite;
  double porosite;
};


//
//   Fonctions inline de la classe Eval_Darcy_VEF_Face
//
inline Eval_Darcy_VEF_Face::Eval_Darcy_VEF_Face(): porosite(1)
{}

inline Eval_Darcy_VEF_Face::Eval_Darcy_VEF_Face(const Eval_Darcy_VEF_Face& eval)
  : Evaluateur_Source_VEF_Face(eval),porosite(1)
{
}

// Description:
// associe le champ de diffusivite
void Eval_Darcy_VEF_Face::associer(const Champ_Don& diffu)
{
  int nb_faces_tot = la_zone.valeur().nb_faces_tot();
  db_diffusivite.resize(nb_faces_tot,Array_base::NOCOPY_NOINIT);
  diffusivite_ = diffu.valeur();
  mettre_a_jour();
}

void Eval_Darcy_VEF_Face::associer(const Champ_Inc& v)
{
  vitesse = v;
}

inline void Eval_Darcy_VEF_Face::mettre_a_jour()
{
  int nb_faces_tot = la_zone.valeur().nb_faces_tot();
  if (sub_type(Champ_Uniforme,diffusivite_.valeur()))
    {
      db_diffusivite = diffusivite_.valeur()(0,0);
    }
  else
    {
      const DoubleTab& val_diff = diffusivite_.valeur().valeurs();
      const IntTab& face_vois = la_zone.valeur().face_voisins();
      const DoubleVect& volumes = la_zone.valeur().volumes();
      db_diffusivite = 0.;
      // Compute nu(fac)=(nu(elem1).vol(elem1)+nu(elem2).vol(elem2))/(vol(elem1)+vol(elem2))
      // Viscosity at face is the volume weighted average of viscosity on elements
      for (int fac=0; fac<nb_faces_tot; fac++)
        {
          int elem1 = face_vois(fac,0);
          int elem2 = face_vois(fac,1);
          double vol = 0;
          if (elem1!=-1)
            {
              db_diffusivite(fac) += val_diff(elem1)*volumes(elem1);
              vol += volumes(elem1);
            }
          if (elem2!=-1)
            {
              db_diffusivite(fac) += val_diff(elem2)*volumes(elem2);
              vol += volumes(elem2);
            }
          db_diffusivite(fac) /= vol;
        }
    }
}

// Compute -mu.psi.U/K.dvol
inline void Eval_Darcy_VEF_Face::calculer_terme_source(int num_face, DoubleVect& source, const DoubleVect& volumes) const
{
  int size=source.size();
  for (int i=0; i<size; i++)
    source(i) = -db_diffusivite(num_face)/modK->getK(porosite)*volumes(num_face)*porosite_surf(num_face)*(vitesse->valeurs()(num_face,i));
}

inline void Eval_Darcy_VEF_Face::calculer_terme_source_standard(int num_face, DoubleVect& source) const
{
  calculer_terme_source(num_face, source, volumes_entrelaces);
}
inline void Eval_Darcy_VEF_Face::calculer_terme_source_non_standard(int num_face, DoubleVect& source) const
{
  calculer_terme_source(num_face, source, volumes_entrelaces_Cl);
}
inline double Eval_Darcy_VEF_Face::calculer_terme_source_standard(int num_face) const
{
  Cerr << "Eval_Darcy_VEF_Face::calculer_terme_source_standard(int num_face) can't be used on a vector source term." << finl;
  Process::exit();
  return 0;
}
inline double Eval_Darcy_VEF_Face::calculer_terme_source_non_standard(int num_face) const
{
  Cerr << "Eval_Darcy_VEF_Face::calculer_terme_source_non_standard(int num_face) can't be used on a vector source term." << finl;
  Process::exit();
  return 0;
}

#endif
