/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef TRUSTTab_included
#define TRUSTTab_included

#include <MD_Vector_base.h>
#include <TRUSTVect.h>
#include <math.h>

#include <View_Types.h>  // Kokkos stuff

/*! @brief : Tableau a n entrees pour n<= 4.
 *
 * Repose sur un TRUSTVect avec calculs de l'indice corespondant
 *
 */
template<typename _TYPE_, typename _SIZE_>
class TRUSTTab : public TRUSTVect<_TYPE_,_SIZE_>
{
protected:
  inline unsigned taille_memoire() const override { throw; }

  inline int duplique() const override
  {
    TRUSTTab* xxx = new  TRUSTTab(*this);
    if(!xxx) Process::exit("Not enough memory ");
    return xxx->numero();
  }

  /*! @brief ecriture d'un tableau sequentiel (idem que TRUSTVect::printOn() on ne sait pas quoi faire de pertinent pour un tableau distribue).
   *
   */
  Sortie& printOn(Sortie& os) const override
  {
#ifndef LATATOOLS
    assert(verifie_LINE_SIZE());
    if (TRUSTVect<_TYPE_,_SIZE_>::nproc() > 1 && TRUSTVect<_TYPE_,_SIZE_>::get_md_vector().non_nul())
      Process::exit("Error in TRUSTTab::printOn: try to print a parallel vector");
    os << nb_dim_ << finl;
    assert(dimensions_[0] == dimension_tot_0_);
    if (nb_dim_ > 0) os.put(dimensions_, nb_dim_, nb_dim_);

    const _SIZE_ sz = TRUSTVect<_TYPE_,_SIZE_>::size_array();
    os << sz << finl;
    const int l_size = TRUSTVect<_TYPE_,_SIZE_>::line_size();
    const _TYPE_ *data = TRUSTVect<_TYPE_,_SIZE_>::addr();

    if (sz > 0)  os.put(data, sz, l_size);
#endif
    return os;
  }

  /*! @brief lecture d'un tableau sequentiel
   *
   */
  Entree& readOn(Entree& is) override
  {
#ifndef LATATOOLS
    // Que veut-on faire si on lit dans un vecteur ayant deja une structure parallele ?
    if (TRUSTVect<_TYPE_,_SIZE_>::get_md_vector().non_nul())
      Process::exit("Error in TRUSTTab::readOn: vector has a parallel structure");

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
        l_size *= (int)dimensions_[i];
      }
    dimension_tot_0_ = dimensions_[0];
    TRUSTVect<_TYPE_,_SIZE_>::readOn(is);
    TRUSTVect<_TYPE_,_SIZE_>::set_line_size_(l_size);
    if (dimension_tot_0_ * l_size != TRUSTVect<_TYPE_,_SIZE_>::size_array())
      {
        Cerr << "Error in TRUSTTab::readOn: wrong size_array " << TRUSTVect<_TYPE_,_SIZE_>::size_array() << ", expected " << dimension_tot_0_ * l_size << finl;
        Process::exit();
      }
    assert(verifie_LINE_SIZE());
#endif
    return is;
  }

public:
  // Useful for from_tid_to_int() method implementation.
  friend TRUSTTab<int, int>;

  TRUSTTab() : nb_dim_(1), dimension_tot_0_(0)
  {
    init_dimensions(dimensions_);
    dimensions_[0] = 0;
  }

  TRUSTTab(const TRUSTTab& dbt): TRUSTVect<_TYPE_,_SIZE_>(dbt), nb_dim_(dbt.nb_dim_), dimension_tot_0_(dbt.dimension_tot_0_)
  {
    for (int i = 0; i < MAXDIM_TAB; i++) dimensions_[i] = dbt.dimensions_[i];
  }

  TRUSTTab(_SIZE_ n) : TRUSTVect<_TYPE_,_SIZE_>(n), nb_dim_(1), dimension_tot_0_(n)
  {
    init_dimensions(dimensions_);
    dimensions_[0] = n;
  }

  TRUSTTab(_SIZE_ n1, int n2): TRUSTVect<_TYPE_,_SIZE_>(n1*n2), nb_dim_(2), dimension_tot_0_(n1)
  {
    assert(n1 >= 0 && n2 >= 0);
    if (std::is_same<_TYPE_,int>::value && (long)n1*(long)n2 > (long)std::numeric_limits<int>::max())
      {
        Cerr << "n1*n2 > 2^31. Error! Contact TRUST support, integer 32 bits limit exceeded with n1=" << n1 << " and n2=" << n2 << finl;
        Process::exit();
      }
    init_dimensions(dimensions_);
    dimensions_[0]=n1;
    dimensions_[1]=n2;
    TRUSTVect<_TYPE_,_SIZE_>::set_line_size_(n2);
  }

  TRUSTTab(_SIZE_ n1, int n2, int n3) : TRUSTVect<_TYPE_,_SIZE_>(n1*n2*n3), nb_dim_(3), dimension_tot_0_(n1)
  {
    assert(n1 >= 0 && n2 >= 0 && n3 >= 0);
    if (std::is_same<_TYPE_,int>::value && (long)n1*(long)n2*(long)n3 > (long)std::numeric_limits<int>::max())
      {
        Cerr << "n1*n2*n3 > 2^31. Error! Contact TRUST support, integer 32 bits limit exceeded with n1=" << n1 << " and n2=" << n2 << " and n3=" << n3 << finl;
        Process::exit();
      }
    init_dimensions(dimensions_);
    dimensions_[0]=n1;
    dimensions_[1]=n2;
    dimensions_[2]=n3;
    TRUSTVect<_TYPE_,_SIZE_>::set_line_size_(n2*n3);
  }

  TRUSTTab(_SIZE_ n1, int n2, int n3, int n4) : TRUSTVect<_TYPE_,_SIZE_>(n1*n2*n3*n4), nb_dim_(4), dimension_tot_0_(n1)
  {
    assert(n1 >= 0 && n2 >= 0 && n3 >= 0 && n4 >= 0);
    if (std::is_same<_TYPE_,int>::value && (long)n1*(long)n2*(long)n3*(long)n4 > (long)std::numeric_limits<int>::max())
      {
        Cerr << "n1*n2*n3*n4 > 2^31. Error! Contact TRUST support, integer 32 bits limit exceeded with n1=" << n1 << " and n2=" << n2 << " and n3=" << n3 << " and n3=" << n3 << finl;
        Process::exit();
      }
    init_dimensions(dimensions_);
    dimensions_[0]=n1;
    dimensions_[1]=n2;
    dimensions_[2]=n3;
    dimensions_[3]=n4;
    TRUSTVect<_TYPE_,_SIZE_>::set_line_size_(n2*n3*n4);
  }

  inline void resize_dim0(_SIZE_ n, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);
  inline void resize(_SIZE_ n, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);
  inline void resize(_SIZE_ n1, int n2, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);
  inline void resize(_SIZE_ n1, int n2, int n3, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);
  inline void resize(_SIZE_ n1, int n2, int n3, int n4, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);
  inline void resize(const TRUSTArray<_SIZE_,int>& tailles, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);
  inline void copy(const TRUSTTab&, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);
  inline void append_line(_TYPE_);
  inline void append_line(_TYPE_, _TYPE_);
  inline void append_line(_TYPE_, _TYPE_, _TYPE_);
  inline void append_line(_TYPE_, _TYPE_, _TYPE_, _TYPE_);

  // See doc in .tpp file!
  _SIZE_ dimension(int d) const;
  int dimension_int(int d) const;

  inline int nb_dim() const { return nb_dim_; }

  // See same method in TRUSTArray - CAREFUL, this is not an override because arg types are different (tab vs arr)
  inline void from_tid_to_int(TRUSTTab<int, int>& out) const;

  inline TRUSTTab& operator=(const TRUSTTab&);
  inline TRUSTTab& operator=(const TRUSTVect<_TYPE_,_SIZE_>&);
  inline TRUSTTab& operator=(_TYPE_ d);
  inline _TYPE_& operator()(const TRUSTArray<_SIZE_,int>& indice);
  inline _TYPE_ operator()(const TRUSTArray<_SIZE_,int>& indice) const;
  inline _TYPE_& operator[](_SIZE_ i);
  inline const _TYPE_& operator[](_SIZE_ i) const ;
  inline _TYPE_& operator()(_SIZE_ i);
  inline const _TYPE_& operator()(_SIZE_ i) const ;
  inline _TYPE_& operator()(_SIZE_ i1, int i2);
  inline const _TYPE_& operator()(_SIZE_ i1, int i2) const ;
  inline _TYPE_& operator()(_SIZE_ i1, int i2, int i3);
  inline const _TYPE_& operator()(_SIZE_ i1, int i2, int i3) const ;
  inline _TYPE_& operator()(_SIZE_ i1, int i2, int i3, int i4);
  inline const _TYPE_& operator()(_SIZE_ i1, int i2, int i3, int i4) const ;

  // Juste pour TRUSTTab<double/float>
  template <typename _T_> inline void ajoute_produit_tensoriel(_T_ alpha, const TRUSTTab<_T_,_SIZE_>&, const TRUSTTab<_T_,_SIZE_>&); // z+=alpha*x*y;
  template <typename _T_> inline void resoud_LU(_SIZE_, TRUSTArray<int,_SIZE_>&, const TRUSTArray<_T_,_SIZE_>&, TRUSTArray<_T_,_SIZE_>&);
  template <typename _T_> inline bool inverse_LU(const TRUSTArray<_T_,_SIZE_>&, TRUSTArray<_T_,_SIZE_>&);
  template <typename _T_> inline bool decomp_LU(_SIZE_, TRUSTArray<int,_SIZE_>&, TRUSTTab<_T_,_SIZE_>&);
  template <typename _T_> inline _T_ max_du_u(const TRUSTTab<_T_,_SIZE_>&);

  void ajoute_produit_tensoriel(int alpha, const TRUSTTab<int,_SIZE_>&, const TRUSTTab<int,_SIZE_>&) = delete; // z+=alpha*x*y;
  void resoud_LU(int, TRUSTArray<int,_SIZE_>&, const TRUSTArray<int,_SIZE_>&, TRUSTArray<int,_SIZE_>&) = delete;
  int inverse_LU(const TRUSTArray<int,_SIZE_>&, TRUSTArray<int,_SIZE_>&) = delete;
  int decomp_LU(int, TRUSTArray<int,_SIZE_>&, TRUSTTab<int,_SIZE_>&) = delete;
  int max_du_u(const TRUSTTab<int,_SIZE_>&) = delete;

  // methodes virtuelles
  inline _SIZE_ dimension_tot(int) const override;
  inline virtual void ref(const TRUSTTab&);
  inline virtual void ref_tab(TRUSTTab&, _SIZE_ start_line=0, _SIZE_ nb_lines=-1);
  inline void set_md_vector(const MD_Vector&) override;
  inline void jump(Entree&) override;
  inline void lit(Entree&, bool resize_and_read=true) override;
  inline void ecrit(Sortie&) const override;
  inline void ref(const TRUSTVect<_TYPE_,_SIZE_>&) override;
  inline void ref_data(_TYPE_* ptr, _SIZE_ size) override;
  inline void ref_array(TRUSTArray<_TYPE_,_SIZE_>&, _SIZE_ start=0, _SIZE_ sz=-1) override;
  inline void reset() override;
  inline void resize_tab(_SIZE_ n, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT) override;

#ifdef KOKKOS

  //Overriden accessors from TRUSTArray. The default shape is 2 for tabs, then calls the Vect accessor

  // Read-only
  template <int SHAPE = 2, typename EXEC_SPACE = Kokkos::DefaultExecutionSpace>
  inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ConstView<_TYPE_, SHAPE>>
                                                                                    view_ro() const
  {
    return TRUSTVect<_TYPE_, _SIZE_>::template view_ro<SHAPE, EXEC_SPACE>();
  }

  template <int SHAPE = 2, typename EXEC_SPACE = Kokkos::DefaultExecutionSpace>
  inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, ConstHostView<_TYPE_, SHAPE>>
                                                                                     view_ro() const
  {
    return TRUSTVect<_TYPE_, _SIZE_>::template view_ro<SHAPE, EXEC_SPACE>();
  }

  // Write-only
  template <int SHAPE = 2, typename EXEC_SPACE = Kokkos::DefaultExecutionSpace>
  inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, View<_TYPE_, SHAPE>>
                                                                               view_wo()
  {
    return TRUSTVect<_TYPE_, _SIZE_>::template view_wo<SHAPE, EXEC_SPACE>();
  }

  template <int SHAPE = 2, typename EXEC_SPACE = Kokkos::DefaultExecutionSpace>
  inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, HostView<_TYPE_, SHAPE>>
                                                                                view_wo()
  {
    return TRUSTVect<_TYPE_, _SIZE_>::template view_wo<SHAPE, EXEC_SPACE>();
  }

  // Read-write
  template <int SHAPE = 2, typename EXEC_SPACE = Kokkos::DefaultExecutionSpace>
  inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, View<_TYPE_, SHAPE>>
                                                                               view_rw()
  {
    return TRUSTVect<_TYPE_, _SIZE_>::template view_rw<SHAPE, EXEC_SPACE>();
  }

  template <int SHAPE = 2, typename EXEC_SPACE = Kokkos::DefaultExecutionSpace>
  inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, HostView<_TYPE_, SHAPE>>
                                                                                view_rw()
  {
    return TRUSTVect<_TYPE_, _SIZE_>::template view_rw<SHAPE, EXEC_SPACE>();
  }

#endif
private:
  static constexpr int MAXDIM_TAB = 4;
  // Nombre de dimensions du tableau (nb_dim_>=1)
  int nb_dim_; //Now Also in TrustArray

  /*! Dimensions "reelles" (dimensions_[0] * line_size() = size_reelle()) : line_size() est egal au produit des dimensions_[i] pour 1 <= i < nb_dim_
   *  Everything is stored as _SIZE_ but higher dims (>=1) should fit in an int. See line_size().
   */
  _SIZE_ dimensions_[MAXDIM_TAB];

  // Dimension totale (nombre de lignes du tableau) = nb lignes reeles + nb lignes virtuelles
  // Les dimensions dimension_tot(i>=1) sont implicitement egales a dimension(i)
  _SIZE_ dimension_tot_0_;

  inline void verifie_MAXDIM_TAB() const
  {
    if (MAXDIM_TAB != 4)
      {
        Cerr << "Update code for MAXDIM_TAB for CHECK_LINE_SIZE" << finl;
        throw;
      }
  }

  inline bool verifie_LINE_SIZE() const
  {
    return ((TRUSTVect<_TYPE_,_SIZE_>::line_size() == ((nb_dim_ > 1) ? dimensions_[1] : 1) * ((nb_dim_ > 2) ? dimensions_[2] : 1) * ((nb_dim_ > 3) ? dimensions_[3] : 1))
            && (TRUSTVect<_TYPE_,_SIZE_>::line_size() * dimension_tot_0_ == TRUSTVect<_TYPE_,_SIZE_>::size_array()));
  }

  inline void init_dimensions(_SIZE_ * tab)
  {
#ifndef NDEBUG
    for (int i = 0; i < MAXDIM_TAB; i++) tab[i] = -1;
#endif
  }
};

using DoubleTab = TRUSTTab<double, int>;
using FloatTab = TRUSTTab<float, int>;
using IntTab = TRUSTTab<int, int>;
using TIDTab = TRUSTTab<trustIdType, int>;

template <typename _TYPE_>
using BigTRUSTTab = TRUSTTab<_TYPE_, trustIdType>;

using BigDoubleTab = BigTRUSTTab<double>;
using BigIntTab = BigTRUSTTab<int>;
using BigTIDTab = BigTRUSTTab<trustIdType>;

/* ********************************* *
 * FONCTIONS NON MEMBRES DE TRUSTTab *
 * ********************************* */

#include <TRUSTTab_tools.tpp> // external templates function specializations ici ;)

/* ***************************** *
 * FONCTIONS MEMBRES DE TRUSTTab *
 * ***************************** */

#include <TRUSTTab.tpp> // The rest here!

#endif /* TRUSTTab_included */
