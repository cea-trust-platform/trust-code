/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef Tetraedriser_homogene_compact_included
#define Tetraedriser_homogene_compact_included



/*! @brief class Tetra_homogene_compact Realise un maillage en decoupant chaque pave en 40 tetraedres
 *
 *  Le maillage obtenu n'a pas de directions preferentielles
 *
 *
 * @sa Interprete
 */

#include <Triangulation_base.h>
#include <Domaine.h>

class Domaine;

class Tetraedriser_homogene_compact : public Triangulation_base
{
  Declare_instanciable(Tetraedriser_homogene_compact);

public :

  void decoupe(Domaine& , Faces& , IntTab&, IntTab&, int ) const;
  void trianguler(Domaine&) const override;
  int creer_sommet (Domaine& ,DoubleTab& ,IntTab& ,IntTab& ,int , IntTab& , int& , int , int&, IntTab&, int&  ) const;
  inline int dimension_application() const override;
};

inline int Tetraedriser_homogene_compact::dimension_application() const
{
  return 3;
}

#endif

