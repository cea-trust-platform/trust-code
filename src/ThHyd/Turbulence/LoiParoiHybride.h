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
// File:        LoiParoiHybride.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef LoiParoiHybride_included
#define LoiParoiHybride_included

#include <Noms.h>
#include <Vect_Turbulence_paroi.h>

class Zone_dis_base;
class Zone_Cl_dis_base;
class DoubleTab;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION LoiParoiHybride
//    Classe Turbulence_paroi_base
//    Cette loi de paroi permet de choisir des lois de parois differentes par conditions aux limites.
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
class LoiParoiHybride
{

public :


  Entree& lire(Entree& , const Noms&, const Mod_turb_hyd_base&);
  void associer(const Zone_dis&, const Zone_Cl_dis&);
  int init_lois_paroi() ;

  // Simple appel aux calculer_hyd des differentes lois de parois
  int calculer_hyd(DoubleTab& ) ;

  //Appel des calculer_hyd des differentes lois de parois et remplissage du tableau de Cisaillement
  int calculer_hyd(DoubleTab& tab1, const Zone_dis_base& , const Zone_Cl_dis_base& , DoubleTab& );

  // Simple appel aux calculer_hyd des differentes lois de parois
  int calculer_hyd(DoubleTab& , DoubleTab& ) ;
  //Appel des calculer_hyd des differentes lois de parois et remplissage du tableau de Cisaillement
  int calculer_hyd(DoubleTab& , DoubleTab&, const Zone_dis_base& , const Zone_Cl_dis_base& , DoubleTab&  ) ;

  // Imprimer_u_star de la loi de paroi Hybride n'est pas encore OK.
  //void imprimer_ustar(Sortie& ) const ;



private:
  VECT(Turbulence_paroi) vect_lp;
  IntVect lp_bord; // indice dans le vecteur precedent de la loi de paroi correspondant a chaque  bord
};


#endif
