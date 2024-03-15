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

#ifndef Ecrire_fichier_xyz_valeur_included
#define Ecrire_fichier_xyz_valeur_included

#include <TRUST_Ref.h>
#include <vector>
#include <string>
#include <Param.h>

class Operateur_Statistique_tps_base;
class Equation_base;
class Champ_base;
class Frontiere;

/*! @brief classe Ecrire_fichier_xyz_valeur This class allows to write user specified field values into a dedicated file
 */
class Ecrire_fichier_xyz_valeur : public Objet_U
{
  Declare_instanciable(Ecrire_fichier_xyz_valeur);

public :
  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle& mot, Entree& is) override;
  void write_fields() const;
  inline void associer_eqn(const Equation_base& eqn)
  {
    eqn_=eqn;
  }

private :

  Entree& fill_vector_from_datafile_(Entree& is, std::vector<std::string>& v);
  bool write_field_during_current_timestep_() const;
  bool getStatField_(const std::string& fname, REF(Champ_base)& field, REF(Operateur_Statistique_tps_base)& op_stat) const;
  void writeValuesOnBoundary_(const std::string& fname, const std::string& bname, const DoubleTab& pos, const DoubleTab& val) const;

  REF(Equation_base) eqn_;
  int binary_file_;
  double dt_; //file writing frequency
  std::vector<std::string> fields_names_;
  std::vector<std::string> boundary_names_;

};

#endif
