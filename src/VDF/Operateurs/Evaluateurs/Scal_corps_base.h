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
// File:        Scal_corps_base.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////
#ifndef MULTD
#define MULTD 2*
#endif
#ifdef D_AXI
#define Dist_norm_bord(face) MULTD la_zone->dist_norm_bord_axi(face)
#define Dist_face_elem0(face,n0) dist_face_elem0_axi(face,n0)
#define Dist_face_elem1(face,n1) dist_face_elem1_axi(face,n1)
#ifndef DEQUIV
#define Dist_norm_bord_externe_(global_face) la_zone->dist_norm_bord_axi(face)
#endif
#else
#define Dist_norm_bord(face) MULTD la_zone->dist_norm_bord(face)
#define Dist_face_elem0(face,n0) dist_face_elem0(face,n0)
#define Dist_face_elem1(face,n1) dist_face_elem1(face,n1)
#ifndef DEQUIV
#define Dist_norm_bord_externe_(global_face) la_zone->dist_norm_bord(face)
#endif
#endif

#ifdef MODIF_DEQ
#define Dist_norm_bord_externe(boundary_index,global_face,local_face)	     \
  if (ind_Fluctu_Term==1)                                                    \
      e=Dist_norm_bord_externe_(global_face) ;				     \
  else                                                                       \
    e=equivalent_distance[boundary_index](local_face);
//e=d_equiv(face-la_zone->premiere_face_bord());
#else
#ifdef DEQUIV
#define Dist_norm_bord_externe(boundary_index,global_face,local_face)	    \
  e=equivalent_distance[boundary_index](local_face);
//  e=d_equiv(face-la_zone->premiere_face_bord());
#else
#define Dist_norm_bord_externe(boundary_index,global_face,local_face) e=Dist_norm_bord_externe_(global_face)
#endif
#endif
// Fonctions de calcul des flux pour une grandeur scalaire

//// flux_face avec Dirichlet_entree_fluide
//

inline double CLASSNAME::flux_face(const DoubleTab& inco, int face,
                                   const Dirichlet_entree_fluide& la_cl,
                                   int num1) const
{
  double T_imp = la_cl.val_imp(face-num1);
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  // Olga avait mis :
  // double dist = 2*Dist_norm_bord(face);
  // Pierre dit que :
  double dist = Dist_norm_bord(face);
  double flux;
  if (n0 != -1)
    {
      flux = (T_imp-inco[n0])*surface(face)/dist;
      flux *= (porosite(face)*nu_1(n0));
    }
  else   // n1 != -1
    {
      flux = (inco[n1]-T_imp)*surface(face)/dist;
      flux*= (porosite(face)*nu_1(n1));
    }
  return flux;
}


//// coeffs_face avec Dirichlet_entree_fluide
//

inline void CLASSNAME::coeffs_face(int face, int,
                                   const Dirichlet_entree_fluide& la_cl,
                                   double& aii, double& ajj) const
{
  int i = elem_(face,0);
  //  int j = elem(face,1);
  double dist = Dist_norm_bord(face);
  if (i != -1)
    {
      aii =  porosite(face)*nu_1(i)*surface(face)/dist;
      ajj = 0;
    }
  else   // j != -1
    {
      ajj =  porosite(face)*nu_1(elem_(face,1))*surface(face)/dist;
      aii = 0;
    }
}

//// secmem_face avec Dirichlet_entree_fluide
//

inline double CLASSNAME::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,
                                     int num1) const
{
  double T_imp = la_cl.val_imp(face-num1);
  int i = elem_(face,0);
  //  int j = elem(face,1);
  double dist = Dist_norm_bord(face);

  double flux;
  if (i != -1)
    {
      flux = T_imp*surface(face)/dist;
      flux *= porosite(face)*nu_1(i);
    }
  else   // j != -1
    {
      flux = -T_imp*surface(face)/dist;
      flux *= (porosite(face)*nu_1(elem_(face,1)));
    }
  return flux;
}

//// flux_face avec Dirichlet_paroi_defilante
//

inline double CLASSNAME::flux_face(const DoubleTab&, int ,
                                   const Dirichlet_paroi_defilante&, int ) const
{
  return 0;
}

//// coeffs_face avec Dirichlet_paroi_defilante
//

inline void CLASSNAME::coeffs_face(int , int,
                                   const Dirichlet_paroi_defilante&,
                                   double&, double& ) const
{
  ;
}

//// secmem_face avec Dirichlet_paroi_defilante
//

inline double CLASSNAME::secmem_face(int, const Dirichlet_paroi_defilante&,
                                     int ) const
{
  return 0;
}

//// flux_face avec Dirichlet_paroi_fixe
//

inline double CLASSNAME::flux_face(const DoubleTab&, int ,
                                   const Dirichlet_paroi_fixe&,
                                   int ) const
{
  return 0;
}

//// coeffs_face avec Dirichlet_paroi_fixe
//

inline void CLASSNAME::coeffs_face(int ,int,
                                   const Dirichlet_paroi_fixe&,
                                   double&, double& ) const
{
  ;
}

//// secmem_face avec Dirichlet_paroi_fixe
//

inline double CLASSNAME::secmem_face(int, const Dirichlet_paroi_fixe&,
                                     int ) const
{
  return 0;
}


//// flux_face avec Echange_externe_impose
//

inline double CLASSNAME::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                   const Echange_externe_impose& la_cl,
                                   int num1) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double h_total_inv;
  double h_imp = la_cl.h_imp(face-num1);
  double T_ext = la_cl.T_ext(face-num1);
  double e;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  Dist_norm_bord_externe(boundary_index,face,local_face);
  //  int ori = orientation(face);
  double flux;
  if (n0 != -1)
    {
      //e = la_zone->xv(face,ori) - la_zone->xp(n0,ori);
      h_total_inv = 1/h_imp + e/nu_2(n0);
      flux = (T_ext - inco[n0])*surface(face)/h_total_inv;
    }
  else
    {
      //e = la_zone->xp(n1,ori) - la_zone->xv(face,ori);
      h_total_inv = 1/h_imp + e/nu_2(n1);
      flux = (inco[n1] - T_ext)*surface(face)/h_total_inv;
    }
  return flux;
}

//// coeffs_face avec Echange_externe_impose
//

inline void CLASSNAME::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                   const Echange_externe_impose& la_cl,
                                   double& aii, double& ajj) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double h_total_inv;
  double h_imp = la_cl.h_imp(face-num1);
  double e;
  int i = elem_(face,0);
  //  int j = elem(face,1);
  //  int ori = orientation(face);
  Dist_norm_bord_externe(boundary_index,face,local_face);
  if (i != -1)
    {
      // e = la_zone->xv(face,ori) - la_zone->xp(i,ori);
      h_total_inv =  1/h_imp + e/nu_2(i);
      aii = surface(face)/h_total_inv;
      ajj = 0;
    }
  else
    {
      // e = la_zone->xp(j,ori) - la_zone->xv(face,ori);
      h_total_inv =  1/h_imp + e/nu_2(elem_(face,1));
      ajj = surface(face)/h_total_inv;
      aii = 0;
    }
}

//// secmem_face avec Echange_externe_impose
//

inline double CLASSNAME::secmem_face(int boundary_index,int face, int local_face, const Echange_externe_impose& la_cl,
                                     int num1) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double h_total_inv;
  double h_imp = la_cl.h_imp(face-num1);
  double T_ext = la_cl.T_ext(face-num1);
  double e;
  int i = elem_(face,0);
  //  int j = elem(face,1);
  //  int ori = orientation(face);
  Dist_norm_bord_externe(boundary_index,face,local_face);
  double flux;
  if (i != -1)
    {
      //e = la_zone->xv(face,ori) - la_zone->xp(i,ori);
      h_total_inv = 1/h_imp + e/nu_2(i);
      flux = T_ext*surface(face)/h_total_inv;
    }
  else
    {
      //e = la_zone->xp(j,ori) - la_zone->xv(face,ori);
      h_total_inv = 1/h_imp + e/nu_2(elem_(face,1));
      flux = - T_ext*surface(face)/h_total_inv;
    }
  return flux;
}

//// flux_face avec Echange_global_impose
//

inline double CLASSNAME::flux_face(const DoubleTab& inco, int face ,
                                   const Echange_global_impose& la_cl, int num1) const
{
  double h = la_cl.h_imp(face-num1);
  double T_ext = la_cl.T_ext(face-num1);
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double flux;
  if (n0 != -1)
    flux = h*(T_ext - inco[n0])*surface(face);
  else
    flux = h*(inco[n1] - T_ext)*surface(face);
  return flux;
}

//// coeffs_face avec Echange_global_impose
//

inline void CLASSNAME::coeffs_face(int face, int num1,
                                   const Echange_global_impose& la_cl,
                                   double& aii, double& ajj) const
{
  double h = la_cl.h_imp(face-num1);
  int i = elem_(face,0);
  //  int j = elem(face,1);
  if (i!= -1)
    {
      aii = h*surface(face);
      ajj = 0;
    }
  else
    {
      ajj = h*surface(face);
      aii = 0;
    }
}

//// secmem_face avec Echange_global_impose
//

inline double CLASSNAME::secmem_face(int face, const Echange_global_impose& la_cl,
                                     int num1) const
{
  double h = la_cl.h_imp(face-num1);
  double T_ext = la_cl.T_ext(face-num1);
  int i = elem_(face,0);
  //  int j = elem(face,1);
  double flux;
  if (i != -1)
    flux = h*T_ext*surface(face);
  else
    flux = -h*T_ext*surface(face);
  return flux;
}


//// flux_face avec Neumann_paroi
//

inline double CLASSNAME::flux_face(const DoubleTab& , int face,
                                   const Neumann_paroi& la_cl,
                                   int num1) const
{
  int i = elem_(face,0);
  //  int j = elem(face,1);
  double flux;
  if (i != -1)
    flux = la_cl.flux_impose(face-num1)*surface(face);
  else
    flux = -la_cl.flux_impose(face-num1)*surface(face);

  return flux;
}

//// coeffs_face avec Neumann_paroi
//

inline void CLASSNAME::coeffs_face(int , int,
                                   const Neumann_paroi& ,
                                   double& aii, double& ajj) const
{
  aii = 0;
  ajj = 0;
}

//// secmem_face avec Neumann_paroi
//

inline double CLASSNAME::secmem_face(int face, const Neumann_paroi& la_cl,
                                     int num1) const
{
  int i = elem_(face,0);
  //  int j = elem(face,1);
  double flux;
  if (i != -1)
    flux = la_cl.flux_impose(face-num1)*surface(face);
  else
    flux = -la_cl.flux_impose(face-num1)*surface(face);

  return flux;
}


//// flux_face avec Neumann_paroi_adiabatique
//

inline double CLASSNAME::flux_face(const DoubleTab& , int ,
                                   const Neumann_paroi_adiabatique&, int ) const
{
  return 0;
}

//// coeffs_face avec Neumann_paroi_adiabatique
//

inline void CLASSNAME::coeffs_face(int , int,
                                   const Neumann_paroi_adiabatique&,
                                   double&, double&) const
{
  ;
}

//// secmem_face avec Neumann_paroi_adiabatique
//

inline double CLASSNAME::secmem_face(int, const Neumann_paroi_adiabatique&,
                                     int ) const
{
  return 0;
}

//// flux_face avec Neumann_sortie_libre
//

inline double CLASSNAME::flux_face(const DoubleTab& , int  ,
                                   const Neumann_sortie_libre& , int  ) const
{
  return 0;
}

//// coeffs_face avec Neumann_sortie_libre
//

inline void CLASSNAME::coeffs_face(int ,int,const Neumann_sortie_libre& ,
                                   double&, double&) const
{
  ;
}

//// secmem_face avec Neumann_sortie_libre
//

inline double CLASSNAME::secmem_face(int, const Neumann_sortie_libre& ,
                                     int ) const
{
  return 0;
}


//// flux_face avec Symetrie
//

inline double CLASSNAME::flux_face(const DoubleTab&, int ,
                                   const Symetrie&, int ) const
{
  return 0;
}

//// coeffs_face avec Symetrie
//

inline void CLASSNAME::coeffs_face(int ,int, const Symetrie&, double&, double& ) const
{
  ;
}

//// secmem_face avec Symetrie
//

inline double CLASSNAME::secmem_face(int, const Symetrie&, int ) const
{
  return 0;
}

//// flux_face avec Periodique
//

inline double CLASSNAME::flux_face(const DoubleTab& inco, int face,
                                   const Periodique& la_cl, int ) const
{
  double flux;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double d0 = la_zone->dist_face_elem0_period(face,n0,la_cl.distance());
  double d1 = la_zone->dist_face_elem1_period(face,n1,la_cl.distance());
  double heq;
  f_heq(d0,n0,d1,n1);
  flux = heq*(inco[n1] - inco[n0])*surface(face)*porosite(face);
  return flux;
}

//// coeffs_face avec Periodique

inline void CLASSNAME::coeffs_face(int face,int,
                                   const Periodique& la_cl,
                                   double& aii, double& ajj) const
{
  int i = elem_(face,0);
  int j = elem_(face,1);
  double d0 = la_zone->dist_face_elem0_period(face,i,la_cl.distance());
  double d1 = la_zone->dist_face_elem1_period(face,j,la_cl.distance());
  double heq;
  f_heq(d0,i,d1,j);
  aii = ajj = heq*surface(face)*porosite(face);
}

//// secmem_face avec Periodique
//

inline double CLASSNAME::secmem_face(int, const Periodique&,
                                     int ) const
{
  return 0;
}



//// flux_face avec NSCBC
//

inline double CLASSNAME::flux_face(const DoubleTab& inco , int faceD ,
                                   const NSCBC& la_cl , int ndeb ) const
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
  double heq = (nu_1(elem0)/d0);
  double flux = heq*(inco(elem0) - inco(elem3))*surface(faceD)*porosite(faceD);

  return flux;
}


//// coeffs_face avec NSCBC
//
inline void CLASSNAME::coeffs_face(int face, int num1,const NSCBC& la_cl,
                                   double& aii, double& ajj) const
{
  /*  Cerr<<"Eval_Diff_VDF_var_Elem::coeffs_face n'est pas codee"<<finl;
      Cerr<<"pour la condition NSCBC"<<finl;
      exit();*/
  aii = ajj = 0.;
}

//// secmem_face avec NSCBC
//

inline double CLASSNAME::secmem_face(int, const NSCBC& ,
                                     int ) const
{
  /*  Cerr<<"Eval_Diff_VDF_var_Elem::coeffs_face n'est pas codee"<<finl;
      Cerr<<"pour la condition NSCBC"<<finl;
      exit();*/
  return 0;
}



//// flux_faces_interne
//

inline double CLASSNAME::flux_faces_interne(const DoubleTab& inco, int face) const
{
  double flux;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double d0 = la_zone->Dist_face_elem0(face,n0);
  double d1 = la_zone->Dist_face_elem1(face,n1);
  assert(nu_1(n0)!=0.0);
  assert(nu_1(n1)!=0.0);
  double heq;
  f_heq(d0,n0,d1,n1);
  flux = heq*(inco[n1] - inco[n0])*surface(face)*porosite(face);
  return flux;
}

//// coeffs_faces_interne

inline void CLASSNAME::coeffs_faces_interne(int face, double& aii, double& ajj) const
{
  int i = elem_(face,0);
  int j = elem_(face,1);
  double d0 = la_zone->Dist_face_elem0(face,i);
  double d1 = la_zone->Dist_face_elem1(face,j);
  assert(nu_1(i)!=0.0);
  assert(nu_1(j)!=0.0);
  double heq;
  f_heq(d0,i,d1,j);
  aii = ajj = heq*surface(face)*porosite(face);
}


//// secmem_faces_interne
//

inline double CLASSNAME::secmem_faces_interne( int ) const
{
  return 0;
}


