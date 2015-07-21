/****************************************************************************
* Copyright (c) 2015, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Integrale_tps_produit_champs.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Statistiques_temps
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Integrale_tps_produit_champs.h>

#include <Zone_VF.h>

Implemente_instanciable(Integrale_tps_produit_champs,"Integrale_tps_produit_champs",Integrale_tps_Champ);

// printOn

Sortie& Integrale_tps_produit_champs::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}


// readOn

Entree& Integrale_tps_produit_champs::readOn(Entree& s)
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
void Integrale_tps_produit_champs::mettre_a_jour_integrale()
{
  Champ espace_stockage_source,espace_stockage_source2;
  const Champ_base& source = mon_premier_champ()->get_champ(espace_stockage_source);
  const Champ_base& source2 = mon_second_champ()->get_champ(espace_stockage_source2);
  const Noms nom = mon_premier_champ()->get_property("nom");
  const Noms nom2 = mon_second_champ()->get_property("nom");
  double t_courant = mon_premier_champ()->get_time();

  if (t_courant != mon_second_champ()->get_time())
    {
      Cerr << "Integrale_tps_produit_champs::mettre_a_jour_integrale()" << finl;
      Cerr << "the current time of the field named " << nom[0] << " =" << t_courant << finl;
      Cerr << "is different of the second field current time " << nom2[0] << " =" <<  source2.temps() << finl;
      exit();
    }
  if (t_fin_ < t_debut_)
    {
      Cerr << " Statistics begining and ending times are not coherent " << finl;
      Cerr << " t_fin_=" << t_fin_ << " < t_debut_=" << t_debut_ << finl;
      exit();
    }
  if ( inf_ou_egal(t_debut_ ,t_courant) &&  inf_ou_egal(t_courant,t_fin_) )
    {
      double dt = t_courant - tps_integrale;
      if (dt > 0)
        {
          //DoubleTab& mes_val = valeurs();
          if (premiere_puissance() == 1 && seconde_puissance() == 1)
            {
              ////ajoute_produit_tensoriel(dt, mon_premier_champ(), mon_second_champ());
              ajoute_produit_tensoriel(dt, source, source2);
            }
          else
            {
              Cerr << "Integrale_tps_produit_champs::mettre_a_jour_integrale() : case not coded." << finl;
            }
          tps_integrale = t_courant;
          dt_integr_calcul += dt;
        }
    }

}

void Integrale_tps_produit_champs::ajoute_produit_tensoriel(double alpha, const Champ_base& a, const Champ_base& b)
{
  if (support_different_)
    {
      // On ramene au centre des elements
      const DoubleTab& xp = ref_cast(Zone_VF,zone_dis_base()).xp();
      int nb_elem_tot = xp.dimension_tot(0);
      DoubleTab val_a,val_b;
      // Le jour ou les champs seront mieux foutus, on n'aura
      // pas a faire ca:
      if (a.nb_comp()>1)
        val_a.resize(nb_elem_tot,a.nb_comp());
      else
        val_a.resize(nb_elem_tot);
      if (b.nb_comp()>1)
        val_b.resize(nb_elem_tot,b.nb_comp());
      else
        val_b.resize(nb_elem_tot);
      a.valeur_aux(xp, val_a);
      b.valeur_aux(xp, val_b);
      valeurs().ajoute_produit_tensoriel(alpha,val_a,val_b);
      valeurs().echange_espace_virtuel();
    }
  else
    {
      const DoubleTab& val_a = a.valeurs();
      const DoubleTab& val_b = b.valeurs();
      valeurs().ajoute_produit_tensoriel(alpha,val_a,val_b);
    }
}


