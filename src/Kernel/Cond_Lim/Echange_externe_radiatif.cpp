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

Implemente_instanciable(Echange_externe_radiatif, "Echange_externe_radiatif|Paroi_echange_externe_radiatif", Echange_externe_impose);

// XD echange_externe_radiatif condlim_base echange_externe_radiatif -1 Combines radiative $(sigma * eps * (T^4 - T_ext^4))$ and convective $(h * (T - T_ext))$ heat transfer boundary conditions, where sigma is the Stefan-Boltzmann constant, eps is the emi
// XD attr h_imp chaine(into=["h_imp","t_ext","emissivite"]) h_imp 0 Heat exchange coefficient value (expressed in W.m-2.K-1).
// XD attr himpc front_field_base himpc 0 Boundary field type.
// XD attr emissivite chaine(into=["emissivite","h_imp","t_ext"]) emissivite 0 Emissivity coefficient value.
// XD attr emissivitebc front_field_base emissivitebc 0 Boundary field type.
// XD attr t_ext chaine(into=["t_ext","h_imp","emissivite"]) t_ext 0 External temperature value (expressed in oC or K).
// XD attr ch front_field_base ch 0 Boundary field type.
// XD attr temp_unit chaine(into=["temperature_unit"]) temp_unit 0 Temperature unit
// XD attr temp_unit_val chaine(into=["kelvin","celsius"]) temp_unit_val 0 Temperature unit

Sortie& Echange_externe_radiatif::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Echange_externe_radiatif::readOn(Entree& s)
{
  if (app_domains.size() == 0)
    app_domains = { Motcle("Thermique") };

  if (supp_discs.size() == 0)
    supp_discs = { Nom("VDF"), Nom("VEFPreP1B")};

  Motcle motlu, T_unit;
  Motcles les_motcles(4);
  {
    les_motcles[0] = "emissivite";
    les_motcles[1] = "h_imp";
    les_motcles[2] = "T_ext";
    les_motcles[3] = "Temperature_unit";
  }

  int ind = 0;
  while (ind < 4)
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
        case 3:
          {
            s >> T_unit;

            if (T_unit == "CELSIUS")
              {
                // TODO FIXME LATER
                Cerr << "Error in Echange_externe_radiatif::readOn !!! This boundary condition requires temperature in Kelvin !" << finl;
                Cerr << "Please update your data file ..." << finl;
                Process::exit();
                t_en_kelvin_ = false;
              }
            else if (T_unit == "KELVIN")
              t_en_kelvin_ = true;
            else
              {
                Cerr << "Unkown option in Echange_externe_radiatif::readOn !!!" << finl;
                Cerr << "   - Temperature_unit can either be CELSIUS or KELVIN !!!" << finl;
                Process::exit();
              }

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
