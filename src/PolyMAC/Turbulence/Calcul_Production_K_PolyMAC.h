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
// File:        Calcul_Production_K_PolyMAC.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Turbulence
// Version:     /main/2
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Calcul_Production_K_PolyMAC_included
#define Calcul_Production_K_PolyMAC_included

/////////////////////////////////////////////////////////////////////////
//
//  CLASS Calcul_Production_K_PolyMAC
//
// Classe qui porte les fonctions de calcul des termes de production
// de l'energie cinetique turbulente et de destruction de cette energie
// Cette classe ne derive d'aucune autre car nous voulons l'utiliser
// pour faire de l'heritage multiple.
//
/////////////////////////////////////////////////////////////////////////


class DoubleVect;
class DoubleTab;
class Zone_Cl_dis;
class Zone_Cl_PolyMAC;
class Champ_Face_PolyMAC;
class Zone_PolyMAC;

class Calcul_Production_K_PolyMAC
{

protected:

  Calcul_Production_K_PolyMAC() {}

  DoubleVect& calculer_terme_production_K(const Zone_PolyMAC& ,const Zone_Cl_PolyMAC&, DoubleVect& , const DoubleTab& ,
                                          const DoubleTab& , const Champ_Face_PolyMAC& , const DoubleTab& ) const;

  DoubleVect& calculer_terme_destruction_K(const Zone_PolyMAC& , const Zone_Cl_PolyMAC& , DoubleVect& ,
                                           const DoubleTab& , const DoubleTab& ,
                                           const DoubleTab& ,const DoubleVect&   ) const;

  DoubleVect& calculer_terme_destruction_K(const Zone_PolyMAC& , const Zone_Cl_PolyMAC& , DoubleVect& ,
                                           const DoubleTab& , const DoubleTab& ,
                                           double ,const DoubleVect&  ) const;

  DoubleVect& calculer_terme_destruction_K(const Zone_PolyMAC& , const Zone_Cl_PolyMAC& , DoubleVect& ,
                                           const DoubleTab& , const DoubleTab& ,
                                           const DoubleVect& ,const DoubleVect&  ,int ) const;
  void mettre_a_jour(double temps)
  {
    ;
  };
protected:


  DoubleTab& calculer_u_teta(const Zone_PolyMAC& , const Zone_Cl_PolyMAC& ,
                             const DoubleTab& , const DoubleTab& ,DoubleTab& ) const;

  DoubleTab& calculer_u_conc(const Zone_PolyMAC& , const Zone_Cl_PolyMAC& ,
                             const DoubleTab& , const DoubleTab& ,DoubleTab& ,int ) const;
};

#endif
