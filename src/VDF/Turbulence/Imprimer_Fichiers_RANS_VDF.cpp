/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Imprimer_Fichiers_RANS_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Imprimer_Fichiers_RANS_VDF.h>
#include <EFichier.h>
#include <Probleme_base.h>
#include <SFichier.h>

Implemente_instanciable(Imprimer_Fichiers_RANS_VDF,"Imprimer_Fichiers_RANS_VDF",Interprete);

Sortie& Imprimer_Fichiers_RANS_VDF::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


Entree& Imprimer_Fichiers_RANS_VDF::readOn(Entree& is)
{
  //   Motcle mot_lu;
  //   Motcle temperature("temperature");

  //   is >> mot_lu;
  //   if(mot_lu==temperature)
  //     {
  //       temperature = 1;
  //       Cerr << "La temperature va etre sauvegardee" << finl;
  //     }

  return Interprete::readOn(is);
}


Entree& Imprimer_Fichiers_RANS_VDF::interpreter(Entree& is)
{

  Nom nom_pb;
  is >> nom_pb;
  if(! sub_type(Probleme_base, objet(nom_pb)))
    {
      Cerr << nom_pb << " est du type " << objet(nom_pb).que_suis_je() << finl;
      Cerr << "On attendait un objet de type Probleme_base" << finl;
      exit();
    }
  Probleme_base& pb=ref_cast(Probleme_base, objet(nom_pb));

  const Zone_VDF& zone_VDF = ref_cast(Zone_VDF,pb.equation(0).zone_dis().valeur());
  const Equation_base& eqn_hydr = pb.equation(0);
  const DoubleVect& vit = eqn_hydr.inconnue().valeurs(); //vitesse
  SFichier fic_vit("vitesse_RANS.dat");

  int ndeb = 0, nfin = zone_VDF.nb_faces_tot() ;

  for(int num_face = ndeb ; num_face < nfin ; num_face ++)
    {
      fic_vit << num_face << " " << vit(num_face) << finl;
    }

  //   if(temperature != 0)
  //     {
  SFichier fic_temp("temperature_RANS.dat");
  const Equation_base& eqn_therm = pb.equation(1);
  const DoubleVect& temp = eqn_therm.inconnue().valeurs(); //temperature

  int ndeb2 = 0, nfin2 = zone_VDF.nb_elem_tot() ;

  for(int num_elem = ndeb2 ; num_elem < nfin2 ; num_elem++)
    {
      fic_temp << num_elem << " " << temp(num_elem) << finl;
    }
  //     }

  return is;
}
