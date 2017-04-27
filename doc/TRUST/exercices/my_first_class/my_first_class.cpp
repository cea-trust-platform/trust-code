/****************************************************************************
 * * Copyright (c) 2015 - 2016, CEA
 * * All rights reserved.
 * *
 * * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 * *
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * *
 * *****************************************************************************/
/////////////////////////////////////////////////////////////////////////////
////
//// File      : my_first_class.cpp
//// Directory : $BASIC_ROOT/src
////
///////////////////////////////////////////////////////////////////////////////

#include <my_first_class.h>
#include <Param.h>
#include <Domaine.h>
#include <Probleme_base.h>
#include <Zone_VF.h>
#include <Equation_base.h>
#include <SFichier.h>

Implemente_instanciable(my_first_class,"my_first_class",Interprete_geometrique_base) ;

// Method to print the object my_first_class on the output stream os:
Sortie& my_first_class::printOn(Sortie& os) const
{
  Interprete_geometrique_base::printOn(os);
  return os;
}

// Method to read the object my_first_class from the input stream is:
Entree& my_first_class::readOn(Entree& is)
{
  Interprete_geometrique_base::readOn(is);
  return is;
}

// Method called by Interprete_geometrique_base::interpreter() method
// when my_first_class keyword is read in the datafile:
Entree& my_first_class::interpreter_(Entree& is)
{
  Cerr << "-My first keyword!" << finl;

  // Read the domain name thanks to Interprete_geometrique_base::associer_domaine(Entree&) method:
  associer_domaine(is);

  // Declare a param object to read parameters
  Param param(que_suis_je());

  // First parameter
  int option_number=-1;
  param.ajouter("option",&option_number,Param::REQUIRED); 	// option parameter is REQUIRED

  // Second parameter
  Nom problem_name;
  param.ajouter("problem",&problem_name,Param::REQUIRED);       // option parameter is REQUIRED

  // Read the parameters into brackets:
  param.lire_avec_accolades_depuis(is);

  // Check that everything is read:
  Cerr << "-> Option number " << option_number << " has been read on the domain " << domaine(0).le_nom() << finl;

  // Loop on the boundaries to print
  // the name of the boundaries and
  // the number of faces:
  const Zone& zone = domaine().zone(0);
  for (int i=0; i<zone.nb_bords(); i++)
    {
      Cerr << "-> The boundary named " << zone.bord(i).le_nom() << " has " << zone.bord(i).nb_faces() << " faces." << finl;
    }

  // We need to use the control volumes (located in the discretized zone Zone_VF):
  // The discretized zone is only available from the problem:
  Probleme_base& problem=ref_cast(Probleme_base, objet(problem_name));
  const Zone_VF& zone_vf=ref_cast(Zone_VF,problem.equation(0).zone_dis().valeur());
  const DoubleVect& control_volumes=zone_vf.volumes_entrelaces();
  Cerr << "Control volume array size: " << control_volumes.size() << finl;

  //  Sum the control volumes:
  double sum=0;
  for (int face=0; face<zone_vf.nb_faces(); face++)
    sum+=control_volumes(face);

  // Create a file prefixed the name of the data file
  // and print the sum into it:
  Nom filename(Objet_U::nom_du_cas());
  filename+="_result.txt";
  SFichier file(filename);
  file << "Control volumes sum: " << sum << finl;
  file.close();

  // The previous algorithm sum is wrong in parallel
  // The correct one is:
  sum = mp_somme_vect(control_volumes);
  if (Process::je_suis_maitre())
    {
      filename=Objet_U::nom_du_cas();
      filename+="_result.txt";
      file.ouvrir(filename);
      file << "Control volumes sum: " << sum << finl;
    }

  return is;
}
