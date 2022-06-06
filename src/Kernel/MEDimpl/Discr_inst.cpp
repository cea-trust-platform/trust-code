/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Discr_inst.h>
#include <Champ_Don.h>
#include <Champ_Fonc.h>
#include <Motcle.h>
#include <Zone_dis_base.h>
#include <Domaine.h>

Implemente_instanciable(Discr_inst,"VF_inst",Discretisation_base);

Entree& Discr_inst::readOn(Entree& s)
{
  return s ;
}

Sortie& Discr_inst::printOn(Sortie& s) const
{
  return s ;
}


void Discr_inst::discretiser_champ(const Motcle& directive, const Zone_dis_base& z,
                                   Nature_du_champ nature,
                                   const Noms& nom, const Noms& unite,
                                   int nb_comp, int nb_pas_dt, double temps,
                                   Champ_Inc& champ, const Nom& sous_type) const
{
  assert(0);
  throw;
}
// Description:
// Idem que Discr_inst::discretiser_champ(... , Champ_Inc)
// pour un Champ_Fonc.
void Discr_inst::discretiser_champ(
  const Motcle& directive, const Zone_dis_base& z,
  Nature_du_champ nature,
  const Noms& noms, const Noms& unites,
  int nb_comp, double temps,
  Champ_Fonc& champ) const
{
  discretiser_champ_fonc_don(directive, z,
                             nature, noms, unites,
                             nb_comp, temps, champ);
}

// Description:
// Idem que Discr_inst::discretiser_champ(... , Champ_Inc)
// pour un Champ_Don.
void Discr_inst::discretiser_champ(
  const Motcle& directive, const Zone_dis_base& z,
  Nature_du_champ nature,
  const Noms& noms, const Noms& unites,
  int nb_comp, double temps,
  Champ_Don& champ) const
{
  discretiser_champ_fonc_don(directive, z,
                             nature, noms, unites,
                             nb_comp, temps, champ);
}

// Description:
// Idem que VEF_discretisation::discretiser_champ(... , Champ_Inc)
// Traitement commun aux champ_fonc et champ_don.
// Cette methode est privee (passage d'un Objet_U pas propre vu
// de l'exterieur ...)
void Discr_inst::discretiser_champ_fonc_don(
  const Motcle& directive, const Zone_dis_base& z,
  Nature_du_champ nature,
  const Noms& noms, const Noms& unites,
  int nb_comp, double temps,
  Objet_U& champ) const
{
  // Deux pointeurs pour acceder facilement au champ_don ou au champ_fonc,
  // suivant le type de l'objet champ.
  Champ_Fonc * champ_fonc = 0;
  Champ_Don * champ_don = 0;
  if (sub_type(Champ_Fonc, champ))
    champ_fonc = & ref_cast(Champ_Fonc, champ);
  else
    champ_don  = & ref_cast(Champ_Don, champ);

  Motcles motcles(2);
  motcles[0] = "champ_elem";  // Creer un champ aux elements (de type P0)
  motcles[1] = "champ_sommets";   // Creer un champ aux sommets

  Nom type;
  int rang = motcles.search(directive);
  //int default_nb_comp = 1;
  const Domaine& domaine=z.zone().domaine();
  int nb_som_elem = domaine.zone(0).nb_som_elem();
  int nb_ddl=-1;
  switch(rang)
    {
    case 0:
      type = "Champ_Fonc_P0_MED";
      nb_ddl = domaine.zone(0).nb_elem();
      break;
    case 1:
      type = "Champ_Fonc_P1_MED";
      nb_ddl = domaine.nb_som();
      if (((dimension==2) && (nb_som_elem==4)) || ((dimension==3) && (nb_som_elem==8)))
        {
          type = "Champ_Fonc_Q1_MED";
        }
      break;

    default:
      assert(rang < 0);

      break;
    }

  if (directive == demande_description)
    Cerr << "Discr_inst : " << motcles;

  // Si on n'a pas compris la directive (ou si c'est une demande_description)
  // alors on appelle l'ancetre :
  if (rang < 0)
    {
      if (champ_fonc)
        Discretisation_base::discretiser_champ(directive, z, nature, noms, unites,
                                               nb_comp, temps, *champ_fonc);
      else
        Discretisation_base::discretiser_champ(directive, z, nature, noms, unites,
                                               nb_comp, temps, *champ_don);


      return;
    }



  if (champ_fonc)
    {
      Champ_Fonc& ch=*champ_fonc;
      ch.typer(type);
      ch->associer_zone_dis_base(z);
      ch->fixer_nb_comp(nb_comp);
      ch->fixer_nature_du_champ(nature);
      ch->fixer_nb_valeurs_nodales(nb_ddl);
    }
  else
    {
      assert(0);
      throw;
    }


}
