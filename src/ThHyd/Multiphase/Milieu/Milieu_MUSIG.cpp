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

#include <Milieu_MUSIG.h>

Implemente_instanciable( Milieu_MUSIG, "Milieu_MUSIG", Milieu_composite ) ;
// XD Milieu_MUSIG listobj Milieu_MUSIG -1 milieu_base 0 MUSIG medium made of several sub mediums.

Sortie& Milieu_MUSIG::printOn(Sortie& os) const { return Objet_U::printOn(os); }

Entree& Milieu_MUSIG::readOn( Entree& is )
{
  int i = 0;
  std::vector<std::pair<std::string, int>> especes; // string pour phase_nom. int : 0 pour liquide et 1 pour gaz
  Nom mot;
  is >> mot;
  if (mot != "{")
    {
      Cerr << "Milieu_MUSIG : { expected instead of " << mot << finl;
      Process::exit();
    }

  for (is >> mot; mot != "}"; is >> mot)
    {
      if (Motcle(mot) == "POROSITES_CHAMP") is >> porosites_champ;
      else if (Motcle(mot) == "DIAMETRE_HYD_CHAMP") is >> diametre_hyd_champ;
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
      else if (!mot.debute_par("saturation") && !mot.debute_par("interface")) // on ajout les phases
        {
          Nom nomPhase(mot);

          Cerr << "Milieu_MUSIG : ajout la phase " << mot << " ... " << finl;
          Fluide fluide;
          is >> fluide;

          if(fluide->que_suis_je() == "Fluide_MUSIG")
            {
              Fluide_MUSIG& fluide_cast = ref_cast(Fluide_MUSIG,fluide.valeur());
              for(int k =0; k<fluide_cast.getNbSubPhase(); k++)
                {
                  Fluide subFluide(fluide_cast.getFluide());

                  if (subFluide->get_porosites_champ().non_nul())
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
                  subFluide->nommer(nomPhase+"_"+std::to_string(k)); // XXX
                  fluides.push_back(subFluide);
                  std::vector<int> lineIndex;
                  lineIndex.push_back((int)fluides.size()-1);
                  lineIndex.push_back((int)fluidesMUSIG_.size());
                  lineIndex.push_back(k);
                  indexMilieuToIndexFluide_.push_back(lineIndex);
                  especes.push_back(check_fluid_name(subFluide->le_nom()));

                }
              fluidesMUSIG_.push_back(fluide_cast);
            }
          else
            {
              if (fluide->get_porosites_champ().non_nul())
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
              fluides.push_back(fluide);

              std::vector<int> lineIndex;
              lineIndex.push_back((int)fluides.size()-1);
              lineIndex.push_back(-1);
              lineIndex.push_back(-1);
              indexMilieuToIndexFluide_.push_back(lineIndex);
              especes.push_back(check_fluid_name(fluide->le_nom()));
            }

        }
      else if (mot.debute_par("saturation")) // on ajout la saturation
        {
          has_saturation_ = true;
          Cerr << "Milieu_composite : ajout la saturation " << mot << " ... " << finl;
          is >> sat_lu;
        }
      else // on ajout l'interface
        {
          has_interface_ = true;
          Cerr << "Milieu_composite : ajout l'interface " << mot << " ... " << finl;
          is >> inter_lu;
        }
    }

  // Sais pas s'il faut tester ca ou non ... Yannick, Antoine ! help :/
  if (has_saturation() && has_interface())
    {
      Cerr << "You define both interface and saturation in Milieu_composite ???" << finl;
      Process::exit();
    }

  // Traitement pour les interfaces
  const int N = (int)fluides.size();
  for (int n = 0; n < N; n++)
    {
      std::vector<Interface_base *> inter;
      for (int m = 0; m < N; m++)
        {
          const std::string espn = especes[n].first, espm = especes[m].first;
          const int pn = especes[n].second, pm = especes[m].second;

          if (pn != pm && (has_interface() || (espn == espm && has_saturation())))
            {
              Cerr << "Interface between fluid " << n << " : " << fluides[n]->le_nom() << " and " << m << " : " << fluides[m]->le_nom() << finl;
              inter.push_back(&ref_cast(Interface_base, has_saturation_ ? sat_lu.valeur() : inter_lu.valeur()));
              const Saturation_base *sat = sub_type(Saturation_base, *inter.back()) ? &ref_cast(Saturation_base, *inter.back()) : nullptr;
              if (sat && sat->get_Pref() > 0) // pour loi en e = e0 + cp * (T - T0)
                {
                  const double hn = pn ? sat->Hvs(sat->get_Pref()) : sat->Hls(sat->get_Pref()),
                               hm = pm ? sat->Hvs(sat->get_Pref()) : sat->Hls(sat->get_Pref()),
                               T0 = sat->Tsat(sat->get_Pref());
                  fluides[n]->set_h0_T0(hn, T0), fluides[m]->set_h0_T0(hm, T0);
                }
            }
          else inter.push_back(nullptr);
        }
      tab_interface.push_back(inter);
    }

  return is;
}

double Milieu_MUSIG::getDiameter(int iPhaseMilieu)
{
  std::vector<int> index = indexMilieuToIndexFluide_[iPhaseMilieu];
  double diametre = -1;
  if(index[2] < 0)
    {
      Cerr << que_suis_je() + " : the function getDiameter is only available with FluideMusig" << finl;
      Process::exit();
    }

  Fluide_MUSIG fluideMUSIG = fluidesMUSIG_[index[1]];

  if(fluideMUSIG.getdiametres().size()<1)
    {
      Cerr << que_suis_je() + " : you need to define diameters for this fluideMusig" << finl;
      Process::exit();
    }
  diametre = fluideMUSIG.getdiametres()[index[2]];

  return diametre;
}
