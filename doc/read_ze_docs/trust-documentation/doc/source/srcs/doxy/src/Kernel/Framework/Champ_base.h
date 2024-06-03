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


#ifndef Champ_base_included
#define Champ_base_included

#include <TRUSTTabs_forward.h>
#include <Champ_Proto.h>
#include <Field_base.h>
#include <Noms.h>
class Motcle;
class Domaine;
class Domaine_dis_base;
class Format_Post_base;
class Frontiere_dis_base;
class Domaine_Cl_dis_base;

/*! @brief classe Champ_base Cette classe est la base de la hierarchie des champs.
 *
 *      Ses membres sont les attributs et les methodes communs
 *      a toutes les classes qui representent des champs.
 *      Champ_base derive de Champ_Proto, afin d'avoir une interface conforme
 *      a tous les champs, et d'heriter des operations courantes sur les
 *      Champs.
 *      Un champ possede un nom, une unite et des valeurs.
 *
 * @sa Champ_Proto Ch_Inc_base Ch_Don_base, Classe abstraite, Methodes abstraites:, Champ_base& affecter_(const Champ_base& ), Champ_base& affecter_compo(const Champ_base&, int compo )
 */
class Champ_base : public Field_base, public Champ_Proto
{
  Declare_base_sans_constructeur(Champ_base);

public:
  Champ_base();

  //
  // Nouvelles methodes
  //
  virtual int nb_valeurs_nodales() const
  {
    Cerr<<"We do not have to pass here "<<__FILE__<<(int)__LINE__<<finl;
    exit();
    return -1;
  };

  double temps() const;
  virtual double changer_temps(const double t);

  // Attention, il faut avoir fixe nb_comp et nature_du_champ avant
  // de fixer le nb_valeurs_nodales.
  virtual int fixer_nb_valeurs_nodales(int n);
  // Par defaut, ces deux methodes provoquent une erreur. L'appel
  // est invalide sauf si le champ possede un Domaine_dis
  virtual void associer_domaine_dis_base(const Domaine_dis_base&);
  virtual const Domaine_dis_base& domaine_dis_base() const;
  virtual int a_un_domaine_dis_base() const { return 0; } // Par defaut, on ne sait pas si un domain_dis_base sera defini

  virtual void mettre_a_jour(double);
  virtual void abortTimeStep();
  virtual Champ_base& affecter_(const Champ_base&) =0;
  Champ_base& affecter(const Champ_base&);
  void affecter_erreur();
  virtual Champ_base& affecter_compo(const Champ_base&, int compo) =0;
  virtual int imprime(Sortie&, int) const =0;
  // methode supprime pour eviter confusion avec  trace(const Frontiere_dis_base&, DoubleTab&,,double)
  //DoubleTab& trace(const Frontiere_dis_base&, DoubleTab&,int distant) const;
  virtual DoubleTab& trace(const Frontiere_dis_base&, DoubleTab&, double, int distant) const;

  virtual DoubleVect& valeur_a(const DoubleVect& position, DoubleVect& valeurs) const;
  virtual DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& valeurs, int le_poly) const;
  virtual double valeur_a_compo(const DoubleVect& position, int ncomp) const;
  virtual double valeur_a_elem_compo(const DoubleVect& position, int le_poly, int ncomp) const;

  virtual DoubleTab& valeur_aux(const DoubleTab& positions, DoubleTab& valeurs) const;
  virtual DoubleTab& valeur_aux_centres_de_gravite(const DoubleTab& positions, DoubleTab& valeurs) const;
  virtual DoubleVect& valeur_aux_compo(const DoubleTab& positions, DoubleVect& valeurs, int ncomp) const;
  virtual DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& valeurs) const;
  virtual DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& valeurs, int ncomp) const;
  virtual DoubleTab& valeur_aux_elems_smooth(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& valeurs);
  virtual DoubleVect& valeur_aux_elems_compo_smooth(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& valeurs, int ncomp);
  virtual DoubleVect& valeur_a_sommet(int, const Domaine&, DoubleVect&) const;
  virtual double valeur_a_sommet_compo(int, int, int) const;
  virtual DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const;
  virtual DoubleVect& valeur_aux_sommets_compo(const Domaine&, DoubleVect&, int) const;

  /* ces methodes ne s'appliquent que si a_un_domaine_dis_base() */
  virtual DoubleTab& valeur_aux_faces(DoubleTab& result) const;
  virtual DoubleTab valeur_aux_bords() const;

  int calculer_valeurs_som_post(DoubleTab& valeurs, int nbsom, Nom& nom_post, const Domaine& dom) const;
  int calculer_valeurs_som_compo_post(DoubleTab& valeurs, int ncomp, int nbsom, Nom& nom_post, const Domaine& dom, int appliquer_cl = 0) const;
  int calculer_valeurs_elem_post(DoubleTab& valeurs, int nbelem, Nom& nom_post, const Domaine& dom) const;
  int calculer_valeurs_elem_compo_post(DoubleTab& valeurs, int ncomp, int nbelem, Nom& nom_post, const Domaine& dom) const;
  void corriger_unite_nom_compo();
  virtual int completer_post_champ(const Domaine& dom, const int axi, const Nom& loc_post, const Nom& le_nom_champ_post, Format_Post_base& format) const;
  virtual void completer(const Domaine_Cl_dis_base& zcl);

protected:

  double temps_;
};

#endif
