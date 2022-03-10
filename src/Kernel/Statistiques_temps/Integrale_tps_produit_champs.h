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
// File:        Integrale_tps_produit_champs.h
// Directory:   $TRUST_ROOT/src/Kernel/Statistiques_temps
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Integrale_tps_produit_champs_included
#define Integrale_tps_produit_champs_included

#include <Integrale_tps_Champ.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Integrale_tps_produit_champs
//    Cette classe represente l'integrale en temps d'un produit de 2 champs
//    eleves a une puissance entiere et positive entre un temps initial et un
//    temps courant
//    Un objet Integrale_tps_produit_champs a donc 2 champs associes dont
//    il represente l'integrale du produit.
// .SECTION voir aussi
//    Classe hors hierarchie TrioU
//////////////////////////////////////////////////////////////////////////////
class Integrale_tps_produit_champs : public Integrale_tps_Champ
{
  Declare_instanciable(Integrale_tps_produit_champs);
public:

  inline const REF(Champ_Generique_base)& mon_premier_champ() const
  {
    return mon_champ;
  };
  inline const REF(Champ_Generique_base)& mon_second_champ() const
  {
    return mon_second_champ_;
  };
  inline int premiere_puissance() const
  {
    return puissance;
  }
  inline int seconde_puissance() const
  {
    return seconde_puissance_;
  }
  inline const int& get_support_different() const
  {
    return support_different_;
  };
  inline int& support_different()
  {
    return support_different_;
  };
  inline void associer(const Champ_Generique_base&, const Champ_Generique_base&, int, int, double, double);
  inline void mettre_a_jour(double ) override;
  void mettre_a_jour_integrale() override;
  void ajoute_produit_tensoriel(double, const Champ_base&, const Champ_base&);

protected :

  REF(Champ_Generique_base) mon_second_champ_;
  int seconde_puissance_;
  int support_different_;
};

// Description:
//    Associe les champs et les temps
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
// Postcondition:
inline void Integrale_tps_produit_champs::associer(const Champ_Generique_base& ch1, const Champ_Generique_base& ch2, int n1, int n2, double t0, double t1)
{
  Integrale_tps_Champ::associer(ch1, n1, t0, t1);
  mon_second_champ_=ch2;
  seconde_puissance_=n2;
}

inline void Integrale_tps_produit_champs::mettre_a_jour(double )
{
  mettre_a_jour_integrale();
  Champ espace_stockage_source;
  const Champ_base& source = mon_champ->get_champ(espace_stockage_source);
  changer_temps(source.temps());
}

#endif
