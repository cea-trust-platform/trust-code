/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Moyenne_volumique_included
#define Moyenne_volumique_included

#include <TRUSTTabs_forward.h>
#include <Interprete.h>
#include <TRUST_Ref.h>
#include <Parser.h>

class Convolution_function_base;
class Format_Post_base;
class Domaine_VF;
class Champ_base;
class Motcles;

/*! @brief : cet interprete permet, a la fin du calcul (apres "resoudre"), de calculer et de stocker dans un fichier lata le produit de convolution
 *
 *   de certains champs d'un probleme avec une fonction filtre arbitraire.
 *   On peut l'utiliser comme un interprete (voir Moyenne_volumique::interpreter())
 *   ou n'importe ou a l'interieur du code comme ceci:
 *    Moyenne_volumique moy;
 *    is >> moy; // lecture de la fonction filtre
 *    moy.calculer_convolution_champ_elem/face(...)
 *
 */
class Moyenne_volumique: public Interprete
{
  Declare_instanciable(Moyenne_volumique);

public:
  Entree& interpreter(Entree&) override;

  virtual void calculer_convolution_champ_elem(const Domaine_VF& domaine_source, const DoubleTab& champ_source, const DoubleTab& coords_to_compute, DoubleTab& resu) const;

  virtual void calculer_convolution_champ_face(const Domaine_VF& domaine_source, const DoubleTab& champ_source, const DoubleTab& coords_to_compute, DoubleTab& resu) const;

  virtual void eval_filtre(const DoubleTab& coords, ArrOfDouble& result) const;
  double box_size() const { return box_size_;  }

protected:
  void traiter_champs(const Motcles& noms_champs, const Nom& nom_pb, const Nom& nom_dom, const DoubleTab& coords, Format_Post_base& post, double temps, const Motcle& localisation);

  int get_champ(const Nom& nom_pb, const Nom& nom_champ,
                REF(Champ_base) &ref_champ);

  virtual void calculer_convolution(const Domaine_VF& domaine_source, const DoubleTab& champ_source, const DoubleTab& coords_to_compute, DoubleTab& resu) const;

  enum Type { ERROR, BOITE, CHAPEAU, GAUSSIENNE, PARSER, QUADRA };
  // Si type_ != parser, il faut renseigner l_, sinon il faut renseigner expression_parser_
  Type type_ = ERROR;
  // parametre du filtre (largeur de boite, chapeau ou gaussienne)
  double l_= -1.;
  // box_size_ doit toujours etre rempli (demi-cote d'un cube contenant le support du filtre)
  double box_size_ = -100.;
  Nom expression_parser_;
  mutable Parser parser_;
};

#endif
