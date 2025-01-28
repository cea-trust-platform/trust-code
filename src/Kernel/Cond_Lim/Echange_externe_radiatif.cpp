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

#include <Echange_externe_radiatif.h>

Implemente_instanciable(Echange_externe_radiatif, "Echange_externe_radiatif", Echange_externe_impose);
// XD paroi_echange_externe_radiatif paroi_echange_externe_impose paroi_echange_externe_radiatif -1 Combines radiative (sigma * eps * (T^4 - T_ext^4)) and convective (h * (T - T_ext)) heat transfer boundary conditions, where sigma is the Stefan-Boltzmann constant, eps is the emissivity, h is the convective heat transfer coefficient, T is the surface temperature, and T_ext is the external temperature.
// XD attr emissivite chaine emissivite 0 Emissivity coefficient value.
// XD attr emissivitebc front_field_base emissivitebc 0 Boundary field type.

Sortie& Echange_externe_radiatif::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Echange_externe_radiatif::readOn(Entree& s)
{
  if (app_domains.size() == 0)
    app_domains = { Motcle("Thermique") };

  if (supp_discs.size() == 0)
    supp_discs = { Nom("VDF"), Nom("VEFPreP1B")};

  Motcle motlu;
  Motcles les_motcles(3);
  {
    les_motcles[0] = "emissivite";
    les_motcles[1] = "h_imp";
    les_motcles[2] = "T_ext";
  }

  int ind = 0;
  while (ind < 3)
    {
      s >> motlu;
      int rang = les_motcles.search(motlu);

      switch(rang)
        {
        case 0:
          {
            s >> emissivite_;
            break;
          }
        case 1:
          {
            s >> h_imp_;
            break;
          }
        case 2:
          {
            s >> le_champ_front;
            break;
          }
        default:
          {
            Cerr << "Error while reading BC of type Echange_externe_radiatif " << finl;
            Cerr << "we expected a keyword among: " << les_motcles << " instead of " << motlu << finl;
            exit();
          }
        }
      ind++;
    }

  return s;
}
