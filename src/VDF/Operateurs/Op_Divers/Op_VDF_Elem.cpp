/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Matrice_Morse.h>
#include <Equation_base.h>
#include <Matrix_tools.h>
#include <Op_VDF_Elem.h>
#include <Domaine_Cl_VDF.h>
#include <Array_tools.h>
#include <Periodique.h>
#include <Matrix_tools.h>
#include <Domaine_VDF.h>

void Op_VDF_Elem::dimensionner(const Domaine_VDF& le_dom, const Domaine_Cl_VDF& le_dom_cl, Matrice_Morse& la_matrice, int multi) const
{
  const DoubleTab& inco = le_dom_cl.equation().inconnue().valeurs();
  const int ne = le_dom.nb_elem_tot(), M = inco.line_size();
  const IntTab& f_e = le_dom.face_voisins(), &e_f = le_dom.elem_faces();

  IntTab sten(0, 2);

  for (int e = 0; e < ne; e++)
    for (int i = 0, f, n; i < e_f.dimension(1); i++)
      if ((f = e_f(e, i)) >= 0)
        for (int j = 0; j < 2; j++)
          if ((n = f_e(f, j)) >= 0)
            for (int k = 0; k < M; k++)
              for (int m = (multi ? 0 : k); m < (multi ? M : k + 1); m++)
                sten.append_line(M * e + k, M * n + m);

  tableau_trier_retirer_doublons(sten);
  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), sten, la_matrice);
}

void Op_VDF_Elem:: modifier_pour_Cl(const Domaine_VDF& , const Domaine_Cl_VDF& , Matrice_Morse& , DoubleTab& ) const { /* Do nothing */ }
