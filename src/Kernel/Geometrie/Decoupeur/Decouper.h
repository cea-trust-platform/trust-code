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
#ifndef Decouper_included
#define Decouper_included

#include <Interprete.h>
#include <Partitionneur_base.h>
#include <vector>
#include <Ref_Domaine.h>

class Domaine;

// .DESCRIPTION
//  Interprete Decouper. Aucun algorithme ici, uniquement lecture
//  de parametres dans le fichier .data et execution du partitionneur
//  et du decoupeur. Voir la methode interprete()

class Decouper : public Interprete
{
  Declare_instanciable(Decouper);
public:
  enum ZonesFileOutputType { BINARY_MULTIPLE, HDF5_SINGLE };

  Entree& interpreter(Entree& is) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  static int print_more_infos;

  Entree& lire(Entree& is);                 //lecture des parametres
  //ecriture d'une partition elem_part donnee
  //som_raccord (optionnel) : som_raccord[s] -> process auxquels est raccorde le sommet
  //                                            s par un raccord a un autre domaine
  void ecrire(IntVect& elem_part, const Static_Int_Lists *som_raccord = NULL);

  Nom nom_domaine;
  DERIV(Partitionneur_base) deriv_partitionneur;
  REF(Domaine) ref_domaine;
  int nb_parts_tot = -1;
  Noms liste_bords_periodiques;

private:
  const Domaine& find_domain(const Nom& nom);
  // Parametres du decoupage:

  //parametres remplis par lire()
  int epaisseur_joint = 1;
  Nom nom_zones_decoup = "?";
  Nom nom_fichier_decoupage = "?";
  Nom nom_fichier_lata = "?";
  int format_binaire = 1;
  int format_hdf = 0;
  int reorder = 0;
};

#endif
