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
//////////////////////////////////////////////////////////////////////////////
//
// File:        TRUSTTab.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTTab_included
#define TRUSTTab_included

#include <MD_Vector_base.h>
#include <TRUSTVect.h>
#include <math.h>

// TODO : FIXME : constexpr !
#ifndef MAXDIM_TAB
#define MAXDIM_TAB 4
#endif

// Verifie la coherence entre size_array(), line_size() et les dimensions du tableau
#if MAXDIM_TAB != 4
#error Mettre a jour le code pour MAXDIM_TAB pour CHECK_LINE_SIZE
#endif

#define CHECK_LINE_SIZE                                                        \
  (((this)->template line_size() == ((nb_dim_>1)?dimensions_[1]:1)*((nb_dim_>2)?dimensions_[2]:1)*((nb_dim_>3)?dimensions_[3]:1)) \
   && ((this)->template line_size() * dimension_tot_0_ == (this)->template size_array()))

template<typename _TYPE_>
class TRUSTTab : public TRUSTVect<_TYPE_>
{
protected:
  inline unsigned taille_memoire() const override { throw; }

  inline int duplique() const override
  {
    TRUSTTab* xxx = new  TRUSTTab(*this);
    if(!xxx)
      {
        Cerr << "Not enough memory " << finl;
        Process::exit();
      }
    return xxx->numero();
  }

  // Description: ecriture d'un tableau sequentiel (idem que Int/DoubleVect::printOn() on ne sait pas quoi faire de pertinent pour un tableau distribue).
  Sortie& printOn(Sortie& os) const override
  {
    assert(CHECK_LINE_SIZE);
    if (TRUSTVect<_TYPE_>::nproc() > 1 && TRUSTVect<_TYPE_>::get_md_vector().non_nul())
      {
        Cerr << "Error in TRUSTTab::printOn: try to print a parallel vector" << finl;
        Process::exit();
      }
    os << nb_dim_ << finl;
    assert(dimensions_[0] == dimension_tot_0_);
    if (nb_dim_ > 0) os.put(dimensions_, nb_dim_, nb_dim_);

    const int sz = (this)->template size_array();
    os << sz << finl;
    const int l_size = (this)->template line_size();
    const _TYPE_ *data = (this)->template addr();

    if (sz > 0)  os.put(data, sz, l_size);
    return os;
  }

  // Description: lecture d'un tableau sequentiel
  // Precondition: le md_vector_ doit etre nul.
  Entree& readOn(Entree& is) override
  {
    if (TRUSTVect<_TYPE_>::get_md_vector().non_nul())
      {
        // Que veut-on faire si on lit dans un vecteur ayant deja une structure parallele ?
        Cerr << "Error in TRUSTTab::readOn: vector has a parallel structure" << finl;
        Process::exit();
      }

    is >> nb_dim_;
    if (nb_dim_ < 1 || nb_dim_ > MAXDIM_TAB)
      {
        Cerr << "Error in TRUSTTab::readOn: wrong nb_dim_ = " << nb_dim_ << finl;
        Process::exit();
      }
    is.get(dimensions_, nb_dim_);
    if (dimensions_[0] < 0)
      {
        Cerr << "Error in TRUSTTab::readOn: wrong dimension(0) = " << dimensions_[0] << finl;
        Process::exit();
      }
    int l_size = 1;
    for (int i = 1; i < nb_dim_; i++)
      {
        if (dimensions_[i] < 0)
          {
            Cerr << "Error in TRUSTTab::readOn: wrong dimension(" << i << ") = " << dimensions_[i] << finl;
            Process::exit();
          }
        l_size *= dimensions_[i];
      }
    dimension_tot_0_ = dimensions_[0];
    TRUSTVect<_TYPE_>::readOn(is);
    (this)->template set_line_size_(l_size);
    if (dimension_tot_0_ * l_size != (this)->template size_array())
      {
        Cerr << "Error in TRUSTTab::readOn: wrong size_array " << (this)->template size_array() << ", expected " << dimension_tot_0_ * l_size << finl;
        Process::exit();
      }
    assert(CHECK_LINE_SIZE);
    return is;
  }

public:

  TRUSTTab() : nb_dim_(1), dimension_tot_0_(0)
  {
    init_dimensions(dimensions_);
    dimensions_[0] = 0;
  }

  TRUSTTab(const TRUSTTab& dbt): TRUSTVect<_TYPE_>(dbt), nb_dim_(dbt.nb_dim_), dimension_tot_0_(dbt.dimension_tot_0_)
  {
    for (int i = 0; i < MAXDIM_TAB; i++) dimensions_[i] = dbt.dimensions_[i];
  }

#ifdef INT_is_64_
  TRUSTTab(int n1, True_int n2) : IntVect(n1*n2), nb_dim_(2), dimension_tot_0_(n1)
  {
    assert(n1 >= 0 && n2 >= 0);
    if (std::is_same<_TYPE_,int>::value && n1*n2 < 0)
      {
        Cerr << "n1*n2 > 2^31. Error! Contact TRUST support, integer 32 bits limit exceeded with n1=" << n1 << " and n2=" << (int)n2 << finl;
        Process::exit();
      }
    init_dimensions(dimensions_);
    dimensions_[0]=n1;
    dimensions_[1]=n2;
    (this)->template set_line_size_(n2);
  }
#endif

  TRUSTTab(int n) : TRUSTVect<_TYPE_>(n), nb_dim_(1), dimension_tot_0_(n)
  {
    init_dimensions(dimensions_);
    dimensions_[0] = n;
  }

  TRUSTTab(int n1, int n2): TRUSTVect<_TYPE_>(n1*n2), nb_dim_(2), dimension_tot_0_(n1)
  {
    assert(n1 >= 0 && n2 >= 0);
    if (std::is_same<_TYPE_,int>::value && n1*n2 < 0)
      {
        Cerr << "n1*n2 > 2^31. Error! Contact TRUST support, integer 32 bits limit exceeded with n1=" << n1 << " and n2=" << n2 << finl;
        Process::exit();
      }
    init_dimensions(dimensions_);
    dimensions_[0]=n1;
    dimensions_[1]=n2;
    (this)->template set_line_size_(n2);
  }

  TRUSTTab(int n1, int n2, int n3) : TRUSTVect<_TYPE_>(n1*n2*n3), nb_dim_(3), dimension_tot_0_(n1)
  {
    assert(n1 >= 0 && n2 >= 0 && n3 >= 0);
    if (std::is_same<_TYPE_,int>::value && n1*n2*n3 < 0)
      {
        Cerr << "n1*n2*n3 > 2^31. Error! Contact TRUST support, integer 32 bits limit exceeded with n1=" << n1 << " and n2=" << n2 << " and n3=" << n3 << finl;
        Process::exit();
      }
    init_dimensions(dimensions_);
    dimensions_[0]=n1;
    dimensions_[1]=n2;
    dimensions_[2]=n3;
    (this)->template set_line_size_(n2*n3);
  }

  TRUSTTab(int n1, int n2, int n3, int n4) : TRUSTVect<_TYPE_>(n1*n2*n3*n4), nb_dim_(4), dimension_tot_0_(n1)
  {
    assert(n1 >= 0 && n2 >= 0 && n3 >= 0 && n4 >= 0);
    if (std::is_same<_TYPE_,int>::value && n1*n2*n3*n4 < 0)
      {
        Cerr << "n1*n2*n3*n4 > 2^31. Error! Contact TRUST support, integer 32 bits limit exceeded with n1=" << n1 << " and n2=" << n2 << " and n3=" << n3 << " and n3=" << n3 << finl;
        Process::exit();
      }
    init_dimensions(dimensions_);
    dimensions_[0]=n1;
    dimensions_[1]=n2;
    dimensions_[2]=n3;
    dimensions_[3]=n4;
    (this)->template set_line_size_(n2*n3*n4);
  }

  inline void resize_dim0(int n, Array_base::Resize_Options opt = Array_base::COPY_INIT);
  inline void resize(int n, Array_base::Resize_Options opt = Array_base::COPY_INIT);
  inline void resize(int n1, int n2, Array_base::Resize_Options opt = Array_base::COPY_INIT);
  inline void resize(int n1, int n2, int n3, Array_base::Resize_Options opt = Array_base::COPY_INIT);
  inline void resize(int n1, int n2, int n3, int n4, Array_base::Resize_Options opt = Array_base::COPY_INIT);
  inline void resize(const TRUSTArray<int>& tailles, Array_base::Resize_Options opt = Array_base::COPY_INIT);
  inline void copy(const TRUSTTab&, Array_base::Resize_Options opt = Array_base::COPY_INIT);
  inline void append_line(_TYPE_);
  inline void append_line(_TYPE_, _TYPE_);
  inline void append_line(_TYPE_, _TYPE_, _TYPE_);
  inline void append_line(_TYPE_, _TYPE_, _TYPE_, _TYPE_);
  inline int dimension(int) const;
  inline int dimension_tot(int) const;
  inline int nb_dim() const { return nb_dim_; }

  inline TRUSTTab& operator=(const TRUSTTab&);
  inline TRUSTTab& operator=(const TRUSTVect<_TYPE_>&);
  inline TRUSTTab& operator=(_TYPE_ d);
  inline _TYPE_& operator()(const TRUSTArray<int>& indice);
  inline _TYPE_ operator()(const TRUSTArray<int>& indice) const;
  inline _TYPE_& operator[](int i);
  inline const _TYPE_& operator[](int i) const ;
  inline _TYPE_& operator()(int i);
  inline const _TYPE_& operator()(int i) const ;
  inline _TYPE_& operator()(int i1, int i2);
  inline const _TYPE_& operator()(int i1, int i2) const ;
  inline _TYPE_& operator()(int i1, int i2, int i3);
  inline const _TYPE_& operator()(int i1, int i2, int i3) const ;
  inline _TYPE_& operator()(int i1, int i2, int i3, int i4);
  inline const _TYPE_& operator()(int i1, int i2, int i3, int i4) const ;

  // ------------------------------------------------------
  // TODO : FIXME : juste pour double
  inline void ajoute_produit_tensoriel(double alpha, const TRUSTTab<double>&, const TRUSTTab<double>&); // z+=alpha*x*y;
  inline void resoud_LU(int, TRUSTArray<int>&, const TRUSTArray<double>&, TRUSTArray<double>&);
  inline int inverse_LU(const TRUSTArray<double>&, TRUSTArray<double>&);
  inline int decomp_LU(int, TRUSTArray<int>&, TRUSTTab<double>&);
  inline double max_du_u(const TRUSTTab<double>&);
  // TODO : FIXME : juste pour double
  // ------------------------------------------------------

  // methodes virtuelles
  inline virtual void ref(const TRUSTTab&);
  inline virtual void ref_tab(TRUSTTab&, int start_line = 0, int nb_lines = -1);
  inline void set_md_vector(const MD_Vector&) override;
  inline void jump(Entree&) override;
  inline void lit(Entree&, int resize_and_read=1) override;
  inline void ecrit(Sortie&) const override;
  inline void ref(const TRUSTVect<_TYPE_>&) override;
  inline void ref_data(_TYPE_* ptr, int size) override;
  inline void ref_array(TRUSTArray<_TYPE_>&, int start = 0, int sz = -1) override;
  inline void reset() override;
  inline void resize_tab(int n, Array_base::Resize_Options opt = Array_base::COPY_INIT) override;

private:
  // Nombre de dimensions du tableau (nb_dim_>=1)
  int nb_dim_;

  // Dimensions "reelles" (dimensions_[0] * line_size() = size_reelle()) : line_size() est egal au produit des dimensions_[i] pour 1 <= i < nb_dim_
  int dimensions_[MAXDIM_TAB];

  // Dimension totale (nombre de lignes du tableau) = nb lignes reeles + nb lignes virtuelles
  // Les dimensions dimension_tot(i>=1) sont implicitement egales a dimension(i)
  int dimension_tot_0_;

  inline void init_dimensions(int * tab)
  {
#ifndef NDEBUG
    for (int i = 0; i < MAXDIM_TAB; i++) tab[i] = -1;
#endif
  }
};

using IntTab = TRUSTTab<int>;
using DoubleTab = TRUSTTab<double>;

/* ********************************* *
 * FONCTIONS NON MEMBRES DE TRUSTTab *
 * ********************************* */

#include <TRUSTTab_tools.tpp> // external templates function specializations ici ;)

/* ***************************** *
 * FONCTIONS MEMBRES DE TRUSTTab *
 * ***************************** */

#include <TRUSTTab.tpp> // templates specializations ici ;)

#endif /* TRUSTTab_included */
