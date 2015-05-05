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
// File:        Champ_Generique_Extraction.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Generique_Extraction.h>
#include <Champ_Generique_refChamp.h>
#include <Zone_VF.h>
#include <Postraitement.h>
#include <Discretisation_base.h>
#include <Deriv_Zone.h>
#include <Equation_base.h>
#include <Champ_front_uniforme.h>
#include <Dirichlet_homogene.h>
#include <NettoieNoeuds.h>
#include <Synonyme_info.h>
#include <Param.h>
#include <Champ_Generique_Interpolation.h>

Implemente_instanciable_sans_constructeur(Champ_Generique_Extraction,"Extraction",Champ_Gen_de_Champs_Gen);
Add_synonym(Champ_Generique_Extraction,"Champ_Post_Extraction");

// Description: voir reset()
Champ_Generique_Extraction::Champ_Generique_Extraction()
{
  // valeurs par defaut
  reset();
  methode_="trace";
}

//cf Champ_Gen_de_Champs_Gen::readOn
Entree& Champ_Generique_Extraction::readOn(Entree& is)
{
  Champ_Gen_de_Champs_Gen::readOn(is);
  // Verifications:
  if (get_nb_sources() != 1)
    {
      Cerr << "Error in Champ_Generique_Extraction::readOn\n"
           << " You must specify exactly 1 \"Source\"." << finl;
    }
  return is;
}

// Description: appel invalide
Sortie& Champ_Generique_Extraction::printOn(Sortie& os) const
{
  exit();
  return os;
}

//  domaine           : lecture du nom du domaine d extraction pour initialiser la reference domaine_
//  nom_frontiere : lecture du nom de la frontiere sur laquelle on veut faire l extraction
//  methode           : specification de la methode choisie pour extraire les valeurs
//                    (optionnel : trace par defaut ou champ_frontiere)
void Champ_Generique_Extraction::set_param(Param& param)
{
  Champ_Gen_de_Champs_Gen::set_param(param);
  param.ajouter("domaine",&dom_extrac_,Param::REQUIRED);
  param.ajouter("nom_frontiere",&nom_fr_,Param::REQUIRED);
  param.ajouter("methode",&methode_);
}

//Renvoie la directive (champ_elem, champ_sommets, champ_face ou pression)
//pour lancer la discretisation de l espace de stockage rendu par
//la methode get_champ() du Champ_Generique qui a lance l appel de cette methode
const Motcle Champ_Generique_Extraction::get_directive_pour_discr() const
{
  Motcle directive;
  directive = "champ_elem";
  return directive;
}

// Description:  Extraction des valeurs d un champ (trace ou champ frontiere) sur un bord du domaine
// Precondition: L extraction est actuellement limitee aux champs inconnus du probleme
//                 Elle est realisee uniquement sur des frontieres planes
const Champ_base& Champ_Generique_Extraction::get_champ(Champ& espace_stockage) const
{

  Champ source_espace_stockage;
  const Champ_Generique_base& source = get_source(0);
  const Champ_base& source_stockage = source.get_champ(source_espace_stockage);

  const DoubleTab& source_valeurs = source_stockage.valeurs();
  const Zone_dis_base& zone_dis_source = source.get_ref_zone_dis_base();
  const Nature_du_champ& nature_source = source_stockage.nature_du_champ();

  //Le test suivant n est pas dans completer() car utilise equation() : la reference doit etre initialisee
  if (methode_=="champ_frontiere")
    {
      int num_bord =  zone_dis_source.rang_frontiere(nom_fr_);
      const Champ_Inc_base& source_inconnue = ref_cast(Champ_Inc_base,source_stockage);
      const Cond_lim& cl =  source_inconnue.equation().zone_Cl_dis().les_conditions_limites(num_bord);
      if (sub_type(Dirichlet_homogene,cl.valeur()))
        {
          Cerr<<"A boundary condition of type Dirichlet homogeneous does not use field boundary"<<finl;
          Cerr<<"The extraction of field "<<source_stockage.le_nom()<<" with the option champ_frontiere"<<finl;
          Cerr<<"is therefore not possible for this type of boundary condition"<<finl;
          Cerr<<"You can possibly remove the option champ_frontiere for postprocess the trace of the field"<<finl;
          exit();
        }
    }


  //Probleme pour discretisation de la zone du domaine d extraction (dimension)
  ////const Zone_dis_base& zone_dis = get_ref_zone_dis_base();

  int nb_comp_source = source_stockage.nb_comp();
  double temps = source.get_time();

  Nom type_source, type_espace_stockage, disc;
  int nb_comp = 0;
  int nb_ddl = 0;

  if (zone_dis_source.que_suis_je().debute_par("Zone_VDF"))
    disc = "VDF";
  else if (zone_dis_source.que_suis_je().debute_par("Zone_VEF"))
    disc = "VEF";
  else
    {
      Cerr<<"Champ_Generique_Extraction::get_champ()"<<finl;
      Cerr<<"We do not recognize the type of area discretized"<<zone_dis_source.que_suis_je()<<finl;
      exit();
    }

  type_espace_stockage = "Champ_Fonc_P0_";
  type_espace_stockage += disc;

  if (source_stockage.que_suis_je()=="Champ_Face")
    nb_comp = dimension;
  else
    nb_comp = nb_comp_source;

  //La trace rend systematiquement un tableau de valeurs aux faces de depart
  //et un champ frontiere discretise est dimensionne par rapport au nombre de faces
  //de la frontiere : dans les deux cas nb_ddl = nb_faces
  const Frontiere_dis_base& fr_dis = zone_dis_source.frontiere_dis(nom_fr_);
  const Frontiere& la_frontiere = fr_dis.frontiere();
  nb_ddl = la_frontiere.nb_faces();

  //La zone discretisee a associer n est actuellement pas disponible
  //On associe pas de zone_discretisee et on ne fixe pas nb_valeurs_nodales
  espace_stockage.typer(type_espace_stockage);
  ////espace_stockage->associer_zone_dis_base(zone_dis);
  espace_stockage->fixer_nb_comp(nb_comp);
  ////espace_stockage->fixer_nb_valeurs_nodales(nb_ddl);
  espace_stockage->fixer_nature_du_champ(nature_source);

  DoubleTab& espace_valeurs = espace_stockage->valeurs();
  int nb_dim_source = source_valeurs.nb_dim();
  espace_valeurs.resize(nb_ddl,nb_comp);

  if (methode_=="trace")                         //On prend la trace du champ sur un bord
    {
      source_stockage.trace(fr_dis,espace_valeurs,temps);
    }
  else if (methode_=="champ_frontiere")                //On recupere le champ frontiere
    {
      int num_bord =  zone_dis_source.rang_frontiere(nom_fr_);
      const Champ_Inc_base& source_inconnue = ref_cast(Champ_Inc_base,source_stockage);
      const Champ_front& champ_fr =  source_inconnue.equation().zone_Cl_dis().les_conditions_limites(num_bord)->champ_front();

      if (sub_type(Champ_front_uniforme,champ_fr.valeur()))
        {
          if (nb_dim_source==1)
            {
              double val_champ_fr =  champ_fr.valeur()(0,0);
              for (int i=0; i<nb_ddl; i++)
                {
                  espace_valeurs(i,0)= val_champ_fr;
                }
            }
          else
            {
              for (int j=0; j<nb_dim_source; j++)
                {
                  double val_champ_fr =  champ_fr.valeur()(0,j);
                  for (int i=0; i<nb_ddl; i++)
                    {
                      espace_valeurs(i,j) = val_champ_fr;
                    }
                }
            }
        }
      else
        espace_valeurs = champ_fr->valeurs();
    }

  //L espace de stockage n a pas actuellement d espace virtuel
  ////espace_valeurs.echange_espace_virtuel();

  return espace_stockage.valeur();
}

const Noms Champ_Generique_Extraction::get_property(const Motcle& query) const
{

  Motcles motcles(1);
  motcles[0] = "composantes";
  int rang = motcles.search(query);
  switch(rang)
    {

    case 0:
      {
        Noms source_compos = get_source(0).get_property("composantes");
        int nb_comp = source_compos.size();
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

  return Champ_Gen_de_Champs_Gen::get_property(query);

}
//Nomme le champ en tant que source par defaut
//"Extraction_" + nom_champ_source
void Champ_Generique_Extraction::nommer_source()
{
  if (nom_post_=="??")
    {
      Nom nom_post_source, nom_champ_source;
      const Noms nom = get_source(0).get_property("nom");
      nom_champ_source = nom[0];
      nom_post_source = "Extraction_";
      nom_post_source += nom_champ_source;
      nommer(nom_post_source);
    }
}

//Description :
// La methode completer complete le domaine d extraction declare dans le jeu de donnes
// auquel la classe fait reference:
// - ajout d une zone (de type Pave)
// - on determine les coordonnees des sommets contenus dans le plan d extraction
// Actuellement on ne procede pas a la discretisation de la zone du domaine d extraction (pb dimension)
void Champ_Generique_Extraction::completer(const Postraitement_base& post)
{
  Champ_Gen_de_Champs_Gen::completer(post);
  const Objet_U& ob = interprete().objet(dom_extrac_);
  if (!sub_type(Domaine, ob))
    {
      Cerr << "Error in  Champ_Generique_Extraction:get_champ"<<finl;
      Cerr<<"We do not retrieve domain"<<finl;
      exit();
    }
  domaine_ = ref_cast(Domaine,ob);
  const Champ_Generique_base& source = get_source(0);

  if (methode_=="champ_frontiere")
    {
      if (sub_type(Champ_Generique_refChamp,source))
        {
          Champ source_espace_stockage;
          const Champ_base& source_stockage = source.get_champ(source_espace_stockage);
          if (!sub_type(Champ_Inc_base,source_stockage))
            {
              Cerr<<"The method "<<methode_<<" can be applied to extract only unknown fields of the problem."<<finl;
              Cerr<<"The field to extract "<<source_stockage.le_nom()<<" do not satisfied this requirement."<<finl;
              exit();
            }
        }
      else
        {
          const Noms nom_champ = get_property("nom");
          Cerr<<"The method "<<methode_<<" can be applied to extract only unknown fields of the problem."<<finl;
          Cerr<<"The source of the post-processing field "<<nom_champ[0]<<" do not encapsulate an unknown field."<<finl;
          exit();
        }

    }

  const Domaine& dom = source.get_ref_domain();
  const Zone_dis_base& zone_dis_source = source.get_ref_zone_dis_base();
  const Zone_VF& zvf_source = ref_cast(Zone_VF,zone_dis_source);
  // Pour eviter un crash:
  if (zvf_source.nb_frontiere_dis()==0)
    {
      Cerr << "You can't extract a field onto a boundary of the domain " << dom.le_nom() << finl;
      Cerr << "because there is NO boundaries defined on it." << finl;
      if (sub_type(Champ_Generique_Interpolation,source))
        {
          Cerr << "It is not possible to extract on a boundary a field interpolated other than the calculation domain." << finl;
          Cerr << "So, change the domain in Interpolation definition into domain " << ref_cast(Champ_Generique_Interpolation,source).get_source(0).get_ref_domain().le_nom() <<  finl;
        }
      exit();
    }
  const Frontiere_dis_base& fr_dis = zone_dis_source.frontiere_dis(nom_fr_);
  const Frontiere& la_frontiere = fr_dis.frontiere();
  const Type_Face type_face_source = la_frontiere.faces().type_face();
  int nb_faces = la_frontiere.nb_faces();

  const DoubleTab& sommets_source = dom.les_sommets();
  int num_premiere_face = la_frontiere.num_premiere_face();
  Entity index1,index2;
  index1 = FACE;
  index2 = NODE;
  const IntTab& face_sommets = source.get_ref_connectivity(index1,index2);
  int nb_som_faces = zvf_source.nb_som_face();

  Nom type_zone,type_elem;
  if (type_face_source==segment_2D || type_face_source==segment_2D_axi)
    type_elem = "Segment"; // Pour MC2
  else if (type_face_source==5)
    type_elem = "Segment"; // Pour MC2
  else if (type_face_source==quadrangle_3D)
    type_elem = "Quadrangle";
  else if (type_face_source==triangle_3D)
    type_elem = "Triangle";
  //Cas suivant possible en parallele
  else if ((type_face_source==vide_0D) && (nb_faces==0))
    {
      if (zvf_source.que_suis_je().debute_par("Zone_VDF"))
        type_elem = "Rectangle";
      else if (zvf_source.que_suis_je().debute_par("Zone_VEF"))
        type_elem = "Triangle";
      else
        {
          Cerr<<"The type of zone discretized"<<zvf_source.que_suis_je()<<" is not recognized"<<finl;
          exit();
        }
    }
  else
    {
      Cerr<<"The type of faces of the boundary name "<<nom_fr_<<" is neither type quadrangle_3D nor triangle_3D"<<finl;
      Cerr<<"We do not realize the extraction"<<finl;
      exit();
    }

  Zone zz;
  domaine_->add(zz);
  Zone& ajoutee= domaine_->zone(0);
  ajoutee.typer(type_elem);
  ajoutee.associer_domaine(domaine_);

  IntTab& mes_elems_zone = ajoutee.les_elems();
  mes_elems_zone.reset();
  mes_elems_zone.resize(nb_faces,nb_som_faces);

  // Destruction du descripteur parallele, et on reconstruit...
  domaine_->les_sommets().reset();
  domaine_->les_sommets()=sommets_source;
  for (int face=0; face<nb_faces; face++)
    for (int s=0; s<nb_som_faces; s++)
      mes_elems_zone(face,s) =face_sommets(num_premiere_face+face,s);
  NettoieNoeuds::nettoie(domaine_);

  //Renumerotation
  ajoutee.associer_domaine(domaine_);
  // Discretisation du domaine d'extraction
  discretiser_domaine();
}

const Domaine& Champ_Generique_Extraction::get_ref_domain() const
{
  if (domaine_.non_nul())
    return domaine_.valeur();
  else
    {
      Cerr<<"There is no extraction domain created to this Champ_Generique_Extraction"<<finl;
      exit();
    }
  //Pour compilation
  return get_ref_domain();
}

void Champ_Generique_Extraction::get_copy_domain(Domaine& domain) const
{
  const Domaine& dom = get_ref_domain();
  domain = dom;
}

const Zone_dis_base& Champ_Generique_Extraction::get_ref_zone_dis_base() const
{
  if (domaine_.non_nul())
    {
      const Zone_dis_base& zone_dis = le_dom_dis.zone_dis(0).valeur();
      return  zone_dis;
    }
  else
    {
      Cerr<<"There is no domain associated to this Champ_Generique_Extraction"<<finl;
      exit();
    }
  //Pour compilation
  return get_ref_zone_dis_base();
}

Entity Champ_Generique_Extraction::get_localisation(const int index) const
{
  Entity loc;
  loc = ELEMENT;
  return loc;
}


//Discretisation du domaine d extraction
//Cette discretisation est necessaire pour associer une zone discretisee
//a l espace de stockage dans la methode get_champ()
void Champ_Generique_Extraction::discretiser_domaine()
{
  if (domaine_.non_nul())
    {
      le_dom_dis.associer_domaine(domaine_.valeur());
      const Probleme_base& Pb = get_ref_pb_base();
      const Discretisation_base& discr = Pb.discretisation();
      const Nom& type_discr = discr.que_suis_je();
      Nom type = "NO_FACE_Zone_";
      type += type_discr;
      le_dom_dis.discretiser(type);
    }
}


