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
// File:        Format_Post_Lata.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Format_Post_Lata_included
#define Format_Post_Lata_included

#include <TRUSTTabs_forward.h>
#include <Format_Post_base.h>

class Fichier_Lata;

// .DESCRIPTION        :
//  Classe de postraitement des champs euleriens au format lata.

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
  // Methodes declarees dans la classe de base (interface commune a tous
  // les formats de postraitment de champs):
  void reset() override;
  void set_param(Param& param) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  int initialize_by_default(const Nom& file_basename) override;
  int initialize(const Nom& file_basename, const int format, const Nom& option_para) override;
  int modify_file_basename(const Nom file_basename, const int a_faire, const double tinit) override;
  virtual int reconstruct(const Nom file_basename, const Nom, const double tinit);
  virtual int finir_sans_reprise(const Nom file_basename);
  int ecrire_entete(const double temps_courant, const int reprise, const int est_le_premier_post) override;
  int completer_post(const Domaine& dom, const int axi, const Nature_du_champ& nature, const int nb_compo, const Noms& noms_compo, const Motcle& loc_post, const Nom& le_nom_champ_post) override;

  int preparer_post(const Nom& id_du_domaine, const int est_le_premier_post, const int reprise, const double t_init) override;
  int ecrire_domaine(const Domaine& domaine, const int est_le_premier_post) override;
  int ecrire_temps(const double temps) override;

  int finir(int& est_le_dernier_post) override;

  int ecrire_champ(const Domaine& domaine, const Noms& unite_, const Noms& noms_compo, int ncomp, double temps_, double temps_courant, const Nom& id_du_champ, const Nom& id_du_domaine,
                   const Nom& localisation, const Nom& nature, const DoubleTab& data) override;

  int ecrire_item_int(const Nom& id_item, const Nom& id_du_domaine, const Nom& id_zone, const Nom& localisation, const Nom& reference, const IntVect& data, const int reference_size) override;


  //Actuellement on commente ces methodes car pas decrites dans l interface commune
  //aux differentes classes dans le CDC.
  //Ces methodes sont declarees dans Format_Post_base

  //Methode ecrire_bords() on utilise nb_sommets alors que sommets dans Format_Post_base
  /*
    virtual int ecrire_bords(const Nom    & id_du_domaine,
    const Motcle & type_faces,
    const int   nb_sommets,
    const IntTab & faces_sommets,
    const IntTab & faces_num_bord,
    const Noms   & bords_nom);

    virtual int ecrire_noms(const Nom  & id_de_la_liste,
    const Nom  & id_du_domaine,
    const Nom  & localisation,
    const Noms & liste_noms);
  */

  //Methodes statiques appelees par les methodes communes de l interface
  // Methodes specifiques a ce format:
  enum Format { ASCII, BINAIRE };
  enum Options_Para { SINGLE_FILE, SINGLE_FILE_MPIIO, MULTIPLE_FILES };
  enum Status { RESET, INITIALIZED, WRITING_TIME };


  Format_Post_Lata();
  virtual int initialize_lata(const Nom& file_basename, const Format format = ASCII, const Options_Para options_para = SINGLE_FILE);

  static int ecrire_entete_lata(const Nom& base_name, const Options_Para& option, const Format& format, const int est_le_premier_post);
  static int ecrire_temps_lata(const double temps, double& temps_format, const Nom& base_name, Status& stat, const Options_Para& option);

protected:

  static int write_doubletab(Fichier_Lata& fichier, const DoubleTab& tab, int& nb_colonnes, const Options_Para& option, const int bloc_number = 0, const int nb_blocs = 1);

  static int write_inttab(Fichier_Lata& fichier, int decalage, int decalage_partiel, const IntTab& tab, int& nb_colonnes, const Options_Para& option);

  int deja_fait_, file_existe_;
  double tinit_;
  Nom lata_basename_;
  Format format_;
  Options_Para options_para_;
  Status status;


  //Passe en public pour etre utilise dans ecrire_temps_lata
  // L'etat actuel du fichier:
  //  (on est dans l'etat writing_time apres le premier appel a ecrire temps)
  ////enum Status { RESET, INITIALIZED, WRITING_TIME };
  ////Status status;

  // Le temps en cours d'ecriture
  double temps_courant_;

public:
  static const char * extension_lata();
  static const char * remove_path(const char * filename);
};

#endif
