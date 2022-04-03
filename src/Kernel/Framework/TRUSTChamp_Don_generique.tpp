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
// File:        TRUSTChamp_Don_generique.tpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTChamp_Don_generique_TPP_included
#define TRUSTChamp_Don_generique_TPP_included

template <Champ_Don_Type _TYPE_>
Sortie& TRUSTChamp_Don_generique<_TYPE_>::printOn(Sortie& os) const
{
  const DoubleTab& tab = valeurs();
  os << tab.size() << " ";
  for (int i = 0; i < tab.size(); i++) os << tab(0, i);
  return os;
}

// Description: Renvoie la valeur du champ au point specifie par ses coordonnees.
// Parametre: DoubleVect& x
//    Signification: les coordonnees du point de calcul
// Parametre: DoubleVect& val
//    Signification: la valeur du champ au point specifie
template <Champ_Don_Type _TYPE_>
DoubleVect& TRUSTChamp_Don_generique<_TYPE_>::valeur_a(const DoubleVect& x, DoubleVect& val) const
{
  static constexpr bool IS_XYZ = (_TYPE_ == Champ_Don_Type::XYZ);
  if (val.size() != nb_compo_)
    {
      Cerr << "Error TRUST in TRUSTChamp_Don_generique<_TYPE_>::valeur_a()" << finl;
      Cerr << "The DoubleVect val doesn't have the correct size !" << finl;
      Process::exit();
    }

  IS_XYZ ? eval_fct(x,val) : eval_fct(x,temps(),val);
  return val;
}

template <Champ_Don_Type _TYPE_>
double TRUSTChamp_Don_generique<_TYPE_>::valeur_a_compo(const DoubleVect& x, int ncomp) const
{
  static constexpr bool IS_TXYZ = (_TYPE_ == Champ_Don_Type::TXYZ);
  if (IS_TXYZ) return Champ_Don_base::valeur_a_compo(x, ncomp); // appel simple !

  if (ncomp > nb_compo_)
    {
      Cerr << "Error TRUST in TRUSTChamp_Don_generique<XYZ>::valeur_a_compo()" << finl;
      Cerr << "the integer ncomp is upper than the number of field components !" << finl;
      Process::exit();
    }

  DoubleTab positions(1, dimension);
  DoubleTab val_fct(1);
  positions(0, 0) = x(0);
  positions(0, 1) = x(1);
  if (dimension > 2) positions(0, 2) = x(2);

  eval_fct(positions, val_fct, ncomp);
  return val_fct(0);
}

// Description:
//    Renvoie la valeur du champ au point specifie par ses coordonnees, en indiquant que ce point est
//    situe dans un element specifie.
// Parametre: DoubleVect&
//    Signification: les coordonnees du point de calcul
// Parametre: DoubleVect& val
//    Signification: la valeur du champ au point specifie
// Parametre: int le_poly
//    Signification: l'element dans lequel est situe le point de calcul (inutile)
template <Champ_Don_Type _TYPE_>
DoubleVect& TRUSTChamp_Don_generique<_TYPE_>::valeur_a_elem(const DoubleVect& x, DoubleVect& val, int ) const
{
  return valeur_a(x,val);
}

// Description:
//    Renvoie la valeur d'une composante du champ au point specifie
//    par ses coordonnees, en indiquant que ce point est situe dans un element specifie.
// Parametre: DoubleVect&
//    Signification: les coordonnees du point de calcul
// Parametre: int le_poly
//    Signification: l'element dans lequel est situe le point de calcul (inutile)
// Parametre: int ncomp
//    Signification: l'index de la composante du champ a calculer
template <Champ_Don_Type _TYPE_> template <Champ_Don_Type T>
enable_if_t<T == Champ_Don_Type::XYZ, double>
TRUSTChamp_Don_generique<_TYPE_>::valeur_a_elem_compo_(const DoubleVect& x, int , int ncomp) const
{
  if (ncomp > nb_compo_)
    {
      Cerr << "Error TRUST in TRUSTChamp_Don_generique<XYZ>::valeur_a_elem_compo()" << finl;
      Cerr << "the integer ncomp is upper than the number of field components !" << finl;
      Process::exit();
    }
  return valeur_a_compo(x,ncomp);
}

template <Champ_Don_Type _TYPE_> template <Champ_Don_Type T>
enable_if_t<T == Champ_Don_Type::TXYZ, double>
TRUSTChamp_Don_generique<_TYPE_>::valeur_a_elem_compo_(const DoubleVect& x, int , int ncomp) const
{
  double val;
  DoubleTab positions(1,dimension);
  DoubleVect val_fct(1);
  double tps = temps();
  positions(0,0) = x(0);
  positions(0,1) = x(1);
  if (dimension>2)
    positions(0,2) = x(2);

  eval_fct(positions,tps,val_fct,ncomp);
  val = val_fct(0);

  return val;
}

// Description: Renvoie les valeurs du champ aux points specifies par leurs coordonnees.
// Parametre: DoubleTab& x
//    Signification: le tableau des coordonnees des points de calcul
// Parametre: DoubleTab& val
//    Signification: le tableau des valeurs du champ aux points specifies
template <Champ_Don_Type _TYPE_> template <Champ_Don_Type T>
enable_if_t<T == Champ_Don_Type::XYZ, DoubleTab&>
TRUSTChamp_Don_generique<_TYPE_>::valeur_aux_(const DoubleTab& x, DoubleTab& val) const
{
  if (val.nb_dim() > 2)
    {
      Cerr << "Error TRUST in TRUSTChamp_Don_generique<XYZ>::valeur_aux()" << finl;
      Cerr << "The DoubleTab val has more than 2 entries" << finl;
      Process::exit();
    }
  eval_fct(x,val);
  return val;
}

template <Champ_Don_Type _TYPE_> template <Champ_Don_Type T>
enable_if_t<T == Champ_Don_Type::TXYZ, DoubleTab&>
TRUSTChamp_Don_generique<_TYPE_>::valeur_aux_(const DoubleTab& x, DoubleTab& val) const
{
  if (val.nb_dim() == 1) assert(nb_compo_ == 1);
  else if (val.nb_dim() == 2) assert(val.dimension(1) == nb_compo_);
  else
    {
      Cerr << "Error TRUST in TRUSTChamp_Don_generique<TXYZ>::valeur_aux()" << finl;
      Cerr << "The DoubleTab val has more than 2 entries" << finl;
      Process::exit();
    }
  eval_fct(x,temps(),val);
  return val;
}

// Description:
//    Renvoie les valeurs d'une composante du champ aux points specifies par leurs coordonnees.
// Parametre: DoubleVect& x
//    Signification: le tableau des coordonnees des points de calcul
// Parametre: DoubleVect& valeurs
//    Signification: le tableau des valeurs de la composante du champ aux points specifies
// Parametre: int ncomp
//    Signification: l'index de la composante du champ a calculer
template <Champ_Don_Type _TYPE_>
DoubleVect& TRUSTChamp_Don_generique<_TYPE_>::valeur_aux_compo(const DoubleTab& x, DoubleVect& val, int ncomp) const
{
  static constexpr bool IS_XYZ = (_TYPE_ == Champ_Don_Type::XYZ);
  IS_XYZ ? eval_fct(x,val,ncomp) : eval_fct(x,temps(),val,ncomp);
  return val;
}

// Description:
//    Renvoie les valeurs du champ aux points specifies
//    par leurs coordonnees, en indiquant que les points de calculs sont situes dans les elements indiques.
// Precondition: le nombre de valeurs demandes doit etre egal aux nombre d'elements specifies
// Precondition: la dernier dimension du tableau des valeurs doit etre egal au nombre de composante du champ
// Parametre: DoubleTab&
//    Signification: le tableau des coordonnees des points de calcul
// Parametre: IntVect& les_polys
//    Signification: le tableau des elements dans lesquels sont situes les points de calcul (inutile)
// Parametre: DoubleTab& val
//    Signification: le tableau des valeurs du champ aux points specifies
template <Champ_Don_Type _TYPE_>
DoubleTab& TRUSTChamp_Don_generique<_TYPE_>::valeur_aux_elems(const DoubleTab& x, const IntVect&, DoubleTab& val) const
{
  return valeur_aux(x, val);
}

// Description:
//    Renvoie les valeurs d'une composante du champ aux points specifies
//    par leurs coordonnees, en indiquant que les points de calculs sont situes dans les elements indiques.
// Parametre: DoubleTab& x
//    Signification: le tableau des coordonnees des points de calcul
// Parametre: IntVect& les_polys
//    Signification: le tableau des elements dans lesquels sont situes les points de calcul (inutile)
// Parametre: DoubleVect& val
//    Signification: le tableau des valeurs de la composante du champ aux points specifies
// Parametre: int ncomp
//    Signification: l'index de la composante du champ a calculer
template <Champ_Don_Type _TYPE_>
DoubleVect& TRUSTChamp_Don_generique<_TYPE_>::valeur_aux_elems_compo(const DoubleTab& x, const IntVect&, DoubleVect& val, int ncomp) const
{
  return valeur_aux_compo(x, val, ncomp);
}

// methode protected
template <Champ_Don_Type _TYPE_>
void TRUSTChamp_Don_generique<_TYPE_>::mettre_a_jour_positions(DoubleTab& positions)
{
  double x, y, z;
  const int nb_elems = mon_domaine->zone(0).nb_elem();
  const IntTab& les_Polys = mon_domaine->zone(0).les_elems();
  for (int num_elem = 0; num_elem < nb_elems; num_elem++)
    {
      x = y = z = 0;
      int nb_som = 0;
      for (int s = 0, num_som; s < les_Polys.dimension(1); s++)
        if ((num_som = les_Polys(num_elem, s)) >= 0)
          {
            x += mon_domaine->coord(num_som, 0);
            y += mon_domaine->coord(num_som, 1);
            if (dimension > 2) z += mon_domaine->coord(num_som, 2);
            nb_som++;
          }
      positions(num_elem, 0) = x / nb_som;
      positions(num_elem, 1) = y / nb_som;
      if (dimension > 2) positions(num_elem, 2) = z / nb_som;
    }
}

#endif /* TRUSTChamp_Don_generique_TPP_included */
