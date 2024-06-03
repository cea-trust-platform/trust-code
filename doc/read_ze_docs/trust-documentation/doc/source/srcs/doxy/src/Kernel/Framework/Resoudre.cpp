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

#include <Resoudre.h>
#include <Probleme_U.h>
#include <Catch_SIGINIT.h>
#include <signal.h>

Implemente_instanciable(Resoudre,"Resoudre|Solve",Interprete);


/*! @brief Simple appel a: Interprete::printOn(Sortie&)
 *
 *     Imprime l'interprete sur un flot de sortie
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Resoudre::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


/*! @brief Simple appel a: Interprete::readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Resoudre::readOn(Entree& is)
{
  return Interprete::readOn(is);
}


/*! @brief Task of the interpretor: Solve a problem (name read from the input stream)
 *
 */
Entree& Resoudre::interpreter(Entree& is)
{
  Nom problem_name;
  is >> problem_name;
  Probleme_U& pb=ref_cast(Probleme_U,objet(problem_name));

  Nom string("=====================================================");
  Cerr << string << finl;
  Cerr << "Initialization of the problem " << pb.le_nom() << " ("<<que_suis_je()<<") in progress ... " << finl;
  Cerr << string << finl;
  // Initialize the problem
  pb.initialize();

  // Register signal and signal handler (SIGINT) if user presses ctrl-c during exec
  if (!Objet_U::DEACTIVATE_SIGINT_CATCH)
    {
      Catch_SIGINIT sig;
      sig.set_nom_cas_pour_signal(pb.nom_du_cas());
      signal(SIGINT, Catch_SIGINIT::signal_callback_handler);
    }

  Cerr << string << finl;
  Cerr << "Solving of the problem " << pb.le_nom() << " ("<<que_suis_je()<<") in progress ... " << finl;
  Cerr << string << finl;
  // Run the problem
  bool ok = pb.run();

  // Terminate the problem
  pb.terminate();
  Cerr << string << finl;
  Cerr << "End of solving of the problem " << pb.le_nom() << " ("<<que_suis_je()<<")" << finl;
  Cerr << string << finl;
  if (!ok)
    {
      Cerr << "Error while solving!" << finl;
      exit();
    }

  return is;
}

