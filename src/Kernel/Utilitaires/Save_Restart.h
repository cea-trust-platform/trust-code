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

#ifndef Save_Restart_included
#define Save_Restart_included

#include <Sortie_Fichier_base.h>
#include <Sortie_Brute.h>
#include <TRUST_Deriv.h>
#include <TRUST_Ref.h>

class Probleme_base;

class Save_Restart
{
public:
  void assoscier_pb_base(const Probleme_base&);

  void allocation() const;
  int allocate_file_size(long int& size) const;
  inline const char* reprise_format_temps() const;

  inline bool& reprise_effectuee() { return restart_done_; }
  inline bool reprise_effectuee() const { return restart_done_; }

  void preparer_calcul() ;

  void sauver_xyz(int) const;

  bool is_restart_in_progress() const { return restart_in_progress_; }
  void set_restart_in_progress(const bool res) { restart_in_progress_ = res; }

  int is_sauvegarde_simple() const { return simple_restart_; }
  const Nom& checkpoint_filename() const { return checkpoint_filename_; }
  const Nom& restart_filename() const { return restart_filename_; }
  const Nom& checkpoint_format() const { return checkpoint_format_; }
  const Nom& yaml_filename() const { return yaml_fname_; }

  void lire_sauvegarde_reprise(Entree& is, Motcle& motlu) ;
  int sauver() const;
  void finir();

private:

  void lire_reprise(Entree& is, Motcle& motlu) ;
  void lire_sauvegarde(Entree& is, Motcle& motlu) ;
  void lire_pdi_sauvegarde_reprise(Entree& is, Motcle& motlu, Nom& restart_file_name, Nom& yaml_fname);

  void setTinitFromLastTime(double last_time);
  void prepare_PDI_restart(int resume_last_time);
  void checkVersion(const Nom& nomfic);

  OBS_PTR(Probleme_base) pb_base_;

  mutable OWN_PTR(Sortie_Fichier_base) ficsauv_;

  mutable Sortie_Brute* osauv_hdf_ = nullptr; // !! DEPRECATED !! TO DELETE !!

  Nom checkpoint_filename_;  // Name of the file for save
  Nom restart_filename_;  // Name of the file for restart
  Nom checkpoint_format_;     // Format for the save restart
  Nom yaml_fname_;         // The name of the yaml file that will be used to initialize PDI (can be provided by the (advanced)user, or is automatically generated)
  bool restart_done_ = false;         // Has a restart been done?
  int simple_restart_ = 0;       // Do we only save the last time step (simple restart) or do we save them all ?
  int restart_version_ = 155;         // Version number, for example 155 (1.5.5) -> used to manage old restart files
  bool restart_in_progress_ = false;  //true variable only during the time step during which a resumption of computation is carried out
  mutable bool ficsauv_created_ = false;      // flag to know if the checkpoint file has been created (useful for PDI)

  static long int File_size_;        // Espace disque pris par les sauvegarde XYZ
  static int Bad_allocate_;        // 1 si allocation reussi, 0 sinon
  static int Nb_pb_total_;        // Nombre total de probleme
  static int Num_pb_;                // numero du probleme
  mutable Nom error_;                // Erreur d'allocation

};

// Method which may be called from anywhere:
inline const char* time_format_from(const int reprise_version)
{
  // Depuis la 155 le format de la balise temps est en scientifique
  // pour eviter des erreurs (incoherence entre temps)
  if (reprise_version<155)
    return "%f";
  else
    return "%e";
}

inline const char* Save_Restart::reprise_format_temps() const
{
  return time_format_from(restart_version_);
}

#endif /* Save_Restart_included */
