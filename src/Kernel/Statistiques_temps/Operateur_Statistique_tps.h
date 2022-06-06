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

#ifndef Operateur_Statistique_tps_included
#define Operateur_Statistique_tps_included


#include <Operateur_Statistique_tps_base.h>
class Probleme_base;
class Motcle;


Declare_deriv(Operateur_Statistique_tps_base);


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Operateur_Statistique_tps
//     Classe generique de la hierarchie des operateurs statistiques en
//     en temps. Un objet Operateur_Statistique_tps peut referencer
//     n'importe quel objet derivant de Operateur_Statistique_tps_base.
// .SECTION voir aussi
//     Operateur_Statistique_tps_base
//////////////////////////////////////////////////////////////////////////////
class Operateur_Statistique_tps : public DERIV(Operateur_Statistique_tps_base)
{

  Declare_instanciable(Operateur_Statistique_tps);

public:

  inline Operateur_Statistique_tps& operator=(const Operateur_Statistique_tps_base& );
  inline void completer(Probleme_base& pb)
  {
    valeur().completer(pb);
  };
  inline DoubleTab calculer_valeurs() const
  {
    return valeur().calculer_valeurs();
  };
  inline void mettre_a_jour(double un_temps)
  {
    valeur().mettre_a_jour(un_temps);
  };
  inline const Nom& le_nom() const override
  {
    return valeur().le_nom();
  };
  inline double temps() const
  {
    return valeur().temps();
  };
  inline const Integrale_tps_Champ& integrale() const
  {
    return valeur().integrale();
  };
  inline void initialiser(double val)
  {
    valeur().initialiser(val);
  };
  inline void fixer_tstat_deb(double t1, double t2)
  {
    valeur().fixer_tstat_deb(t1,t2);
  };
  inline void fixer_tstat_fin(double t2)
  {
    valeur().fixer_tstat_fin(t2);
  };
  inline int sauvegarder(Sortie& os) const override
  {
    return valeur().sauvegarder(os);
  };
  inline int reprendre(Entree& is) override
  {
    return valeur().reprendre(is);
  };
  inline const Nom& localisation_post() const
  {
    return valeur().localisation_post();
  };
  virtual inline int completer_post_statistiques(const Domaine& dom,const int is_axi,Format_Post_base& format);
};



// Description:
//    Operateur d'affectation d'un Operateur_Statistique_tps_base dans
//    un Operateur_Statistique_tps.
// Precondition:
// Parametre: Operateur_Statistique_tps_base& op_stat_base
//    Signification: la partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Operateur_Statistique_tps&
//    Signification: le resulrtat de l'affectation (*this)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Operateur_Statistique_tps&
Operateur_Statistique_tps::operator=(const Operateur_Statistique_tps_base& op_stat_base)
{
  DERIV(Operateur_Statistique_tps_base)::operator=(op_stat_base);
  return *this;
}

inline int Operateur_Statistique_tps::completer_post_statistiques(const Domaine& dom,const int is_axi,Format_Post_base& format)
{
  return valeur().completer_post_statistiques(dom,is_axi,format);
}
#endif
