/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Champ_Generique_refChamp_special.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Generique_refChamp_special.h>
#include <Param.h>
#include <Champ_Inc_base.h>

Implemente_instanciable( Champ_Generique_refChamp_special, "Champ_Generique_refChamp_special|refChamp_special", Champ_Generique_refChamp ) ;

Sortie& Champ_Generique_refChamp_special::printOn( Sortie& os ) const
{
  Champ_Generique_refChamp::printOn( os );
  return os;
}

Entree& Champ_Generique_refChamp_special::readOn( Entree& is )
{
  Champ_Generique_refChamp::readOn( is );
  return is;
}
void Champ_Generique_refChamp_special::set_param(Param& param)
{
  Champ_Generique_refChamp::set_param( param);
  param.ajouter("case",&case_,Param::REQUIRED);
}

const DoubleTab& Champ_Generique_refChamp_special::get_ref_values() const
{
  // Appel a get_localisation pour verifier que le champ est bien un champ discret
  // (multi-support ou non)
  get_localisation(0);
  // Renvoie les valeurs du champ
  const DoubleTab& val = ref_cast(Champ_Inc_base,ref_champ_.valeur()).futur(case_);
  return val;
}
const Champ_base& Champ_Generique_refChamp_special::get_champ(Champ& espace_stockage) const
{
  {

    Objet_U& ob = Interprete::objet(nom_pb_);
    const Probleme_base& pb = ref_cast(Probleme_base,ob);
    const Nom& nom_cible = ref_champ_.valeur().le_nom();
    pb.get_champ(nom_cible);

    espace_stockage= get_ref_champ_base();
    if (case_>0)
      ref_cast(Champ_Inc_base,espace_stockage.valeur()).avancer(case_);
    if (case_<0)
      {
        Cerr<<"KO si case_<0; pourquoi ????????,"<<finl;
        ref_cast(Champ_Inc_base,espace_stockage.valeur()).reculer(-case_);
        exit();
      }
    return espace_stockage.valeur();


  }

}
