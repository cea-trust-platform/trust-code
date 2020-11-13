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
// File:        Solv_GCP.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Solv_GCP_included
#define Solv_GCP_included

#include <Precond.h>
#include <solv_iteratif.h>
#include <Matrice_Morse_Sym.h>
#include <Matrice_SuperMorse.h>

class Solv_GCP : public solv_iteratif
{
  Declare_instanciable_sans_constructeur(Solv_GCP);
public :
  Solv_GCP();
  int resoudre_systeme(const Matrice_Base&, const DoubleVect&, DoubleVect&);
  int resoudre_systeme(const Matrice_Base&, const DoubleVect&, DoubleVect& , int);
  inline const Precond& get_precond() const;
  inline void set_precond(const Precond&);
  void reinit();
  int supporte_matrice_morse_sym()
  {
    return !le_precond_.non_nul() || le_precond_.supporte_matrice_morse_sym();
  };
  // GCP does not need that b has an updated virtual space...
  int get_flag_updated_input() const
  {
    return 0;
  }
protected:
  void prepare_data(const Matrice_Base& matrice, const DoubleVect& secmem, DoubleVect& solution);
  int resoudre_(const Matrice_Base&, const DoubleVect&, DoubleVect&, int);

  int optimized_;
  Precond le_precond_;
  // Parametre du jdd: veut-on appliquer un preconditionnement diagonal global ?
  // Dans ce cas, on copie la matrice, on multiplie la matrice a gauche et a droite par 1/sqrt(diagonale)
  // et on multiplie second membre avant et solution apres par la meme chose.
  // (attention, cela change la metrique du produit scalaire, donc l'interpretation du seuil):
  //     A * X = B
  // <=> D * A * X = D * B    (avec D = 1 / sqrt(diagonale))
  // <=> (D * A * D) * Y = D * B, et X = D * Y
  // Propriete: les termes diagonaux de D * A * D sont egaux a 1
  int precond_diag_;
  // Un tableau avec items virtuels
  DoubleVect tmp_p_avec_items_virt_;
  // Quatre tableaux sans items virtuels (on pourrait mettre des espaces virtuels a tous les vecteurs,
  // mais cela economise de la place dans le cache).
  DoubleVect resu_;
  DoubleVect residu_;
  // tmp_p_ pointe sur la meme zone que tmp_p_avec_items_virt_
  // (on cree cet alias car les operations entre vecteurs verifient que
  //  les tailles et structures paralleles sont identiques,
  //  par exemple le preconditionneur...)
  DoubleVect tmp_p_;
  DoubleVect tmp_solution_;
  // Matrice des coefficients reels-reels
  Matrice_Morse_Sym tmp_mat_;
  // Matrice des coefficients reels-virtuels (stockage sans les lignes vides)
  Matrice_SuperMorse tmp_mat_virt_;
  // La zone de memoire dans laquelle on stocke les vecteurs temporaires et eventuellement
  // les matrices (ne pas lire dedans, il y a aussi des entiers !)
  // Je prends des double pour etre certain que la zone alloueee est correctement alignee en memoire
  ArrOfDouble tmp_data_block_;
  // Tableau de renumerotation entre le second membre et les vecteurs temporaires
  // (suppression des items communs et virtuels non utilises)
  IntVect renum_;
  // Si on veut appliquer le preconditionnement diagonal,
  // inverse de la racine carree des termes diagonaux de la matrice
  // Attention, on met l'espace virtuel de ce vecteur a jour car on en a besoin
  // pour D*A*D
  DoubleVect inv_sqrt_diag_;
  int reinit_; // 0=> rien n'est pret 1=> memoire allouee, coeffs matrice a copier, 2=> ok
  int nb_it_max_;
};

inline void Solv_GCP::set_precond(const Precond& pre)
{
  le_precond_ = pre;
}

inline const Precond& Solv_GCP::get_precond() const
{
  return le_precond_;
}

#endif
