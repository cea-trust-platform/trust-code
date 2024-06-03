/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Operateur_Statistique_tps_base_included
#define Operateur_Statistique_tps_base_included

#include <Integrale_tps_Champ.h>
class Probleme_base;
class Domaine_dis_base;
class Domaine;
class Nom;


/*! @brief classe Operateur_Statistique_tps_base Represente des operations statistiques sur les champs.
 *
 *      Des operateurs statistiques sont utilises pour le postraitement.
 *
 * @sa Operateurs_Statistique_tps Postraitement, Classe abstraite, Methodes abstraites:, void completer(const Probleme_base& )
 */
class Operateur_Statistique_tps_base : public Objet_U
{

  Declare_base(Operateur_Statistique_tps_base);

public:

  inline const Nom& localisation_post() const
  {
    return localisation_post_;
  };
  inline void associer(const Nom& loc_post)
  {
    localisation_post_=loc_post;
  };
  inline virtual void associer_op_stat(const Operateur_Statistique_tps_base&)                 {};
  virtual void associer(const Domaine_dis_base& ,const Champ_Generique_base& ,double ,double) =0;
  virtual void fixer_tstat_deb(double, double ) =0;
  virtual void fixer_tstat_fin(double ) =0;
  virtual void mettre_a_jour(double temps) =0;
  const Nom& le_nom() const override =0;
  virtual double temps() const =0;
  virtual const Integrale_tps_Champ& integrale() const =0;
  virtual void initialiser(double val) =0;
  virtual void completer(const Probleme_base& ) =0;
  virtual DoubleTab calculer_valeurs() const =0;
  virtual int completer_post_statistiques(const Domaine& dom,const int is_axi,Format_Post_base& format);
  inline double tstat_deb() const { return tstat_deb_; }
  inline double tstat_fin() const { return tstat_fin_; }
  inline double tstat_dernier_calcul() const { return tstat_dernier_calcul_; }

protected:
  Nom localisation_post_;
  double tstat_deb_=-123.;         //
  double tstat_dernier_calcul_=-123.;     // dernier temps auquel les statistiques ont ete calcules
  double tstat_fin_=-123.;         // valeur choisie par l'utilisateur
};

#endif
