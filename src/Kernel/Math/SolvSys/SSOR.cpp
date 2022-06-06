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

#include <Matrice_Morse_Sym.h>
#include <Matrice_Bloc_Sym.h>
#include <MD_Vector_tools.h>
#include <MD_Vector_base.h>
#include <TRUSTTab_parts.h>
#include <Motcle.h>
#include <Param.h>
#include <SSOR.h>

Implemente_instanciable_sans_constructeur(SSOR,"SSOR",Precond_base);

SSOR::SSOR() : omega_(1.6), algo_fortran_(-1), avec_assert_(-1), algo_items_communs_(-1), line_size_(0) { }

Sortie& SSOR::printOn(Sortie& s ) const
{
  s << " { omega  "<<omega_ << " } ";
  return s;
}

Entree& SSOR::readOn(Entree& is )
{
  Param param(que_suis_je());
  param.ajouter("omega", &omega_);
  param.lire_avec_accolades(is);

  if (omega_ <= 0. || omega_ >= 2.)
    {
      Cerr << "SSOR::readOn, omega is not within [0, 2]: SSOR not activated (you should use precond nul instead) "  << finl;
    }
  return is;
}

void SSOR::prepare_(const Matrice_Base& la_matrice, const DoubleVect& secmem)
{
  if (get_status() >= REINIT_ALL)
    {
      md_secmem_ = secmem.get_md_vector();
      line_size_ = secmem.line_size();
      // Pour le prochain preconditionnement, verifier la matrice
      avec_assert_ = 1;

      if (nproc() == 1 || !(md_secmem_.non_nul())) algo_items_communs_ = 0;
      else
        {
          // Nombre d'items sequentiels sur ce proc
          const int sz_tot = secmem.size_totale();
          items_a_traiter_.reset();
          items_a_traiter_.resize(sz_tot / line_size_, line_size_, Array_base::NOCOPY_NOINIT);
          items_a_traiter_.set_md_vector(md_secmem_);
          int n = MD_Vector_tools::get_sequential_items_flags(md_secmem_, items_a_traiter_, line_size_);
          int sz = md_secmem_.valeur().get_nb_items_reels();

          if (sz < 0) // size() est invalide, les items reels ne sont pas groupes a debut !
            algo_items_communs_ = 1;
          else
            {
              assert(sz >= n);
              if (mp_sum(sz) > mp_sum(n)) algo_items_communs_ = 1; // Il y a des items partages parmi les items reels
              else
                {
                  algo_items_communs_ = 0;
                  items_a_traiter_.reset();
                }
            }
        }
    }

  if (get_status() >= REINIT_COEFF) { /* Do nothing */ }
  Precond_base::prepare_(la_matrice, secmem);
}

// Description:
//    Calcule la solution du systeme lineaire: A * solution = b avec la methode de relaxation SSOR.
int SSOR::preconditionner_(const Matrice_Base& la_matrice, const DoubleVect& b, DoubleVect& solution)
{
  // pour compatibilite historique:
  if (omega_ <= 0. || omega_ >= 2)
    {
      operator_egal(solution, b);
      return 1;
    }

  operator_egal(solution, b);

  if (sub_type(Matrice_Morse_Sym, la_matrice))
    {
      const Matrice_Morse_Sym& matrice = ref_cast(Matrice_Morse_Sym, la_matrice);
      ssor(matrice, solution);
    }
  else if (sub_type(Matrice_Bloc_Sym, la_matrice))
    {
      // Matrice correspondant a un vecteur multi-localisation (MD_Vector_composite)
      const Matrice_Bloc_Sym& matrice = ref_cast(Matrice_Bloc_Sym, la_matrice);
      ssor(matrice, solution);
    }
  else if (sub_type(Matrice_Bloc, la_matrice))
    {
      // On suppose une matrice reelle-reelle et une matrice reelle-virtuelle
      const Matrice_Bloc& mat = ref_cast(Matrice_Bloc, la_matrice);
      const Matrice_Morse_Sym& matrice = ref_cast(Matrice_Morse_Sym, mat.get_bloc(0, 0).valeur());
      ssor(matrice, solution);
    }
  else
    {
      Cerr << "SSOR::preconditionner not coded for type " << la_matrice.que_suis_je() << finl;
      exit();
    }
  if (echange_ev_solution_) solution.echange_espace_virtuel();
  return 1;
}

void traite_diagonale(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur)
{
  const int nb_lignes_a_traiter = vecteur.size_reelle_ok() ? vecteur.size_reelle() : vecteur.size_totale();

  const double psi = (2. - omega) / omega;
  const double *coeff_fortran = coeff.addr() - 1; // indexable par index fortran
  const int *tab1_ptr = tab1.addr();
  double *vect_ptr = vecteur.addr();
  for (int i = nb_lignes_a_traiter; i; i--, tab1_ptr++, vect_ptr++)
    {
      const int j = *tab1_ptr;
      // Coefficient diagonale de la ligne i:
      const double coeff_i_i = coeff_fortran[j];
      (*vect_ptr) *= psi * coeff_i_i;
    }
}

enum class descente_enum { NORMAL , NORMAL_ASSERT , DIAG , DIAG_ASSERT };

template<descente_enum _TYPE_>
void descente_generique(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur)
{
  static constexpr bool IS_NORMAL_ASSERT = (_TYPE_ == descente_enum::NORMAL_ASSERT), IS_DIAG_ASSERT = (_TYPE_ == descente_enum::DIAG_ASSERT), NOT_DIAG = (_TYPE_ != descente_enum::DIAG && _TYPE_ != descente_enum::DIAG_ASSERT);

  const int nb_lignes_a_traiter = vecteur.size_reelle_ok() ? vecteur.size_reelle() : vecteur.size_totale();
  // pointeur "fortran" vers le tableau solution (indexable avec index fortran) le pointeur est constant, pas le tableau pointe.
  double * const sol_fortran = vecteur.addr() - 1;
  const int *tab1_ptr = tab1.addr();
  assert(nb_lignes_a_traiter <= tab1.size_array() + 1);
  assert(*tab1_ptr == 1); // sinon 2 lignes ci-dessous fausses.
  const int *tab2_ptr = tab2.addr();
  const double *coeff_ptr = coeff.addr();
  int last_tab1_de_i = *tab1_ptr;
  tab1_ptr++;
  for (int i = 1; i <= nb_lignes_a_traiter; i++, tab1_ptr++)
    {
      const int tab1_de_i = *tab1_ptr; // = tab1[i]
      const int nvois = tab1_de_i - last_tab1_de_i;

      if (IS_NORMAL_ASSERT || IS_DIAG_ASSERT) assert(nvois >= 0 && (tab1_de_i - 1) <= tab2.size_array());

      last_tab1_de_i = tab1_de_i;
      // Ce test doit rester, sinon on pollue les autres lignes du vecteur sol avec des valeurs dependant des items communs et virtuels
      {
        double v_i;
        if (NOT_DIAG)
          {
            // Le premier coeff doit etre le coef diagonal et doit etre strictement positif
            if (IS_NORMAL_ASSERT)  assert(nvois >= 1 && (*tab2_ptr) == i && (*coeff_ptr) > 0.);

            const double omega_coeff_i_i = omega / (*coeff_ptr);
            v_i = sol_fortran[i] *= omega_coeff_i_i;
            tab2_ptr++;
            coeff_ptr++;
          }
        else // PRECOND DIAG !!
          {
            // Pas de coefficient diagonal stocke (ni dans tab2 ni dans coeff), la diagonale vaut 1:
            v_i = sol_fortran[i] *= omega;
          }

        for (int j = nvois-1; j; j--, tab2_ptr++, coeff_ptr++)
          {
            const int i2 = *tab2_ptr; // indice de colonne pour le prochain coefficient
            const double coeff_i_i2 = *coeff_ptr;
            // la matrice n'a que des coeffs diagonaux superieurs et on a deja traite la diagonale, donc i2 > i. Pas d'items virtuels autorises !
            if (IS_NORMAL_ASSERT || IS_DIAG_ASSERT) assert(i2 > i && i2 <= nb_lignes_a_traiter);
            // B.M.: ... en revanche, le test sur les items communs est superflu ici car la valeur sera annulee (voir **Annulation items communs**),
            // gain de perfs si on ne fait pas le test
            sol_fortran[i2] -= coeff_i_i2 * v_i;
          }
      }
    }
}

void descente(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur)
{
  descente_generique<descente_enum::NORMAL>(omega,tab1,tab2,coeff,vecteur);
}

void descente_assert(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur)
{
  descente_generique<descente_enum::NORMAL_ASSERT>(omega,tab1,tab2,coeff,vecteur);
}

void descente_diag_ok_assert(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur)
{
  descente_generique<descente_enum::NORMAL_ASSERT>(omega,tab1,tab2,coeff,vecteur); /* meme qu'avant :D */
}

void descente_precond_diag(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur)
{
  descente_generique<descente_enum::DIAG>(omega,tab1,tab2,coeff,vecteur);
}

void descente_assert_precond_diag(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur)
{
  descente_generique<descente_enum::DIAG_ASSERT>(omega,tab1,tab2,coeff,vecteur);
}

// Descente sur un bloc extradiagonal. Methode appelee par SSOR(const Matrice_bloc & ...)
// vecteur: de taille "nombre de lignes de la matrice", vecteur2: "nombre de colonnes"
void descente_bloc_extradiag_assert(const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, const DoubleVect& vecteur, DoubleVect& vecteur2)
{
  const int nb_lignes = vecteur.size_reelle_ok() ? vecteur.size_reelle() : vecteur.size_totale();
  const double *vecteur_ptr = vecteur.addr();
  double *vecteur2_fortran_ptr = vecteur2.addr() - 1;
  const int *tab2_fortran_ptr = tab2.addr() - 1;
  const double *coeff_fortran_ptr = coeff.addr() - 1;
  assert(coeff.size_array() == tab2.size_array());
  for (int i_ligne = 0; i_ligne < nb_lignes; i_ligne++, vecteur_ptr++)
    {
      {
        const double v_i = *vecteur_ptr;
        int index = tab1[i_ligne];
        const int index_fin = tab1[i_ligne + 1];
        // Il peut n'y avoir aucun coefficient sur la ligne => index_fin == index
        assert(index > 0 && index_fin >= index && index_fin <= tab2.size_array() + 1);
        const int *tab2_ptr = tab2_fortran_ptr + index;
        const double *coeff_ptr = coeff_fortran_ptr + index;
        for (; index < index_fin; index++, tab2_ptr++, coeff_ptr++)
          {
            const int i_colonne = *tab2_ptr;
            assert(i_colonne >= 1 && i_colonne <= vecteur2.size_array());
            const double c = *coeff_ptr;
            // pas de test item commun sur les colonnes, voir **Annulation items communs**
            vecteur2_fortran_ptr[i_colonne] -= c * v_i;
          }
      }
    }
}

template<descente_enum _TYPE_>
void descente_generique(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur, const ArrOfInt& items_a_traiter)
{
  static constexpr bool IS_NORMAL_ASSERT = (_TYPE_ == descente_enum::NORMAL_ASSERT), IS_DIAG_ASSERT = (_TYPE_ == descente_enum::DIAG_ASSERT), NOT_DIAG = (_TYPE_ != descente_enum::DIAG && _TYPE_ != descente_enum::DIAG_ASSERT);

  const int nb_lignes_a_traiter = vecteur.size_reelle_ok() ? vecteur.size_reelle() : vecteur.size_totale();
  // pointeur "fortran" vers le tableau solution (indexable avec index fortran) le pointeur est constant, pas le tableau pointe.
  double * const sol_fortran = vecteur.addr() - 1;
  const int *flags_ptr = items_a_traiter.addr();
  assert(nb_lignes_a_traiter <= items_a_traiter.size_array());
  const int *tab1_ptr = tab1.addr();
  assert(nb_lignes_a_traiter <= tab1.size_array() + 1);
  assert(*tab1_ptr == 1); // sinon 2 lignes ci-dessous fausses.
  const int *tab2_ptr = tab2.addr();
  const double *coeff_ptr = coeff.addr();
  int last_tab1_de_i = *tab1_ptr;
  tab1_ptr++;
  for (int i = 1; i <= nb_lignes_a_traiter; i++, tab1_ptr++)
    {
      const int tab1_de_i = *tab1_ptr; // = tab1[i]
      const int nvois = tab1_de_i - last_tab1_de_i;

      if (IS_NORMAL_ASSERT || IS_DIAG_ASSERT) assert(nvois >= 0 && (tab1_de_i - 1) <= tab2.size_array());

      last_tab1_de_i = tab1_de_i;
      // Ce test doit rester, sinon on pollue les autres lignes du vecteur sol avec des valeurs dependant des items communs et virtuels
      if (IS_NORMAL_ASSERT || IS_DIAG_ASSERT) assert((flags_ptr - items_a_traiter.addr()) == (i-1));

      const int item_a_traiter = *(flags_ptr++);
      if (item_a_traiter)
        {
          double v_i;
          if (NOT_DIAG)
            {
              // Le premier coeff doit etre le coef diagonal et doit etre strictement positif
              if (IS_NORMAL_ASSERT) assert(nvois >= 1 && (*tab2_ptr) == i && (*coeff_ptr) > 0.);

              const double omega_coeff_i_i = omega / (*coeff_ptr);
              v_i = sol_fortran[i] *= omega_coeff_i_i;
              tab2_ptr++;
              coeff_ptr++;
            }
          else // PRECOND DIAG !!
            {
              // Pas de coefficient diagonal stocke (ni dans tab2 ni dans coeff), la diagonale vaut 1:
              v_i = sol_fortran[i] *= omega;
            }

          for (int j = nvois-1; j; j--, tab2_ptr++, coeff_ptr++)
            {
              const int i2 = *tab2_ptr; // indice de colonne pour le prochain coefficient
              const double coeff_i_i2 = *coeff_ptr;
              // la matrice n'a que des coeffs diagonaux superieurs et on a deja traite la diagonale, donc i2 > i.
              if (IS_NORMAL_ASSERT || IS_DIAG_ASSERT) assert(i2 > i && i2 <= vecteur.size_totale());

              // B.M.: ... en revanche, le test sur les items communs est superflu ici car
              // la valeur sera annulee (voir **Annulation items communs**), gain de perfs si on ne fait pas le test
              sol_fortran[i2] -= coeff_i_i2 * v_i;
            }
        }
      else
        {
          // **Annulation items communs**
          // c'est la derniere fois qu'on ecrit dans sol_fortran[i] car la matrice est diagonale superieure. Pour eviter le test sur items_a_traiter_
          // dans la remontee, on annule la solution pour les items communs et virtuels (sol_fortran[i] ne sera plus modifie ensuite dans la descente)
          sol_fortran[i] = 0.;
          coeff_ptr += nvois;
          tab2_ptr += nvois;
        }
    }
  // **Annulation items communs** : Pour la remontee il faut annuler les valeurs dans les cases virtuelles
  {
    const int fin = vecteur.size_totale();
    for (int i = nb_lignes_a_traiter+1; i <= fin; i++)
      sol_fortran[i] = 0.;
  }
}

void descente(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur, const ArrOfInt& items_a_traiter)
{
  descente_generique<descente_enum::NORMAL>(omega,tab1,tab2,coeff,vecteur,items_a_traiter);
}

void descente_assert(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur, const ArrOfInt& items_a_traiter)
{
  descente_generique<descente_enum::NORMAL_ASSERT>(omega,tab1,tab2,coeff,vecteur,items_a_traiter);
}

void descente_diag_ok_assert(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur, const ArrOfInt& items_a_traiter)
{
  descente_generique<descente_enum::NORMAL_ASSERT>(omega,tab1,tab2,coeff,vecteur,items_a_traiter); /* meme qu'avant :D */
}

void descente_precond_diag(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur, const ArrOfInt& items_a_traiter)
{
  descente_generique<descente_enum::DIAG>(omega,tab1,tab2,coeff,vecteur,items_a_traiter);
}

void descente_assert_precond_diag(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur, const ArrOfInt& items_a_traiter)
{
  descente_generique<descente_enum::DIAG_ASSERT>(omega,tab1,tab2,coeff,vecteur,items_a_traiter);
}

// Descente sur un bloc extradiagonal. Methode appelee par SSOR(const Matrice_bloc & ...)
// vecteur: de taille "nombre de lignes de la matrice", vecteur2: "nombre de colonnes"
void descente_bloc_extradiag_assert(const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, const DoubleVect& vecteur, DoubleVect& vecteur2, const ArrOfInt& items_a_traiter)
{
  const int nb_lignes = vecteur.size_reelle_ok() ? vecteur.size_reelle() : vecteur.size_totale();
  const int *flags_ptr = items_a_traiter.addr();
  const double *vecteur_ptr = vecteur.addr();
  double *vecteur2_fortran_ptr = vecteur2.addr() - 1;
  const int *tab2_fortran_ptr = tab2.addr() - 1;
  const double *coeff_fortran_ptr = coeff.addr() - 1;
  assert(coeff.size_array() == tab2.size_array());
  for (int i_ligne = 0; i_ligne < nb_lignes; i_ligne++, vecteur_ptr++)
    {
      if (*(flags_ptr++))
        {
          const double v_i = *vecteur_ptr;
          int index = tab1[i_ligne];
          const int index_fin = tab1[i_ligne + 1];
          // Il peut n'y avoir aucun coefficient sur la ligne => index_fin == index
          assert(index > 0 && index_fin >= index && index_fin <= tab2.size_array() + 1);
          const int *tab2_ptr = tab2_fortran_ptr + index;
          const double *coeff_ptr = coeff_fortran_ptr + index;
          for (; index < index_fin; index++, tab2_ptr++, coeff_ptr++)
            {
              const int i_colonne = *tab2_ptr;
              assert(i_colonne >= 1 && i_colonne <= vecteur2.size_array());
              const double c = *coeff_ptr;
              // pas de test item commun sur les colonnes, voir **Annulation items communs**
              vecteur2_fortran_ptr[i_colonne] -= c * v_i;
            }
        }
    }
}

enum class remontee_enum { NORMAL , NORMAL_ASSERT , DIAG_OK_ASSERT , DIAG , DIAG_ASSERT };

template<remontee_enum _TYPE_>
void remontee_generique(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur)
{
  static constexpr bool IS_NORMAL_ASSERT = (_TYPE_ == remontee_enum::NORMAL_ASSERT), IS_DIAG_OK_ASSERT = (_TYPE_ == remontee_enum::DIAG_OK_ASSERT), IS_DIAG_ASSERT = (_TYPE_ == remontee_enum::DIAG_ASSERT),
                        NOT_DIAG = (_TYPE_ != remontee_enum::DIAG && _TYPE_ != remontee_enum::DIAG_ASSERT);

  const int nb_lignes_a_traiter = vecteur.size_reelle_ok() ? vecteur.size_reelle() : vecteur.size_totale();
  const double psi = IS_DIAG_OK_ASSERT ? -1e10 : (2. - omega) / omega;

  // pointeur "fortran" vers le tableau solution (indexable avec index fortran)
  const double *const sol_fortran = vecteur.addr() - 1;
  const int *tab1_ptr = tab1.addr() + nb_lignes_a_traiter;
  int last_tab1_de_i = *tab1_ptr;
  tab1_ptr--;
  // On ne va pas a la fin de tab2 car on n'est pas sur que nb_lignes_a_traiter = tab1.size_array() :
  // -2 car last_tab1_de_i est l'indice du premier coefficient de la ligne suivante en fortran
  //  (dont -1 pour fortran->c et -1 pour passer au dernier coeff de la ligne precedente)
  const int *tab2_ptr = tab2.addr() + last_tab1_de_i - 2;
  const double *coeff_ptr = coeff.addr() + last_tab1_de_i - 2;
  double *soli_ptr = vecteur.addr() + nb_lignes_a_traiter - 1;
  for (int i = nb_lignes_a_traiter; i; i--, tab1_ptr--, soli_ptr--)
    {
      const int tab1_de_i = *tab1_ptr; // = tab1[i]
      const int nvois = last_tab1_de_i - tab1_de_i;

      if (IS_NORMAL_ASSERT || IS_DIAG_OK_ASSERT || IS_DIAG_ASSERT) assert(nvois >= 0 && tab1_de_i > 0);

      last_tab1_de_i = tab1_de_i;
      // Ce test doit rester car il ne faut pas modifier sol[i] pour les items communs
      // et virtuels (ils sont nuls et doivent le rester pour ne pas polluer les autres lignes):
      if (1)
        {
          // Operation "diagonale":
          double x = 0.;
          for (int j = nvois - 1; j; j--, tab2_ptr--, coeff_ptr--)
            {
              const int i2 = *tab2_ptr;
              const double coeff_i_i2 = *coeff_ptr;
              // Operation nulle pour les items communs et virtuels
              // (on a annule le second membre lors de la descente)
              x += coeff_i_i2 * sol_fortran[i2];
            }

          if (NOT_DIAG)
            {
              // ici coeff_ptr est le coeff diagonal
              if (IS_NORMAL_ASSERT || IS_DIAG_OK_ASSERT) assert((*tab2_ptr) == i);

              const double coeff_i_i = *coeff_ptr;
              const double omega_coeff_i_i = omega / coeff_i_i;
              // Si IS_DIAG_OK_ASSERT : La diagonale a deja ete multipliee par psi * coeff_i_i
              *soli_ptr = IS_DIAG_OK_ASSERT ? ((*soli_ptr) - x) * omega_coeff_i_i : (*soli_ptr) * psi * omega - x * omega_coeff_i_i;
              coeff_ptr--;
              tab2_ptr--;
            }
          else // PRECOND DIAG !!
            {
              // Preconditionnement diagonal, pas de coefficient diagonal stocke:
              *soli_ptr = ((*soli_ptr) * psi - x) * omega;
            }
        }
      else
        {
          assert((*soli_ptr) == 0.);
          coeff_ptr -= nvois;
          tab2_ptr -= nvois;
        }
    }
}

void remontee(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur)
{
  remontee_generique<remontee_enum::NORMAL>(omega,tab1,tab2,coeff,vecteur);
}

void remontee_assert(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur)
{
  remontee_generique<remontee_enum::NORMAL_ASSERT>(omega,tab1,tab2,coeff,vecteur);
}

void remontee_diag_ok_assert(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur)
{
  remontee_generique<remontee_enum::DIAG_OK_ASSERT>(omega,tab1,tab2,coeff,vecteur);
}

void remontee_precond_diag(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur)
{
  remontee_generique<remontee_enum::DIAG>(omega,tab1,tab2,coeff,vecteur);
}

void remontee_assert_precond_diag(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur)
{
  remontee_generique<remontee_enum::DIAG_ASSERT>(omega,tab1,tab2,coeff,vecteur);
}

// Remontee sur un bloc extradiagonal. Methode appelee par SSOR(const Matrice_bloc & ...)
// vecteur: de taille "nombre de lignes de la matrice", vecteur2: "nombre de colonnes"
void remontee_bloc_extradiag_assert(const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur, const DoubleVect& vecteur2)
{
  const int nb_lignes = vecteur.size_reelle_ok() ? vecteur.size_reelle() : vecteur.size_totale();
  double *vecteur_ptr = vecteur.addr();
  const double *vecteur2_fortran_ptr = vecteur2.addr() - 1;
  const int *tab2_fortran_ptr = tab2.addr() - 1;
  const double *coeff_fortran_ptr = coeff.addr() - 1;
  assert(coeff.size_array() == tab2.size_array());
  for (int i_ligne = 0; i_ligne < nb_lignes; i_ligne++, vecteur_ptr++)
    {
      {
        double x = *vecteur_ptr;
        int index = tab1[i_ligne];
        const int index_fin = tab1[i_ligne + 1];
        // Il peut n'y avoir aucun coefficient sur la ligne => index_fin == index
        assert(index > 0 && index_fin >= index && index_fin <= tab2.size_array() + 1);
        const int *tab2_ptr = tab2_fortran_ptr + index;
        const double *coeff_ptr = coeff_fortran_ptr + index;
        for (; index < index_fin; index++, tab2_ptr++, coeff_ptr++)
          {
            const int i_colonne = *tab2_ptr;
            assert(i_colonne >= 1 && i_colonne <= vecteur2.size_array());
            const double c = *coeff_ptr;
            const double x2 = vecteur2_fortran_ptr[i_colonne];
            // pas de test item commun sur les colonnes, voir **Annulation items communs**
            x -= c * x2;
          }
        *vecteur_ptr = x;
      }
    }
}

template<remontee_enum _TYPE_>
void remontee_generique(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur, const ArrOfInt& items_a_traiter)
{
  static constexpr bool IS_NORMAL_ASSERT = (_TYPE_ == remontee_enum::NORMAL_ASSERT), IS_DIAG_OK_ASSERT = (_TYPE_ == remontee_enum::DIAG_OK_ASSERT), IS_DIAG_ASSERT = (_TYPE_ == remontee_enum::DIAG_ASSERT),
                        NOT_DIAG = (_TYPE_ != remontee_enum::DIAG && _TYPE_ != remontee_enum::DIAG_ASSERT);

  const int nb_lignes_a_traiter = vecteur.size_reelle_ok() ? vecteur.size_reelle() : vecteur.size_totale();
  const int *flags_ptr = items_a_traiter.addr() + nb_lignes_a_traiter - 1;
  assert(nb_lignes_a_traiter <= items_a_traiter.size_array());
  const double psi =  IS_DIAG_OK_ASSERT ? -1e10 : (2. - omega) / omega;

  // pointeur "fortran" vers le tableau solution (indexable avec index fortran)
  const double * const sol_fortran = vecteur.addr() - 1;
  const int *tab1_ptr = tab1.addr() + nb_lignes_a_traiter;
  int last_tab1_de_i = *tab1_ptr;
  tab1_ptr--;
  // On ne va pas a la fin de tab2 car on n'est pas sur que nb_lignes_a_traiter = tab1.size_array() :
  // -2 car last_tab1_de_i est l'indice du premier coefficient de la ligne suivante en fortran
  //  (dont -1 pour fortran->c et -1 pour passer au dernier coeff de la ligne precedente)
  const int *tab2_ptr = tab2.addr() + last_tab1_de_i - 2;
  const double *coeff_ptr = coeff.addr() + last_tab1_de_i - 2;
  double * soli_ptr = vecteur.addr() + nb_lignes_a_traiter - 1;
  for (int i = nb_lignes_a_traiter; i; i--, tab1_ptr--, soli_ptr--)
    {
      const int tab1_de_i = *tab1_ptr; // = tab1[i]
      const int nvois = last_tab1_de_i - tab1_de_i;
      if (IS_NORMAL_ASSERT || IS_DIAG_OK_ASSERT || IS_DIAG_ASSERT) assert(nvois >= 0 && tab1_de_i > 0);

      last_tab1_de_i = tab1_de_i;
      // Ce test doit rester car il ne faut pas modifier sol[i] pour les items communs
      // et virtuels (ils sont nuls et doivent le rester pour ne pas polluer les autres lignes):
      if (*(flags_ptr--))
        {
          // Operation "diagonale":
          double x = 0.;
          for (int j = nvois-1; j; j--, tab2_ptr--, coeff_ptr--)
            {
              const int i2 = *tab2_ptr;
              const double coeff_i_i2 = *coeff_ptr;
              // Operation nulle pour les items communs et virtuels
              // (on a annule le second membre lors de la descente)
              x += coeff_i_i2 * sol_fortran[i2];
            }
          if (NOT_DIAG)
            {
              // ici coeff_ptr est le coeff diagonal
              if (IS_NORMAL_ASSERT || IS_DIAG_OK_ASSERT) assert((*tab2_ptr) == i);

              const double coeff_i_i = *coeff_ptr;
              const double omega_coeff_i_i = omega / coeff_i_i;
              // Si IS_DIAG_OK_ASSERT : La diagonale a deja ete multipliee par psi * coeff_i_i
              *soli_ptr = IS_DIAG_OK_ASSERT ? ((*soli_ptr) - x) * omega_coeff_i_i : (*soli_ptr) * psi * omega - x * omega_coeff_i_i;
              coeff_ptr--;
              tab2_ptr--;
            }
          else // PRECOND DIAG !!
            {
              // Preconditionnement diagonal, pas de coefficient diagonal stocke:
              *soli_ptr = ((*soli_ptr) * psi - x) * omega;
            }
        }
      else
        {
          assert((*soli_ptr) == 0.);
          coeff_ptr -= nvois;
          tab2_ptr -= nvois;
        }
    }
}

void remontee(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur, const ArrOfInt& items_a_traiter)
{
  remontee_generique<remontee_enum::NORMAL>(omega,tab1,tab2,coeff,vecteur,items_a_traiter);
}

void remontee_assert(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur, const ArrOfInt& items_a_traiter)
{
  remontee_generique<remontee_enum::NORMAL_ASSERT>(omega,tab1,tab2,coeff,vecteur,items_a_traiter);
}

void remontee_diag_ok_assert(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur, const ArrOfInt& items_a_traiter)
{
  remontee_generique<remontee_enum::DIAG_OK_ASSERT>(omega,tab1,tab2,coeff,vecteur,items_a_traiter);
}

void remontee_precond_diag(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur, const ArrOfInt& items_a_traiter)
{
  remontee_generique<remontee_enum::DIAG>(omega,tab1,tab2,coeff,vecteur,items_a_traiter);
}

void remontee_assert_precond_diag(const double omega, const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur, const ArrOfInt& items_a_traiter)
{
  remontee_generique<remontee_enum::DIAG_ASSERT>(omega,tab1,tab2,coeff,vecteur,items_a_traiter);
}

// Remontee sur un bloc extradiagonal. Methode appelee par SSOR(const Matrice_bloc & ...)
// vecteur: de taille "nombre de lignes de la matrice", vecteur2: "nombre de colonnes"
void remontee_bloc_extradiag_assert(const ArrOfInt& tab1, const ArrOfInt& tab2, const ArrOfDouble& coeff, DoubleVect& vecteur, const DoubleVect& vecteur2, const ArrOfInt& items_a_traiter)
{
  const int nb_lignes = vecteur.size_reelle_ok() ? vecteur.size_reelle() : vecteur.size_totale();
  const int *flags_ptr = items_a_traiter.addr();
  double *vecteur_ptr = vecteur.addr();
  const double *vecteur2_fortran_ptr = vecteur2.addr() - 1;
  const int *tab2_fortran_ptr = tab2.addr() - 1;
  const double *coeff_fortran_ptr = coeff.addr() - 1;
  assert(coeff.size_array() == tab2.size_array());
  for (int i_ligne = 0; i_ligne < nb_lignes; i_ligne++, vecteur_ptr++)
    {
      if (*(flags_ptr++))
        {
          double x = *vecteur_ptr;
          int index = tab1[i_ligne];
          const int index_fin = tab1[i_ligne + 1];
          // Il peut n'y avoir aucun coefficient sur la ligne => index_fin == index
          assert(index > 0 && index_fin >= index && index_fin <= tab2.size_array() + 1);
          const int *tab2_ptr = tab2_fortran_ptr + index;
          const double *coeff_ptr = coeff_fortran_ptr + index;
          for (; index < index_fin; index++, tab2_ptr++, coeff_ptr++)
            {
              const int i_colonne = *tab2_ptr;
              assert(i_colonne >= 1 && i_colonne <= vecteur2.size_array());
              const double c = *coeff_ptr;
              const double x2 = vecteur2_fortran_ptr[i_colonne];
              // pas de test item commun sur les colonnes, voir **Annulation items communs**
              x -= c * x2;
            }
          *vecteur_ptr = x;
        }
    }
}

// On calcule solution = inverse(C)*b avec:
//   inverse(C) = inverse((1/w D - E)) * (2-w/w D) * inverse((1/wD -E)t)
//   D :partie diagonale de la matrice, E :partie triangulaire inferieure de la matrice
void SSOR::ssor(const Matrice_Morse_Sym& matrice, DoubleVect& solution)
{
  const ArrOfInt& tab1 = matrice.get_tab1(), &tab2 = matrice.get_tab2();
  const ArrOfDouble& coeff = matrice.get_coeff();

  if (tab2.size_array() > 0 && tab2[0] == 1)
    {
      // La diagonale est presente dans la matrice
      if (avec_assert_)
        {
          if (algo_items_communs_)
            {
              descente_assert(omega_, tab1, tab2, coeff, solution, items_a_traiter_);
              remontee_assert(omega_, tab1, tab2, coeff, solution, items_a_traiter_);
            }
          else
            {
              descente_assert(omega_, tab1, tab2, coeff, solution);
              remontee_assert(omega_, tab1, tab2, coeff, solution);
            }
        }
      else
        {
          if (algo_items_communs_)
            {
              descente(omega_, tab1, tab2, coeff, solution, items_a_traiter_);
              remontee(omega_, tab1, tab2, coeff, solution, items_a_traiter_);
            }
          else
            {
              descente(omega_, tab1, tab2, coeff, solution);
              remontee(omega_, tab1, tab2, coeff, solution);
            }
        }
    }
  else
    {
      // Pas de diagonale stockee, on suppose qu'on a que des 1 sur la diagonale (preconditionnement diagonal)
      if (avec_assert_)
        {
          if (algo_items_communs_)
            {
              descente_assert_precond_diag(omega_, tab1, tab2, coeff, solution, items_a_traiter_);
              remontee_assert_precond_diag(omega_, tab1, tab2, coeff, solution, items_a_traiter_);
            }
          else
            {
              descente_assert_precond_diag(omega_, tab1, tab2, coeff, solution);
              remontee_assert_precond_diag(omega_, tab1, tab2, coeff, solution);
            }
        }
      else
        {
          if (algo_items_communs_)
            {
              descente_precond_diag(omega_, tab1, tab2, coeff, solution, items_a_traiter_);
              remontee_precond_diag(omega_, tab1, tab2, coeff, solution, items_a_traiter_);
            }
          else
            {
              descente_precond_diag(omega_, tab1, tab2, coeff, solution);
              remontee_precond_diag(omega_, tab1, tab2, coeff, solution);
            }
        }
    }
  avec_assert_ = 0; // Ne pas reverifier au prochain preconditionnement...
}

void SSOR::ssor(const Matrice_Bloc_Sym& matrice, DoubleVect& solution)
{
  DoubleTab_parts s_parts(solution);
  ConstIntTab_parts items_parts;
  if (algo_items_communs_) items_parts.initialize(items_a_traiter_);

  const int nb_parts = s_parts.size();
  assert(s_parts.size() == nb_parts);
  assert(matrice.nb_bloc_lignes() == nb_parts);

  // Descente
  int i_part;
  for (i_part = 0; i_part < nb_parts; i_part++)
    {
      const Matrice_Bloc& matrice0 = ref_cast(Matrice_Bloc, matrice.get_bloc(i_part, i_part).valeur());
      const Matrice_Morse_Sym& MB00 = ref_cast(Matrice_Morse_Sym, matrice0.get_bloc(0, 0).valeur());
      DoubleVect& partie = s_parts[i_part];

      if (algo_items_communs_) descente_diag_ok_assert(omega_, MB00.get_tab1(), MB00.get_tab2(), MB00.get_coeff(), partie, items_parts[i_part]);
      else descente_diag_ok_assert(omega_, MB00.get_tab1(), MB00.get_tab2(), MB00.get_coeff(), partie);

      // blocs extra-diagonaux
      for (int j_part = i_part + 1; j_part < nb_parts; j_part++)
        {
          const Matrice_Bloc& Aij = ref_cast(Matrice_Bloc, matrice.get_bloc(i_part, j_part).valeur());
          const Matrice_Morse& MB00bis = ref_cast(Matrice_Morse, Aij.get_bloc(0, 0).valeur());
          DoubleVect& partie_j = s_parts[j_part];
          // Attention: le test sur les items communs concerne les lignes de la matrice, pas les colonnes (on passe items_parts[i_part], pas j_part)
          // (note BM: je crois que la version precedente etait buggee mais ca ne s'est pas vu parce que la matrice elem-elem arrive en premier et qu'il n'y a pas d'items communs sur les elements)
          if (algo_items_communs_) descente_bloc_extradiag_assert(MB00bis.get_tab1(), MB00bis.get_tab2(), MB00bis.get_coeff(), partie, partie_j, items_parts[i_part]);
          else descente_bloc_extradiag_assert(MB00bis.get_tab1(), MB00bis.get_tab2(), MB00bis.get_coeff(), partie, partie_j);
        }
    }
  // Traitement de la diagonale
  for (i_part = 0; i_part < nb_parts; i_part++)
    {
      const Matrice_Bloc& matrice0 = ref_cast(Matrice_Bloc, matrice.get_bloc(i_part, i_part).valeur());
      const Matrice_Morse_Sym& MB00 = ref_cast(Matrice_Morse_Sym, matrice0.get_bloc(0, 0).valeur());
      DoubleVect& partie = s_parts[i_part];
      traite_diagonale(omega_, MB00.get_tab1(), MB00.get_tab2(), MB00.get_coeff(), partie);
    }

  // Remontee
  for (i_part = nb_parts - 1; i_part >= 0; i_part--)
    {
      const Matrice_Bloc& matrice0 = ref_cast(Matrice_Bloc, matrice.get_bloc(i_part, i_part).valeur());
      const Matrice_Morse_Sym& MB00bis = ref_cast(Matrice_Morse_Sym, matrice0.get_bloc(0, 0).valeur());
      DoubleVect& partie = s_parts[i_part];
      if (algo_items_communs_) remontee_diag_ok_assert(omega_, MB00bis.get_tab1(), MB00bis.get_tab2(), MB00bis.get_coeff(), partie, items_parts[i_part]);
      else remontee_diag_ok_assert(omega_, MB00bis.get_tab1(), MB00bis.get_tab2(), MB00bis.get_coeff(), partie);

      // Blocs extra-diagonaux (parcours horizontal au lieu de vertical)
      for (int j_part = 0; j_part < i_part; j_part++)
        {
          const Matrice_Bloc& Aij = ref_cast(Matrice_Bloc, matrice.get_bloc(j_part, i_part).valeur());
          const Matrice_Morse& MB00 = ref_cast(Matrice_Morse, Aij.get_bloc(0, 0).valeur());
          DoubleVect& partie_j = s_parts[j_part];
          // Attention: le test sur les items communs concerne les lignes de la matrice, pas les colonnes (on passe items_parts[i_part], pas j_part)
          if (algo_items_communs_) remontee_bloc_extradiag_assert(MB00.get_tab1(), MB00.get_tab2(), MB00.get_coeff(), partie_j, partie, items_parts[j_part]);
          else remontee_bloc_extradiag_assert(MB00.get_tab1(), MB00.get_tab2(), MB00.get_coeff(), partie_j, partie);
        }
    }
}
