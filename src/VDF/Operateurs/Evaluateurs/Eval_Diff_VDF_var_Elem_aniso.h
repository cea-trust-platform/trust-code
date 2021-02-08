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
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Diff_VDF_var_Elem_aniso_included
#define Eval_Diff_VDF_var_Elem_aniso_included

#include <Eval_Diff_VDF_var_aniso.h>
#include <Eval_VDF_Elem.h>

//
// .DESCRIPTION class Eval_Diff_VDF_var_Elem_aniso
//
// Evaluateur VDF pour la diffusion
// Le champ diffuse est scalaire (Champ_P0_VDF)
// Le champ de diffusivite n'est pas constant.

//
// .SECTION voir aussi Eval_Diff_VDF_var_aniso

class Eval_Diff_VDF_var_Elem_aniso : public Eval_Diff_VDF_var_aniso, public Eval_VDF_Elem
{

public:

  inline Eval_Diff_VDF_var_Elem_aniso() {}

  inline int calculer_flux_faces_echange_externe_impose() const
  {
    return 1;
  }
  inline int calculer_flux_faces_echange_global_impose() const
  {
    return 1;
  }
  inline int calculer_flux_faces_entree_fluide() const
  {
    return 1;
  }
  inline int calculer_flux_faces_paroi() const
  {
    return 1;
  }
  inline int calculer_flux_faces_paroi_adiabatique() const
  {
    return 0;
  }
  inline int calculer_flux_faces_paroi_defilante() const
  {
    return 0;
  }
  inline int calculer_flux_faces_paroi_fixe() const
  {
    return 1;
  }
  inline int calculer_flux_faces_sortie_libre() const
  {
    return 0;
  }
  inline int calculer_flux_faces_symetrie() const
  {
    return 0;
  }
  inline int calculer_flux_faces_periodique() const
  {
    return 1;
  }
  inline int calculer_flux_faces_NSCBC() const
  {
    return 0;
  }

  // Fonctions qui servent a calculer le flux de grandeurs scalaires
  // Elles sont de type double et renvoient le flux

  template<typename BC>
  inline double flux_face(const DoubleTab& inco, int face, const BC& la_cl, int num1) const
  {
    // Generic return
    return 0;
  }

  inline double flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&, int ) const;

  inline double flux_faces_interne(const DoubleTab&, int ) const;

  // Fonctions qui servent a calculer le flux de grandeurs vectorielles
  // Elles sont de type void et remplissent le tableau flux
  template<typename BC>
  inline void flux_face(const DoubleTab&, int , const BC&, int, DoubleVect& flux) const
  {
    Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
    assert(0);
    Process::exit();
  }

  void flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&, int ,DoubleVect& flux) const
  {
      Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
      assert(0);
      Process::exit();
  }


  inline void flux_faces_interne(const DoubleTab&, int ,
                                 DoubleVect& flux) const
  {
    Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
    assert(0);
    Process::exit();

  }

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // scalaires.

  template<typename BC>
  inline void coeffs_face(int,int, const BC&, double& aii, double& ajj ) const
  {
    /* Generic version
       Do nothing */
  }

  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, double& aii, double& ajj ) const;


  inline void coeffs_faces_interne(int, double& aii, double& ajj ) const;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l"implicite pour les grandeurs scalaires.

  template<typename BC>
  inline double secmem_face(int, const BC&, int ) const
  {
    /* Generic version */
    return 0;
  }

  inline double secmem_face(int, int, int, const Echange_externe_impose&, int ) const;
  inline double secmem_faces_interne(int ) const
  {
    return 0;
  }
  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // vectorielles.

  template<typename BC>
  inline void coeffs_face(int,int, const BC&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
    assert(0);
    Process::exit();
  }

  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, DoubleVect& aii, DoubleVect& ajj ) const
  {
    Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
    assert(0);
    Process::exit();
  }

  inline void coeffs_faces_interne(int, DoubleVect& aii, DoubleVect& ajj ) const
  {
    Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
    assert(0);
    Process::exit();
  }

  template<typename BC>
  inline void secmem_face(int, const BC&, int, DoubleVect& ) const
  {
    Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
    assert(0);
    Process::exit();
  }

  inline void secmem_face(int, int, int, const Echange_externe_impose&, int, DoubleVect& ) const
  {
    Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
    assert(0);
    Process::exit();
  }

  inline void secmem_faces_interne(int, DoubleVect& ) const
  {
    Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
    assert(0);
    Process::exit();
  }

protected:

private:

};


/*
 Fonctions de calcul des flux pour une grandeur scalaire.
 Templates specialization for specific BC :

 	 - Dirichlet_entree_fluide
 	 - Echange_global_impose
 	 - Neumann_paroi
 	 - Periodique
 	 - NSCDB
 */
template<>
inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& inco, int face, const Dirichlet_entree_fluide& la_cl, int num1) const
{
  double T_imp = la_cl.val_imp(face-num1);
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int ori= orientation(face);
  // Olga avait mis :
  // double dist = 2*Dist_norm_bord(face);
  // Pierre dit que :
  double dist = la_zone->dist_norm_bord(face);
  double flux;
  if (n0 != -1)
    {
      flux = (T_imp-inco[n0])*surface(face)/dist;
      flux *= (porosite(face)*dt_diffusivite(n0,ori));
    }
  else   // n1 != -1
    {
      flux = (inco[n1]-T_imp)*surface(face)/dist;
      flux*= (porosite(face)*dt_diffusivite(n1,ori));
    }
  return flux;
}


template<>
inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& inco, int face ,
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

template<>
inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& , int face,
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

template<>
inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& inco, int face,
                                                      const Periodique& la_cl, int ) const
{
  double flux;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double d0 = la_zone->dist_face_elem0_period(face,n0,la_cl.distance());
  double d1 = la_zone->dist_face_elem1_period(face,n1,la_cl.distance());
  double heq;
  int ori = orientation(face);
  heq=1./(d0/dt_diffusivite(n0,ori) + d1/dt_diffusivite(n1,ori));
  flux = heq*(inco[n1] - inco[n0])*surface(face)*porosite(face);
  return flux;
}

template<>
inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& inco , int faceD ,
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
  double heq = (dt_diffusivite(elem0,oriD)/d0);
  double flux = heq*(inco(elem0) - inco(elem3))*surface(faceD)*porosite(faceD);

  return flux;
}

//// flux_face avec Echange_externe_impose
//
inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
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
  e=la_zone->dist_norm_bord(face);
  int ori = orientation(face);
  double flux;
  if (n0 != -1)
    {
      //e = la_zone->xv(face,ori) - la_zone->xp(n0,ori);
      if (dt_diffusivite(n0,ori) == 0.0)
        {
          heq = h_imp; // Should be 0 ??
        }
      else
        {
          h_total_inv = 1.0/h_imp + e/dt_diffusivite(n0,ori);
          heq = 1.0 / h_total_inv;
        }

      flux = heq*(T_ext - inco[n0])*surface(face);
    }
  else
    {
      //e = la_zone->xp(n1,ori) - la_zone->xv(face,ori);
      if (dt_diffusivite(n1,ori) == 0.0)
        {
          heq = h_imp; // Should be 0 ??
        }
      else
        {
          h_total_inv = 1.0/h_imp + e/dt_diffusivite(n1,ori);
          heq = 1.0 / h_total_inv;
        }

      flux = heq*(inco[n1] - T_ext)*surface(face);
    }
  return flux;
}

//// flux_faces_interne
//

inline double Eval_Diff_VDF_var_Elem_aniso::flux_faces_interne(const DoubleTab& inco, int face) const
{
  double flux;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double d0 = la_zone->dist_face_elem0(face,n0);
  double d1 = la_zone->dist_face_elem1(face,n1);
  int ori = orientation(face);
  double heq;
  if (dt_diffusivite(n0,ori)==0.0 && dt_diffusivite(n1,ori)==0.0)
    {
      heq = 0.;
    }
  else
    {
      assert(dt_diffusivite(n0,ori)!=0.0);
      assert(dt_diffusivite(n1,ori)!=0.0);
      heq=1./(d0/dt_diffusivite(n0,ori) + d1/dt_diffusivite(n1,ori));
    }
  flux = heq*(inco[n1] - inco[n0])*surface(face)*porosite(face);
  return flux;
}


/*
 Fonctions de calcul des coefs_faces pour une grandeur scalaire.
 Templates specialization for specific BC :

 	 - Dirichlet_entree_fluide
 	 - Echange_global_impose
 	 - Neumann_paroi
 	 - Periodique
 	 - NSCDB
 */
template<>
inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int face, int,
                                                      const Dirichlet_entree_fluide& la_cl,
                                                      double& aii, double& ajj) const
{
  int i = elem_(face,0);
  int ori= orientation(face);
  //  int j = elem(face,1);
  double dist = la_zone->dist_norm_bord(face);
  if (i != -1)
    {
      aii =  porosite(face)*dt_diffusivite(i,ori)*surface(face)/dist;
      ajj = 0;
    }
  else   // j != -1
    {
      ajj =  porosite(face)*dt_diffusivite(elem_(face,1),ori)*surface(face)/dist;
      aii = 0;
    }
}

template<>
inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int face, int num1,
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

template<>
inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int , int,
                                                      const Neumann_paroi& ,
                                                      double& aii, double& ajj) const
{
  aii = 0;
  ajj = 0;
}

template<>
inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int face,int,
                                                      const Periodique& la_cl,
                                                      double& aii, double& ajj) const
{
  int i = elem_(face,0);
  int j = elem_(face,1);
  double d0 = la_zone->dist_face_elem0_period(face,i,la_cl.distance());
  double d1 = la_zone->dist_face_elem1_period(face,j,la_cl.distance());
  double heq;
  int ori = orientation(face);
  heq=1./(d0/dt_diffusivite(i,ori) + d1/dt_diffusivite(j,ori));
  aii = ajj = heq*surface(face)*porosite(face);
}

template<>
inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int face, int num1,const NSCBC& la_cl,
                                                      double& aii, double& ajj) const
{
  /*  Cerr<<"Eval_Diff_VDF_var_Elem::coeffs_face n'est pas codee"<<finl;
      Cerr<<"pour la condition NSCBC"<<finl;
      exit();*/
  aii = ajj = 0.;
}

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int boundary_index, int face, int local_face, int num1,
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
  e=la_zone->dist_norm_bord(face);
  if (i != -1)
    {
      // e = la_zone->xv(face,ori) - la_zone->xp(i,ori);
      if (dt_diffusivite(i,ori) == 0.0)
        {
          heq = h_imp; // Should be 0 ??
        }
      else
        {
          h_total_inv =  1.0/h_imp + e/dt_diffusivite(i,ori);
          heq = 1.0 / h_total_inv;
        }

      aii = heq*surface(face);
      ajj = 0;
    }
  else
    {
      // e = la_zone->xp(j,ori) - la_zone->xv(face,ori);
      if (dt_diffusivite(elem_(face,1),ori) == 0.0)
        {
          heq = h_imp; // Should be 0 ??
        }
      else
        {
          h_total_inv =  1.0/h_imp + e/dt_diffusivite(elem_(face,1),ori);
          heq = 1.0 / h_total_inv;
        }

      ajj = heq*surface(face);
      aii = 0;
    }
}

template<>
inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,
                                                        int num1) const
{
  double T_imp = la_cl.val_imp(face-num1);
  int i = elem_(face,0);
  int ori= orientation(face);
  //  int j = elem(face,1);
  double dist = la_zone->dist_norm_bord(face);

  double flux;
  if (i != -1)
    {
      flux = T_imp*surface(face)/dist;
      flux *= porosite(face)*dt_diffusivite(i,ori);
    }
  else   // j != -1
    {
      flux = -T_imp*surface(face)/dist;
      flux *= (porosite(face)*dt_diffusivite(elem_(face,1),ori));
    }
  return flux;
}

template<>
inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int face, const Echange_global_impose& la_cl,
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

template<>
inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int face, const Neumann_paroi& la_cl,
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


inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_faces_interne(int face, double& aii, double& ajj) const
{
  int i = elem_(face,0);
  int j = elem_(face,1);
  double d0 = la_zone->dist_face_elem0(face,i);
  double d1 = la_zone->dist_face_elem1(face,j);
  int ori = orientation(face);
  double heq;
  if (dt_diffusivite(i,ori)==0.0 && dt_diffusivite(j,ori)==0.0)
    {
      heq = 0.;
    }
  else
    {
      assert(dt_diffusivite(i,ori)!=0.0);
      assert(dt_diffusivite(j,ori)!=0.0);
      heq=1./(d0/dt_diffusivite(i,ori) + d1/dt_diffusivite(j,ori));
    }
  aii = ajj = heq*surface(face)*porosite(face);
}

inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int boundary_index,int face, int local_face, const Echange_externe_impose& la_cl,
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
  e=la_zone->dist_norm_bord(face);
  double flux;
  if (i != -1)
    {
      //e = la_zone->xv(face,ori) - la_zone->xp(i,ori);
      if (dt_diffusivite(i,ori) == 0.0)
        {
          heq = h_imp; // Should be 0 ??
        }
      else
        {
          h_total_inv = 1.0/h_imp + e/dt_diffusivite(i,ori);
          heq = 1.0 / h_total_inv;
        }

      flux = heq*T_ext*surface(face);
    }
  else
    {
      //e = la_zone->xp(j,ori) - la_zone->xv(face,ori);
      if (dt_diffusivite(elem_(face,1),ori) == 0.0)
        {
          heq = h_imp; // Should be 0 ??
        }
      else
        {
          h_total_inv = 1.0/h_imp + e/dt_diffusivite(elem_(face,1),ori);
          heq = 1.0 / h_total_inv;
        }

      flux = - heq*T_ext*surface(face);
    }
  return flux;
}


#endif
