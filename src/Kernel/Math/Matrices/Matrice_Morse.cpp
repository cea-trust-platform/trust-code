/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Matrice_Morse.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/62
//
//////////////////////////////////////////////////////////////////////////////

#include <Matrice_Morse.h>
#include <DoubleLists.h>
#include <IntLists.h>
#include <Sparskit.h>
#include <Matrice_Morse_Sym.h>
#include <Check_espace_virtuel.h>
#include <comm_incl.h>
#include <Noms.h>
#include <ArrOfBit.h>
#include <Array_tools.h>
#include <IntTrav.h>
#include <DoubleTrav.h>
#include <Matrix_tools.h>

Implemente_instanciable_sans_constructeur(Matrice_Morse,"Matrice_Morse",Matrice_Base);

// Description:
//    Ecrit les trois tableaux de la structure de stockage
//    Morse sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie& s
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Matrice_Morse::printOn(Sortie& s) const
{
  s << tab1_;
  s << tab2_;
  s << coeff_;
  s << m_ << finl;
  return s;
}

// Description:
//    NON CODE
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree& s
//    Signification: le flot d'entree
//    Contraintes:
// Exception: NON CODE
// Effets de bord:
// Postcondition:
Entree& Matrice_Morse::readOn(Entree& s)
{
  s >> tab1_;
  s >> tab2_;
  s >> coeff_;
  s >> m_;
  morse_matrix_structure_has_changed_=1;
  return s;
}

Sortie& Matrice_Morse::imprimer(Sortie& s) const
{
  int n=nb_lignes();
  for(int i=0; i<n; i++)
    {
      s <<i << ": " <<finl;
      s << "--------------------------------" << finl;
      for (int k=tab1_(i)-1; k<tab1_(i+1)-1; k++)
        {
          s << "("<<(tab2_(k)-1) << "),(" <<coeff_(k)<< ") "
            << " k= " << k << finl;
        }
      s <<finl;
    }
  return s;
}

Sortie& Matrice_Morse::imprimer_formatte(Sortie& s) const
{
  return imprimer_formatte(s,0);
}

Sortie& Matrice_Morse::imprimer_formatte(Sortie& s, int symetrie) const
{
  int numerotation_fortran=(tab1_.mp_min_vect()==1);
  for (int proc=0; proc<Process::nproc(); proc++)
    {
      if (proc==Process::me())
        {
          s << "Matrix morse on the processor " << proc << " : " << finl;
          int n=nb_lignes();
          Noms tab_imp;
          tab_imp.dimensionner(nb_colonnes());
          for(int i=0; i<n; i++)
            {
              for (int k=0; k<nb_colonnes(); k++)
                tab_imp[k]="  .  ";
              if (i<10)
                s <<i << " :" ;
              else
                s <<i << ":" ;

              if (symetrie)
                {
                  for (int j=0; j<i; j++)
                    {
                      for (int k=tab1_(j)-numerotation_fortran; k<tab1_(j+1)-numerotation_fortran; k++)
                        if (tab2_(k)-numerotation_fortran==i)
                          tab_imp[j] = coeff_(k);
                    }
                  int ligne=tab2_(tab1_(i)-numerotation_fortran)-numerotation_fortran;
                  if (i!=ligne)
                    {
                      Cerr << "Problem detected on this Matrice_Morse_Sym." << finl;
                      Cerr << "The diagonal of the line " << ligne << " must be stored even if it is null." << finl;
                      exit();
                    }
                }
              for (int k=tab1_(i)-numerotation_fortran; k<tab1_(i+1)-numerotation_fortran; k++)
                if (tab2_(k)+!numerotation_fortran==0)
                  Cerr<<"Line " <<i<< " no coefficient "<<k<<finl;
                else
                  {
                    if (coeff_(k)>=0)
                      tab_imp[tab2_(k)-numerotation_fortran]=" ";
                    else
                      tab_imp[tab2_(k)-numerotation_fortran]="";
                    tab_imp[tab2_(k)-numerotation_fortran] += (Nom)coeff_(k);
                  }
              for(int k=0; k<nb_colonnes(); k++)
                s<<tab_imp[k];
              s<<finl;
            }
        }
      Process::barrier();
    }
  return s;
}

Sortie& Matrice_Morse::imprimer_image(Sortie& s) const
{
  return imprimer_image(s,0);
}

Sortie& Matrice_Morse::imprimer_image(Sortie& s, int symetrie) const
{
  int numerotation_fortran=(tab1_.mp_min_vect()==1);
  for (int proc=0; proc<Process::nproc(); proc++)
    {
      if (proc==Process::me())
        {
          s << "Matrix morse on the processor " << proc << " : " << finl;
          int n=nb_lignes();
          Noms tab_imp;
          tab_imp.dimensionner(nb_colonnes());
          for(int i=0; i<n; i++)
            {
              for (int k=0; k<nb_colonnes(); k++)
                tab_imp[k]="\u2588\u2588";
              if (i<10)
                s <<i << " :" ;
              else
                s <<i << ":" ;

              if (symetrie)
                {
                  for (int j=0; j<i; j++)
                    {
                      for (int k=tab1_(j)-numerotation_fortran; k<tab1_(j+1)-numerotation_fortran; k++)
                        if (tab2_(k)-numerotation_fortran==i)
                          tab_imp[j] = (coeff_(k) >= 0) ? "  " : "\u2592\u2592";
                    }
                  int ligne=tab2_(tab1_(i)-numerotation_fortran)-numerotation_fortran;
                  if (i!=ligne)
                    {
                      Cerr << "Problem detected on this Matrice_Morse_Sym." << finl;
                      Cerr << "The diagonal of the line " << ligne << " must be stored even if it is null." << finl;
                      exit();
                    }
                }
              for (int k=tab1_(i)-numerotation_fortran; k<tab1_(i+1)-numerotation_fortran; k++)
                if (tab2_(k)+!numerotation_fortran==0)
                  Cerr<<"Line " <<i<< " no coefficient "<<k<<finl;
                else
                  tab_imp[tab2_(k)-numerotation_fortran] = (coeff_(k) >= 0) ? "  " : "\u2592\u2592";

              for(int k=0; k<nb_colonnes(); k++)
                s<<tab_imp[k];
              s<<finl;
            }
        }
      Process::barrier();
    }
  return s;
}


// Description:
//    Constructeur par copie d'une Matrice_Morse.
//    Copie de chaque membre donne du paramtre.
// Precondition:
// Parametre: Matrice_Morse& acopier
//    Signification: la matrice morse a copier
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse::Matrice_Morse(const Matrice_Morse& acopier) :Matrice_Base(),
  tab1_(acopier.tab1_),
  tab2_(acopier.tab2_),
  coeff_(acopier.coeff_),
  m_(acopier.m_),
  symetrique_(0),
  zero_(0)
{
  morse_matrix_structure_has_changed_=1;
  is_stencil_up_to_date_ = acopier.is_stencil_up_to_date_ ;
}
// Description:
//    Constructeur d'une matrice Morse carree d'ordre n
//    et pouvant stocker au maximum nnz elements non nuls.
//    Egalement constructeur par defaut car les 2 parametres
//    ont une valeur par defaut.
// Precondition:
// Parametre: int n
//    Signification: l'ordre de la matrice carree a construire
//    Valeurs par defaut: 1
//    Contraintes:
//    Acces:
// Parametre: int nnz
//    Signification: le nombre d'elements non nuls que pourra
//                   stocker la matrice.
//    Valeurs par defaut: 1
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse::Matrice_Morse(int n, int nnz) :
  morse_matrix_structure_has_changed_(1), symetrique_(0), zero_(0)
{
  dimensionner(n,nnz);
  is_stencil_up_to_date_ = false ;
}

Matrice_Morse::Matrice_Morse()
{
  dimensionner(0,0);
  morse_matrix_structure_has_changed_=1;
  symetrique_ = 0;
  zero_ = 0;
  is_stencil_up_to_date_ = false ;
}


// Description:
//    Constructeur d'une matrice Morse avec n lignes et m colonnes
//    pouvant stocker au maximum nnz elements non nuls.
// Precondition:
// Parametre: int n
//    Signification: le nombre de ligne de la matrice
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int m
//    Signification: le nombre de colonne de la matrice
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int nnz
//    Signification: le nombre d'elements non nuls que pourra
//                   stocker la matrice.
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse::Matrice_Morse(int n, int m, int nnz):
  morse_matrix_structure_has_changed_(1), symetrique_(0), zero_(0)
{
  dimensionner(n,m,nnz);
  is_stencil_up_to_date_ = false ;
}


Matrice_Morse::Matrice_Morse(int n, int nnz, const IntLists& voisins,
                             const DoubleLists& valeurs,
                             const DoubleVect& terme_diag)
  :  morse_matrix_structure_has_changed_(1), symetrique_(0) , zero_(0)
{
  dimensionner(n,n,nnz);
  remplir(voisins, valeurs, terme_diag);
  is_stencil_up_to_date_ = false;
}

void Matrice_Morse::set_nb_columns( const int nb_col )
{
  m_ = nb_col;
}

void Matrice_Morse::set_symmetric( const int symmetric )
{
  symetrique_ = symmetric ;
}

// Description:
// Size the matrix with n lines and n columns and nnz zero-values coefficients
void Matrice_Morse::dimensionner(int n, int nnz)
{
  dimensionner(n,n,nnz);
  return ;
}


// Description
//   Redimensionne la matrice creuse en ajoutant eventuellement
//   des coefficients non nuls
//
//   Parametre : const IntTab &Ind
//      Signification : tableau de taille nc * 2
//                      ou nc est le nombre de couples (i,j)
//                      pour les indices des nouveaux coefficients
//
//   Postcondition:
//      A la sortie, les coefficients non nuls sur une ligne
//      ne sont plus necessairement trie par ordre croissant
//      d'indice de colonne (a faire ?)
// (modif MT)
//
void Matrice_Morse::dimensionner(const IntTab& Ind)
{
  if (Ind.size()==0) return; // On ne fait rien si la structure est vide
  int n_ancien = nb_lignes(), m_ancien = nb_colonnes();

  assert(Ind.nb_dim() == 2);
  assert(Ind.dimension(1) == 2);

  // Calcul du nouveau nombre de lignes
  //   = max (ancien, indices de ligne des nouveaux coeffs)
  //
  // et du nouveau nombre de colonnes
  //   = max (ancien, indices de colonne des nouveaux coeffs)

  int nInd = Ind.dimension(0);
  int n = 0;
  int m = 0;
  for (int i=0; i<nInd; i++)
    {
      if (n < Ind(i,0)) n = Ind(i,0);
      if (m < Ind(i,1)) m = Ind(i,1);
    }
  n++;
  m++;
  if (n < n_ancien) n = n_ancien;
  if (m < m_ancien) m = m_ancien;

  // Copies des anciens tableaux d'indices

  IntVect tab1_temp(tab1_);

  // Initialisation au nombre de coefficients deja presents a chaque ligne

  tab1_.resize(n+1);
  m_ = m;

  for (int i=1; i<=n_ancien; i++)
    tab1_[i] = tab1_temp[i] - tab1_temp[i-1];
  for (int i=n_ancien+1; i<=n; i++)
    tab1_[i] = 0;

  // Parcourt des indices des nouveaux coeffs pour voir s'ils sont
  // deja presents

  int i_nouveaux = 0;
  for (int i=0; i<nInd; i++)
    {
      int i0 = Ind(i,0);
      int i1 = Ind(i,1) + 1;

      int test_present = 0;

      if (i0 < n_ancien)
        {
          int kmin = tab1_temp[i0]-1;
          int kmax = tab1_temp[i0+1]-1;
          for (int k=kmin; k<kmax; k++)
            if (tab2_[k] == i1)
              {
                test_present = 1;
                break;
              }
        }
      if (!test_present)
        {
          i_nouveaux++;
          tab1_[i0+1] += 1;
        }
    }
  if (i_nouveaux == 0)
    {
      tab1_=tab1_temp;
      return;
    }

  // Nouveau tableau des positions des premiers coeffs de chaque ligne
  tab1_[0] = 1;
  for (int i=1; i<=n; i++)
    tab1_[i] += tab1_[i-1];

  int nnz_ancien = tab2_.size();
  int nnz = nnz_ancien + i_nouveaux;

  IntVect tab2_temp(tab2_);
  DoubleVect coeff_temp(coeff_);
  tab2_.resize(nnz);
  coeff_.resize(nnz);

  // Recopie des anciens coefficients et de leurs indices
  // de colonne dans les nouveaux tableaux

  tab2_ = -1;
  for (int i=0; i<n_ancien; i++)
    {
      for (int j1 = tab1_temp[i]-1, j2 = tab1_[i]-1;
           j1 < tab1_temp[i+1]-1;
           j1++, j2++)
        {
          tab2_[j2] = tab2_temp[j1];
          coeff_[j2] = coeff_temp[j1];
        }
    }

  for (int i=0; i<nInd; i++)
    {
      int j0 = Ind(i,0);
      int j1 = Ind(i,1) + 1;
      int k;
      for (k=tab1_[j0]-1; tab2_[k] >= 0; k++)
        if (tab2_[k] == j1)
          {
            break;
          }
      if (tab2_[k] < 0)
        {
          tab2_[k] = j1;
          coeff_[k] = 0.0;
        }
    }
  // on remet les coeffs dans l'ordre... pas optimal mais pour voir..
  coeff_=0;
  //
  {
    int nbis=nb_lignes();
    for(int i=0; i<nbis; i++)
      {
        for (int k=tab1_(i)-1; k<tab1_(i+1)-1; k++)
          {
            for (int  k2=k; k2<tab1_(i+1)-1; k2++)
              {
                int j1=tab2_(k);
                int j2=tab2_(k2);
                if (j1>j2)
                  {
                    tab2_(k)=j2;
                    tab2_(k2)=j1;
                  }
              }
          }
      }
  }
  morse_matrix_structure_has_changed_=1;
}

// Description:
// Size the matrix with n lines, m columns with nnz zero-values coefficients
void Matrice_Morse::dimensionner(int n, int m, int nnz)
{
  tab2_.resize(nnz);
  coeff_.resize(nnz);
  m_=m;

  // on regarde si tab1 a la bonne taille et si tab1[n1]==nnz.
  if ( tab1_.size_array()!=(n+1) || (tab1_[n]-1)!=nnz )
    {
      tab1_.resize(n+1);
      tab1_=1;
    }
  tab1_.resize(n+1);
  tab1_[n]=nnz+1;

  morse_matrix_structure_has_changed_=1;
}

// Description
//   Initialisation a la matrice unite
// (modif MT)
void Matrice_Morse::unite()
{
  coeff_ = 0.0;
  int i,n  = ordre();
  for (i=0; i<n; i++)
    operator()(i,i) = 1.0;
}

// Description:
//    Renvoie l'ordre de la matrice:
//     - le nombre de lignes si la matrice est carree
//     - 0 sinon
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: l'ordre de la matrice
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Matrice_Morse::ordre() const
{
  if(nb_lignes()==nb_colonnes())
    return nb_lignes();
  else
    return 0;
}

// Description:
// Method to check/clean the Matrice_Morse matrix:
// -Suppress coefficient defined several times
// -elim_coeff_nul=0, on ne supprime pas les coefficients nuls de la matrice
// -elim_coeff_nul=1, on supprime les coefficients nuls de la matrice
// -elim_coeff_nul=2, on supprime les coefficients nuls et quasi-nuls de la matrice
void Matrice_Morse::compacte(int elim_coeff_nul)
{
  int n=nb_lignes();

  int coeff_nuls=0;
  int coeff_quasi_nuls=0;
  IntTrav elim_coeff(tab2_);
  if (elim_coeff_nul)
    {
      DoubleTrav coeff_max(n);
      // Recherche des coefficients nuls hors diagonale a supprimer de la matrice morse
      for(int i=0; i<n; i++)
        for (int k=tab1_(i)-1; k<tab1_(i+1)-1; k++)
          {
            if (dabs(coeff_(k))>coeff_max(i)) coeff_max(i)=dabs(coeff_(k));
            if (coeff_(k)==0)
              {
                coeff_nuls++;
                elim_coeff(k)=1;
              }
          }

      if (elim_coeff_nul==2)
        {
          // Recherche des coefficients quasi nuls hors diagonale (1.e-12 plus petit que le coefficient le plus grand de la ligne) a supprimer de la matrice morse
          for(int i=0; i<n; i++)
            if (!est_egal(coeff_max(i),0)         // Le plus grand coefficient doit etre strictement positif
                && coeff_max(i)<1e10)          // On ne supprime pas un coefficient quasi-nul d'une ligne ou la diagonale peut etre mise a 1e12
              for (int k=tab1_(i)-1; k<tab1_(i+1)-1; k++)
                if (coeff_(k)!=0                 // Les coefficients nuls ont deja ete trouves
                    && est_egal(dabs(coeff_(k))/coeff_max(i),0))
                  {
                    coeff_quasi_nuls++;
                    elim_coeff(k)=1;
                  }
        }
    }
  // Recherche des coefficients doublons
  int doublons=0;
  for(int i=0; i<n; i++)
    {
      int kmin=tab1_(i)-1;
      int kmax=tab1_(i+1)-1;
      int jmax=-1; // Highest column of a coefficient in the line i
      for (int k=kmin; k<kmax; k++)
        {
          int j = tab2_(k)-1;
          if (j>jmax)
            jmax=j;
          else
            {
              // Found a column j lower than jmax, check if not defined before:
              for (int kk=k-1; kk>=kmin; kk--)
                {
                  int jj = tab2_(kk)-1;
                  if (jj == j)
                    {
                      // Already defined!
                      doublons=1;
                      elim_coeff(k)=1;
                      // Check if same coefficients:
                      if (coeff_(kk)!=coeff_(k))
                        {
                          Cerr << "Error in a Matrix Morse:" << finl;
                          Cerr << "A("<<i<<","<<j<<")="<<coeff_(k)<<" != "<<"A("<<i<<","<<jj<<")="<<coeff_(kk)<<finl;
                          exit();
                        }
                      break;
                    }
                }
            }
        }
    }

  int nnz=0;
  int kdeb = tab1_(0)-1;
  int coefficient_suppressed=0; // Nombre de coefficients supprimes
  if (doublons || coeff_nuls || coeff_quasi_nuls)
    {
      // Suppress coefficients
      for (int i=0; i<n; i++)
        {
          int kfin = tab1_(i+1)-1;
          for (int k=kdeb; k<kfin; k++)
            {
              if (!elim_coeff(k))
                {
                  coeff_(nnz) = coeff_(k);
                  tab2_(nnz) = tab2_(k);
                  nnz++;
                }
              else
                coefficient_suppressed++;
            }
          // Modification de tab1_ en fonction du nombre de coefficients deja enleves
          tab1_(i+1)-=coefficient_suppressed;
          kdeb = kfin;
        }
    }
  else
    {
      nnz = tab1_[n] - 1;
    }


  // On redimensionne les tableaux
  tab2_.resize(nnz);
  coeff_.resize(nnz);

  morse_matrix_structure_has_changed_=1;
  assert_check_morse_matrix_structure( );
}

// Description:
//    Operateur d'affectation d'une Matrice_Morse dans une
//    autre Matrice_Morse.
// Precondition:
// Parametre: Matrice_Morse& a
//    Signification: la partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse& Matrice_Morse::operator=(const Matrice_Morse& a )
{
  tab1_.reset(), tab1_.copy(a.get_tab1());
  tab2_.reset(), tab2_.copy(a.get_tab2());
  coeff_.reset(), coeff_.copy(a.get_coeff());
  m_=a.nb_colonnes();
  morse_matrix_structure_has_changed_=1;
  is_stencil_up_to_date_=a.is_stencil_up_to_date();
  return(*this);
}

// Description:
//    *this = a transposee.
// Precondition:
// Parametre: Matrice_Morse& a
//    Signification: la matrice a transposee
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse& Matrice_Morse::transpose(const Matrice_Morse& a)
{

  int n=a.nb_lignes();
  int jk=nb_lignes();
  int job=1;
  int ipos=1;
  int m=a.nb_colonnes();
  int l=nb_lignes();
  if(m!=jk)
    {
      Cerr << "Matrice_Morse::transpose bad dimensions" << finl;
      exit();
    }
  m=a.nb_lignes();
  l=nb_colonnes();
  if(m!=l)
    {
      Cerr << "Matrice_Morse::transpose bad dimensions" << finl;
      exit();
    }

  {
    int i, j, k , next;

    //  F77NAME(CSRCSC)(&n,&o,&k,&k,a.coeff_,a.tab2_,a.tab1_,coeff_.addr(),tab2_.addr(),tab1_.addr());
    //        subroutine csrcsc (n,jk,job,ipos,a,ja,ia,ao,jao,iao)
    //      integer ia(n+1),iao(n+1),ja(*),jao(*)
    //      real*8  a(*),ao(*)
    //c-----------------------------------------------------------------------
    //c Compressed Sparse Row     to      Compressed Sparse Column
    //c
    //c on entry:
    //c----------
    //c n        = dimension of A.
    //c jk     = dimensoin of Ao
    //c job        = integer to indicate whether to fill the values (job.eq.1) of the
    //c         matrix ao or only the pattern., i.e.,ia, and ja (job .ne.1)
    //c
    //c ipos  = starting position in ao, jao of the transposed matrix.
    //c         the iao array takes this into account (thus iao(1) is set to ipos.)
    //c         Note: this may be useful if one needs to append the data structure
    //c         of the transpose to that of A. In this case use for example
    //c                call csrcsc (n,1,n+2,a,ja,ia,a,ja,ia(n+2))
    //c          for any other normal usage, enter ipos=1.
    //c a        = real array of length nnz (nnz=number of nonzero elements in input
    //c         matrix) containing the nonzero elements.
    //c ja        = integer array of length nnz containing the column positions
    //c           of the corresponding elements in a.
    //c ia        = integer of size n+1. ia(k) contains the position in a, ja of
    //c          the beginning of the k-th row.
    //c
    //c on return:
    //c ----------
    //c output arguments:
    //c ao        = real array of size nzz containing the "a" part of the transpose
    //c jao        = integer array of size nnz containing the column indices.
    //c iao        = integer array of size n+1 containing the "ia" index array of
    //c          the transpose.
    //c
    //c-----------------------------------------------------------------------
    //c----------------- compute lengths of rows of transp(A) ----------------
    //      do 1 i=1,jk+1
    //         iao(i) = 0
    // 1    continue

    for (i=0; i<=jk; i++ ) tab1_[i] = 0 ;

    //      do 3 i=1, n
    //         do 2 k=ia(i), ia(i+1)-1
    //            j = ja(k)+1
    //            iao(j) = iao(j)+1
    // 2       continue
    // 3    continue

    for(i=0; i<n; i++)
      {
        for(k=a.tab1_[i]-1; k<a.tab1_[i+1]-1; k++)
          {
            j = a.tab2_[k] ;
            tab1_[j] = tab1_[j] +1 ;
          }
      }

    //c---------- compute pointers from lengths ------------------------------
    //      iao(1) = ipos
    //      do 4 i=1,jk
    //         iao(i+1) = iao(i) + iao(i+1)
    // 4    continue

    tab1_[0] = ipos ;
    for(i=0; i<jk; i++) tab1_[i+1] = tab1_[i] + tab1_[i+1] ;

    //c--------------- now do the actual copying -----------------------------
    //      do 6 i=1,n
    //         do 62 k=ia(i),ia(i+1)-1
    //            j = ja(k)
    //            next = iao(j)
    //            if (job .eq. 1)  ao(next) = a(k)
    //            jao(next) = i
    //            iao(j) = next+1
    // 62      continue
    // 6    continue

    for(i=0; i<n; i++)
      {
        for(k=a.tab1_[i]-1; k<a.tab1_[i+1]-1; k++)
          {
            j    = a.tab2_[k]-1 ;
            next = tab1_[j] ;
            if (job == 1)  coeff_[next-1] = a.coeff_[k] ;
            tab2_[next-1] = i+1 ;
            tab1_[j]    = next+1 ;
          }
      }

    //c-------------------------- reshift iao and leave ----------------------
    //      do 7 i=jk,1,-1
    //         iao(i+1) = iao(i)
    // 7    continue
    //      iao(1) = ipos

    for(i=jk-1; i>=0; i--)  tab1_[i+1] = tab1_[i] ;
    tab1_[0] = ipos ;

    //c--------------- end of csrcsc -----------------------------------------
    //c-----------------------------------------------------------------------
    //      end
    //c-----------------------------------------------------------------------
  }
  morse_matrix_structure_has_changed_=1;
  return(*this);
}


//A=x*A avec x une matrice diagonale stockee dans un vecteur
//la meme methode peut etre utilisee pour stocke le resultat dans
//un autre matrice que la matrice initiale
Matrice_Morse& Matrice_Morse::diagmulmat(const DoubleVect& x)
{
  int m=nb_lignes();
  int l=0;
  int n=x.size_array();
  if(n!=m)
    {
      Cerr << "Matrice_Morse::diagmulmat bad dimensions" << finl;
      exit();
    }
#ifndef CRAY
  F77NAME(DIAMUA)(&m ,&l,
                  coeff_.addr(),tab2_.addr(),tab1_.addr(),x.addr(),
                  coeff_.addr(),tab2_.addr(),tab1_.addr());
#else
  Cerr << "Matrice_Morse::diagmulmat DIAMUA call invalid for CRAY"<<finl;
  exit();
#endif
  return(*this);
}

//extraction de la partie superieure d'une matrice morse
//la matrice resultat est celle appelante
Matrice_Morse& Matrice_Morse::partie_sup(const Matrice_Morse& a)
{
  int m=nb_lignes();
  int n=a.nb_lignes();
  if(m!=n)
    {
      Cerr << "Matrice_Morse::partie_sup : bad dimensions m!=n." << finl;
      exit();
    }
  double t ;
  int ko, k, i, kdiag, kfirst ;
  ko = -1 ;
  for( i=0; i< n; i++)
    {
      kfirst = ko + 1 ;
      kdiag = -1 ;
      for( k = a.tab1_[i]-1; k< a.tab1_[i+1]-1; k++)
        {
          if (a.tab2_[k]-1 >= i)
            {
              ko++ ;
              coeff_[ko] = a.coeff_[k] ;
              tab2_[ko] = a.tab2_[k] ;
              if (a.tab2_[k] == i) kdiag = ko ;
            }
        }
      if (kdiag != -1 && kdiag != kfirst)
        {
          t = coeff_[kdiag] ;
          coeff_[kdiag] = coeff_[kfirst] ;
          coeff_[kfirst] = t ;
          k = tab2_[kdiag] ;
          tab2_[kdiag] = tab2_[kfirst] ;
          tab2_[kfirst] = k ;
        }
      tab1_[i] = kfirst+1 ;
    }
  int nnz = (ko + 1) ;

  tab1_[n] = (nnz) + 1 ;
  tab2_.resize( nnz );
  coeff_.resize( nnz );
  morse_matrix_structure_has_changed_=1;
  return(*this);
}

// Description:
//    Operation de multiplication-accumulation (saxpy) matrice vecteur.
//    Operation: resu = resu + A*x
DoubleVect& Matrice_Morse::ajouter_multvect_(const DoubleVect& x,DoubleVect& resu) const
{
  assert_check_morse_matrix_structure( );
  const int n = tab1_.size_array() - 1;

  assert(x.size_array() == nb_colonnes());
  // Test dans cet ordre car l'attribut size() peut etre invalide:
  assert(resu.size_array() == n || resu.size() == n);

  const int *tab1_ptr = tab1_.addr() + 1;
  const int *tab2_ptr = tab2_.addr();
  const double *coeff_ptr = coeff_.addr();
  const double * x_fortran = x.addr() - 1; // Pour indexer x avec un indice fortran
  int k_fortran = 1; // indice fortran dans tab2 et coeff
  for (int i = 0; i < n; i++, tab1_ptr++)
    {
      const int kmax = *tab1_ptr; // tab1_[i+1] = indice fortran dans tab2_
      assert(kmax >= k_fortran && kmax <= (tab2_.size_array()+1));
      double t = resu[i];
      assert(k_fortran == tab1_[i] && tab2_ptr == tab2_.addr()+(k_fortran-1));
      for (; k_fortran < kmax; k_fortran++, tab2_ptr++, coeff_ptr++)
        {
          int colonne = *tab2_ptr; // indice fortran
          assert(colonne >= 1 && colonne <= nb_colonnes());
          t += (*coeff_ptr) * x_fortran[colonne];
        }
      resu[i] = t;
    }
  return resu;
}

// Multiplication de la matrice par un vecteur x en prenant uniquement les items reels non communs pour x
ArrOfDouble& Matrice_Morse::ajouter_multvect_(const ArrOfDouble& x,ArrOfDouble& resu,ArrOfInt& est_reel_pas_com) const
{
  assert_check_morse_matrix_structure( );
  int n = nb_lignes();

  assert(nb_colonnes()==x.size_array());
  assert(n==resu.size_array());
  for(int i=0; i<n; i++)
    {
      double t = 0.0;
      for (int k=tab1_(i)-1; k<tab1_(i+1)-1; k++)
        {
          int j=tab2_(k)-1;
          if (est_reel_pas_com(j)) t += coeff_(k)*x(j);
        }
      resu(i) += t ;
    }
  return resu;
}

// Description:
//    Operation de multiplication-accumulation (saxpy) matrice matrice
//    (matrice X representee par un tableau)
//    Operation: RESU = RESU + A*X
// Precondition:
// Parametre: DoubleTab& x
//    Signification: la matrice a multiplier
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& resu
//    Signification: la matrice resultat de l'operation
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: la matrice resultat de l'operation
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Matrice_Morse::ajouter_multTab_(const DoubleTab& x,DoubleTab& resu) const
{

  if ( (x.nb_dim() == 1) && (resu.nb_dim() == 1))
    {
      ajouter_multvect(x,resu);
      return resu;
    }

  assert_check_morse_matrix_structure( );
  int i, k;
  int nb_comp = x.dimension(1);

  assert(resu.dimension(1) == nb_comp);
  double* t= new double[nb_comp];
  int ncomp;
  int n=nb_lignes();
  for(i=0; i<n; i++)
    {
      for (ncomp=0; ncomp<nb_comp; ncomp++)
        t[ncomp] = 0.0;
      for (k=tab1_(i)-1; k<tab1_(i+1)-1; k++)
        for (ncomp=0; ncomp<nb_comp; ncomp++)
          t[ncomp] += coeff_(k)*x(tab2_(k)-1,ncomp);
      for (ncomp=0; ncomp<nb_comp; ncomp++)
        resu(i,ncomp) += t[ncomp] ;
    }
  delete [] t;
  return resu;
}


// Description:
//    Operation de multiplication-accumulation (saxpy) matrice vecteur,
//    par la matrice transposee.
//    Operation: resu = resu + A^{T}*x
// Precondition:
// Parametre: DoubleVect& x
//    Signification: le vecteur a multiplier
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& resu
//    Signification: le vecteur resultat de l'operation
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleVect&
//    Signification: le vecteur resultat de l'operation
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleVect& Matrice_Morse::ajouter_multvectT_(const DoubleVect& x,DoubleVect& resu) const
{
  assert_check_morse_matrix_structure( );

  int n=nb_lignes();
  for(int i=0; i<n; i++)
    {
      double xi = x(i);
      for (int k=tab1_(i)-1; k<tab1_(i+1)-1; k++)
        resu(tab2_(k)-1) += coeff_(k) * xi;
    }
  return resu;
}

// Multiplication de la tranposee de la matrice par un vecteur x en prenant uniquement les items reels non communs
ArrOfDouble& Matrice_Morse::ajouter_multvectT_(const ArrOfDouble& x,ArrOfDouble& resu,ArrOfInt& est_reel_pas_com) const
{
  assert_check_morse_matrix_structure( );
  int n=nb_lignes();

  assert(n==x.size_array());
  assert(nb_colonnes()==resu.size_array());
  for(int i=0; i<n; i++)
    {
      if (est_reel_pas_com(i))
        {
          double xi = x(i);
          for (int k=tab1_(i)-1; k<tab1_(i+1)-1; k++)
            resu(tab2_(k)-1) += coeff_(k) * xi;
        }
    }
  return resu;
}

// Description:
//    Fonction (hors classe) amie de la classe Matrice_Morse
//    Addition de 2 matrices au format Morse.
//    Operation: renvoie (A+B)
// Precondition:
// Parametre: Matrice_Morse& A
//    Signification: une matrice au format Morse
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: Matrice_Morse& B
//    Signification: une matrice au format Morse
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Matrice_Morse
//    Signification: le resultat de l'operation
//    Contraintes: la matrice resultat est allouee par la fonction
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse operator+(const Matrice_Morse& A , const Matrice_Morse& B )
{
  int nrow=A.nb_lignes();
  int ncol=A.nb_colonnes();
  int job=1;
  int ierr;
  IntVect iw(ncol);
  Matrice_Morse C;
  // PL: avant de dimensionner a nzmax on verifie si A et B n'ont pas la meme structure par hasard...
  // Cela evite un pic memoire provoque par l'addition de matrices dans Equation_base::dimensionner_matrice
  // Sinon, APLB n'est pas optimal, voir une autre implementation genre MKL:
  // https://software.intel.com/en-us/mkl-developer-reference-c-mkl-sparse-add
  int nzmax = A.has_same_morse_matrix_structure(B) ? A.nb_coeff() : A.nb_coeff()+B.nb_coeff();
  C.dimensionner(nrow, ncol, nzmax);
  F77NAME(APLB) (&nrow, &ncol, &job, A.get_coeff().addr(), A.get_tab2().addr(), A.get_tab1().addr(),
                 B.get_coeff().addr(), B.get_tab2().addr(), B.get_tab1().addr(), C.get_set_coeff().addr(),
                 C.get_set_tab2().addr(), C.get_set_tab1().addr(),
                 &nzmax,iw.addr(),&ierr);

  const int nnz = C.tab1_[nrow] - 1;
  C.get_set_tab2().resize( nnz );
  C.get_set_coeff().resize( nnz );
  C.morse_matrix_structure_has_changed_=1;
  return(C);
}

bool Matrice_Morse::has_same_morse_matrix_structure(const Matrice_Morse& A) const
{
  int nrow=A.nb_lignes();
  for (int i=0; i<nrow; i++)
    if (tab1_(i)!=A.tab1_(i))
      return false;
  return true;
}

// Description:
//    Calcule la solution du systeme lineaire: A * solution = secmem.
//    La methode utilisee est GMRES preconditionnee avec ILUT.
//  ATTENTION: cette methode n'a vraisemblablement jamais ete testee en parallele
// Precondition:
// Parametre: DoubleVect& secmem
//    Signification: le second membre du systeme lineaire
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& solution
//    Signification: la solution du systeme
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Parametre: double coeff_seuil
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception: Erreur dans ilut 'matrix may be wrong' dixit SAAD
// Exception: Erreur dans ilut : debordement dans L
// Exception: Erreur dans ilut : debordement dans U
// Exception: Valeur illegale de lfil : sans doute ecrasement memoire
// Exception: Ligne vide rencontree
// Exception: Pivot nul rencontre ! au pas
// Exception: Il s'est passe quelque chose de bizarre : je prefere tout arreter.
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Matrice_Morse::inverse(const DoubleVect& secmem, DoubleVect& solution,
                           double coeff_seuil) const
{
  // B.Mat: au 20/4/2005, DoubleVect::norme() est bugge, donc la methode n'a
  // sans doute jamais ete validee en parallele. Je retire norme() et je mets
  // ceci:
  if (Process::nproc() > 1)
    {
      Cerr << "Matrice_Morse::inverse(const DoubleVect& secmem, DoubleVect& solution, "
           << "coeff_seuil double) const \n has never been tested in parallel" << finl;
      Cerr << "Try 'Solveur Gmres { diag }' or 'Solveur Petsc Gmres { precond diag { } }'" << finl;
      Cerr << "instead of 'Solveur Gmres { }' which is not parallelized yet." << finl;
      exit();
    }

  DoubleVect toto(secmem);
  int prems=1;                        // On ne calcul L et U que si prems =1
  int lf = min(10,ordre()/2);        // nombre de termes ajouter dans L et U
  int nn = ordre();                // ordre de la matrice
  int ima = min(10,ordre()/2);        // degre de l'espace de Krilov
  IntVect ju;                        // tableau pour L et U
  DoubleVect alu;                // tableau pour L et U
  IntVect jlu;                        // tableau pour L et U
  DoubleVect vv;                // tableau de travail pour pgmres
  //  DoubleVect Resini(nn);
  DoubleVect Resini(toto);

  int ie=1;                                // niveau d'erreur
  int n2 = nb_coeff()+(2*lf*nn);        // nombre de termes non nuls dans LU

  double r;
  double coeff_seuilr;
precond :
  {
    if (prems)
      {
        //Cerr << "  ordre de la matrice de pression = " << nn << finl;
        //Cerr << "  nb termes non nuls dans la matrice de pression = "
        //<< coeff_.size() << finl;
        int  iw = n2 + 2;
        ju.resize(nn);
        jlu.resize(iw);
        alu.resize(iw);
        double to = 1.e-10;                // filtrage des coefficents de L U
        DoubleVect w(nn+1);                // tableau de travail pour ilut
        //        DoubleVect wl(nn);                // tableau de travail pour ilut
        IntVect jw(2*nn);                        // tableau de travail pour ilut
        //        IntVect jwu(nn);                        // tableau de travail pour ilut
        //IntVect jwl(nn);                        // tableau de travail pour ilut
        //coeff_[0]*=2.0;                        // pour rendre A non singuliere!

        //Cerr << "  ****  Preconditionnement ILUT de la matrice de pression  ****" << finl;
#ifndef CRAY
        F77NAME(ILUT)(&nn, coeff_.addr(), tab2_.addr(), tab1_.addr(), &lf,
                      &to, alu.addr(), jlu.addr(), ju.addr(),
                      &iw, w.addr(),
                      jw.addr(), &ie);
        //         to=0;
        //         wu=0.0;
        //         wl=0.0;
        //         jr=0;
        //         jwu=0;
        //         jwl=0;
        //         ie=0;
#else
        Cerr << "Matrice_Morse::inverse ILUT call invalid for cray"<<finl;
        exit();
#endif

        //coeff_[0]*=0.5;                        // pour rendre A singuliere!
        //Cerr << "     nb termes non nuls dans LU = " << n2 << finl;
        switch(ie)
          {
          case 0 :
            /*{
              Cerr << "     printing of the L and U matrixes : " << finl;
              Nom nom_fich = "matrices_L_U.precond";
              ofstream os(nom_fich);
              os << "impression des matrices L et U :\n";
              int* tab2=jlu;
              double* coeff=alu;
              double x;
              os << "termes diagonaux :" << "\n";
              for (int num=0; num<nn; num++) {
              x=coeff[num];
              os << "(" << num+1 << " , " << num+1 << ")  =  " << x << "\n";
              }
              os << "hors diagonale : " << "\n";
              for ( num=0; num<nn; num++) {
              os << "ligne " << num+1 << " : ";
              int nb_vois=tab2[num+1]-tab2[num];
              double x=0.;
              for(int vois=0; vois<nb_vois; vois++) {
              int num_vois=tab2[tab2[num]+vois-1];
              x=coeff[tab2[num]+vois-1];
              os << "(" << num_vois << " , " << x << " ) " ;
              }
              os << "\n";
              }
              os.flush();
              }
            */
            break;
          case -1 :
            Cerr << "Error in ilut 'matrix may be wrong' dixit SAAD" << finl;
            exit();
            break;
          case  -2  :
            Cerr << "Error in ilut : overflow in L " << finl;
            exit();
            break;
          case   -3  :
            Cerr << "Error in ilut : overflow in U " << finl;
            exit();
            break;
          case    -4   :
            Cerr << "Illegal value for lfil : it may be a memory trouble " << finl;
            exit();
            break;
          case    -5  :
            Cerr << "Empty line met " << finl;
            exit();
            break;
          default :
            Cerr << "Pivot empty met ! at step " << ie << finl;;
            exit();
            break;
          }
        prems=0;
      }
  }
  //resol :
  {
    vv.resize(nn*(ima+1));
    // Debog::verifier("Matrice_Morse::inverse solution 0 ",solution);
    assert_espace_virtuel_vect(solution);
    multvect(solution, Resini);
    Resini -= toto;
    {
      r = mp_prodscal(Resini, Resini);
      r = sqrt(r);
    }
    Cout << " Initial residu : " << r << finl;
    if (r==0)
      coeff_seuilr=DMAXFLOAT;
    else
      coeff_seuilr=coeff_seuil/r;
    Resini=toto;
    int maxits = max(10, nn);
    int io = 0;                                // fichier de sortie
#ifndef CRAY
    F77NAME(PGMRES)(&nn, &ima, toto.addr(), solution.addr(), vv.addr(), &coeff_seuilr,
                    &maxits, &io, coeff_.addr(), tab2_.addr(), tab1_.addr(),
                    alu.addr(), jlu.addr(), ju.addr(), &ie);

#else
    Cerr << "Matrice_Morse::inverse PGMRES call invalid for CRAY"<<finl;
    exit();
#endif
    switch(ie)
      {
      case 0:
        Cout << "     ** PGMRES has converged **" << finl;
        break;
      case 1:
        Cout << "     ** No convergence after " << maxits << " iterations **" << finl;
        ref_cast(DoubleVect, toto)=Resini;
        if( lf<50)
          {
            lf +=5;
            Cerr << "  The degree of the preconditioning matrix LU is increased :   " << lf << finl;
            n2 = tab2_.size()+(2*lf*nn);
            prems=1;
            goto precond ;
          }
        /*else{
          if(ima==50)
          exit();
          Cout << "  The degree of the Krylov space for PGMRES is increased :   " << (ima+=2) << finl;
          Cout << "    (=> working array for pgmres at " << (nn*(ima+1)) << " elements.)" << finl;
          goto resol ;
          }*/
        break;
      case -1:
        Cerr << "convergence reached after " << (int)0 << "iterations !! " << finl;
        Cerr << "'stationnary state may be obtened' dixit us " << finl;
        break;
      default :
        Cerr << "Something abnormal has happened : it is preferable to stop." << finl;
        exit();
      }
  }
  return(1);
}

//copie de la fonction precedente avec le parametre max_iteration en plus,
//et si ce nb d'iterations est atteint, return(0), au lieu de recommencer preconditionnement
//utilise pour l'implicite hyperbolique (si recoit 0 : divise le dt par 2 et recommence)
int Matrice_Morse::inverse(const DoubleVect& secmem, DoubleVect& solution,
                           double coeff_seuil, int max_iter) const
{
  // B.Mat: au 20/4/2005, DoubleVect::norme() est bugge, donc la methode n'a
  // sans doute jamais ete validee en parallele. Je retire norme() et je mets
  // ceci:
  if (Process::nproc() > 1)
    {
      Cerr << "Matrice_Morse::inverse(const DoubleVect& secmem, DoubleVect& solution, "
           << "double coeff_seuil) const \n has never been tested in parallel" << finl;
      assert(0);
      exit();
    }


  DoubleVect toto(secmem);
  int prems=1;                        // On ne calcul L et U que si prems =1
  int lf = min(10,ordre()/2);        // nombre de termes ajouter dans L et U
  int nn = ordre();                // ordre de la matrice

  int ima = min(10,ordre()/2);        // degre de l'espace de Krilov
  IntVect ju;                        // tableau pour L et U
  DoubleVect alu;                // tableau pour L et U
  IntVect jlu;                        // tableau pour L et U
  DoubleVect vv;                // tableau de travail pour pgmres
  DoubleVect Resini(nn);

  int ie=1;                                // niveau d'erreur
  int n2 = nb_coeff()+(2*lf*nn);        // nombre de termes non nuls dans LU

  double r;
  double coeff_seuilr;
  //  precond :
  {
    if (prems)
      {
        //Cerr << "  ordre de la matrice de pression = " << nn << finl;
        //Cerr << "  nb termes non nuls dans la matrice de pression = "
        //<< tab2_.size() << finl;
        int  iw = n2 + 2;
        ju.resize(nn);
        jlu.resize(iw);
        alu.resize(iw);
        double to = 1.e-10;                // filtrage des coefficents de L U
        DoubleVect w(nn+1);                // tableau de travail pour ilut

        //        IntVect jr(nn);                        // tableau de travail pour ilut
        IntVect jw(2*nn);                        // tableau de travail pour ilut
        //        IntVect jwl(nn);                        // tableau de travail pour ilut
        //coeff_[0]*=2.0;                        // pour rendre A non singuliere!

        //Cerr << "  ****  Preconditionnement ILUT de la matrice de pression  ****" << finl;
#ifndef CRAY
        F77NAME(ILUT)(&nn, coeff_.addr(), tab2_.addr(), tab1_.addr(), &lf,
                      &to, alu.addr(), jlu.addr(), ju.addr(),
                      &iw, w.addr(),
                      jw.addr(), &ie);
#else
        Cerr << "Matrice_Morse::inverse ILUT call invalid for CRAY"<<finl;
        exit();
#endif

        //coeff_[0]*=0.5;                        // pour rendre A singuliere!
        //Cerr << "     nb termes non nuls dans LU = " << n2 << finl;
        switch(ie)
          {
          case 0 :
            /*{
              Cerr << "     printing of L and U matrixes : " << finl;
              Nom nom_fich = "matrices_L_U.precond";
              ofstream os(nom_fich);
              os << "impression des matrices L et U :\n";
              int* tab2=jlu;
              double* coeff=alu;
              double x;
              os << "termes diagonaux :" << "\n";
              for (int num=0; num<nn; num++) {
              x=coeff[num];
              os << "(" << num+1 << " , " << num+1 << ")  =  " << x << "\n";
              }
              os << "hors diagonale : " << "\n";
              for ( num=0; num<nn; num++) {
              os << "ligne " << num+1 << " : ";
              int nb_vois=tab2[num+1]-tab2[num];
              double x=0.;
              for(int vois=0; vois<nb_vois; vois++) {
              int num_vois=tab2[tab2[num]+vois-1];
              x=coeff[tab2[num]+vois-1];
              os << "(" << num_vois << " , " << x << " ) " ;
              }
              os << "\n";
              }
              os.flush();
              }
            */
            //Cerr << "     matrices L et U :  OK" << finl;
            break;
          case -1 :
            Cerr << "Error in ilut 'matrix may be wrong' dixit SAAD" << finl;
            exit();
            break;
          case  -2  :
            Cerr << "Error in ilut : overflow in L " << finl;
            exit();
            break;
          case   -3  :
            Cerr << "Error in ilut : overflow in U " << finl;
            exit();
            break;
          case    -4   :
            Cerr << "Illegal value for lfil : it may be a memory trouble " << finl;
            exit();
            break;
          case    -5  :
            Cerr << "Empty line met " << finl;
            exit();
            break;
          default :
            Cerr << "Pivot null met ! at step " << ie << finl;;
            exit();
            break;
          }
        prems=0;
      }
  }
  //resol :
  {
    vv.resize(nn*(ima+1));
    multvect(solution, Resini);
    Resini -= toto;
    r = mp_norme_vect(Resini);
    Cout << " Initial residu : " << r << finl;
    coeff_seuilr=coeff_seuil/r;
    Resini=toto;
    int maxits = max(10, max_iter);
    int io = 0;                                // fichier de sortie
#ifndef CRAY
    F77NAME(PGMRES)(&nn, &ima, toto.addr(), solution.addr(), vv.addr(), &coeff_seuilr,
                    &maxits, &io, coeff_.addr(), tab2_.addr(), tab1_.addr(),
                    alu.addr(), jlu.addr(), ju.addr(), &ie);
#else
    Cerr << "Matrice_Morse::inverse PGMRES call invalid for CRAY"<<finl;
    exit();
#endif

    //     multvect(solution, Resini);
    //     Resini -= secmem;
    //     r=Resini.norme();
    //     Cerr << " Residu final : " << r <<" coeff "<<coeff_seuilr*r<<" Its "<<maxits<< finl;
    switch(ie)
      {
      case 0:
        Cout << "     ** PGMRES has converged ** " << finl;
        break;
      case 1:
        Cout << "     ** No convergence after " << maxits << " iterations **" << finl;
        return(0);
        //       (DoubleVect&) toto=Resini;
        //       if( lf<50){
        //         lf +=5;
        //         Cerr << "  J'augmente le degre de la matrice de precondition. LU:   " << lf << finl;
        //         n2 = coeff_.size()+(2*lf*nn);
        //         prems=1;
        //         goto precond ;
        //       }
        /*else{
          if(ima==50)
          exit();
          Cerr << "  The degree of the Krylov space for PGMRES is increased :   " << (ima+=2) << finl;
          Cerr << "    (=> working array for pgmres at " << (nn*(ima+1)) << " elements.)" << finl;
          goto resol ;
          }*/
      case -1:
        Cerr << "Convergence after " << (int)0 << "iterations !! " << finl;
        Cerr << "'stationnary state may be obtened' dixit us " << finl;
        break;
      default :
        Cerr << "Something abnormal has happened : it is preferable to stop." << finl;
        exit();
      }
  }
  return(1);
}


// Description:
//    Operateur de multiplication d'une matrice par un vecteur:
//    scaling des lignes de la matrice par les coefficients
//    correspondants du vecteur passe en parametre.
//    A *= x, effectue les scaling suivants:
//      A(i,:) = A(i,:) * x(i), pour toutes les lignes i de A
// Precondition:
// Parametre: DoubleVect& x
//    Signification: vecteur de scaling
//    Valeurs par defaut:
//    Contraintes: x.size() = nb ligne de la matrice
//    Acces:
// Retour: Matrice_Morse&
//    Signification: le resultat de l'operation (*this)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse& Matrice_Morse::operator *=(const DoubleVect& x)
{
  int i,k;
  for(i = 0; i<nb_lignes(); i++)
    for(k = tab1_(i)-1; k<tab1_(i+1)-1; k++)
      coeff_(k) *= x(i);
  return *this;
}


// Description:
//    Affecte le produit de 2 matrices Morse A et B a l'objet (this).
//    Operation: this = A * B
// Precondition:
// Parametre: Matrice_Morse& A
//    Signification: une matrice au format Morse
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: Matrice_Morse& B
//    Signification: une matrice au format Morse
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Matrice_Morse&
//    Signification: le resultat de l'operation (*this)
//    Contraintes:
// Exception:
// Effets de bord: redimensionne la matrice en cas de besoin,
//                 si pas assez d'espace pour les elements non nuls generes par le produit.
// Postcondition:
Matrice_Morse& Matrice_Morse::affecte_prod(const Matrice_Morse& a, const Matrice_Morse& b)
{
  int nrow= a.nb_lignes();                // nb de lignes de A
  int ncol= b.nb_colonnes();                // nb de colonnes de B
  //assert(nrow==ncol);
  // Jloi non?
  assert(nrow==ncol);
  tab1_.resize(nrow+1);
  int job = 1 ;                      // on recupere tout (tab1, tab2, coeff de matrice_resu)
  int  nzmax = nb_coeff();                // nb de valeurs maximales de la matrice resultante
  if(nzmax==0)
    {
      nzmax=a.nb_coeff();
      tab2_.resize(nzmax);
      coeff_.resize(nzmax);
      assert(nzmax==nb_coeff());
    }
  IntVect iw(ncol+1);                        // tableau de travail


  //  Cerr << " iao1 " << a.tab1_ << finl ;
  //  Cerr << " jao1 " << a.tab2_ << finl ;
  //  Cerr << " aao1 " << a.coeff_ << finl ;
  //
  //  Cerr << " iao2 " << b.tab1_ << finl ;
  //  Cerr << " jao2 " << b.tab2_ << finl ;
  //  Cerr << " aao2 " << b.coeff_ << finl ;


  //  while(ierr=0)
  //  {
  //    F77NAME(AMUB)(&nb_lig, &nb_col, &job, A.coeff_, A.tab2_, A.tab1_,
  //                  B.coeff_, B.tab2_, B.tab1_,
  //                  coeff_.addr(), tab2_.addr(), tab1_.addr(), &nzmax, iw, &ierr);
  //
  //
  //c----------------------------------------------------------------------c
  //       subroutine amub (nrow,ncol,job,a,ja,ia,b,jb,ib,
  //     *                 c,jc,ic,nzmax,iw,ierr)
  //      real*8 a(*), b(*), c(*)
  //      integer ja(*),jb(*),jc(*),ia(nrow+1),ib(ncol+1),
  //     *     ic(ncol+1),iw(ncol)
  //c-----------------------------------------------------------------------
  //c performs the matrix by matrix product C = A B
  //c-----------------------------------------------------------------------
  //c on entry:
  //c ---------
  //c nrow        = integer. The row dimension of A
  //c ncol  = integer. The column dimension of B
  //c job   = integer. Job indicator. When job = 0, only the structure
  //c                  (i.e. the arrays jc, ic) is computed and the
  //c                  real values are ignored.
  //c
  //c a,
  //c ja,
  //c ia   = Matrix A in compressed sparse row format.
  //c
  //c b,
  //c jb,
  //c ib        =  Matrix B in compressed sparse row format.
  //c
  //c nzmax        = integer. The  length of the arrays c and jc.
  //c         amub will stop if the result matrix C  has a number
  //c         of elements that exceeds exceeds nzmax. See ierr.
  //c
  //c on return:
  //c----------
  //c c,
  //c jc,
  //c ic        = resulting matrix C in compressed sparse row sparse format.
  //c
  //c ierr        = integer. serving as error message.
  //c         ierr = 0 means normal return,
  //c         ierr .gt. 0 means that amub stopped while computing the
  //c         i-th row  of C with i=ierr, because the number
  //c         of elements in C exceeds nzmax.
  //c
  //c work arrays:
  //c------------
  //c iw        = integer work array of length equal to the number of
  //c         columns in A.
  //c Notes:
  //c-------
  //c         The column dimension of B is not needed.
  //c
  //c-----------------------------------------------------------------
  //      real*8 scal
  //      logical values
  //      values = (job .ne. 0)
  //      len = 0
  //      ic(1) = 1
  //      ierr = 0
  //c     initialize array iw.
  //      do 1 j=1, ncol
  //         iw(j) = 0
  // 1    continue
  //c

  double scal=0. ;
  int ii, jj ,k , ka, kb ;
  int values = 0;
  if (job != 0) values = 1 ;
  int len = -1 ;
  tab1_[0] = 1  ;
  iw = -1 ;

  //      do 500 ii=1, nrow
  //c     row i
  //         do 200 ka=ia(ii), ia(ii+1)-1
  //            if (values) scal = a(ka)
  //            jj   = ja(ka)
  //            do 100 kb=ib(jj),ib(jj+1)-1
  //               jcol = jb(kb)
  //               jpos = iw(jcol)
  //               if (jpos .eq. 0) then
  //                  len = len+1
  //                  if (len .gt. nzmax) then
  //                     ierr = ii
  //                     return
  //                  endif
  //                  jc(len) = jcol
  //                  iw(jcol)= len
  //                  if (values) c(len)  = scal*b(kb)
  //               else
  //                  if (values) c(jpos) = c(jpos) + scal*b(kb)
  //               endif
  // 100            continue
  // 200     continue
  //         do 201 k=ic(ii), len
  //            iw(jc(k)) = 0
  // 201     continue
  //         ic(ii+1) = len+1
  // 500  continue

  //    Cerr << "Matrice_Morse::affect_prod nrow " << nrow << finl;

  for(ii=0; ii< nrow; ii++)
    {
      for(ka=a.tab1_[ii]-1; ka < a.tab1_[ii+1]-1; ka++)
        {
          if (values == 1) scal = a.coeff_[ka] ;
          jj   = a.tab2_[ka] - 1 ;
          for (kb=b.tab1_[jj]-1; kb < b.tab1_[jj+1]-1; kb++)
            {
              int jcol = b.tab2_[kb] -1 ;
              int jpos = iw[jcol]  ;
              if (jpos == -1)
                {
                  len++ ;
                  if (len > nzmax-1)
                    {
                      // Cerr << "Matrice_Morse::affect_prod len > nzmax -1 " << nzmax << finl;
                      nzmax += nrow/ii*nzmax;
                      coeff_.resize(nzmax);
                      tab2_.resize(nzmax);
                    }
                  tab2_[len] = jcol + 1 ;
                  iw[jcol]= len ;
                  if (values == 1) coeff_[len]  = scal*b.coeff_[kb] ;
                }
              else
                {
                  if (values == 1) coeff_[jpos] += scal*b.coeff_[kb] ;
                }
            }
        }

      for (k=tab1_[ii]-1; k < len+1 ; k++) iw[tab2_[k]-1] = -1 ;
      tab1_[ii+1] = (len+1) + 1 ;
    }

  //      return
  //c-------------end-of-amub-----------------------------------------------
  //c-----------------------------------------------------------------------
  //
  //    if(ierr)
  //    {
  //      nzmax+=nrow/ierr*nzmax;
  //      coeff_.resize(nzmax);
  //      tab2_.resize(nzmax);
  //    }
  //
  //  }

  coeff_.resize(tab1_[nrow]-1);
  tab2_.resize(tab1_[nrow]-1);
  morse_matrix_structure_has_changed_=1;
  return *this;

}



// Description:
//    Fonction (hors classe) amie de la classe Matrice_Morse
//    Scaling de la matrice par un scalaire: multiplie tous
//    les elements de la matrice par un scalaire.
//    Operation: renvoie x*A
// Precondition:
// Parametre: double x
//    Signification: valeur du scaling
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: Matrice_Morse& B
//    Signification: une matrice au format Morse
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Matrice_Morse
//    Signification: le resultat de l'operation
//    Contraintes: la matrice resultat est allouee par la fonction
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse operator *(double x , const Matrice_Morse& A)
{
  Matrice_Morse mat_res(A);
  mat_res.coeff_*=x;
  return(mat_res);
}

// Description:
//    Operateur de negation unaire, renvoie l'opposee de la matrice: - A
//    Appelle operator*(double,const Matrice_Morse&)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Matrice_Morse
//    Signification: le resultat de l'appel a operator*(double,const Matrice_Morse&)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Matrice_Morse Matrice_Morse::operator -() const
{
  return((-1)*(*this));
}


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Matrice_Morse&
//    Signification: une matrice morse
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Retour: Matrice_Morse&
//    Signification: renvoie toujours *this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse& Matrice_Morse::operator +=(const Matrice_Morse& A)
{
  // PL: Avant de verifier de faire des operations couteuses en RAM, on verifie
  // si ce n'est pas la meme structure:
  if (has_same_morse_matrix_structure(A))
    {
      int size = A.nb_coeff();
      const DoubleVect& coeff = A.get_coeff();
      for (int i=0; i<size; i++)
        coeff_(i)+=coeff(i);
    }
  else
    {
      *this = *this + A;
      morse_matrix_structure_has_changed_ = 1;
    }
  return *this;
}


// Description:
//    Operateur de multiplication (de tous les elements)
//    d'une matrice par un scalaire.
//    Operation: A = x * A
// Precondition:
// Parametre: double x
//    Signification: le parametre de scaling
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Matrice_Morse&
//    Signification: le resultat de l'operation (*this)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse& Matrice_Morse::operator *=(double x )
{
  scale( x );
  return(*this);
}

void Matrice_Morse::scale( const double& x )
{
  coeff_ *= x;
}

void Matrice_Morse::get_stencil( IntTab& stencil ) const
{
  assert_check_morse_matrix_structure( );

  if( is_stencil_up_to_date_ )
    {
      stencil = stencil_;
      return;
    }


  stencil.resize( 0, 2 );
  stencil.resize(tab2_.size_array(), 2);
  stencil.set_smart_resize( 1 );

  ArrOfInt tmp;
  tmp.set_smart_resize( 1 );

  int compteur = 0;

  const int nb_lines = nb_lignes( );
  for ( int i=0; i<nb_lines; ++i )
    {
      int k0   = tab1_( i ) - 1;
      int k1   = tab1_( i + 1 ) - 1;
      int size = k1 - k0;

      tmp.resize_array( 0 );
      tmp.resize_array( size );

      for ( int k=0; k<size; ++k )
        {
          tmp[ k ] = tab2_( k + k0 ) - 1;
        }

      tmp.ordonne_array( );

      for ( int k=0; k<size; ++k )
        {
          stencil( k+compteur , 0 ) = i;
          stencil( k+compteur , 1 ) =  tmp[ k ];
        }
      compteur += size;
    }

  stencil.set_smart_resize( 0 );
}

void Matrice_Morse::get_stencil_and_coefficients( IntTab&      stencil,
                                                  ArrOfDouble& coefficients ) const
{
  assert_check_morse_matrix_structure( );

  if( is_stencil_up_to_date_ )
    {
      if( coeff_.size( ) == 0 )
        {
          Cerr << "Error in Matrice_Morse::get_stencil_and_coefficients( )"<<finl;
          Cerr << "  The coefficients are not filled."<<finl;
          Cerr << "  Aborting..." << finl;
          Process::abort( );
        }
      stencil = stencil_ ;
      coefficients = coeff_ ;
      return;
    }

  stencil.resize( 0, 2 );
  stencil.resize(tab2_.size_array(), 2);
  stencil.set_smart_resize( 1 );

  coefficients.resize( 0 );
  coefficients.set_smart_resize( 1 );
  coefficients.resize(tab2_.size_array());

  IntTab tmp1(0);
  tmp1.set_smart_resize( 1 );

  ArrOfDouble tmp2;
  tmp2.set_smart_resize( 1 );

  ArrOfInt index;
  index.set_smart_resize( 1 );

  int compteur = 0;

  const int nb_lines = nb_lignes( );
  for ( int i=0; i<nb_lines; ++i )
    {
      int k0   = tab1_( i ) - 1;
      int k1   = tab1_( i + 1 ) - 1;
      int size = k1 - k0;

      tmp1.resize( 0 );
      tmp1.resize( size );

      tmp2.resize_array( 0 );
      tmp2.resize_array( size );

      index.resize_array( 0 );

      for ( int k=0; k<size; ++k )
        {
          tmp1( k ) = tab2_( k + k0 ) - 1;
          tmp2[ k ] = coeff_( k + k0 );
        }

      tri_lexicographique_tableau_indirect( tmp1, index );

      for ( int k=0; k<size; ++k )
        {
          int l = index[ k ];
          stencil( compteur + k , 0 ) = i;
          stencil( compteur + k , 1 ) = tmp1[ l ];
          coefficients[ compteur + k ] = tmp2[ l ];
        }
      compteur += size;
    }

  assert( coefficients.size_array( ) == stencil.dimension( 0 ));

  stencil.set_smart_resize( 0 );

  coefficients.set_smart_resize( 0 );
}

// Description:
//    Operateur de division (de tous les elements)
//    d'une matrice par un scalaire.
//    Operation: A =  A / x
// Precondition:
// Parametre: double x
//    Signification: le parametre de scaling
//    Valeurs par defaut:
//    Contraintes: x doit etre different de zero
//    Acces:
// Retour: Matrice_Morse&
//    Signification: le resultat de l'operation (*this)
//    Contraintes:
// Exception: division par zero impossible
// Effets de bord:
// Postcondition:
Matrice_Morse& Matrice_Morse::operator /=(double x )
{
  coeff_/=x;
  return(*this);
}

void Matrice_Morse::remplir(const IntLists& voisins,
                            const DoubleLists& valeurs,
                            const DoubleVect& terme_diag)
{

  int num_elem;
  int compteur,rang =0;

  // Remplissage des tableaux tab1, tab2 et coeff_ :
  int* p_tab1 = tab1_.addr();
  int* p_tab2 = tab2_.addr();
  double* p_coeff = coeff_.addr();

  int* tab2_ptr = p_tab2;
  int n=nb_lignes();

  for (num_elem=0; num_elem<n; num_elem++)
    {

      IntList_Curseur liste_vois(voisins[num_elem]);
      DoubleList_Curseur liste_val(valeurs[num_elem]);
      compteur =0;
      *p_tab1++ = rang;

      *tab2_ptr++=num_elem;
      *p_coeff++ = terme_diag[num_elem];

      while  (liste_vois)
        {
          *tab2_ptr++ = liste_vois.valeur();
          *p_coeff++ = liste_val.valeur();
          ++liste_vois;
          ++liste_val;
          compteur++;
        }
      //       tab2[rang] = compteur;
      rang += (compteur + 1);
    }
  tab1_(num_elem)=rang;
  formeF();
  morse_matrix_structure_has_changed_=1;
  is_stencil_up_to_date_=false;
}

void Matrice_Morse::remplir(const IntLists& voisins,
                            const DoubleLists& valeurs)
{

  int num_elem;
  int compteur,rang =0;

  // Remplissage des tableaux tab1, tab2 et coeff_ :
  int* p_tab1 = tab1_.addr();
  int* p_tab2 = tab2_.addr();
  double* p_coeff = coeff_.addr();

  int* tab2_ptr = p_tab2;
  int n=nb_lignes();

  for (num_elem=0; num_elem<n; num_elem++)
    {

      IntList_Curseur liste_vois(voisins[num_elem]);
      DoubleList_Curseur liste_val(valeurs[num_elem]);
      compteur =0;
      *p_tab1++ = rang;

      while  (liste_vois)
        {
          *tab2_ptr++ = liste_vois.valeur();
          *p_coeff++ = liste_val.valeur();
          ++liste_vois;
          ++liste_val;
          compteur++;
        }
      //       tab2[rang] = compteur;
      rang += (compteur);
    }
  tab1_(num_elem)=rang;
  formeF();
  morse_matrix_structure_has_changed_=1;
  is_stencil_up_to_date_=false;
}

// Description:
// Remplissage d'une matrice morse par une matrice morse plus petite
void Matrice_Morse::remplir(const int ideb, const int jdeb, const int n, const int m, const Matrice_Morse& mat)
{
  // Verification
  assert(ideb<=n);
  assert(jdeb<=m);

  // On va construire une matrice locale
  Matrice_Morse matrice_locale(mat);
  // Cas ou la matrice locale est symetrique
  if (sub_type(Matrice_Morse_Sym,mat))
    {
      // Creation de la partie inferieure L
      Matrice_Morse L(matrice_locale);
      L.transpose(matrice_locale);
      int lordre = L.ordre();
      for (int i=0; i<lordre; i++)
        L(i, i) = 0.;
      // On ajoute M=U+L
      matrice_locale += L;
    }

  // Dimensionnement de la matrice globale
  int nnz=matrice_locale.nb_coeff();
  dimensionner(n,m,nnz);

  // Remplissage de la matrice globale par la matrice locale:
  // Remplissage de tab1_ avec le decalage par ideb:
  int mon_nb_lignes=matrice_locale.nb_lignes();
  assert(mon_nb_lignes+ideb<=n);
  for (int i=0; i<ideb; i++)
    tab1_(i)=1;
  for (int i=0; i<mon_nb_lignes; i++)
    tab1_(i+ideb)=matrice_locale.tab1_(i);
  for (int i=mon_nb_lignes+ideb; i<n+1; i++)
    tab1_(i)=matrice_locale.tab1_(mon_nb_lignes);

  // Remplissage de tab2_ avec le decalage par jdeb:
  for (int i=0; i<nnz; i++)
    tab2_(i)=matrice_locale.tab2_(i)+jdeb;

  // Remplissage de coeff_:
  for (int i=0; i<nnz; i++)
    coeff_(i)=matrice_locale.coeff_(i);

  morse_matrix_structure_has_changed_=1;
  is_stencil_up_to_date_=false;
}

void Matrice_Morse::formeC()
{
  int ii, k;
  int n=nb_lignes();
  for( ii=0; ii<=n; ii++)
    tab1_(ii)--;
  for( ii=0; ii<n; ii++)
    tab2_(tab1_(ii))=nb_vois(ii);
  for(k=0; k<nb_coeff(); k++)
    tab2_(k)--;
  morse_matrix_structure_has_changed_=1;
  is_stencil_up_to_date_=false;
}

void Matrice_Morse::formeF()
{
  int ii, k;
  int n=nb_lignes();
  for( ii=0; ii<=n; ii++)
    tab1_(ii)++;
  for(k=0; k<nb_coeff(); k++)
    tab2_(k)++;
  morse_matrix_structure_has_changed_=1;
  is_stencil_up_to_date_=false;
}




// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Matrice_Morse_test()
{
  return 1;
}

// Description:
//   Remplit la matrice avec des zeros.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: void
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:

void Matrice_Morse::clean()
{
  for (int i=0; i<nb_coeff(); i++)
    coeff_[i]=0.;
}

// Description:
// Calcule la largeur de bande d'une matrice morse
// Precondition:
// Parametre:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Matrice_Morse::largeur_de_bande() const
{
  int ldist,min = 0;
  const int* p_tab1_ = get_tab1().addr();
  const int* p_tab2_ = get_tab2().addr();
  int N=ordre();

  for(int i=0; i<N; i++)
    for(int k = p_tab1_[i]; k < p_tab1_[i+1]; k++)
      {
        if (p_tab2_[k-1]-1<N)
          {
            ldist = p_tab2_[k-1] - i;
            if( min < ldist ) min = ldist;
          }
      };
  return min;
}

bool Matrice_Morse::check_morse_matrix_structure( void ) const
{
  const int nb_lines   = nb_lignes( );
  const int nb_columns = nb_colonnes( );
  const int nb_coefficients = tab1_( nb_lines ) - 1;

  if ( tab2_.size( ) != nb_coefficients )
    {
      Cerr << "invalid tab2 size" << finl;
      return false;
    }

  if ( coeff_.size( ) != nb_coefficients )
    {
      Cerr << "invalid coeff size" << finl;
      return false;
    }

  ArrOfBit flags( nb_columns );

  for ( int i=0; i<nb_lines; ++i )
    {
      flags = 0;

      int k0 = tab1_( i ) - 1;
      int k1 = tab1_( i + 1 ) - 1;

      for ( int k=k0; k<k1; ++k )
        {
          int j = tab2_( k ) - 1;

          if ( j < 0 )
            {
              Cerr << "invalid column index (<0): " << j << finl;
              return false;
            }

          if ( j >= nb_columns )
            {
              Cerr << "invalid column index (>nb_cols): " << j << " > " << nb_columns << finl;
              return false;
            }

          if ( flags[ j ] )
            {
              Cerr << "invalid coefficient ( " << i << ", " << j << " ): already defined ( " << k << " )" << finl;
              return false;
            }

          flags.setbit( j );
        }
    }

  return true;
}

bool Matrice_Morse::check_sorted_morse_matrix_structure( void ) const
{
  const int nb_lines   = nb_lignes( );
  const int nb_columns = nb_colonnes( );
  const int nb_coefficients = tab1_( nb_lines ) - 1;

  if ( tab2_.size( ) != nb_coefficients )
    {
      Cerr << "invalid tab2 size" << finl;
      return false;
    }

  if ( coeff_.size( ) != nb_coefficients )
    {
      Cerr << "invalid coeff size" << finl;
      return false;
    }

  ArrOfBit flags( nb_columns );

  for ( int i=0; i<nb_lines; ++i )
    {
      flags = 0;

      int k0 = tab1_( i ) - 1;
      int k1 = tab1_( i + 1 ) - 1;

      int j0 = tab2_( k0 ) - 1 - 1;

      for ( int k=k0; k<k1; ++k )
        {
          int j = tab2_( k ) - 1;

          if ( j < 0 )
            {
              Cerr << "invalid column index (<0): " << j << finl;
              return false;
            }

          if ( j >= nb_columns )
            {
              Cerr << "invalid column index (>nb_cols): " << j << " > " << nb_columns << finl;
              return false;
            }

          if ( flags[ j ] )
            {
              Cerr << "invalid coefficient ( " << i << ", " << j << " ): already defined ( " << k << " )" << finl;
              return false;
            }

          if ( j <= j0 )
            {
              Cerr << "unsorted coefficient: ( " << i << ", " << j << " ) after ( " << i << ", " << j0 << " ) " << finl;;
              return false;
            }

          j0 = j;
          flags.setbit( j );
        }
    }

  return true;
}


void Matrice_Morse::assert_check_morse_matrix_structure( void ) const
{
  if (!morse_matrix_structure_has_changed_) return;
#ifndef NDEBUG
  if ( ! ( check_morse_matrix_structure( ) ) )
    {
      Cerr << "Error in 'Matrice_Morse::assert_check_morse_matrix_structure( )':" << finl;
      Cerr << "  Exiting..." << finl;
      Process::exit( );
    }
  else
    morse_matrix_structure_has_changed_=0;
#endif
}

void Matrice_Morse::assert_check_sorted_morse_matrix_structure( void ) const
{
  if (!morse_matrix_structure_has_changed_) return;
#ifndef NDEBUG
  if ( ! ( check_sorted_morse_matrix_structure( ) ) )
    {
      Cerr << "Error in 'Matrice_Morse::assert_check_sorted_morse_matrix_structure( )':" << finl;
      Cerr << "  Exiting..." << finl;
      Process::exit( );
    }
  else
    morse_matrix_structure_has_changed_=0;
#endif
}

// Build a new Morse matrix spanning the rectangular area defined by the two points (nl0, nc0) and (nl1, nc1)
// in the original matrix.
// Indices are provided in C mode (0-based indexing).
void Matrice_Morse::construire_sous_bloc(int nl0, int nc0, int nl1, int nc1, Matrice_Morse& result) const
{
  // count non-zero entries:
  assert(nl0 >= 0);
  assert(nc0 >= 0);
  assert(nl0 <= nl1);
  assert(nc0 <= nc1);

  int max_nnz = tab1_(nl1+1) - tab1_(nl0); // maximum number of zeros that we will find
  int tot=0;
  IntTab loca(max_nnz, 2);
  DoubleTab sub_coeffs(max_nnz);
  for (int li=nl0; li <= nl1; li++)
    {
      int idx_coeff = tab1_(li)-1;
      int nb_coeff_on_line = tab1_(li+1)-tab1_(li);
      for (int j=0; j < nb_coeff_on_line; j++)
        {
          int col_idx = tab2_(j+idx_coeff)-1;
          if (col_idx >= nc0 && col_idx <= nc1) // is the coeff in the window?
            {
              loca(tot, 0) = li - nl0;
              loca(tot, 1) = col_idx - nc0;
              sub_coeffs(tot) = coeff_(j+idx_coeff);
              tot++;
            }
        }
    }
  loca.resize(tot,2);
  sub_coeffs.resize(tot);

  result.dimensionner(loca);
  // Set coefficient values:
  for (int i =0 ; i < tot; i++)
    {
      int il = loca(i, 0);
      int ic = loca(i, 1);
      result.coef(il, ic) = sub_coeffs(i);
    }
}

