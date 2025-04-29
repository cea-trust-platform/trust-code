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
          if (fluide->has_porosites())
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
      else if(Motcle(mot) == "DIAMETRES" || Motcle(mot) == "DIAMETERS") // Definition of the parameter diameters
        {
          if (nbSubPhases_ < 0) // Check that the number of underlying phases is defined
            {
              Cerr << "Fluide_MUSIG : you need to define nbPhases before " << mot  << finl;
              Process::exit();
            }

          double dMin=-1, dMax=-1;
          int repartionType=0; ///< save the repartition law of the diameters (0=linear,1=logarithmic)
          is >> mot;
          if (mot != "{")
            {
              Cerr << "Fluide_MUSIG : { expected after the key word diametre instead of " << mot << finl;
              Process::exit();
            }
          for (is >> mot; mot != "}"; is >> mot)
            {
              if (Motcle(mot) == "DMIN")
                {
                  is >> dMin;
                }
              if (Motcle(mot) == "DMAX")
                {
                  is >> dMax;
                }
              if (Motcle(mot) == "LIN")
                {
                  repartionType=0;
                }
              if (Motcle(mot) == "LOG")
                {
                  repartionType=1;
                }
            }
          if(dMin<0 || dMax <0 || dMin > dMax)
            {
              Cerr << "You need to define dMin and dMax with 0 < dMin < dMax " << mot << finl;
              Process::exit();
            }
          //Storing of diameters
          diametres_.resize(nbSubPhases_+1);
          for( int i =0; i<nbSubPhases_+1; i++)
            {
              if(repartionType == 0) // linear
                {
                  diametres_[i] = dMin + i*(dMax-dMin)/(nbSubPhases_);
                }
              else if(repartionType == 1) //logarithmic
                {
                  double lnDiameter = log(dMin)+i*(log(dMax)-log(dMin))/(nbSubPhases_);
                  diametres_[i] = exp(lnDiameter);
                }
              else
                {
                  Cout << "Fluide_MUSIG - Diametre : You need to choose between LIN - LOG for diameters repartitions" << finl;
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

int Fluide_MUSIG::initialiser(const double temps) // This overload might not be necessary
{
  return 1;
}
