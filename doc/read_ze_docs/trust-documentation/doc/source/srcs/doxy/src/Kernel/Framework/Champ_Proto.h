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

#ifndef Champ_Proto_included
#define Champ_Proto_included

#include <TRUSTTab.h>

class Nom;
class Entree;

/*! @brief classe Champ_Proto Classe representant un prototype de Champ.
 *
 * Tous les classes
 *      representant des champs derivent de Champ_proto. Champ_proto offre
 *      des methodes classiques sur des champs (min, max, norme, += ...)
 *      mais ne contient pas de membre representant des donnees.
 *      La plupart des methodes de Champ_proto font appel aux methodes de
 *      meme nom d'un objet DoubleTab renvoye par la methode abstraite
 *      de Champ_proto valeurs().
 *
 * @sa Champ_base DoubleTab, Classe abstraite., Methode abstraite:, DoubleTab& valeurs()=0, const DoubleTab& valeurs() const
 */
class Champ_Proto
{
public:
  virtual ~Champ_Proto() { }
  virtual DoubleTab& valeurs()=0;
  virtual const DoubleTab& valeurs() const =0;

  /* par defaut, ces methodes renvoient valeurs() */
  virtual DoubleTab& valeurs(double temps) { return valeurs(); }
  virtual const DoubleTab& valeurs(double temps) const { return valeurs(); }
  virtual DoubleTab& futur(int i = 1) { return valeurs(); }
  virtual const DoubleTab& futur(int i = 1) const { return valeurs(); }
  virtual DoubleTab& passe(int i = 1) { return valeurs(); }
  virtual const DoubleTab& passe(int i = 1) const { return valeurs(); }

  int lire_dimension(Entree&, const Nom&);
  int lire_dimension(int dim, const Nom& le_nom_);

  // Attention : ces operateurs sont tres couteux car ils
  // utilisent la methode virtuelle valeurs(). Conseil:
  // ne pas les utiliser
  double operator()(int i, int j) const;
  double& operator()(int i, int j);
  double operator()(int i) const;
  double& operator()(int i);
};

#endif
