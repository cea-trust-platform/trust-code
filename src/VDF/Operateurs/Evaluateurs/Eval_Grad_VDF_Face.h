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
// File:        Eval_Grad_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Eval_Grad_VDF_Face_included
#define Eval_Grad_VDF_Face_included

#include <Eval_Grad_VDF.h>
#include <Eval_VDF_Face.h>

//
// .DESCRIPTION class Eval_Grad_VDF_Face
//
// Evaluateur VDF pour le gradient

//
// .SECTION voir aussi Eval_Grad_VDF


class Eval_Grad_VDF_Face : public Eval_Grad_VDF, public Eval_VDF_Face
{

public:

  inline Eval_Grad_VDF_Face();

  inline int calculer_arete_bord() const ;
  inline int calculer_arete_fluide() const ;
  inline int calculer_arete_paroi() const ;
  inline int calculer_arete_paroi_fluide() const ;
  inline int calculer_arete_symetrie() const ;
  inline int calculer_arete_interne() const ;
  inline int calculer_arete_mixte() const ;
  inline int calculer_fa7_sortie_libre() const ;
  inline int calculer_arete_periodicite() const;

  // Fonctions qui servent a calculer le flux de grandeurs scalaires
  // Elles sont de type double et renvoient le flux

  inline double flux_fa7_sortie_libre(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const;
  inline double flux_fa7_elem(const DoubleTab&, int, int, int) const;
  inline double flux_arete_interne(const DoubleTab&, int, int, int, int) const;
  inline double flux_arete_mixte(const DoubleTab&, int, int, int, int) const;
  inline double flux_arete_paroi(const DoubleTab&, int, int, int, int) const;
  inline double flux_arete_symetrie(const DoubleTab&, int, int, int, int) const;
  inline void flux_arete_fluide(const DoubleTab&, int, int, int, int, double&, double&) const;
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int, int, int, double&, double&) const;
  inline void flux_arete_periodicite(const DoubleTab&, int, int, int, int, double&, double&) const;

  // Fonctions qui servent a calculer le flux de grandeurs vectorielles
  // Elles sont de type void et remplissent le tableau flux

  inline void flux_fa7_elem(const DoubleTab&, int, int, int, DoubleVect& flux) const;
  inline void flux_fa7_sortie_libre(const DoubleTab&, int , const Neumann_sortie_libre&,
                                    int, DoubleVect& flux) const;
  inline void flux_arete_interne(const DoubleTab&, int, int, int,
                                 int, DoubleVect& flux) const ;
  inline void flux_arete_mixte(const DoubleTab&, int, int, int,
                               int, DoubleVect& flux) const;
  inline void flux_arete_symetrie(const DoubleTab&, int, int, int,
                                  int, DoubleVect& flux) const;
  inline void flux_arete_paroi(const DoubleTab&, int, int, int,
                               int, DoubleVect& flux) const ;
  inline void flux_arete_fluide(const DoubleTab&, int, int, int,
                                int, DoubleVect& ,DoubleVect&) const;
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int,
                                      int, int, DoubleVect& ,DoubleVect&) const;
  inline void flux_arete_periodicite(const DoubleTab&, int, int,
                                     int, int, DoubleVect&, DoubleVect&) const;
};

//
// Fonctions inline de la classe Eval_Grad_VDF_Face
//

inline Eval_Grad_VDF_Face::Eval_Grad_VDF_Face() {}


//// calculer_aretebord
//
inline int Eval_Grad_VDF_Face::calculer_arete_bord() const
{
  return 0;
}
//// calculer_arete_fluide
//

inline int Eval_Grad_VDF_Face::calculer_arete_fluide() const
{
  return 0;
}


//// calculer_arete_paroi
//

inline int Eval_Grad_VDF_Face::calculer_arete_paroi() const
{
  return 0;
}


//// calculer_arete_paroi_fluide
//

inline int Eval_Grad_VDF_Face::calculer_arete_paroi_fluide() const
{
  return 0;
}


//// calculer_arete_symetrie
//

inline int Eval_Grad_VDF_Face::calculer_arete_symetrie() const
{
  return 0;
}

//// calculer_arete_interne
//

inline int Eval_Grad_VDF_Face::calculer_arete_interne() const
{
  return 0;
}

//// calculer_arete_mixte
//

inline int Eval_Grad_VDF_Face::calculer_arete_mixte() const
{
  return 0;
}

//// calculer_fa7_sortie_libre
//

inline int Eval_Grad_VDF_Face::calculer_fa7_sortie_libre() const
{
  return 1;
}

//// calculer_arete_periodicite
//

inline int Eval_Grad_VDF_Face::calculer_arete_periodicite() const
{
  return 0;
}

// Fonctions de calcul des flux pour une inconnue scalaire

//// flux_arete_fluide
//

inline void Eval_Grad_VDF_Face::flux_arete_fluide(const DoubleTab&, int, int,
                                                  int, int, double& , double& ) const
{
  ;
}


//// flux_arete_interne
//

inline double Eval_Grad_VDF_Face::flux_arete_interne(const DoubleTab&, int, int, int, int) const
{
  return 0;
}


//// flux_arete_mixte
//

inline double Eval_Grad_VDF_Face::flux_arete_mixte(const DoubleTab&, int, int, int, int) const
{
  return 0;
}


//// flux_arete_paroi
//

inline double Eval_Grad_VDF_Face::flux_arete_paroi(const DoubleTab&, int, int, int, int) const
{
  return 0;
}


//// flux_arete_paroi_fluide
//

inline void Eval_Grad_VDF_Face::flux_arete_paroi_fluide(const DoubleTab&, int, int,
                                                        int, int, double& , double& ) const
{
  ;
}

//// flux_arete_periodicite
//

inline void Eval_Grad_VDF_Face::flux_arete_periodicite(const DoubleTab&, int, int,
                                                       int, int, double& , double& ) const
{
  ;
}

//// flux_arete_symetrie
//

inline double Eval_Grad_VDF_Face::flux_arete_symetrie(const DoubleTab&, int, int, int, int) const
{
  return 0;
}

//// flux_fa7_elem
//

inline double Eval_Grad_VDF_Face::flux_fa7_elem(const DoubleTab& inco, int elem,
                                                int fac1, int ) const
{
  return inco[elem]*surface(fac1);
}

//// flux_fa7_sortie_libre
//

inline double Eval_Grad_VDF_Face::flux_fa7_sortie_libre(const DoubleTab& , int face,
                                                        const Neumann_sortie_libre& la_cl,
                                                        int num1) const
{
  return (la_cl.flux_impose(face-num1)*surface(face));
}

// Fonctions de calcul des flux pour une inconnue vectorielle

//// flux_arete_fluide
//

inline void Eval_Grad_VDF_Face::flux_arete_fluide(const DoubleTab&, int, int, int,
                                                  int, DoubleVect&, DoubleVect& ) const
{
  ;
}


//// flux_arete_interne
//

inline void Eval_Grad_VDF_Face::flux_arete_interne(const DoubleTab&, int, int, int,
                                                   int, DoubleVect& ) const
{
  ;
}


//// flux_arete_mixte
//

inline void Eval_Grad_VDF_Face::flux_arete_mixte(const DoubleTab&, int, int, int,
                                                 int, DoubleVect& ) const
{
  ;
}


//// flux_arete_paroi
//

inline void Eval_Grad_VDF_Face::flux_arete_paroi(const DoubleTab&, int, int, int,
                                                 int, DoubleVect& ) const
{
  ;
}


//// flux_arete_paroi_fluide
//

inline void Eval_Grad_VDF_Face::flux_arete_paroi_fluide(const DoubleTab&, int, int, int,
                                                        int, DoubleVect&, DoubleVect& ) const
{
  ;
}

//// flux_arete_periodicite
//

inline void Eval_Grad_VDF_Face::flux_arete_periodicite(const DoubleTab&, int, int, int,
                                                       int, DoubleVect&, DoubleVect& ) const
{
  ;
}

//// flux_arete_symetrie
//

inline void Eval_Grad_VDF_Face::flux_arete_symetrie(const DoubleTab&, int, int, int,
                                                    int, DoubleVect& ) const
{
  ;
}

//// flux_fa7_elem
//

inline void Eval_Grad_VDF_Face::flux_fa7_elem(const DoubleTab& inco, int elem,
                                              int fac1, int, DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++)
    flux(k) = inco(elem,k)*surface(fac1);
}

//// flux_fa7_sortie_libre
//

inline void Eval_Grad_VDF_Face::flux_fa7_sortie_libre(const DoubleTab& , int face,
                                                      const Neumann_sortie_libre& la_cl,
                                                      int num1,DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++)
    flux(k) = la_cl.flux_impose(face-num1,k)*surface(face);
}

#endif
