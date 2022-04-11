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
// File:        Champ.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_included
#define Champ_included

#include <Deriv.h>
#include <Champ_base.h>
#include <List.h>

Declare_deriv(Champ_base);

//
// .DESCRIPTION class Champ
//
// Cette classe sert a representer n'importe quelle classe
// derivee de la classe Champ_base
//

class Champ : public DERIV(Champ_base), public Champ_Proto
{
  Declare_instanciable(Champ);
public :
  //  inline Champ(const Champ_base& ch):DERIV(Champ_base)(ch) {}
  inline const Champ_base& operator=(const Champ_base& ch)
  {
    DERIV(Champ_base)::operator=(ch);
    return this->valeur();
  }
  inline void nommer(const Nom& ) override;
  inline const Nom& le_nom() const override;
  inline void fixer_nb_comp(int i);
  inline int nb_comp() const ;
  inline double changer_temps(const double t);
  inline double temps() const;
  inline const Noms& fixer_unites(const Noms& );
  inline const Nom& fixer_unite(const Nom& );
  inline const Nom& fixer_unite(int i, const Nom&);
  inline const Noms& unites() const ;
  inline const Nom& unite() const ;
  inline const Nom& unite(int ) const ;
  inline const Noms& fixer_noms_compo(const Noms& ) ;
  inline const Nom& fixer_nom_compo(int, const Nom& ) ;
  inline const Nom& fixer_nom_compo(const Nom& ) ;
  inline const Noms& noms_compo() const ;
  inline const Nom& nom_compo(int ) const ;
  inline const Nom& nom_compo() const ;
  inline void mettre_a_jour(double );
  inline Champ_base& affecter(const Champ_base& );
  inline Champ_base& affecter_compo(const Champ_base& , int );
  inline DoubleVect& valeur_a(const DoubleVect& position,
                              DoubleVect& les_valeurs) const ;
  inline DoubleVect& valeur_a_elem(const DoubleVect& position,
                                   DoubleVect& valeurs,
                                   int le_poly) const ;
  inline double valeur_a_elem_compo(const DoubleVect& position,
                                    int le_poly,int ncomp) const ;
  inline double valeur_a_sommet_compo(int, int, int) const;
  inline DoubleTab& valeur_aux(const DoubleTab& positions,
                               DoubleTab& valeurs) const ;
  inline DoubleVect& valeur_aux_compo(const DoubleTab& positions,
                                      DoubleVect& valeurs, int ncomp) const ;
  inline DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                                     const IntVect& les_polys,
                                     DoubleTab& valeurs) const ;
  inline DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions,
                                            const IntVect& les_polys,
                                            DoubleVect& valeurs,
                                            int ncomp) const ;

  using Champ_Proto::valeurs;
  inline DoubleTab& valeurs() override ;
  inline const DoubleTab& valeurs() const override ;

};

inline void Champ::nommer(const Nom& un_nom)
{
  valeur().nommer(un_nom);
}
inline const Nom& Champ::le_nom() const
{
  return valeur().le_nom();
}
inline void Champ::fixer_nb_comp(int i)
{
  valeur().fixer_nb_comp(i);
}
inline int Champ::nb_comp() const
{
  return valeur().nb_comp();
}
inline double Champ::changer_temps(const double t)
{
  return valeur().changer_temps(t);
}
inline double Champ::temps() const
{
  return valeur().temps();
}
inline const Noms& Champ::fixer_unites(const Noms& les_unites)
{
  return valeur().fixer_unites(les_unites);
}
inline const Nom& Champ::fixer_unite(const Nom& lunite)
{
  return valeur().fixer_unite(lunite);
}
inline const Nom& Champ::fixer_unite(int i, const Nom& lunite)
{
  return valeur().fixer_unite(i, lunite);
}
inline const Noms& Champ::unites() const
{
  return valeur().unites();
}
inline const Nom& Champ::unite() const
{
  return valeur().unite();
}
inline const Nom& Champ::unite(int i) const
{
  return valeur().unite(i);
}
inline const Noms& Champ::fixer_noms_compo(const Noms& les_noms)
{
  return valeur().fixer_noms_compo(les_noms);
}
inline const Nom& Champ::fixer_nom_compo(int i, const Nom& nom)
{
  return valeur().fixer_nom_compo(i, nom);
}
inline const Nom& Champ::fixer_nom_compo(const Nom& nom)
{
  return valeur().fixer_nom_compo(nom);
}
inline const Noms& Champ::noms_compo() const
{
  return valeur().noms_compo();
}
inline const Nom& Champ::nom_compo(int i) const
{
  return valeur().nom_compo(i);
}
inline const Nom& Champ::nom_compo() const
{
  return valeur().nom_compo();
}
inline void Champ::mettre_a_jour(double un_temps)
{
  valeur().mettre_a_jour(un_temps);
}
inline Champ_base& Champ::affecter(const Champ_base& ch)
{
  return valeur().affecter(ch);
}
inline Champ_base& Champ::affecter_compo(const Champ_base& ch,
                                         int compo)
{
  return valeur().affecter_compo(ch, compo);
}
inline DoubleVect& Champ::valeur_a(const DoubleVect& position,
                                   DoubleVect& les_valeurs) const
{
  return valeur().valeur_a(position, les_valeurs);
}
inline DoubleVect& Champ::valeur_a_elem(const DoubleVect& position,
                                        DoubleVect& les_valeurs,
                                        int le_poly) const
{
  return valeur().valeur_a_elem(position, les_valeurs, le_poly);
}
inline double Champ::valeur_a_elem_compo(const DoubleVect& position,
                                         int le_poly,int ncomp) const
{
  return valeur().valeur_a_elem_compo(position, le_poly, ncomp);
}
inline double Champ::valeur_a_sommet_compo(int num_som, int le_poly, int ncomp) const
{
  return valeur().valeur_a_sommet_compo(num_som, le_poly, ncomp);
}
inline DoubleTab& Champ::valeur_aux(const DoubleTab& positions,
                                    DoubleTab& les_valeurs) const
{
  return valeur().valeur_aux(positions, les_valeurs);
}
inline DoubleVect& Champ::valeur_aux_compo(const DoubleTab& positions,
                                           DoubleVect& les_valeurs, int ncomp) const
{
  return valeur().valeur_aux_compo(positions, les_valeurs, ncomp);
}
inline DoubleTab& Champ::valeur_aux_elems(const DoubleTab& positions,
                                          const IntVect& les_polys,
                                          DoubleTab& les_valeurs) const
{
  return valeur().valeur_aux_elems(positions, les_polys, les_valeurs);
}
inline DoubleVect& Champ::valeur_aux_elems_compo(const DoubleTab& positions,
                                                 const IntVect& les_polys,
                                                 DoubleVect& les_valeurs,
                                                 int ncomp) const
{
  return valeur().valeur_aux_elems_compo(positions, les_polys, les_valeurs, ncomp);
}

inline DoubleTab& Champ::valeurs()
{
  return valeur().valeurs();
}
inline const DoubleTab& Champ::valeurs() const
{
  return valeur().valeurs();
}
Declare_liste(Champ);
#endif
