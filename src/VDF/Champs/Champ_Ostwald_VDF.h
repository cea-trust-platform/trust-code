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
// File:        Champ_Ostwald_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Champs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Ostwald_VDF_included
#define Champ_Ostwald_VDF_included




#include <Champ_Ostwald.h>
#include <Champ_Face.h>
#include <Ref_Champ_Face.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_Ostwald_VDF
//     Represente un champ en discretisation VDF qui varie en fonction
//        de la consistance et de l'indice de structure.
//         Champ utilise pour le fluide d'Ostwald et la discretisation VDF.
//            Fait reference a Champ_Face pour lancer le calcul de D:D dans Champ4.cpp
//                    zone_VDF pou utiliser la
// .SECTION voir aussi
//     Champ_Ostwald
//////////////////////////////////////////////////////////////////////////////
class Champ_Ostwald_VDF : public Champ_Ostwald
{

  Declare_instanciable(Champ_Ostwald_VDF);

public :

  inline const Champ_Face& mon_champs() const;
  //   inline const Fluide_Ostwald& mon_fluide() const;
  inline void associer_champ(const Champ_Face&);

  inline void associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base);

  // inline void associer_fluide(const Fluide_Ostwald&);

  const Zone_dis_base& zone_dis_base() const;

  void mettre_a_jour(double temps);
  void me_calculer(double tps);
  int initialiser(const double& temps);

protected :

  void calculer_mu(DoubleTab& );
  REF(Champ_Face) mon_champ_;  // pour calculer D::D
  REF(Zone_VDF) la_zone_VDF;

};


inline const Champ_Face& Champ_Ostwald_VDF::mon_champs() const
{
  return mon_champ_.valeur();
}

// programme dans classe Champ_Ostwald
/*inline const Fluide_Ostwald& Champ_Ostwald_VDF::mon_fluide() const
  {
  return mon_fluide_.valeur();
  }*/


inline void Champ_Ostwald_VDF::associer_champ(const Champ_Face& un_champ)
{
  mon_champ_ = un_champ;
}


inline void Champ_Ostwald_VDF::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  Cerr<<"on est dans  Champ_Ostwald_VDF::associer_zone_dis_base "<<finl;
  la_zone_VDF = (const Zone_VDF&) la_zone_dis_base;
}



// programme dans classe Champ_Ostwald
/*
  inline void Champ_Ostwald_VDF::associer_fluide(const Fluide_Ostwald& le_fluide)
  {
  mon_fluide_ = le_fluide;
  }*/

#endif

