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
// File:        Op_Ecart_type.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Statistiques_temps
// Version:     /main/34
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Ecart_type.h>
#include <Probleme_base.h>
#include <Schema_Temps.h>

Implemente_instanciable(Op_Ecart_type,"Op_Ecart_type",Operateur_Statistique_tps_base);

//// printOn
//

Sortie& Op_Ecart_type::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Op_Ecart_type::readOn(Entree& s )
{
  return s ;
}

//////////////////////////////////////////////////////////////////
//
//         Fonctions de la classe Op_Ecart_type
//
/////////////////////////////////////////////////////////////////

void Op_Ecart_type::completer(const Probleme_base& Pb)
{
  Nom nom_pour_post("Ecart_type_");
  const REF(Champ_Generique_base)& mon_champ=integrale_carre_champ.le_champ();
  const Noms noms = mon_champ->get_property("nom");
  nom_pour_post += Motcle(noms[0]);

  integrale_carre_champ->nommer(nom_pour_post);
  if (la_moyenne.non_nul())
    integrale_champ = moyenne().integrale();

  // Dimensionnement du champ integrale_champ a la meme taille que mon_champ
  Champ espace_stockage_source;
  const Champ_base& source = mon_champ->get_champ(espace_stockage_source);
  const DoubleTab& tab1 = source.valeurs();

  // Initialisation de la structure du tableau val.
  //  Est-bien le bon endroit pour faire ca ? (plutot dans integrale_carre_champ ?)
  integrale_carre_champ.associer_zone_dis_base(Pb.domaine_dis().zone_dis(0).valeur());
  int nb_comp = source.nb_comp();
  integrale_carre_champ->fixer_nb_comp(nb_comp);
  DoubleTab& val = valeurs_carre();
  val.reset();
  val = tab1;
  val = 0.;

  if (nb_comp == 1)
    integrale_carre_champ->fixer_unite(source.unite());
  else
    integrale_carre_champ->fixer_unites(source.unites());
  integrale_carre_champ->changer_temps(Pb.schema_temps().temps_courant());
}

DoubleTab Op_Ecart_type::calculer_valeurs() const
{
  double dt = dt_integration();
  if (!est_egal(dt,dt_integration_carre()))
    {
      Cerr << "Not implemented yet in Op_Ecart_type::calculer_valeurs()" << finl;
      exit();
    }
  DoubleTab ecart_type(valeurs_carre());
  if ( dt > 0 )
    {
      ecart_type *= dt;                      // sum(I^2)*dt
      ecart_type.ajoute_carre(-1,valeurs()); // sum(I^2)*dt-sum(I)^2
      ecart_type /= (dt*dt);                 // sum(I^2)/dt-(sum(I)/dt)^2)
      ecart_type.abs();                      // To avoid negative number ?
      ecart_type.racine_carree();            // sqrt(mean(I^2)-mean(I)^2)
    }
  return ecart_type;
}
