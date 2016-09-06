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
// File:        Champ_Generique_Interpolation.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Generique_Interpolation.h>
#include <Postraitement.h>
#include <Discretisation_base.h>
#include <Zone_VF.h>
#include <Synonyme_info.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Champ_Generique_Interpolation,"Interpolation",Champ_Gen_de_Champs_Gen);
Add_synonym(Champ_Generique_Interpolation,"Champ_Post_Interpolation");

// Description: voir reset()
Champ_Generique_Interpolation::Champ_Generique_Interpolation()
{
  // valeurs par defaut
  reset();
}

//cf Champ_Gen_de_Champs_Gen::readOn
Entree& Champ_Generique_Interpolation::readOn(Entree& is)
{
  Champ_Gen_de_Champs_Gen::readOn(is);
  set_localisation(localisation_);
  set_methode(methode_);

  return is;
}

// Description: appel invalide
Sortie& Champ_Generique_Interpolation::printOn(Sortie& os) const
{
  exit();
  return os;
}

//  localisation : localisation de l interpolation (elem ou som)
//  domaine          : domaine d interpolation (optionnel)
//  methode          : type de methode de calcul "calculer_champ_post" (optionnel, une seule methode disponible)
void Champ_Generique_Interpolation::set_param(Param& param)
{
// XD interpolation champ_post_de_champs_post interpolation -1 To create a field which is an interpolation of the field given by the keyword source.
  Champ_Gen_de_Champs_Gen::set_param(param);
  param.ajouter("localisation",&localisation_,Param::REQUIRED); // XD_ADD_P chaine type_loc indicate where is done the interpolation (elem for element or som for node).
  param.ajouter("methode",&methode_); //  XD_ADD_P chaine The optional keyword methode is limited to calculer_champ_post for the moment.
  param.ajouter("domaine",&nom_domaine_lu_);  //  XD_ADD_P chaine  the domain name where the interpolation is done (by default, the calculation domain)
  param.ajouter("optimisation_sous_maillage",&optimisation_demande_); // XD_ADD_P dico not_set
  param.dictionnaire("default",-1); // XD_ADD_DICO not_set
  param.dictionnaire("yes",1);  // XD_ADD_DICO not_set
  param.dictionnaire("no",0); // XD_ADD_DICO not_set

}
// Description: restore la configuration initiale de l'objet
//  localisation non specifiee, source non specifiee, methode=calculer_champ_post, domaine natif
void Champ_Generique_Interpolation::reset()
{
  nom_domaine_lu_ = "";
  localisation_ = "";
  methode_      = "calculer_champ_post";
  domaine_.reset();
  Champ_Gen_de_Champs_Gen::reset();
  optimisation_sous_maillage_=-1;
  optimisation_demande_=-1;
}

// Description: Initialisation de la classe: initialisation de la localisation
//  demandee. Parametres valides : elem, som.
int Champ_Generique_Interpolation::set_localisation(const Motcle& loc, int exit_on_error)
{
  Motcles localisations(3);
  localisations[0] = "elem";
  localisations[1] = "som";
  localisations[2] = "faces";
  int ok = 0;
  if (localisations.search(loc) >= 0)
    {
      localisation_ = loc;
      ok = 1;
    }
  else
    {
      Cerr << "Error in Champ_Generique_Interpolation::set_localisation\n"
           << " Keyword read : " << loc << "\n The localizations allowed are " << localisations << finl;
      if (exit_on_error)
        {
          exit();
        }
    }
  return ok;
}

// Description: Initialisation de la classe: initialisation de la methode d'interpolation
//  Parametres valides : calculer_champ_post
//  (Idees pour la suite: affecter, affecter avec integrale...)
int Champ_Generique_Interpolation::set_methode(const Motcle& methode, int exit_on_error)
{
  Motcles methodes_interpolation(1);
  methodes_interpolation[0] = "calculer_champ_post";
  int ok = 0;
  if (methodes_interpolation.search(methode) >= 0)
    {
      methode_ = methode;
      ok = 1;
    }
  else
    {
      Cerr << "Error in Champ_Generique_Interpolation::set_methode\n"
           << " Keyword read : " << methode << "\n The methods allowed are " << methodes_interpolation << finl;
      if (exit_on_error)
        {
          exit();
        }
    }
  return ok;
}

// Description: Initialisation de la classe: initialisation du domaine d'interpolation
//  Parametres valides : "" => domaine natif, ou le nom d'un objet Domaine connu de l'interprete
int Champ_Generique_Interpolation::set_domaine(const Nom& nom_domaine, int exit_on_error)
{
  int ok = 0;
  if (nom_domaine == "")
    {
      domaine_.reset(); // reference nulle => domaine natif du champ
      ok = 1;
    }
  else
    {
      const Objet_U& ob = interprete().objet(nom_domaine);
      if (!sub_type(Domaine, ob))
        {
          Cerr << "Error in Champ_Generique_Interpolation::set_domaine(nom_domaine)\n"
               << " The object " << nom_domaine << " is not a domain" << finl;
          if (exit_on_error)
            {
              exit();
            }
        }
      else
        {
          domaine_ = ref_cast(Domaine, ob);
        }
    }
  return ok;
}

// Description: Interpolation du champ source en fonction de la methode, localisation et
//  domaine demandes.
// Precondition: localisation et source_[0] sont initialises
const Champ_base& Champ_Generique_Interpolation::get_champ(Champ& espace_stockage) const
{
  if (localisation_ == "")
    {
      Cerr << "Error in Champ_Generique_Interpolation::get_champ()\n"
           << " Localisation has not been initialized" << finl;
      exit();
    }

  if (methode_ == "calculer_champ_post")
    {
      get_champ_with_calculer_champ_post(espace_stockage);
    }
  else
    {
      Cerr << "Champ_Generique_Interpolation::get_champ : methode " << methode_ << " uncoded" << finl;
      exit();
    }

  return espace_stockage.valeur();
}

// Description: Interpolation du champ source a l'aide de Champ_base::calculer_champ_xxx_post
const Champ_base& Champ_Generique_Interpolation::get_champ_with_calculer_champ_post(Champ& espace_stockage) const
{
  Champ espace_stockage_source;
  const Champ_base& source0 = get_source(0).get_champ(espace_stockage_source);
  Champ source_bis;

  if (optimisation_sous_maillage_==-1)
    {
      source_bis=source0;
      //source.typer(source0.que_suis_je());

    }
  const Champ_base& source  = ((optimisation_sous_maillage_==-1)?source_bis.valeur():source0);
  Nom nom_champ_interpole;

  // Domaine sur lequel on interpole le champ :
  //  si domaine_ est une ref nulle, on prend le domaine natif du champ.
  const Domaine& domaine = get_ref_domain();
  const Noms compo = get_property("composantes");
  const Noms nom_champ = get_property("nom");
  const Noms syno = get_property("synonyms");

  int ncomp;
  //On fixe identifiant_appel_ a nom_champ[0] dans le cas ou il est egal a ??
  if (identifiant_appel_=="??")
    {
      ncomp = -1;
    }
  else
    {
      ncomp = Champ_Generique_base::composante(identifiant_appel_,nom_champ[0],compo,syno);
    }

  //Creation du champ espace_stockage
  Nature_du_champ nature_source = (ncomp==-1)?source.nature_du_champ():scalaire;
  nature_source = source.nature_du_champ();
  int nb_comp = source.nb_comp();

  Champ_Fonc es_tmp;
  espace_stockage = creer_espace_stockage(nature_source,nb_comp,es_tmp);

  //double default_value=-1e35;
  //espace_stockage.valeurs()=default_value;
  int decal=10;
  if (optimisation_sous_maillage_==-1)
    {
      espace_stockage.valeurs()=0;
      // premier appel avec maillage different (ou on a force) , on essaye de voir si on peut optimiser
      // on champ la source on y met val(i)=i, pour recuperer le numero de la maille apres
      DoubleTab& val=source_bis.valeurs();

      int dim0 = val.dimension_tot(0);
      if (val.nb_dim()==2)
        {
          for (int i=0; i<dim0; i++)
            for (int j=0; j<nb_comp; j++)
              val(i,j) = i+decal;
        }
      else
        {
          for (int i=0; i<dim0; i++)
            val(i) = i+decal;
        }
    }

  //Evaluation des valeurs du champ espace_stockage
  DoubleTab& espace_valeurs = espace_stockage->valeurs();

  if (optimisation_sous_maillage_==1)
    {
      const DoubleTab& val_temp= source.valeurs();
      int dim0 = espace_valeurs.dimension(0);
      if (ncomp==-1)
        {
          if (nb_comp==1)
            for (int i=0; i<dim0; i++)
              {
                int n=renumerotation_maillage_[i];
                if (n>=0)
                  espace_valeurs(i) = val_temp(n);
              }
          else
            for (int i=0; i<dim0; i++)
              for (int j=0; j<nb_comp; j++)
                {
                  int n=renumerotation_maillage_[i];
                  if (n>=0)
                    espace_valeurs(i,j) = val_temp(n,j);
                }
        }
      else
        //On construit un tableau de valeurs a nb_comp composantes meme si ncomp!=-1
        {
          for (int i=0; i<dim0; i++)
            for (int j=0; j<nb_comp; j++)
              if (j==ncomp)
                {
                  espace_valeurs(i,j) = val_temp(renumerotation_maillage_[i]);
                }
        }




      espace_valeurs.echange_espace_virtuel();
      return espace_stockage.valeur();
    }

  int imax=0;
  //  int jmax=0;;
  imax = espace_valeurs.dimension(0);
  //if (espace_valeurs.nb_dim()==2)
  //  jmax = espace_valeurs.dimension(1);

  if (localisation_ == "elem")
    {
      const int nb_elements = domaine.zone(0).nb_elem();

      if (ncomp==-1)
        {
          DoubleTab val_temp;
          source.calculer_valeurs_elem_post(val_temp,
                                            nb_elements,
                                            nom_champ_interpole,
                                            domaine);

          if (nb_comp==1)
            for (int i_val=0; i_val<imax; i_val++)
              {
                espace_valeurs(i_val) = val_temp(i_val);
              }
          else
            for (int i_val=0; i_val<imax; i_val++)
              for (int j_val=0; j_val<nb_comp; j_val++)
                {
                  espace_valeurs(i_val,j_val) = val_temp(i_val,j_val);
                }

        }
      else
        //On construit un tableau de valeurs a nb_comp composantes meme si ncomp!=-1
        {
          DoubleTab val_temp;
          source.calculer_valeurs_elem_compo_post(val_temp,
                                                  ncomp,
                                                  nb_elements,
                                                  nom_champ_interpole,
                                                  domaine);

          int dim0 = val_temp.dimension(0);
          for (int i=0; i<dim0; i++)
            for (int j=0; j<nb_comp; j++)
              if (j==ncomp)
                {
                  espace_valeurs(i,j) = val_temp(i);
                }
        }

    }
  else if (localisation_=="som")
    {
      const int nb_sommets = domaine.nb_som();

      if (ncomp==-1)
        {
          DoubleTab val_temp;
          source.calculer_valeurs_som_post(val_temp,
                                           nb_sommets,
                                           nom_champ_interpole,
                                           domaine);
          if (nb_comp==1)
            for (int i_val=0; i_val<imax; i_val++)
              {
                espace_valeurs(i_val) = val_temp(i_val);
              }
          else
            for (int i_val=0; i_val<imax; i_val++)
              for (int j_val=0; j_val<nb_comp; j_val++)
                {
                  espace_valeurs(i_val,j_val) = val_temp(i_val,j_val);
                }
        }
      else
        //On construit un tableau de valeurs a nb_comp composantes meme si ncomp!=-1
        {
          DoubleTab val_temp;
          source.calculer_valeurs_som_compo_post(val_temp,
                                                 ncomp,
                                                 nb_sommets,
                                                 nom_champ_interpole,
                                                 domaine);

          int dim0 = val_temp.dimension(0);
          for (int i=0; i<dim0; i++)
            for (int j=0; j<nb_comp; j++)
              if (j==ncomp)
                {
                  espace_valeurs(i,j) = val_temp(i);
                }
        }
    }
  else
    {
      //     const Nom& nom_source = source.que_suis_je();
      //if (((nom_source=="Champ_P1NC") || (nom_source=="Champ_Fonc_P1NC")) && (localisation_=="faces"))
      // {
      espace_stockage->affecter(source);
      // }
      /* else
          {
            Cerr << "Champ_Generique_Interpolation::get_champ localisation uncoded " << localisation_ << " for " <<source.que_suis_je()<<finl;
            exit();
          }
      */
    }



  if (optimisation_sous_maillage_==-1)
    {
      // on recalcule le bon champ

      DoubleTab& val=espace_valeurs;
      int dim0 = val.dimension_tot(0);
      ArrOfInt& renumerotation_maillage=ref_cast_non_const(ArrOfInt, renumerotation_maillage_);
      renumerotation_maillage.resize_array(dim0);
      int ok=1;

      for (int i=0; (i<dim0)&& ok; i++)
        {
          double v;
          if (val.nb_dim()==2)
            v=espace_valeurs(i,0) ;
          else
            v=espace_valeurs(i) ;

          int iproche=-1;
          if (v>0)
            {
              iproche=int(v+0.1); // on ajoute 0.1 pour le cas ou v vaut i-epsilon
              if (dabs(iproche-v)>1e-5)
                {
                  ok=0;
                  Cerr<<nom_champ[0]<<" optimisation ko "<< finl;

                }
            }
          renumerotation_maillage[i]=iproche-decal;

        }

      int test=(int)mp_min(ok);
      if (test)
        {
          ref_cast_non_const(Champ_Generique_Interpolation,(*this)).optimisation_sous_maillage_=0;
          Champ espace_stockage_test;
          get_champ_with_calculer_champ_post(espace_stockage_test);
          ref_cast_non_const(Champ_Generique_Interpolation,(*this)).optimisation_sous_maillage_=test;
          get_champ_with_calculer_champ_post(espace_stockage);
          espace_stockage_test.valeurs()-=espace_stockage.valeurs();
          double dmax= mp_max_abs_vect(espace_stockage_test.valeurs());
          if (dmax > 1e-7)
            {
              Cerr<<nom_champ[0]<<" optimisation ko "<< dmax<<finl;
              exit();
            }
          else
            Cerr<<nom_champ[0]<<" optimisation ok "<< dmax<<finl;
        }

      else
        {
          if (optimisation_demande_==1)
            {
              Cerr<<nom_champ[0]<<" optimisation not allowed "<<finl;
              exit();
            }
        }
      ref_cast_non_const(Champ_Generique_Interpolation,(*this)).optimisation_sous_maillage_=test;
      return get_champ_with_calculer_champ_post(espace_stockage);
    }

  espace_valeurs.echange_espace_virtuel();
  return espace_stockage.valeur();
}

const DoubleTab& Champ_Generique_Interpolation::get_ref_values() const
{
  Champ champ_a_ecrire;
  get_champ(champ_a_ecrire);
  return champ_a_ecrire.valeurs();
}

void Champ_Generique_Interpolation::get_copy_values(DoubleTab& values) const
{
  const DoubleTab& val = get_ref_values();
  // Cree une copie du tableau
  values = val;
}

Entity Champ_Generique_Interpolation::get_localisation(const int index) const
{
  Entity loc;
  if (localisation_=="elem")
    {
      loc = ELEMENT;
    }
  else if (localisation_=="som")
    {
      loc = NODE;
    }
  else if (localisation_=="faces")
    {
      loc = FACE;
    }
  else
    {
      Cerr << "Error of type : localisation should be specified to elem or som or faces for the defined field " << nom_post_ << finl;
      throw Champ_Generique_erreur("INVALID");
    }
  return loc;
}

const Domaine& Champ_Generique_Interpolation::get_ref_domain() const
{
  if (domaine_.non_nul())
    {
      return domaine_.valeur();
    }
  else
    {
      return get_source(0).get_ref_domain();
    }
}

void Champ_Generique_Interpolation::get_copy_domain(Domaine& domain) const
{
  const Domaine& dom = get_ref_domain();
  domain = dom;
}

const DoubleTab& Champ_Generique_Interpolation::get_ref_coordinates() const
{
  const DoubleTab& coord = get_ref_domain().coord_sommets();
  return coord;
}

void Champ_Generique_Interpolation::get_copy_coordinates(DoubleTab& coordinates) const
{
  const DoubleTab& coord = get_ref_coordinates();
  coordinates = coord;
}

const Noms Champ_Generique_Interpolation::get_property(const Motcle& query) const
{

  Motcles motcles(2);
  motcles[0] = "composantes";
  motcles[1] = "synonyms";
  int rang = motcles.search(query);
  switch(rang)
    {

    case 0:
      {
        Noms source_compos = get_source(0).get_property("composantes");
        int nb_comp = source_compos.size();
        Noms compo(nb_comp);

        if (compo_.size()>1)
          {
            //Le champ a ete cree par macro et on a rempli l attribut compo_ du Champ_Generique
            //Cela permet de reproduire les noms de composantes dans les lml
            compo=compo_;
          }
        else
          {
            for (int i=0; i<nb_comp; i++)
              {
                Nom nume(i);
                compo[i] = nom_post_+nume;
              }
          }
        return compo;
        break;
      }
    case 1:
      {
        if (syno_.size()>0)
          {
            return syno_;
          }
        return get_source(0).get_property("synonyms");
        break;
      }

    }

  return Champ_Gen_de_Champs_Gen::get_property(query);

}
//Nomme le champ en tant que source par defaut
//nom_champ_source + "_" + localisation_ + "_" +nom_dom_interp;
void Champ_Generique_Interpolation::nommer_source()
{
  if (nom_post_=="??")
    {
      Nom nom_post_source, nom_champ_source, nom_dom_interp;
      const Noms nom = get_source(0).get_property("nom");
      nom_champ_source = nom[0];
      nom_dom_interp = get_ref_domain().le_nom();
      nom_post_source = nom_champ_source + "_" + localisation_ + "_" +nom_dom_interp;
      nommer(nom_post_source);
    }
}

const Noms& Champ_Generique_Interpolation::fixer_noms_synonyms(const Noms& noms)
{
  return syno_=noms;
}
const Noms& Champ_Generique_Interpolation::fixer_noms_compo(const Noms& noms)
{
  return compo_=noms;
}

void Champ_Generique_Interpolation::completer(const Postraitement_base& post)
{
  Champ_Gen_de_Champs_Gen::completer(post);
  // Verifications:
  if (get_nb_sources() != 1)
    {
      Cerr << "Error in Champ_Generique_Interpolation::readOn\n"
           << " You must specify exactly 1 \"Source\"." << finl;
      exit();
    }
  // Si le domaine lu n'est pas le domaine de calcul, on fixe le domaine
  const Domaine& domaine_calcul = get_source(0).get_ref_domain();
  if (optimisation_demande_!=1)
    optimisation_sous_maillage_=0;
  if ( (nom_domaine_lu_!="") && (nom_domaine_lu_!=domaine_calcul.le_nom()) )
    {
      set_domaine(nom_domaine_lu_);
    }
  else
    {
      // optimisation sous maillage inutile
      if (optimisation_demande_==-1)
        optimisation_sous_maillage_=0;
    }

  discretiser_domaine();
}
const Zone_dis_base& Champ_Generique_Interpolation::get_ref_zone_dis_base() const
{
  if (domaine_.non_nul())
    {
      if (le_dom_dis.nombre_de_zones()==0)
        {
          Cerr << "Error in Interpolation definition:" << finl;
          Cerr << "The domain " << domaine_.le_nom() << " is not built." << finl;
          exit();
        }
      const Zone_dis_base& zone_dis = le_dom_dis.zone_dis(0).valeur();
      return zone_dis;
    }
  else
    {
      return Champ_Gen_de_Champs_Gen::get_ref_zone_dis_base();
    }

  //Pour compilation
  return get_ref_zone_dis_base();

}

const Motcle Champ_Generique_Interpolation::get_directive_pour_discr() const
{
  Motcle directive;

  if (localisation_=="elem")
    {
      directive = "champ_elem";
    }
  else if (localisation_=="som")
    {
      directive = "champ_sommets";
    }
  else if (localisation_=="faces")
    {
      directive = "champ_face";
    }
  else
    {
      Cerr<<"Localisation "<<localisation_<<" is not recognized by "<<que_suis_je()<<finl;
      exit();
    }

  return directive;
}

//Discretisation du domaine d interpolation si celui-ci n est pas le domaine de calcul
//Cette discretisation est necessaire pour associer une zone discretisee a l espace de
//stockage dans la methode get_champ()
void Champ_Generique_Interpolation::discretiser_domaine()
{
  if (domaine_.non_nul())
    {
      le_dom_dis.associer_domaine(domaine_.valeur());
      const Probleme_base& Pb = get_ref_pb_base();
      const Discretisation_base& discr = Pb.discretisation();
      const Nom& type_discr = discr.que_suis_je();
      Nom type;
      // on ne cree pas les faces sauf si on veut une interpolation aux faces
      type = "NO_FACE_Zone_";
      if (localisation_=="faces")
        {
          type="Zone_";
          // On verifie que la localisation aux faces est possible sur le domaine (sinon elem ou som)
          if (domaine_.valeur().zone(0).type_elem().valeur().nb_som_face()<=2 && dimension==3)
            {
              Cerr << "'localisation faces' is not possible in 3D on the 2D surface mesh " << domaine_.le_nom() << " with the field: " << get_property("NOM")[0] << finl;
              Cerr << "Please use 'localisation elem' instead." << finl;
              exit();
            }
        }
      type += type_discr;
      le_dom_dis.discretiser(type);
    }
}
