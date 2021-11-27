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
// File:        Eval_Conv_VDF_leaves.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Conv
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Conv_VDF_leaves_included
#define Eval_Conv_VDF_leaves_included

#include <Eval_Conv_VDF_Elem.h>
#include <Eval_Conv_VDF.h>

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/*
 * xxx xxx
 * THIS IS ONLY SPECIFIC TO TRUST CHECK_SOURCE :
 * WE ARE OBLIGED TO HAVE THE 1ST CLASS WITH THE SAME NAME AS THE FILE OTHERWISE DO NOT COMPILE
 *
 * We use DOXYGEN_SHOULD_SKIP_THIS macro to skip this class in the doxygen documentation
 */
class Eval_Conv_VDF_leaves: public Eval_Conv_VDF_Elem<Eval_Conv_VDF_leaves>,
  public Eval_Conv_VDF {};
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/*
 * NOTA BENE : (pour tous les evals)
 * Les evaluateurs de flux convectifs calculent en fait le terme convectif qui figure au second
 * membre de l'equation d'evolution c.a.d l'oppose du flux convectif pour l'EXPLICITE.
 *
 * Dans le cas de la methode IMPLICITE les evaluateurs calculent la quantite qui figure dans le
 * premier membre de l'equation, nous ne prenons pas par consequent l'oppose en ce qui concerne
 * les termes pour la matrice, par contre pour le second membre nous procedons comme en explicite
 * mais en ne fesant intervenir que les valeurs fournies par les conditions limites.
 */

// .DESCRIPTION class Eval_Amont_VDF_Elem
// Evaluateur VDF pour la convection Le champ convecte est scalaire (Champ_P0_VDF)
// Schema de convection Amont
class Eval_Amont_VDF_Elem : public Eval_Conv_VDF_Elem<Eval_Amont_VDF_Elem>, public Eval_Conv_VDF
{
public:
  static constexpr bool IS_AMONT = true;
};

// .DESCRIPTION class Eval_Centre_VDF_Elem
// Evaluateur VDF pour la convection Le champ convecte est scalaire (Champ_P0_VDF)
// Schema de convection Centre ordre 2
class Eval_Centre_VDF_Elem : public Eval_Conv_VDF_Elem<Eval_Centre_VDF_Elem>, public Eval_Conv_VDF
{
public:
  static constexpr bool IS_CENTRE = true;
  inline int amont_amont(int face, int i) const { return la_zone->amont_amont(face, i); }
  inline double dim_elem(int n1, int k) const { return la_zone->dim_elem(n1,k); }
  inline double dist_elem(int n1, int n2, int k) const { return la_zone->dist_elem(n1,n2,k); }
  inline double dist_face_elem1(int num_face,int n1) const { return la_zone->dist_face_elem1(num_face, n1); }
  inline double qcentre(const double& psc, const int num0, const int num1, const int num0_0, const int num1_1, const int face,const DoubleTab& transporte) const
  { return qcentre2_impl(psc,num0,num1,num0_0,num1_1,face,transporte); }

  inline void qcentre(const double& psc, const int num0, const int num1, const int num0_0, const int num1_1,const int face,const DoubleTab& transporte,ArrOfDouble& flux) const
  { qcentre2_impl(psc,num0,num1,num0_0,num1_1,face,transporte,flux); }
};

// .DESCRIPTION class Eval_Centre4_VDF_Elem
// Evaluateur VDF pour la convection Le champ convecte est scalaire (Champ_P0_VDF)
// Schema de convection Centre ordre 4
class Eval_Centre4_VDF_Elem : public Eval_Conv_VDF_Elem<Eval_Centre4_VDF_Elem>, public Eval_Conv_VDF
{

public:
  static constexpr bool IS_CENTRE4 = true;
  inline int amont_amont(int face, int i) const { return la_zone->amont_amont(face, i); }
  inline double dist_elem(int n1, int n2, int k) const { return la_zone->dist_elem_period(n1,n2,k); }
  inline double qcentre(const double& ,const int ,const int ,const int ,const int ,const int ,const DoubleTab& ) const;
  inline void qcentre(const double& ,const int ,const int ,const int ,const int ,const int , const DoubleTab& ,ArrOfDouble& ) const;
};

inline double Eval_Centre4_VDF_Elem::qcentre(const double& psc, const int num0, const int num1, const int num0_0, const int num1_1, const int face,const DoubleTab& transporte) const
{
  const int ori = orientation(face);
  const double dx = dist_elem(num0, num1, ori), dxam = dist_elem(num0_0, num0, ori), dxav = dist_elem(num1, num1_1, ori);
  return qcentre4_impl(ori,dx,dxam,dxav,psc,num0,num1,num0_0,num1_1,face,transporte);
}

inline void Eval_Centre4_VDF_Elem::qcentre(const double& psc, const int num0, const int num1, const int num0_0, const int num1_1, const int face, const DoubleTab& transporte,ArrOfDouble& flux) const
{
  const int ori = orientation(face);
  const double dx = dist_elem(num0, num1, ori), dxam = dist_elem(num0_0, num0, ori), dxav = dist_elem(num1, num1_1, ori);
  qcentre4_impl(ori,dx,dxam,dxav,psc,num0,num1,num0_0,num1_1,face,transporte,flux);
}

// .DESCRIPTION class Eval_Quick_VDF_Elem
// Evaluateur VDF pour la convection Le champ convecte est scalaire (Champ_P0_VDF)
// Schema de convection Quick
class Eval_Quick_VDF_Elem : public Eval_Conv_VDF_Elem<Eval_Quick_VDF_Elem>, public Eval_Conv_VDF
{
public:
  static constexpr bool IS_QUICK = true;
  inline int amont_amont(int face, int i) const { return la_zone->amont_amont(face, i); }
  inline double dim_elem(int n1, int k) const { return la_zone->dim_elem(n1,k); }
  inline double dist_elem(int n1, int n2, int k) const { return la_zone->dist_elem_period(n1,n2,k); }
  inline double quick_fram(const double&, const int, const int,const int, const int ,const int ,const DoubleTab& ) const;
  inline void quick_fram(const double&, const int, const int,const int, const int ,const int ,const DoubleTab&,ArrOfDouble& ) const;
};

inline double Eval_Quick_VDF_Elem::quick_fram(const double& psc, const int num0, const int num1,const int num0_0, const int num1_1, const int face,const DoubleTab& transporte) const
{
  const int ori = orientation(face);
  const double dx = dist_elem(num0, num1, ori),
               dm1 = dim_elem(num0, ori), dxam1 = dist_elem(num0_0, num0, ori),
               dm2 = dim_elem(num1, ori), dxam2 = dist_elem(num1, num1_1, ori);
  return quick_fram_impl(ori,dx,dm1,dxam1,dm2,dxam2,psc,num0,num1,num0_0,num1_1,face,transporte);
}

inline void Eval_Quick_VDF_Elem::quick_fram(const double& psc, const int num0, const int num1,const int num0_0, const int num1_1, const int face,const DoubleTab& transporte,ArrOfDouble& flux) const
{
  const int ori = orientation(face);
  const double dx = dist_elem(num0, num1, ori),
               dm0 = dim_elem(num0, ori), dxam0 = (num0_0!=-1?dist_elem(num0_0, num0, ori):0),
               dm1 = dim_elem(num1, ori), dxam1 = (num1_1!=-1?dist_elem(num1, num1_1, ori):0);
  quick_fram_impl(ori,dx,dm0,dxam0,dm1,dxam1,psc,num0,num1,num0_0,num1_1,face,transporte,flux);
}

#endif /* Eval_Conv_VDF_leaves_included */
