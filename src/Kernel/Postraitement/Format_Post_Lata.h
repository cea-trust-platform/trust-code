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

#ifndef Format_Post_Lata_included
#define Format_Post_Lata_included

#include <TRUSTTabs_forward.h>
#include <Format_Post_base.h>

class Fichier_Lata;
class SFichier;

/*! @brief : Classe de postraitement des champs euleriens au format lata
 *
 */

//Pour creer un fichier valide, il faut faire:
//    (initialisation) initialize("base_nom_fichier", ASCII, SINGLE_FILE);
//                       ecrire_entete(temps_courant,reprise,est_le_premier_post)
//                     ecrire_domaine(domaine,est_le_premier_post)

//    (pour chaque dt) ecrire_temps(temps_courant)
//                     ecrire_champ(const Domaine& domaine, const Noms& unite_, const Noms& noms_compo,
//                                      int ncomp,double temps_,double temps_courant
//                                      const Nom  & id_du_champ,
//                                    const Nom  & id_du_domaine,
//                                    const Nom  & localisation,
//                                    const DoubleTab & data)
//                    [...                            ]

//  Si option_para = MULTIPLE_FILES :
//   on ecrit un fichier de postraitement par processeur
//  Sinon :
//   on ecrit un fichier unique qui contient l'ensemble des donnees.
//   Les methodes doivent alors etre appelees simultanement sur tous
//   les processeurs.
//  Les specifications du format LATA sont decrites dans le fichier
//   Specifications_lata_v2.pdf

class Format_Post_Lata : public Format_Post_base
{
  Declare_instanciable_sans_constructeur(Format_Post_Lata);
public:
  //
  // Methodes declarees dans la classe de base (interface commune a tous
  // les formats de postraitment de champs):
  //
  void set_param(Param& param) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;

  void reset() override;
  void resetTime(double t, const std::string dirname) override;

  int completer_post(const Domaine& dom, const int axi, const Nature_du_champ& nature, const int nb_compo, const Noms& noms_compo, const Motcle& loc_post,
                     const Nom& le_nom_champ_post) override;
  int preparer_post(const Nom& id_du_domaine, const int est_le_premier_post, const int reprise, const double t_init) override;

  int initialize_by_default(const Nom& file_basename) override;
  int initialize(const Nom& file_basename, const int format, const Nom& option_para) override;

  int modify_file_basename(const Nom file_basename, bool for_restart, const double tinit) override;

  int ecrire_entete(const double temps_courant, const int reprise, const int est_le_premier_post) override;
  int ecrire_domaine(const Domaine& domaine, const int est_le_premier_post) override;
  int ecrire_temps(const double temps) override;
  int ecrire_champ(const Domaine& domaine, const Noms& unite_, const Noms& noms_compo, int ncomp, double temps_, const Nom& id_du_champ, const Nom& id_du_domaine,
                   const Nom& localisation, const Nom& nature, const DoubleTab& data) override;
  int ecrire_item_int(const Nom& id_item, const Nom& id_du_domaine, const Nom& id_domaine, const Nom& localisation, const Nom& reference,
                      const IntVect& data, const int reference_size) override;

  int finir(const int est_le_dernier_post) override;

  //
  // Other methods
  //
  virtual int reconstruct(const Nom file_basename, const Nom, const double tinit);
  virtual int finir_sans_reprise(const Nom file_basename);

  void ecrire_domaine_low_level(const Nom& id_dom, const DoubleTab& sommets, const IntTab& elements, const Motcle& type_elem);

  // When writing joints, we need to write trustIdType (=big) values - only for LATA (?):
  int ecrire_item_tid(const Nom& id_item, const Nom& id_du_domaine, const Nom& id_domaine, const Nom& localisation,
                      const Nom& reference, const TIDVect& data, const int reference_size);

  //
  // Methodes specifiques a ce format:
  //
  enum Format { ASCII, BINAIRE };
  enum Options_Para { SINGLE_FILE, SINGLE_FILE_MPIIO, MULTIPLE_FILES };
  enum Status { RESET, INITIALIZED, WRITING_TIME };

  Format_Post_Lata();
  virtual int initialize_lata(const Nom& file_basename, const Format format = ASCII, const Options_Para options_para = SINGLE_FILE);

  static int ecrire_entete_lata(const Nom& base_name, const Options_Para& option, const Format& format, const int est_le_premier_post);
  static int ecrire_temps_lata(const double temps, double& temps_format, const Nom& base_name, Status& stat, const Options_Para& option);

  void set_single_lata_option(const bool sing_lata) override { un_seul_fichier_lata_ = sing_lata; }

  static const char * extension_lata();
  static const char * remove_path(const char * filename);

protected:
  static trustIdType write_doubletab(Fichier_Lata& fichier, const DoubleTab& tab, int& nb_colonnes, const Options_Para& option);
  static trustIdType write_inttab(Fichier_Lata& fichier, bool decal_fort, trustIdType decalage_partiel, const IntTab& tab, int& nb_colonnes, const Options_Para& option);

  Nom lata_basename_;
  Format format_;
  Options_Para options_para_;
  Status status;

  bool restart_already_moved_;  // Whether the postprocess file was already renamed/move after a restart

  double temps_courant_;  // The time being written
  double tinit_;
  bool un_seul_fichier_lata_ = false;
  long int offset_elem_ = -1, offset_som_ = -1; // offset used if single_lata

private:
  template<typename TYP>
  int ecrire_item_integral_T(const Nom& id_item, const Nom& id_du_domaine, const Nom& id_domaine, const Nom& localisation,
                             const Nom& reference, const TRUSTVect<TYP, int>& val, const int reference_size);
  void ecrire_offset(SFichier& sfichier, long int offset);
};

#endif /* Format_Post_Lata_included */
