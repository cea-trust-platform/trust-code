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
// File:        Eval_Quick_PolyMAC_Elem.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs/Evaluateurs
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Eval_Quick_PolyMAC_Elem_included
#define Eval_Quick_PolyMAC_Elem_included

#include <Eval_Amont_PolyMAC_Elem.h>
#include <Eval_PolyMAC_Elem.h>
#include <Zone_PolyMAC.h>
#include <Champ_P0_PolyMAC.h>

// .DESCRIPTION class Eval_Quick_PolyMAC_Elem
//
// Evaluateur PolyMAC pour la convection
// Le champ convecte est scalaire (Champ_P0_PolyMAC)
// Schema de convection Quick
// Rq:Les evaluateurs de flux convectifs calculent en fait le terme
// convectif qui figure au second membre de l'equation d'evolution
// c.a.d l'oppose du flux convectif

//
// .SECTION voir aussi Eval_Conv_PolyMAC



class Eval_Quick_PolyMAC_Elem : public Eval_Amont_PolyMAC_Elem
{

public:

  inline Eval_Quick_PolyMAC_Elem();

  inline int calculer_flux_faces_echange_externe_impose() const;
  inline int calculer_flux_faces_echange_global_impose() const;
  inline int calculer_flux_faces_entree_fluide() const;
  inline int calculer_flux_faces_paroi() const;
  inline int calculer_flux_faces_paroi_adiabatique() const;
  inline int calculer_flux_faces_paroi_defilante() const;
  inline int calculer_flux_faces_paroi_fixe() const;
  inline int calculer_flux_faces_sortie_libre() const;
  inline int calculer_flux_faces_symetrie() const;
  inline int calculer_flux_faces_periodique() const ;

  // Fonctions qui servent a calculer le flux de grandeurs scalaires
  // Elles sont de type double et renvoient le flux

  inline double flux_face(const DoubleTab&, int , const Dirichlet_entree_fluide&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Dirichlet_paroi_defilante&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Dirichlet_paroi_fixe&, int ) const;
  inline double flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Echange_global_impose&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Neumann_paroi&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Neumann_paroi_adiabatique&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Symetrie&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Periodique&, int ) const;
  inline double flux_faces_interne(const DoubleTab&, int ) const;

  // Fonctions qui servent a calculer le flux de grandeurs vectorielles
  // Elles sont de type void et remplissent le tableau flux

  inline void flux_face(const DoubleTab&, int , const Symetrie&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Periodique&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_sortie_libre&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Dirichlet_entree_fluide&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Dirichlet_paroi_fixe&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Dirichlet_paroi_defilante&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_paroi_adiabatique&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_paroi&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Echange_global_impose&,
                        int, DoubleVect& flux) const;
  //inline void flux_face(const DoubleTab&, int ,const Nouvelle_Cl_PolyMAC&,
  //                      int, DoubleVect& flux) const=0;

  inline void flux_faces_interne(const DoubleTab&, int ,
                                 DoubleVect& flux) const;


protected:

  inline double dim_elem(int , int ) const;
  inline double dist_elem(int , int, int ) const;
  inline int amont_amont(int , int ) const;
  inline double quick_fram(const double&, const int,
                           const int, const int, const int,
                           const int, const DoubleTab& ) const;
  inline void quick_fram(const double&, const int,
                         const int, const int, const int,
                         const int, const DoubleTab&, ArrOfDouble& ) const;

};

/////////////////////////////////////////////////////////////
// Fonctions inline de la classe Eval_Quick_PolyMAC_Elem
////////////////////////////////////////////////////////////

inline Eval_Quick_PolyMAC_Elem::Eval_Quick_PolyMAC_Elem()
{
}


//// calculer_flux_faces_echange_externe_impose
//

inline int Eval_Quick_PolyMAC_Elem::calculer_flux_faces_echange_externe_impose() const
{
  return 0;
}


//// calculer_flux_faces_echange_global_impose
//

inline int Eval_Quick_PolyMAC_Elem::calculer_flux_faces_echange_global_impose() const
{
  return 0;
}


//// calculer_flux_faces_entree_fluide
//

inline int Eval_Quick_PolyMAC_Elem::calculer_flux_faces_entree_fluide() const
{
  return 1;
}


//// calculer_flux_faces_paroi
//

inline int Eval_Quick_PolyMAC_Elem::calculer_flux_faces_paroi() const
{
  return 0;
}


//// calculer_flux_faces_paroi_adiabatique
//

inline int Eval_Quick_PolyMAC_Elem::calculer_flux_faces_paroi_adiabatique() const
{
  return 0;
}


//// calculer_flux_faces_paroi_defilante
//

inline int Eval_Quick_PolyMAC_Elem::calculer_flux_faces_paroi_defilante() const
{
  return 0;
}


//// calculer_flux_faces_paroi_fixe
//

inline int Eval_Quick_PolyMAC_Elem::calculer_flux_faces_paroi_fixe() const
{
  return 0;
}


//// calculer_flux_faces_sortie_libre
//

inline int Eval_Quick_PolyMAC_Elem::calculer_flux_faces_sortie_libre() const
{
  return 1;
}


//// calculer_flux_faces_symetrie
//

inline int Eval_Quick_PolyMAC_Elem::calculer_flux_faces_symetrie() const
{
  return 0;
}

//// calculer_flux_faces_periodique
//

inline int Eval_Quick_PolyMAC_Elem::calculer_flux_faces_periodique() const
{
  return 1;
}

//////////////////////////////////////////////////////////////
//  Fonctions de calcul des flux pour une grandeur scalaire
//////////////////////////////////////////////////////////////

//// flux_face avec Dirichlet_entree_fluide
//

inline double Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab& inco, int face,
                                                 const Dirichlet_entree_fluide& la_cl,
                                                 int num1) const
{
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double flux;
  double psc = dt_vitesse[face]*surface(face)*porosite(face);
  double T_imp = la_cl.val_imp(face-num1);
  if (n0 != -1)
    {
      if (psc > 0)
        flux= psc*inco[n0];
      else
        flux= psc*T_imp;
    }
  else   // n1 != -1
    {
      if (psc>0)
        flux= psc*T_imp;
      else
        flux= psc*inco[n1];
    }
  return -flux;
}
//// flux_face avec Dirichlet_paroi_defilante
//

inline double Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab&, int ,
                                                 const Dirichlet_paroi_defilante&, int ) const
{
  return 0;
}


//// flux_face avec Dirichlet_paroi_fixe
//
inline double Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab&, int ,
                                                 const Dirichlet_paroi_fixe&, int ) const
{
  return 0;
}




//// flux_face avec Echange_externe_impose
//

inline double Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab&, int , int, int,
                                                 const Echange_externe_impose&, int ) const
{
  return 0;
}

//// flux_face avec Echange_global_impose
//

inline double Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab&, int ,
                                                 const Echange_global_impose&, int ) const
{
  return 0;
}

//// flux_face avec Neumann_paroi
//

inline double Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab&, int ,
                                                 const Neumann_paroi&, int ) const
{
  return 0;
}

//// flux_face avec Neumann_paroi_adiabatique
//

inline double Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab&, int ,
                                                 const Neumann_paroi_adiabatique&, int ) const
{
  return 0;
}


//// flux_face avec Neumann_sortie_libre
//

inline double Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab& inco, int face,
                                                 const Neumann_sortie_libre& la_cl, int num1) const
{
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double flux;
  double psc = dt_vitesse[face]*surface(face)*porosite(face);
  if (n0 != -1)
    {
      if (psc > 0)
        flux= psc*inco[n0];
      else
        flux= psc*la_cl.val_ext(face-num1);
    }
  else   // n1 != -1
    {
      if (psc > 0)
        flux=psc*la_cl.val_ext(face-num1);
      else
        flux= psc*inco[n1];
    }
  return -flux;
}

//// flux_face avec Symetrie
//

inline double Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab&, int ,
                                                 const Symetrie&, int ) const
{
  return 0;
}

//// flux_face avec Periodique
//

inline double Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab& inco, int face,
                                                 const Periodique& la_cl, int ) const
{
  // 31/05/2002
  // ALEX C.
  double flux;
  double psc = dt_vitesse[face]*surface(face)*porosite(face);
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int n0_0 = amont_amont(face,0);
  int n1_1 = amont_amont(face,1);

  // on applique le schema Quick_fram
  flux = quick_fram(psc,n0,n1,n0_0,n1_1,face,inco);
  return -flux;
}

//// flux_faces_interne
//

inline double Eval_Quick_PolyMAC_Elem::flux_faces_interne(const DoubleTab& inco, int face) const
{
  double flux;
  double psc = dt_vitesse[face]*surface(face)*porosite(face);
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int n0_0 = amont_amont(face,0);
  int n1_1 = amont_amont(face,1);
  /*
       if ( (n0_0 == -1) || (n1_1 == -1) )
       // Alexandre C. : 19/02/2003
       // We do not use the first order upwind scheme as it was done before
       // because in LES computations all turbulence is damped and we can not
       // recover a proper behaviour of physical turbulent characteristics,
       // especially when using wall-functions.
       // Therefore we use the 2nd order centered scheme.
       flux = psc*(inco(n0)+inco(n1))/2.;

       Pierre L. 14/10/04: Correction car le centre explose sur le cas VALIDA
       On revient au quick en essayant d'ameliorer: on prend le quick si psc
       est encore favorable pour avoir les 3 points necessaires au calcul du
       quick. Cela est deja ce qui est fait pour le quick-sharp de l'evaluateur
       aux faces.
  */
  if ( (n0_0 == -1 && psc >= 0 ) || (n1_1 == -1 && psc <= 0 ) )
    {
      if (psc > 0)
        flux = psc*inco(n0);
      else
        flux = psc*inco(n1);
    }
  else  // on applique le schema Quick_fram
    flux = quick_fram(psc,n0,n1,n0_0,n1_1,face,inco);
  return -flux;
}


////////////////////////////////////////////////////////////////
// Fonctions de calcul des flux pour une grandeur vectorielle
////////////////////////////////////////////////////////////////

//// flux_face avec Dirichlet_entree_fluide
//

inline void Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab& inco, int face,
                                               const Dirichlet_entree_fluide& la_cl,
                                               int num1, DoubleVect& flux) const
{
  int k;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double psc = dt_vitesse(face)*surface(face)*porosite(face);

  if (n0 != -1)
    {
      if (psc > 0)
        for (k=0; k<flux.size(); k++)
          flux(k) = -psc*inco(n0,k);
      else
        for (k=0; k<flux.size(); k++)
          flux(k)= -psc*la_cl.val_imp(face-num1,k);
    }
  else // n1 != -1
    {
      if (psc>0)
        for (k=0; k<flux.size(); k++)
          flux(k)= -psc*la_cl.val_imp(face-num1,k);
      else
        for (k=0; k<flux.size(); k++)
          flux(k)= -psc*inco(n1,k);
    }
}
//// flux_face avec Dirichlet_paroi_defilante
//

inline void Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab&, int ,
                                               const Dirichlet_paroi_defilante&,
                                               int , DoubleVect& ) const
{
  ;
}

//// flux_face avec Dirichlet_paroi_fixe
//

inline void Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab&, int ,
                                               const Dirichlet_paroi_fixe&,
                                               int , DoubleVect& ) const
{
  ;
}
//// flux_face avec Echange_externe_impose
//

inline void Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab&, int , int, int,
                                               const Echange_externe_impose&,
                                               int , DoubleVect& ) const
{
  ;
}

//// flux_face avec Echange_global_impose
//

inline void Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab&, int ,
                                               const Echange_global_impose&,
                                               int , DoubleVect& ) const
{
  ;
}
//// flux_face avec Neumann_paroi
//

inline void Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab&, int ,
                                               const Neumann_paroi&,
                                               int , DoubleVect& ) const
{
  ;
}

//// flux_face avec Neumann_paroi_adiabatique
//

inline void Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab&, int ,
                                               const Neumann_paroi_adiabatique&,
                                               int , DoubleVect& ) const
{
  ;
}

//// flux_face avec Neumann_sortie_libre
//

inline void Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab& inco, int face,
                                               const Neumann_sortie_libre& la_cl,
                                               int num1, DoubleVect& flux) const
{
  int k;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double psc = dt_vitesse[face]*surface(face)*porosite(face);
  if (n0 != -1)
    {
      if (psc > 0)
        for (k=0; k<flux.size(); k++)
          flux(k) = -psc*inco(n0,k);
      else
        for (k=0; k<flux.size(); k++)
          flux(k) = -psc*la_cl.val_ext(face-num1,k);
    }
  else  // n1 != -1
    {
      if (psc > 0)
        for (k=0; k<flux.size(); k++)
          flux(k) = -psc*la_cl.val_ext(face-num1,k);
      else
        for (k=0; k<flux.size(); k++)
          flux(k) = -psc*inco(n1,k);
    }
}
//// flux_face avec Symetrie
//

inline void Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab&, int ,
                                               const Symetrie&,
                                               int , DoubleVect& ) const
{
  ;
}

//// flux_face avec Periodique
//

inline void Eval_Quick_PolyMAC_Elem::flux_face(const DoubleTab& inco, int face,
                                               const Periodique& la_cl,
                                               int, DoubleVect& flux) const
{
  // Codage Periodique
  //30/05/2002
  // ALEX C.
  int k;
  double psc = dt_vitesse[face]*surface(face)*porosite(face);
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int n0_0 = amont_amont(face,0);
  int n1_1 = amont_amont(face,1);

  // on applique le schema Quick
  quick_fram(psc,n0,n1,n0_0,n1_1,face,inco,flux);
  for (k=0; k<flux.size(); k++)
    flux(k) *= -1;
}


//// flux_faces_interne
//

inline void Eval_Quick_PolyMAC_Elem::flux_faces_interne(const DoubleTab& inco,
                                                        int face,DoubleVect& flux) const
{
  int k;
  double psc = dt_vitesse[face]*surface(face)*porosite(face);
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int n0_0 = amont_amont(face,0);
  int n1_1 = amont_amont(face,1);
  /*
    if ((n0_0 == -1)||(n1_1 == -1))
    // Alexandre C. : 19/02/2003
    // We do not use the first order upwind scheme as it was done before
    // because in LES computations all turbulence is damped and we can not
    // recover a proper behaviour of physical turbulent characteristics,
    // especially when using wall-functions !
    // Therefore we use the 2nd order centered scheme.
    for (k=0; k<flux.size(); k++)
    flux(k) = -psc*(inco(n0,k)+inco(n1,k))/2.;

    Pierre L. 14/10/04: Correction car le centre explose sur le cas VALIDA
    On revient au quick en essayant d'ameliorer: on prend le quick si psc
    est encore favorable pour avoir les 3 points necessaires au calcul du
    quick. Cela est deja ce qui est fait pour le quick-sharp de l'evaluateur
    aux faces.
  */
  if ( (n0_0 == -1 && psc >= 0 ) || (n1_1 == -1 && psc <= 0 ) )
    {
      if (psc > 0)
        for (k=0; k<flux.size(); k++)
          flux(k) = -psc*inco(n0,k);
      else
        for (k=0; k<flux.size(); k++)
          flux(k) = -psc*inco(n1,k);
    }
  else // on applique le schema Quick
    {
      quick_fram(psc,n0,n1,n0_0,n1_1,face,inco,flux);
      for (k=0; k<flux.size(); k++)
        flux(k) *= -1;
    }
}


//// Fram
//

inline double Fram(const double& s1,const double& s2,
                   const double& s3,const double& s4)
{
  double smin0 = dmin(s4,s2);
  double smax0 = dmax(s4,s2);
  double smin1 = dmin(s3,s1);
  double smax1 = dmax(s3,s1);
  /* double sr0 = (smax0-smin0==0 ? 0 : (s3-smin0)/(smax0-smin0)); */
  /* double sr1 = (smax1-smin1==0 ? 0 : (s2-smin1)/(smax1-smin1)); */
  double sr0 = (dabs(smax0-smin0)<DMINFLOAT ? 0. : (s3-smin0)/(smax0-smin0));
  double sr1 = (dabs(smax1-smin1)<DMINFLOAT ? 0. : (s2-smin1)/(smax1-smin1));
  double fr = 2.*dmax(dabs(sr0-0.5),dabs(sr1-0.5));
  fr *= fr;
  fr *= fr;
  return dmin(fr,1.0);
}


//// amont_amont
//

inline int Eval_Quick_PolyMAC_Elem::amont_amont(int face, int i) const
{
  return la_zone->amont_amont(face, i);
}


//// quick_fram pour un champ transporte scalaire
//

inline double Eval_Quick_PolyMAC_Elem::quick_fram(const double& psc, const int num0, const int num1,
                                                  const int num0_0, const int num1_1, const int face,
                                                  const DoubleTab& transporte) const
{

  double flux,trans_amont,fr;
  int ori = orientation(face);
  double dx = dist_elem(num0, num1, ori);
  double curv;
  double T0 = transporte[num0];
  double T1 = transporte[num1];
  double T0_0=-1,T1_1=-1;
  if (num0_0 != -1) T0_0 = transporte[num0_0];
  if (num1_1 != -1) T1_1 = transporte[num1_1];
  if (psc > 0)
    {
      assert(num0_0!=-1);
      trans_amont = T0;
      double dm = dim_elem(num0, ori);
      double dxam = dist_elem(num0_0, num0, ori);
      curv = ( (T1 - T0)/dx - (T0 - T0_0)/dxam )/dm ;
    }
  else
    {
      assert(num1_1!=-1);
      trans_amont = T1;
      double dm = dim_elem(num1, ori);
      double dxam = dist_elem(num1, num1_1, ori);
      curv = ( (T1_1 - T1)/dxam - (T1 - T0)/dx )/dm;
    }

  flux = 0.5*(T0+T1) - 0.125*(dx*dx)*curv;
  if ( num0_0 != -1 && num1_1 != -1 )
    fr = Fram(T0_0,T0,T1,T1_1);
  else
    fr = 1.;
  flux = ((1.-fr)*flux + fr*trans_amont)*psc;

  return flux;
}

//// quick_fram pour un champ transporte vectoriel
//

inline void Eval_Quick_PolyMAC_Elem::quick_fram(const double& psc, const int num0, const int num1,
                                                const int num0_0, const int num1_1, const int face,
                                                const DoubleTab& transporte,ArrOfDouble& flux) const
{
  int ori = orientation(face);
  double dx = dist_elem(num0, num1, ori);
  double T0,T0_0=0,T1,T1_1=0,trans_amont,curv;
  double fr;
  double dm0 = dim_elem(num0, ori);
  double dxam0 = (num0_0!=-1?dist_elem(num0_0, num0, ori):0);
  double dm1 = dim_elem(num1, ori);
  double dxam1 = (num1_1!=-1?dist_elem(num1, num1_1, ori):0);

  int ncomp = flux.size_array();
  for (int k=0; k<ncomp; k++)
    {
      T0 = transporte(num0,k);
      T0_0 = (num0_0!=-1?transporte(num0_0,k):0);
      T1 = transporte(num1,k);
      T1_1 = (num1_1!=-1?transporte(num1_1,k):0);

      if (psc > 0)
        {
          assert(num0_0!=-1);
          trans_amont = T0;
          curv = ( (T1 - T0)/dx - (T0 - T0_0)/dxam0 )/dm0 ;
        }
      else
        {
          assert(num1_1!=-1);
          trans_amont = T1;
          curv = ( (T1_1 - T1)/dxam1 - (T1 - T0)/dx )/dm1;
        }
      flux(k) = 0.5*(T0+T1) - 0.125*(dx*dx)*curv;
      // On applique le filtre Fram:
      if ( num0_0 != -1 && num1_1 != -1 )
        fr = Fram(T0_0,T0,T1,T1_1);
      else
        fr = 1.;
      flux(k) = ((1.-fr)*flux(k) + fr*trans_amont)*psc;
    }
}

inline double Eval_Quick_PolyMAC_Elem::dim_elem(int n1, int k) const
{
  return la_zone->dim_elem(n1,k);
}


inline double Eval_Quick_PolyMAC_Elem::dist_elem(int n1, int n2, int k) const
{
  return la_zone->dist_elem_period(n1,n2,k);
}

#endif
