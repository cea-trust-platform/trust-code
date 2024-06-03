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

#ifndef Source_included
#define Source_included

#include <Source_base.h>
#include <TRUST_Deriv.h>

class Equation_base;

/*! @brief classe Source Classe generique de la hierarchie des sources, un objet Source peut
 *
 *      referencer n'importe quel d'objet derivant de Source_base.
 *      La plupart des methodes appellent les methodes de l'objet Probleme
 *      sous-jacent via la methode valeur() declaree grace a la macro
 *
 * @sa Source_base
 */
class Source : public DERIV(Source_base)
{
  Declare_instanciable(Source);
public :
  DoubleTab& ajouter(DoubleTab& ) const;
  DoubleTab& calculer(DoubleTab& ) const;
  inline void mettre_a_jour(double temps);
  void completer();
  int impr(Sortie&) const;
  void typer_direct(const Nom& );
  void typer(const Nom&, const Equation_base&);
  int initialiser(double temps);
};

/*! @brief Appel a l'objet sous-jacent.
 *
 * Mise a jour (en temps) de la source.
 *
 * @param (double temps) le pas de temps de mise a jour
 */
inline void Source::mettre_a_jour(double temps)
{
  valeur().mettre_a_jour(temps);
}

#endif
