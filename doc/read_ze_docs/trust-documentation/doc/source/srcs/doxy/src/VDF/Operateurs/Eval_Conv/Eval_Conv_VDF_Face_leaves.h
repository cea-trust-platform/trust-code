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

#ifndef Eval_Conv_VDF_Face_leaves_included
#define Eval_Conv_VDF_Face_leaves_included

#include <Eval_Conv_VDF_Face.h>
#include <Eval_Conv_VDF.h>

/// \cond DO_NOT_DOCUMENT
class Eval_Conv_VDF_Face_leaves : public Eval_Conv_VDF_Face<Eval_Conv_VDF_Face_leaves>,
  public Eval_Conv_VDF {};
/// \endcond

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

/*! @brief class Eval_Amont_VDF_Face Evaluateur VDF pour la convection Le champ convecte est un Champ_Face_VDF
 *
 *  Schema de convection Amont
 *
 */
class Eval_Amont_VDF_Face : public Eval_Conv_VDF_Face<Eval_Amont_VDF_Face>, public Eval_Conv_VDF
{
public:
  static constexpr bool IS_AMONT = true, CALC_ARR_COIN_FL = true;
};

/*! @brief class Eval_Centre_VDF_Face Evaluateur VDF pour la convection Le champ convecte est un Champ_Face_VDF
 *
 *  Schema de convection Centre
 *
 */
class Eval_Centre_VDF_Face : public Eval_Conv_VDF_Face<Eval_Centre_VDF_Face>, public Eval_Conv_VDF
{
public:
  static constexpr bool IS_CENTRE = true;
};

/*! @brief class Eval_Centre4_VDF_Face Evaluateur VDF pour la convection Le champ convecte est un Champ_Face_VDF
 *
 *  Schema de convection Centre4 (sur 4 points)
 *
 */
class Eval_Centre4_VDF_Face : public Eval_Conv_VDF_Face<Eval_Centre4_VDF_Face>, public Eval_Conv_VDF
{
public:
  static constexpr bool IS_CENTRE4 = true;

  inline int face_amont_conj(int num_face,int i,int k) const override { return le_dom->face_amont_conj(num_face, i, k); }
  inline int face_amont_princ(int num_face,int i) const override { return le_dom->face_amont_princ(num_face, i); }
  inline double dist_face(int n1,int n2,int k) const { return le_dom->dist_face(n1,n2,k); }
  inline double dist_face_period(int n1,int n2,int k) const { return le_dom->dist_face_period(n1,n2,k); }
  inline double dist_elem_period(int n1,int n2,int k) const override { return le_dom->dist_elem_period(n1,n2,k); }
  inline double dim_elem(int n1,int k) const override { return le_dom->dim_elem(n1,k); }
  inline double conv_centre(const double psc,const double vit_0_0, const double vit_0, const double vit_1,const double vit1_1,double g1, double g2, double g3,double g4) const override
  { return (g1*vit_0_0 + g2*vit_0 + g3*vit_1 + g4*vit1_1) * psc; }

  inline void calcul_g(const double dxam, const double dx, const double dxav, double& g1, double& g2, double& g3, double& g4) const override
  {return calcul_g_impl(dxam,dx,dxav,g1,g2,g3,g4); }
};

/*! @brief class Eval_Quick_VDF_Face Evaluateur VDF pour la convection Le champ convecte est un Champ_Face_VDF
 *
 *  Schema de convection Quick
 *
 */
class Eval_Quick_VDF_Face : public Eval_Conv_VDF_Face<Eval_Quick_VDF_Face>, public Eval_Conv_VDF
{
public:
  static constexpr bool IS_QUICK = true;

  inline int face_amont_conj(int num_face,int i,int k) const override { return le_dom->face_amont_conj(num_face, i, k); }
  inline int face_amont_princ(int num_face,int i) const override { return le_dom->face_amont_princ(num_face, i); }
  inline double dim_elem(int n1,int k) const override { return le_dom->dim_elem(n1,k); }
  inline double dim_face(int n1,int k) const override { return le_dom->dim_face(n1,k); }
  inline double dist_face(int n1,int n2,int k) const { return le_dom->dist_face(n1,n2,k); }
  inline double dist_elem(int n1,int n2,int k) const override { return le_dom->dist_elem(n1,n2,k); }
  inline double dist_elem_period(int n1, int n2, int k) const override { return le_dom->dist_elem_period(n1,n2,k); }
  inline double dist_face_period(int n1,int n2,int k) const { return le_dom->dist_face_period(n1,n2,k); }
  inline double conv_quick_sharp_plus(const double psc,const double vit_0, const double vit_1, const double vit_0_0,
                                      const double dx, const double dm, const double dxam) const override
  { return conv_quick_sharp_plus_impl(psc,vit_0,vit_1,vit_0_0,dx,dm,dxam); }

  inline double conv_quick_sharp_moins(const double psc,const double vit_0,const double vit_1, const double vit_1_1,
                                       const double dx, const double dm,const double dxam) const override
  { return conv_quick_sharp_moins_impl(psc,vit_0,vit_1,vit_1_1,dx,dm,dxam); }
};

/*! @brief class Eval_Quick_VDF_Face_Axi Evaluateur VDF pour la convection en coordonnees cylindriques : Le champ convecte est un Champ_Face_VDF
 *
 *  Schema de convection Quick
 *
 */
class Eval_Quick_VDF_Face_Axi : public Eval_Conv_VDF_Face<Eval_Quick_VDF_Face_Axi>, public Eval_Conv_VDF
{
public:
  static constexpr bool IS_AXI = true, IS_QUICK = true, CALC_ARR_PERIO = false, CALC_ARR_NAVIER_FL = false;

  inline int face_amont_princ(int num_face,int i) const override { return le_dom->face_amont_princ(num_face, i); }
  inline int face_amont_conj(int ,int ,int ) const override;
  inline double dist_face(int ,int ,int ) const;
  inline double dist_elem_period(int n1, int n2, int k) const override { return dist_face(n1,n2,k); }
  inline double dim_face(int ,int ) const override;
  inline double dist_elem(int ,int ,int ) const override;
  inline double dim_elem(int ,int ) const override;
  inline double conv_quick_sharp_plus(const double psc,const double vit_0, const double vit_1, const double vit_0_0,
                                      const double dx, const double dm, const double dxam) const override
  { return conv_quick_sharp_plus_impl(psc,vit_0,vit_1,vit_0_0,dx,dm,dxam); }

  inline double conv_quick_sharp_moins(const double psc,const double vit_0,const double vit_1, const double vit_1_1,
                                       const double dx, const double dm,const double dxam) const override
  { return conv_quick_sharp_moins_impl(psc,vit_0,vit_1,vit_1_1,dx,dm,dxam); }
};

inline double Eval_Quick_VDF_Face_Axi::dim_elem(int n1, int k) const
{
  const IntTab& elem_faces_ = le_dom->elem_faces();
  return dist_face(elem_faces_(n1,k), elem_faces_(n1,k+dimension), k) ;
}

inline double Eval_Quick_VDF_Face_Axi::dist_elem(int n1, int n2, int k) const
{
  const DoubleTab& xp_ = le_dom->xp();
  return dist_elem_axi_impl(n1,n2,k,xp_);
}

inline double Eval_Quick_VDF_Face_Axi::dist_face(int n1, int n2, int k) const
{
  const DoubleTab& xv_ = le_dom->xv();
  return dist_face_axi_impl(n1,n2,k,xv_);
}

inline double Eval_Quick_VDF_Face_Axi::dim_face(int n1, int k) const
{
  const IntTab& face_voisins_ = le_dom->face_voisins();
  const int elem0 = face_voisins_(n1,0), elem1 = face_voisins_(n1,1) ;
  if (elem0 < 0 ) return  dim_elem(elem1, k) ;
  if (elem1 < 0 ) return  dim_elem(elem0, k) ;
  return (dim_elem(elem0, k) + dim_elem(elem1, k)) * 0.5 ;
}

inline int Eval_Quick_VDF_Face_Axi::face_amont_conj(int num_face, int k, int i) const
{
  const IntTab& face_voisins_ = le_dom->face_voisins();
  const IntTab& elem_faces_   = le_dom->elem_faces();
  const IntVect& orientation_ = le_dom->orientation();
  return face_amont_conj_axi_impl(num_face,k,i,dimension,face_voisins_,elem_faces_,orientation_);
}

#endif /* Eval_Conv_VDF_Face_leaves_included */
