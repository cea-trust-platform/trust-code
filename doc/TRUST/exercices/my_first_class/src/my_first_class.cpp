/****************************************************************************
* Copyright (c) 2017, CEA
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

#include <my_first_class.h>
#include <Param.h>
#include <Domaine.h>
#include <Probleme_base.h>
#include <Domaine_VF.h>
#include <Equation_base.h>
#include <SFichier.h>

Implemente_instanciable( my_first_class, "my_first_class", Interprete ) ;

// Method to print the object my_first_class on the output stream os:
Sortie& my_first_class::printOn(Sortie& os) const
{
  return Interprete::printOn( os );
}

// Method to read the object my_first_class from the input stream is:
Entree& my_first_class::readOn(Entree& is)
{
  return Interprete::readOn( is );
}

// Method called by Interprete_geometrique_base::interpreter() method
// when my_first_class keyword is read in the datafile:
Entree& my_first_class::interpreter(Entree& is)
{
// XD my_first_class interprete my_first_class -3 First class created in the baltik tutorial
  Cerr << "- My first keyword!" << finl;

  // Declare a param object to read parameters
  Param param(que_suis_je());

  // First parameter
  Nom domain_name;
  param.ajouter("domaine",&domain_name,Param::REQUIRED);  // XD_ADD_P chaine domain name

  // Second parameter
  int option_number=-1;
  param.ajouter("option",&option_number,Param::REQUIRED);  // XD_ADD_P int parameter

  // Third parameter
  Nom problem_name;
  param.ajouter("problem",&problem_name,Param::REQUIRED);  // XD_ADD_P chaine problem name

  // Read the parameters into brackets:
  param.lire_avec_accolades_depuis(is);

  // Retrieve the domain object name given by the user, and find back the actual instance in memory:
  Objet_U& obj = Interprete::objet(domain_name);
  if(!sub_type(Domaine, obj))
    Process::exit("Object passed to 'my_first_class' is of wrong type!");
  // Fill our local reference:
  domaine_ref_ = ref_cast(Domaine, obj);

  // Check that everything is read:
  Cerr << "- Option number " << option_number << " has been read on the domain " << domaine_ref_->le_nom() << "." << finl;

  // Loop on the boundaries to print
  // the name of the boundaries and
  // the number of faces:
  for (int i=0; i<domaine_ref_->nb_bords(); i++)
    {
      Cerr << "- The boundary named " << domaine_ref_->bord(i).le_nom() << " has " << domaine_ref_->bord(i).nb_faces() << " faces." << finl;
    }

  // We need to use the control volumes (located in the discretized domaine Domaine_VF):
  // The discretized domain is only available from the problem:
  Probleme_base& problem=ref_cast(Probleme_base, Interprete::objet(problem_name));
  Cerr << "- The problem name is " << problem.le_nom() << "." <<finl;

  Cerr<<"- There are " << problem.nombre_d_equations() << " equations."<<finl;
  for (int i=0; i<problem.nombre_d_equations(); i++)
    Cerr<<"- The equation number " << i << " is of type " << problem.equation(i).le_type() << "." << finl;

  const Domaine_VF& dom_vf=ref_cast(Domaine_VF,problem.domaine_dis());
  const DoubleVect& control_volumes=dom_vf.volumes_entrelaces();
  Cerr << "- Control volume array size: " << control_volumes.size() << finl;

  // Sum the control volumes:
  double sum=0;
  for (int face=0; face<dom_vf.nb_faces(); face++)
    sum+=control_volumes(face);
  Cerr<<"- Control volume sum = "<< sum << finl;

  // Using mp_sum()
  double sum2 = Process::mp_sum(sum);
  Cerr<<"- Control volume sum with mp_sum = "<< sum2 << finl;

  // The previous algorithm sum is wrong in parallel
  // The correct one is:
  double sum3 = mp_somme_vect(control_volumes);
  Cerr <<"- Control volume sum with mp_somme_vect = " << sum3 << finl;

  // Create a file prefixed the name of the data file
  Nom filename(Objet_U::nom_du_cas());
  Cerr<<"- Name of the test case = "<< filename << "." << finl;
  filename+="_result.txt";
  Cerr<<"- Name of the file = "<< filename << "." << finl;

  // Uniquement le process maitre qui ecrit
  if (Process::je_suis_maitre())
    {
      SFichier file(filename);
      file << "Control volumes sum: " << sum << finl;
      file << "Control volumes sum with mp_sum: " << sum2 << finl;
      file << "Control volume sum with mp_somme_vect = " << sum3 << finl;
      file.close();
    }

  return is;
}

