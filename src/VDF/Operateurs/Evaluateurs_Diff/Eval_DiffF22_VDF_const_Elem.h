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
#include <Eval_Diff_VDF_Elem.h>
#include <Ref_Champ_Inc.h>
#include <Champ_Inc.h>

// .DESCRIPTION class Eval_DiffF22_VDF_const_Elem
// Evaluateur VDF pour la diffusion dans l'equation de F22 (modele V2F)
// Le champ diffuse est scalaire (Champ_P0_VDF)
// Le champ de diffusivite est constant.
// .SECTION voir aussi Eval_DiffF22_VDF_const
class Eval_DiffF22_VDF_const_Elem : public Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>, public Eval_Diff_VDF_const
{
public:
  // Overload Eval_VDF_Elem
  static constexpr bool CALC_FLUX_FACES_ECH_EXT_IMP = false, CALC_FLUX_FACES_ECH_GLOB_IMP = false, CALC_FLUX_FACES_PAR = false,
                        CALC_FLUX_FACES_SORTIE_LIB = true, CALC_FLUX_FACES_SYMM = true, CALC_FLUX_FACES_PERIO = false;

  inline void associer_keps(const Champ_Inc& keps, const Champ_Inc& champv2)
  {
    KEps = keps;
    v2 = champv2;
  }

private:
  REF(Champ_Inc) KEps;
  REF(Champ_Inc) v2;
};

template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::flux_face(const DoubleTab& , int , const Dirichlet_entree_fluide& , int ) const
{
  Cerr << " si je suis ici en flux, ce n'est pas bon " << finl ;
  return 0;
}

template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int , int ,  const Dirichlet_entree_fluide& , double& aii, double& ajj) const
{
  aii = ajj = 0.;
}

template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::secmem_face(int , const Dirichlet_entree_fluide& , int ) const
{
  return 0;
}

#ifdef DOXYGEN_SHOULD_SKIP_THIS
template<>template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int face, int num1, const Dirichlet_paroi_fixe& la_cl, double& aii, double& ajj) const
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

template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::flux_face(const DoubleTab& , int , int , int , const Echange_externe_impose& , int ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int , int , int , int , const Echange_externe_impose& ,  double& , double& ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
}

template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::secmem_face(int , int , int , const Echange_externe_impose& , int ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::flux_face(const DoubleTab& , int , const Echange_global_impose& , int ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int , int , const Echange_global_impose& ,  double& , double& ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
}

//// secmem_face avec Echange_global_impose
//
template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::secmem_face(int , const Echange_global_impose& ,  int ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::flux_face(const DoubleTab& , int ,const Neumann_paroi& ,int ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int , int ,const Neumann_paroi& ,double& , double& ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
}

template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::secmem_face(int , const Neumann_paroi& ,int ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

#ifdef DOXYGEN_SHOULD_SKIP_THIS
template<>template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int , int ,const Neumann_sortie_libre& ,double& aii, double& ajj) const
{
  aii = ajj = 0.;
}

template<>template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int  , int , const Symetrie& , double& aii, double& ajj) const
{
  aii = ajj = 0;
}

template<>template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::secmem_face(int , const Symetrie& , int ) const
{
  return 0;
}
#endif

template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::flux_face(const DoubleTab& , int ,const Periodique& ,int ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
  return 0;
}

template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_face(int , int ,const Periodique& ,double& , double& ) const
{
  Cerr << " si je suis ici, ce n'est pas bon " << finl ;
}

template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::flux_faces_interne(const DoubleTab& , int ) const
{
  Cerr << " dans flux_faces_interne " << finl;
  return 0.;
}

template<>
inline void Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::coeffs_faces_interne(int face,double& aii, double& ajj) const
{
  double dist = la_zone->dist_norm(face);
  aii = ajj =  porosite(face)*surface(face)/dist;
}

template<>
inline double Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>::secmem_faces_interne( int ) const
{
  return 0;
}

#endif /* Eval_DiffF22_VDF_const_Elem_included */
