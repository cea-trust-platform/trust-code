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

#include <SolveurSys_base.h>
#include <EcrFicCollecte.h>
#include <Matrice_Base.h>
#include <MD_Vector_tools.h>


Implemente_base_sans_constructeur(SolveurSys_base,"SolveurSys_base",Objet_U);
SolveurSys_base::SolveurSys_base()
{
  limpr_=0;
  schema_temps_limpr_=1;
  nouvelle_matrice_=1;
  // By default: result will have an updated virtual space:
  echange_ev_resu_ = 1;
  save_matrice_=0;
  reuse_preconditioner_=false;
}

// printOn

Sortie& SolveurSys_base::printOn(Sortie& s ) const
{
  return s  << que_suis_je();
}


// readOn

Entree& SolveurSys_base::readOn(Entree& is )
{
  return is;
}

int SolveurSys_base::resoudre_systeme(const Matrice_Base& A,
                                      const DoubleVect& b,
                                      DoubleVect& x,
                                      int niter_max)
{
  save_matrice_secmem_conditionnel(A, b, x);
  return resoudre_systeme(A,b,x);
}


void SolveurSys_base::save_matrice_secmem_conditionnel(const Matrice_Base& la_matrice, const DoubleVect& secmem, const DoubleVect& solution, int binaire2 )
{
  int binaire=binaire2;
  if (save_matrice_)
    {
      // on simule l'ecriture d'une Matrice et non d'une Matrice_Base
      // pour aider a la relecture
      {
        EcrFicCollecte sortie;
        sortie.set_bin(binaire);
        sortie.ouvrir("Matrice.sa");
        sortie<<la_matrice.que_suis_je()<<finl;
        sortie<<la_matrice;
      }
      // on sauvegarde le Secmem avec les espaces virtuels !!!!!
      {
        EcrFicCollecte sortie;
        sortie.set_bin(binaire);
        sortie.ouvrir("Secmem.sa");
        MD_Vector_tools::dump_vector_with_md(secmem, sortie);
      }
      // on sauvegarde la solution avec les espaces virtuels !!!!!
      {
        EcrFicCollecte sortie;
        sortie.set_bin(binaire);
        sortie.ouvrir("Solution.sa");
        MD_Vector_tools::dump_vector_with_md(solution, sortie);
      }
      Cout <<"Saving of the matrix, secmem and solution ended."<<finl;
    }
}

