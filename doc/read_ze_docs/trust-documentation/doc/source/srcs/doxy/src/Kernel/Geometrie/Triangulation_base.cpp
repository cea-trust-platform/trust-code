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

#include <Triangulation_base.h>
#include <Domaine.h>
#include <Scatter.h>

Implemente_base(Triangulation_base,"Triangulation_base",Interprete_geometrique_base);

Sortie& Triangulation_base::printOn(Sortie& os) const
{
  return os;
}

Entree& Triangulation_base::readOn(Entree& is)
{
  return is;
}

Entree& Triangulation_base::interpreter_(Entree& is)
{
  if(dimension !=dimension_application())
    {
      Cerr << "Interpreter "<<que_suis_je()<<" cannot be applied for dimension " << dimension <<finl;
      exit();
    }
  if (Process::nproc()>1)
    {
      Cerr << "Interpreter "<<que_suis_je()<<" cannot be applied during a parallel calculation !" << finl;
      Cerr << "The mesh can't be changed after it has been partitioned." << finl;
      Process::exit();
    }

  associer_domaine(is);
  Scatter::uninit_sequential_domain(domaine());
  trianguler(domaine());
  Scatter::init_sequential_domain(domaine());
  return is;
}
