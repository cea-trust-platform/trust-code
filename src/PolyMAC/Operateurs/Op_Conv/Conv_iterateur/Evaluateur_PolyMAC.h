/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Evaluateur_PolyMAC_included
#define Evaluateur_PolyMAC_included

#include <TRUSTTab.h>
#include <TRUST_Ref.h>
class Champ_base;
class Domaine_Cl_PolyMAC;
class Domaine_PolyMAC;

/*! @brief class Evaluateur_PolyMAC
 *
 *  Classe de base des evaluateurs PolyMAC. Cette classe n'appartient pas a la
 *  hierarchie des Objet_U.
 *  Cette classe porte une reference a un objet de type Domaine_PolyMAC et une
 *  reference a un objet de type Domaine_Cl_PolyMAC. Elle porte des tableaux locaux
 *  qui sont en fait des references aux tableaux de l'objet de type Domaine_PolyMAC
 *  (ces tableaux locaux n'existent pas en memoire).
 *
 */
class Evaluateur_PolyMAC
{
public:
  Evaluateur_PolyMAC() { }
  virtual ~Evaluateur_PolyMAC() { }

  Evaluateur_PolyMAC(const Evaluateur_PolyMAC& );
  virtual void associer_domaines(const Domaine_PolyMAC& , const Domaine_Cl_PolyMAC& );
  virtual void associer_porosite(const DoubleVect&);

protected:
  REF(Domaine_PolyMAC) le_domaine;
  REF(Domaine_Cl_PolyMAC) la_zcl;
  int dimension = -3;
  int premiere_face_bord = -100;
  IntTab elem_;                       // les 2 elements voisins d'une face
  DoubleVect surface;          // surfaces des faces
  DoubleVect porosite;               // porosites surfaciques
  DoubleVect volume_entrelaces;//
  DoubleTab xv;                // coord des centres des faces
};

#endif /* Evaluateur_PolyMAC_included */
