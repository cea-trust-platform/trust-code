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
// File:        Eval_Diff_VDF_var_Elem_aniso.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/New_eval
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Diff_VDF_var_Elem_aniso_included
#define Eval_Diff_VDF_var_Elem_aniso_included

#include <Eval_Diff_VDF_var_aniso.h>
#include <Eval_Diff_VDF_Elem.h>

//
// .DESCRIPTION class Eval_Diff_VDF_var_Elem_aniso
//
// Evaluateur VDF pour la diffusion
// Le champ diffuse est scalaire (Champ_P0_VDF)
// Le champ de diffusivite n'est pas constant.

//
// .SECTION voir aussi Eval_Diff_VDF_var_aniso

class Eval_Diff_VDF_var_Elem_aniso : public Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_aniso>,
  public Eval_Diff_VDF_var_aniso
{
};

/*****************************************************************************/
/* Class template specializations for the class Eval_Diff_VDF_var_Elem_aniso */
/*****************************************************************************/

/* Class template specialization for anisotrope Dirichlet_entree_fluide */
template<>
inline double Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_aniso>::flux_face(const DoubleTab& inco,
                                                                          int face,const Dirichlet_entree_fluide& la_cl,
                                                                          int num1) const
{
  double T_imp = la_cl.val_imp(face-num1);
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int ori= orientation(face);
  // Olga avait mis :
  // double dist = 2*Dist_norm_bord(face);
  // Pierre dit que :
  double dist = Dist_norm_bord(face);
  double flux;
  if (n0 != -1)
    {
      flux = (T_imp-inco[n0])*surface(face)/dist;
      flux *= (porosite(face)*nu_1(n0,ori));
    }
  else   // n1 != -1
    {
      flux = (inco[n1]-T_imp)*surface(face)/dist;
      flux*= (porosite(face)*nu_1(n1,ori));
    }
  return flux;
}

/* Class template specialization for anisotrope Dirichlet_entree_fluide */
template<>
inline void Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_aniso>::coeffs_face(int face, int,
                                                                          const Dirichlet_entree_fluide& la_cl,
                                                                          double& aii, double& ajj) const
{
  int i = elem_(face,0);
  int ori= orientation(face);
  //  int j = elem(face,1);
  double dist = Dist_norm_bord(face);

  if (i != -1)
    {
      aii =  porosite(face)*nu_1(i,ori)*surface(face)/dist;
      ajj = 0;
    }
  else   // j != -1
    {
      ajj =  porosite(face)*nu_1(elem_(face,1),ori)*surface(face)/dist;
      aii = 0;
    }
}

/* Class template specialization for anisotrope Dirichlet_entree_fluide */
template<>
inline double Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_aniso>::secmem_face(int face,
                                                                            const Dirichlet_entree_fluide& la_cl,
                                                                            int num1) const
{
  double T_imp = la_cl.val_imp(face-num1);
  int i = elem_(face,0);
  int ori= orientation(face);
  //  int j = elem(face,1);
  double dist = Dist_norm_bord(face);

  double flux;
  if (i != -1)
    {
      flux = T_imp*surface(face)/dist;
      flux *= porosite(face)*nu_1(i,ori);
    }
  else   // j != -1
    {
      flux = -T_imp*surface(face)/dist;
      flux *= (porosite(face)*nu_1(elem_(face,1),ori));
    }
  return flux;
}

/* Class template specialization for anisotrope Echange_externe_impose */
template<>
inline double Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_aniso>::flux_face(const DoubleTab& inco,
                                                                          int boundary_index, int face, int local_face,
                                                                          const Echange_externe_impose& la_cl,
                                                                          int num1) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double h_total_inv, heq;
  double h_imp = la_cl.h_imp(face-num1);
  double T_ext = la_cl.T_ext(face-num1);
  double e;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
//  Dist_norm_bord_externe(boundary_index,face,local_face);

  if (Eval_Diff_VDF_var_Elem_aniso::Is_Modif_Deq)
    {
      // TODO : FIXME : to implement when opp dift follow the procedure
      Cerr << "Op_Dift are not templatized yet !" << finl;
      Process::exit();
//      if (ind_Fluctu_Term==1)
//        e=Dist_norm_bord_externe_(face) ;
//      else
//        e=equivalent_distance[boundary_index](local_face);
//      //e=d_equiv(face-la_zone->premiere_face_bord());
    }
  else
    {
      if (Eval_Diff_VDF_var_Elem_aniso::Is_Dequiv)
        {
          // TODO : FIXME : to implement when opp dift follow the procedure
          Cerr << "Op_Dift is not templatized yet !" << finl;
          Process::exit();
//          e=equivalent_distance[boundary_index](local_face);
//          //  e=d_equiv(face-la_zone->premiere_face_bord());
        }
      else
        e=Dist_norm_bord_externe_(face);
    }

  int ori = orientation(face);
  double flux;
  if (n0 != -1)
    {
      //e = la_zone->xv(face,ori) - la_zone->xp(n0,ori);
      if (nu_1(n0,ori) == 0.0)
        {
          heq = h_imp; // Should be 0 ??
        }
      else
        {
          h_total_inv = 1.0/h_imp + e/nu_1(n0,ori);
          heq = 1.0 / h_total_inv;
        }

      flux = heq*(T_ext - inco[n0])*surface(face);
    }
  else
    {
      //e = la_zone->xp(n1,ori) - la_zone->xv(face,ori);
      if (nu_1(n1,ori) == 0.0)
        {
          heq = h_imp; // Should be 0 ??
        }
      else
        {
          h_total_inv = 1.0/h_imp + e/nu_1(n1,ori);
          heq = 1.0 / h_total_inv;
        }

      flux = heq*(inco[n1] - T_ext)*surface(face);
    }
  return flux;
}


/* Class template specialization for anisotrope Echange_externe_impose */
template<>
inline void Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_aniso>::coeffs_face(int boundary_index,
                                                                          int face, int local_face, int num1,
                                                                          const Echange_externe_impose& la_cl,
                                                                          double& aii, double& ajj) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double h_total_inv, heq;
  double h_imp = la_cl.h_imp(face-num1);
  double e;
  int i = elem_(face,0);
  //  int j = elem(face,1);
  int ori = orientation(face);
//  Dist_norm_bord_externe(boundary_index,face,local_face);

  if (Eval_Diff_VDF_var_Elem_aniso::Is_Modif_Deq)
    {
      // TODO : FIXME : to implement when opp dift follow the procedure
      Cerr << "Op_Dift are not templatized yet !" << finl;
      Process::exit();
//      if (ind_Fluctu_Term==1)
//        e=Dist_norm_bord_externe_(face) ;
//      else
//        e=equivalent_distance[boundary_index](local_face);
//      //e=d_equiv(face-la_zone->premiere_face_bord());
    }
  else
    {
      if (Eval_Diff_VDF_var_Elem_aniso::Is_Dequiv)
        {
          // TODO : FIXME : to implement when opp dift follow the procedure
          Cerr << "Op_Dift is not templatized yet !" << finl;
          Process::exit();
//          e=equivalent_distance[boundary_index](local_face);
//          //  e=d_equiv(face-la_zone->premiere_face_bord());
        }
      else
        e=Dist_norm_bord_externe_(face);
    }

  if (i != -1)
    {
      // e = la_zone->xv(face,ori) - la_zone->xp(i,ori);
      if (nu_1(i,ori) == 0.0)
        {
          heq = h_imp; // Should be 0 ??
        }
      else
        {
          h_total_inv =  1.0/h_imp + e/nu_1(i,ori);
          heq = 1.0 / h_total_inv;
        }

      aii = heq*surface(face);
      ajj = 0;
    }
  else
    {
      // e = la_zone->xp(j,ori) - la_zone->xv(face,ori);
      if (nu_1(elem_(face,1),ori) == 0.0)
        {
          heq = h_imp; // Should be 0 ??
        }
      else
        {
          h_total_inv =  1.0/h_imp + e/nu_1(elem_(face,1),ori);
          heq = 1.0 / h_total_inv;
        }

      ajj = heq*surface(face);
      aii = 0;
    }
}

/* Class template specialization for anisotrope Echange_externe_impose */
template<>
inline double Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_aniso>::secmem_face(int boundary_index,
                                                                            int face, int local_face,
                                                                            const Echange_externe_impose& la_cl,
                                                                            int num1) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double h_total_inv, heq;
  double h_imp = la_cl.h_imp(face-num1);
  double T_ext = la_cl.T_ext(face-num1);
  double e;
  int i = elem_(face,0);
  //  int j = elem(face,1);
  int ori = orientation(face);
//  Dist_norm_bord_externe(boundary_index,face,local_face);

  if (Eval_Diff_VDF_var_Elem_aniso::Is_Modif_Deq)
    {
      // TODO : FIXME : to implement when opp dift follow the procedure
      Cerr << "Op_Dift are not templatized yet !" << finl;
      Process::exit();
//      if (ind_Fluctu_Term==1)
//        e=Dist_norm_bord_externe_(face) ;
//      else
//        e=equivalent_distance[boundary_index](local_face);
//      //e=d_equiv(face-la_zone->premiere_face_bord());
    }
  else
    {
      if (Eval_Diff_VDF_var_Elem_aniso::Is_Dequiv)
        {
          // TODO : FIXME : to implement when opp dift follow the procedure
          Cerr << "Op_Dift is not templatized yet !" << finl;
          Process::exit();
//          e=equivalent_distance[boundary_index](local_face);
//          //  e=d_equiv(face-la_zone->premiere_face_bord());
        }
      else
        e=Dist_norm_bord_externe_(face);
    }

  double flux;
  if (i != -1)
    {
      //e = la_zone->xv(face,ori) - la_zone->xp(i,ori);
      if (nu_1(i,ori) == 0.0)
        {
          heq = h_imp; // Should be 0 ??
        }
      else
        {
          h_total_inv = 1.0/h_imp + e/nu_1(i,ori);
          heq = 1.0 / h_total_inv;
        }

      flux = heq*T_ext*surface(face);
    }
  else
    {
      //e = la_zone->xp(j,ori) - la_zone->xv(face,ori);
      if (nu_1(elem_(face,1),ori) == 0.0)
        {
          heq = h_imp; // Should be 0 ??
        }
      else
        {
          h_total_inv = 1.0/h_imp + e/nu_1(elem_(face,1),ori);
          heq = 1.0 / h_total_inv;
        }

      flux = - heq*T_ext*surface(face);
    }
  return flux;
}

/* Class template specialization for anisotrope Periodique */
template<>
inline double Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_aniso>::flux_face(const DoubleTab& inco,
                                                                          int face,const Periodique& la_cl, int ) const
{
  double flux;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double d0 = la_zone->dist_face_elem0_period(face,n0,la_cl.distance());
  double d1 = la_zone->dist_face_elem1_period(face,n1,la_cl.distance());
  int ori = orientation(face);
  double heq=  compute_heq(d0,n0,d1,n1,ori);
  flux = heq*(inco[n1] - inco[n0])*surface(face)*porosite(face);
  return flux;
}

/* Class template specialization for anisotrope Periodique */
template<>
inline void Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_aniso>::coeffs_face(int face,int,
                                                                          const Periodique& la_cl,
                                                                          double& aii, double& ajj) const
{
  int i = elem_(face,0);
  int j = elem_(face,1);
  double d0 = la_zone->dist_face_elem0_period(face,i,la_cl.distance());
  double d1 = la_zone->dist_face_elem1_period(face,j,la_cl.distance());
  int ori = orientation(face);
  double heq=  compute_heq(d0,i,d1,j,ori);
  aii = ajj = heq*surface(face)*porosite(face);
}

/* Class template specialization for anisotrope NSCBC */
template<>
inline double Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_aniso>::flux_face(const DoubleTab& inco ,
                                                                          int faceD, const NSCBC& la_cl , int ndeb ) const
{
  // On evalue le flux au bord a partir des premieres mailles internes
  const Zone_VDF& zvdf = la_zone;
  int oriD = zvdf.orientation(faceD);
  int faceC;
  int elem3;

  int elem0 = zvdf.face_voisins(faceD,0);
  //double signe = -1.;
  if (elem0 == -1)
    {
      elem0 = zvdf.face_voisins(faceD,1);
      faceC = zvdf.elem_faces(elem0,oriD+dimension);
      elem3 = zvdf.face_voisins(faceC,1);
      //  signe = 1.;
    }
  else
    {
      faceC = zvdf.elem_faces(elem0,oriD);
      elem3 = zvdf.face_voisins(faceC,0);
    }

  assert(faceC != faceD);
  assert(elem3 != elem0);

  double d0 = zvdf.xp(elem0,orientation(faceD)) - zvdf.xp(elem3,orientation(faceD));
  double heq = (nu_1(elem0,oriD)/d0);
  double flux = heq*(inco(elem0) - inco(elem3))*surface(faceD)*porosite(faceD);

  return flux;
}

/* Class template specialization for anisotrope flux_faces_interne */
template<>
inline double Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_aniso>::flux_faces_interne(const DoubleTab& inco,
                                                                                   int face) const
{
  double flux;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double d0 = Dist_face_elem0(face,n0);
  double d1 = Dist_face_elem1(face,n1);
  int ori = orientation(face);
  double heq;
  if (nu_1(n0,ori)==0.0 && nu_1(n1,ori)==0.0)
    {
      heq = 0.;
    }
  else
    {
      assert(nu_1(n0,ori)!=0.0);
      assert(nu_1(n1,ori)!=0.0);
      heq=1./(d0/nu_1(n0,ori) + d1/nu_1(n1,ori));
    }
  flux = heq*(inco[n1] - inco[n0])*surface(face)*porosite(face);
  return flux;
}

/* Class template specialization for anisotrope coeffs_faces_interne */
template<>
inline void Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_aniso>::coeffs_faces_interne(int face,
                                                                                   double& aii, double& ajj) const
{
  int i = elem_(face,0);
  int j = elem_(face,1);
  double d0 = Dist_face_elem0(face,i);
  double d1 = Dist_face_elem1(face,j);
  int ori = orientation(face);
  double heq;
  if (nu_1(i,ori)==0.0 && nu_2(j,ori)==0.0)
    {
      heq = 0.;
    }
  else
    {
      assert(nu_1(i,ori)!=0.0);
      assert(nu_2(j,ori)!=0.0);
      heq=1./(d0/nu_1(i,ori) + d1/nu_2(j,ori));
    }
  aii = ajj = heq*surface(face)*porosite(face);
}

#endif /* Eval_Diff_VDF_var_Elem_aniso_included */
