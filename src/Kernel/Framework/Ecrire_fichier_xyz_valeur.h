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
#include <Noms.h>
#include <Param.h>
#include <vector>
#include <Domaine_forward.h>

class Operateur_Statistique_tps_base;
class Equation_base;
class Champ_base;

/*! @brief classe Ecrire_fichier_xyz_valeur This class allows to dump fields values on some boundaries into a dedicated text file.
 */
class Ecrire_fichier_xyz_valeur : public Objet_U
{
  Declare_instanciable(Ecrire_fichier_xyz_valeur);

public :
  void set_param(Param& param);
  void write_fields() const;
  inline void associer_eqn(const Equation_base& eqn)
  {
    eqn_=eqn;
  }

private :

  bool write_field_during_current_timestep_() const;
  bool getStatField_(const Nom& fname, REF(Champ_base)& field, REF(Operateur_Statistique_tps_base)& op_stat) const;
  void writeValuesOnBoundary_(const Nom& fname, const std::string& bname, const DoubleTab& pos, const DoubleTab& val) const;

  REF(Equation_base) eqn_;
  int binary_file_;
  double dt_; //file writing frequency
  Noms fields_names_;
  Noms boundary_names_;

};

#endif
