/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Vect_corps_base.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/8
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
#define Dist_norm_bord_externe(boundary_index,global_face,local_face) la_zone->dist_norm_bord_axi(global_face)
#endif
#else
#define Dist_norm_bord(face) MULTD dist_norm_bord(face)
#define Dist_face_elem0(face,n0) dist_face_elem0(face,n0)
#define Dist_face_elem1(face,n1) dist_face_elem1(face,n1)
#ifndef DEQUIV
#define Dist_norm_bord_externe(boundary_index,global_face,local_face) dist_norm_bord(global_face)
#endif
#endif

#ifdef DEQUIV
#define Dist_norm_bord_externe(boundary_index,global_face,local_face) equivalent_distance[boundary_index](local_face)
#endif
// Fonctions de calcul des flux pour une grandeur vectorielle

//// flux_face avec Dirichlet_entree_fluide
//

inline void CLASSNAME::flux_face(const DoubleTab& inco, int face,
                                 const Dirichlet_entree_fluide& la_cl,
                                 int num1,DoubleVect& flux) const
{
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int k;
  double dist = Dist_norm_bord(face);
  if (n0 != -1)
#ifdef ISQUASI
    for (k=0; k<flux.size(); k++)
      flux(k) = (la_cl.val_imp(face-num1,k)-inco(n0,k))/dv_mvol(n0)
                *surface(face)*porosite(face)*nu_1(n0,k)/dist;
#else
    for (k=0; k<flux.size(); k++)
      flux(k) = (la_cl.val_imp(face-num1,k)-inco(n0,k))
                *surface(face)*porosite(face)*nu_1(n0,k)/dist;
#endif
  else  // n1 != -1
#ifdef ISQUASI
    for (k=0; k<flux.size(); k++)
      flux(k) = (inco(n1,k)-la_cl.val_imp(face-num1,k))/dv_mvol(n1)
                *surface(face)*porosite(face)*nu_1(n1,k)/dist;
#else
    for (k=0; k<flux.size(); k++)
      flux(k) = (inco(n1,k)-la_cl.val_imp(face-num1,k))
                *surface(face)*porosite(face)*nu_1(n1,k)/dist;
#endif
}

//// coeffs_face avec Dirichlet_entree_fluide
//

inline void CLASSNAME::coeffs_face(int face,int, const Dirichlet_entree_fluide& la_cl,DoubleVect& aii, DoubleVect& ajj) const
{
  int k;
  int i = elem_(face,0);
  // int j = elem(face,1);
  double dist = Dist_norm_bord(face);

  if (i != -1)
    {
      for (k=0; k<aii.size(); k++)
        aii(k) = surface(face)*porosite(face)*nu_1(i,k)/dist;
      for (k=0; k<ajj.size(); k++)
        ajj(k) = 0;
    }
  else  // j != -1
    {
      for (k=0; k<ajj.size(); k++)
        ajj(k) = surface(face)*(porosite(face)*nu_1(elem_(face,1),k))/dist;
      for (k=0; k<aii.size(); k++)
        aii(k) = 0;
    }
}

//// secmem_face avec Dirichlet_entree_fluide
//

inline void CLASSNAME::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,  int num1,DoubleVect& flux) const
{
  int i = elem_(face,0);
  //  int j = elem(face,1);
  int k;
  double dist = Dist_norm_bord(face);

  if (i != -1)
    for (k=0; k<flux.size(); k++)
      flux(k) = la_cl.val_imp(face-num1,k)
                *surface(face)*porosite(face)*nu_1(i,k)/dist;

  else // j != -1
    for (k=0; k<flux.size(); k++)
      flux(k) = -la_cl.val_imp(face-num1,k)
                *surface(face)*(porosite(face)*nu_1(elem_(face,1),k))/dist;
}

//// flux_face avec Dirichlet_paroi_defilante
//

inline void CLASSNAME::flux_face(const DoubleTab&, int ,
                                 const Dirichlet_paroi_defilante&,
                                 int, DoubleVect&  ) const
{
  // coder dans evbasreyconst comme entree_fluide ????
  assert(0);
  Process::exit();
  ;
}

//// coeffs_face avec Dirichlet_paroi_defilante
//

inline void CLASSNAME::coeffs_face(int , int,
                                   const Dirichlet_paroi_defilante&,
                                   DoubleVect&, DoubleVect& ) const
{
  // coder dans evbasreyconst comme entree_fluide ????
  assert(0);
  Process::exit();
  ;
}

//// secmem_face avec Dirichlet_paroi_defilante
//

inline void CLASSNAME::secmem_face(int, const Dirichlet_paroi_defilante&,
                                   int, DoubleVect& ) const
{
  // coder dans evbasreyconst comme entree_fluide ????
  assert(0);
  Process::exit();
  ;
}


//// flux_face avec Dirichlet_paroi_fixe
//

inline void CLASSNAME::flux_face(const DoubleTab& inco, int face,
                                 const Dirichlet_paroi_fixe&,
                                 int num1, DoubleVect& flux ) const
{
  // coder dans evbasreyconst comme entree_fluide ????
  //
  flux=0;;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int k;
  double dist = Dist_norm_bord(face);
  if (n0 != -1)
#ifdef ISQUASI
    for (k=0; k<flux.size(); k++)
      flux(k) = (-inco(n0,k))
                *surface(face)*porosite(face)*nu_1(n0,k)/dv_mvol(n0)/dist;
#else
    for (k=0; k<flux.size(); k++)
      flux(k) = (-inco(n0,k))
                *surface(face)*porosite(face)*nu_1(n0,k)/dist;
#endif
  else  // n1 != -1
#ifdef ISQUASI
    for (k=0; k<flux.size(); k++)
      flux(k) = (inco(n1,k))
                *surface(face)*porosite(face)*nu_1(n1,k)/dv_mvol(n1)/dist;
#else
    for (k=0; k<flux.size(); k++)
      flux(k) = (inco(n1,k))
                *surface(face)*porosite(face)*nu_1(n1,k)/dist;
#endif
}

//// coeffs_face avec Dirichlet_paroi_fixe
//

inline void CLASSNAME::coeffs_face(int face,int num1, const Dirichlet_paroi_fixe&,
                                   DoubleVect& aii , DoubleVect& ajj) const
{
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int k;
  double dist = Dist_norm_bord(face);
  if (n0 != -1)
    {
#ifdef ISQUASI
      for (k=0; k<aii.size(); k++)
        aii(k) =
          surface(face)*porosite(face)*nu_1(n0,k)/dv_mvol(n0)/dist;
#else
      for (k=0; k<aii.size(); k++)
        aii(k) =
          surface(face)*porosite(face)*nu_1(n0,k)/dist;
#endif
      for (k=0; k<aii.size(); k++)
        ajj(k) =0;
    }

  else  // n1 != -1
    {
      if (n1==-1) Process::exit();
#ifdef ISQUASI
      for (k=0; k<ajj.size(); k++)
        ajj(k) =
          surface(face)*porosite(face)*nu_1(n1,k)/dv_mvol(n1)/dist;
#else
      for (k=0; k<ajj.size(); k++)
        ajj(k) =
          surface(face)*porosite(face)*nu_1(n1,k)/dist;

#endif
      for (k=0; k<ajj.size(); k++)
        aii(k) =0.;

    }
}

//// secmem_face avec Dirichlet_paroi_fixe
//

inline void CLASSNAME::secmem_face(int face, const Dirichlet_paroi_fixe&,
                                   int, DoubleVect& flux) const
{
  // coder dans evbasreyconst comme entree_fluide ????
  flux=0  ;
}


//// flux_face avec Echange_externe_impose
//

inline void CLASSNAME::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                 const Echange_externe_impose& la_cl,
                                 int num1,DoubleVect& flux) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

#ifdef ISQUASI
  Cerr<<__FILE__<< " QC BORD non code ligne "<<(int)__LINE__<<finl;
  Process::exit();
#endif
  double h_total_inv,e;
  int k;
  //  int ori = orientation(face);
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  e=Dist_norm_bord_externe(boundary_index,face,local_face);
  if (n0 != -1)
    {
      //e = la_zone->xv(face,ori) - la_zone->xp(n0,ori);
      for (k=0; k<flux.size(); k++)
        {
          h_total_inv = 1/la_cl.h_imp(face-num1,k) + e/nu_2(n0,k);
          flux(k) = (la_cl.T_ext(face-num1,k) - inco(n0,k))*surface(face)/h_total_inv;
        }
    }
  else
    {
      //    e = la_zone->xp(n1,ori) - la_zone->xv(face,ori);
      for (k=0; k<flux.size(); k++)
        {
          h_total_inv = 1/la_cl.h_imp(face-num1,k) + e/nu_2(n1,k);
          flux(k) = (inco(n1,k) - la_cl.T_ext(face-num1,k))*surface(face)/h_total_inv;
        }
    }
}

//// coeffs_face avec Echange_externe_impose
//

inline void CLASSNAME::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                   const Echange_externe_impose& la_cl,
                                   DoubleVect& aii, DoubleVect& ajj) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double e,h_total_inv;
  int i = elem_(face,0);
  //  int j = elem(face,1);
  //  int ori = orientation(face);
  int k;
  e=Dist_norm_bord_externe(boundary_index, face, local_face);
  if (i != -1)
    {
      //      e = la_zone->xv(face,ori) - la_zone->xp(i,ori);
      for (k=0; k<aii.size(); k++)
        {
          h_total_inv =  1/la_cl.h_imp(face-num1,k) + e/nu_2(i,k);
          aii(k) = surface(face)/h_total_inv;
        }
      for (k=0; k<ajj.size(); k++)
        ajj(k) = 0;
    }
  else
    {
      //e = la_zone->xp(j,ori) - la_zone->xv(face,ori);
      for (k=0; k<ajj.size(); k++)
        {
          h_total_inv = 1/la_cl.h_imp(face-num1,k) + e/nu_2(elem_(face,1),k);
          ajj(k) = surface(face)/h_total_inv;
        }
      for (k=0; k<aii.size(); k++)
        aii(k) = 0;
    }
}

//// secmem

inline void CLASSNAME::secmem_face(int boundary_index, int face, int local_face, const Echange_externe_impose& la_cl,
                                   int num1,DoubleVect& flux) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double e,h_total_inv;
  int k;
  double h_imp;
  int i = elem_(face,0);
  //  int j = elem(face,1);
  //  int ori = orientation(face);
  e=Dist_norm_bord_externe(boundary_index, face, local_face);
  if (i != -1)
    {
      //e = la_zone->xv(face,ori) - la_zone->xp(i,ori);
      for (k=0; k<flux.size(); k++)
        {
          h_imp = la_cl.h_imp(face-num1,k);
          h_total_inv = 1/h_imp + e/nu_2(i,k);
          flux(k) = la_cl.T_ext(face-num1,k) *surface(face)/h_total_inv;
        }
    }
  else
    {
      //e = la_zone->xp(j,ori) - la_zone->xv(face,ori);
      for (k=0; k<flux.size(); k++)
        {
          h_imp = la_cl.h_imp(face-num1,k);
          h_total_inv = 1/h_imp + e/nu_2(elem_(face,1),k);
          flux(k) =  - la_cl.T_ext(face-num1,k)*surface(face)/h_total_inv;
        }
    }
}

//// flux_face avec Echange_global_impose
//

inline void CLASSNAME::flux_face(const DoubleTab& inco, int face ,
                                 const Echange_global_impose& la_cl,
                                 int num1,DoubleVect& flux) const
{
#ifdef ISQUASI
  Cerr<<__FILE__<< " QC BORD non code ligne "<<(int)__LINE__<<finl;
  Process::exit();
#endif
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int k;
  if (n0 != -1)
    for (k=0; k<flux.size(); k++)
      flux(k) = la_cl.h_imp(face-num1,k)*(la_cl.T_ext(face-num1,k) - inco(n0,k))
                *surface(face);
  else
    for (k=0; k<flux.size(); k++)
      flux(k) = la_cl.h_imp(face-num1,k)*(inco(n1,k) - la_cl.T_ext(face-num1,k))
                *surface(face);
}

//// coeffs_face avec Echange_global_impose
//

inline void CLASSNAME::coeffs_face(int face,int num1,  const Echange_global_impose& la_cl, DoubleVect& aii, DoubleVect& ajj ) const
{
  int i = elem_(face,0);
  //  int j = elem(face,1);
  int k;
  if (i != -1)
    {
      for (k=0; k<aii.size(); k++)
        aii(k) = la_cl.h_imp(face-num1,k)*surface(face);
      for (k=0; k<ajj.size(); k++)
        ajj(k) = 0;
    }
  else
    {
      for (k=0; k<ajj.size(); k++)
        ajj(k) = la_cl.h_imp(face-num1,k)*surface(face);
      for (k=0; k<aii.size(); k++)
        aii(k) = 0;
    }
}

//// secmem_face avec Echange_global_impose
//

inline void CLASSNAME::secmem_face(int face, const Echange_global_impose& la_cl,
                                   int num1,DoubleVect& flux) const
{
  int k;
  double h;
  int i = elem_(face,0);
  //int j = elem(face,1);
  if (i != -1)
    for (k=0; k<flux.size(); k++)
      {
        h = la_cl.h_imp(face-num1,k);
        flux(k) = h*la_cl.T_ext(face-num1,k)*surface(face);
      }
  else
    for (k=0; k<flux.size(); k++)
      {
        h = la_cl.h_imp(face-num1,k);
        flux(k) = -h*la_cl.T_ext(face-num1,k)*surface(face);
      }
}


//// flux_face avec Neumann_paroi
//

inline void CLASSNAME::flux_face(const DoubleTab& , int face,
                                 const Neumann_paroi& la_cl,
                                 int num1,DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++)
    flux(k) = la_cl.flux_impose(face-num1,k)*surface(face);
}

//// coeffs_face avec Neumann_paroi
//

inline void CLASSNAME::coeffs_face(int ,int,
                                   const Neumann_paroi& ,
                                   DoubleVect& , DoubleVect& ) const
{
  ;
}

//// secmem_face avec Neumann_paroi
//

inline void CLASSNAME::secmem_face(int face, const Neumann_paroi& la_cl,
                                   int num1, DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++)
    flux(k) = la_cl.flux_impose(face-num1,k)*surface(face);
}


//// flux_face avec Neumann_paroi_adiabatique
//

inline void CLASSNAME::flux_face(const DoubleTab& , int ,
                                 const Neumann_paroi_adiabatique&,
                                 int ,DoubleVect& ) const
{
  ;
}

//// coeffs_face avec Neumann_paroi_adiabatique
//

inline void CLASSNAME::coeffs_face(int ,int,
                                   const Neumann_paroi_adiabatique&,
                                   DoubleVect&, DoubleVect& ) const
{
  ;
}

//// secmem_face avec Neumann_paroi_adiabatique
//

inline void CLASSNAME::secmem_face(int, const Neumann_paroi_adiabatique&,
                                   int, DoubleVect& ) const
{
  ;
}

//// flux_face avec Neumann_sortie_libre
//

inline void CLASSNAME::flux_face(const DoubleTab& , int ,
                                 const Neumann_sortie_libre& ,
                                 int ,DoubleVect& flux ) const
{
  for (int k=0; k<flux.size(); k++)
    {
      if (flux(k)!=0)
        {
          cerr<<" et OUI"<<endl;
          Process::exit();
        }
    }

}

//// coeffs_face avec Neumann_sortie_libre
//

inline void CLASSNAME::coeffs_face(int,int, const Neumann_sortie_libre& ,
                                   DoubleVect&, DoubleVect&) const
{
  ;
}

//// secmem_face avec Neumann_sortie_libre
//

inline void CLASSNAME::secmem_face(int, const Neumann_sortie_libre& ,
                                   int , DoubleVect& ) const
{
  ;
}

//// flux_face avec Symetrie
//

inline void CLASSNAME::flux_face(const DoubleTab&, int ,
                                 const Symetrie&,
                                 int ,DoubleVect& ) const
{
  ;
}

//// coeffs_face avec Symetrie
//

inline void CLASSNAME::coeffs_face(int, int,const Symetrie&,
                                   DoubleVect&, DoubleVect&) const
{
  ;
}

//// secmem_face avec Symetrie
//

inline void CLASSNAME::secmem_face(int, const Symetrie&,
                                   int, DoubleVect&) const
{
  ;
}

//// flux_face avec Periodique
//

inline void CLASSNAME::flux_face(const DoubleTab& inco, int face,
                                 const Periodique& la_cl,
                                 int , DoubleVect& flux) const
{
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double d0 = la_zone->dist_face_elem0_period(face,n0,la_cl.distance());
  double d1 = la_zone->dist_face_elem1_period(face,n1,la_cl.distance());
  double heq;
#ifdef ISQUASI
  for (int k=0; k<flux.size(); k++)
    {
      f_heq(d0,n0,d1,n1,k);
      flux(k) = heq*(inco(n1,k)/dv_mvol(n1) - inco(n0,k)/dv_mvol(n0))*surface(face)*porosite(face);
    }
#else
  for (int k=0; k<flux.size(); k++)
    {
      f_heq(d0,n0,d1,n1,k);
      flux(k) = heq*(inco(n1,k) - inco(n0,k))*surface(face)*porosite(face);
    }
#endif
}

//// coeffs_face avec Periodique
//

inline void CLASSNAME::coeffs_face(int face,int, const Periodique& la_cl,
                                   DoubleVect& aii, DoubleVect& ajj ) const
{
  int k;
  int i = elem_(face,0);
  int j = elem_(face,1);
  double d0 = la_zone->dist_face_elem0_period(face,i,la_cl.distance());
  double d1 = la_zone->dist_face_elem1_period(face,j,la_cl.distance());
  double heq;

  for (k=0; k<aii.size(); k++)
    {
      f_heq(d0,i,d1,j,k);
      aii(k) =  heq*surface(face)*porosite(face);
    }
  for (k=0; k<ajj.size(); k++)
    {
      f_heq(d0,i,d1,j,k);
      ajj(k)=  heq*surface(face)*porosite(face);
    }
}


//// secmem_face avec Periodique
//

inline void CLASSNAME::secmem_face(int, const Periodique&,
                                   int, DoubleVect&) const
{
  ;
}


//// flux_faces_interne
//

inline void CLASSNAME::flux_faces_interne(const DoubleTab& inco,
                                          int face,DoubleVect& flux) const
{
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double d0 = la_zone->Dist_face_elem0(face,n0);
  double d1 = la_zone->Dist_face_elem1(face,n1);
  double heq;
  for (int k=0; k<flux.size(); k++)
    {
      f_heq(d0,n0,d1,n1,k);
#ifdef ISQUASI
      flux(k) = heq*(inco(n1,k)/dv_mvol(n1) - inco(n0,k)/dv_mvol(n0))*surface(face)*porosite(face);
#else
      flux(k) = heq*(inco(n1,k) - inco(n0,k))*surface(face)*porosite(face);
#endif
    }
}

//// coeffs_faces_interne
//

inline void CLASSNAME::coeffs_faces_interne(int face, DoubleVect& aii, DoubleVect& ajj ) const
{
  int k;
  int i = elem_(face,0);
  int j = elem_(face,1);
  double d0 = la_zone->Dist_face_elem0(face,i);
  double d1 = la_zone->Dist_face_elem1(face,j);
  double heq;
  for ( k=0; k<aii.size(); k++)
    {
      f_heq(d0,i,d1,j,k);
      aii(k) = heq*surface(face)*porosite(face);
    }
  for ( k=0; k<ajj.size(); k++)
    {
      f_heq(d0,i,d1,j,k);
      ajj(k) = heq*surface(face)*porosite(face);
    }
}


//// secmem_faces_interne
//

inline void CLASSNAME::secmem_faces_interne( int, DoubleVect& flux ) const
{
  int k;
  for ( k=0; k<flux.size(); k++)
    {
      flux(k)=0.;
    }
}

