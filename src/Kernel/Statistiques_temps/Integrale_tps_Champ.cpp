/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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

#include <Integrale_tps_Champ.h>

Implemente_instanciable(Integrale_tps_Champ,"Integrale_tps_Champ",Champ_Fonc);


// printOn

Sortie& Integrale_tps_Champ::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}


// readOn

Entree& Integrale_tps_Champ::readOn(Entree& s)
{
  return s ;
}

// Description:
//    Mets a jour l'integrale.
//    Verifie que le temps de l'integrale est inferieur a celui du
//    champ associe et poursuit l'integration jusqu'au temps courant.
//    si la borne superieure de l'integrale n'est pas depassee.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Integrale_tps_Champ::mettre_a_jour_integrale()
{


  Champ espace_stockage_source;
  const Champ_base& source = mon_champ->get_champ(espace_stockage_source);
  double t_courant = mon_champ->get_time();

  if (t_fin_ < t_debut_)
    {
      Cerr << " Statistic begining and ending times are not coherent " << finl;
      Cerr << " t_fin_=" << t_fin_ << " < t_debut_=" << t_debut_ << finl;
      exit();
    }
  if ( inf_ou_egal(t_debut_ ,t_courant) &&  inf_ou_egal(t_courant,t_fin_) )
    {
      double dt = t_courant - tps_integrale;
      if (dt > 0)
        {
          const DoubleTab& val = source.valeurs();
          DoubleTab& mes_val = valeurs();
          if (puissance == 1)
            mes_val.ajoute(dt,val);
          else if (puissance == 2)
            mes_val.ajoute_carre(dt,val);
          else
            for (int i=0; i<val.size(); i++)
              mes_val[i] += dt*pow(val[i],puissance);
          tps_integrale = t_courant;
          dt_integr_calcul += dt;
        }
    }
}
