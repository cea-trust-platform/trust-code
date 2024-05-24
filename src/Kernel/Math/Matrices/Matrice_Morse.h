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

#ifndef Matrice_Morse_included
#define Matrice_Morse_included

#include <TRUSTTabs_forward.h>
#include <Matrice_Base.h>
#include <TRUSTLists.h>
#include <TRUSTTab.h>
#include <algorithm>
#include <kokkos++.h>
#include <TRUSTArray_kokkos.tpp>
#include <TRUSTTab_kokkos.tpp>

/*! @brief Classe Matrice_Morse Represente une matrice M (creuse), non necessairement carree
 *
 *     stockee au format Morse.
 *     -----------------------------------------------------------------------
 *     On utilise 3 tableaux tab1(n+1), tab2(nnz) et coeff_(nnz):
 *     On note Vi = { j differents de i / M(i,j) est non nul }
 *             tab1[i] = rang dans tab2 de la ieme ligne
 *             pour tab1[i] <= j < tab1[i+1],  tab2[j] decrit Vi
 *             et coeff_[j] = M(i,tab2[j])
 *             tab1 et tab2 sont des rangs au sens fortran:
 *                   1 <= tab2[i] <= n
 *                   tab1[n+1] = nnz+1
 *     Remarque: dans ce commentaire le [] est a prendre au sens fortran:
 *                     tab1[1] designe la premiere valeur de tab1
 *
 *     C'est aussi le format decrit dans la page Wikipedia :
 *         https://fr.wikipedia.org/wiki/Matrice_creuse
 *     en faisant un +1 sur tous les elements des tableaux d'indices (IA et JA dans la page)
 *     -----------------------------------------------------------------------
 *
 * @sa Matrice_Base Matrice_Morse_Sym
 */
class Matrice_Morse : public Matrice_Base
{

  Declare_instanciable_sans_constructeur(Matrice_Morse);

public :

  //constructeurs :

  // par defaut le scalaire 0:
  Matrice_Morse() ;
  Matrice_Morse(int n,int nnz) ;

  // Une matrice a n lignes et m colonnes a nnz coefficients non nuls :
  Matrice_Morse(int n, int m, int nnz) ;

  // copie :
  Matrice_Morse(const Matrice_Morse& ) ;
  Matrice_Morse(int , int , const IntLists& ,const DoubleLists& ,const DoubleVect& );

  Sortie& imprimer(Sortie& s) const override;
  Sortie& imprimer_formatte(Sortie& s) const override;
  Sortie& imprimer_formatte(Sortie& s, int symetrie) const;
  Sortie& imprimer_image(Sortie& s) const;
  Sortie& imprimer_image(Sortie& s, int symetrie) const;
  void WriteFileMTX(const Nom&) const;
  int largeur_de_bande() const;         // Retourne la largeur de bande
  void remplir(const IntLists& ,const DoubleLists& ,const DoubleVect& );
  void remplir(const IntLists& ,const DoubleLists&);
  void remplir(const int, const int, const int, const int, const Matrice_Morse& ) ;
  //dimensionner
  void dimensionner(int n, int nnz);
  void dimensionner(int n, int m, int nnz);

  // place pour d'eventuels nouveaux coefficients non nuls
  // (modif MT)
  void dimensionner(const IntTab&);

  // ordre retourne n si n==m
  int ordre() const override;

  int nb_lignes() const override { return tab1_.size()-1; } // nb_lignes retourne n
  int nb_colonnes() const override { return m_; } // nb_colonnes retourne m
  int nb_coeff() const { return coeff_.size(); } // nb_coeff retourne nnz

  void set_nb_columns( const int );
  void set_symmetric( const int );
  int get_symmetric( ) const { return symetrique_; }

  IntVect& get_set_tab1()
  {
    is_stencil_up_to_date_ = false ;
    return tab1_ ;
  }
  IntVect& get_set_tab2()
  {
    is_stencil_up_to_date_ = false ;
    return tab2_ ;
  }
  DoubleVect& get_set_coeff() { return coeff_ ; }

  const IntVect& get_tab1() const { return tab1_ ; }
  const IntVect& get_tab2() const { return tab2_ ; }
  const DoubleVect& get_coeff() const { return coeff_ ; }

  int nb_vois(int i) const
  {
    return get_tab1()(i+1)-get_tab1()(i); // nb_vois(i) : nombre d'elements non nuls de la ligne i
  }

  //methode pour nettoyer la matrice.
  void clean() override;

  // operateurs :
  // 0<=i,j<=n-1
  inline double& operator()(int i, int j);
  inline double operator()(int i, int j) const;
  // Ne pas supprimer ces deux methodes coef(i,j) qui bien qu'elles fassent la meme chose que les
  // deux precedents sont utilisees tres souvent par OVAP:
  // Access to coefficients do not modify the stencil so we can leave these two access functions
  inline double coef(int i, int j) const { return operator()(i,j); }
  inline double& coef(int i,int j) { return operator()(i,j); }

  Matrice_Morse& operator=(const Matrice_Morse& );
  friend Matrice_Morse operator +(const Matrice_Morse&, const Matrice_Morse& );
  Matrice_Morse& operator +=(const Matrice_Morse& );
  Matrice_Morse& operator *=(double );
  void scale( const double x ) override;

  void get_stencil( IntTab& stencil ) const override;

  void get_stencil_and_coefficients(IntTab& stencil, ArrOfDouble& coefficients) const override;
  void get_stencil_and_coeff_ptrs(IntTab& stencil, std::vector<const double *>& coeff_ptr) const override;

  Matrice_Morse& operator /=(double );
  Matrice_Morse& operator *=(const DoubleVect& );
  friend Matrice_Morse operator *(double, const Matrice_Morse& );
  Matrice_Morse operator -() const;
  virtual int inverse(const DoubleVect&, DoubleVect&, double ) const ;
  virtual int inverse(const DoubleVect&, DoubleVect&, double, int ) const ;
  Matrice_Morse& affecte_prod(const Matrice_Morse& A, const Matrice_Morse& B);

  void compacte(int elim_coeff_nul=0);

  // y += Ax
  DoubleVect& ajouter_multvect_(const DoubleVect& ,DoubleVect& ) const override;
  ArrOfDouble& ajouter_multvect_(const ArrOfDouble& ,ArrOfDouble&, ArrOfInt& ) const;

  // Y += AX ou X et Y sont des DoubleTab a 2 dimensions
  DoubleTab& ajouter_multTab_(const DoubleTab& ,DoubleTab& ) const override;

  // y += transposee(A) x
  DoubleVect& ajouter_multvectT_(const DoubleVect& ,DoubleVect& ) const override;
  ArrOfDouble& ajouter_multvectT_(const ArrOfDouble& ,ArrOfDouble&, ArrOfInt& ) const;

  // A= creat_transposee(B)
  virtual Matrice_Morse& transpose(const Matrice_Morse& a);

  // A=x*A (x vecteur diag)
  virtual Matrice_Morse& diagmulmat(const DoubleVect& x);

  //recupere la partie sup de la matrice et la stocke dans celle-ci
  virtual Matrice_Morse& partie_sup(const Matrice_Morse& a);

  // initialisation a la matrice unite
  void unite();

  // extraction d'un sous-bloc
  void construire_sous_bloc(int nl0, int nc0, int nl1, int nc1, Matrice_Morse& result) const;

  void formeC() ;
  void formeF() ;

  bool has_same_morse_matrix_structure(const Matrice_Morse&) const;
  bool check_morse_matrix_structure() const;
  bool check_sorted_morse_matrix_structure() const;
  void assert_check_morse_matrix_structure() const;
  void assert_check_sorted_morse_matrix_structure() const;
  void sort_stencil();
  bool is_sorted_stencil() const;
  bool is_diagonal();

  mutable int morse_matrix_structure_has_changed_; // Flag if matrix structure changes
  mutable int sorted_; //1 si le stencil est classe : obtenu en appellant sort_stencil()

protected :
  IntVect tab1_;
  IntVect tab2_;
  DoubleVect coeff_;

  int m_;          // Number of columns
  int symetrique_; // Pour inliner operator()(i,j) afin d'optimiser

  template<typename _TAB_T_, typename _VALUE_T_>
  inline void get_stencil_coeff_templ( IntTab& stencil, _TAB_T_& coeffs_span) const;

private :
  double zero_;
};

int Matrice_Morse_test();

inline double Matrice_Morse::operator()(int i, int j) const
{
  assert( (symetrique_==0 && que_suis_je()=="Matrice_Morse")
          || (symetrique_==1 && que_suis_je()=="Matrice_Morse_Sym")
          || (symetrique_==2 && que_suis_je()=="Matrice_Morse_Diag") );
  if ((symetrique_==1) && ((j-i)<0)) std::swap(i,j);
  int k1=tab1_[i]-1;
  int k2=tab1_[i+1]-1;
  if (sorted_)
    {
      int k = (int) (std::lower_bound(tab2_.addr() + k1, tab2_.addr() + k2, j + 1) - tab2_.addr());
      if (k < k2 && tab2_[k] == j + 1)
        return coeff_[k];
    }
  else
    for (int k=k1; k<k2; k++)
      if (tab2_[k]-1 == j) return(coeff_[k]);
  // Si coefficient non trouve c'est qu'il est nul:
  return(0);
}

inline double& Matrice_Morse::operator()(int i, int j)
{
  assert( (symetrique_==0 && que_suis_je()=="Matrice_Morse")
          || (symetrique_==1 && que_suis_je()=="Matrice_Morse_Sym")
          || (symetrique_==2 && que_suis_je()=="Matrice_Morse_Diag") );
  //if (symetrique_==1 && j<i) std::swap(i,j); // Do not use, possible error during compile: "signed overflow does not occur when assuming that (X + c) < X is always false"
  if ((symetrique_==1) && ((j-i)<0)) std::swap(i,j);
  int k1=tab1_[i]-1;
  int k2=tab1_[i+1]-1;
  if (sorted_)
    {
      int k = (int) (std::lower_bound(tab2_.addr() + k1, tab2_.addr() + k2, j + 1) - tab2_.addr());
      if (k < k2 && tab2_[k] == j + 1)
        return coeff_[k];
    }
  else
    for (int k=k1; k<k2; k++)
      if (tab2_[k]-1 == j) return(coeff_[k]);
  if (symetrique_==2) return zero_; // Pour Matrice_Morse_Diag, on ne verifie pas si la case est definie et l'on renvoie 0
#ifndef NDEBUG
  // Uniquement en debug afin de permettre l'inline en optimise
  Cerr << "i or j are not suitable " << finl;
  Cerr << "i=" << i << finl;
  Cerr << "j=" << j << finl;
  Cerr << "n_lignes=" << nb_lignes() << finl;
  Cerr << "n_colonnes=" << nb_colonnes() << finl;
#endif
  // This message happens when you try to fill a coefficient in a matrix whereas is was not scheduled
  // If it is a symmetric matrix, it -may- be a parallelism default. Check it by running a PETSc solver
  // in debug mode: there is a test to check the parallelism of the symmetric matrix...
  Cerr << "Error Matrice_Morse::operator("<< i << "," << j << ") not defined!" << finl;
  exit();
  return coeff_[0];     // On ne passe jamais ici
}

// Kokkos: First (and quick) implementation of a Matrix view. Future: Kokkos kernels ?
#ifdef KOKKOS
struct Matrice_Morse_View
{
  CIntArrView tab1_v;
  CIntArrView tab2_v;
  mutable DoubleArrView coeff_v;
  int symetrique_ = 0;
  int sorted_ = 0;
  mutable double zero_ = 0;
  void set(Matrice_Morse& matrice)
  {
    tab1_v = matrice.get_tab1().view_ro();
    tab2_v = matrice.get_tab2().view_ro();
    coeff_v = matrice.get_set_coeff().view_rw();
    symetrique_ = matrice.get_symmetric();
    sorted_ = matrice.sorted_;
  }
  KOKKOS_INLINE_FUNCTION
  double& operator()(int i, int j) const
  {
    if ((symetrique_==1) && ((j-i)<0))
      {
        // std::swap(i,j) refused by HIP:  reference to __host__ function 'swap<int>' in __host__ __device__ function
        int k = j;
        j = i;
        i = k;
      }
    int k1=tab1_v(i)-1;
    int k2=tab1_v(i+1)-1;
    // ToDo Kokkos for faster access:
    /*
    if (sorted_)
      {
        int k = (int) (std::lower_bound(tab2_.addr() + k1, tab2_.addr() + k2, j + 1) - tab2_.addr());
        if (k < k2 && tab2_v[k] == j + 1)
          return coeff_v[k];
      }
    else */
    for (int k=k1; k<k2; k++)
      if (tab2_v(k)-1 == j) return(coeff_v(k));
    if (symetrique_==2) return zero_; // Pour Matrice_Morse_Diag, on ne verifie pas si la case est definie et l'on renvoie 0
    printf("Error Matrice_Morse::operator(%d, %d) not defined!\n", (True_int)i, (True_int)j);
#ifdef _OPENMP_TARGET
    Kokkos::abort("Error");
#else
    abort();
#endif
    return zero_;
  }
};
#endif
#endif

