/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Fluide_MUSIG.h>

Implemente_instanciable( Fluide_MUSIG, "Fluide_MUSIG", Fluide_base ) ;

Sortie& Fluide_MUSIG::printOn(Sortie& os) const { return Objet_U::printOn(os); }

Entree& Fluide_MUSIG::readOn( Entree& is )
{
  Nom mot;
  int verifFluide = 0;
  is >> mot;
  diametres_.resize(0);
  if (mot != "{")
    {
      Cerr << "Fluide_MUSIG : { expected instead of " << mot << finl;
      Process::exit();
    }
  for (is >> mot; mot != "}"; is >> mot)
    {
      if(Motcle(mot) == "FLUIDE")
        {
          OWN_PTR(Fluide_base) fluide;
          fluide.typer_lire_simple(is, "Typing the fluid medium ...");

          verifFluide++;
          if (fluide->get_porosites_champ().non_nul())
            {
              Cerr << que_suis_je() + " : porosity should be defined only once in the milieu_MUSIG block, not in " + fluide->que_suis_je() << finl;
              Process::exit();
            }
          if (fluide->a_gravite())
            {
              Cerr << que_suis_je() + " : gravity should be defined only once in the milieu_MUSIG block, not in " + fluide->que_suis_je() << finl;
              Process::exit();
            }
          if (fluide->has_hydr_diam())
            {
              Cerr << que_suis_je() + " : diametre_hyd_champ should be defined only once in the milieu_MUSIG block, not in " + fluide->que_suis_je() << finl;
              Process::exit();
            }
          if (!sub_type(Fluide_base,fluide.valeur()))
            {
              Cerr << que_suis_je() + " : only real fluids should be read and not a fluid of type " + fluide->que_suis_je() << finl;
              Process::exit();
            }
          fluide_= fluide;

        }
      else if(Motcle(mot) == "NBPHASES")
        {
          is >> nbSubPhases_;
        }
      else if(Motcle(mot) == "DIAMETRES" || Motcle(mot) == "DIAMETERS") // Definition du parametre diametres
        {
          if (nbSubPhases_ < 0) // on verifie que le nombre de sous phases est eja defini
            {
              Cerr << "Fluide_MUSIG : you need to define nbPhases before " << mot  << finl;
              Process::exit();
            }

          double rMin=-1, rMax=-1;
          int repartionType=0; //repartitionType sauvegarde la loi de repartition des diametre (lineaire =0; exponentiel =1; logarithme =);
          is >> mot;
          if (mot != "{")
            {
              Cerr << "Fluide_MUSIG : { expected after the key word diametre instead of " << mot << finl;
              Process::exit();
            }
          for (is >> mot; mot != "}"; is >> mot)
            {
              if (Motcle(mot) == "RMIN")
                {
                  is >> rMin;
                }
              if (Motcle(mot) == "RMAX")
                {
                  is >> rMax;
                }
              if (Motcle(mot) == "LIN")
                {
                  repartionType=0;
                }
              if (Motcle(mot) == "EXP")
                {
                  repartionType=1;
                }
              if (Motcle(mot) == "LOG")
                {
                  repartionType=2;
                }
            }
          if(rMin<0 || rMax <0 || rMin > rMax)
            {
              Cerr << "You need to define Min and rMax with 0 < rMin < rMax " << mot << finl;
              Process::exit();
            }
          //Stockage dans diametres_
          diametres_.resize(nbSubPhases_);
          for( int i =0; i<nbSubPhases_; i++)
            {
              if(repartionType == 0) //lineaire
                {
                  Cout << "definitions of diameters" << finl;
                  diametres_[i] = rMin + i*(rMax-rMin)/(nbSubPhases_-1);
                  Cout << "End of definitions of diameters" << finl;
                }
              else if(repartionType == 1) //exponentielle
                {
                  Cout << "Fluide_MUSIG - Diametre : repartition exponentielle pas encore implementee" << finl;
                  Process::exit();
                }
              else if(repartionType == 2) //logarithmique
                {
                  Cout << "Fluide_MUSIG - Diametre : repartition logarithmique pas encore impementee" << finl;
                  Process::exit();
                }
              else
                {
                  Cout << "Fluide_MUSIG - Diametre : You need to choose between LIN - EXP - LOG for diameters repartitions" << finl;
                  Process::exit();
                }
            }
        }
    }


  if(verifFluide == 0)
    {
      Cerr << que_suis_je() + ": You need to define a fluid inside FLuide_MUSIG with parameter fluide"  << finl;
      Process::exit();
    }
  else if (verifFluide > 1 )
    {
      Cerr << que_suis_je() + ": You need to define only one fluid inside Fluide_MUSIG"  << finl;
      Process::exit();
    }
  else if (nbSubPhases_ < 1)
    {
      Cerr << que_suis_je() + ": You need to precise the number of sub-phases with parameter nbPhases"  << finl;
      Process::exit();
    }
  return is;
}

int Fluide_MUSIG::initialiser(const double temps) // Cette surcahrge est probablement pas necessaire
{
  return 1;
}
