/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Evaluateur_CoviMAC.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs/Conv_iterateur
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Evaluateur_CoviMAC_included
#define Evaluateur_CoviMAC_included

#include <IntTab.h>
#include <DoubleTab.h>
#include <Ref_Zone_CoviMAC.h>
#include <Ref_Zone_Cl_CoviMAC.h>

//
// .DESCRIPTION class Evaluateur_CoviMAC
//
// Classe de base des evaluateurs CoviMAC. Cette classe n'appartient pas a la
// hierarchie des Objet_U.
// Cette classe porte une reference a un objet de type Zone_CoviMAC et une
// reference a un objet de type Zone_Cl_CoviMAC. Elle porte des tableaux locaux
// qui sont en fait des references aux tableaux de l'objet de type Zone_CoviMAC
// (ces tableaux locaux n'existent pas en memoire).

//
// .SECTION voir aussi
//
//


class Evaluateur_CoviMAC
{

public:

  inline Evaluateur_CoviMAC();
  inline virtual ~Evaluateur_CoviMAC() {};

  Evaluateur_CoviMAC(const Evaluateur_CoviMAC& );
  virtual void associer_zones(const Zone_CoviMAC& , const Zone_Cl_CoviMAC& );
  virtual void associer_porosite(const DoubleVect&);

protected:

  REF(Zone_CoviMAC) la_zone;
  REF(Zone_Cl_CoviMAC) la_zcl;
  int dimension;
  int premiere_face_bord;
  IntTab elem_;                       // les 2 elements voisins d'une face
  DoubleVect surface;          // surfaces des faces
  DoubleVect porosite;               // porosites surfaciques
  DoubleVect volume_entrelaces;//
  DoubleTab xv;                // coord des centres des faces


};

//
//   Fonctions inline de Evaluateur_CoviMAC
//

inline Evaluateur_CoviMAC::Evaluateur_CoviMAC()
{}


#endif
