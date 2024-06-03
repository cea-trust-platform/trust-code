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

#ifndef TroisDto2D_included
#define TroisDto2D_included

#include <Interprete_geometrique_base.h>
#include <Domaine.h>


/*! @brief Classe TroisDto2D x->alpha x
 *
 * @sa Interprete Pave, Actuellement le seul tyep d'objet reconnu par Trio-U pour mailler, une domain est l'objet Pave
 */
class TroisDto2D : public Interprete_geometrique_base
{
  Declare_instanciable(TroisDto2D);
public :
  Entree& interpreter_(Entree&) override;
  void extraire_2D(const Domaine&, Domaine&, const Bord&,const Nom& , int);

  // OC, 28/10/2004, Modif pour pouvoir extraire un bord d'orientation quelconque.
  // Renvoi la composante X  de l'origine A du repere local au bord, calcule par la classe.
  double getXa() { return xa; }
  // Renvoi la composante Y de l'origine A du repere local au bord, calcule par la classe.
  double getYa() { return ya; }
  // Renvoi la composante Z de l'origine A du repere local au bord, calcule par la classe.
  double getZa() { return za; }
  // Renvoi la composante X du premier vecteur de base I du repere local au bord, calcule par la classe.
  double getIx() { return Ix; }
  // Renvoi la composante Y du premier vecteur de base I du repere local au bord, calcule par la classe.
  double getIy() { return Iy; }
  // Renvoi la composante Z du premier vecteur de base I du repere local au bord, calcule par la classe.
  double getIz() { return Iz; }
  // Renvoi la composante X du  vecteur de base J du repere local au bord, calcule par la classe.
  double getJx() { return Jx; }
  // Renvoi la composante Y du  vecteur de base J du repere local au bord, calcule par la classe.
  double getJy() { return Jy; }
  // Renvoi la composante Z du  vecteur de base J du repere local au bord, calcule par la classe.
  double getJz() { return Jz; }
  // Renvoi la composante X du  vecteur de base K du repere local au bord, calcule par la classe.
  double getKx() { return Kx; }
  // Renvoi la composante Y du  vecteur de base K du repere local au bord, calcule par la classe.
  double getKy() { return Ky; }
  // Renvoi la composante Z du  vecteur de base K du repere local au bord, calcule par la classe.
  double getKz() { return Kz; }

  inline int& coupe() { return coupe_; }

protected :

  int coupe_ = 1;

private :
  double xa = -100., ya = -100., za = -100.;
  double Ix = -100., Iy = -100., Iz = -100.;
  double Jx = -100., Jy = -100., Jz = -100.;
  double Kx = -100., Ky = -100., Kz = -100.;
};

#endif

