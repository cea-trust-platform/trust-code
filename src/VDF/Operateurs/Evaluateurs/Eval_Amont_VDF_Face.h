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
// File:        Eval_Amont_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Amont_VDF_Face_included
#define Eval_Amont_VDF_Face_included

#include <Eval_Conv_VDF.h>
#include <Eval_VDF_Face.h>

//
// .DESCRIPTION class Eval_Amont_VDF_Face
//
// Evaluateur VDF pour la convection
// Le champ convecte est un Champ_Face
// Schema de convection Amont
// Rq:Les evaluateurs de flux convectifs calculent en fait le terme
// convectif qui figure au second membre de l'equation d'evolution
// c.a.d l'oppose du flux convectif pour l'EXPLICITE.
//
// Dans le cas de la methode IMPLICITE les evaluateurs calculent la quantite qui figure
// dans le premier membre de l'equation, nous ne prenons pas par consequent l'oppose en
// ce qui concerne les termes pour la matrice, par contre pour le second membre nous
// procedons comme en explicite mais en ne fesant intervenir que les valeurs fournies
// par les conditions limites.
//

//
// .SECTION voir aussi Eval_Conv_VDF



class Eval_Amont_VDF_Face : public Eval_Conv_VDF, public Eval_VDF_Face
{

public:
  inline Eval_Amont_VDF_Face();

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

  inline double flux_fa7_elem(const DoubleTab&, int, int, int) const ;
  inline double flux_fa7_sortie_libre(const DoubleTab&, int ,
                                      const Neumann_sortie_libre&, int ) const;
  inline double flux_arete_interne(const DoubleTab&, int, int, int, int ) const;
  inline double flux_arete_mixte(const DoubleTab&, int, int, int, int ) const;
  inline double flux_arete_paroi(const DoubleTab&, int, int, int, int ) const;
  inline double flux_arete_symetrie(const DoubleTab&, int, int, int, int ) const;
  inline void flux_arete_fluide(const DoubleTab&, int, int, int, int, double&, double&) const ;
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int, int, int, double&, double&) const ;
  inline void flux_arete_periodicite(const DoubleTab&, int, int, int, int, double&, double&) const ;
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
                                  int, DoubleVect& ) const ;
  inline void flux_arete_paroi(const DoubleTab&, int, int, int,
                               int, DoubleVect& ) const ;
  inline void flux_arete_fluide(const DoubleTab&, int, int, int,
                                int, DoubleVect&, DoubleVect& ) const;
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int,
                                      int, int, DoubleVect&, DoubleVect& ) const ;
  inline void flux_arete_periodicite(const DoubleTab&, int, int,
                                     int, int, DoubleVect&, DoubleVect& ) const ;
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int, int,
                                         int, DoubleVect&, DoubleVect& ) const;
  inline void flux_arete_symetrie_paroi(const DoubleTab&, int, int, int,
                                        int, DoubleVect& flux) const ;

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // scalaires.

  inline void coeffs_fa7_elem(int, int, int, double& aii, double& ajj) const;
  inline void coeffs_fa7_sortie_libre(int, const Neumann_sortie_libre&, double& aii, double& ajj) const;
  inline void coeffs_arete_interne(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_mixte(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_symetrie(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_paroi(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_paroi_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_periodicite(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_symetrie_fluide(int, int, int, int,
                                           double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_symetrie_paroi(int, int, int, int, double& aii1_2,
                                          double& aii3_4, double& ajj1_2) const;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite dans le cas scalaire.

  inline double secmem_fa7_elem( int, int, int) const;
  inline double secmem_fa7_sortie_libre(int, const Neumann_sortie_libre&, int ) const;
  inline double secmem_arete_interne(int, int, int, int) const;
  inline double secmem_arete_mixte(int, int, int, int) const;
  inline double secmem_arete_symetrie(int, int, int, int) const;
  inline double secmem_arete_paroi(int, int, int, int ) const;
  inline void secmem_arete_fluide(int, int, int, int, double&, double&) const;
  inline void secmem_arete_paroi_fluide(int, int, int, int, double&, double&) const;
  inline void secmem_arete_periodicite(int, int, int, int, double&, double&) const;
  inline void secmem_arete_symetrie_fluide(int, int, int, int, double&, double&) const;
  inline double secmem_arete_symetrie_paroi(int, int, int, int ) const;

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // vectorielles.

  inline void coeffs_fa7_elem(int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_fa7_sortie_libre(int , const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_interne(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_mixte(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_symetrie(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_paroi(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_paroi_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_periodicite(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline  void coeffs_arete_symetrie_fluide(int, int, int, int,
                                            DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_symetrie_paroi(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite.

  inline void secmem_fa7_elem(int, int, int, DoubleVect& flux) const;
  inline void secmem_fa7_sortie_libre(int , const Neumann_sortie_libre&, int, DoubleVect& flux) const;
  inline void secmem_arete_interne(int, int, int, int, DoubleVect& flux) const;
  inline void secmem_arete_mixte(int, int, int, int, DoubleVect& flux) const;
  inline void secmem_arete_symetrie(int, int, int, int, DoubleVect& ) const;
  inline void secmem_arete_paroi(int, int, int, int, DoubleVect& ) const;
  inline void secmem_arete_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline void secmem_arete_paroi_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline void secmem_arete_periodicite(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline void secmem_arete_symetrie_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline void secmem_arete_symetrie_paroi(int, int, int, int, DoubleVect& ) const;
};

/////////////////////////////////////////////////////////
// Fonctions inline de la classe Eval_Amont_VDF_Face
/////////////////////////////////////////////////////////

inline Eval_Amont_VDF_Face::Eval_Amont_VDF_Face()
{
}

//// calculer_arete_fluide
//

inline int Eval_Amont_VDF_Face::calculer_arete_fluide() const
{
  return 1;
}


//// calculer_arete_paroi
//

inline int Eval_Amont_VDF_Face::calculer_arete_paroi() const
{
  return 0;
}


//// calculer_arete_paroi_fluide
//

inline int Eval_Amont_VDF_Face::calculer_arete_paroi_fluide() const
{
  return 1;
}

//// calculer_arete_periodicite
//

inline int Eval_Amont_VDF_Face::calculer_arete_periodicite() const
{
  return 1;
}

//// calculer_arete_symetrie
//

inline int Eval_Amont_VDF_Face::calculer_arete_symetrie() const
{
  return 0;
}

//// calculer_arete_interne
//

inline int Eval_Amont_VDF_Face::calculer_arete_interne() const
{
  return 1;
}

//// calculer_arete_mixte
//

inline int Eval_Amont_VDF_Face::calculer_arete_mixte() const
{
  return 1;
}

//// calculer_fa7_sortie_libre
//

inline int Eval_Amont_VDF_Face::calculer_fa7_sortie_libre() const
{
  return 1;
}

//// calculer_arete_symetrie_paroi
//

inline int Eval_Amont_VDF_Face::calculer_arete_symetrie_paroi() const
{
  return 0;
}

//// calculer_arete_symetrie_fluide
//

inline int Eval_Amont_VDF_Face::calculer_arete_symetrie_fluide() const
{
  return 1;
}

///////////////////////////////////////////////////////////////
// Fonctions de calcul de flux pour une inconnue scalaire
///////////////////////////////////////////////////////////////

//// flux_arete_fluide
//

inline void Eval_Amont_VDF_Face::flux_arete_fluide(const DoubleTab& inco,
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
      flux = 0.5*(inconnue->val_imp_face_bord(rang1,ori)
                  +  inconnue->val_imp_face_bord(rang2,ori))* psc ;
    }

  flux3 =  -flux;

  // Calcul de flux1_2:

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    flux = psc*inco[fac1];
  else
    flux = psc*inco[fac2];

  flux1_2 = -flux;
}

//// coeffs_arete_fluide
//

inline void Eval_Amont_VDF_Face::coeffs_arete_fluide(int fac1, int fac2,
                                                     int fac3, int signe,
                                                     double& aii1_2, double& aii3_4,
                                                     double& ajj1_2) const
{
  double psc;

  // Calcul de flux3:

  psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                 dt_vitesse[fac2]*porosite[fac2] )  *
                (surface(fac1) + surface(fac2))  );

  if ((psc*signe)>0)
    aii3_4 = psc ;
  else
    aii3_4 = 0;

  // Calcul de flux1_2:

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);

  if (psc>0)
    {
      aii1_2 = psc;
      ajj1_2 = 0;
    }
  else
    {
      ajj1_2 = -psc;
      aii1_2 = 0;
    }
}

//// secmem_arete_fluide
//

inline void Eval_Amont_VDF_Face::secmem_arete_fluide(int fac1, int fac2,
                                                     int fac3, int signe,
                                                     double& flux3, double& flux1_2) const
{
  double psc;

  // Calcul de flux3:

  psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                 dt_vitesse[fac2]*porosite[fac2] )  *
                (surface(fac1) + surface(fac2))  );

  if ((psc*signe)<0)
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int ori = orientation(fac3);
      flux3 = -0.5*(inconnue->val_imp_face_bord(rang1,ori)
                    + inconnue->val_imp_face_bord(rang2,ori))* psc ;
    }
  else
    {
      flux3 = 0;
    }

  flux1_2 = 0;
}

//// flux_arete_interne
//

inline double Eval_Amont_VDF_Face::flux_arete_interne(const DoubleTab& inco ,
                                                      int fac1, int fac2,
                                                      int fac3, int fac4) const
{
  double flux;
  double psc = 0.25 * ( dt_vitesse[fac1]*porosite[fac1] +
                        dt_vitesse[fac2]*porosite[fac2] ) *
               ( surface[fac1] + surface[fac2] );
  if (psc>0)
    flux = psc*inco[fac3];
  else
    flux = psc*inco[fac4];

  return -flux;
}

//// coeffs_arete_interne
//

inline void Eval_Amont_VDF_Face::coeffs_arete_interne(int fac1, int fac2,
                                                      int fac3, int fac4, double& aii, double& ajj) const
{
  double psc = 0.25 * ( dt_vitesse[fac1]*porosite[fac1] +
                        dt_vitesse[fac2]*porosite[fac2] ) *
               ( surface[fac1] + surface[fac2] );
  if (psc>0)
    {
      aii = psc;
      ajj=0;
    }
  else
    {
      ajj = -psc;
      aii=0;
    }
}

//// secmem_arete_interne
//

inline double Eval_Amont_VDF_Face::secmem_arete_interne(int , int ,
                                                        int , int ) const
{
  return 0;
}


//// flux_arete_mixte
//

inline double Eval_Amont_VDF_Face::flux_arete_mixte(const DoubleTab& inco ,
                                                    int fac1, int fac2,
                                                    int fac3, int fac4) const
{
  double flux;
  double psc = 0.25 * ( dt_vitesse[fac1]*porosite[fac1] +
                        dt_vitesse[fac2]*porosite[fac2] ) *
               ( surface[fac1] + surface[fac2] );
  if (psc>0)
    flux = psc*inco[fac3];
  else
    flux = psc*inco[fac4];

  return -flux;
}


//// coeffs_arete_mixte
//

inline void Eval_Amont_VDF_Face::coeffs_arete_mixte(int fac1, int fac2, int fac3, int fac4,
                                                    double& aii, double& ajj) const
{
  double psc = 0.25 * ( dt_vitesse[fac1]*porosite[fac1] +
                        dt_vitesse[fac2]*porosite[fac2] ) *
               ( surface[fac1] + surface[fac2] );

  if (psc>0)
    {
      aii = psc;
      ajj = 0;
    }
  else
    {
      ajj = -psc;
      aii = 0;
    }
}


//// secmem_arete_mixte
//

inline double Eval_Amont_VDF_Face::secmem_arete_mixte(int , int ,
                                                      int , int ) const
{
  return 0;
}


//// flux_arete_paroi
//

inline double Eval_Amont_VDF_Face::flux_arete_paroi(const DoubleTab&, int, int, int, int) const
{
  return 0;
}


//// coeffs_arete_paroi
//

inline void Eval_Amont_VDF_Face::coeffs_arete_paroi(int, int, int, int, double&, double&, double&) const
{
  ;
}


//// secmem_arete_paroi
//

inline double Eval_Amont_VDF_Face::secmem_arete_paroi(int, int, int, int) const
{
  Cerr << " dans secmem_arete_paroi de la convection " << finl;
  return 0;
}

//// flux_arete_paroi_fluide
//

inline void Eval_Amont_VDF_Face::flux_arete_paroi_fluide(const DoubleTab& inco,
                                                         int fac1, int fac2,
                                                         int fac3, int signe,
                                                         double& flux3 , double& flux1_2) const
{
  double psc,flux;
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
      flux = 0.5*(inconnue->val_imp_face_bord(rang1,ori)
                  +  inconnue->val_imp_face_bord(rang2,ori))* psc ;
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

//// coeffs_arete_paroi_fluide
//

inline void Eval_Amont_VDF_Face::coeffs_arete_paroi_fluide(int fac1, int fac2,
                                                           int fac3, int signe, double& aii1_2,
                                                           double& aii3_4, double& ajj1_2) const
{
  double psc;
  psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                 dt_vitesse[fac2]*porosite[fac2] )  *
                (surface(fac1) + surface(fac2))  );
  if ((psc*signe)>0)
    aii3_4 = psc ;
  else
    aii3_4 = 0 ;

  // Calcul des coefficients entre les faces 1 et 2.

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);

  if (psc>0)
    {
      aii1_2 = psc;
      ajj1_2 = 0;
    }
  else
    {
      ajj1_2 = -psc;
      aii1_2 = 0;
    }
}

//// secmem_arete_paroi_fluide
//

inline void Eval_Amont_VDF_Face::secmem_arete_paroi_fluide(int fac1, int fac2,
                                                           int fac3, int signe,
                                                           double& flux3 , double& flux1_2) const
{
  double psc;
  psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                 dt_vitesse[fac2]*porosite[fac2] )  *
                (surface(fac1) + surface(fac2))  );

  if ((psc*signe)<0)
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int ori = orientation(fac3);
      flux3 = -0.5*(inconnue->val_imp_face_bord(rang1,ori)
                    +  inconnue->val_imp_face_bord(rang2,ori))* psc ;
    }
  else
    flux3 = 0;

  flux1_2 = 0;
}

//// flux_arete_periodicite
//

inline void Eval_Amont_VDF_Face::flux_arete_periodicite(const DoubleTab& inco,
                                                        int fac1, int fac2,
                                                        int fac3, int fac4,
                                                        double& flux3_4 , double& flux1_2) const
{
  double flux;
  double psc;

  // On calcule le flux convectif entre les volumes de controle associes a fac3 et fac4:

  psc = 0.25 * ( dt_vitesse[fac1]*porosite[fac1] +
                 dt_vitesse[fac2]*porosite[fac2] ) *
        ( surface[fac1] + surface[fac2] );

  if (psc>0)
    flux = psc*inco[fac3];
  else
    flux = psc*inco[fac4];

  flux3_4 = -flux;

  // On calcule le flux convectif entre les volumes de controle associes a fac1 et fac2:

  psc = 0.25 * ( dt_vitesse[fac3]*porosite[fac3] +
                 dt_vitesse[fac4]*porosite[fac4] ) *
        ( surface[fac3] + surface[fac4] );

  if (psc>0)
    flux = psc*inco[fac1];
  else
    flux = psc*inco[fac2];

  flux1_2 = -flux;

}

//// coeffs_arete_periodicite
//


inline void Eval_Amont_VDF_Face::coeffs_arete_periodicite(int fac1, int fac2,
                                                          int fac3, int fac4,
                                                          double& aii , double& ajj) const
{
  double psc;

  // On calcule le flux convectif entre les volumes de controle associes a fac3 et fac4:

  psc = 0.25 * ( dt_vitesse[fac1]*porosite[fac1] +
                 dt_vitesse[fac2]*porosite[fac2] ) *
        ( surface[fac1] + surface[fac2] );

  if (psc>0)
    {
      aii = psc;
      ajj = 0;
    }
  else
    {
      ajj = -psc;
      aii = 0;
    }
}


//// secmem_arete_periodicite
//

inline void Eval_Amont_VDF_Face::secmem_arete_periodicite(int fac1, int fac2,
                                                          int fac3, int fac4, double& flux3_4 , double& flux1_2) const
{
  ;
}


//// flux_arete_symetrie
//

inline double Eval_Amont_VDF_Face::flux_arete_symetrie(const DoubleTab&, int, int, int, int) const
{
  return 0;
}

//// coeffs_arete_symetrie
//

inline void Eval_Amont_VDF_Face::coeffs_arete_symetrie(int, int, int, int, double& , double&, double&) const
{
  ;
}

//// secmem_arete_symetrie
//

inline double Eval_Amont_VDF_Face::secmem_arete_symetrie(int, int, int, int) const
{
  return 0;
}

//// flux_fa7_elem
//

inline double Eval_Amont_VDF_Face::flux_fa7_elem(const DoubleTab& inco, int ,
                                                 int fac1, int fac2) const
{
  double flux;
  double psc = 0.25 * (dt_vitesse[fac1]+dt_vitesse[fac2]) *
               (surface[fac1]+surface[fac2]);
  if (psc>0)
    flux = psc * inco[fac1]*porosite[fac1];
  else
    flux = psc * inco[fac2]*porosite[fac2];

  return -flux;
}

//// coeffs_fa7_elem
//

inline void Eval_Amont_VDF_Face::coeffs_fa7_elem(int, int fac1, int fac2, double& aii, double& ajj) const
{
  double psc = 0.25 * (dt_vitesse[fac1]+dt_vitesse[fac2]) *
               (surface[fac1]+surface[fac2]);
  if (psc>0)
    {
      aii = psc*porosite[fac1];
      ajj = 0;
    }
  else
    {
      ajj = -psc*porosite[fac2];
      aii = 0;
    }
}

//// secmem_fa7_elem
//

inline double Eval_Amont_VDF_Face::secmem_fa7_elem(int, int , int ) const
{
  return 0;
}

//// flux_fa7_sortie_libre
//

inline double Eval_Amont_VDF_Face::flux_fa7_sortie_libre(const DoubleTab& inco, int face,
                                                         const Neumann_sortie_libre& la_cl,
                                                         int num1) const
{
  double flux;
  double psc = dt_vitesse[face]*surface(face);
  int elem1 = elem_(face,0);
  if (elem1 != -1)
    if (psc > 0)
      flux = psc*inco[face]*porosite[face];
    else
      flux = psc*la_cl.val_ext(face-num1,orientation(face));
  else // (elem2 != -1)
    if (psc < 0)
      flux = psc*inco[face]*porosite[face];
    else
      flux = psc*la_cl.val_ext(face-num1,orientation(face));
  return -flux;
}

//// coeffs_fa7_sortie_libre
//

inline void Eval_Amont_VDF_Face::coeffs_fa7_sortie_libre(int face, const Neumann_sortie_libre& la_cl,
                                                         double& aii, double& ajj) const
{
  double psc = dt_vitesse[face]*surface(face);

  if (psc > 0)
    {
      aii = psc*porosite[face];
      ajj = 0;
    }
  else
    {
      ajj = -psc*porosite[face];
      aii = 0;
    }
}

//// secmem_fa7_sortie_libre
//

inline double Eval_Amont_VDF_Face::secmem_fa7_sortie_libre(int face, const Neumann_sortie_libre& la_cl,
                                                           int num1) const
{
  double flux;
  double psc = dt_vitesse[face]*surface(face);
  int i = elem_(face,0);
  if (i != -1)
    {
      if (psc < 0)
        flux = psc*la_cl.val_ext(face-num1,orientation(face));
      else
        flux = 0;
    }
  else   // (elem2 != -1)
    {
      if (psc > 0)
        flux = psc*la_cl.val_ext(face-num1,orientation(face));
      else
        flux = 0;
    }
  return -flux;
}

//// flux_arete_symetrie_fluide
//

inline void Eval_Amont_VDF_Face::flux_arete_symetrie_fluide(const DoubleTab& inco,
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
      flux = 0.5*(inconnue->val_imp_face_bord(rang1,ori)
                  +  inconnue->val_imp_face_bord(rang2,ori))* psc ;
    }

  flux3 =  -flux;

  // Calcul de flux1_2:

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    flux = psc*inco[fac1];
  else
    flux = psc*inco[fac2];

  flux1_2 = -flux;
}

//// coeffs_arete_symetrie_fluide
//

inline void Eval_Amont_VDF_Face::coeffs_arete_symetrie_fluide(int fac1, int fac2,
                                                              int fac3, int signe,
                                                              double& aii1_2, double& aii3_4,
                                                              double& ajj1_2) const
{
  double psc;

  // Calcul de flux3:

  psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                 dt_vitesse[fac2]*porosite[fac2] )  *
                (surface(fac1) + surface(fac2))  );

  if ((psc*signe)>0)
    aii3_4 = psc ;
  else
    aii3_4 = 0;

  // Calcul de flux1_2:

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);

  if (psc>0)
    {
      aii1_2 = psc;
      ajj1_2 = 0;
    }
  else
    {
      ajj1_2 = -psc;
      aii1_2 = 0;
    }
}

//// secmem_arete_symetrie_fluide
//

inline void Eval_Amont_VDF_Face::secmem_arete_symetrie_fluide(int fac1, int fac2,
                                                              int fac3, int signe,
                                                              double& flux3, double& flux1_2) const
{
  double psc;

  // Calcul de flux3:

  psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                 dt_vitesse[fac2]*porosite[fac2] )  *
                (surface(fac1) + surface(fac2))  );

  if ((psc*signe)<0)
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int ori = orientation(fac3);
      flux3 = -0.5*(inconnue->val_imp_face_bord(rang1,ori)
                    + inconnue->val_imp_face_bord(rang2,ori))* psc ;
    }
  else
    {
      flux3 = 0;
    }

  flux1_2 = 0;
}

//// flux_arete_symetrie_paroi
//

inline double Eval_Amont_VDF_Face::flux_arete_symetrie_paroi(const DoubleTab&, int, int, int, int) const
{
  return 0;
}


//// coeffs_arete_paroi
//

inline void Eval_Amont_VDF_Face::coeffs_arete_symetrie_paroi(int, int, int, int, double&, double&, double&) const
{
  ;
}


//// secmem_arete_paroi
//

inline double Eval_Amont_VDF_Face::secmem_arete_symetrie_paroi(int, int, int, int) const
{
  return 0.;
}

//////////////////////////////////////////////////////////////
// Fonctions de calcul de flux pour une inconnue vectorielle
/////////////////////////////////////////////////////////////

//// flux_arete_fluide
//

inline void Eval_Amont_VDF_Face::flux_arete_fluide(const DoubleTab& inco,int fac1,
                                                   int fac2, int fac3, int signe,
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
        flux3(k) = -0.5*(inconnue->val_imp_face_bord(rang1,k,ori)
                         +  inconnue->val_imp_face_bord(rang2,k,ori))* psc ;
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

//// coeffs_arete_fluide
//

inline void Eval_Amont_VDF_Face::coeffs_arete_fluide(int fac1, int fac2, int fac3, int signe,
                                                     DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
{
  int k;
  double psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                        dt_vitesse[fac2]*porosite[fac2] )  *
                       (surface(fac1) + surface(fac2))  );
  if ((psc*signe)>0)
    for (k=0; k<aii3_4.size(); k++)
      aii3_4(k) = psc ;
  else
    for (k=0; k<aii3_4.size(); k++)
      aii3_4(k) = 0 ;

  // Calcul des coefficients entre 1 et 2:

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    {
      for (k=0; k<aii1_2.size(); k++)
        aii1_2(k) = psc;
      for (k=0; k<ajj1_2.size(); k++)
        ajj1_2(k) = 0;
    }
  else
    {
      for (k=0; k<aii1_2.size(); k++)
        aii1_2(k) = 0;
      for (k=0; k<ajj1_2.size(); k++)
        ajj1_2(k) = -psc;
    }
}


//// secmem_arete_fluide
//

inline void Eval_Amont_VDF_Face::secmem_arete_fluide(int fac1,
                                                     int fac2, int fac3, int signe,
                                                     DoubleVect& flux3, DoubleVect& flux1_2) const
{
  double psc;
  int k;

  // Calcul de flux3:

  psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                 dt_vitesse[fac2]*porosite[fac2] )  *
                (surface(fac1) + surface(fac2))  );

  if ((psc*signe)<0)
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int ori = orientation(fac3);
      for (k=0; k<flux3.size(); k++)
        flux3(k) = -0.5*(inconnue->val_imp_face_bord(rang1,k,ori)
                         + inconnue->val_imp_face_bord(rang2,k,ori))* psc ;
    }
  else
    for (k=0; k<flux3.size(); k++)
      flux3(k) = 0;

  for (k=0; k<flux1_2.size(); k++)
    flux1_2(k) = 0;
}

//// flux_arete_interne
//

inline void Eval_Amont_VDF_Face::flux_arete_interne(const DoubleTab& inco,int fac1,
                                                    int fac2,int fac3, int fac4,
                                                    DoubleVect& flux) const
{
  int k;
  double psc = 0.25 * ( dt_vitesse[fac1]*porosite[fac1] +
                        dt_vitesse[fac2]*porosite[fac2] ) *
               ( surface[fac1] + surface[fac2] );
  if (psc>0)
    for (k=0; k<flux.size(); k++)
      flux(k) = -psc*inco(fac3,k);
  else
    for (k=0; k<flux.size(); k++)
      flux(k) = -psc*inco(fac4,k);
}


//// coeffs_arete_interne
//

inline void Eval_Amont_VDF_Face::coeffs_arete_interne(int fac1, int fac2,int fac3, int fac4,
                                                      DoubleVect& aii, DoubleVect& ajj ) const
{
  int k;
  double psc = 0.25 * ( dt_vitesse[fac1]*porosite[fac1] +
                        dt_vitesse[fac2]*porosite[fac2] ) *
               ( surface[fac1] + surface[fac2] );
  if (psc>0)
    {
      for (k=0; k<aii.size(); k++)
        aii(k) = psc;
      for (k=0; k<ajj.size(); k++)
        ajj(k) = 0;
    }
  else
    {
      for (k=0; k<ajj.size(); k++)
        ajj(k) = -psc;
      for (k=0; k<aii.size(); k++)
        aii(k) = 0;
    }
}


//// secmem_arete_interne
//

inline void Eval_Amont_VDF_Face::secmem_arete_interne(int, int ,int , int,
                                                      DoubleVect& ) const
{
  ;
}

//// flux_arete_mixte
//

inline void Eval_Amont_VDF_Face::flux_arete_mixte(const DoubleTab& inco,int fac1,
                                                  int fac2,int fac3, int fac4,
                                                  DoubleVect& flux) const
{
  int k;
  double psc = 0.25 * ( dt_vitesse[fac1]*porosite[fac1] +
                        dt_vitesse[fac2]*porosite[fac2] ) *
               ( surface[fac1] + surface[fac2] );
  if (psc>0)
    for (k=0; k<flux.size(); k++)
      flux(k) = -psc*inco(fac3,k);
  else
    for (k=0; k<flux.size(); k++)
      flux(k) = -psc*inco(fac4,k);
}

//// coeffs_arete_mixte
//

inline void Eval_Amont_VDF_Face::coeffs_arete_mixte(int fac1, int fac2,int fac3, int fac4,
                                                    DoubleVect& aii, DoubleVect& ajj) const
{
  int k;
  double psc = 0.25 * ( dt_vitesse[fac1]*porosite[fac1] +
                        dt_vitesse[fac2]*porosite[fac2] ) *
               ( surface[fac1] + surface[fac2] );

  if (psc>0)
    {
      for (k=0; k<aii.size(); k++)
        aii(k) = psc;
      for (k=0; k<ajj.size(); k++)
        ajj(k) = 0;
    }
  else
    {
      for (k=0; k<ajj.size(); k++)
        ajj(k) = -psc;
      for (k=0; k<aii.size(); k++)
        aii(k) = 0;
    }
}

//// secmem_arete_mixte
//

inline void Eval_Amont_VDF_Face::secmem_arete_mixte(int , int ,int , int ,
                                                    DoubleVect& ) const
{
  ;
}

//// flux_arete_paroi
//

inline void Eval_Amont_VDF_Face::flux_arete_paroi(const DoubleTab&, int,
                                                  int, int, int,
                                                  DoubleVect& ) const
{
  ;
}

//// coeffs_arete_paroi
//

inline void Eval_Amont_VDF_Face::coeffs_arete_paroi(int, int, int, int, DoubleVect&, DoubleVect&,
                                                    DoubleVect&) const
{
  ;
}

//// secmem_arete_paroi
//

inline void Eval_Amont_VDF_Face::secmem_arete_paroi(int, int, int, int, DoubleVect& ) const
{
  ;
}

//// flux_arete_paroi_fluide
//

inline void Eval_Amont_VDF_Face::flux_arete_paroi_fluide(const DoubleTab& inco,int fac1,
                                                         int fac2,int fac3,int signe,
                                                         DoubleVect& flux3,DoubleVect& flux1_2) const
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
        flux3(k) = -0.5*(inconnue->val_imp_face_bord(rang1,k,ori)
                         +  inconnue->val_imp_face_bord(rang2,k,ori))* psc ;
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

//// coeffs_arete_paroi_fluide
//

inline void Eval_Amont_VDF_Face::coeffs_arete_paroi_fluide(int fac1, int fac2,int fac3,int signe,
                                                           DoubleVect& aii1_2, DoubleVect& aii3_4,
                                                           DoubleVect& ajj1_2) const
{
  int k;
  double psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                        dt_vitesse[fac2]*porosite[fac2] )  *
                       (surface(fac1) + surface(fac2))  );
  if ((psc*signe)>0)
    for (k=0; k<aii3_4.size(); k++)
      aii3_4(k) = psc ;
  else
    for (k=0; k<aii3_4.size(); k++)
      aii3_4(k) = 0;

  // Calcul des coefficients entre 1 et 2

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);

  if (psc>0)
    {
      for (k=0; k<aii1_2.size(); k++)
        aii1_2(k) = psc;
      for (k=0; k<ajj1_2.size(); k++)
        ajj1_2(k) = 0;
    }
  else
    {
      for (k=0; k<aii1_2.size(); k++)
        aii1_2(k) = 0;
      for (k=0; k<ajj1_2.size(); k++)
        ajj1_2(k) = -psc;
    }
}

//// secmem_arete_paroi_fluide
//

inline void Eval_Amont_VDF_Face::secmem_arete_paroi_fluide(int fac1, int fac2,int fac3,int signe,
                                                           DoubleVect& flux3, DoubleVect& flux1_2) const
{
  // Calcul de flux3

  int k;
  double psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                        dt_vitesse[fac2]*porosite[fac2] )  *
                       (surface(fac1) + surface(fac2))  );
  if ((psc*signe)<0)
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int ori = orientation(fac3);
      for (k=0; k<flux3.size(); k++)
        flux3(k) = -0.5*(inconnue->val_imp_face_bord(rang1,k,ori)
                         +  inconnue->val_imp_face_bord(rang2,k,ori))* psc ;
    }
  else
    for (k=0; k<flux3.size(); k++)
      flux3(k) = 0;

  for (k=0; k<flux1_2.size(); k++)
    flux1_2(k) = 0;
}

//// flux_arete_periodicite
//

inline void Eval_Amont_VDF_Face::flux_arete_periodicite(const DoubleTab& inco,
                                                        int fac1, int fac2 , int fac3, int fac4,
                                                        DoubleVect& flux3_4, DoubleVect& flux1_2) const
{
  double psc;
  int k;

  // On calcule le flux convectif entre les volumes de controle associes a fac3 et fac4:

  psc = 0.25 * ( dt_vitesse[fac1]*porosite[fac1] +
                 dt_vitesse[fac2]*porosite[fac2] ) *
        ( surface[fac1] + surface[fac2] );

  if (psc>0)
    for (k=0; k<flux3_4.size(); k++)
      flux3_4(k) = -psc*inco(fac3,k);
  else
    for (k=0; k<flux3_4.size(); k++)
      flux3_4(k) = -psc*inco(fac4,k);


  // On calcule le flux convectifs entre les volumes de controle associes a fac1 et fac2:

  psc = 0.25 * ( dt_vitesse[fac3]*porosite[fac3] +
                 dt_vitesse[fac4]*porosite[fac4] ) *
        ( surface[fac3] + surface[fac4] );

  if (psc>0)
    for (k=0; k<flux1_2.size(); k++)
      flux1_2(k) = -psc*inco(fac1,k);
  else
    for (k=0; k<flux3_4.size(); k++)
      flux1_2(k) = -psc*inco(fac2,k);
}

//// coeffs_arete_periodicite
//

inline void Eval_Amont_VDF_Face::coeffs_arete_periodicite(int fac1, int fac2 , int fac3, int fac4,
                                                          DoubleVect& aii, DoubleVect& ajj) const
{
  double psc;
  int k;

  // On calcule le flux convectif entre les volumes de controle associes a fac3 et fac4:

  psc = 0.25 * ( dt_vitesse[fac1]*porosite[fac1] +
                 dt_vitesse[fac2]*porosite[fac2] ) *
        ( surface[fac1] + surface[fac2] );

  if (psc>0)
    {
      for (k=0; k<aii.size(); k++)
        aii(k) = psc;
      for (k=0; k<ajj.size(); k++)
        ajj(k) = 0;
    }
  else
    {
      for (k=0; k<ajj.size(); k++)
        ajj(k) = -psc;
      for (k=0; k<aii.size(); k++)
        aii(k) = 0;
    }
}


//// secmem_arete_periodicite
//

inline void Eval_Amont_VDF_Face::secmem_arete_periodicite(int, int , int, int ,
                                                          DoubleVect&, DoubleVect&) const
{
  ;
}

//// flux_arete_symetrie
//

inline void Eval_Amont_VDF_Face::flux_arete_symetrie(const DoubleTab&, int,
                                                     int, int, int, DoubleVect& ) const
{
  ;
}

// coeff_arete_symetrie
//
inline void Eval_Amont_VDF_Face::coeffs_arete_symetrie(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const
{
  ;
}

//// secmem_arete_symetrie
//

inline void Eval_Amont_VDF_Face::secmem_arete_symetrie(int, int, int, int, DoubleVect&) const
{
  ;
}

//// flux_fa7_elem
//

inline void Eval_Amont_VDF_Face::flux_fa7_elem(const DoubleTab& inco, int ,
                                               int fac1, int fac2,
                                               DoubleVect& flux) const
{
  int k;
  double psc = 0.25 * (dt_vitesse[fac1]+dt_vitesse[fac2])*
               (surface[fac1]+surface[fac2]);
  if (psc>0)
    for (k=0; k<flux.size(); k++)
      flux(k) = -psc*inco(fac1,k)*porosite[fac1];
  else
    for (k=0; k<flux.size(); k++)
      flux(k) = -psc*inco(fac2,k)*porosite[fac2];
}

//// coeffs_fa7_elem
//

inline void Eval_Amont_VDF_Face::coeffs_fa7_elem(int, int fac1, int fac2,
                                                 DoubleVect& aii, DoubleVect& ajj ) const
{
  int k;
  double psc = 0.25 * (dt_vitesse[fac1]+dt_vitesse[fac2])*
               (surface[fac1]+surface[fac2]);
  if (psc>0)
    {
      for (k=0; k<aii.size(); k++)
        aii(k) = psc*porosite[fac1];
      for (k=0; k<ajj.size(); k++)
        ajj(k) = 0;
    }
  else
    {
      for (k=0; k<aii.size(); k++)
        aii(k) = 0;
      for (k=0; k<ajj.size(); k++)
        ajj(k) = -psc*porosite[fac2];
    }
}

//// secmem_fa7_elem
//

inline void Eval_Amont_VDF_Face::secmem_fa7_elem(int, int, int,
                                                 DoubleVect&) const
{
  ;
}

//// flux_fa7_sortie_libre
//

inline void Eval_Amont_VDF_Face::flux_fa7_sortie_libre(const DoubleTab& inco, int face,
                                                       const Neumann_sortie_libre& la_cl,
                                                       int num1,DoubleVect& flux) const
{
  double psc = dt_vitesse[face]*surface(face);
  int elem1 = elem_(face,0);
  int k;
  if (elem1 != -1)
    if (psc > 0)
      for (k=0; k<flux.size(); k++)
        flux(k) = -psc*inco(face,k)*porosite[face];
    else
      for (k=0; k<flux.size(); k++)
        flux(k) = -psc*la_cl.val_ext(face-num1,k);
  else // (elem2 != -1)
    if (psc < 0)
      for (k=0; k<flux.size(); k++)
        flux(k) = -psc*inco(face,k)*porosite[face];
    else
      for (k=0; k<flux.size(); k++)
        flux(k) = -psc*la_cl.val_ext(face-num1,k);
}

//// coeffs_fa7_sortie_libre
//

inline void Eval_Amont_VDF_Face::coeffs_fa7_sortie_libre(int face,const Neumann_sortie_libre& la_cl,
                                                         DoubleVect& aii, DoubleVect& ajj) const
{
  double psc = dt_vitesse[face]*surface(face);
  //  int i = elem(face,0);
  //  int j = elem(face,1);
  int k;

  if (psc>0)
    {
      for (k=0; k<aii.size(); k++)
        aii(k) = psc*porosite[face];
      for (k=0; k<ajj.size(); k++)
        ajj(k) = 0;
    }
  else
    {
      for (k=0; k<ajj.size(); k++)
        ajj(k) = -psc*porosite[face];
      for (k=0; k<aii.size(); k++)
        aii(k) = 0;
    }
}
//// secmem_fa7_sortie_libre
//

inline void Eval_Amont_VDF_Face::secmem_fa7_sortie_libre(int face,
                                                         const Neumann_sortie_libre& la_cl,
                                                         int num1,DoubleVect& flux) const
{
  double psc = dt_vitesse[face]*surface(face);
  int i = elem_(face,0);
  //  int j = elem(face,1);
  int k;
  if (i != -1)
    {
      if (psc < 0)
        for (k=0; k<flux.size(); k++)
          flux(k) = -psc*la_cl.val_ext(face-num1,k);
      else
        for (k=0; k<flux.size(); k++)
          flux(k) = 0;
    }
  else   // (elem2 != -1)
    {
      if (psc > 0)
        for (k=0; k<flux.size(); k++)
          flux(k) = -psc*la_cl.val_ext(face-num1,k);
      else
        for (k=0; k<flux.size(); k++)
          flux(k) = 0;
    }
}

//// flux_arete_symetrie_fluide
//

inline void Eval_Amont_VDF_Face::flux_arete_symetrie_fluide(const DoubleTab& inco,int fac1,
                                                            int fac2, int fac3, int signe,
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
        flux3(k) = -0.5*(inconnue->val_imp_face_bord(rang1,k,ori)
                         +  inconnue->val_imp_face_bord(rang2,k,ori))* psc ;
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

//// coeffs_arete_fluide
//

inline void Eval_Amont_VDF_Face::coeffs_arete_symetrie_fluide(int fac1, int fac2, int fac3, int signe,
                                                              DoubleVect& aii1_2,
                                                              DoubleVect& aii3_4, DoubleVect& ajj1_2) const
{
  int k;
  double psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                        dt_vitesse[fac2]*porosite[fac2] )  *
                       (surface(fac1) + surface(fac2))  );
  if ((psc*signe)>0)
    for (k=0; k<aii3_4.size(); k++)
      aii3_4(k) = psc ;
  else
    for (k=0; k<aii3_4.size(); k++)
      aii3_4(k) = 0 ;

  // Calcul des coefficients entre 1 et 2:

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    {
      for (k=0; k<aii1_2.size(); k++)
        aii1_2(k) = psc;
      for (k=0; k<ajj1_2.size(); k++)
        ajj1_2(k) = 0;
    }
  else
    {
      for (k=0; k<aii1_2.size(); k++)
        aii1_2(k) = 0;
      for (k=0; k<ajj1_2.size(); k++)
        ajj1_2(k) = -psc;
    }
}


//// secmem_arete_fluide
//

inline void Eval_Amont_VDF_Face::secmem_arete_symetrie_fluide(int fac1,
                                                              int fac2, int fac3, int signe,
                                                              DoubleVect& flux3, DoubleVect& flux1_2) const
{
  double psc;
  int k;

  // Calcul de flux3:

  psc = 0.25 * ((dt_vitesse[fac1]*porosite[fac1]    +
                 dt_vitesse[fac2]*porosite[fac2] )  *
                (surface(fac1) + surface(fac2))  );

  if ((psc*signe)<0)
    {
      int rang1 = (fac1-premiere_face_bord);
      int rang2 = (fac2-premiere_face_bord);
      int ori = orientation(fac3);
      for (k=0; k<flux3.size(); k++)
        flux3(k) = -0.5*(inconnue->val_imp_face_bord(rang1,k,ori)
                         + inconnue->val_imp_face_bord(rang2,k,ori))* psc ;
    }
  else
    for (k=0; k<flux3.size(); k++)
      flux3(k) = 0;

  for (k=0; k<flux1_2.size(); k++)
    flux1_2(k) = 0;
}

//// flux_arete_symetrie_paroi
//

inline void Eval_Amont_VDF_Face::flux_arete_symetrie_paroi(const DoubleTab&, int,
                                                           int, int, int,
                                                           DoubleVect& ) const
{
  ;
}

//// coeffs_arete_symetrie_paroi
//

inline void Eval_Amont_VDF_Face::coeffs_arete_symetrie_paroi(int, int, int, int,
                                                             DoubleVect&, DoubleVect&, DoubleVect&) const
{
  ;
}

//// secmem_arete_symetrie_paroi
//

inline void Eval_Amont_VDF_Face::secmem_arete_symetrie_paroi(int, int, int, int, DoubleVect& ) const
{
  ;
}


#endif
