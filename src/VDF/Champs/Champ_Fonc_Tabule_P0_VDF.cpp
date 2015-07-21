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
// File:        Champ_Fonc_Tabule_P0_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Champs
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_Tabule_P0_VDF.h>
#include <Champ_Inc.h>
#include <Table.h>

Implemente_instanciable(Champ_Fonc_Tabule_P0_VDF,"Champ_Fonc_Tabule_P0_VDF",Champ_Fonc_P0_VDF);

// printOn
Sortie& Champ_Fonc_Tabule_P0_VDF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

// readOn
Entree& Champ_Fonc_Tabule_P0_VDF::readOn(Entree& s)
{
  return s ;
}

void Champ_Fonc_Tabule_P0_VDF::associer_param(const Champ_base& un_champ_inc,
                                              const Table& une_table)
{
  le_champ_parametre = un_champ_inc;
  la_table = une_table;
}


void Champ_Fonc_Tabule_P0_VDF::mettre_a_jour(double t)
{
  const Zone_VDF& zone_VDF =la_zone_VDF.valeur();
  const Table& table = la_table.valeur();
  const int& isfct = table.isfonction();
  const DoubleTab& val_param = le_champ_parametre->valeurs();
  DoubleTab& mes_valeurs = valeurs();
  if (!(val_param.nb_dim() == mes_valeurs.nb_dim()))
    {
      Cerr << "Erreur a l'initialisation d'un Champ_Fonc_Tabule" << finl;
      Cerr << "Le champ parametre et le champ a initialiser ne sont pas compatibles" << finl;
      exit();
    }
  if (isfct!=2)
    {
      int nb_elems = zone_VDF.nb_elem();
      if (val_param.nb_dim() == 1)
        for (int num_elem=0; num_elem<nb_elems; num_elem++)
          mes_valeurs(num_elem) = table.val(val_param(num_elem));
      else
        {
          int nb_comps = val_param.nb_dim();
          for (int num_elem=0; num_elem<nb_elems; num_elem++)
            for (int ncomp=0; ncomp<nb_comps; ncomp++)
              mes_valeurs(num_elem,ncomp) = table.val(val_param(num_elem,ncomp));
        }
    }
  else
    {
      const DoubleTab& centres_de_gravites = zone_VDF.xp();
      table.valeurs(val_param,centres_de_gravites,t,mes_valeurs);
    }

  Champ_Fonc_base::mettre_a_jour(t);
}

int Champ_Fonc_Tabule_P0_VDF::initialiser(const double& un_temps)
{
  Champ_Fonc_Tabule_P0_VDF::mettre_a_jour(un_temps);
  return 1;
}
