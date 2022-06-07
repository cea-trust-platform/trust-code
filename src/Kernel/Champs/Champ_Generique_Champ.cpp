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

#include <Champ_Generique_Champ.h>
#include <Param.h>

Implemente_instanciable( Champ_Generique_Champ, "Champ_Generique_Champ", Champ_Generique_refChamp ) ;

Sortie& Champ_Generique_Champ::printOn( Sortie& os ) const
{
  Champ_Generique_refChamp::printOn( os );
  return os;
}

Entree& Champ_Generique_Champ::readOn( Entree& is )
{
  Champ_Generique_refChamp::readOn( is );
  return is;
}


// Description:
//  pb_champ :   declenche la lecture du nom du probleme (nom_pb_) auquel appartient
//                 le champ discret et le nom de ce champ discret (nom_champ_)
//  nom_source : option pour nommer le champ en tant que source (sinon nommer par defaut)
void Champ_Generique_Champ::set_param(Param& param)
{
  param.ajouter_non_std("nom_source",(this));
  param.ajouter_non_std("champ",(this),Param::REQUIRED);
  param.ajouter("nom_pb",&nom_pb_,Param::REQUIRED);
}

int Champ_Generique_Champ::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="champ")
    {
      // Lecture du champ
      is >> champ_;
      set_ref_champ(champ_.valeur());
      ref_champ_.reset();
      return 1;
    }
  else
    return Champ_Generique_refChamp::lire_motcle_non_standard(mot,is);
  return 1;
}
void Champ_Generique_Champ::mettre_a_jour(double temps)
{
  champ_.mettre_a_jour(temps);
}
// Description:
//  Voir Champ_Generique_base::get_champ.
//  Ici, l'espace_stockage n'est pas utilise, le champ existe deja
const Champ_base& Champ_Generique_Champ::get_champ(Champ& espace_stockage) const
{
  return champ_.valeur();
}

const Champ_base& Champ_Generique_Champ::get_ref_champ_base() const
{
  return champ_.valeur();
}
void Champ_Generique_Champ::reset()
{
  champ_.detach();
  Champ_Generique_refChamp::reset();
}
