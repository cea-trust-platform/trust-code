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

#ifndef Sondes_included
#define Sondes_included

#include <Champ_Generique_base.h>
#include <Champs_compris.h>
#include <TRUST_List.h>
#include <Champ.h>
#include <Sonde.h>
#include <Noms.h>

/*! @brief classe Sondes Cette classe represente une liste de sondes.
 *
 * @sa Sonde Postraitement
 */
class Sondes : public STLLIST(Sonde)
{
  Declare_instanciable(Sondes);
public:
  inline void ouvrir_fichiers();
  inline void fermer_fichiers();
  void completer();
  inline void init_bords();
  void associer_post(const Postraitement&);
  void postraiter();
  void mettre_a_jour(double temps, double tinit);
  void lire_fichier(const Nom& nom_fichier);
  REF(Champ_base) get_from_cache(REF(Champ_Generique_base)& mon_champ, const Nom& nom_champ_lu_);
  void clear_cache();
  void set_noms_champs_postraitables();
  const Motcles& get_noms_champs_postraitables() const { return noms_champs_postraitables_; }

private:
  // Mecanisme de cache pour les sondes:
  STLLIST(REF(Champ_base)) sourceList;
  STLLIST(Champ) espaceStockageList;
  Noms sourceNoms;
  REF(Postraitement) mon_post;
  Motcles noms_champs_postraitables_;
};

inline void Sondes::init_bords()
{
  auto& list = get_stl_list();
  for (auto& itr : list) itr.init_bords();
}

/*! @brief Ouvre tous les fichiers associes a chacune des sondes de la liste.
 *
 */
inline void Sondes::ouvrir_fichiers()
{
  auto& list = get_stl_list();
  for (auto &itr : list) itr.ouvrir_fichier();
}

/*! @brief Ferme tous les fichiers des sondes de la liste.
 *
 */
inline void Sondes::fermer_fichiers()
{
  auto& list = get_stl_list();
  for (auto &itr : list) itr.fermer_fichier();
}

#endif /* Sondes_included */
