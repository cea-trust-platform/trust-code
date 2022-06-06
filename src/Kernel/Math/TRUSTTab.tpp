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

#ifndef TRUSTTab_TPP_included
#define TRUSTTab_TPP_included

// TODO : FIXME : delete
template<typename _TYPE_>
inline _TYPE_& TRUSTTab<_TYPE_>::operator[](int i)
{
  assert(nb_dim_ == 1 || (nb_dim_ == 2 && dimensions_[1] == 1));
  assert(i >= 0 && i < dimension_tot_0_);
  return TRUSTVect<_TYPE_>::operator[](i);
}

// TODO : FIXME : delete
template<typename _TYPE_>
inline const _TYPE_& TRUSTTab<_TYPE_>::operator[](int i) const
{
  assert(nb_dim_ == 1 || (nb_dim_ == 2 && dimensions_[1] == 1));
  assert(i >= 0 && i < dimension_tot_0_);
  return TRUSTVect<_TYPE_>::operator[](i);
}

template<typename _TYPE_>
inline _TYPE_& TRUSTTab<_TYPE_>::operator()(int i)
{
  assert(nb_dim_ == 1 || (nb_dim_ == 2 && dimensions_[1] == 1));
  assert(i >= 0 && i < dimension_tot_0_);
  return TRUSTVect<_TYPE_>::operator[](i);
}

template<typename _TYPE_>
inline const _TYPE_& TRUSTTab<_TYPE_>::operator()(int i) const
{
  assert(nb_dim_ == 1 || (nb_dim_ == 2 && dimensions_[1] == 1));
  assert(i >= 0 && i < dimension_tot_0_);
  return TRUSTVect<_TYPE_>::operator[](i);
}

#pragma GCC diagnostic ignored "-Wstrict-overflow"
template<typename _TYPE_>
inline _TYPE_& TRUSTTab<_TYPE_>::operator()(int i1, int i2)
{
  assert(nb_dim_ == 2);
  assert(i1 >= 0 && i1 < dimension_tot_0_);
  assert(i2 >= 0 && i2 < dimensions_[1]);
  return TRUSTVect<_TYPE_>::operator[](i1*dimensions_[1]+i2);
}

template<typename _TYPE_>
inline const _TYPE_& TRUSTTab<_TYPE_>::operator()(int i1, int i2) const
{
  assert(nb_dim_ == 2);
  assert(i1 >= 0 && i1 < dimension_tot_0_);
  assert(i2 >= 0 && i2 < dimensions_[1]);
  return TRUSTVect<_TYPE_>::operator[](i1*dimensions_[1]+i2);
}

template<typename _TYPE_>
inline _TYPE_& TRUSTTab<_TYPE_>::operator()(int i1, int i2, int i3)
{
  assert(nb_dim_ == 3);
  assert(i1 >= 0 && i1 < dimension_tot_0_);
  assert(i2 >= 0 && i2 < dimensions_[1]);
  assert(i3 >= 0 && i3 < dimensions_[2]);
  return TRUSTVect<_TYPE_>::operator[]((i1*dimensions_[1]+i2)*dimensions_[2]+i3);
}

template<typename _TYPE_>
inline const _TYPE_& TRUSTTab<_TYPE_>::operator()(int i1, int i2, int i3) const
{
  assert(nb_dim_ == 3);
  assert(i1 >= 0 && i1 < dimension_tot_0_);
  assert(i2 >= 0 && i2 < dimensions_[1]);
  assert(i3 >= 0 && i3 < dimensions_[2]);
  return TRUSTVect<_TYPE_>::operator[]((i1*dimensions_[1]+i2)*dimensions_[2]+i3);
}

template<typename _TYPE_>
inline _TYPE_& TRUSTTab<_TYPE_>::operator()(int i1, int i2, int i3, int i4)
{
  assert(nb_dim_ == 4);
  assert(i1 >= 0 && i1 < dimension_tot_0_);
  assert(i2 >= 0 && i2 < dimensions_[1]);
  assert(i3 >= 0 && i3 < dimensions_[2]);
  assert(i4 >= 0 && i4 < dimensions_[3]);
  return TRUSTVect<_TYPE_>::operator[](((i1*dimensions_[1]+i2)*dimensions_[2]+i3)*dimensions_[3]+i4);
}

template<typename _TYPE_>
inline const _TYPE_& TRUSTTab<_TYPE_>::operator()(int i1, int i2, int i3, int i4) const
{
  assert(nb_dim_ == 4);
  assert(i1 >= 0 && i1 < dimension_tot_0_);
  assert(i2 >= 0 && i2 < dimensions_[1]);
  assert(i3 >= 0 && i3 < dimensions_[2]);
  assert(i4 >= 0 && i4 < dimensions_[3]);
  return TRUSTVect<_TYPE_>::operator[](((i1*dimensions_[1]+i2)*dimensions_[2]+i3)*dimensions_[3]+i4);
}

//  Returns one of the "real" dimensions of the multi-dimensionnal array, as defined by:
//   dimension(0) = size_reelle() / line_size(), or 0 if line_size()==0
//  and, for i >= 1 : dimension(i) is equal to dimension_tot(i)
//  If TRUSTVect<_TYPE_>::size_reelle_ok() returns 0, it is invalid to ask for dimension(0). You can only ask for dimension_tot(0) (see TRUSTVect<_TYPE_>::size_reelle_ok())
template<typename _TYPE_>
inline int TRUSTTab<_TYPE_>::dimension(int i) const
{
  assert(i >= 0 && i < nb_dim_);
  // Si dimension_[0] == -1, c'est que c'est un vecteur distribue et que l'attribut size() est invalide. Il faut alors utiliser dimension_tot pour ce tableau.
  assert(dimensions_[i] >= 0);
  return dimensions_[i];
}

//  Returns the total dimensions of the multi-dimensionnal array, including virtual items (used in parallel distributed arrays)
template<typename _TYPE_>
inline int TRUSTTab<_TYPE_>::dimension_tot(int i) const
{
  assert(i >= 0 && i < nb_dim_);
  return (i == 0) ? dimension_tot_0_ : dimensions_[i];
}

//  Adds 1 to dimension_tot(0) and puts a in the added line.
// Precondition: line_size() must be equal to 1 and the array must be resizable.
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::append_line(_TYPE_ a)
{
  assert(TRUSTVect<_TYPE_>::line_size() == 1);
  assert(dimension_tot_0_ * TRUSTVect<_TYPE_>::line_size() == TRUSTVect<_TYPE_>::size_array());
  const int n = dimension_tot_0_;
  dimensions_[0] = ++dimension_tot_0_;
  TRUSTVect<_TYPE_>::resize_vect_(n+1, Array_base::COPY_NOINIT);
  _TYPE_ * ptr = TRUSTVect<_TYPE_>::addr() + n;
  ptr[0] = a;
}

//  Adds 1 to dimension_tot(0) and puts a and b in the added line.
// Precondition: line_size() must be equal to 2 and the array must be resizable.
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::append_line(_TYPE_ a, _TYPE_ b)
{
  assert(TRUSTVect<_TYPE_>::line_size() == 2);
  assert(dimension_tot_0_ * TRUSTVect<_TYPE_>::line_size() == TRUSTVect<_TYPE_>::size_array());
  const int n = dimension_tot_0_ * 2;
  dimensions_[0] = ++dimension_tot_0_;
  TRUSTVect<_TYPE_>::resize_vect_(n+2, Array_base::COPY_NOINIT);
  _TYPE_ * ptr = TRUSTVect<_TYPE_>::addr() + n;
  ptr[0] = a;
  ptr[1] = b;
}

//  Like append_line(i,j), but for arrays with line_size()==3
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::append_line(_TYPE_ a, _TYPE_ b, _TYPE_ c)
{
  assert(TRUSTVect<_TYPE_>::line_size() == 3);
  assert(dimension_tot_0_  * TRUSTVect<_TYPE_>::line_size() == TRUSTVect<_TYPE_>::size_array());
  const int n = dimension_tot_0_ * 3;
  dimensions_[0] = ++dimension_tot_0_;
  TRUSTVect<_TYPE_>::resize_vect_(n+3, Array_base::COPY_NOINIT);
  _TYPE_ * ptr = TRUSTVect<_TYPE_>::addr() + n;
  ptr[0] = a;
  ptr[1] = b;
  ptr[2] = c;
}

//  Like append_line(i,j), but for arrays with line_size()==4
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::append_line(_TYPE_ a, _TYPE_ b, _TYPE_ c, _TYPE_ d)
{
  assert(TRUSTVect<_TYPE_>::line_size() == 4);
  assert(dimension_tot_0_  * TRUSTVect<_TYPE_>::line_size() == TRUSTVect<_TYPE_>::size_array());
  const int n = dimension_tot_0_ * 4;
  dimensions_[0] = ++dimension_tot_0_;
  TRUSTVect<_TYPE_>::resize_vect_(n+4, Array_base::COPY_NOINIT);
  _TYPE_ * ptr = TRUSTVect<_TYPE_>::addr() + n;
  ptr[0] = a;
  ptr[1] = b;
  ptr[2] = c;
  ptr[3] = d;
}

//  fait pointer le tableau sur le vecteur v et en associant la meme structure parallele.
//  Attention, si line_size du vecteur v est different de 1, on cree un tableau bidimensionnel (on peut avoir un vecteur
//  de ce type si on copie un Tab dans un Vect puis on prend une ref sur ce Vect).
// Precondition: le vecteur v doit vraiment etre de type Vect ! (sinon utiliser DoubleTab::ref(const DoubleTab &)
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::ref(const TRUSTVect<_TYPE_>& v)
{
  assert(std::string(typeid(v).name()).find("TRUSTVect") != std::string::npos);
  TRUSTVect<_TYPE_>::ref(v);
  const int l = v.line_size();
  // Attention: En prenant la ref, on est oblige de conserver l'attribut line_size du Vect (sinon echange_espace_virtuel ne fonctionnera pas car
  //  on n'aura pas le bon facteur multiplicatif des items geometriques). Si on voulait creer un tableau monodimensionnel avec line_size > 1,
  //  le tableau devient invalide car on n'a plus line_size = produit des dimensions > 1.
  //  On peut le faire a condition de laisser tomber le md_vector_ en faisant  tab.ref_array(v) au lieu de  tab.ref(v)
  if (l == 1)  nb_dim_ = 1;
  else
    {
      nb_dim_ = 2;
      dimensions_[1] = l;
    }

  if (v.size_reelle_ok())
    {
      int sz = v.size_reelle();
      dimensions_[0] = sz / l;
    }
  else dimensions_[0] = -1;

  dimension_tot_0_ = TRUSTVect<_TYPE_>::size_array() / l;
  assert(verifie_LINE_SIZE());
}

//  fait pointer le tableau sur le tableau t en recuperant la structure parallele. Attention, on fige le tableau qui ne pourra plus etre resize
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::ref(const TRUSTTab& src)
{
  TRUSTVect<_TYPE_>::ref(src);
  nb_dim_ = src.nb_dim_;
  for (int i = 0; i < MAXDIM_TAB; i++) dimensions_[i] = src.dimensions_[i];
  dimension_tot_0_ = src.dimension_tot_0_;
  assert(verifie_LINE_SIZE());
}

//  identique a DoubleVect::ref_data()
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::ref_data(_TYPE_* ptr, int new_size)
{
  TRUSTVect<_TYPE_>::ref_data(ptr, new_size);
  if (new_size<0) new_size=-new_size;
  assert(!TRUSTVect<_TYPE_>::get_md_vector().non_nul() && TRUSTVect<_TYPE_>::size_reelle() == TRUSTVect<_TYPE_>::size_array());
  nb_dim_ = 1;
  dimensions_[0] = dimension_tot_0_ = new_size;
  assert(verifie_LINE_SIZE());
}

//  identique a TRUSTVect::ref_array() (cree un tableau monodimensionnel sans structure parallele)
//  Attention, le tableau source et destination sont figes (resize interdit)
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::ref_array(TRUSTArray<_TYPE_>& src, int start, int sz)
{
  TRUSTVect<_TYPE_>::ref_array(src, start, sz);
  assert(!TRUSTVect<_TYPE_>::get_md_vector().non_nul() && TRUSTVect<_TYPE_>::size_reelle() == TRUSTVect<_TYPE_>::size_array());
  nb_dim_ = 1;
  dimensions_[0] = dimension_tot_0_ = TRUSTVect<_TYPE_>::size_array(); // pas sz qui peut valoir -1
  assert(verifie_LINE_SIZE());
}

//  fait pointer le tableau sur une sous-partie du tableau t definie par la valeur du premier indice et ne nombre de "lignes" du tableau
//   a recuperer (une ligne = toutes les valeurs tab(i,j,k,...) pour un i donne). Le nombre de dimensions du tableau est le meme que pour t,
//   les dimension(i) pour i>=1 sont les memes et dimension(0) = nb_lines.
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::ref_tab(TRUSTTab<_TYPE_>& t, int start_line, int nb_lines)
{
  if (nb_lines < 0) nb_lines = t.dimension_tot_0_ - start_line;
  assert(start_line >= 0 && nb_lines >= 0 && start_line + nb_lines <= t.dimension_tot_0_);
  const int l_size = t.line_size();
  TRUSTVect<_TYPE_>::ref_array(t, start_line * l_size, nb_lines * l_size);
  assert(!TRUSTVect<_TYPE_>::get_md_vector().non_nul() && TRUSTVect<_TYPE_>::size_reelle() == TRUSTVect<_TYPE_>::size_array());
  TRUSTVect<_TYPE_>::set_line_size_(l_size);
  nb_dim_ = t.nb_dim_;
  dimension_tot_0_ = nb_lines;
  dimensions_[0] = nb_lines;
  for (int i = 1; i < MAXDIM_TAB; i++) dimensions_[i] = t.dimensions_[i];
  assert(verifie_LINE_SIZE());
}

//  met le tableau dans l'etat obtenu par le constructeur par defaut
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::reset()
{
  nb_dim_ = 1;
  dimensions_[0] = 0;
  dimension_tot_0_ = 0;
  TRUSTVect<_TYPE_>::reset();
  assert(verifie_LINE_SIZE());
}

//  methode virtuelle qui force le tableau a changer de taille. Change aussi nb_dim_ a 1. Equivalent a TRUSTTab::resize(n, opt)
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::resize_tab(int n, Array_base::Resize_Options opt)
{
  resize(n, opt);
  assert(verifie_LINE_SIZE());
}

//  change la dimension[0] du tableau en conservant les autres.
// Precondition: le tableau ne doit pas avoir de structure parallele
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::resize_dim0(int n, Array_base::Resize_Options opt)
{
  assert(n >= 0);
  assert(dimension_tot_0_ * TRUSTVect<_TYPE_>::line_size() == TRUSTVect<_TYPE_>::size_array());
  TRUSTVect<_TYPE_>::resize_vect_(n * TRUSTVect<_TYPE_>::line_size(), opt);
  dimensions_[0] = dimension_tot_0_ = n;
  assert(verifie_LINE_SIZE());
}

template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::resize(int n, Array_base::Resize_Options opt)
{
  assert(n >= 0);
  TRUSTVect<_TYPE_>::set_line_size_(1);
  TRUSTVect<_TYPE_>::resize_vect_(n, opt);
  nb_dim_ = 1;
  dimensions_[0] = dimension_tot_0_ = n;
  assert(verifie_LINE_SIZE());
}

template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::resize(int n, int n2, Array_base::Resize_Options opt)
{
  assert(n >= 0 && n2 >= 0);
  TRUSTVect<_TYPE_>::set_line_size_(n2);
  int new_size = n * n2;

  if (std::is_same<_TYPE_,int>::value && new_size < 0)
    {
      Cerr << "n1*n2 > 2^31. Error! Contact TRUST support, integer 32 bits limit exceeded with n1=" << n << " and n2=" << n2 << finl;
      Process::exit();
    }

  TRUSTVect<_TYPE_>::resize_vect_(new_size, opt);
  nb_dim_ = 2;
  dimensions_[0] = dimension_tot_0_ = n;
  dimensions_[1] = n2;
  assert(verifie_LINE_SIZE());
}

template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::resize(int n, int n2, int n3, Array_base::Resize_Options opt)
{
  assert(n >= 0 && n2 >= 0 && n3 >= 0);
  TRUSTVect<_TYPE_>::set_line_size_(n2 * n3);
  int new_size = n * n2 * n3;

  if (std::is_same<_TYPE_,int>::value && new_size < 0)
    {
      Cerr << "n1*n2*n3 > 2^31. Error! Contact TRUST support, integer 32 bits limit exceeded with n1=" << n << " and n2=" << n2 << " and n3=" << n3 << finl;
      Process::exit();
    }

  TRUSTVect<_TYPE_>::resize_vect_(new_size, opt);
  nb_dim_ = 3;
  dimensions_[0] = dimension_tot_0_ = n;
  dimensions_[1] = n2;
  dimensions_[2] = n3;
  assert(verifie_LINE_SIZE());
}

template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::resize(int n, int n2, int n3, int n4, Array_base::Resize_Options opt)
{
  assert(n >= 0 && n2 >= 0 && n3 >= 0 && n4 >= 0);
  TRUSTVect<_TYPE_>::set_line_size_(n2 * n3 * n4);
  int new_size = n * n2 * n3 * n4;

  if (std::is_same<_TYPE_,int>::value && new_size<0)
    {
      Cerr << "n1*n2*n3*n4 > 2^31. Error! Contact TRUST support, integer 32 bits limit exceeded with n1=" << n << " and n2=" << n2 << " and n3=" << n3 << " and n4=" << n4 << finl;
      Process::exit();
    }

  TRUSTVect<_TYPE_>::resize_vect_(new_size, opt);
  nb_dim_ = 4;
  dimensions_[0] = dimension_tot_0_ = n;
  dimensions_[1] = n2;
  dimensions_[2] = n3;
  dimensions_[3] = n4;
  assert(verifie_LINE_SIZE());
}

//  redimensionne le tableau (nb_dim_ sera egal a tailles.size_array() et dimension(i) a tailles[i].
// Precondition: identiques a TRUSTVect::resize_vect_()
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::resize(const TRUSTArray<int>& tailles, Array_base::Resize_Options opt)
{
  nb_dim_ = tailles.size_array();
  if (nb_dim_ <= 0 || nb_dim_ > MAXDIM_TAB)
    {
      Cerr << "Internal error in TRUSTTab::resize(const ArrOfInt & tailles, ...) \n" << " wrong dimensions number " << nb_dim_ << finl;
      Process::exit();
    }
  int l_size = 1;
  for (int i = 0; i < nb_dim_; i++)
    {
      const int n = tailles[i];
      dimensions_[i] = n;
      if (i > 0)
        l_size *= n;
      if (n < 0)
        {
          Cerr << "Internal error in TRUSTTab::resize(const ArrOfInt & tailles, ...) \n" << " wrong dimensions: " << tailles << finl;
          Process::exit();
        }
    }
  dimension_tot_0_ = dimensions_[0];
  TRUSTVect<_TYPE_>::set_line_size_(l_size);
  TRUSTVect<_TYPE_>::resize_vect_(dimensions_[0] * l_size, opt);
  assert(verifie_LINE_SIZE());
}

//  copie la structure et les valeurs du tableau src
//   Restrictions et preconditions identiques a TRUSTVect::operator=(const TRUSTVect & v)
template<typename _TYPE_>
inline TRUSTTab<_TYPE_>& TRUSTTab<_TYPE_>::operator=(const TRUSTTab<_TYPE_>& src)
{
  copy(src);
  return *this;
}

//  copie la structure et les valeurs de src. Attention: appel invalide si src est un type derive de Vect
//  (sinon quoi faire, un tableau unidimensionnel, ou une copie de la structure ?)
template<typename _TYPE_>
inline TRUSTTab<_TYPE_>& TRUSTTab<_TYPE_>::operator=(const TRUSTVect<_TYPE_>& src)
{
  assert(std::string(typeid(src).name()).find("TRUSTVect") != std::string::npos);
  TRUSTVect<_TYPE_>::copy_(src);

  // Idem que dans ref(TRUSTVect<_TYPE_>) pour le nombre de dimensions du tableau cree
  const int l = src.line_size();
  if (l == 1) nb_dim_ = 1;
  else
    {
      nb_dim_ = 2;
      dimensions_[1] = l;
    }

  if (src.size_reelle_ok())
    {
      int sz = src.size_reelle();
      dimensions_[0] = sz / l;
      assert(sz % l == 0);
    }
  else dimensions_[0] = -1;

  dimension_tot_0_ = TRUSTVect<_TYPE_>::size_array() / l;
  assert(verifie_LINE_SIZE());
  return *this;
}

template<typename _TYPE_>
inline TRUSTTab<_TYPE_>& TRUSTTab<_TYPE_>::operator=(_TYPE_ d)
{
  TRUSTVect<_TYPE_>::operator=(d);
  return *this;
}

template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::copy(const TRUSTTab<_TYPE_>& src, Array_base::Resize_Options opt)
{
  if (&src != this)
    {
      TRUSTVect<_TYPE_>::copy_(src, opt);
      nb_dim_ = src.nb_dim_;
      for (int i = 0; i < MAXDIM_TAB; i++) dimensions_[i] = src.dimensions_[i];
      dimension_tot_0_ = src.dimension_tot_0_;
      assert(verifie_LINE_SIZE());
    }
}

template<typename _TYPE_>
inline _TYPE_& TRUSTTab<_TYPE_>::operator()(const TRUSTArray<int>& indice)
{
  assert(indice.size_array() == nb_dim_);
  verifie_MAXDIM_TAB();
  switch(nb_dim_)
    {
    case 1:
      return operator()(indice[0]);
    case 2:
      return operator()(indice[0], indice[1]);
    case 3:
      return operator()(indice[0], indice[1], indice[2]);
    default:
      return operator()(indice[0], indice[1], indice[2], indice[3]);
    }
}

template<typename _TYPE_>
inline _TYPE_ TRUSTTab<_TYPE_>::operator()(const TRUSTArray<int>& indice) const
{
  assert(indice.size_array() == nb_dim_);
  verifie_MAXDIM_TAB();
  switch(nb_dim_)
    {
    case 1:
      return operator()(indice[0]);
    case 2:
      return operator()(indice[0], indice[1]);
    case 3:
      return operator()(indice[0], indice[1], indice[2]);
    default:
      return operator()(indice[0], indice[1], indice[2], indice[3]);
    }
}

//  associe le md_vector au vecteur (voir TRUSTVect::set_md_vector()) dimension(0) sera initialise a md_vector...get_nb_items_reels().
// Precondition: en plus des preconditions de TRUSTVect::set_md_vector(), dimension_tot(0) doit etre egal a get_nb_items_tot() du md_vector.
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::set_md_vector(const MD_Vector& md_vector)
{
  int dim0 = dimension_tot_0_;
  if (md_vector.non_nul())
    {
      // renvoie -1 si l'appel et invalide:
      dim0 = md_vector.valeur().get_nb_items_reels();
    }
  dimensions_[0] = dim0;
  assert(verifie_LINE_SIZE());
  // a appeler meme pour un md_vector nul (pour remettre size_reelle_):
  TRUSTVect<_TYPE_>::set_md_vector(md_vector);
}

template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::ecrit(Sortie& os) const
{
  os << nb_dim_ << finl;
  if (nb_dim_ > 0) os.put(dimensions_, nb_dim_, nb_dim_);
  TRUSTArray<int> tmp(nb_dim_);
  for (int i = 0; i < nb_dim_; i++) tmp[i] = dimension_tot(i);
  os << tmp;
  TRUSTVect<_TYPE_>::ecrit(os);
}

template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::jump(Entree& is)
{
  TRUSTTab<_TYPE_>::lit(is, 0 /* Do not resize&read the array */);
}

//  lecture d'un tableau pour reprise de calcul. On lit les valeurs "raw".
//  Attention, si le tableau n'est pas vide, il doit deja avoir la bonne taille et la bonne structure, sinon erreur !
// Parameter resize_and_read if the array is sized AND read (by default, yes)
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::lit(Entree& is, int resize_and_read)
{
  TRUSTArray<int> tmp;
  is >> tmp;
  int ok = 1;
  if (tmp.size_array() != nb_dim_) ok = 0;

  if (ok)
    {
      if (TRUSTVect<_TYPE_>::size_reelle_ok() && dimension(0) != tmp[0]) ok = 0;
      for (int i = 1; i < nb_dim_; i++)
        if (dimension(i) != tmp[i]) ok = 0;
    }
  is >> tmp;
  if (ok && tmp.size_array() != nb_dim_) ok = 0;
  if (ok)
    for (int i = 0; i < nb_dim_; i++)
      if (dimension_tot(i) != tmp[i]) ok = 0;

  // Autorisation ancien format des champs scalaire 183:
  if (tmp.size_array()==1 && nb_dim_==2 && dimension(1)==1 && dimension_tot(0) == tmp[0]) ok = 1;

  if (resize_and_read)
    {
      if (TRUSTVect<_TYPE_>::size_array() == 0 && (!TRUSTVect<_TYPE_>::get_md_vector().non_nul())) resize(tmp, Array_base::NOCOPY_NOINIT);
      else if (!ok)
        {
          // Si on cherche a relire un tableau de taille inconnue, le tableau doit etre reset() a l'entree. On n'aura pas la structure parallele du tableau !
          Cerr << "Error in DoubleTab::lit: array has wrong dimensions" << finl;
          Process::exit();
        }
    }
  TRUSTVect<_TYPE_>::lit(is,resize_and_read);
}

// ------------------------------------------------------
// Juste pour double

//  Quelqu'un veut-il expliquer ce que fait cette methode ?
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::ajoute_produit_tensoriel(double alpha, const TRUSTTab<double>& x, const TRUSTTab<double>& y)
{
  // Tableaux vus comme des tableaux unidimensionnels (pour ne pas avoir a gerer nb_dim)
  const TRUSTVect<double>& vx = x, &vy = y;
  TRUSTVect<double>& v = *this;

  const int line_size_x = vx.line_size(), line_size_y = vy.line_size(), line_size_xy = v.line_size();
  assert(line_size_xy == line_size_x * line_size_y);
  // Pour ne pas diviser par line_size()
  assert(vx.size_totale() * line_size_xy == v.size_totale() * line_size_x);
  assert(vy.size_totale() * line_size_xy == v.size_totale() * line_size_y);

  // blocs d'items a traiter (un bloc par defaut: tout le tableau)
  int default_bloc[2];
  default_bloc[0] = 0;
  default_bloc[1] = (v.line_size() > 0) ? (v.size_totale() / v.line_size()) : 0;
  const int *blocs = default_bloc;
  int nb_blocs = 1;
  if (v.get_md_vector().non_nul())
    {
      const TRUSTArray<int>& blk = v.get_md_vector().valeur().get_items_to_compute();
      blocs = blk.addr();
      nb_blocs = blk.size_array() / 2;
    }

  for (int i_bloc = 0; i_bloc < nb_blocs; i_bloc++)
    {
      const int debut = blocs[i_bloc*2], fin = blocs[i_bloc*2+1];
      int v_index = debut * line_size_xy;
      for (int i = debut; i < fin; i++)
        for (int j = 0; j < line_size_x; j++)
          {
            double xval = vx[i * line_size_x + j];
            for (int k = 0; k < line_size_y; k++)
              {
                double yval = vy[i * line_size_y + k];
                v[v_index] += alpha * xval * yval;
                v_index++;
              }
          }
    }
}

//  Resolution du systeme Ax=b
template<typename _TYPE_>
inline int TRUSTTab<_TYPE_>::inverse_LU(const TRUSTArray<double>& b, TRUSTArray<double>& solution)
{
  int n = b.size_array();
  TRUSTArray<int> index(n);
  TRUSTTab<double> lu_dec(n,n);
  int cvg = (*this).decomp_LU(n,index,lu_dec);

  if(cvg == 1) lu_dec.resoud_LU(n,index,b,solution);

  return cvg;
}

//  Decomposition d'une matrice en L.U: methode de Crout (diagonale de L =1)
// Retour: matrice A_ = assemblage (L-diagonale)+U
template<typename _TYPE_>
inline int TRUSTTab<_TYPE_>::decomp_LU(int n, TRUSTArray<int>& index, TRUSTTab<double>& matLU)
{
  TRUSTArray<double> vv(n);
  int i, j, k,  imax = -1, cvg = 1;
  double big, dum, sum, temp;
  matLU = (*this);

  //Recupere le coeff max d'une ligne, stocke dans vv
  for (i=0 ; i<n ; i++)
    {
      big = 0;
      for (j=0 ; j<n ; j++)
        if ((temp = std::fabs(matLU(i,j))) > big) big = temp;

      if (big == 0)
        {
          Cerr <<"Singular matrix in LU decomposition"<<finl;
          cvg = 0;
          Process::exit();
        }
      vv[i] = 1./big;
    }

  //calcul de la matrice matLU
  for (j=0 ; j<n ; j++)
    {
      for (i=0 ; i<j ; i++)
        {
          sum = matLU(i,j);
          for (k=0 ; k<i ; k++) sum -= matLU(i,k) * matLU(k,j);
          matLU(i,j) = sum;
        }

      big = 0;
      for (i=j ; i<n ; i++)
        {
          sum = matLU(i,j);
          for (k=0 ; k<j ; k++) sum -= matLU(i,k) * matLU(k,j);
          matLU(i,j) = sum;
          if ((dum = vv[i]*std::fabs(sum)) >= big)
            {
              big = dum;
              imax = i;
            }
        }

      if (j != imax)
        {
          for (k=0 ; k<n ; k++)
            {
              dum = matLU(imax,k);
              matLU(imax,k) = matLU(j,k);
              matLU(j,k) = dum;
            }
          vv[imax] = vv[j];
        }

      index[j] = imax;
      dum = 1./matLU(j,j);
      for (i=j+1 ; i<n ; i++) matLU(i,j) *= dum;
    }
  return cvg;
}

//  Resolution du systeme A_x=b : A_ contenant le decompostion LU de A (stockee dans une seule matrice)
template<typename _TYPE_>
inline void TRUSTTab<_TYPE_>::resoud_LU(int n, TRUSTArray<int>& index, const TRUSTArray<double>& b, TRUSTArray<double>& solution)
{
  int i,ii=-1,ip,j;
  double sum;
  solution = b;
  for (i=0 ; i<n ; i++)
    {
      ip = index[i];
      sum = solution[ip];
      solution[ip] = solution[i];
      if (ii!=-1)
        for (j=ii ; j<i ; j++) sum -= (*this)(i,j)*solution[j];
      else if (sum) ii =i;

      solution[i] = sum;
    }

  for (i=n-1 ; i>=0 ; i--)
    {
      sum = solution[i];
      for (j=i+1 ; j<n ; j++) sum -= (*this)(i,j)*solution[j];
      solution[i] = sum/(*this)(i,i);
    }
}

//  Fonction utilisee pour le calcul du du/u (pour convergence implicite)
//    renvoie le max de abs(du(i)/u(i)). utilisation    max_ = (u(n+1)-u(n)).max_du_u(u(n))
template<typename _TYPE_>
inline double TRUSTTab<_TYPE_>::max_du_u(const TRUSTTab<double>& u)
{
  assert(TRUSTVect<_TYPE_>::size_array() == u.size_array());
  const double *du_ptr = TRUSTVect<_TYPE_>::addr();
  const double *u_ptr = u.addr();
  const double epsilon = 1.e-8;
  double res = 0.;
  for (int n = TRUSTVect<_TYPE_>::size_array(); n; n--)
    {
      double a = std::fabs(*du_ptr), b = std::fabs(*u_ptr), c = a / (b + epsilon);
      if (b > 1.e-2 && c > res) res = c;
      du_ptr++;
      u_ptr++;
    }
  return res;
}

#endif /* TRUSTTab_TPP_included */
