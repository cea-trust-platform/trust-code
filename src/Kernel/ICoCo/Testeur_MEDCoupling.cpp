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

#include <Testeur_MEDCoupling.h>
#include <ICoCoMEDDoubleField.hxx>
#include <MEDCouplingFieldDouble.hxx>
#include <Convert_ICoCoTrioField.h>
#include <Probleme_base.h>
#include <Champ_base.h>

Implemente_instanciable( Testeur_MEDCoupling, "Testeur_MEDCoupling", Interprete ) ;
// XD testeur_medcoupling interprete testeur_medcoupling 0 not_set
// XD  attr pb_name chaine pb_name 0 Name of domain.
// XD  attr field_name chaine filed_name 0 Name of domain.

Sortie& Testeur_MEDCoupling::printOn( Sortie& os ) const
{
  Interprete::printOn( os );
  return os;
}

Entree& Testeur_MEDCoupling::readOn( Entree& is )
{
  Interprete::readOn( is );
  return is;
}

Entree& Testeur_MEDCoupling::interpreter(Entree& is)
{

  Nom nom_pb,nom_champ;
  is >> nom_pb;
  is >> nom_champ;

#ifdef NO_MEDFIELD
  Cerr<<"Version compiled without MEDCoupling"<<finl;
  exit();
#else

  const Probleme_base& pb = ref_cast(Probleme_base,objet(nom_pb));
  const OBS_PTR(Champ_Generique_base)& ref_ch=pb.findOutputField(nom_champ);
  const Champ_Generique_base& ch= ref_ch.valeur();
  ICoCo::MEDDoubleField medfield=build_medfield(ch);
  medfield.getMCField()->writeVTK(nom_du_cas().getString(),false);
#endif
  return is;
}


