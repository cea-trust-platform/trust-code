/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Eval_DiffF22_VDF_const_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Diff
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_DiffF22_VDF_const_Elem_included
#define Eval_DiffF22_VDF_const_Elem_included

#include <Eval_Diff_VDF_const.h>
#include <Champ_Inc.h>
#include <Ref_Champ_Inc.h>
#include <Eval_Diff_VDF_Elem.h>

//
// .DESCRIPTION class Eval_DiffF22_VDF_const_Elem
//
// Evaluateur VDF pour la diffusion dans l'equation de F22 (modele V2F)
// Le champ diffuse est scalaire (Champ_P0_VDF)
// Le champ de diffusivite est constant.

//
// .SECTION voir aussi Eval_DiffF22_VDF_const


class Eval_DiffF22_VDF_const_Elem : public Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>,
  public Eval_Diff_VDF_const
{
public:
  inline void associer_keps(const Champ_Inc& keps, const Champ_Inc& champv2)
  {
    KEps = keps;
    v2 = champv2;
  }

private:
  REF(Champ_Inc) KEps;
  REF(Champ_Inc) v2;
};

//// calculer_flux_faces_echange_externe_impose
//
template<>
inline int Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::calculer_flux_faces_echange_externe_impose() const
{
  return 0;
}

//// calculer_flux_faces_echange_global_impose
//
template<>
inline int Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::calculer_flux_faces_echange_global_impose() const
{
  return 0;
}

//// calculer_flux_faces_paroi
//
template<>
inline int Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::calculer_flux_faces_paroi() const
{
  return 0;
}

//// calculer_flux_faces_sortie_libre
//
template<>
inline int Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::calculer_flux_faces_sortie_libre() const
{
  return 1;
}

//// calculer_flux_faces_symetrie
//
template<>
inline int Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::calculer_flux_faces_symetrie() const
{
  return 1;
}

//// calculer_flux_faces_periodique
//
template<>
inline int Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::calculer_flux_faces_periodique() const
{
  return 0;
}

//// flux_face avec Dirichlet_entree_fluide
//
template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::flux_face(const DoubleTab& inco, int face,
                                                                         const Dirichlet_entree_fluide& la_cl,
                                                                         int num1) const
{
  Cerr << " si je suis ici en flux, ce n'est pas bon " << finl ;
  return 0;
}

//// coeffs_face avec Dirichlet_entree_fluide
//
template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int face, int num1,
                                                                         const Dirichlet_entree_fluide& la_cl,
                                                                         double& aii, double& ajj) const
{
  aii = ajj = 0.;
}

//// secmem_face avec Dirichlet_entree_fluide
//
template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,
                                                                           int num1) const
{
  return 0;
}

//// coeffs_face avec Dirichlet_paroi_fixe
//
#ifdef DOXYGEN_SHOULD_SKIP_THIS
template<>template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int face, int num1,
                                                                         const Dirichlet_paroi_fixe& la_cl,
                                                                         double& aii, double& ajj) const
{
  int i = elem_(face,0);
  double dist = dist_norm_bord(face);

  if (i != -1)
    {
      aii = porosite(face)*surface(face)/dist;
      ajj = 0;
    }
  else   // j != -1
    {
      ajj = porosite(face)*surface(face)/dist;
      aii = 0;
    }
}
#endif
//// flux_face avec Echange_externe_impose
//
template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                                                         const Echange_externe_impose& la_cl,
                                                                         int num1) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// coeffs_face avec Echange_externe_impose
//
template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                                                         const Echange_externe_impose& la_cl,
                                                                         double& aii, double& ajj) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return ;
}

//// secmem_face avec Echange_externe_impose
//
template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::secmem_face(int boundary_index, int face, int local_face, const Echange_externe_impose& la_cl,
                                                                           int num1) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// flux_face avec Echange_global_impose
//
template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::flux_face(const DoubleTab& inco, int face,
                                                                         const Echange_global_impose& la_cl,
                                                                         int num1) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// coeffs_face avec Echange_global_impose
//
template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int face, int num1,
                                                                         const Echange_global_impose& la_cl,
                                                                         double& aii, double& ajj) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return ;
}

//// secmem_face avec Echange_global_impose
//
template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::secmem_face(int face, const Echange_global_impose& la_cl,
                                                                           int num1) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// flux_face avec Neumann_paroi
//
template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::flux_face(const DoubleTab& , int face,
                                                                         const Neumann_paroi& la_cl,
                                                                         int num1) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// coeffs_face avec Neumann_paroi
//
template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int face, int num1,
                                                                         const Neumann_paroi& la_cl,
                                                                         double& aii, double& ajj) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return ;
}

//// secmem_face avec Neumann_paroi
//
template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::secmem_face(int face, const Neumann_paroi& la_cl,
                                                                           int num1) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// coeffs_face avec Neumann_sortie_libre
//
#ifdef DOXYGEN_SHOULD_SKIP_THIS
template<>template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int face, int num1,
                                                                         const Neumann_sortie_libre& la_cl,
                                                                         double& aii, double& ajj) const
{
  aii = ajj = 0.;
}

//// coeffs_face avec Symetrie
//
template<>template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int face , int num1,
                                                                         const Symetrie& la_cl, double& aii, double& ajj) const
{
  aii = ajj = 0;
}

//// secmem_face avec Symetrie
//
template<>template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::secmem_face(int face, const Symetrie& la_cl, int num1) const
{
  return 0;
}
#endif
//// flux_face avec Periodique
//
template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::flux_face(const DoubleTab& inco,
                                                                         int face,
                                                                         const Periodique& la_cl,
                                                                         int ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

//// coeffs_face avec Periodique
template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int face, int num1,
                                                                         const Periodique& la_cl,
                                                                         double& aii, double& ajj) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return ;
}

//// flux_face avec NSCBC
//
template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::flux_face(const DoubleTab& , int ,
                                                                         const NSCBC& ,
                                                                         int ) const
{
  return 0;
}

//// coeffs_face avec NSCBC
//
template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int , int,
                                                                         const NSCBC& ,
                                                                         double&, double&) const
{
}

//// flux_faces_interne
//
template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::flux_faces_interne(const DoubleTab& inco, int face) const
{
  Cerr << " dans flux_faces_interne " << finl;
  return 0.;
}

//// coeffs_faces_interne
template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_faces_interne(int face,double& aii, double& ajj) const
{
  double dist = la_zone->dist_norm(face);
  aii = ajj =  porosite(face)*surface(face)/dist;
}

//// secmem_faces_interne
//
template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::secmem_faces_interne( int ) const
{
  return 0;
}

#endif /* Eval_DiffF22_VDF_const_Elem_included */
