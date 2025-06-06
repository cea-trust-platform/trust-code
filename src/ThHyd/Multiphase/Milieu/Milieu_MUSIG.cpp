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

#include <Milieu_MUSIG.h>

Implemente_instanciable( Milieu_MUSIG, "Milieu_MUSIG", Milieu_composite ) ;
// XD Milieu_MUSIG listobj Milieu_MUSIG -1 milieu_base 0 MUSIG medium made of several sub mediums.

Sortie& Milieu_MUSIG::printOn(Sortie& os) const { return Objet_U::printOn(os); }

Entree& Milieu_MUSIG::readOn( Entree& is )
{
  int i = 0;
  std::vector<std::pair<std::string, int>> especes; // string for phase_nom. int : 0 for liquid and 1 for gas
  Nom mot;
  is >> mot;
  if (mot != "{")
    {
      Cerr << "Milieu_MUSIG : { expected instead of " << mot << finl;
      Process::exit();
    }

  for (is >> mot; mot != "}"; is >> mot)
    {
      if (Motcle(mot) == "POROSITES_CHAMP") is >> ch_porosites_;
      else if (Motcle(mot) == "DIAMETRE_HYD_CHAMP") is >> ch_diametre_hyd_;
      else if (Motcle(mot) == "POROSITES")
        {
          Cerr << "You should use porosites_champ and not porosites ! Call the 911 !" << finl;
          Process::exit();
        }
      else if (Motcle(mot) == "GRAVITE")
        {
          Cerr << que_suis_je() << " : gravity should not be defined in Pb_Multiphase ! Use source_qdm if you want gravity in QDM equation !" << finl;
          Process::exit();
        }
      else if (!mot.debute_par("saturation") && !mot.debute_par("interface")) // add phases
        {
          Nom nomPhase(mot);

          Cerr << "Milieu_MUSIG : ajout la phase " << mot << " ... " << finl;
          OWN_PTR(Fluide_base) fluide;
          fluide.typer_lire_simple(is, "Typing the fluid medium ...");

          if(fluide->que_suis_je() == "Fluide_MUSIG")
            {
              Fluide_MUSIG& fluide_cast = ref_cast(Fluide_MUSIG,fluide.valeur());
              for(int k =0; k<fluide_cast.get_NbSubPhase(); k++)
                {
                  OWN_PTR(Fluide_base) subFluide(fluide_cast.get_Fluide());

                  if (subFluide->has_porosites())
                    {
                      Cerr << que_suis_je() + " : porosity should be defined only once in the milieu_composite block, not in " + subFluide->que_suis_je() << finl;
                      Process::exit();
                    }
                  if (subFluide->a_gravite())
                    {
                      Cerr << que_suis_je() + " : gravity should be defined only once in the milieu_composite block, not in " + subFluide->que_suis_je() << finl;
                      Process::exit();
                    }
                  if (subFluide->has_hydr_diam())
                    {
                      Cerr << que_suis_je() + " : diametre_hyd_champ should be defined only once in the milieu_composite block, not in " + subFluide->que_suis_je() << finl;
                      Process::exit();
                    }
                  if (!sub_type(Fluide_base,subFluide.valeur()))
                    {
                      Cerr << que_suis_je() + " : only real fluids should be read and not a fluid of type " + subFluide->que_suis_je() << finl;
                      Process::exit();
                    }
                  noms_phases_.add(nomPhase+"_"+std::to_string(k));
                  subFluide->set_id_composite(i++);
                  subFluide->nommer(nomPhase);
                  fluides_.push_back(subFluide);
                  std::array<int,3> lineIndex;
                  lineIndex[0]=(int)fluides_.size()-1;
                  lineIndex[1]=(int)fluidesMUSIG_.size();
                  lineIndex[2]=k;
                  indexMilieuToIndexFluide_.push_back(lineIndex);
                  especes.push_back(check_fluid_name(subFluide->le_nom()));

                }
              fluidesMUSIG_.push_back(fluide_cast);
            }
          else
            {
              if (fluide->has_porosites())
                {
                  Cerr << que_suis_je() + " : porosity should be defined only once in the milieu_composite block, not in " + fluide->que_suis_je() << finl;
                  Process::exit();
                }
              if (fluide->a_gravite())
                {
                  Cerr << que_suis_je() + " : gravity should be defined only once in the milieu_composite block, not in " + fluide->que_suis_je() << finl;
                  Process::exit();
                }
              if (fluide->has_hydr_diam())
                {
                  Cerr << que_suis_je() + " : diametre_hyd_champ should be defined only once in the milieu_composite block, not in " + fluide->que_suis_je() << finl;
                  Process::exit();
                }
              if (!sub_type(Fluide_base,fluide.valeur()))
                {
                  Cerr << que_suis_je() + " : only real fluids should be read and not a fluid of type " + fluide->que_suis_je() << finl;
                  Process::exit();
                }

              noms_phases_.add(nomPhase);
              fluide->set_id_composite(i++);
              fluide->nommer(nomPhase); // XXX
              fluides_.push_back(fluide);
              std::array<int,3> lineIndex;
              lineIndex[0]=(int)fluides_.size()-1;
              lineIndex[1]=-1;
              lineIndex[2]=-1;
              indexMilieuToIndexFluide_.push_back(lineIndex);
              especes.push_back(check_fluid_name(fluide->le_nom()));
            }

        }
      else if (mot.debute_par("saturation")) // add saturation
        {
          has_saturation_ = true;
          Cerr << "Milieu_composite : ajout la saturation " << mot << " ... " << finl;
          sat_lu_.typer_lire_simple(is, "Typing the saturation ...");
        }
      else // on ajout l'interface
        {
          has_interface_ = true;
          Cerr << "Milieu_composite : ajout l'interface " << mot << " ... " << finl;
          inter_lu_.typer_lire_simple(is, "Typing the interface ...");
        }
    }

  // Sais pas s'il faut tester ca ou non ... Yannick, Antoine ! help :/
  if (has_saturation() && has_interface())
    {
      Cerr << "You define both interface and saturation in Milieu_composite ???" << finl;
      Process::exit();
    }

  // Interface treatment
  const int N = (int)fluides_.size();
  for (int n = 0; n < N; n++)
    {
      std::vector<Interface_base *> inter;
      for (int m = 0; m < N; m++)
        {
          const std::string espn = especes[n].first, espm = especes[m].first;
          const int pn = especes[n].second, pm = especes[m].second;

          if (pn != pm && (has_interface() || (espn == espm && has_saturation())))
            {
              Cerr << "Interface between fluid " << n << " : " << fluides_[n]->le_nom() << " and " << m << " : " << fluides_[m]->le_nom() << finl;
              inter.push_back(&ref_cast(Interface_base, has_saturation_ ? sat_lu_.valeur() : inter_lu_.valeur()));
              const Saturation_base *sat = sub_type(Saturation_base, *inter.back()) ? &ref_cast(Saturation_base, *inter.back()) : nullptr;
              if (sat && sat->get_Pref() > 0) // for an EOS of the type:  e = e0 + cp * (T - T0)
                {
                  const double hn = pn ? sat->Hvs(sat->get_Pref()) : sat->Hls(sat->get_Pref()),
                               hm = pm ? sat->Hvs(sat->get_Pref()) : sat->Hls(sat->get_Pref()),
                               T0 = sat->Tsat(sat->get_Pref());
                  fluides_[n]->set_h0_T0(hn, T0), fluides_[m]->set_h0_T0(hm, T0);
                }
            }
          else inter.push_back(nullptr);
        }
      tab_interface_.push_back(inter);
    }

  return is;
}

double Milieu_MUSIG::get_Diameter_Inf(int iPhaseMilieu) const
{
  std::array<int,3> index = indexMilieuToIndexFluide_[iPhaseMilieu];
  double diametre = -1;
  if(index[2] < 0)
    {
      Cerr << que_suis_je() + " : the function get_Diameter is only available with FluideMusig" << finl;
      Process::exit();
    }

  Fluide_MUSIG fluideMUSIG = fluidesMUSIG_[index[1]];

  if(fluideMUSIG.get_Diametres().size()<1)
    {
      Cerr << que_suis_je() + " : you need to define diameters for this fluideMusig" << finl;
      Process::exit();
    }
  diametre = fluideMUSIG.get_Diametres()[index[2]];

  // A decommenter si verification du tableau indexMilieuToIndexFluide_
  // std::vector<int> indexPhase;
  // const int N = (int)fluides_.size();
  // for (int n = 0; n < N; n++)
  //   {
  //     indexPhase = indexMilieuToIndexFluide_[n];
  //     cout << "Phase N° "<< indexPhase[0] << " Nom Phase " << noms_phases_[n]<< endl;
  //     cout << "  Fluide N° "<< indexPhase[1] << " Nom Fluide " << fluides_[n].le_nom()<< endl;
  //     cout << "  Index Fluide Musig N° "<< indexPhase[2] << endl;
  //     cout << endl;
  //   }

  // A decommenter si necessaire
  //const int N = (int)fluides_.size();
  // for (int k = 0; k < N; k++)
  //   {
  //     int phase = fluides_[k].le_nom().debute_par("gaz_dispersee");
  //     Nom espece = phase ? fluides_[k].le_nom().getSuffix("gaz_") : fluides_[k].le_nom().getSuffix("liquide_");
  //     cout << "nom " << espece << endl;
  //   }

  return diametre;
}

double Milieu_MUSIG::get_Diameter_Sup(int iPhaseMilieu) const
{
  std::array<int,3> index = indexMilieuToIndexFluide_[iPhaseMilieu];
  double diametre = -1;
  if(index[2] < 0)
    {
      Cerr << que_suis_je() + " : the function get_Diameter is only available with FluideMusig" << finl;
      Process::exit();
    }

  Fluide_MUSIG fluideMUSIG = fluidesMUSIG_[index[1]];

  if(fluideMUSIG.get_Diametres().size()<1)
    {
      Cerr << que_suis_je() + " : you need to define diameters for this fluideMusig" << finl;
      Process::exit();
    }
  diametre = fluideMUSIG.get_Diametres()[index[2]+1];

  return diametre;
}

double Milieu_MUSIG::get_Diameter_Sauter(int iPhaseMilieu) const
{
  double diametre = -1;

  double dInf = get_Diameter_Inf(iPhaseMilieu);
  double dSup = get_Diameter_Sup(iPhaseMilieu);

  diametre = dInf;
  if (dSup != dInf) diametre = (dSup-dInf)/log(dSup/dInf);

  // test
  // if (iPhaseMilieu == 1) diametre = get_Diameter_Inf(1);
  // if (iPhaseMilieu == 2) diametre = get_Diameter_Inf(1);
  // if (iPhaseMilieu == 3) diametre = get_Diameter_Sup(3);

  if ((diametre < dInf) || (diametre > dSup))
    {
      Cerr << que_suis_je() + " : Diam Sauter < dInf or Diam Sauter > dSup of phase =" << iPhaseMilieu << finl;
      Process::exit();
    }

  return diametre;
}

bool Milieu_MUSIG::has_dispersed_gas(int k) const
{
  bool dispersedGas = false;
  if (fluides_[k].le_nom().debute_par("gaz_dispersee")) dispersedGas = true;
  return dispersedGas;
}

bool Milieu_MUSIG::has_dispersed_liquid(int k) const
{
  bool dispersedLiquid = false;
  if (fluides_[k].le_nom().debute_par("liquide_dispersee")) dispersedLiquid = true;
  return dispersedLiquid;
}

bool Milieu_MUSIG::has_carrier_liquid(int k) const
{
  bool carrierLiquid = false;
  if (fluides_[k].le_nom().debute_par("liquide_continu")) carrierLiquid = true;
  return carrierLiquid;
}

bool Milieu_MUSIG::has_carrier_gas(int k) const
{
  bool carrierGas = false;
  if (fluides_[k].le_nom().debute_par("gaz_continu")) carrierGas = true;
  return carrierGas;
}
