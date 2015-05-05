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
// File:        Champs_compris_interface.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champs_compris_interface_included
#define Champs_compris_interface_included

#include <Ref_Champ_base.h>

class Motcle;
class Nom;
class Champ_base;

enum Option { NONE, DESCRIPTION };

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champs_compris_interface
//                   Cette classe contient une interface de methodes destinees a gerer
//              les champs compris (postraitables) pour une classe donnee.
//              Les classes qui heritent de cette classe sont : Probleme_base, Equation_base, Milieu_base,
//              Operateur_base, Source_base, Traitement_particulier_NS_base, Traitement_particulier_Solide_base,
//              Mod_turb_hyd_base, Modele_turbulence_scal_base, Loi_Etat_base, Modele_Fonc_Bas_Reynolds_Base,
//              Modele_Fonc_Bas_Reynolds_Thermique_Base et l interface est propagee dans leurs classes derivees
//
//        Methodes de l interface
//        creer_champ()
//        get_champ()
//        get_noms_champs_postraitables()
//
//////////////////////////////////////////////////////////////////////////////
class Champs_compris_interface
{
public :
  virtual inline ~Champs_compris_interface() {};
  virtual void creer_champ(const Motcle& motlu) =0;
  virtual const Champ_base& get_champ(const Motcle& nom) const=0;
  virtual void get_noms_champs_postraitables(Noms& nom, Option opt=NONE) const=0;


  //Pour illustrer la description des methodes faite ci-dessous, on pourra
  //se referer a leur codage dans Probleme_base, Equation_base et Navier_Stokes_std

  //Description de la methode creer_champ()
  /////////////////////////////////////////////////////////////////////////////////
  //Cette methode permet de creer un "champ calcule". Ce champ de type Champ_Fonc
  //(ex : vorticite) est estime uniquement pour etre postraite et ne sert pas a
  //la resolution du probleme.
  //Le probleme delegue a son milieu et ses equations la creation du champ.
  //Une classe qui identifie le champ comme etant un de ses attributs, le cree
  //en lancant sa discretisation. Une classe qui porte un "champ calcule" possedera
  //par consequnt un methode creer_champ(). Cette methode contiendra l appel de la
  //methode d une classe mere qui possede aussi un ou des "champs calcules"
  //La mise a jour du champ ne sera faite que si une requete par la methode get_champ()
  //(voir ci-dessous) est effectuee.
  //Actuellement seules les equations portent des champs calcules car elles
  //disposent d un objet discretisation.
  //////////////////////////////////////////////////////////////////////////////

  //Description de la methode get_champ()
  ///////////////////////////////////////////////////////////////////////
  //Cette methode permet de lancer une requete pour recuperer une REF a un
  //champ a partir de son identifiant (nom) ou de celui d une de ses composantes
  //Le mecanisme general est le suivant :
  //-Le probleme delegue la requete a son milieu puis a ses equations si le milieu n a pas identifie
  //le nom
  //-La classe qui effectue une requete (ex : Navier_Stokes) interroge sa classe mere pour savoir si
  //l identifiant correspond a un champs compris par celle-ci ou a l un de ses attributs.
  //Le mecanisme est recursif, ainsi la classe mere interroge sa propre classe mere ...
  //Si la hierarchie amont de la classe qui a lance la requete (ex : Navier_Stokes) ne reconnait pas
  //l identifiant, celle-ci teste ses propres champs compris puis delegue la requete a ses attributs
  //en cas de reponse negative
  ////////////////////////////////////////////////////////////////////////

  //Description de la methode get_noms_champs_postraitables()
  ////////////////////////////////////////////////////////////////////////
  //Cette methode permet d informer l utilisateur sur les champs postraitables pour un probleme donne
  //Le principe de balayage des classes est le meme que pour la methode get_champ() (voir ci-dessus)
  //-Si l option DESCRIPTION est activee :
  //Chaque classe concernee par le probleme ecrit l identifiant de ses champs (ou de leurs composantes)
  //dans le fichier de sortie (err)
  //-Si l'option NONE est activee (valeur par defaut)
  //la liste des noms est ajoutee dans nom
  ////////////////////////////////////////////////////////////////////////

protected :

};

#endif
