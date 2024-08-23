/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Ecrire_MED_included
#define Ecrire_MED_included

#include <TRUSTTabs_forward.h>
#include <Interprete.h>
#include <TRUST_Ref.h>
#include <med++.h>
#include <Nom.h>
#include <map>
#include <Domaine_forward.h>

#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDCouplingUMesh.hxx>
#include <MEDFileMesh.hxx>
#endif

class Nom;
class Noms;
class Champ_Inc_base;
class Domaine_dis_base;

/*! @brief Classe Ecrire_MED Ecrit un fichier MED.
 *
 * Warning: fields and everything related to discretised domains can only be invoked with the 32 bit version.
 */
template <typename _SIZE_>
class Ecrire_MED_32_64 : public Interprete
{
  Declare_instanciable_32_64(Ecrire_MED_32_64);

public :
  using int_t = _SIZE_;
  using ArrOfInt_t = ArrOfInt_T<_SIZE_>;
  using IntTab_t = IntTab_T<_SIZE_>;
  using Domaine_t = Domaine_32_64<_SIZE_>;

  Ecrire_MED_32_64(const Nom& file_name, const Domaine_t& dom);

  void set_file_name_and_dom(const Nom& file_name, const Domaine_t& dom);

  ///! Set major mode for MED file writing. See major_mode member below.
  void set_major_mode(bool majorMod) { major_mode_ = majorMod; }
  bool get_major_mode() { return major_mode_; }

#ifdef MED_
  inline Nom version()  { return major_mode_ ? _MED_VERSION(MED_NUM_MAJEUR,0,0) : MED_VERSION_STR; }
#else
  inline Nom version()  { return "NOT INSTALLED"; }
#endif

  Entree& interpreter(Entree&) override;

  // This method can be called in both 32 and 64b:
  void ecrire_domaine(bool append=true);
  void ecrire_domaine_dis(const OBS_PTR(Domaine_dis_base)& domaine_dis_base, bool append=true);

  void ecrire_champ(const Nom& type, const Nom& nom_cha1,const DoubleTab& val,const Noms& unite,const Noms& noms_compo,const Nom& type_elem,double time);
  void ecrire_champ(const Nom& type, const Nom& nom_cha1,const DoubleTab& val,const Noms& unite,const Nom& type_elem,double time,const Champ_Inc_base& le_champ);

protected:
  void get_bords_infos(Noms& noms_bords_and_jnts, ArrOfInt_t& sz_bords_and_jnts) const;
  void fill_faces_and_boundaries(const OBS_PTR(Domaine_dis_base)& domaine_dis_base);

  bool major_mode_ = false;   ///< False by default. If true, the MED file will be written in the major mode of the release version (3.0 for example if current MED version is 3.2)
  Nom nom_fichier_;           ///< Name of the MED file to write
  OBS_PTR(Domaine_t) dom_;          ///< Domain that will be written
  std::map<std::string, int> timestep_;
  int mesh_dimension_ = -1;

#ifdef MEDCOUPLING_
  const MEDCoupling::MEDCouplingUMesh *mcumesh_ = nullptr;   ///! Real owner is Domain class
  MEDCoupling::MCAuto<MEDCoupling::MEDFileUMesh> mfumesh_;   ///! Ecrire_MED is the owner
#endif
};

using Ecrire_MED = Ecrire_MED_32_64<int>;
using Ecrire_MED_64 = Ecrire_MED_32_64<trustIdType>;

#endif /* Ecrire_MED_included */
