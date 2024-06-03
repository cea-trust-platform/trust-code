/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Evaluateur_VDF_included
#define Evaluateur_VDF_included

#include <Domaine_VDF.h>
#include <TRUST_Ref.h>
#include <TRUSTTab.h>

class Domaine_Cl_VDF;

/*! @brief class Evaluateur_VDF Classe de base des evaluateurs VDF.
 *
 *  Cette classe n'appartient pas a la hierarchie des Objet_U.
 *  Cette classe porte une reference a un objet de type Domaine_VDF et une reference a un objet de type Domaine_Cl_VDF. Elle porte des tableaux locaux
 *  qui sont en fait des references aux tableaux de l'objet de type Domaine_VDF (ces tableaux locaux n'existent pas en memoire).
 *
 */

class Evaluateur_VDF
{
public:
  Evaluateur_VDF() {}
  virtual ~Evaluateur_VDF() {}
  Evaluateur_VDF(const Evaluateur_VDF& );
  virtual void associer_domaines(const Domaine_VDF& , const Domaine_Cl_VDF& );
  virtual void associer_porosite(const DoubleVect&);

  inline double dist_face_period(int fac1, int fac2, int k) const { return le_dom->dist_face_period(fac1,fac2,k); }
  inline double dist_face(int fac1, int fac2, int k) const
  {
    return xv(fac2,k) - xv(fac1,k);
    //return le_dom->dist_face(fac1, fac2, k);
  }

protected:
  REF(Domaine_VDF) le_dom;
  REF(Domaine_Cl_VDF) la_zcl;
  int dimension = -100, premiere_face_bord = -100;
  IntTab elem_;                       // les 2 elements voisins d'une face
  DoubleVect surface;          // surfaces des faces
  IntVect orientation;         // orientations des faces
  DoubleVect porosite;               // porosites surfaciques
  DoubleVect volume_entrelaces;//
  DoubleTab xv;                // coord des centres des faces
  double dist_norm_bord(int) const;
};

#endif /* Evaluateur_VDF_included */
