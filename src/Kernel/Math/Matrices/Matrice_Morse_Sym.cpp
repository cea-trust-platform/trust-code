/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Matrice_Morse_Sym.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/38
//
//////////////////////////////////////////////////////////////////////////////

#include <Matrice_Morse_Sym.h>
#include <IntList.h>
#include <DoubleList.h>
#include <Sparskit.h>
#include <comm_incl.h>
#include <Noms.h>
#include <Vect_ArrOfInt.h>
#include <Vect_IntTab.h>
#include <Array_tools.h>

Implemente_instanciable_sans_constructeur(Matrice_Morse_Sym,"Matrice_Morse_Sym",Matrice_Morse);

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
Sortie& Matrice_Morse_Sym::printOn(Sortie& s) const
{
  s<<get_est_definie()<<finl;
  return  Matrice_Morse::printOn(s);
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
Entree& Matrice_Morse_Sym::readOn(Entree& s)
{
  int est_def;
  s>>est_def;
  set_est_definie(est_def);
  return Matrice_Morse::readOn(s) ;
}

// Description:
//    Constructeur d'une Matrice_Morse_Sym comportant
//    n1 lignes et pouvant stocker n2 elements non-nuls (au maximum).
//    Les elements de la matrice et la table des connectivites
//    sont donnes dans les 3 derniers parametres.
// Precondition:
// Parametre: int n1
//    Signification: le nombre de ligne de la matrice
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int n2
//    Signification: le nombre d'element non nuls stockable
//                   par la matrice
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: IntLists& voisins
//    Signification: liste des voisins de chaque lignes
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleLists& valeurs
//    Signification: liste des valeurs
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& terme_diag
//    Signification: le vecteur des termes diagonaux
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse_Sym::Matrice_Morse_Sym(int n1, int n2, const IntLists& voisins,
                                     const DoubleLists& valeurs,
                                     const DoubleVect& terme_diag)
  : Matrice_Morse(n1, n2)
{
  symetrique_=1;
  remplir(voisins, valeurs, terme_diag);
  morse_matrix_structure_has_changed_=1;
}


// Description:
//    Constructeur d'une Matrice_Morse_Sym par copie
//    d'une Matrice_Morse.
// Precondition:
// Parametre: Matrice_Morse& acopier
//    Signification: la matrice a copier
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse_Sym::Matrice_Morse_Sym(const Matrice_Morse& A)
  :Matrice_Morse(A)
{
  symetrique_=1;
  partie_sup(A);
  compacte();
  morse_matrix_structure_has_changed_=1;
}
Matrice_Morse_Sym::Matrice_Morse_Sym(const Matrice& A)
{
  const Matrice_Base& A_base=A.valeur();
  if(sub_type(Matrice_Morse_Sym, A_base))
    *this=ref_cast(Matrice_Morse_Sym, A_base);
  else if(sub_type(Matrice_Morse, A_base))
    *this=ref_cast(Matrice_Morse, A_base);
  symetrique_=1;
  morse_matrix_structure_has_changed_=1;
}
Matrice_Morse_Sym& Matrice_Morse_Sym::operator=(const Matrice_Morse& A)
{
  tab1_=A.tab1_;
  tab2_=A.tab2_;
  coeff_=A.coeff_;
  m_=A.m_;
  symetrique_=A.symetrique_;
  partie_sup(A);
  compacte();
  morse_matrix_structure_has_changed_=1;
  return *this;
}
Matrice_Morse_Sym& Matrice_Morse_Sym::operator=(const Matrice& A)
{
  const Matrice_Base& A_base=A.valeur();
  if(sub_type(Matrice_Morse_Sym, A_base))
    *this=ref_cast(Matrice_Morse_Sym, A_base);
  else if(sub_type(Matrice_Morse, A_base))
    *this=ref_cast(Matrice_Morse, A_base);
  morse_matrix_structure_has_changed_=1;
  return *this;
}

// Description:
//    Constructeur d'une Matrice_Morse_Sym par copie
//    d'une Matrice_Morse.
// Precondition:
// Parametre: Matrice_Morse& acopier
//    Signification: la matrice a copier
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse_Sym::Matrice_Morse_Sym(const Matrice_Morse_Sym& acopier) :
  Matrice_Morse(acopier),Matrice_Sym()
{
  set_est_definie(acopier.get_est_definie());
  symetrique_=1;
  morse_matrix_structure_has_changed_=1;
}

// Description:
//    Operation de multiplication-accumulation (saxpy) matrice matrice
//    (matrice represente par un tableau)
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
// Exception: taille du resultat incompatible avec la taille de x
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Matrice_Morse_Sym::ajouter_multTab_(const DoubleTab& x, DoubleTab& resu) const
{
  if ( (x.nb_dim() == 1) && (resu.nb_dim() == 1))
    {
      ajouter_multvect(x,resu);
      return resu;
    }

  assert_check_symmetric_morse_matrix_structure( );
  int i,k,j;
  double aij;
  int comp;
  int nb_com=x.dimension(1);
  assert(resu.dimension(1)==nb_com);
  double* t=new double[nb_com];
  int n=nb_lignes();
  for(i=0; i<n; i++)
    {
      for(comp=0; comp<nb_com; comp++)
        t[comp] = coeff_(tab1_(i)-1)*x(i,comp);
      for (k=tab1_(i); k<tab1_(i+1)-1; k++)
        {
          j = tab2_(k)-1;
          aij = coeff_(k);
          for(comp=0; comp<nb_com; comp++)
            {
              t[comp] += aij*x(j,comp);
              resu(j,comp) += aij*x(i,comp);
            }
        }
      for(comp=0; comp<nb_com; comp++)
        resu(i,comp) += t[comp] ;
    }
  delete t;
  return resu;
}

double Matrice_Morse_Sym::multvect_et_prodscal(const DoubleVect& x, DoubleVect& resu) const
{
  assert_check_symmetric_morse_matrix_structure( );

  assert(x.size_totale() == nb_lignes() || x.size() == nb_lignes());
  assert(resu.size_totale() == nb_lignes() || resu.size() == nb_lignes());
  assert(m_ <= x.size_totale());

  double prod_scal_local = 0.;
  operator_egal(resu, 0.);

  const int fin = nb_lignes() + 1;
  {
    const double * thecoef = coeff_.addr() - 1;
    const double * xx = x.addr() - 1;
    double * res = resu.addr() - 1;
    const int * index1 = tab1_.addr() - 1;
    const int * index2 = tab2_.addr() - 1;
#ifdef COMPILER_BLRTS_XLC
#pragma disjoint (*coef, *xx, *res)
#endif
    int i;
    for (i = 1; i < fin; i++)
      {
        int j = index1[i];
        int j_next= index1[i+1];
        //int ncoeffs = j_next - j;
        assert(i==index2[j]); // La diagonale meme nulle doit etre stockee dans une Mat_Morse_Sym
        double xi = xx[i];
        double resu_tmp = res[i] + thecoef[j] * xi;
        j++;
        for (; j < j_next; j++)
          {
            int nj     = index2[j];
            double coef_j = thecoef[j];
            double xn     = xx[nj];
            resu_tmp   += coef_j * xn;
            res[nj] += coef_j * xi;
          }
        res[i] = resu_tmp;
        prod_scal_local += resu_tmp * xi;
      }
  }

  return prod_scal_local;
}


// Description:
//    Operation de multiplication-accumulation (saxpy) matrice vecteur.
//    Operation: resu = resu + A*x
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

DoubleVect& Matrice_Morse_Sym::ajouter_multvect_(const DoubleVect& x, DoubleVect& resu) const
{
  assert_check_symmetric_morse_matrix_structure( );

  assert(x.size_totale() == nb_lignes() || x.size() == nb_lignes());
  assert(resu.size_totale() == nb_lignes() || resu.size() == nb_lignes());
  assert(m_ <= x.size_totale());

  const int fin = nb_lignes() + 1;
  {
    const double * thecoef = coeff_.addr() - 1;
    const double * xx = x.addr() - 1;
    double * res = resu.addr() - 1;
    const int * index1 = tab1_.addr() - 1;
    const int * index2 = tab2_.addr() - 1;
#ifdef COMPILER_BLRTS_XLC
#pragma disjoint (*coef, *xx, *res)
#endif
    int i;
    for (i = 1; i < fin; i++)
      {
        int j = index1[i];
        int j_next= index1[i+1];
        assert(i==index2[j]); // La diagonale meme nulle doit etre stockee dans une Mat_Morse_Sym
#if 0
        // Note B.M.: sur les procs intel ce deroulage n'a aucun effet benefique.
        int ncoeffs = j_next - j;
        if (ncoeffs == 4)
          {
            int n2 = index2[j+1];
            int n3 = index2[j+2];
            int n4 = index2[j+3];
            double xi = xx[i];
            double res_i  = res[i];
            double res_n2 = res[n2];
            double res_n3 = res[n3];
            double res_n4 = res[n4];
            double a1 = thecoef[j];
            double a2 = thecoef[j+1];
            double a3 = thecoef[j+2];
            double a4 = thecoef[j+3];
            res[i] = res_i + a1 * xi + a2 * xx[n2] + a3 * xx[n3] + a4 * xx[n4];
            res[n2] = res_n2 + a2 * xi;
            res[n3] = res_n3 + a3 * xi;
            res[n4] = res_n4 + a4 * xi;
          }
        else
#endif
          {
            double xi = xx[i];
            double resu_tmp = res[i] + thecoef[j] * xi;
            j++;
            for (; j < j_next; j++)
              {
                int nj     = index2[j];
                double coef_j = thecoef[j];
                double xn     = xx[nj];
                resu_tmp   += coef_j * xn;
                res[nj] += coef_j * xi;
              }
            res[i] = resu_tmp;
          }
      }
  }
#if 0
  int fin2,l;
  m=tab1_(0);
  for(i=debut; i<n; i++)
    {
      xi=x(i);
      l=m;
      m=tab1_(i+1);
      t = coeff_(l-1)*xi;
      assert(i==tab2(l-1)-1); // La diagonale meme nulle doit etre stockee dans une Mat_Morse_Sym
      //aij=coeff_(tab1_(i)-1);
      fin2=m-1;
      for (k=l; k<fin2; k++)
        {
          j = tab2_(k)-1;
          assert(j<n);
          aij = coeff_(k);
          t += aij*x(j);
          resu(j) += aij*xi;
        }
      resu(i) += t ;
    }
#endif
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
DoubleVect& Matrice_Morse_Sym::ajouter_multvectT_(const DoubleVect& x,DoubleVect& resu) const
{
  Cerr <<"Matrice_Morse_Sym::ajouter_multvectT_ is not coded" << finl;
  exit();
  return resu;
}


// Description:
//    Fonction (hors classe) amie de la classe Matrice_Morse_Sym
//    NE FAIT RIEN : NON CODE
// Precondition:
// Parametre: Matrice_Morse_Sym&
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Parametre: Matrice_Morse_Sym&
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Retour: Matrice_Morse_Sym
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse_Sym operator+(const Matrice_Morse_Sym& A,const Matrice_Morse_Sym& B)
{
  //Il faut evidemment que les matrices soient de meme taille
  if (A.nb_lignes() != B.nb_lignes())
    {
      Cerr << "Error in Matrice_Morse_Sym::operator+" << finl;
      Cerr << "The 2 matrices must have the same dimension" << finl;
      Cerr << "The first matrix is of dimension: " << A.nb_lignes()<< finl;
      Cerr << "The second matrix is of dimension: " << B.nb_lignes()<< finl;
      Process::exit();
    }

  Matrice_Morse_Sym somme(A.nb_lignes(),A.nb_coeff()+B.nb_coeff());
  IntList colonnes_A_ligne_i,colonnes_B_ligne_i;
  int min_colonnes_A,min_colonnes_B,non_nuls_ligne_i;
  DoubleList coeffs_A_ligne_i,coeffs_B_ligne_i;
  double coeff_A,coeff_B;

  //Pour s'assurer que compacte() va marcher
  for (int i=0; i<somme.coeff_.size(); i++)
    somme.coeff(i) = 0.;

  //On va remplir tab2_ par ordre croissant de colonnes
  somme.tab1(0) = 1;
  for (int i=0; i<A.nb_lignes(); i++) //boucle sur les lignes
    {
      //Pour eviter les effets de bord
      if (!colonnes_A_ligne_i.est_vide()) colonnes_A_ligne_i.vide();
      if (!colonnes_B_ligne_i.est_vide()) colonnes_B_ligne_i.vide();

      //Initialisation des colonnes_A et colonnes_B
      //ATTENTION a la numerotation C++
      for (int j=0; j<A.tab1(i+1)-A.tab1(i); j++)
        {
          colonnes_A_ligne_i.add_if_not(A.tab2(A.tab1(i)+j-1));
          coeffs_A_ligne_i.add(A.coeff(A.tab1(i)+j-1));
        }//fin for

      for (int j=0; j<B.tab1(i+1)-B.tab1(i); j++)
        {
          colonnes_B_ligne_i.add_if_not(B.tab2(B.tab1(i)+j-1));
          coeffs_B_ligne_i.add(B.coeff(B.tab1(i)+j-1));
        }//fin for

      //On initialise d'autres varibles
      non_nuls_ligne_i = 0;

      //Debut de l'algorithme
      while (!colonnes_A_ligne_i.est_vide() || !colonnes_B_ligne_i.est_vide())
        {
          //Si l'une des listes est vide et pas l'autre
          if (colonnes_A_ligne_i.est_vide() && !colonnes_B_ligne_i.est_vide())
            {
              somme.tab2(somme.tab1(i)+non_nuls_ligne_i-1) =
                colonnes_B_ligne_i[0];

              somme.coeff(somme.tab1(i)+non_nuls_ligne_i-1) =
                coeffs_B_ligne_i[0] ;

              colonnes_B_ligne_i.suppr(colonnes_B_ligne_i[0]);
              coeffs_B_ligne_i.suppr(coeffs_B_ligne_i[0]);
              non_nuls_ligne_i++;
            }//fin if

          if (colonnes_B_ligne_i.est_vide() && !colonnes_A_ligne_i.est_vide())
            {
              somme.tab2(somme.tab1(i)+non_nuls_ligne_i-1) =
                colonnes_A_ligne_i[0];

              somme.coeff(somme.tab1(i)+non_nuls_ligne_i-1) =
                coeffs_A_ligne_i[0];

              colonnes_A_ligne_i.suppr(colonnes_A_ligne_i[0]);
              coeffs_A_ligne_i.suppr(coeffs_A_ligne_i[0]);
              non_nuls_ligne_i++;
            }//fin if

          //Aucune des deux listes n'est vide
          min_colonnes_A = colonnes_A_ligne_i[0];
          min_colonnes_B = colonnes_B_ligne_i[0];
          coeff_A = coeffs_A_ligne_i[0];
          coeff_B = coeffs_B_ligne_i[0];

          if (min_colonnes_A < min_colonnes_B)
            {
              somme.tab2(somme.tab1(i)+non_nuls_ligne_i-1) = min_colonnes_A;
              somme.coeff(somme.tab1(i)+non_nuls_ligne_i-1) = coeff_A;

              colonnes_A_ligne_i.suppr(min_colonnes_A);
              coeffs_A_ligne_i.suppr(coeff_A);
              non_nuls_ligne_i++;
            }//fin if

          if (min_colonnes_B < min_colonnes_A)
            {
              somme.tab2(somme.tab1(i)+non_nuls_ligne_i-1) = min_colonnes_B;
              somme.coeff(somme.tab1(i)+non_nuls_ligne_i-1) = coeff_B;

              colonnes_B_ligne_i.suppr(min_colonnes_B);
              coeffs_B_ligne_i.suppr(coeff_B);
              non_nuls_ligne_i++;
            }//fin if

          if (min_colonnes_A == min_colonnes_B)
            {
              somme.tab2(somme.tab1(i)+non_nuls_ligne_i-1) = min_colonnes_A;
              somme.coeff(somme.tab1(i)+non_nuls_ligne_i-1) = coeff_A+coeff_B;

              colonnes_A_ligne_i.suppr(min_colonnes_A);
              colonnes_B_ligne_i.suppr(min_colonnes_B);
              coeffs_A_ligne_i.suppr(coeff_A);
              coeffs_B_ligne_i.suppr(coeff_B);
              non_nuls_ligne_i++;
            }//fin if

        }// fin while

      somme.tab1(i+1) = somme.tab1(i)+non_nuls_ligne_i;//?????
    }// fin for

  somme.compacte();
  somme.morse_matrix_structure_has_changed_=1;
  return somme;
}


// Description:
//    Fonction (hors classe) amie de la classe Matrice_Morse_Sym
//    NE FAIT RIEN : NON CODE
// Precondition:
// Parametre: double
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: NON ACCEDE
// Parametre: Matrice_Morse_Sym& A
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Matrice_Morse_Sym
//    Signification:
//    Contraintes:
// Exception: NON CODE
// Effets de bord:
// Postcondition:
Matrice_Morse_Sym operator *(double x, const Matrice_Morse_Sym& A)
{
  Matrice_Morse_Sym mat_res(A);
  mat_res.coeff_*=x;
  mat_res.morse_matrix_structure_has_changed_=1;
  return(mat_res);
}


// Description:
//    Fonction (hors classe) amie de la classe Matrice_Morse_Sym
//    Simple appel a operator*(double,const Matrice_Morse_Sym&) (qui est NON CODE)
// Precondition:
// Parametre: Matrice_Morse_Sym& A
//    Signification: la matrice a multiplier par x
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: double x
//    Signification: un scalaire
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Matrice_Morse_Sym
//    Signification: le resultat de l'appel sous-jacent
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse_Sym operator *(const Matrice_Morse_Sym& A, double x)
{
  return(x*A);
}


// Description:
//    NE FAIT RIEN : NON CODE
// Precondition:
// Parametre: double
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: NON ACCEDE
// Retour: Matrice_Morse_Sym
//    Signification:
//    Contraintes:
// Exception: NON CODE
// Effets de bord:
// Postcondition:
Matrice_Morse_Sym& Matrice_Morse_Sym::operator *=( double x )
{
  scale( x );
  return(*this);
}

void Matrice_Morse_Sym::scale( const double& x )
{
  coeff_ *= x;
}

void Matrice_Morse_Sym::get_stencil( IntTab& stencil ) const
{
  assert_check_symmetric_morse_matrix_structure( );

  IntTab symmetric_stencil;
  get_symmetric_stencil( symmetric_stencil );

  Matrice_Sym::unsymmetrize_stencil( nb_lignes( ),
                                     symmetric_stencil,
                                     stencil );
}

void Matrice_Morse_Sym::get_symmetric_stencil( IntTab& stencil ) const
{
  assert_check_symmetric_morse_matrix_structure( );

  stencil.resize( 0, 2 );
  stencil.set_smart_resize( 1 );

  ArrOfInt tmp;
  tmp.set_smart_resize( 1 );

  const int nb_lines = nb_lignes( );
  for ( int i=0; i<nb_lines; ++i )
    {
      int k0 = tab1_( i ) - 1;
      int k1 = tab1_( i + 1 ) - 1;
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
          stencil.append_line( i, tmp[ k ] );
        }
    }

  const int new_size = stencil.dimension( 0 );
  stencil.set_smart_resize( 0 );
  stencil.resize( new_size, 2 );
}

void Matrice_Morse_Sym::get_stencil_and_coefficients( IntTab&      stencil,
                                                      ArrOfDouble& coefficients ) const
{
  assert_check_symmetric_morse_matrix_structure( );

  IntTab symmetric_stencil;
  ArrOfDouble symmetric_coefficients;
  get_symmetric_stencil_and_coefficients( symmetric_stencil, symmetric_coefficients );

  Matrice_Sym::unsymmetrize_stencil_and_coefficients( nb_lignes( ),
                                                      symmetric_stencil,
                                                      symmetric_coefficients,
                                                      stencil,
                                                      coefficients );
}


void Matrice_Morse_Sym::get_symmetric_stencil_and_coefficients( IntTab&      stencil,
                                                                ArrOfDouble& coefficients ) const
{
  assert_check_symmetric_morse_matrix_structure( );

  stencil.resize( 0, 2 );
  stencil.set_smart_resize( 1 );

  coefficients.resize( 0 );
  coefficients.set_smart_resize( 1 );

  IntTab tmp1(0);
  tmp1.set_smart_resize( 1 );

  ArrOfDouble tmp2;
  tmp2.set_smart_resize( 1 );

  ArrOfInt index;
  index.set_smart_resize( 1 );

  const int nb_lines = nb_lignes( );
  for ( int i=0; i<nb_lines; ++i )
    {
      int k0   = tab1_( i ) - 1;
      int k1   = tab1_( i + 1 ) - 1;
      int size = k1 - k0;

      index.resize_array( 0 );

      tmp1.resize( 0 );
      tmp1.resize( size );

      tmp2.resize_array( 0 );
      tmp2.resize_array( size );

      for ( int k=0; k<size; ++k )
        {
          tmp1( k ) = tab2_( k + k0 ) - 1;
          tmp2[ k ] = coeff_( k + k0 );
        }

      tri_lexicographique_tableau_indirect( tmp1, index );

      for ( int k=0; k<size; ++k )
        {
          int l = index[ k ];
          stencil.append_line( i, tmp1[ l ] );
          coefficients.append_array( tmp2[ l ] );
        }
    }

  const int new_size = stencil.dimension( 0 );
  assert( coefficients.size_array( ) == new_size );

  stencil.set_smart_resize( 0 );
  stencil.resize( new_size, 2 );

  coefficients.set_smart_resize( 0 );
  coefficients.resize_array( new_size );
}

// Description:
//    Operateur de negation unaire,
//    renvoie l'opposee de la matrice:  - A.
//    Appelle operator*(const Matrice_Morse_Sym&,double)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Matrice_Morse_Sym Matrice_Morse_Sym::operator -() const
{
  return((*this)*(-1));
}

static int commun(const ArrOfInt& items,
                  int prems,
                  int der,
                  int val,
                  int& item_courant)
{
  if (val==items[item_courant])
    return item_courant;
  if((val<items[0]) || (val>items[der]))
    return item_courant;
  if (val<items[item_courant])
    {
      --item_courant;
      if(val>=items[item_courant]) return item_courant;
      der=item_courant;
      item_courant=(prems+item_courant)/2;
      return commun(items,prems,der,val,item_courant);
    }
  if (val>items[item_courant])
    {
      ++item_courant;
      if(val<items[item_courant]) return (--item_courant);
      if(val==items[item_courant]) return (item_courant);
      prems=item_courant;
      item_courant=(item_courant+der)/2;
      return commun(items,prems,der,val,item_courant);
    }
  return item_courant;
}


// Description:
//    Operateur d'affectatiob d'une Matrice_Morse_Sym
//    dans une Matrice_Morse_Sym.
// Precondition:
// Parametre: Matrice_Morse_Sym& a
//    Signification: la partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Matrice_Morse_Sym&
//    Signification: le resultat de l'affectation (*this)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Matrice_Morse_Sym& Matrice_Morse_Sym::operator=(const Matrice_Morse_Sym& a )
{
  tab1_  =a.tab1_;
  tab2_  =a.tab2_;
  coeff_ =a.coeff_;
  m_=a.m_;
  set_est_definie(a.get_est_definie());
  morse_matrix_structure_has_changed_=1;
  return(*this);
}

int Matrice_Morse_Sym_test()
{
  return 1;
}


int Matrice_Morse_Sym::inverse(const DoubleVect& secmem, DoubleVect& solution,
                               double coeff_seuil) const
{
  Cerr << "Not coded." << finl;
  exit();
  return 0;
}

int Matrice_Morse_Sym::inverse(const DoubleVect& secmem, DoubleVect& solution,
                               double coeff_seuil, int max_iter) const
{
  Cerr << "Not coded." << finl;
  exit();
  return 0;
}

Sortie& Matrice_Morse_Sym::imprimer_formatte(Sortie& s) const
{
  return Matrice_Morse::imprimer_formatte(s, 1);
}

// Description:
//    Suppression des doublons
//    on ordonne tab2;
//    Verification de la diagonale: tous ses elements doivent etre stockes
// Precondition:
// Parametre:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Matrice_Morse_Sym::compacte(int elim_coeff_nul)
{
  Matrice_Morse::compacte(elim_coeff_nul);
  // Verification si tous les elements de la diagonale sont bien stockes
  int n=nb_lignes();
  int elements_diagonaux_non_stockes=0;
  int size_tab2_ = tab2_.size_array();
  for (int i=0; i<n; i++)
    {
      int k=tab1_(i)-1;
      if (k>=size_tab2_ || i!=tab2_(k)-1)
        {
          elements_diagonaux_non_stockes++;
          // Cerr << "Probleme rencontre dans une Matrice_Morse_Sym:" << finl;
          // Cerr << "La ligne "<<i<<" n'a pas sa diagonale correctement stockee." << finl;
          // Cerr << "Les elements diagonaux meme nuls doivent etre stockes dans une Mat_Morse_Sym TRUST." << finl;
        }
    }
  if (elements_diagonaux_non_stockes)
    {
      // On resize
      int nnz=size_tab2_+elements_diagonaux_non_stockes;
      tab2_.resize(nnz);
      coeff_.resize(nnz);
      // On change la matrice en partant de la fin
      for (int i=n-1; i>=0; i--)
        {
          // On copie en decalant
          int k1=tab1_(i)-1;
          int k2=tab1_(i+1)-1;
          for (int j=k2-1; j>=k1; j--)
            {
              tab2_(j+elements_diagonaux_non_stockes)=tab2_(j);
              coeff_(j+elements_diagonaux_non_stockes)=coeff_(j);
            }
          tab1_(i+1)+=elements_diagonaux_non_stockes;

          if (k1>=size_tab2_ || i!=tab2_(k1)-1)
            {
              // On insere l'element diagonal nul manquant
              elements_diagonaux_non_stockes--;
              tab2_(k1+elements_diagonaux_non_stockes)=i+1;
              coeff_(k1+elements_diagonaux_non_stockes)=0.;
            }
        }
      assert(elements_diagonaux_non_stockes==0);
    }
  morse_matrix_structure_has_changed_=1;
}

// Description:
//   Renumerotation d'une matrice afin de reduire la largeur de bande
// Precondition:
// Parametre:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Matrice_Morse_Sym::renumerote() const
{
  Cerr << "Bandwidth of the matrix : " << largeur_de_bande() << finl;
  Cerr << "Renumbering the matrix ..." << finl;
  // rovisoire chercher a reecrire
  const Matrice_Morse_Sym& matrice_initial = *this;
  ArrOfInt& tab_iperm = matrice_initial.permutation_inverse();

  // transformation d une matrice morse symetrique en matrice morse

  Matrice_Morse matrice2(matrice_initial);
  Matrice_Morse matrice(matrice_initial);

  int mon_ordre = matrice.ordre();
  //int i;

  matrice2.transpose(matrice);
  for (int i=0; i<mon_ordre; i++) matrice2(i, i) = 0.;
  matrice2 += matrice ;


  // Matrice_Bande_Sym matrice_tmp;
  // matrice_tmp.charger_coeff(matrice);
  //Cout<<"avant renumerotation largeur de bande:"<<matrice_tmp.dim(0)<<finl;


  // calcul de la permutation a effectuer

  //  int nnz = matrice2.coeff_.size();
  const int n = mon_ordre;
  const int* tab1tmp = matrice2.tab1();
  const int* tab2tmp = matrice2.tab2();
  // const int nfirst=1;


  int init = 1;


  tab_iperm.resize_array(n);
  tab_iperm[0] = 1;

  int* masktmp  = new int[n];
  for (int i=0 ; i<n; i++ ) masktmp[i] = 1;
  const int* mask = (const int*) masktmp;
  const int maskval = 1;
  // GF passage a n+1 pour permettre de faire du Cholesky sur 1 maillage 1xN
  int*  level = new int[n+1];
  int nlev;

  // renumerotation des noeuds
  // subroutine perphn(n,ja,ia,init,iperm,mask,maskval,nlev,riord,levels)
  // SPARSKIT2/ORDERINGS/levset.f
  F77NAME(PERPHN)(&n, tab2tmp, tab1tmp, &init,  mask, &maskval,
                  &nlev, tab_iperm.addr(), level);


  delete []masktmp;
  delete []level;

  ArrOfInt tab_perm(n);
  for (int i=0 ; i<n; i++ ) tab_perm[tab_iperm[i] - 1] = i + 1;

  matrice2 = matrice;

  const double* a = matrice.coeff();
  const int* ja = matrice.tab2();
  const int* ia = matrice.tab1();
  const double* tao = matrice2.coeff();
  double* ao = (double*)tao;
  const  int* tjao = matrice2.tab2();
  int* jao = (int*)tjao;
  const  int* tiao = matrice2.tab1();
  int* iao = (int*)tiao;

  const int* perm = tab_perm.addr();
  const int* perm_inv = tab_iperm.addr();//normalement inutile
  const int job = 1;


  // permutation de la matrice i.e. calcul de P A tP
  // ici on ne permute que la partie supeieure

  // subroutine dperm (nrow,a,ja,ia,ao,jao,iao,perm,qperm,job)
  // SPARSKIT2/FORMATS/unary.f
  F77NAME(DPERM) (&n, a, ja, ia, ao, jao, iao, perm, perm_inv, &job);

  matrice.transpose(matrice2);
  for (int i=0; i<mon_ordre; i++) matrice2(i, i) = 0.;
  matrice2 += matrice ;
  matrice.partie_sup(matrice2);
  matrice_renumerotee_.typer("Matrice_Morse_Sym");
  ref_cast(Matrice_Morse_Sym,matrice_renumerotee_.valeur()) = matrice;

  // Construction de permutation() rovisoire deja fait avec
  int size=permutation_inverse().size_array();
  permutation().resize_array(size);
  for(int i=0; i<size; i++)
    permutation()[permutation_inverse()[i]-1]=i+1;
  Cerr << "Bandwidth of the renumbered matrix : " << ref_cast(Matrice_Morse,matrice_renumerotee_.valeur()).largeur_de_bande() << finl;
  morse_matrix_structure_has_changed_=1;
}

bool Matrice_Morse_Sym::check_symmetric_morse_matrix_structure( void ) const
{
  if ( ! ( check_morse_matrix_structure( ) ) )
    {
      Cerr << "Invalid morse structure" << finl;
      return false;
    }

  const int nb_lines = nb_lignes( );
  for ( int i=0; i<nb_lines; ++i )
    {
      int k0 = tab1_( i ) - 1;
      int k1 = tab1_( i + 1 ) - 1;

      for ( int k=k0; k<k1; ++k )
        {
          int j = tab2_( k ) - 1;
          if  ( j < i )
            {
              Cerr << "( j < i ) : line index : " << i << " and column index " << j << finl;
              return false;
            }
        }
    }
  return true;
}

bool Matrice_Morse_Sym::check_sorted_symmetric_morse_matrix_structure( void ) const
{
  if ( ! ( check_sorted_morse_matrix_structure( ) ) )
    {
      return false;
    }

  const int nb_lines = nb_lignes( );
  for ( int i=0; i<nb_lines; ++i )
    {
      int k0 = tab1_( i ) - 1;
      int k1 = tab1_( i + 1 ) - 1;

      for ( int k=k0; k<k1; ++k )
        {
          int j = tab2_( k ) - 1;
          if  ( j < i )
            {
              Cerr << "( j < i ) : line index : " << i << " and column index " << j << finl;
              return false;
            }
        }
    }
  return true;
}


void Matrice_Morse_Sym::assert_check_symmetric_morse_matrix_structure( void ) const
{
  if (!morse_matrix_structure_has_changed_) return;
#ifndef NDEBUG
  if ( ! ( check_symmetric_morse_matrix_structure( ) ) )
    {
      Cerr << "Error in 'Matrice_Morse_Sym::assert_check_symmetric_morse_matrix_structure( )':" << finl;
      Cerr << "  Exiting..." << finl;
      Process::exit( );
    }
  else
    morse_matrix_structure_has_changed_=0;
#endif
}

void Matrice_Morse_Sym::assert_check_sorted_symmetric_morse_matrix_structure( void ) const
{
  if (!morse_matrix_structure_has_changed_) return;
#ifndef NDEBUG
  if ( ! ( check_symmetric_morse_matrix_structure( ) ) )
    {
      Cerr << "Error in 'Matrice_Morse_Sym::assert_check_sorted_symmetric_morse_matrix_structure( )':" << finl;
      Cerr << "  Exiting..." << finl;
      Process::exit( );
    }
  else
    morse_matrix_structure_has_changed_=0;
#endif
}
