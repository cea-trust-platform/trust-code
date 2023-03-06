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

#ifndef EcrMED_included
#define EcrMED_included

#include <TRUSTTabs_forward.h>
#include <Interprete.h>
#include <TRUST_Ref.h>
#include <med++.h>
#include <Nom.h>
#include <map>

class Nom;
class Noms;
class Champ_Inc_base;
class Domaine_dis_base;
class Domaine;

/*! @brief Classe EcrMED Ecrit un fichier MED.
 *
 *     Structure du jeu de donnee (en dimension 2) :
 *     EcrMED dom medfile
 */
class EcrMED : public Interprete
{
  Declare_instanciable(EcrMED);
public :
  EcrMED(const Nom& file_name, const Domaine& dom);

  void set_file_name_and_dom(const Nom& file_name, const Domaine& dom);

  ///! Set major mode for MED file writing. See major_mode member below.
  void set_major_mode(bool majorMod) { major_mode_ = majorMod; }
  bool get_major_mode() { return major_mode_; }

  inline Nom version()  { return major_mode_ ? _MED_VERSION(MED_NUM_MAJEUR,0,0) : MED_VERSION_STR; }

  Entree& interpreter(Entree&) override;

  void ecrire_domaine(bool append=true);
  void ecrire_domaine_dis(const REF(Domaine_dis_base)& domaine_dis_base, bool append=true);

  void ecrire_champ(const Nom& type, const Nom& nom_cha1,const DoubleTab& val,const Noms& unite,const Noms& noms_compo,const Nom& type_elem,double time);
  void ecrire_champ(const Nom& type, const Nom& nom_cha1,const DoubleTab& val,const Noms& unite,const Nom& type_elem,double time,const Champ_Inc_base& le_champ);

protected:
  void creer_all_faces_bord(Noms& type_face,IntTabs& all_faces_bord, Noms& noms_bords,ArrsOfInt& familles);

  bool major_mode_ = false;   ///< False by default. If true, the MED file will be written in the major mode of the release version (3.0 for example if current MED version is 3.2)
  Nom nom_fichier_;           ///< Name of the MED file to write
  REF(Domaine) dom_;          ///< Domain that will be written
  std::map<std::string, int> timestep_;
};

#endif /* EcrMED_included */
