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
// File:        Discret_Thermique.cpp
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Discret_Thermique.h>
#include <Zone_dis.h>
#include <Schema_Temps_base.h>
#include <Champ_Inc.h>
#include <Milieu_base.h>
#include <Champ_Fonc_Tabule.h>



Implemente_base(Discret_Thermique,"Discret_Thermique",Discretisation_base);


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Discret_Thermique::printOn(Sortie& s) const
{
  return s;
}


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Discret_Thermique::readOn(Entree& s)
{
  return s ;
}

void Discret_Thermique::temperature(const Schema_Temps_base& sch,
                                    Zone_dis& z,
                                    Champ_Inc& ch) const
{
  Cerr << "Discretisation de la temperature" << finl;
  discretiser_champ("temperature",z.valeur(),"temperature","K",1,sch.nb_valeurs_temporelles(),sch.temps_courant(),ch);

}

/* void Discret_Thermique::t_paroi(const Zone_dis& z,const Zone_Cl_dis& zcl, const Equation_base& eqn,Champ_Fonc& ch) const
{
  Cerr << "Discret_Thyd::t_paroi() ne fait rien" << finl;
  Cerr <<  que_suis_je() << "doit la surcharger !" << finl;
  exit();
} */

void Discret_Thermique::Fluctu_Temperature(const Schema_Temps_base& sch,
                                           Zone_dis& z,
                                           Champ_Inc& ch) const
{
  Cerr << "Discretisation des fluctuations en temperature" << finl;
  Noms noms(2);
  Noms unit(2);
  noms[0]="variance_temperature";
  noms[1]="taux_dissipation_temperature";
  unit[0]="K2";
  unit[1]="m2s-3";
  discretiser_champ("temperature",z.valeur(),multi_scalaire,noms,unit,2,sch.nb_valeurs_temporelles(),sch.temps_courant(),ch);
  ch.valeur().nommer("Fluctu_Temperature");
}
void Discret_Thermique::Flux_Chaleur_Turb(const Schema_Temps_base& sch,
                                          Zone_dis& z,
                                          Champ_Inc& ch) const
{
  Cerr << "Discretisation du flux de chaleur turbulente" << finl;
  discretiser_champ("vitesse",z.valeur(),"Flux_Chaleur_Turbulente","truc1",dimension,sch.nb_valeurs_temporelles(),
                    sch.temps_courant(),ch);

}
