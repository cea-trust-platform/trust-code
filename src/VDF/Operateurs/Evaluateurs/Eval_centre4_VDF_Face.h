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
// File:        Eval_centre4_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Eval_centre4_VDF_Face_included
#define Eval_centre4_VDF_Face_included

#include <Eval_Conv_VDF.h>
#include <Eval_VDF_Face.h>
#include <Zone_VDF.h>
//
// .DESCRIPTION class Eval_centre4_VDF_Face
//
// Evaluateur VDF pour la convection
// Le champ convecte est un Champ_Face
// Schema de convection centre4 (sur 4 points)
// Rq:Les evaluateurs de flux convectifs calculent en fait le terme
// convectif qui figure au second membre de l'equation d'evolution
// c.a.d l'oppose du flux convectif

//
// .SECTION voir aussi Eval_Conv_VDF


class Eval_centre4_VDF_Face : public Eval_Conv_VDF, public Eval_VDF_Face
{


public:
  inline Eval_centre4_VDF_Face();

  inline int calculer_arete_fluide() const ;
  inline int calculer_arete_paroi() const ;
  inline int calculer_arete_paroi_fluide() const ;
  inline int calculer_arete_symetrie() const ;
  inline int calculer_arete_interne() const ;
  inline int calculer_arete_mixte() const ;
  inline int calculer_fa7_sortie_libre() const ;
  inline int calculer_arete_periodicite() const;
  inline int calculer_arete_symetrie_paroi() const;
  inline int calculer_arete_symetrie_fluide() const;

  // Fonctions qui servent a calculer le flux de grandeurs scalaires
  // Elles sont de type double et renvoient le flux

  inline double flux_fa7_sortie_libre(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const;
  inline double flux_fa7_elem(const DoubleTab&, int, int, int) const ;
  inline double flux_arete_interne(const DoubleTab&, int, int, int, int) const ;
  inline double flux_arete_mixte(const DoubleTab&, int, int, int, int) const ;
  inline double flux_arete_paroi(const DoubleTab&, int, int, int, int ) const ;
  inline double flux_arete_symetrie(const DoubleTab&, int, int, int, int) const ;
  inline void flux_arete_fluide(const DoubleTab&, int, int, int, int,
                                double& ,double&) const ;
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int, int, int,
                                      double& ,double&) const ;
  inline void flux_arete_periodicite(const DoubleTab&, int, int, int, int,
                                     double&, double&) const ;
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int, int, int, double&, double&) const ;
  inline double flux_arete_symetrie_paroi(const DoubleTab&, int, int, int, int) const ;

  // Fonctions qui servent a calculer le flux de grandeurs vectorielles
  // Elles sont de type void et remplissent le tableau flux

  inline void flux_fa7_elem(const DoubleTab&, int, int, int, DoubleVect& flux) const;
  inline void flux_fa7_sortie_libre(const DoubleTab&, int , const Neumann_sortie_libre&,
                                    int, DoubleVect& flux) const;
  inline void flux_arete_interne(const DoubleTab&, int, int, int,
                                 int, DoubleVect& flux) const ;
  inline void flux_arete_mixte(const DoubleTab&, int, int, int,
                               int, DoubleVect& flux) const ;
  inline void flux_arete_symetrie(const DoubleTab&, int, int, int,
                                  int, DoubleVect& flux) const ;
  inline void flux_arete_paroi(const DoubleTab&, int, int, int,
                               int, DoubleVect& flux) const ;
  inline void flux_arete_fluide(const DoubleTab&, int, int, int,
                                int, DoubleVect&, DoubleVect&) const;
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int,
                                      int, int, DoubleVect& , DoubleVect&) const ;
  inline void flux_arete_periodicite(const DoubleTab&, int, int,
                                     int, int, DoubleVect&, DoubleVect& ) const ;
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int, int,
                                         int, DoubleVect&, DoubleVect& ) const;
  inline void flux_arete_symetrie_paroi(const DoubleTab&, int, int, int,
                                        int, DoubleVect& flux) const ;
protected:

private:
  inline double dist_face(int n1,int n2,int k) const
  {
    return la_zone->dist_face(n1,n2,k);
  };
  inline double dist_face_period(int n1,int n2,int k) const
  {
    return la_zone->dist_face_period(n1,n2,k);
  };
  inline double dist_elem_period(int n1,int n2,int k) const
  {
    return la_zone->dist_elem_period(n1,n2,k);
  };
  inline int face_amont_conj(int ,int ,int ) const;
  inline int face_amont_princ(int ,int ) const;
  inline double dim_elem(int n1,int k) const
  {
    return la_zone->dim_elem(n1,k);
  };
};

//
// Fonctions inline de la classe Eval_centre4_VDF_Face
//

inline Eval_centre4_VDF_Face::Eval_centre4_VDF_Face()
{
}


//// calculer_arete_fluide
//

inline int Eval_centre4_VDF_Face::calculer_arete_fluide() const
{
  return 1;
}


//// calculer_arete_paroi
//

inline int Eval_centre4_VDF_Face::calculer_arete_paroi() const
{
  return 0;
}


//// calculer_arete_paroi_fluide
//

inline int Eval_centre4_VDF_Face::calculer_arete_paroi_fluide() const
{
  return 1;
}


//// calculer_arete_symetrie
//

inline int Eval_centre4_VDF_Face::calculer_arete_symetrie() const
{
  return 0;
}

//// calculer_arete_interne
//

inline int Eval_centre4_VDF_Face::calculer_arete_interne() const
{
  return 1;
}

//// calculer_arete_mixte
//

inline int Eval_centre4_VDF_Face::calculer_arete_mixte() const
{
  return 1;
}

//// calculer_fa7_sortie_libre
//

inline int Eval_centre4_VDF_Face::calculer_fa7_sortie_libre() const
{
  return 1;
}

//// calculer_arete_periodicite
//

inline int Eval_centre4_VDF_Face::calculer_arete_periodicite() const
{
  return 1;
}

//// calculer_arete_symetrie_paroi
//

inline int Eval_centre4_VDF_Face::calculer_arete_symetrie_paroi() const
{
  return 0;
}

//// calculer_arete_symetrie_fluide
//

inline int Eval_centre4_VDF_Face::calculer_arete_symetrie_fluide() const
{
  return 1;
}

// Calcul des coefficients g1,g2,g3,g4 a partir de dxam,dx,dxav
inline void calcul_g(const double& dxam, const double& dx, const double& dxav, double& g1, double& g2, double& g3, double& g4)
{
  g1 = -dx*dx*(dx/2+dxav)/(4*(dx+dxam+dxav)*(dx+dxam)*dxam);
  g2 =  (dx+2*dxam)*(dx+2*dxav)/(8*dxam*(dx+dxav));
  g3 =  (dx+2*dxam)*(dx+2*dxav)/(8*dxav*(dx+dxam));
  g4 = -dx*dx*(dx/2+dxam)/(4*(dx+dxam+dxav)*dxav*(dx+dxav));
}

inline double conv_centre(const double& psc,const double& vit_0_0, const double& vit_0,
                          const double& vit_1,const double& vit1_1,
                          double g1, double g2, double g3,double g4)
{
  return  (g1*vit_0_0 + g2*vit_0 + g3*vit_1 + g4*vit1_1) * psc;
}

inline int Eval_centre4_VDF_Face::face_amont_conj(int num_face, int i, int k) const
{
  return la_zone->face_amont_conj(num_face, i, k);
}

inline int Eval_centre4_VDF_Face::face_amont_princ(int num_face, int i) const
{
  return la_zone->face_amont_princ(num_face, i);
}


// Fonctions de calcul des flux pour une inconnue scalaire

//// flux_arete_fluide
//

inline void Eval_centre4_VDF_Face::flux_arete_fluide(const DoubleTab& inco,
                                                     int fac1, int fac2,
                                                     int fac3, int signe,
                                                     double& flux3,double& flux1_2) const
{
  double flux,psc;
  int ori;
  // calcul de flux3:
  psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                 dt_vitesse[fac2]*porosite[fac2] )  *
                (surface(fac1) + surface(fac2))  );

  if ( psc*signe >0)
    flux=inco[fac3]*psc ;
  else
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      ori =orientation(fac3);
      flux = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)
                  +  Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl)) * psc ;
    }
  flux3 = -flux;

  // Calcul de flux1_2:

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    flux = psc*inco[fac1];
  else
    flux = psc*inco[fac2];

  flux1_2 = -flux;
}


//// flux_arete_interne
//

inline double Eval_centre4_VDF_Face::flux_arete_interne(const DoubleTab& inco, int fac1,
                                                        int fac2, int fac3, int fac4) const
{

  double flux;
  double psc = 0.25* (dt_vitesse[fac1]*porosite(fac1) + dt_vitesse[fac2]*porosite(fac2))
               * (surface(fac1) + surface(fac2));

  int ori = orientation(fac1);
  int num0_0 = face_amont_conj(fac3,ori,0);
  int num1_1 = face_amont_conj(fac4,ori,1);

  if ( (num0_0 == -1)||(num1_1== -1) )  // Schema centre 2 (pas assez de faces)
    flux = 0.5*(inco[fac3]+inco[fac4])*psc ;
  else  // Schema Centre
    {
      double dx = dist_face(fac3,fac4,ori); // Inutile de prendre dist_face_period car fac3 et fac4 ne peuvent etre periodiques (arete interne)
      double dxam = dist_face_period(num0_0,fac3,ori);
      double dxav = dist_face_period(fac4,num1_1,ori);
      double vit_0 = inco[fac3];
      double vit_0_0 = inco[num0_0];
      double vit_1 = inco[fac4];
      double vit_1_1 = inco[num1_1];
      double g1,g2,g3,g4;
      calcul_g(dxam,dx,dxav,g1,g2,g3,g4);
      flux = conv_centre(psc,vit_0_0,vit_0,vit_1,vit_1_1,g1,g2,g3,g4);
    }
  return -flux;
}


//// flux_arete_mixte
//

inline double Eval_centre4_VDF_Face::flux_arete_mixte(const DoubleTab& inco ,
                                                      int fac1, int fac2,
                                                      int fac3, int fac4) const
{
  double flux;
  double psc = 0.25 * ( dt_vitesse[fac1]*porosite[fac1] +
                        dt_vitesse[fac2]*porosite[fac2] )  *
               ( surface[fac1] + surface[fac2] );
  if (psc>0)
    flux = psc*inco[fac3];
  else
    flux = psc*inco[fac4];

  return -flux;
}


//// flux_arete_paroi
//

inline double Eval_centre4_VDF_Face::flux_arete_paroi(const DoubleTab&, int, int, int, int) const
{
  return 0;
}


//// flux_arete_paroi_fluide
//

inline void Eval_centre4_VDF_Face::flux_arete_paroi_fluide(const DoubleTab& inco,
                                                           int fac1, int fac2,
                                                           int fac3, int signe,
                                                           double& flux3, double& flux1_2) const
{
  double flux,psc;
  int ori;
  psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                 dt_vitesse[fac2]*porosite[fac2] )  *
                (surface(fac1) + surface(fac2))  );

  if ((psc*signe)>0)
    flux=inco[fac3]*psc ;
  else
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      ori =orientation(fac3);
      flux = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1, ori,la_zcl)
                  +  Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2, ori,la_zcl))* psc ;
    }

  flux3 = -flux;

  // Calcul de flux1_2:

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    flux = psc*inco[fac1];
  else
    flux = psc*inco[fac2];

  flux1_2 = -flux;
}

//// flux_arete_periodicite
//

inline void Eval_centre4_VDF_Face::flux_arete_periodicite(const DoubleTab& inco, int fac1,
                                                          int fac2, int fac3, int fac4,
                                                          double& flux3_4, double& flux1_2) const
{
  // On calcule le flux convectif entre les volumes de controle associes a fac3 et fac4:

  double psc = 0.25* (dt_vitesse[fac1]*porosite(fac1) + dt_vitesse[fac2]*porosite(fac2))
               * (surface(fac1) + surface(fac2));
  int ori = orientation(fac1);
  int num0_0 = face_amont_conj(fac3,ori,0);
  int num1_1 = face_amont_conj(fac4,ori,1);

  if ( (num0_0 == -1)||(num1_1== -1) )  // Schema centre 2 (pas assez de faces)
    flux3_4 = -psc*0.5*(inco[fac3]+inco[fac4]);
  else                                                             // Schema Centre
    {
      double dx = dist_face_period(fac3,fac4,ori);
      double dxam = dist_face_period(num0_0,fac3,ori);
      double dxav = dist_face_period(fac4,num1_1,ori);
      double vit_0 = inco[fac3];
      double vit_0_0 = inco[num0_0];
      double vit_1 = inco[fac4];
      double vit_1_1 = inco[num1_1];
      double g1,g2,g3,g4;
      calcul_g(dxam,dx,dxav,g1,g2,g3,g4);
      flux3_4 = -conv_centre(psc,vit_0_0,vit_0,vit_1,vit_1_1,g1,g2,g3,g4);
    }

  // On calcule le flux convectif entre les volumes de controle associes a fac1 et fac2:
  psc = 0.25* (dt_vitesse[fac3]*porosite(fac3) + dt_vitesse[fac4]*porosite(fac4))
        * (surface(fac3) + surface(fac4));

  ori = orientation(fac3);
  num0_0 = face_amont_conj(fac1,ori,0);
  num1_1 = face_amont_conj(fac2,ori,1);

  if ( (num0_0 == -1)||(num1_1== -1) )  // Schema centre 2 (pas assez de faces)
    flux1_2 = -psc*0.5*(inco[fac1]+inco[fac2]);
  else                                                             // Schema Centre
    {
      double dx = dist_face_period(fac1,fac2,ori);
      double dxam = dist_face_period(num0_0,fac1,ori);
      double dxav = dist_face_period(fac2,num1_1,ori);
      double vit_0 = inco[fac1];
      double vit_0_0 = inco[num0_0];
      double vit_1 = inco[fac2];
      double vit_1_1 = inco[num1_1];
      double g1,g2,g3,g4;
      calcul_g(dxam,dx,dxav,g1,g2,g3,g4);
      flux1_2 = -conv_centre(psc,vit_0_0,vit_0,vit_1,vit_1_1,g1,g2,g3,g4);
    }
}


//// flux_arete_symetrie
//

inline double Eval_centre4_VDF_Face::flux_arete_symetrie(const DoubleTab&, int, int, int, int) const
{
  return 0;
}


//// flux_fa7_elem
//

inline double Eval_centre4_VDF_Face::flux_fa7_elem(const DoubleTab& inco, int num_elem,
                                                   int fac1, int fac2) const
{

  double flux;
  int num0_0 = face_amont_princ(fac1,0);
  int num1_1 = face_amont_princ(fac2,1);
  double psc = 0.25 * (dt_vitesse[fac1]+dt_vitesse[fac2])*(surface(fac1)+surface(fac2));

  if  ( (num0_0 == -1) || (num1_1== -1) )       // Schema centre 2
    flux = psc * 0.5*(inco[fac1]*porosite[fac1]+inco[fac2]*porosite[fac2]);
  else                                                                    // Schema centre
    {
      int ori=orientation(fac1);
      double dx = dim_elem(num_elem,ori);
      double dxam = dim_elem(elem_(fac1,0),ori);
      double dxav = dim_elem(elem_(fac2,1),ori);
      double vit_0 = inco[fac1]*porosite(fac1);
      double vit_0_0 = inco[num0_0]*porosite(num0_0);
      double vit_1 = inco[fac2]*porosite(fac2);
      double vit_1_1 = inco[num1_1]*porosite(num1_1);
      double g1,g2,g3,g4;
      calcul_g(dxam,dx,dxav,g1,g2,g3,g4);
      flux = conv_centre(psc,vit_0_0,vit_0,vit_1,vit_1_1,g1,g2,g3,g4);
    }
  return -flux;
}


inline double Eval_centre4_VDF_Face::flux_fa7_sortie_libre(const DoubleTab& inco, int face,
                                                           const Neumann_sortie_libre& la_cl,
                                                           int num1) const
{
  double flux;
  double psc = dt_vitesse[face]*surface(face);
  int elem1 = elem_(face,0);
  if (elem1 != -1)
    if (psc > 0)
      flux = psc*inco[face];
    else
      flux = psc*la_cl.val_ext(face-num1,orientation(face));
  else // (elem2 != -1)
    if (psc < 0)
      flux = psc*inco[face];
    else
      flux = psc*la_cl.val_ext(face-num1,orientation(face));

  return -flux;
}

//// flux_arete_symetrie_fluide
//
inline void Eval_centre4_VDF_Face::flux_arete_symetrie_fluide(const DoubleTab& inco,
                                                              int fac1, int fac2,
                                                              int fac3, int signe,
                                                              double& flux3, double& flux1_2) const
{
  double flux,psc;

  // Calcul de flux3:

  psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                 dt_vitesse[fac2]*porosite[fac2] )  *
                (surface(fac1) + surface(fac2))  );

  if ((psc*signe)>0)
    flux=inco[fac3]*psc ;
  else
    {

      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int ori = orientation(fac3);
      flux = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl)
                  +  Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl))* psc ;
    }

  flux3 =  -flux;

  // Calcul de flux1_2:

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);

  if (psc>0)
    flux = psc*inco[fac1];
  else

    flux = psc*0.5*(inco[fac1]+inco[fac2]);

  flux1_2 = -flux;
}

//// flux_arete_symetrie_paroi
//

inline double Eval_centre4_VDF_Face::flux_arete_symetrie_paroi(const DoubleTab&, int, int, int, int) const
{
  return 0;
}

// Fonctions de calcul des flux pour une inconnue vectorielle

//// flux_arete_fluide
//

inline void Eval_centre4_VDF_Face::flux_arete_fluide(const DoubleTab& inco,int fac1,
                                                     int fac2,int fac3,int signe,
                                                     DoubleVect& flux3, DoubleVect& flux1_2) const
{

  int k;
  double psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                        dt_vitesse[fac2]*porosite[fac2] )  *
                       (surface(fac1) + surface(fac2))  );

  if ((psc*signe)>0)
    for (k=0; k<flux3.size(); k++)
      flux3(k) = -inco(fac3,k)*psc ;
  else
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int ori = orientation(fac3);
      for (k=0; k<flux3.size(); k++)
        flux3(k) = -0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,ori,la_zcl)
                         +  Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,ori,la_zcl)) * psc ;
    }

  // Calcul de flux1_2:

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    for (k=0; k<flux1_2.size(); k++)
      flux1_2(k) = -psc*inco(fac1,k);
  else
    for (k=0; k<flux1_2.size(); k++)
      flux1_2(k) = -psc*inco(fac2,k);

}


//// flux_arete_interne
//

inline void Eval_centre4_VDF_Face::flux_arete_interne(const DoubleTab& inco, int fac1,
                                                      int fac2, int fac3, int fac4,
                                                      DoubleVect& flux) const
{

  int k;
  int ncomp = flux.size();
  double psc = 0.25* (dt_vitesse[fac1]*porosite(fac1) + dt_vitesse[fac2]*porosite(fac2))
               * (surface(fac1) + surface(fac2));
  int ori = orientation(fac1);
  int num0_0 = face_amont_conj(fac3,ori,0);
  int num1_1 = face_amont_conj(fac4,ori,1);

  if  ( (num0_0 == -1) || (num1_1== -1) ) // Schema centre 2
    for (k=0; k<ncomp; k++)
      flux(k) = -psc*0.5*(inco(fac3,k)+inco(fac4,k));
  else   // Schema centre4
    {

      double dx = dist_face(fac3,fac4,ori);
      double dxam = dist_face_period(num0_0,fac3,ori);
      double dxav = dist_face_period(fac4,num1_1,ori);
      ArrOfDouble vit_0(ncomp);
      ArrOfDouble vit_0_0(ncomp);
      ArrOfDouble vit_1_1(ncomp);
      ArrOfDouble vit_1(ncomp);
      double g1,g2,g3,g4;
      calcul_g(dxam,dx,dxav,g1,g2,g3,g4);

      for (k=0; k<ncomp; k++)
        {
          vit_0(k) = inco(fac3,k);
          vit_0_0(k) = inco(num0_0,k);
          vit_1(k) = inco(fac4,k);
          vit_1_1(k)=inco(num1_1,k);
          flux(k) = -conv_centre(psc,vit_0_0(k),vit_0(k),vit_1(k),vit_1_1(k),g1,g2,g3,g4);
        }
    }
}



//// flux_arete_mixte
//

inline void Eval_centre4_VDF_Face::flux_arete_mixte(const DoubleTab& inco,int fac1,
                                                    int fac2,int fac3, int fac4,
                                                    DoubleVect& flux) const
{

  int k;
  double psc = 0.25 * ( dt_vitesse[fac1]*porosite[fac1] +
                        dt_vitesse[fac2]*porosite[fac2] )  *
               ( surface[fac1] + surface[fac2] );
  if (psc>0)
    for (k=0; k<flux.size(); k++)
      flux(k) = -psc*inco(fac3,k);
  else
    for (k=0; k<flux.size(); k++)
      flux(k) = -psc*inco(fac4,k);
}


//// flux_arete_paroi
//

inline void Eval_centre4_VDF_Face::flux_arete_paroi(const DoubleTab&, int, int,
                                                    int, int,  DoubleVect& ) const
{
  ;
}


//// flux_arete_paroi_fluide
//

inline void Eval_centre4_VDF_Face::flux_arete_paroi_fluide(const DoubleTab& inco,int fac1,
                                                           int fac2,int fac3,int signe,
                                                           DoubleVect& flux3, DoubleVect& flux1_2) const
{

  int k;
  double psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                        dt_vitesse[fac2]*porosite[fac2] )  *
                       (surface(fac1) + surface(fac2))  );

  if ((psc*signe)>0)
    for (k=0; k<flux3.size(); k++)
      flux3(k) = -inco(fac3,k)*psc;
  else
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int ori = orientation(fac3);
      for (k=0; k<flux3.size(); k++)
        flux3(k) = -0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,ori,la_zcl)
                         +  Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,ori,la_zcl))* psc ;
    }

  // Calcul de flux1_2:

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    for (k=0; k<flux1_2.size(); k++)
      flux1_2(k) = -psc*inco(fac1,k);
  else
    for (k=0; k<flux1_2.size(); k++)
      flux1_2(k) = -psc*inco(fac2,k);

}

//// flux_arete_periodicite
//

inline void Eval_centre4_VDF_Face::flux_arete_periodicite(const DoubleTab& inco, int fac1,
                                                          int fac2, int fac3, int fac4,
                                                          DoubleVect& flux3_4, DoubleVect& flux1_2) const
{
  // On calcule le flux convectif entre les volumes de controle associes a fac3 et fac4:
  int k;
  int ncomp = flux3_4.size();
  double psc = 0.25* (dt_vitesse[fac1]*porosite(fac1) + dt_vitesse[fac2]*porosite(fac2))
               * (surface(fac1) + surface(fac2));
  int ori = orientation(fac1);
  int num0_0 = face_amont_conj(fac3,ori,0);
  int num1_1 = face_amont_conj(fac4,ori,1);

  if  ( (num0_0 == -1) || (num1_1== -1) )                           // Schema centre 2
    for (k=0; k<ncomp; k++)
      flux3_4(k) = -psc*0.5*(inco(fac3,k)+inco(fac4,k));
  else
    {
      // Schema centre4

      double dx = dist_face_period(fac3,fac4,ori);
      double dxam = dist_face_period(num0_0,fac3,ori);
      double dxav = dist_face_period(fac4,num1_1,ori);
      ArrOfDouble vit_0(ncomp);
      ArrOfDouble vit_0_0(ncomp);
      ArrOfDouble vit_1_1(ncomp);
      ArrOfDouble vit_1(ncomp);
      double g1,g2,g3,g4;
      calcul_g(dxam,dx,dxav,g1,g2,g3,g4);

      for (k=0; k<ncomp; k++)
        {
          vit_0(k) = inco(fac3,k);
          vit_0_0(k) = inco(num0_0,k);
          vit_1(k) = inco(fac4,k);
          vit_1_1(k)=inco(num1_1,k);
          flux3_4(k) = -conv_centre(psc,vit_0_0(k),vit_0(k),vit_1(k),vit_1_1(k),g1,g2,g3,g4);
        }
    }

  // On calcule le flux convectif entre les volumes de controle associes a fac1 et fac2:

  psc = 0.25* (dt_vitesse[fac3]*porosite(fac3) + dt_vitesse[fac4]*porosite(fac4))
        * (surface(fac3) + surface(fac4));
  ori = orientation(fac3);
  num0_0 = face_amont_conj(fac1,ori,0);
  num1_1 = face_amont_conj(fac2,ori,1);

  if  ( (num0_0 == -1) || (num1_1== -1) )                           // Schema centre2
    for (k=0; k<ncomp; k++)
      flux1_2(k) = -psc*0.5*(inco(fac1,k)+inco(fac2,k));
  else
    {
      // Schema centre4

      double dx = dist_face_period(fac1,fac2,ori);
      double dxam = dist_face_period(num0_0,fac1,ori);
      double dxav = dist_face_period(fac2,num1_1,ori);
      ArrOfDouble vit_0(ncomp);
      ArrOfDouble vit_0_0(ncomp);
      ArrOfDouble vit_1_1(ncomp);
      ArrOfDouble vit_1(ncomp);
      double g1,g2,g3,g4;
      calcul_g(dxam,dx,dxav,g1,g2,g3,g4);
      for (k=0; k<ncomp; k++)
        {
          vit_0(k) = inco(fac1,k);
          vit_0_0(k) = inco(num0_0,k);
          vit_1(k) = inco(fac2,k);
          vit_1_1(k)=inco(num1_1,k);
          flux1_2(k) = -conv_centre(psc,vit_0_0(k),vit_0(k),vit_1(k),vit_1_1(k),g1,g2,g3,g4);
        }
    }

}

//// flux_arete_symetrie
//

inline void Eval_centre4_VDF_Face::flux_arete_symetrie(const DoubleTab&, int, int,
                                                       int, int, DoubleVect& ) const
{
  ;
}


//// flux_fa7_elem
//

inline void Eval_centre4_VDF_Face::flux_fa7_elem(const DoubleTab& inco,int num_elem,
                                                 int fac1,int fac2,DoubleVect& flux) const
{
  int k;
  int ncomp = flux.size();
  int ori = orientation(fac1);
  int num0_0 = face_amont_princ(fac1,0);
  int num1_1 = face_amont_princ(fac2,1);
  double psc = 0.25* (dt_vitesse[fac1]*porosite(fac1) + dt_vitesse[fac2]*porosite(fac2))
               * (surface(fac1) + surface(fac2));


  if ( (num0_0 == -1) || (num1_1== -1) ) // Schema centre2
    for (k=0; k<ncomp; k++)
      flux(k) = -psc*0.5*(inco(fac1,k)*porosite[fac1]+inco(fac2,k)*porosite[fac2]);
  else       // Schema centre4
    {

      double dx = dim_elem(num_elem,ori);
      double dxam = dim_elem(elem_(fac1,0),ori);
      double dxav = dim_elem(elem_(fac2,1),ori);
      ArrOfDouble vit_0(ncomp);
      ArrOfDouble vit_0_0(ncomp);
      ArrOfDouble vit_1_1(ncomp);
      ArrOfDouble vit_1(ncomp);
      double g1,g2,g3,g4;
      calcul_g(dxam,dx,dxav,g1,g2,g3,g4);
      for (k=0; k<ncomp; k++)
        {
          vit_0(k) = inco(fac1,k)* porosite(fac1);
          vit_0_0(k) = inco(num0_0,k)*porosite(num0_0);
          vit_1(k) = inco(fac2,k)*porosite(fac2);
          vit_1_1(k)=inco(num1_1,k)*porosite(num1_1);
          flux(k) = -conv_centre(psc,vit_0_0(k),vit_0(k),vit_1(k),vit_1_1(k),g1,g2,g3,g4);
        }
    }
}


inline void Eval_centre4_VDF_Face::flux_fa7_sortie_libre(const DoubleTab& inco, int face,
                                                         const Neumann_sortie_libre& la_cl,
                                                         int num1,DoubleVect& flux) const
{
  int k;
  double psc = dt_vitesse[face]*surface(face);
  int elem1 = elem_(face,0);
  if (elem1 != -1)
    if (psc > 0)
      for (k=0; k<flux.size(); k++)
        flux(k) = -psc*inco(face,k);
    else
      for (k=0; k<flux.size(); k++)
        flux(k) = -psc*la_cl.val_ext(face-num1,k);
  else // (elem2 != -1)
    if (psc < 0)
      for (k=0; k<flux.size(); k++)
        flux(k) = -psc*inco(face,k);
    else
      for (k=0; k<flux.size(); k++)
        flux(k) = -psc*la_cl.val_ext(face-num1,k);
}

//// flux_arete_symetrie_fluide
//

inline void Eval_centre4_VDF_Face::flux_arete_symetrie_fluide(const DoubleTab& inco,int fac1,
                                                              int fac2,int fac3,int signe,
                                                              DoubleVect& flux3, DoubleVect& flux1_2) const
{

  int k;
  double psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                        dt_vitesse[fac2]*porosite[fac2] )  *
                       (surface(fac1) + surface(fac2))  );

  if ((psc*signe)>0)
    for (k=0; k<flux3.size(); k++)
      flux3(k) = -inco(fac3,k)*psc ;
  else
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int ori = orientation(fac3);
      for (k=0; k<flux3.size(); k++)
        flux3(k) = -0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,ori,la_zcl)
                         +  Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,ori,la_zcl)) * psc ;
    }

  // Calcul de flux1_2:

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    for (k=0; k<flux1_2.size(); k++)
      flux1_2(k) = -psc*inco(fac1,k);
  else
    for (k=0; k<flux1_2.size(); k++)
      flux1_2(k) = -psc*inco(fac2,k);

}

//// flux_arete_symetrie_paroi
//

inline void Eval_centre4_VDF_Face::flux_arete_symetrie_paroi(const DoubleTab&, int,
                                                             int, int, int,
                                                             DoubleVect& ) const
{
  ;
}


#endif
