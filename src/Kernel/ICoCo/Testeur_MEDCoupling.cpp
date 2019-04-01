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
// File:        Testeur_MEDCoupling.cpp
// Directory:   $TRUST_ROOT/src/Kernel/ICoCo
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Testeur_MEDCoupling.h>
#include <ICoCoMEDField.hxx>
#include <Convert_ICoCoTrioField.h>
#include <Probleme_base.h>
#include <Champ_base.h>

Implemente_instanciable( Testeur_MEDCoupling, "Testeur_MEDCoupling", Interprete ) ;

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
  const Champ_Generique_base& ch=pb.findOutputField(nom_champ);
  ICoCo::MEDField medfield=build_medfield(ch);
  medfield.getField()->writeVTK(nom_du_cas().getString(),false);
#endif
  return is;
}


