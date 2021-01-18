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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Champ_Generique_Transformation.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/patch_168/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Generique_Transformation.h>
#include <Probleme_base.h>
#include <Zone_VF.h>
#include <DoubleTabs.h>
#include <Champ_Fonc.h>
#include <Champ_Inc.h>
#include <Discretisation_base.h>
#include <Interprete.h>
#include <Equation_base.h>
#include <Entree_complete.h>
#include <Milieu_base.h>
#include <Postraitement.h>
#include <Synonyme_info.h>
#include <Param.h>

#include <Linear_algebra_tools_impl.h>
#include <MD_Vector.h>
#include <MD_Vector_composite.h>
#include <Zone_dis_base.h>

Implemente_instanciable(Champ_Generique_Transformation,"Transformation",Champ_Gen_de_Champs_Gen);
Add_synonym(Champ_Generique_Transformation,"Champ_Post_Transformation");

Sortie& Champ_Generique_Transformation::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Champ_Generique_Transformation::readOn(Entree& s )
{
  Champ_Gen_de_Champs_Gen::readOn(s);
  verifier_coherence_donnees();
  return s ;
}

//-methode : type de transformation a realiser
// methodes possible :
//        formule : fonction de champs generiques sources, x, y, z et t
//        vecteur : construction d un vecteur a partir des donnees fournies
//        produit_scalaire : produit scalaire de deux vecteurs specifies
//        norme : norme d un vecteur specifie
//        composante : creation d un champ scalaire a partir d une composante d un champ vectoriel
//-expression : expression de la transformation (cas des methodes formule et vecteur)
//-numero : numero de la composante a extraire (uniquement pour le cas de la methode composante)
//-localisation : localisation du champ de stockage
void Champ_Generique_Transformation::set_param(Param& param)
{
  Champ_Gen_de_Champs_Gen::set_param(param);
  param.ajouter("methode",&methode_,Param::REQUIRED);
  param.ajouter_non_std("expression",(this));
  param.ajouter_non_std("numero",(this));
  param.ajouter("localisation",&localisation_);
}

int Champ_Generique_Transformation::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="expression")
    {
      int dim;
      is >> dim;
      les_fct.dimensionner(dim);
      if ((Motcle(methode_) == "formule") && (dim!=1) )
        {
          Cerr << "The integer N of the expression with FORMULE method must be equal to 1." <<finl;
          Cerr << "Use the VECTEUR expression method for N="<<dim<<finl;
          exit();
        }
      for (int i=0; i<dim; i++)
        is>>les_fct[i];
      return 1;
    }
  else if (mot=="numero")
    {
      int num_compo;
      is >> num_compo;
      nb_comp_ = 1;
      nature_ch = scalaire;
      /*     test numero composante : on ne peut pas se baser sur la dimension de l'espace dans le cas d'un champ multi_scalaire
             Probleme : c'est nb_comp_ qui portera par la suite la dimension des composantes et il est ici initialise a 1 pour eviter bug
             Le test est deplace dans Champ_Generique_Transformation::get_champ

             if ((num_compo<0) || (num_compo>dimension-1))
             {
             //   Cerr<<"Component number must be >=0 and < "<<dimension<<" for composante option."<<finl;
             Cerr<<"Component number must be >=0 for composante option."<<finl;
             exit();
             } */
      les_fct.dimensionner(num_compo+1);
      return 1;
    }
  else
    return Champ_Gen_de_Champs_Gen::lire_motcle_non_standard(mot,is);
  return 1;
}

void Champ_Generique_Transformation::verifier_coherence_donnees()
{
  if ((Motcle(methode_) == "produit_scalaire") || (Motcle(methode_) == "norme"))
    les_fct.dimensionner(1);
  else if ((Motcle(methode_) == "formule") || (Motcle(methode_) == "vecteur"))
    {
      if (les_fct.size()<1)
        {
          Cerr<<"The expression must be read for the methods fonction and vecteur."<<finl;
          exit();
        }
    }
  else
    {
      Motcles methods(6);
      methods[0] = "formule";
      methods[1] = "vecteur";
      methods[2] = "produit_scalaire";
      methods[3] = "norme";
      methods[4] = "composante";
      methods[5] = "composante_normale";
      if (methods.search(methode_) < 0)
        {
          Cerr<<"Error in  "<<que_suis_je()<<finl;
          Cerr<<"Read keyword : " << methode_ << "\n Allowed items are : "<<methods<<finl;
          exit();
        }
    }

  if ((Motcle(methode_) == "vecteur"))
    {
      // On essaie de fixer de suite le nombre de composantes avec vecteur
      nb_comp_ = les_fct.size();

      // la verification des sources est faite lors du completer pour les sources reference
    }
}
void Champ_Generique_Transformation::verifier_localisation()
{
  Motcles localisations(4);
  localisations[0] = "elem";
  localisations[1] = "som";
  localisations[2] = "faces";
  localisations[3] = "elem_som";

  if (localisations.search(localisation_) < 0)
    {
      Cerr << "Error in "<<que_suis_je()<<finl;
      Cerr << "Read keyword : " << localisation_ << "\n Allowed items are " << localisations << finl;
      exit();
    }
}
void Champ_Generique_Transformation::completer(const Postraitement_base& post)
{

  bool sources_location_ok = true;
  bool fictive_source = false;

  int nb_source_fictive=0;
  int nb_sources = get_nb_sources();
  if (nb_sources==0)
    {
      nb_source_fictive = 1;
      Champ_Generique& new_src = get_set_sources().add(Champ_Generique());
      Nom nom_probleme = ref_cast(Postraitement,post).probleme().le_nom();
      const Objet_U& ob = interprete().objet(nom_probleme);
      const Probleme_base& pb = ref_cast(Probleme_base,ob);
      const Nom& nom_champ = pb.equation(0).inconnue().le_nom();
      Entree s_bidon;
      Nom ajout("");
      ajout += " Champ_Post_refChamp { Pb_champ ";
      ajout += pb.le_nom();
      ajout += " ";
      ajout += nom_champ;
      ajout += " }";
      Entree_complete s_complete(ajout,s_bidon);
      s_complete>>new_src;
      nb_sources = get_nb_sources();
    }

  Champ_Gen_de_Champs_Gen::completer(post);

  if (nb_source_fictive==1)
    {
      if (get_nb_sources()>1)
        {
          assert( get_set_sources().size( ) == 1 );
          // on a rajoute une source pour rien ...
          get_set_sources().vide();
          nb_source_fictive = 0;
        }
    }

  nb_sources = get_nb_sources();

  //On determine le nombre de composantes qui sera attribue
  //a l espace de stockage rendu par ce champ de postraitement en
  //recherchant la source qui possede le plus grand nombre de composantes

  nb_comp_ = 1;
  nature_ch = scalaire;

  if (nb_source_fictive==0)
    {
      for (int i=0; i<nb_sources; i++)
        {
          Champ source_espace_stockage;
          const Champ_base& source = get_source(i).get_champ_without_evaluation(source_espace_stockage);

          if ((Motcle(methode_) == "vecteur"))
            {

              if (source.nature_du_champ()!=scalaire)
                {
                  Cerr<<que_suis_je()<<" The source fields must be of scalar nature for option vecteur."<<finl;
                  exit();
                }
            }
          int nb_comp = source.nb_comp();
          nb_comp_ = (nb_comp_<nb_comp)?  nb_comp:nb_comp_;

          if (source.nature_du_champ()==vectoriel)
            nature_ch = vectoriel;
          else if (source.nature_du_champ()==multi_scalaire)
            nature_ch = multi_scalaire;
        }
    }
  //Si aucun champ source n a ete specifie on en ajoute une
  //pour donner acces a la zone discretisee ...

  Noms sources_location;

  if( nb_source_fictive > 0 )
    {
      //special treatment when we have fictives sources
      //the user needs to provide a location
      if( localisation_ == "??" )
        {
          // On prend la localisation de l'inconnue


          Cerr << "Error in Champ_Generique_Transformation::completer "<<finl;
          Cerr << "No sources were specified. So you need to specify at least the location."<<finl;
          Cerr << "Aborting..."<<finl;
          Cerr << "ToDo for developer: guess location by taking the pb.equation(0).inconnue() localization." << finl;
          Process::abort( );
        }
      else
        {
          fictive_source = true;
        }
    }
  else
    {

      //real sources
      for( int i=0; i<nb_sources; i++)
        {
          //loop over sources to check that all sources have the same location
          bool use_directive = false;
          Champ my_field;

          const Champ_base& source_i = get_source( i ).get_champ_without_evaluation( my_field );
          if( source_i.a_une_zone_dis_base( ) )
            {
              const Zone_dis_base& zone_source_i = source_i.zone_dis_base( );
              if( ! sub_type( Zone_VF , zone_source_i ) )
                {
                  Cerr << "Warning in Champ_Generique_Transformation::completer "<<finl;
                  Cerr << "The source number "<<i<<" has a zone_dis_base but not of type Zone_VF so use directive to determine the location"<<finl;
                  use_directive=true;
                }
              if( ! use_directive )
                {
                  const DoubleTab& values_source_i = source_i.valeurs( );
                  const Zone_VF& zvf_source_i = ref_cast( Zone_VF, zone_source_i );


                  MD_Vector md;
                  md = values_source_i.get_md_vector( );

                  //composite case, in particular Champ_{P0,Face}_PolyMAC...
                  if (zvf_source_i.que_suis_je().finit_par("MAC") && sub_type( MD_Vector_composite, md.valeur( )))
                    {
                      const MD_Vector& md0 = ref_cast(MD_Vector_composite, md.valeur()).get_desc_part(0);
                      if (md0 == zvf_source_i.zone( ).les_elems().get_md_vector( ))
                        sources_location.add( "elem" );
                      else if (md0 == zvf_source_i.face_sommets( ).get_md_vector( ))
                        sources_location.add("faces");
                    }
                  else if( ( sub_type( MD_Vector_composite, md.valeur( ) ) ) && (localisation_=="??"))
                    {
                      Cerr << "Error in Champ_Generique_Transformation::completer "<<finl;
                      Cerr << "The source number "<<i<<" is composite and the location was not provided. It is forbidden to apply a 'transformation' on a composite source without providing a location. "<<finl;
                      Cerr << "You must perform an interpolation before or specify a location."<<finl;
                      Cerr << "Aborting..."<<finl;
                      Process::abort( );
                      // if we want to deal with composite case we can do something like...
                      // MD_Vector_composite md_comp = ref_cast( MD_Vector_composite, values_source_i.get_md_vector( ).valeur( ) );
                      // nb_parts = md_comp.nb_parts( ); //to perform loop over parts and then get the considered part :
                      // md = md_comp.get_desc_part( part );
                      // when dealing with pression_pa it leads to have elem and som locations
                    }
                  else if ( md == zvf_source_i.face_sommets( ).get_md_vector( ) )
                    {
                      sources_location.add( "faces" );
                    }
                  else if( md == zvf_source_i.zone( ).les_elems( ).get_md_vector( ) )
                    {
                      sources_location.add( "elem" );
                    }
                  else if( md == zvf_source_i.zone( ).domaine( ).les_sommets( ).get_md_vector( ) )
                    {
                      sources_location.add( "som" );
                    }
                  else if( md == zvf_source_i.xa( ).get_md_vector( ) )
                    {
                      sources_location.add( "xa" );
                    }
                  else
                    {
                      Cerr << "Warning in Champ_Generique_Transformation::completer "<<finl;
                      Cerr << "Impossible to determine the localisation of the source number "<<i<<" using md_vectors so try using directive"<<finl;
                      use_directive = true;
                    }
                  // Cout << "sources location "<<i<<" : "<<sources_location( i )<<finl;
                }//end of not using directive case
            }//end of has zone dis
          else
            {
              //in the case where there is no zone dis base, we look at the directive
              use_directive=true;
            }

          if( use_directive )
            {
              Motcle directive = get_source( i ).get_directive_pour_discr( );
              if (directive=="champ_elem")
                sources_location.add( "elem" );
              else if (directive=="champ_sommets")
                sources_location.add( "som" );
              else if (directive=="champ_face")
                sources_location.add( "faces" );
              else if (directive=="pression")
                {
                  if  (localisation_=="??")
                    {
                      Cerr << "Error in Champ_Generique_Transformation::completer"<<finl;
                      Cerr << "The directive associated to the source number "<<i<<" is pressure and the location was not provided.";
                      Cerr <<" It is forbidden to apply a 'transformation' on a composite source without providing a location. "<<finl;
                      Cerr << "You must perform an interpolation before or specify a location." <<finl;
                      Cerr << "Aborting..."<<finl;
                      Process::abort( );
                    }
                  sources_location.add( "unknown" );
                }
              else if ( directive == "champ_uniforme" )
                sources_location.add( "elem" );
              else
                {
                  Cerr<<"Error in Champ_Generique_Transformation::completer"<<finl;
                  Cerr<<"The discretization of the first source of the generic field "<<nom_post_<<finl;
                  Cerr<<"does not correspond to any TRUST field discretization."<<finl;
                  Cerr<<directive<<finl;
                  Cerr<<"Aborting..."<<finl;
                  Process::abort( );
                }
            }
        }//loop over sources
      const int nb_locations = sources_location.size( );
      if( nb_sources != nb_locations )
        {
          Cerr << "Error in Champ_Generique_Transformation::completer "<<finl;
          Cerr << nb_sources<<" were detected but "<<nb_locations<<" were found"<<finl;
          Cerr << "Aborting..."<<finl;
          Process::abort( );
        }
      Nom reference_location = sources_location( 0 );
      for(int i=1; i<nb_locations; i++)
        {
          if( sources_location( i ) != reference_location )
            {
              sources_location_ok = false;
              Cerr << "Warning in Champ_Generique_Transformation::completer "<<finl;
              Cerr << "All sources have location : "<<finl;
              Cerr <<sources_location;
              Cerr <<"But the specified location is "<<localisation_<<finl;
              //Cerr << "Aborting..."<<finl;
              //Process::abort( );
            }
        }
    } //end dealing with nb_source_fictive <= 0

  if( localisation_ == "??" )
    {
      if( sources_location_ok  )
        {
          Cerr <<"sources have same location : "<< sources_location( 0 )<<finl;
        }
      else
        {
          Cerr << "Warning sources have not the same location, taking "<<sources_location( 0 )<<" as location"<<finl;
        }
      localisation_ = sources_location( 0 ) ;
    }
  else
    {
      if( ! fictive_source && localisation_ != sources_location( 0 ) )
        {
          Cerr << "Warning first source is located to "<<sources_location( 0 )<<" but the user has specified "<< localisation_<<finl;
        }
    }

  verifier_localisation();
  preparer_macro();

  if (Motcle(methode_)=="formule")
    {
      // on redeande le nb_sources car on a ajoute des sources par completer
      nb_sources = get_nb_sources();
      fxyz.dimensionner(1);
      fxyz[0].setNbVar(4+nb_sources);
      fxyz[0].setString(les_fct[0]);
      fxyz[0].addVar("x");
      fxyz[0].addVar("y");
      fxyz[0].addVar("z");
      fxyz[0].addVar("t");
      for (int i=0; i<nb_sources; i++)
        {
          const Noms nom = get_source(i).get_property("nom");
          fxyz[0].addVar(nom[0]);
          Journal()<<" Name of source "<<nom[0]<<finl;
        }
      fxyz[0].parseString();
    }
}


void  projette(DoubleTab& valeurs_espace,const DoubleTab& val_source,const Zone_VF& zvf,const Motcle& loc,int is_VDF)
{
  const Nom& type_elem=zvf.zone().type_elem().valeur().que_suis_je();
  if ((is_VDF)||(loc!="elem")|| (type_elem!="Quadrangle"))
    {
      Cerr<<"option composante_normale not coded in this case"<<finl;
      Process::exit();
    }
  const DoubleTab& coord=zvf.zone().domaine().coord_sommets();
  const IntTab& elems=zvf.zone().les_elems();

  int nb_v= valeurs_espace.dimension(0);
  assert(nb_v==zvf.nb_elem());
  assert(val_source.dimension(1)==Objet_U::dimension);
  for (int i=0; i<nb_v; i++)
    {
      Vecteur3 v1,v2,normal,val;
      int s0=elems(i,0);
      int s1=elems(i,1);
      int s2=elems(i,2);
      if (Objet_U::dimension!=3)
        {
          Cerr<<"not implemented "<<finl;
          assert(0);
          Process::exit();
        }
      v1.set(coord(s1,0)-coord(s0,0),coord(s1,1)-coord(s0,1),coord(s1,2)-coord(s0,2));
      v2.set(coord(s2,0)-coord(s0,0),coord(s2,1)-coord(s0,1),coord(s2,2)-coord(s0,2));
      val.set(val_source(i,0),val_source(i,1),val_source(i,2));

      Vecteur3::produit_vectoriel(v1,v2,normal);
      double norm=Vecteur3::produit_scalaire(normal,normal);
      double inv=1./sqrt(norm);
      normal*=inv;


      double prod_scal= Vecteur3::produit_scalaire(normal,val);
      valeurs_espace(i)=prod_scal;
    }

}
//Construction de l espace de stockage rendu par ce champ de postraitement
//-Caracterisation de l espace de stockage rendu par ce champ de postraitement (Typage ...)
//-Construction du tableau positions qui contient les coordonnees des points de calcul (support)
//-Interpolation des valeurs des sources sur le support retenu
//-Evaluation des valeurs de l espace de stockage en fonction de la fonction (les_fct)

const Champ_base& Champ_Generique_Transformation::get_champ_without_evaluation(Champ& espace_stockage) const
{
  Champ_Fonc es_tmp;

  espace_stockage = creer_espace_stockage(nature_ch,nb_comp_,es_tmp);
  return espace_stockage.valeur();
}
const Champ_base& Champ_Generique_Transformation::get_champ(Champ& espace_stockage) const
{
  const Zone_dis_base& zone_dis = get_ref_zone_dis_base();
  Champ_Fonc es_tmp;

  espace_stockage = creer_espace_stockage(nature_ch,nb_comp_,es_tmp);
  DoubleTab& valeurs_espace = espace_stockage->valeurs();
  //Construction du tableau positions qui contient les coordonnees
  //des points (support) ou vont etre evaluees les valeurs de l espace
  //de stockage
  const Zone_VF& zvf = ref_cast(Zone_VF,zone_dis);
  DoubleTab positions;
  int nb_pos;
  int nb_elem_tot = zvf.nb_elem_tot();
  int nb_som_tot = get_ref_domain().nb_som_tot();
  const Motcle directive = get_directive_pour_discr();
  int is_VDF = 0;
  if (zvf.que_suis_je().debute_par("Zone_VDF"))
    is_VDF = 1;
  if (localisation_ == "elem")
    {
      if (zvf.xp().nb_dim() != 2) /* xp() non initialise */
        {
          zvf.zone().calculer_centres_gravite(positions);
        }
      else
        {
          positions.resize(zvf.nb_elem(), zvf.xp().dimension(1));
          memcpy(positions.addr(), zvf.xp().addr(), positions.size() * sizeof(double));
        }
    }
  else if (localisation_ == "som")
    positions = get_ref_domain().coord_sommets();
  else if (localisation_ == "faces")
    positions = zvf.xv();
  else if (localisation_ == "elem_som")
    {
      const DoubleTab& som = get_ref_domain().coord_sommets();
      int dim = som.dimension(1);
      positions = zvf.xp();
      positions.resize(nb_elem_tot+nb_som_tot,dim);
      for (int i1=nb_elem_tot; i1<nb_elem_tot+nb_som_tot; i1++)
        for (int i2=0; i2<dim; i2++)
          positions(i1,i2) = som(i1-nb_elem_tot,i2);
    }
  else
    {
      Cerr<<"Problem in " <<que_suis_je()<<finl;
      Cerr<<"Item "<<localisation_<<" is not recognized."<<finl;
      exit();
    }

  //Interpolation des valeurs des sources sur le support retenu et stockage dans sources_val

  int nb_sources = get_nb_sources();
  nb_pos = positions.dimension(0);
  assert(nb_pos>0||(methode_=="formule")||(methode_=="composante_normale")||(methode_=="vecteur"));
  double x=0;
  double y=0;
  double z=0;
  DoubleTabs sources_val(nb_sources);
  IntVect nb_comps(nb_sources);
  Noms nom_source(nb_sources);
  int dim_compo = 2*dimension;
  Noms compo(dim_compo);
  double temps;
  temps = get_time();
  int nb_compso = 0; // deplacement du parametre pour qu'il soit reconnu par l'ensemble des methodes en vue test (voir methode composante ci-dessous)
  for (int so=0; so<nb_sources; so++)
    {
      const Champ_Generique_base& source = get_source(so);
      Champ stockage_so;
      const Champ_base& source_so = source.get_champ(stockage_so);
      const DoubleTab& source_so_val = source_so.valeurs();
      const Motcle directive_so = source.get_directive_pour_discr();

      //int nb_compso = source_so.nb_comp();
      nb_compso = source_so.nb_comp();
      nb_comps(so) = nb_compso;
      const Noms nom_champ = source.get_property("nom");
      nom_source[so] = nom_champ[0];
      if ((Motcle(methode_)=="produit_scalaire") || (Motcle(methode_)=="norme"))
        {
          const Noms compo_ch = source.get_property("composantes");
          for (int comp=0; comp<dimension; comp++)
            compo[so*dimension+comp] = compo_ch[comp];
        }

      //Si VDF et traitement d un produit scalaire ou d une norme alors on interpole
      //pour recuperer un tableau avec autant de composantes que la dimension du probleme

      if (directive!=directive_so)
        {
          sources_val[so].resize(nb_pos,nb_compso);
          if (directive == "CHAMP_FACE") source_so.valeur_aux_faces(sources_val[so]);
          else source_so.valeur_aux(positions,sources_val[so]);
        }
      else
        {
          if (source_so_val.nb_dim()==1)
            {
              int nn=source_so_val.dimension_tot(0);
              sources_val[so].resize(nn,1);
              for (int i_loc=0; i_loc<nn; i_loc++)
                sources_val[so](i_loc,0) = source_so_val(i_loc);
            }
          else
            sources_val[so] = source_so_val;
        }

      if ((nb_pos==0)&&(valeurs_espace.dimension_tot(0)!=0))
        {
          int nb_pos_tmp=valeurs_espace.dimension(0);
          if (nb_pos_tmp > sources_val[so].size())
            {
              Cerr << "\nError in Champ_Generique_Transformation::get_champ" << finl;
              Cerr << nom_source(so) << " : Wrong number of elements." << finl;
              exit();
            }
        }
    }
  //Fin du remplissage de sources_val

  //Calcul des valeurs de l espace de stockage en fonction de methode_ selectionne
  if ((Motcle(methode_)=="produit_scalaire") || (Motcle(methode_)=="norme"))
    {
      Parser_U& f = fxyz[0];
      for (int i=0; i<nb_pos; i++)
        {
          for (int so=0; so<nb_sources; so++)
            {
              const DoubleTab& source_so_val = sources_val[so];
              for (int j=0; j<dimension; j++)
                f.setVar(so*dimension+j,source_so_val(i,j));
              // Optimisation plus rapide que:
              //f.setVar(compo[so*dimension+j],source_so_val(i,j));
            }
          valeurs_espace(i) = f.eval();
        }
    }
  else if (Motcle(methode_)=="vecteur")
    {
      for (int j=0; j<nb_comp_; j++)
        fxyz[j].setVar("t",temps);

      if ((is_VDF) && (localisation_=="faces"))
        {
          const IntVect& orientation = zvf.orientation();
          int ori;

          for (int i=0; i<nb_pos; i++)
            {
              x = positions(i,0);
              y = positions(i,1);
              z = 0;
              if (dimension>2)
                z = positions(i,2);
              ori = orientation(i);

              for (int j=0; j<nb_comp_; j++)
                {
                  if (ori==j)
                    {
                      fxyz[j].setVar(0,x);
                      fxyz[j].setVar(1,y);
                      fxyz[j].setVar(2,z);
                      for (int so=0; so<nb_sources; so++)
                        {
                          const DoubleTab& source_so_val = sources_val[so];
                          fxyz[j].setVar(so+4,source_so_val(i,0));
                        }
                      valeurs_espace(i) = fxyz[j].eval();
                    }
                }
            }
        }
      else
        {
          for (int i=0; i<nb_pos; i++)
            {
              x = positions(i,0);
              y = positions(i,1);
              z = 0;
              if (dimension>2)
                z = positions(i,2);

              for (int j=0; j<nb_comp_; j++)
                {
                  fxyz[j].setVar(0,x);
                  fxyz[j].setVar(1,y);
                  fxyz[j].setVar(2,z);
                  for (int so=0; so<nb_sources; so++)
                    {
                      const DoubleTab& source_so_val = sources_val[so];
                      fxyz[j].setVar(so+4,source_so_val(i,0));
                    }
                  valeurs_espace(i,j) = fxyz[j].eval();
                }
            }
        }
    }
  else if (Motcle(methode_)=="composante_normale")
    {
      const DoubleTab& source_so_val = sources_val[0];
      projette(valeurs_espace,source_so_val,zvf,localisation_,is_VDF);
    }
  else if (Motcle(methode_)=="composante")
    {
      int num_compo = les_fct.size()-1;
      if ((num_compo<0)||(num_compo>=nb_compso))
        {
          Cerr<<"Component number must be >=0 and < "<<nb_compso<<" for composante option."<<finl;
          exit();
        }

      const DoubleTab& source_so_val = sources_val[0];
      if ((is_VDF) && (localisation_=="faces"))
        {
          const IntVect& orientation = zvf.orientation();
          int ori;
          for (int i=0; i<nb_pos; i++)
            {
              ori = orientation(i);
              if (ori==num_compo)
                valeurs_espace(i) = source_so_val(i,0);
              else
                valeurs_espace(i) = 0.;
            }
        }
      else
        {
          for (int i=0; i<nb_pos; i++)
            valeurs_espace(i) = source_so_val(i,num_compo);
        }
    }
  else if (Motcle(methode_)=="formule")
    {
      //Evaluation des valeurs de l espace de stockage en fonction de la fonction (les_fct)
      Parser_U& f = fxyz[0];
      f.setVar("t",temps);
      int special=0;
      if ((nb_pos==0)&&(valeurs_espace.dimension_tot(0)!=0))
        {
          special=1;
          nb_pos=valeurs_espace.dimension(0);
          x=1e38;
          y=1e38;
          z=1e38;
          f.setVar(0,x);
          f.setVar(1,y);
          f.setVar(2,z);
        }

      for (int i=0; i<nb_pos; i++)
        {
          if (!special)
            {
              x = positions(i,0);
              y = positions(i,1);
              z = 0;
              if (dimension>2)
                z = positions(i,2);
              f.setVar(0,x);
              f.setVar(1,y);
              f.setVar(2,z);
            }
          /* Plus rapide que de passer par:
             f.setVar("x",x);
             f.setVar("y",y);
             f.setVar("z",z);
          */
          if (valeurs_espace.nb_dim() == 1)
            {
              for (int so=0; so<nb_sources; so++)
                f.setVar(so+4,sources_val[so](i,0));
              valeurs_espace(i) = f.eval();
            }
          else // nb_dim > 1
            {
              for (int j=0; j<nb_comp_; j++)
                {
                  for (int so=0; so<nb_sources; so++)
                    {
                      int nbcomp_loc = nb_comps(so);
                      if (nbcomp_loc == nb_comp_)
                        f.setVar(so+4,sources_val[so](i,j));
                      else if (nbcomp_loc == 1)
                        f.setVar(so+4,sources_val[so](i,0));
                      /* Optimisation plus rapide que de passer par:
                         if (nbcomp_loc == nb_comp_)
                         f.setVar(nom_source[so],sources_val[so](i,j));
                         else if (nbcomp_loc == 1)
                         f.setVar(nom_source[so],sources_val[so](i,0)); */
                      else
                        {
                          Cerr << "The arrays of values don't have compatibles dimensions in "<<que_suis_je()<< finl;
                          exit();
                        }
                    }
                  valeurs_espace(i,j) = f.eval();
                }
            }
        }
    }
  // PL: Suppression d'une synchronisation couteuse tres souvent inutile
  // Voir Champ_Generique_Interpolation (localisation = som) pour le report de l'echange_espace_virtuel
  // valeurs_espace.echange_espace_virtuel();
  return espace_stockage.valeur();
}

//Les localisations elem som faces peuvent etre retenues pour le postraitement
//La localisation elem_som sert uniquement pour l evaluation des valeurs de l espace
//de stockage mais ne peut pas etre retenue comme localisation de postraitement

Entity Champ_Generique_Transformation::get_localisation(const int index) const
{
  Entity loc;
  if (localisation_=="elem")
    loc = ELEMENT;
  else if (localisation_=="som")
    loc = NODE;
  else if (localisation_=="faces")
    loc = FACE;
  else
    {
      Cerr << "Error of type : localisation should be specified to elem or som or faces for the defined field " << nom_post_ << finl;
      throw Champ_Generique_erreur("INVALID");
    }
  return loc;
}

const Noms Champ_Generique_Transformation::get_property(const Motcle& query) const
{
  Motcles motcles(2);
  motcles[0] = "composantes";
  motcles[1] = "unites";
  int rang = motcles.search(query);
  switch(rang)
    {
    case 0:
      {
        Noms compo(nb_comp_);
        for (int i=0; i<nb_comp_; i++)
          {
            Nom nume(i);
            compo[i] = nom_post_+nume;
          }
        return compo;
        break;
      }
    case 1:
      {
        Noms unites(nb_comp_);
        for (int comp=0; comp<nb_comp_; comp++)
          unites[comp] = "bidon";
        return unites;
        break;
      }
    }
  return Champ_Gen_de_Champs_Gen::get_property(query);
}

const Motcle Champ_Generique_Transformation::get_directive_pour_discr() const
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
  else if (localisation_=="elem_som")
    {
      directive = "pression";
    }
  else
    {
      Cerr<<"Localisation "<<localisation_<<" is not recognized by "<<que_suis_je()<<finl;
      exit();
    }
  return directive;
}

//Nomme le champ en tant que source par defaut
//"Combinaison_"+"les_fct"
void Champ_Generique_Transformation::nommer_source()
{
  if (nom_post_=="??")
    {
      Nom nom_post_source;
      nom_post_source =  "Combinaison_";
      nom_post_source += les_fct[0];
      nommer(nom_post_source);
    }
}

//Renvoie le nombre de composantes a manipuler dans l expression
//Verifie la coherence des donnees pour la methode choisie
int Champ_Generique_Transformation::preparer_macro()
{
  int nb_sources = get_nb_sources();
  int nb_var = 0;
  int erreur = 0;
  Nom msg;

  if ((Motcle(methode_)=="produit_scalaire") || (Motcle(methode_)=="norme"))
    {
      for (int i=0; i<nb_sources; i++)
        {
          Champ source_espace_stockage;
          const Champ_base& source = get_source(i).get_champ(source_espace_stockage);
          int nb_comp = source.nb_comp();
          if (source.nature_du_champ()!=vectoriel)
            {
              msg = "At least one of the source fields is not of vector nature.";
              erreur = 1;
            }
          nb_var += nb_comp;
        }

      if ((Motcle(methode_)=="produit_scalaire") && (nb_sources!=2))
        {
          msg = "Exactly two vector source fields must be specified.";
          erreur = 1;
        }
      else if ((Motcle(methode_)=="norme") && (nb_sources!=1))
        {
          msg = "Exactly one vector source field must be specified.";
          erreur = 1;
        }
      // la suite est faite si on n a pas d'erreur
      if ( erreur==0 )
        {
          nb_comp_ = 1;
          nature_ch = scalaire;
          // L'appel a creer_expression_macro est fait si on a un vecteur car elle fait appel aux composantes !
          creer_expression_macro();
          fxyz.dimensionner(1);
          fxyz[0].setNbVar(nb_var);
          fxyz[0].setString(les_fct[0]);
          for (int i=0; i<nb_sources; i++)
            {
              Champ source_espace_stockage;
              const Champ_base& source = get_source(i).get_champ(source_espace_stockage);
              int nb_comp = source.nb_comp();
              const Noms compo = get_source(i).get_property("composantes");
              for (int comp=0; comp<nb_comp; comp++)
                {
                  fxyz[0].addVar(compo[comp]);
                  Journal()<<" Source compo "<<compo[comp]<<finl;
                }
            }
          fxyz[0].parseString();
        }
    }
  else if (Motcle(methode_)=="vecteur")
    {
      nb_comp_ = les_fct.size();
      nature_ch = vectoriel;
      fxyz.dimensionner(nb_comp_);

      for (int comp=0; comp<nb_comp_; comp++)
        {
          fxyz[comp].setNbVar(4+nb_sources);
          fxyz[comp].setString(les_fct[comp]);
          fxyz[comp].addVar("x");
          fxyz[comp].addVar("y");
          fxyz[comp].addVar("z");
          fxyz[comp].addVar("t");
          for (int i=0; i<nb_sources; i++)
            {
              const Noms nom = get_source(i).get_property("nom");
              fxyz[comp].addVar(nom[0]);
              Journal()<<" Name of source "<<nom[0]<<finl;
            }
          fxyz[comp].parseString();
        }
    }
  else if ((Motcle(methode_)=="composante")|| ((Motcle(methode_)=="composante_normale") ))
    {
      if (nb_sources!=1)
        {
          msg = "Exactly one vector source field must be specified.";
          erreur = 1;
        }
      // La suite est faite si on n'a pas d'erreur !!
      if (erreur==0) // 1 source pas la peine de faire une boucle
        {
          Champ source_espace_stockage;
          const Champ_base& source = get_source(0).get_champ_without_evaluation(source_espace_stockage);
          if (source.nature_du_champ()!=vectoriel && source.nature_du_champ()!=multi_scalaire)
            {
              msg = "The source field is not of vector nature.";
              erreur = 1;
            }
        }
      nb_comp_ = 1;
      nature_ch = scalaire;
    }
  else if (Motcle(methode_)=="formule")
    nb_var = -1;

  if ((nature_ch==scalaire) && (localisation_!="elem"))
    {
      const Zone_dis_base& zone_dis = get_ref_zone_dis_base();
      if (zone_dis.que_suis_je().debute_par("Zone_VDF"))
        {
          msg = "The nature of the generated storing field will be scalar.\n";
          msg += "In that case the location elem is required since VDF discretization is considered.";
          erreur = 1;
        }
    }

  if (erreur)
    {
      Cerr<<"Please check the syntax of the Transformation generic field "<<nom_post_<<finl;
      Cerr<<"for wich the selected option is : "<<methode_<<finl;
      Cerr<<msg<<finl;
      exit();
    }
  return nb_var;
}

//Cree des expressions pour des operations specifiques :
//produit scalaire et norme
void Champ_Generique_Transformation::creer_expression_macro()
{
  if ((Motcle(methode_)=="produit_scalaire") || (Motcle(methode_)=="norme"))
    {
      const Noms compo0 = get_source(0).get_property("composantes");

      if (Motcle(methode_)=="norme")
        {
          les_fct[0] = "sqrt(";
          les_fct[0] += compo0[0];
          les_fct[0] += "*";
          les_fct[0] += compo0[0];
          les_fct[0] += "+";
          les_fct[0] += compo0[1];
          les_fct[0] += "*";
          les_fct[0] += compo0[1];
          if (dimension==3)
            {
              les_fct[0] += "+";
              les_fct[0] += compo0[2];
              les_fct[0] += "*";
              les_fct[0] += compo0[2];
            }
          les_fct[0] +=")";
        }

      if (Motcle(methode_)=="produit_scalaire")
        {
          const Noms compo1 = get_source(1).get_property("composantes");
          les_fct[0] = compo0[0];
          les_fct[0] += "*";
          les_fct[0] += compo1[0];
          les_fct[0] += "+";
          les_fct[0] += compo0[1];
          les_fct[0] += "*";
          les_fct[0] += compo1[1];
          if (dimension==3)
            {
              les_fct[0] += "+";
              les_fct[0] += compo0[2];
              les_fct[0] += "*";
              les_fct[0] += compo1[2];
            }
        }
    }
}
