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
// File:        Imprimer_Fichiers_RANS_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <Imprimer_Fichiers_RANS_VEF.h>
#include <SFichier.h>
#include <EFichier.h>
#include <Probleme_base.h>
#include <Zone.h>

Implemente_instanciable(Imprimer_Fichiers_RANS_VEF,"Imprimer_Fichiers_RANS_VEF",Interprete);

Sortie& Imprimer_Fichiers_RANS_VEF::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


Entree& Imprimer_Fichiers_RANS_VEF::readOn(Entree& is)
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


Entree& Imprimer_Fichiers_RANS_VEF::interpreter(Entree& is)
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


  const Zone_VEF& zone_VEF = ref_cast(Zone_VEF,pb.equation(0).zone_dis().valeur());
  const Equation_base& eqn_hydr = pb.equation(0);
  const DoubleTab& vit = eqn_hydr.inconnue().valeurs(); //vitesse

  //   const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  //   const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  //   const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  int nb_faces = zone_VEF.nb_faces();

  SFichier fic("vitesse_RANS.dat");

  for(int num_face=0 ; num_face<nb_faces ; num_face++)
    {
      fic << vit(num_face,0) << " " << vit(num_face,1) << " " << vit(num_face,2) << finl;
    }


  return is;
}
