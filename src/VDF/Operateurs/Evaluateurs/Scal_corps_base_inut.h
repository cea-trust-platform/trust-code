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
// File:        Scal_corps_base_inut.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

//// flux_face avec Dirichlet_entree_fluide
//

inline double CLASSNAME::flux_face(const DoubleTab& inco, int face,
                                   const Dirichlet_entree_fluide& la_cl,
                                   int num1) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();

  return 0;
}

//// coeffs_face avec Dirichlet_entree_fluide
//

inline void CLASSNAME::coeffs_face(int face, int,
                                   const Dirichlet_entree_fluide& la_cl,
                                   double& aii, double& ajj) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec Dirichlet_entree_fluide
//

inline double CLASSNAME::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,
                                     int num1) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();

  return 0;
}


//// flux_face avec Dirichlet_paroi_defilante
//

inline double CLASSNAME::flux_face(const DoubleTab&, int ,
                                   const Dirichlet_paroi_defilante&, int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return 0;
}

//// coeffs_face avec Dirichlet_paroi_defilante
//

inline void CLASSNAME::coeffs_face(int , int,
                                   const Dirichlet_paroi_defilante&,
                                   double&, double& ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// secmem_face avec Dirichlet_paroi_defilante
//

inline double CLASSNAME::secmem_face(int, const Dirichlet_paroi_defilante&,
                                     int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return 0;
}


//// flux_face avec Dirichlet_paroi_fixe
//

inline double CLASSNAME::flux_face(const DoubleTab&, int ,
                                   const Dirichlet_paroi_fixe&, int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return 0;
}

//// coeffs_face avec Dirichlet_paroi_fixe
//

inline void CLASSNAME::coeffs_face(int , int,
                                   const Dirichlet_paroi_fixe&,
                                   double&, double& ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// secmem_face avec Dirichlet_paroi_fixe
//

inline double CLASSNAME::secmem_face(int, const Dirichlet_paroi_fixe&,
                                     int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return 0;
}


//// flux_face avec Echange_externe_impose
//

inline double CLASSNAME::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                   const Echange_externe_impose& la_cl,
                                   int num1) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();

  return 0;
}

//// coeffs_face avec Echange_externe_impose
//

inline void CLASSNAME::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                   const Echange_externe_impose& la_cl,
                                   double& aii, double& ajj) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec Echange_externe_impose
//

inline double CLASSNAME::secmem_face(int boundary_index, int face, int local_face, const Echange_externe_impose& la_cl,
                                     int num1) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  return 0;
}


//// flux_face avec Echange_global_impose
//

inline double CLASSNAME::flux_face(const DoubleTab& inco, int face,
                                   const Echange_global_impose& la_cl,
                                   int num1) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();

  return 0;
}

//// coeffs_face avec Echange_global_impose
//

inline void CLASSNAME::coeffs_face(int face, int num1,
                                   const Echange_global_impose& la_cl,
                                   double& aii, double& ajj) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec Echange_global_impose
//

inline double CLASSNAME::secmem_face(int face, const Echange_global_impose& la_cl,
                                     int num1) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();

  return 0;
}


//// flux_face avec Neumann_paroi
//

inline double CLASSNAME::flux_face(const DoubleTab& , int face ,
                                   const Neumann_paroi& la_cl,
                                   int num1) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return la_cl.flux_impose(face-num1)*surface(face);
}

//// coeffs_face avec Neumann_paroi
//

inline void CLASSNAME::coeffs_face(int face, int,
                                   const Neumann_paroi& la_cl,
                                   double& aii, double& ajj) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// secmem_face avec Neumann_paroi
//

inline double CLASSNAME::secmem_face(int face, const Neumann_paroi& la_cl,
                                     int num1) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return la_cl.flux_impose(face-num1)*surface(face);
}


//// flux_face avec Neumann_paroi_adiabatique
//

inline double CLASSNAME::flux_face(const DoubleTab& , int ,
                                   const Neumann_paroi_adiabatique&,
                                   int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return 0;
}

//// coeffs_face avec Neumann_paroi_adiabatique
//

inline void CLASSNAME::coeffs_face(int , int,
                                   const Neumann_paroi_adiabatique&,
                                   double&, double&) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// secmem_face avec Neumann_paroi_adiabatique
//

inline double CLASSNAME::secmem_face(int, const Neumann_paroi_adiabatique&,
                                     int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return 0;
}


//// flux_face avec Neumann_sortie_libre
//

inline double CLASSNAME::flux_face(const DoubleTab&, int ,
                                   const Neumann_sortie_libre&, int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return 0;
}

//// coeffs_face avec Neumann_sortie_libre
//

inline void CLASSNAME::coeffs_face(int , int,
                                   const Neumann_sortie_libre& ,
                                   double&, double&) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// secmem_face avec Neumann_sortie_libre
//

inline double CLASSNAME::secmem_face(int, const Neumann_sortie_libre& ,
                                     int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return 0;
}

//// flux_face avec Symetrie
//

inline double CLASSNAME::flux_face(const DoubleTab&, int ,
                                   const Symetrie&, int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return 0;
}

//// coeffs_face avec Symetrie
//

inline void CLASSNAME::coeffs_face(int , int,
                                   const Symetrie&, double&, double& ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// secmem_face avec Symetrie
//

inline double CLASSNAME::secmem_face(int, const Symetrie&, int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return 0;
}


//// flux_face avec Periodique
//

inline double CLASSNAME::flux_face(const DoubleTab& inco, int face,
                                   const Periodique& la_cl, int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();


  return 0;
}

//// coeffs_face avec Periodique

inline void CLASSNAME::coeffs_face(int face, int,
                                   const Periodique& la_cl,
                                   double& aii, double& ajj) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec Periodique
//

inline double CLASSNAME::secmem_face(int,
                                     const Periodique&,
                                     int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return 0;
}


//// flux_face avec NSCBC
//

inline double CLASSNAME::flux_face(const DoubleTab& inco, int face,
                                   const NSCBC& la_cl, int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();


  return 0;
}

//// coeffs_face avec NSCBC

inline void CLASSNAME::coeffs_face(int face, int,
                                   const NSCBC& la_cl,
                                   double& aii, double& ajj) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec NSCBC
//

inline double CLASSNAME::secmem_face(int,
                                     const NSCBC&,
                                     int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return 0;
}




//// flux_faces_interne
//

inline double CLASSNAME::flux_faces_interne(const DoubleTab& inco, int face) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();

  return 0;
}

//// coeffs_faces_interne

inline void CLASSNAME::coeffs_faces_interne(int face, double& aii, double& ajj) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();

}


//// secmem_faces_interne
//

inline double CLASSNAME::secmem_faces_interne( int ) const
{
  Cerr<<" CLASSNAME scalaire "<<finl;
  assert(0);
  Process::exit();
  return 0;
}

