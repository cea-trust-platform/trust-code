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

#include <SolvElem_Gmres.h>

Implemente_instanciable(SolvElem_Gmres,"Gmres",SolvElem_base);
// XD gmres solveur_sys_base gmres -1 Gmres method (for non symetric matrix).
// XD attr impr rien impr 1 Keyword which may be used to print the convergence.
// XD attr quiet rien quiet 1 To disable printing of information
// XD attr seuil floattant seuil 1 Convergence value.
// XD attr diag rien diag 1 Keyword to use diagonal preconditionner (in place of pilut that is not parallel).
// XD attr nb_it_max entier nb_it_max 1 Keyword to set the maximum iterations number for the Gmres.
// XD attr controle_residu entier(into=["0","1"]) controle_residu 1 Keyword of Boolean type (by default 0). If set to 1, the convergence occurs if the residu suddenly increases.
// XD attr save_matrice|save_matrix rien save_matrice 1 to save the matrix in a file.
// XD attr dim_espace_krilov entier dim_espace_krilov 1 not_set


// printOn

Sortie& SolvElem_Gmres::printOn(Sortie& s ) const
{
  return s  ;
}


// readOn

Entree& SolvElem_Gmres::readOn(Entree& is )
{
  return is;
}
