/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Champ_Gen_de_Champs_Gen.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Gen_de_Champs_Gen.h>
#include <Champ_Generique_refChamp.h>
#include <Champs_compris.h>
#include <Discretisation_base.h>
#include <Param.h>

Implemente_base(Champ_Gen_de_Champs_Gen,"Champ_Gen_de_Champs_Gen",Champ_Generique_base);

//cf Champ_Generique_base::readOn
Entree& Champ_Gen_de_Champs_Gen::readOn(Entree& is)
{
  Champ_Generique_base::readOn(is);
  return is;
}

Sortie& Champ_Gen_de_Champs_Gen::printOn(Sortie& os) const
{
  exit();
  return os;
}

//  source :           declenche la lecture du champ genrerique source
//  sources_reference : declenche la lecture du nom d un ou plusieurs champs generiques qui doivent etre
//                       deja definis afin de pouvoir initialiser une reference vers ce champ
//  nom_source : option pour nommer le champ en tant que source (sinon nommer par defaut)
void Champ_Gen_de_Champs_Gen::set_param(Param& param)
{
  param.ajouter_non_std("source",(this));
  param.ajouter_non_std("sources",(this));
  param.ajouter_non_std("nom_source",(this));
  param.ajouter_non_std("source_reference",(this));
  param.ajouter("sources_reference",&noms_sources_ref_);
}

int Champ_Gen_de_Champs_Gen::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="source")
    {
      Champ_Generique& new_src = sources_.add(Champ_Generique());
      Nom typ;
      is >> typ;
      Journal() << "Reading a source type " << typ << finl;
      new_src.typer(typ);
      is >> new_src.valeur();
      return 1;
    }
  else if (mot=="source_reference")
    {
      Nom& new_s = noms_sources_ref_.add(Nom());
      is >> new_s;
      Cerr<<"WARNING : obsolete option. Use now sources_reference { " << new_s<< " } instead of source_reference "<<new_s<<finl;
      return 1;
    }
  else if (mot=="sources")
    {
      Nom accouverte="{";
      Nom accfermee="}";
      Nom virgule=",";
      Nom typ;
      is >> typ;
      assert (typ==accouverte);
      is >> typ;
      if(typ==accfermee)
        {
          Cerr<<"We should not use the keyword \"sources\" to specify a generic field"<<finl;
          Cerr<<"of type Champ_Gen_de_Champs_Gen without specifying sources"<<finl;
          exit();
        }
      while(1)
        {
          Champ_Generique& new_src = sources_.add(Champ_Generique());
          Journal() << "Reading a source type " << typ << finl;
          new_src.typer(typ);
          is >> new_src.valeur();
          is >> typ;
          if(typ==accfermee)
            break;
          if(typ!=virgule)
            {
              Cerr << typ << " : We expected a ',' or '}'" << finl;
              exit();
            }
          is >> typ;
        }
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

LIST(Champ_Generique)& Champ_Gen_de_Champs_Gen::get_set_sources()
{
  return sources_;
}

Champ_Generique& Champ_Gen_de_Champs_Gen::get_set_source()
{
  if (sources_.size() < 1)
    sources_.add(Champ_Generique());
  return sources_[0];
}

void Champ_Gen_de_Champs_Gen::reset()
{
  sources_.vide();
}

void Champ_Gen_de_Champs_Gen::mettre_a_jour(double temps)
{
  const int n = sources_.size();
  for (int i = 0; i < n; i++)
    sources_[i].valeur().mettre_a_jour(temps);
}

Champ_Fonc& Champ_Gen_de_Champs_Gen::creer_espace_stockage(const Nature_du_champ& nature,
                                                           const int& nb_comp,
                                                           Champ_Fonc& es_tmp) const
{

  Noms noms;
  Noms unites;
  for (int c=0; c<nb_comp; c++)
    {
      //  noms.add("bidon");
      unites.add("bidon");
    }
  noms.add("bidon");
  double temps;
  temps = get_time();
  const Discretisation_base&  discr = get_discretisation();
  Motcle directive = get_directive_pour_discr();
  const Zone_dis_base& zone_dis = get_ref_zone_dis_base();

  discr.discretiser_champ(directive,zone_dis,nature,noms,unites,nb_comp,temps,es_tmp);

  if (directive=="pression")
    {
      const  Zone_Cl_dis_base& zcl = get_ref_zcl_dis_base();
      es_tmp->completer(zcl);
    }

  return es_tmp;
}

const Champ_Generique_base& Champ_Gen_de_Champs_Gen::get_source(int i) const
{
  // Si sources_ et source_ref sont vide, on n'a pas a etre ici
  if ((sources_reference_.size()==0) && (sources_.size()==0))
    {
      Cerr << finl;
      Cerr << "Error : The field \"" << noms_sources_ref_ << "\" is not recognized in the sources_reference option of the " << que_suis_je() << " keyword." << finl;
      Cerr << "\"" << noms_sources_ref_ << "\" should be defined before." << finl;
      exit();
    }
  int n_sources_ref=sources_reference_.size();
  if (i<n_sources_ref)
    {
      if ( ! sources_reference_[i].non_nul())
        {
          Cerr << finl;
          Cerr << "Error : The field \"" << noms_sources_ref_[0] << "\" is not recognized in the sources_reference option of the " << que_suis_je() << " keyword." << finl;
          Cerr << "\"" << noms_sources_ref_[0] << "\" should be defined before." << finl;
          exit();
        }
      return sources_reference_[i].valeur();
    }
  else
    return sources_[i-n_sources_ref].valeur();
}

Champ_Generique_base& Champ_Gen_de_Champs_Gen::set_source(int i)
{
  // POur eviter de recoder la meme chose que get_source
  return ref_cast_non_const(Champ_Generique_base,get_source(i));
}


int Champ_Gen_de_Champs_Gen::get_nb_sources() const
{
  return sources_reference_.size()+sources_.size();
}

// Description: sauvegarde des differentes sources
int Champ_Gen_de_Champs_Gen::sauvegarder(Sortie& os) const
{
  const int n = sources_.size();
  int bytes = 0;
  for (int i = 0; i < n; i++)
    bytes += sources_[i].valeur().sauvegarder(os);
  return bytes;
}

// Description: reprise des differentes sources
int Champ_Gen_de_Champs_Gen::reprendre(Entree& is)
{
  const int n = sources_.size();
  for (int i = 0; i < n; i++)
    sources_[i].valeur().reprendre(is);
  return 1;
}

void Champ_Gen_de_Champs_Gen::completer(const Postraitement_base& post)
{
  int n_sources_ref=noms_sources_ref_.size();
  if ( n_sources_ref!=sources_reference_.size())
    {
      sources_reference_.vide();
      for (int i=0; i<n_sources_ref; i++)
        {
          REF(Champ_Generique_base)& source_ref=sources_reference_.add(REF(Champ_Generique_base)());
          const Postraitement& postraitement =ref_cast(Postraitement,post);
          //const Probleme_base& pb = ref_cast(Postraitement,post).probleme();
          source_ref = postraitement.get_champ_post(noms_sources_ref_[i]);
        }
    }
  const int n = get_nb_sources();
  for (int i = n_sources_ref ; i < n; i++)
    {
      Champ_Generique_base& source_a_completer = ref_cast(Champ_Generique_base,set_source(i));
      source_a_completer.completer(post);
    }

  const Probleme_base& pb = get_ref_pb_base();
  const Nom& nom_pb = pb.le_nom();
  nom_pb_ = nom_pb;

  nommer_source();
}

//Methodes complementaires
int Champ_Gen_de_Champs_Gen::get_dimension() const
{
  return get_source(0).get_dimension();
}

void Champ_Gen_de_Champs_Gen::get_property_names(Motcles& list) const
{
  get_source(0).get_property_names(list);
}


const Noms Champ_Gen_de_Champs_Gen::get_property(const Motcle& query) const
{
  Motcles motcles(1);
  motcles[0] = "nom";
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
    }
  return get_source(0).get_property(query);
}

Entity Champ_Gen_de_Champs_Gen::get_localisation(const int index) const
{
  return get_source(0).get_localisation(index);
}

const DoubleTab& Champ_Gen_de_Champs_Gen::get_ref_values() const
{
  return get_source(0).get_ref_values();
}

void Champ_Gen_de_Champs_Gen::get_copy_values(DoubleTab& values) const
{
  const DoubleTab& val = get_ref_values();
  // Cree une copie du tableau
  values = val;
}

void Champ_Gen_de_Champs_Gen::get_xyz_values(const DoubleTab& coords, DoubleTab& values, ArrOfBit& validity_flag) const
{
  throw Champ_Generique_erreur("NOT_IMPLEMENTED");
}

const Domaine& Champ_Gen_de_Champs_Gen::get_ref_domain() const
{
  return get_source(0).get_ref_domain();
}

void Champ_Gen_de_Champs_Gen::get_copy_domain(Domaine& domain) const
{
  return get_source(0).get_copy_domain(domain);
}

const Zone_dis_base& Champ_Gen_de_Champs_Gen::get_ref_zone_dis_base() const
{
  return get_source(0).get_ref_zone_dis_base();
}

const Zone_Cl_dis_base& Champ_Gen_de_Champs_Gen::get_ref_zcl_dis_base() const
{
  return get_source(0).get_ref_zcl_dis_base();
}

const DoubleTab& Champ_Gen_de_Champs_Gen::get_ref_coordinates() const
{
  return get_source(0).get_ref_coordinates();
}

void Champ_Gen_de_Champs_Gen::get_copy_coordinates(DoubleTab& coordinates) const
{
  get_source(0).get_copy_coordinates(coordinates);
}

const IntTab& Champ_Gen_de_Champs_Gen::get_ref_connectivity(Entity index1, Entity index2) const
{
  return get_source(0).get_ref_connectivity(index1,index2);
}

void Champ_Gen_de_Champs_Gen::get_copy_connectivity(Entity index1, Entity index2, IntTab& tab) const
{
  get_source(0).get_copy_connectivity(index1,index2,tab);
}

double Champ_Gen_de_Champs_Gen::get_time() const
{
  return get_source(0).get_time();
}

const Probleme_base& Champ_Gen_de_Champs_Gen::get_ref_pb_base() const
{
  return get_source(0).get_ref_pb_base();
}

const Discretisation_base& Champ_Gen_de_Champs_Gen::get_discretisation() const
{
  return get_source(0).get_discretisation();
}

const Motcle Champ_Gen_de_Champs_Gen::get_directive_pour_discr() const
{
  return get_source(0).get_directive_pour_discr();
}

void Champ_Gen_de_Champs_Gen::nommer_sources()
{
  int nb_sources = get_nb_sources();
  for (int i=0; i<nb_sources; i++)
    {
      if (sub_type(Champ_Gen_de_Champs_Gen,sources_[i].valeur()))
        {
          Champ_Gen_de_Champs_Gen& champ = ref_cast(Champ_Gen_de_Champs_Gen,sources_[i].valeur());
          champ.nommer_sources();
          champ.nommer_source();
        }
      else if (sub_type(Champ_Generique_refChamp,sources_[i].valeur()))
        {
          Champ_Generique_refChamp& champ = ref_cast(Champ_Generique_refChamp,sources_[i].valeur());
          champ.nommer_source();
        }
      else
        {
          Cerr<<"There is no method nommer_source() for this type of field "<<finl;
          Cerr<<"sources_[i].valeur().que_suis_je()"<<finl;
          exit();
        }
    }
}

//Cette methode doit etre surchargee pour chacun des Champ_Gen_de_Champs_Gen
void Champ_Gen_de_Champs_Gen::nommer_source()
{

}

int Champ_Gen_de_Champs_Gen::get_info_type_post() const
{
  return get_source(0).get_info_type_post();

}

//Methodes pour changer t_deb et t_fin pour des reprises de statistiques
void Champ_Gen_de_Champs_Gen::fixer_serie(const double& t1, const double& t2)
{
  const int n = get_nb_sources();;
  for (int i = 0; i < n; i++)
    {
      if (sub_type(Champ_Gen_de_Champs_Gen,get_source(i)))
        {
          Champ_Gen_de_Champs_Gen& source = ref_cast(Champ_Gen_de_Champs_Gen,set_source(i));
          source.fixer_serie(t1,t2);
        }
    }
}

void Champ_Gen_de_Champs_Gen::fixer_tstat_deb(const double& t1, const double& t2)
{
  const int n = get_nb_sources();;
  for (int i = 0; i < n; i++)
    {
      if (sub_type(Champ_Gen_de_Champs_Gen,get_source(i)))
        {
          Champ_Gen_de_Champs_Gen& source = ref_cast(Champ_Gen_de_Champs_Gen,set_source(i));
          source.fixer_tstat_deb(t1,t2);
        }
    }
}

void Champ_Gen_de_Champs_Gen::lire_bidon(Entree& is) const
{

  const int n = get_nb_sources();;
  for (int i = 0; i < n; i++)
    {
      if (sub_type(Champ_Gen_de_Champs_Gen,get_source(i)))
        {
          const Champ_Gen_de_Champs_Gen& source = ref_cast(Champ_Gen_de_Champs_Gen,get_source(i));
          source.lire_bidon(is);
        }
    }
}

const Champ_Generique_base& Champ_Gen_de_Champs_Gen::get_champ_post(const Motcle& nom) const
{
  REF(Champ_Generique_base) ref_champ;

  try
    {
      return Champ_Generique_base::get_champ_post(nom);
    }
  catch (Champs_compris_erreur)
    {
    }
  try
    {
      return get_source(0).get_champ_post(nom);
    }
  catch (Champs_compris_erreur)
    {
    }

  throw Champs_compris_erreur();
  return ref_champ;
}

int Champ_Gen_de_Champs_Gen::comprend_champ_post(const Motcle& identifiant) const
{
  if (Champ_Generique_base::comprend_champ_post(identifiant))
    return 1;
  else if (get_source(0).comprend_champ_post(identifiant))
    return 1;

  return 0;
}

