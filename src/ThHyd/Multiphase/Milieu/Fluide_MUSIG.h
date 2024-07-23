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

#ifndef Fluide_MUSIG_included
#define Fluide_MUSIG_included

#include <Fluide_base.h>
#include <TRUST_Deriv.h>
#include <vector>
#include <map>

/*! @brief Classe Fluide_MUSIG
 *
 *  Cette classe permettant d'instancier plusieurs phases d'un coup
 *
 *  Chaque sous phase possede la meme loi d'etat
 *
 *  Deux parametres : 'nbPhases' et 'fluide'
 *
 * @sa Fluide_base
 */
class Fluide_MUSIG : public Fluide_base
{
  Declare_instanciable( Fluide_MUSIG ) ;
public :
  inline DoubleTab& getdiametres() { return diametres_; }
  inline const DoubleTab& getdiametres() const { return diametres_; }

  inline const int& getNbSubPhase() const { return nbSubPhases_; }

  inline OWN_PTR(Fluide_base)& getFluide() { return fluide_; }
  inline const OWN_PTR(Fluide_base)& getFluide() const { return fluide_; }

  int initialiser(const double temps) override;

protected :
  DoubleTab diametres_;
  int nbSubPhases_ = -1;
  OWN_PTR(Fluide_base) fluide_;
};

#endif /* Fluide_MUSIG_included */
