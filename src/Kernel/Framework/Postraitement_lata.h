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
// File:        Postraitement_lata.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Postraitement_lata_included
#define Postraitement_lata_included

#include <Postraitement_base.h>
#include <Parser_U.h>
#include <TRUSTTab.h>
#include <Motcle.h>

class Comm_Group;
class Zone;
class Fichier_lata;
class Postraitement_lata_private_data;
class Zone_dis_base;

// .DESCRIPTION        :
//  Classe de postraitement des champs euleriens au format lata.
//  On peut l'utiliser sans creer d'instance de la classe:
//    Pour creer un fichier valide, il faut faire:
//    (initialisation) ecrire_entete(...)
//                     ecrire_zone(..., zone1, ...)
//                    [ecrire_zone(..., zone2, ...)]
//                    [...                         ]
//    (pour chaque dt) ecrire_temps(..., temps, ...)
//                     ecrire_champ(..., valeurs, ...)
//                    [ecrire_champ(..., valeurs, ...)]
//                    [...                            ]
//  Lors des appels aux fonctions statiques, si comm_group == 0,
//   on ecrit un fichier de postraitement par processeur, sinon on
//   creee un fichier unique partage.
//
//  Si on cree une instance de la classe, l'entete du fichier
//  est ecrite lors du premier appel a "postraitement".

class Postraitement_lata : public Postraitement_base
{
  Declare_instanciable_sans_constructeur(Postraitement_lata);
public:
  Postraitement_lata();
  void set_param(Param& param) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  void postraiter(int forcer) override;
  inline void completer() override {}

  enum Format { ASCII, BINAIRE };
  enum Type_Champ { CHAMP=0, STATISTIQUE=1 };

  static void ecrire_entete(const Nom& basename,
                            const Nom& discretisation,
                            Format format = ASCII,
                            const Comm_Group * comm_group = 0);
  static void ecrire_temps(const Nom& basename,
                           const double temps,
                           Format format = ASCII,
                           const Comm_Group * comm_group = 0);
  static void ecrire_zone(const Nom& basename,
                          const Zone&,
                          Format format = ASCII,
                          const Comm_Group * comm_group = 0,
                          const Zone_dis_base * la_zone_dis = 0);

  static void ecrire_champ(const Nom&         basename,
                           const Nom&         nom_du_champ,
                           Type_Champ         type,
                           char               type_char,
                           const Nom&         nom_domaine,
                           const Nom&         nom_probleme,
                           double             temps,
                           Localisation       localisation,
                           const DoubleTab& data,
                           Format             format = ASCII,
                           const Comm_Group * comm_group = 0);

  static const Nom& extension_lata();
  static const char * remove_path(const char * filename);
  static int finir_lata(const Nom& basename,
                        const Comm_Group * comm_group = 0);
protected:
  void lire_format(Entree&);
  void lire_champs(Entree& is);

  int dt_post_ecoule(double& dt_post_, double temps_dernier_post);
  void postraiter_liste_champs(Localisation loc, const Motcles& liste);

  // Nom de base du fichier .lata -> "lata_basename_.lata"
  Nom lata_basename_;
  // Ascii ou Binaire ?
  Format format_;
  // 1 => Un fichier par processeur, sinon un fichier partage unique.
  int fichiers_multiples_;
  // 1 => L'entete du fichier lata a deja ete ecrite.
  int entete_ok_;
  // 1 => ne pas ecrire l'entete ni le TEMPS (si ce postraitement ecrit
  // dans le meme fichier qu'un autre, il ne faut pas ecrire l'entete a
  // nouveau).
  int skip_header_;
  // Intervalle de postraitement
  double dt_post_;
  // Temps du dernier postraitement
  Parser_U fdt_post;
  // fonction Parser associee
  double temps_dernier_post_;
  // Liste de noms de champs a postraiter
  Motcles liste_champs_aux_sommets;
  Motcles liste_champs_aux_elements;
  Motcles liste_champs_aux_faces;
  // Affichage detaille des operations (pointeur nul si on n'en veut pas)
  Sortie *verbose_stream_;
private:
  // Quelques donnees privees utilisees en interne:
  Postraitement_lata_private_data *private_data_;
};

#endif
