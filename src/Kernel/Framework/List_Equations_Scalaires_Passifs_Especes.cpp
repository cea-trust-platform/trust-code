/****************************************************************************
* Copyright (c) 2021, CEA
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

#include <List_Equations_Scalaires_Passifs_Especes.h>
#include <Probleme_base.h>

Implemente_instanciable_sans_constructeur(List_Equations_Scalaires_Passifs_Especes,"Equations_Scalaires_Passifs|Equations_Especes",Equation_base);

List_Equations_Scalaires_Passifs_Especes::List_Equations_Scalaires_Passifs_Especes() : complet(0) { }

Sortie& List_Equations_Scalaires_Passifs_Especes::printOn(Sortie& os) const
{
  return os;
}

Entree& List_Equations_Scalaires_Passifs_Especes::readOn(Entree& is)
{
  // Ici se situe un gros piratage
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;
  is >> motlu;
  if (motlu!=accolade_ouverte)
    {
      Cerr<<"Big problem in List_Equations_Scalaires_Passifs_Especes::readOn , we expected "<<accolade_ouverte;
      Process::exit();
    }
  is >> motlu;
  while (motlu!=accolade_fermee)
    {
      Equation toto;
      Nom nume(list_eq.size());
      Equation& Eqn_nvelle=list_eq.add(toto);
      Eqn_nvelle.typer(motlu);
      Equation_base& Eqn = ref_cast(Equation_base,Eqn_nvelle.valeur());

      // maintenant on associe le pb
      Eqn.associer_pb_base(probleme());
      Eqn.associer_milieu_base(mil.valeur());
      Eqn.associer_sch_tps_base(schema_temps());

      // on la discretise (pas fait par discretiser)
      Eqn.associer_zone_dis(probleme().domaine_dis().zone_dis(0));
      Eqn.discretiser();

      // on change le nom de l'inconnue et de l equation
      Nom nom;
      nom=Eqn.inconnue().valeur().le_nom();
      Nom nom_eq;
      nom_eq = Eqn.le_nom();
      nom+=nume;
      Cerr<<"The unknown name is modified : new name "<<nom<<finl;
      Eqn.inconnue().valeur().nommer(nom);
      nom_eq+=nume;
      Cerr<<"The equation name is modified : new name "<<nom_eq<<finl;
      Eqn.nommer(nom_eq);

      // enfin on lit
      is >> Eqn;
      is >> motlu;
    }
  complet=1;
  return is ;
}

void List_Equations_Scalaires_Passifs_Especes::associer_milieu_equation()
{
  for (int i = 0; i < nb_equation(); i++) equation(i).associer_milieu_equation();
}
