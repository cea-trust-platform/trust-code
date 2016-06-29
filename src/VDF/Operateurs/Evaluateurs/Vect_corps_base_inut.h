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
// File:        Vect_corps_base_inut.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

//// flux_face avec Dirichlet_entree_fluide
//

inline void CLASSNAME::flux_face(const DoubleTab& inco, int face,
                                 const Dirichlet_entree_fluide& la_cl,
                                 int num1,DoubleVect& flux) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Dirichlet_entree_fluide
//

inline void CLASSNAME::coeffs_face(int face,int, const Dirichlet_entree_fluide& la_cl,DoubleVect& aii, DoubleVect& ajj) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec Dirichlet_entree_fluide
//

inline void CLASSNAME::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,  int num1,DoubleVect& flux) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// flux_face avec Dirichlet_paroi_defilante
//

inline void CLASSNAME::flux_face(const DoubleTab&, int ,
                                 const Dirichlet_paroi_defilante&,
                                 int, DoubleVect&  ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Dirichlet_paroi_defilante
//

inline void CLASSNAME::coeffs_face(int , int,
                                   const Dirichlet_paroi_defilante&,
                                   DoubleVect&, DoubleVect& ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec Dirichlet_paroi_defilante
//

inline void CLASSNAME::secmem_face(int, const Dirichlet_paroi_defilante&,
                                   int, DoubleVect& ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}


//// flux_face avec Dirichlet_paroi_fixe
//

inline void CLASSNAME::flux_face(const DoubleTab&, int ,
                                 const Dirichlet_paroi_fixe&,
                                 int, DoubleVect&  ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Dirichlet_paroi_fixe
//

inline void CLASSNAME::coeffs_face(int,int, const Dirichlet_paroi_fixe&,
                                   DoubleVect&, DoubleVect& ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec Dirichlet_paroi_fixe
//

inline void CLASSNAME::secmem_face(int, const Dirichlet_paroi_fixe&,
                                   int, DoubleVect& ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}


//// flux_face avec Echange_externe_impose
//

inline void CLASSNAME::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                 const Echange_externe_impose& la_cl,
                                 int num1,DoubleVect& flux) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Echange_externe_impose
//

inline void CLASSNAME::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                   const Echange_externe_impose& la_cl,
                                   DoubleVect& aii, DoubleVect& ajj) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// secmem

inline void CLASSNAME::secmem_face(int boundary_index, int face, int local_face, const Echange_externe_impose& la_cl,
                                   int num1,DoubleVect& flux) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// flux_face avec Echange_global_impose
//

inline void CLASSNAME::flux_face(const DoubleTab& inco, int face ,
                                 const Echange_global_impose& la_cl,
                                 int num1,DoubleVect& flux) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Echange_global_impose
//

inline void CLASSNAME::coeffs_face(int face,int num1,  const Echange_global_impose& la_cl, DoubleVect& aii, DoubleVect& ajj ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();


}

//// secmem_face avec Echange_global_impose
//

inline void CLASSNAME::secmem_face(int face, const Echange_global_impose& la_cl,
                                   int num1,DoubleVect& flux) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}


//// flux_face avec Neumann_paroi
//

inline void CLASSNAME::flux_face(const DoubleTab& , int face,
                                 const Neumann_paroi& la_cl,
                                 int num1,DoubleVect& flux) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Neumann_paroi
//

inline void CLASSNAME::coeffs_face(int ,int,
                                   const Neumann_paroi& ,
                                   DoubleVect& , DoubleVect& ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec Neumann_paroi
//

inline void CLASSNAME::secmem_face(int face, const Neumann_paroi& la_cl,
                                   int num1, DoubleVect& flux) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();
}


//// flux_face avec Neumann_paroi_adiabatique
//

inline void CLASSNAME::flux_face(const DoubleTab& , int ,
                                 const Neumann_paroi_adiabatique&,
                                 int ,DoubleVect& ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Neumann_paroi_adiabatique
//

inline void CLASSNAME::coeffs_face(int ,int,
                                   const Neumann_paroi_adiabatique&,
                                   DoubleVect&, DoubleVect& ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec Neumann_paroi_adiabatique
//

inline void CLASSNAME::secmem_face(int, const Neumann_paroi_adiabatique&,
                                   int, DoubleVect& ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// flux_face avec Neumann_sortie_libre
//

inline void CLASSNAME::flux_face(const DoubleTab& , int ,
                                 const Neumann_sortie_libre& ,
                                 int ,DoubleVect& ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// coeffs_face avec Neumann_sortie_libre
//

inline void CLASSNAME::coeffs_face(int,int, const Neumann_sortie_libre& ,
                                   DoubleVect&, DoubleVect&) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// secmem_face avec Neumann_sortie_libre
//

inline void CLASSNAME::secmem_face(int, const Neumann_sortie_libre& ,
                                   int , DoubleVect& ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// flux_face avec Symetrie
//

inline void CLASSNAME::flux_face(const DoubleTab&, int ,
                                 const Symetrie&,
                                 int ,DoubleVect& ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// coeffs_face avec Symetrie
//

inline void CLASSNAME::coeffs_face(int, int,const Symetrie&,
                                   DoubleVect&, DoubleVect&) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// secmem_face avec Symetrie
//

inline void CLASSNAME::secmem_face(int, const Symetrie&,
                                   int, DoubleVect&) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// flux_face avec Periodique
//

inline void CLASSNAME::flux_face(const DoubleTab& inco, int face,
                                 const Periodique& la_cl,
                                 int , DoubleVect& flux) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Periodique
//

inline void CLASSNAME::coeffs_face(int face,int, const Periodique& la_cl,
                                   DoubleVect& aii, DoubleVect& ajj ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}


//// secmem_face avec Periodique
//

inline void CLASSNAME::secmem_face(int, const Periodique&,
                                   int, DoubleVect&) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// flux_faces_interne
//

inline void CLASSNAME::flux_faces_interne(const DoubleTab& inco,
                                          int face,DoubleVect& flux) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_faces_interne
//

inline void CLASSNAME::coeffs_faces_interne(int face, DoubleVect& aii, DoubleVect& ajj ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();

}


//// secmem_faces_interne
//

inline void CLASSNAME::secmem_faces_interne( int, DoubleVect& ) const
{
  Cerr<<" CLASSNAME vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

