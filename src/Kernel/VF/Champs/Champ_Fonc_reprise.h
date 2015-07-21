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
// File:        Champ_Fonc_reprise.h
// Directory:   $TRUST_ROOT/src/Kernel/VF/Champs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION

// .SECTION voir aussi
//
//
///////////////////////////////////////////////////////////////////////////

#ifndef Champ_Fonc_reprise_included
#define Champ_Fonc_reprise_included



#include <Champ_Fonc_base.h>
#include <Champ_Inc.h>

//.DESCRIPTION classe Champ_Fonc_reprise
// Cette classe permet de relire un champ sauvegarde dans un fichier xyz
// elle porte un champ de type Champ_Inc qui est du meme type que le
// champ passe en argument

//.SECTION voir aussi
// Champ_Fonc_P0

class Champ_Fonc_reprise: public Champ_Fonc_base
{

  Declare_instanciable(Champ_Fonc_reprise);

public :

  inline void associer_zone_dis_base(const Zone_dis_base&);
  const Zone_dis_base& zone_dis_base() const;
  virtual void mettre_a_jour(double );

  inline const DoubleTab& valeurs_champ() const;
  inline virtual DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                                             const IntVect& les_polys,
                                             DoubleTab& les_valeurs) const;
  inline virtual DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions,
                                                    const IntVect& les_polys,
                                                    DoubleVect& les_valeurs,
                                                    int ncomp) const ;
  inline virtual DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const;
  inline virtual DoubleVect& valeur_aux_sommets_compo(const Domaine&,
                                                      DoubleVect&, int) const;
  inline virtual DoubleTab& remplir_coord_noeuds(DoubleTab& ) const;
  inline virtual int remplir_coord_noeuds_et_polys(DoubleTab&, IntVect&) const;
  inline  DoubleVect& valeur_aux_compo(const DoubleTab& tab,DoubleVect& les_valeurs, int comp) const;

private:

  REF(Zone_dis_base) zone_dis;

  Champ_Inc vrai_champ_;
  inline virtual const Champ_Inc_base& le_champ() const;
  inline virtual Champ_Inc_base& le_champ();
};

inline void Champ_Fonc_reprise::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  zone_dis=la_zone_dis_base;
}
inline const Champ_Inc_base& Champ_Fonc_reprise::le_champ() const
{
  return vrai_champ_.valeur();
}

inline Champ_Inc_base& Champ_Fonc_reprise::le_champ()
{
  return vrai_champ_.valeur();
}

inline const DoubleTab& Champ_Fonc_reprise::valeurs_champ() const
{
  return le_champ().valeurs();
}

inline DoubleTab& Champ_Fonc_reprise::valeur_aux_elems(const DoubleTab& positions,
                                                       const IntVect& les_polys,
                                                       DoubleTab& les_valeurs) const
{
  return le_champ().valeur_aux_elems(positions, les_polys, les_valeurs);
}

inline DoubleVect& Champ_Fonc_reprise::valeur_aux_elems_compo(const DoubleTab& positions,
                                                              const IntVect& les_polys,
                                                              DoubleVect& les_valeurs,
                                                              int ncomp) const
{
  return le_champ().valeur_aux_elems_compo(positions, les_polys, les_valeurs, ncomp);
}

inline DoubleTab& Champ_Fonc_reprise::valeur_aux_sommets(const Domaine& dom, DoubleTab& sommets) const
{
  return le_champ().valeur_aux_sommets(dom, sommets);
}

inline DoubleVect& Champ_Fonc_reprise::valeur_aux_sommets_compo(const Domaine& dom,
                                                                DoubleVect& sommets, int compo) const
{
  return le_champ().valeur_aux_sommets_compo(dom, sommets, compo);
}

inline DoubleTab& Champ_Fonc_reprise::remplir_coord_noeuds(DoubleTab& coord) const
{
  return le_champ().remplir_coord_noeuds(coord);
}

inline int Champ_Fonc_reprise::remplir_coord_noeuds_et_polys(DoubleTab& coord, IntVect& elems) const
{
  return le_champ().remplir_coord_noeuds_et_polys(coord, elems);
}
inline DoubleVect&   Champ_Fonc_reprise::valeur_aux_compo(const DoubleTab& tab,DoubleVect& les_valeurs, int comp) const
{
  return le_champ().valeur_aux_compo(tab,les_valeurs,comp);
}
#endif
