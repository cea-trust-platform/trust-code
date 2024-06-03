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
#ifndef Format_Post_Lml_included
#define Format_Post_Lml_included

#include <TRUSTTabs_forward.h>
#include <Format_Post_base.h>
#include <EcrFicPartage.h>

/*! @brief : Classe de postraitement des champs euleriens au format lml.
 *
 */

///////////////////////////////////////////////////////////////////////
//    Pour creer un fichier valide, il faut faire:
//    (initialisation) initialize("base_nom_fichier",...);
//                     //base_nom_fichier indique le nom du fichier sans extension ".lml"
//                       //met eventuellement en jeux les informations d autres postraitements
//                       ecrire_entete(temps_courant,reprise,est_le_premier_post)
//                     ecrire_domaine(domaine,est_le_premier_post)
//
//    (pour chaque dt) ecrire_temps(temps_courant)
//                     ecrire_champ(const Domaine& domaine, const Noms& unite_, const Noms& noms_compo,
//                                      int ncomp,double temps_,double temps_courant
//                                      const Nom  & id_du_champ,
//                                    const Nom  & id_du_domaine,
//                                    const Nom  & localisation,
//                                    const DoubleTab & data)
//                    [ecrire_champ(..., data)]
//
//    (finir)          finir(est_le_dernier_post)
//////////////////////////////////////////////////////////////////////

class Format_Post_Lml : public Format_Post_base
{
  Declare_instanciable_sans_constructeur(Format_Post_Lml);
public:
  // Methodes declarees dans la classe de base (interface commune a tous
  // les formats de postraitment de champs):
  void reset() override;
  void set_param(Param& param) override;
  int initialize_by_default(const Nom& file_basename) override;
  int initialize(const Nom& file_basename, const int format, const Nom& option_para) override;
  int ecrire_entete(const double temps_courant, const int reprise, const int est_le_premier_post) override;
  int completer_post(const Domaine& dom, const int axi, const Nature_du_champ& nature, const int nb_compo, const Noms& noms_compo, const Motcle& loc_post, const Nom& le_nom_champ_post) override;

  int preparer_post(const Nom& id_du_domaine, const int est_le_premier_post, const int reprise, const double t_init) override;
  int finir(const int est_le_dernier_post) override;
  int ecrire_domaine(const Domaine& domaine, const int est_le_premie_post) override;
  int ecrire_temps(const double temps) override;

  int ecrire_champ(const Domaine& domaine, const Noms& unite_, const Noms& noms_compo, int ncomp, double temps_, const Nom& id_du_champ, const Nom& id_du_domaine,
                   const Nom& localisation, const Nom& nature, const DoubleTab& data) override;

  int ecrire_item_int(const Nom& id_item, const Nom& id_du_domaine, const Nom& id_domaine, const Nom& localisation, const Nom& reference, const IntVect& data, const int reference_size) override;

  // Methodes statiques specifiques a ce format:

  static int ecrire_domaine_lml(const Domaine& domaine, Nom& nom_fic);
  static int ecrire_temps_lml(const double temps, Nom& nom_fic);

  static int ecrire_champ_lml(const Domaine& domaine, const Noms& unite_, const Noms& noms_compo, int ncomp, double temps_, const Nom& id_du_champ, const Nom& id_du_domaine, const Nom& localisation,
                              const DoubleTab& data, Nom& nom_fic);

  static int ecrire_item_int_lml(const Nom& id_item, const Nom& id_domaine, const IntVect& data, const Nom& nom_fic);

  static int ecrire_entete_lml(Nom& nom_fic, const int est_le_premier_post);
  static int finir_lml(Nom& nom_fic, const int est_le_dernier_post);
  static int completer_post_lml();
  static int preparer_post_lml();

  Format_Post_Lml();

protected:
  Nom  lml_basename_;
};

#endif
