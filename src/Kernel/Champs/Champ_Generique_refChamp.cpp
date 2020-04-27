/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Champ_Generique_refChamp.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Generique_refChamp.h>
#include <Champ_Inc_P0_base.h>
#include <Champ_Inc_P1_base.h>
#include <Champ_Fonc_P0_base.h>
#include <Champ_Fonc_P1_base.h>
#include <Zone_VF.h>
#include <Champ_Uniforme.h>
#include <Champ_Inc_Q1_base.h>
#include <Champ_Fonc_Q1_base.h>
#include <Equation_base.h>
#include <Synonyme_info.h>
#include <Param.h>

Implemente_instanciable(Champ_Generique_refChamp,"refChamp",Champ_Generique_base);
Add_synonym(Champ_Generique_refChamp,"Champ_Post_refChamp");

// Description:
// Imprime sur un flot de sortie.
// Parametre: Sortie& os
// Signification: un flot de sortie
// Retour: Sortie&
// Signification: le flot de sortie modifie
Sortie& Champ_Generique_refChamp::printOn(Sortie& os) const
{
  return os;
}

//cf Champ_Generique_base::readOn
Entree& Champ_Generique_refChamp::readOn(Entree& is)
{
  Champ_Generique_base::readOn(is);
  return is;
}

// Description:
//  pb_champ :   declenche la lecture du nom du probleme (nom_pb_) auquel appartient
//                 le champ discret et le nom de ce champ discret (nom_champ_)
//  nom_source : option pour nommer le champ en tant que source (sinon nommer par defaut)
void Champ_Generique_refChamp::set_param(Param& param)
{
  param.ajouter_non_std("nom_source",(this));
  param.ajouter_non_std("Pb_champ",(this),Param::REQUIRED);
}

int Champ_Generique_refChamp::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="Pb_champ")
    {
      // Lecture du nom du probleme et du nom du champ
      is >> nom_pb_ >> nom_champ_;
      // Recherche du probleme parmi les objets connus de l'interprete
      const Objet_U& ob = interprete().objet(nom_pb_);
      if (!sub_type(Probleme_base, ob))
        {
          Cerr << "Error in Champ_Generique_refChamp ::lire(keyword=\"pb_champ\"\n"
               << " The object " << nom_pb_ << " is not a Probleme_base" << finl;
          exit();
        }
      Probleme_base& pb = ref_cast_non_const(Probleme_base, ob);
      // Recherche du champ "nom_champ" dans le probleme:
      REF(Champ_base) ref_champ;
      Noms liste_noms;
      pb.get_noms_champs_postraitables(liste_noms);
      pb.creer_champ(nom_champ_);
      ref_champ = pb.get_champ(nom_champ_);
      ref_champ.valeur().corriger_unite_nom_compo();
      set_ref_champ(ref_champ.valeur());
      return 1;
    }
  else if (mot=="nom_source")
    {
      Nom id_source;
      is >> id_source;
      nommer(id_source);
      return 1;
    }
  else
    return Champ_Generique_base::lire_motcle_non_standard(mot,is);
  return 1;
}

// Description:
//  On initialise la classe avec le champ en parametre.
//  On prend une ref a ce champ (il doit rester valable ensuite).
void Champ_Generique_refChamp::initialize(const Champ_base& champ)
{
  ref_champ_ = champ;
}

// Description: Renvoie le nombre de coordonnees de chaque sommet du domaine.
//  Voir GenericField_base::get_dimension()
int Champ_Generique_refChamp::get_dimension() const
{
  // Je n'utilise pas Objet_U::dimension expres car j'espere supprimer
  // cette variable statique bientot.
  const DoubleTab coords = get_ref_coordinates();
  const int dim = coords.dimension(1);
  return dim;
}

void Champ_Generique_refChamp::get_property_names(Motcles& list) const
{
  list.add("nom");
  list.add("nom_cible");
  list.add("unites");
  list.add("composantes");
  list.add("synonyms");
}

const Noms Champ_Generique_refChamp::get_property(const Motcle& query) const
{

  Motcles motcles(5);
  motcles[0] = "nom";
  motcles[1] = "nom_cible";
  motcles[2] = "unites";
  motcles[3] = "composantes";
  motcles[4] = "synonyms";

  int rang = motcles.search(query);
  switch(rang)
    {
    case 0:
      {
        Noms mots(1);
        mots[0] = nom_post_;
        return mots;
        break;
      }
    case 1:
      {
        Noms mots(1);
        mots[0] = nom_champ_;
        return mots;
        break;
      }
    case 2 :
      {
        const Noms mots0= get_ref_champ_base().unites();
        ref_cast_non_const(Champ_base,get_ref_champ_base()).corriger_unite_nom_compo();
        const Noms mots = get_ref_champ_base().unites();
        if (mots.size()!=mots0.size())
          {
            Cerr<<"iuuuuuu"<<mots<<" "<<mots0<<finl;
            exit();
          }
        for (int i=0; i<mots.size(); i++)
          {
            if (mots0[i]!=mots[i])
              {
                Cerr <<" iiiiiiiiii"<<mots0[i]<< " "<<mots[i]<<finl;
                exit();
              }
          }
        return mots;
        break;
      }
    case 4 :
      {
        const Noms mots = get_ref_champ_base().get_synonyms();

        return mots;
        break;
      }
    case 3 :
      {

        {
          const Noms mots = get_ref_champ_base().noms_compo();
          int nb_comp = mots.size();

          Noms compo(nb_comp);
          for (int i=0; i<nb_comp; i++)
            {
              Nom nume(i);
              compo[i] = nom_post_+nume;
            }
          return compo;
          break;
        }

      }
    default :
      {
        Cerr<<"The identifiable properties are : "<<motcles<<finl;
        exit();
      }
    }
  //Pour compilation
  // On n'arrive jamais ici
  return get_property(query);

}

// Description:
//  Si le champ n'est pas un champ discret : exception Champ_Generique_erreur("INVALID")
//  Sinon, renvoie la localisation du champ pour le support "index".
//  Si index == -1 (valeur par defaut), leve une exception si le champ est multisupport.
//  Sinon, leve une exception si l'index est superieur au nombre de localisations du champ.
Entity Champ_Generique_refChamp::get_localisation(const int index) const
{
  Entity loc;
  //Pour initialisation
  loc = NODE;

  const Champ_base& ch = get_ref_champ_base();
  const Zone_dis_base& z_dis_base = get_ref_zone_dis_base();

  // Champs discrets a une seule localisation :
  if ((sub_type(Champ_Inc_P0_base, ch) || sub_type(Champ_Fonc_P0_base, ch)) && index <= 0)
    {
      loc = ELEMENT;
    }
  else if ((sub_type(Champ_Inc_P1_base, ch)|| sub_type(Champ_Fonc_P1_base, ch)) && index <= 0)
    {
      loc = NODE;
    }
  else if ((ch.que_suis_je()=="Champ_Face_PolyMAC" || ch.que_suis_je()=="Champ_Face_CoviMAC" || ch.valeurs().dimension(0) == ref_cast(Zone_VF,z_dis_base).nb_faces()) && index <= 0)
    {
      loc = FACE;
    }
  else
    {
      // Champs discrets a plusieurs localisations
      Nom message="Invalid localization used for postprocessing the field ";
      message+=ch.le_nom()+". Change your data file.";
      throw Champ_Generique_erreur(message);
    }
  return loc;
}

// Description: Verifie que le champ est bien un champ discret et
//  renvoie le tableau de valeurs. Sinon, leve l'exception Champ_Generique_erreur("NO_REF")
const DoubleTab& Champ_Generique_refChamp::get_ref_values() const
{
  // Appel a get_localisation pour verifier que le champ est bien un champ discret
  // (multi-support ou non)
  get_localisation(0);
  // Renvoie les valeurs du champ
  const DoubleTab& val = get_ref_champ_base().valeurs();
  return val;
}

// Description: Cree une copie du tableau de valeurs
//  Voir GenericField_base::get_copy_values()
void Champ_Generique_refChamp::get_copy_values(DoubleTab& values) const
{
  const DoubleTab& val = get_ref_values();
  // Cree une copie du tableau
  values = val;
}

// Description: appel a Champ_base::valeur_aux()
void Champ_Generique_refChamp::get_xyz_values(const DoubleTab& coords, DoubleTab& values, ArrOfBit& validity_flag) const
{
  throw Champ_Generique_erreur("NOT_IMPLEMENTED");
}

const Zone_Cl_dis_base& Champ_Generique_refChamp::get_ref_zcl_dis_base() const
{
  const Champ_base& ch = get_ref_champ_base();
  if (sub_type(Champ_Inc_base,ch))
    return ref_cast(Champ_Inc_base,ch).equation().zone_Cl_dis().valeur();
  else
    {
      Cerr<<"No zcl_dis is available for the field "<<ch.que_suis_je()<<finl;
      exit();
    }

  //Pour compilation
  return get_ref_zcl_dis_base();
}

const DoubleTab& Champ_Generique_refChamp::get_ref_coordinates() const
{
  const DoubleTab& coord = get_ref_domain().coord_sommets();
  return coord;
}

void Champ_Generique_refChamp::get_copy_coordinates(DoubleTab& coordinates) const
{
  const DoubleTab& coord = get_ref_coordinates();
  coordinates = coord;
}

const IntTab& Champ_Generique_refChamp::get_ref_connectivity(Entity index1, Entity index2) const
{
  const Champ_base& ch = get_ref_champ_base();
  const Zone_dis_base& zone_dis_base = ch.zone_dis_base();
  const Zone& zone = zone_dis_base.zone();
  const Zone_VF& zone_vf = ref_cast(Zone_VF, zone_dis_base);

  switch(index1)
    {
    case ELEMENT:
      {
        switch(index2)
          {
          case NODE:
            return zone.les_elems();
          case FACE:
            return zone_vf.elem_faces();
          default :
            {
              exit();
            }
          }
        break;
      }
    case FACE:
      {
        switch(index2)
          {
          case NODE:
            return zone_vf.face_sommets();
          case ELEMENT:
            return zone_vf.face_voisins();
          default :
            {
              exit();
            }
          }
        break;
      }
    default :
      {
        exit();
      }
    }
  Nom message="Invalid localization used for postprocessing the field ";
  message+=ch.le_nom()+". Change your data file.";
  throw Champ_Generique_erreur(message);
  // On n'arrive jamais ici
  return get_ref_connectivity(index1,index2);
}

void Champ_Generique_refChamp::get_copy_connectivity(Entity index1, Entity index2, IntTab& tab) const
{
  const IntTab& connectivity = get_ref_connectivity(index1, index2);
  tab = connectivity;
}

// Renvoie le probleme qui porte le champ cible
const Probleme_base& Champ_Generique_refChamp::get_ref_pb_base() const
{
  const Objet_U& ob = interprete().objet(nom_pb_);
  const Probleme_base& pb = ref_cast(Probleme_base,ob);
  return pb;
}

// Description:
//  Renvoie le champ_base sous-jacent. Teste si le champ a bien ete associe.
//  A terme, cette methode disparaitra de la classe GenericField_base mais reste
//  dans celle-ci (on y teste si le champ a ete associe).
// Exceptions:
//  Champ_Generique_erreur("NOT_INITIALIZED")
const Champ_base& Champ_Generique_refChamp::get_ref_champ_base() const
{
  if (!ref_champ_.non_nul())
    throw Champ_Generique_erreur("NOT_INITIALIZED");

  const Champ_base& ch = ref_champ_.valeur();
  return ch;
}

void Champ_Generique_refChamp::reset()
{
  ref_champ_.reset();
  localisation_ = Motcle();
}

void Champ_Generique_refChamp::completer(const Postraitement_base& post)
{
  nommer_source();
}

// Description:
//  Voir Champ_Generique_base::mettre_a_jour
//  Si le champ est champ_inc dans l'equation, il doit deja avoir
//  ete mis a jour par l'equation.
// Si c est un clacule, l actualisation est faite dans get_champ
void Champ_Generique_refChamp::mettre_a_jour(double temps)
{

}

// Description:
//  Voir Champ_Generique_base::get_champ.
//  Ici, l'espace_stockage n'est pas utilise, le champ existe deja
const Champ_base& Champ_Generique_refChamp::get_champ(Champ& espace_stockage) const
{
  {
    Objet_U& ob = Interprete::objet(nom_pb_);
    const Probleme_base& pb = ref_cast(Probleme_base,ob);
    const Nom& nom_cible = get_ref_champ_base().le_nom();
    pb.get_champ(nom_cible);
    return get_ref_champ_base();
  }

}

const Champ_base& Champ_Generique_refChamp::get_champ_without_evaluation(Champ& espace_stockage) const
{
  return get_champ(espace_stockage);
}

// Description:
//  Associe le champ et determine sa localisation.
void Champ_Generique_refChamp::set_ref_champ(const Champ_base& champ)
{
  assert(!ref_champ_.non_nul());
  ref_champ_ = champ;

  // Determination de la localisation du champ
  // On code quelques champs de types connus...
  // Pour les autres, il faut les interpoler quelque part.
  // L'ideal serait la localisation soit une propriete du champ discret lui-meme.

  Nom type = champ.que_suis_je();
  type.majuscule();

  if (type.debute_par("CHAMP_P0"))
    {
      localisation_ = "ELEMENTS";
    }
  else if (type == "CHAMP_P1")
    {
      localisation_ = "SOMMETS";
    }
  else if (type.debute_par("CHAMP_FACE"))
    {
      localisation_ = "FACES";
    }
  else if (type == "CHAMP_P1NC")
    {
      localisation_ = "FACES";
    }
  else
    {
      localisation_ = "LOCALISATION_INCONNUE";
    }
}

//Renvoie le temps du champ cible
double Champ_Generique_refChamp::get_time() const
{
  double temps;
  temps = get_ref_champ_base().temps();
  return temps;
}

//Renvoie la directive (champ_elem, champ_sommets, champ_face ou pression)
//pour lancer la discretisation de l espace de stockage rendu par
//la methode get_champ() du Champ_Generique qui a lance l appel de cette methode
const Motcle Champ_Generique_refChamp::get_directive_pour_discr() const
{
  Motcle directive;
  const Champ_base& ch = get_ref_champ_base();

  // Champs discrets a une seule localisation :
  if (sub_type(Champ_Inc_P0_base,ch) || sub_type(Champ_Fonc_P0_base,ch))
    {
      directive = "champ_elem";
      //    assert(localisation_=="ELEMENTS");
    }
  else if (sub_type(Champ_Inc_P1_base,ch) || sub_type(Champ_Fonc_P1_base,ch)
           || sub_type(Champ_Inc_P1_base,ch) || sub_type(Champ_Inc_Q1_base,ch)
           || sub_type(Champ_Fonc_Q1_base,ch))
    {
      directive = "champ_sommets";
      //   assert(localisation_=="SOMMETS");
    }
  else
    {
      const Nom& type = ch.que_suis_je();
      if ((type.debute_par("Champ_Face")) || (type=="Champ_P1NC") || (type=="Champ_Q1NC") ||
          (type=="Champ_Fonc_Face") || (type=="Champ_Fonc_P1NC") || (type=="Champ_Fonc_Q1NC"))
        {
          directive = "champ_face";
          //      assert(localisation_=="FACES");
        }
      else if ((type=="Champ_P1_isoP1Bulle") || (type=="Champ_Fonc_P1_isoP1Bulle"))
        {
          directive = "pression";
        }
      else if (sub_type(Champ_Uniforme,ch))
        {
          directive = "champ_uniforme";
        }

      else if (sub_type(Champ_Don_base,ch))
        {
          directive = "champ_don";
        }

      else
        {
          Cerr<<"No directive is available to create a storing field for the source field of type "<<ch.que_suis_je()<<finl;
          exit();
        }
    }

  return directive;

}

void Champ_Generique_refChamp::set_nom_champ(const Motcle& nom)
{
  nom_champ_=nom;
}

//Nomme le champ en tant que source par defaut
//nom_champ_base + "_natif_" + nom_dom_natif
void Champ_Generique_refChamp::nommer_source()
{
  if (nom_post_=="??")
    {
      Nom nom_post_source, nom_champ_base, nom_dom_natif;
      nom_champ_base = get_ref_champ_base().le_nom();
      nom_dom_natif = get_ref_domain().le_nom();
      nom_post_source =  nom_champ_base + "_natif_" + nom_dom_natif;
      nommer(nom_post_source);
    }
}

int Champ_Generique_refChamp::get_info_type_post() const
{
  return 0;
}
