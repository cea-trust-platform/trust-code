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
// File:        Format_Post_Lml.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////
#include <Format_Post_Lml.h>
#include <communications.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Format_Post_Lml,"Format_Post_Lml",Format_Post_base);


/////////////////////////////////////////////////////////////////////////////
//Parametre: lml_basename_
// Signification: debut du nom du fichier
//Extension lml

//Specificites de ce format
//Ecriture dans un seul fichier (sequentiel ou parallele)
///////////////////////////////////////////////////////////////////////////

// Description:
//  Constructeur par defaut:
//Specifier dans commentaire ce qui est fixe par defaut

Format_Post_Lml::Format_Post_Lml()
{
  reset();
}

// Description:
//  Remet l'objet dans l'etat obtenu par le constructeur par defaut.
void Format_Post_Lml::reset()
{
  lml_basename_ = "??";
}

// Description: erreur => exit
Sortie& Format_Post_Lml::printOn(Sortie& os) const
{
  Cerr << "Format_Post_Lml::printOn : error" << finl;
  exit();
  return os;
}

// Description: Lecture des parametres du postraitement au format "jeu de donnees"
//  Le format attendu est le suivant:
//  {
//    nom_fichier basename_sans_extension
//  }
Entree& Format_Post_Lml::readOn(Entree& is)
{
  Format_Post_base::readOn(is);
  return is;
}

void Format_Post_Lml::set_param(Param& param)
{
  param.ajouter("nom_fichier",&lml_basename_,Param::REQUIRED);
}

// Description: Initialisation de la classe avec des parametres par
//  defaut
int Format_Post_Lml::initialize_by_default(const Nom& file_basename)
{
  lml_basename_= file_basename;
  return 1;
}

int Format_Post_Lml::initialize(const Nom& file_basename, const int& format, const Nom& option_para)
{

  lml_basename_= file_basename;
  return 1;
}

int Format_Post_Lml::ecrire_entete(double temps_courant,int reprise,const int& est_le_premier_post)
{

  Nom nom_fich(lml_basename_);
  nom_fich +=".";
  Nom format="lml";
  nom_fich += format;

  ecrire_entete_lml(nom_fich,est_le_premier_post);
  return 1;

}

int Format_Post_Lml::finir(int& est_le_dernier_post)
{

  Nom nom_fich(lml_basename_);
  nom_fich +=".";
  Nom format="lml";
  nom_fich += format;

  finir_lml(nom_fich,est_le_dernier_post);
  return 1;
}


int Format_Post_Lml::completer_post(const Domaine& dom,const int is_axi,
                                    const Nature_du_champ& nature,const int nb_compo,const Noms& noms_compo,
                                    const Motcle& loc_post,const Nom& le_nom_champ_post)
{

  completer_post_lml();
  return 1;

}

int Format_Post_Lml::preparer_post(const Nom& id_du_domaine,const int& est_le_premier_post,
                                   const int& reprise,
                                   const double& t_init)
{

  preparer_post_lml();
  return 1;

}

// Description:
//  voir Format_Post_base::ecrire_domaine
int Format_Post_Lml::ecrire_domaine(const Domaine& domaine,const int& est_le_premier_post)
{
  //Appel de la methode statique specifique au format lml
  Nom nom_fich(lml_basename_);
  nom_fich +=".";
  Nom format="lml";
  nom_fich += format;
  ecrire_domaine_lml(domaine,nom_fich);
  return 1; // ok tout va bien
}

// Description: commence l'ecriture d'un nouveau pas de temps
//  En l'occurence pour le format Lml:
//  Ouvre le fichier maitre en mode APPEND et ajoute une ligne
//   "TEMPS xxxxx"
int Format_Post_Lml::ecrire_temps(const double temps)
{

  Nom nom_fich(lml_basename_);
  nom_fich +=".";
  Nom format="lml";
  nom_fich += format;

  ecrire_temps_lml(temps,nom_fich);

  return 1;
}

// Description:
//  voir Format_Post_base::ecrire_champ
int Format_Post_Lml::ecrire_champ(const Domaine& domaine,const Noms& unite_, const Noms& noms_compo,
                                  int ncomp,double temps_,double temps_courant,
                                  const Nom& id_champ,
                                  const Nom& id_du_domaine,
                                  const Nom& localisation,
                                  const Nom&   nature,
                                  const DoubleTab& valeurs)

{

  //Appel de la methode statique specifique au format lml
  Nom nom_fich(lml_basename_);
  nom_fich +=".";
  Nom format="lml";
  nom_fich += format;

  ecrire_champ_lml(domaine,unite_,noms_compo,ncomp,temps_,id_champ,id_du_domaine,localisation,valeurs,nom_fich);

  return 1;
}

// Description:
int Format_Post_Lml::ecrire_item_int(const Nom&     id_item,
                                     const Nom&     id_du_domaine,
                                     const Nom&     id_zone,
                                     const Nom&     localisation,
                                     const Nom&     reference,
                                     const IntVect& valeurs,
                                     const int   reference_size)
{

  Nom nom_fich(lml_basename_);
  nom_fich +=".";
  Nom format="lml";
  nom_fich += format;
  Nom id_zone_dom(id_zone);
  id_zone_dom+="_";
  id_zone_dom+=id_du_domaine;
  ecrire_item_int_lml(id_item,id_zone,valeurs,nom_fich);

  return 1;
}

int Format_Post_Lml::ecrire_entete_lml(Nom& nom_fich,const int& est_le_premier_post)
{
  if (est_le_premier_post)
    {

      EcrFicPartage s;
      s.ouvrir(nom_fich);

      if (Process::je_suis_maitre())
        {
          s << "Trio_U" << " " << "Version "  << "1" << finl;
          s << nom_du_cas() << finl;
          s << "Trio_U" << finl;
        }

      s.flush();
      s.syncfile();

    }
  return 1;
}

int Format_Post_Lml::finir_lml(Nom& nom_fich, const int& est_le_dernier_post)
{

  EcrFicPartage s;
  s.ouvrir(nom_fich,ios::app);

  if (est_le_dernier_post)
    {
      if (Process::je_suis_maitre())
        {
          s << "FIN" << finl;
        }
    }

  s.flush();
  s.syncfile();

  return 1;
}

int Format_Post_Lml::completer_post_lml()
{
  return 1;
}

int Format_Post_Lml::preparer_post_lml()
{
  return 1;
}

int Format_Post_Lml::ecrire_domaine_lml(const Domaine& domaine,Nom& nom_fich)
{
  //Actuellement copie identique de la methode Domaine::postraiter_lml()

  EcrFicPartage s;
  s.ouvrir(nom_fich,ios::app);
  Nom nom_grille("Grille_");
  nom_grille+= domaine.le_nom();

  if(je_suis_maitre())
    s << "GRILLE" << finl;

  int nb_som_tot = Process::mp_sum(domaine.nb_som());
  int dim = domaine.les_sommets().dimension(1);

  // Ecriture des coordonnees des noeuds
  // Toutes les grandeurs qui apparaissent dans le .lml doivent
  // etre exprimees dans le repere des coordonnees cartesiennes
  // Si l'option Axi a ete choisie on recalcule les coordonnees
  // cartesiennes a partir des coordonnees cylindriques.

  if (axi)
    {
      // Ecriture des noeuds du maillage
      if ( dim == 3)
        {
          if(je_suis_maitre())
            s << nom_grille << " " << "3 " << nb_som_tot << "" << finl;
          s.lockfile();

          for (int nsom=0; nsom<domaine.nb_som(); nsom++)
            {
              s << domaine.coord(nsom,0)*cos(domaine.coord(nsom,1)) << " "
                << domaine.coord(nsom,0)*sin(domaine.coord(nsom,1)) << " "
                << domaine.coord(nsom,2) << "" << finl;
            }

          s.unlockfile();
          s.syncfile();
        }
      else if ( dim == 2)
        {
          if(je_suis_maitre())
            s << nom_grille << " " << "3 " << 2*nb_som_tot << "" << finl;
          s.lockfile();
          // Ecriture des noeuds du maillage
          int nsom;

          for (nsom=0; nsom<domaine.nb_som(); nsom++)
            {
              s << domaine.coord(nsom,0)*cos(domaine.coord(nsom,1)) << " "
                << domaine.coord(nsom,0)*sin(domaine.coord(nsom,1)) << " "
                <<  0. << "" << finl;
            }
          for (nsom=0; nsom<domaine.nb_som(); nsom++)
            {
              s << domaine.coord(nsom,0)*cos(domaine.coord(nsom,1)) << " "
                << domaine.coord(nsom,0)*sin(domaine.coord(nsom,1)) << " "
                << 1. << "" << finl;
            }

          s.unlockfile();
          s.syncfile();
        }
    }
  else
    {
      // Ecriture des noeuds du maillage
      if ( dim == 3)
        {
          if(je_suis_maitre())
            s << nom_grille << " " << "3 " << nb_som_tot << "" << finl;
          s.lockfile();

          for (int nsom=0; nsom<domaine.nb_som(); nsom++)
            {
              for (int j=0; j<3; j++)
                s << domaine.coord(nsom, j) << " " ;
              s << "" << finl;
            }

          s.unlockfile();
          s.syncfile();
        }
      else if ( dim == 2)
        {
          if(je_suis_maitre())
            s << nom_grille << " " << "3 " << 2*nb_som_tot << "" << finl;
          s.lockfile();
          // Ecriture des noeuds du maillage
          int nsom;

          for (nsom=0; nsom<domaine.nb_som(); nsom++)
            {
              s << domaine.coord(nsom,0) << " "
                << domaine.coord(nsom,1) << " "
                << 0. << "" << finl;
            }
          for (nsom=0; nsom<domaine.nb_som(); nsom++)
            {
              s << domaine.coord(nsom,0) << " "
                << domaine.coord(nsom,1) << " "
                << 1. << "" << finl;
            }

          s.unlockfile();
          s.syncfile();
        }
    }

  //Actuellement copie identique de la methode Zone::postraiter_lml()

  //////////////////////////////////////////////////////////////

  int j;
  int nsom = domaine.nb_som();
  int nb_elem_tot = Process::mp_sum(domaine.zone(0).nb_elem());
  Nom nom_top("Topologie_");
  nom_top+= domaine.zone(0).le_nom();
  nom_top+="_";
  nom_top+=domaine.le_nom();
  Nom type_maille=domaine.zone(0).type_elem().nom_lml();


  //Ajout pour modification syntaxe ci-dessous
  int nb_elem = domaine.zone(0).nb_elem() ;
  int nb_som_elem = domaine.zone(0).nb_som_elem();
  const IntTab& sommet_elem = domaine.zone(0).les_elems();
  int nb_som_PE = mppartial_sum(nsom);
  if(je_suis_maitre())
    {
      s << "TOPOLOGIE" << finl;
      s << nom_top << " " << nom_grille << "" << finl;
      s <<  "MAILLE" << finl;
      s << nb_elem_tot << "" << finl;
    }

  // Ecriture des elements du maillage
  if (dim == 3 )
    {
      s.lockfile();

      for (int nelem=0; nelem<nb_elem; nelem++)
        {
          s << type_maille << " ";
          for (j=0; j<nb_som_elem; j++)
            s << sommet_elem(nelem,j)+1+nb_som_PE << " " ;
          s << "" << finl;
        }
      s.unlockfile();
      s.syncfile();
    }
  else if ( dim == 2)
    {
      s.lockfile();
      for (int nelem=0; nelem<nb_elem; nelem++)
        {
          s << type_maille << " ";
          for (j=0; j<nb_som_elem; j++)
            s << sommet_elem( nelem,j)+1+2*nb_som_PE << " " ;
          for (j=0; j<nb_som_elem; j++)
            s << sommet_elem( nelem,j)+1+2*nb_som_PE+nsom << " " ;
          s << "" << finl;
          s.flush();
        }
      s.unlockfile();
      s.syncfile();
    }
  if(je_suis_maitre())
    {
      s <<  "FACE" << finl ;
      s << 0 << "" << finl;
    }


  //////////////////////////////////////////////////////////////

  s.flush();
  s.syncfile();

  return 1;
}

int Format_Post_Lml::ecrire_temps_lml(const double temps,Nom& nom_fich)
{

  EcrFicPartage s;
  s.ouvrir(nom_fich,ios::app);
  if (je_suis_maitre())
    s << "TEMPS " << temps << finl;
  s.flush();
  s.syncfile();

  return 1;
}

int Format_Post_Lml::ecrire_champ_lml(const Domaine& domaine,const Noms& unite_,const Noms& noms_compo,
                                      int ncomp,double temps_,
                                      const Nom&   id_du_champ,
                                      const Nom&   id_du_domaine,
                                      const Nom&   localisation,
                                      const DoubleTab& data,Nom& nom_fich)

{

  //////////////////////////////////////////////////////////////////////////////////////
  if (ncomp==-1)
    {

      EcrFicPartage os;
      os.ouvrir(nom_fich,ios::app);
      int dim = domaine.les_sommets().dimension(1);

      int nb_compo_=1;
      if (data.nb_dim()>1)
        nb_compo_=data.dimension(1);


      Nom nom_top("Topologie_");
      nom_top+= domaine.zone(0).le_nom();
      nom_top+="_";
      nom_top+=domaine.le_nom();
      Nom nom_post=id_du_champ;
      Nom nom_dom=domaine.le_nom();

      if(localisation=="SOM")
        {
          int nb_som=domaine.nb_som();
          int nb_som_tot = Process::mp_sum(nb_som);
          int nb_som_PE = mppartial_sum(nb_som);
          if(je_suis_maitre())
            {
              os << "CHAMPPOINT ";
              os << nom_post << " " << nom_top << " " << temps_ << finl;
            }
          int nb_compo_dv=1;
          if(nb_compo_!=1) nb_compo_dv=3;
          if(je_suis_maitre())
            {
              os << nom_post << " " << nb_compo_dv << " " << unite_[0] << " "
                 << "type0" << " " ;
            }
          if (dim ==3)
            {
              if(je_suis_maitre())
                {
                  os <<nb_som_tot << finl;
                }
              if (nb_compo_ == 1)
                {
                  os.lockfile();
                  for (int j=0; j<nb_som; j++)
                    os << j+1+nb_som_PE << " " << data(j,0) << finl;
                  os.unlockfile();
                  os.syncfile();
                }
              else
                {
                  os.lockfile();
                  for (int j=0; j<nb_som; j++)
                    {
                      os << j+1+nb_som_PE << " " ;
                      for (int ncomp2=0; ncomp2<nb_compo_; ncomp2++)
                        os << data(j,ncomp2) << " ";
                      if (nb_compo_==2) os << "0." ;
                      os << finl;
                    }
                  os.unlockfile();
                  os.syncfile();
                }
            }
          else if (dim ==2)
            {
              if(je_suis_maitre())
                {
                  os << 2*nb_som_tot << finl;
                }
              if (nb_compo_ == 1)
                {
                  int j;
                  os.lockfile();
                  for (j=0; j<nb_som; j++)
                    os << j+1+(2*nb_som_PE) << " " << data(j,0) << finl;
                  for (j=0; j<nb_som; j++)
                    os << nb_som+j+1+(2*nb_som_PE) << " " << data(j,0) << finl;
                  os.unlockfile();
                  os.syncfile();
                }
              else
                {
                  int j;
                  os.lockfile();
                  for (j=0; j<nb_som; j++)
                    {
                      os << j+1+(2*nb_som_PE) << " " ;
                      for (int ncomp2=0; ncomp2<nb_compo_; ncomp2++)
                        os << data(j,ncomp2) << " ";
                      if (nb_compo_==2) os << "0." ;
                      os << finl;
                    }
                  for (j=0; j<nb_som; j++)
                    {
                      os << nb_som+j+1+(2*nb_som_PE) << " " ;
                      for (int ncomp2=0; ncomp2<nb_compo_; ncomp2++)
                        os << data(j,ncomp2) << " ";
                      if (nb_compo_==2) os << "0." ;
                      os << finl;
                    }
                  os.unlockfile();
                  os.syncfile();
                }
            }
        }
      else if(localisation=="ELEM")
        {
          const Zone& zone=domaine.zone(0);
          int nb_elem=zone.nb_elem();
          int nb_elem_tot = Process::mp_sum(nb_elem);
          int nb_elem_PE = mppartial_sum(nb_elem);
          if (nb_compo_ == 1)
            {
              if(je_suis_maitre())
                {
                  os << "CHAMPMAILLE ";
                  os << nom_post << " " << nom_top << " " << temps_ << finl;
                  os << nom_post << " " << nb_compo_ << " " << unite_[0] << " " ;
                  os << "type0" << " " << nb_elem_tot << finl;
                }
              os.lockfile();
              for (int num_elem=0; num_elem<nb_elem; num_elem++)
                os << num_elem+1+nb_elem_PE << " " << data(num_elem,0) << finl;
              os.unlockfile();
              os.syncfile();
            }
          else
            {
              Noms noms_post(nb_compo_);
              //On supprime l ajout de l extension elem_dom car les composantes
              //d un Champ_Generique_Interpolation possedent deja l extension loc_nom_domaine
              for (int i=0; i<nb_compo_; i++)
                {
                  noms_post[i] = noms_compo[i];
                }


              for (int ncomp2=0; ncomp2<nb_compo_; ncomp2++)
                {
                  if(je_suis_maitre())
                    {
                      os << "CHAMPMAILLE ";
                      os << noms_post[ncomp2]  << " " << nom_top << " " << temps_ << finl;
                      os << noms_post[ncomp2] << " " << 1 << " " << unite_[ncomp2] << " " ;
                      os << "type0" << " " << nb_elem_tot << finl;
                    }
                  os.lockfile();
                  for (int num_elem=0; num_elem<nb_elem; num_elem++)
                    os << num_elem+1+nb_elem_PE << " " << data(num_elem,ncomp2) << finl;
                  os.unlockfile();
                  os.syncfile();
                }
            }
          //      Debog::verifier("Champ_base::postraiter_lml_dom type_elem valeurs 2 :",valeurs);
        }
      else
        {
          Cerr << "We do not know to postprocess " <<  id_du_champ
               << " with the keyword " << localisation << finl;
          return -1;
        }

      os.flush();
      os.syncfile();

    }
  else
    {
      //////////////////////////////////////////////////////////////////////////

      //////////////////////////////////////////////////////////////////////////


      EcrFicPartage os;
      os.ouvrir(nom_fich,ios::app);

      //Cout << "Champ_base::postraiter_lml_dom " << que_suis_je() << " " << le_nom() << " sur " << dom.le_nom() << " aux " << loc_post << finl;
      Nom nom_top("Topologie_");
      nom_top+= domaine.zone(0).le_nom();
      nom_top+="_";
      nom_top+=domaine.le_nom();

      int dim = domaine.les_sommets().dimension(1);

      //On idendifie nom_post a id_du_champ car complement de nom ajoute dans calculer_valeurs_som_compo_post()
      //ou calculer_valeurs_elem_compo_post()
      ///Nom nom_post = noms_compo[ncomp];
      Nom nom_post = id_du_champ;
      Nom nom_dom= domaine.le_nom();

      if(localisation=="SOM")
        {
          int nb_som=domaine.nb_som();
          int nb_som_tot = Process::mp_sum(nb_som);
          int nb_som_PE = mppartial_sum(nb_som);
          if(je_suis_maitre())
            {
              os << "CHAMPPOINT ";
              os << nom_post << " " << nom_top << " " << temps_ << finl;
            }
          if (dim ==3)
            {
              if(je_suis_maitre())
                {
                  os << nom_post << " " << 1 << " " << unite_[ncomp] << " "
                     << "type0" << " " << nb_som_tot << finl;
                }
              os.lockfile();
              for (int j=0; j<nb_som; j++)
                {
                  os << j+1+nb_som_PE << " " << data(j) << finl;
                }
              os.unlockfile();
              os.syncfile();
            }
          else if (dim ==2)
            {
              if(je_suis_maitre())
                {
                  os << nom_post << " " << 1 << " " << unite_[ncomp] << " "
                     << "type0" << " " << 2*nb_som_tot << finl;
                }
              os.lockfile();
              int j;
              for (j=0; j<nb_som; j++)
                os << j+1+(2*nb_som_PE) << " " << data(j) << finl;
              for (j=0; j<nb_som; j++)
                os << nb_som+j+1+(2*nb_som_PE) << " " << data(j) << finl;
              os.unlockfile();
              os.syncfile();
            }
        }
      else if(localisation=="ELEM")
        {
          const Zone& zone=domaine.zone(0);
          int nb_elem=zone.nb_elem();

          int nb_elem_tot = Process::mp_sum(nb_elem);
          int nb_elem_PE = mppartial_sum(nb_elem);
          if(je_suis_maitre())
            {
              os << "CHAMPMAILLE ";
              os << nom_post << " " << nom_top << " " << temps_ << finl;
              os << nom_post << " " << 1 << " " << unite_[ncomp] << " " ;
              os << "type0" << " " << nb_elem_tot << finl;
            }
          os.lockfile();
          for (int num_elem=0; num_elem<nb_elem; num_elem++)
            os << num_elem+1+nb_elem_PE << " " << data(num_elem) << finl;
          os.unlockfile();
          os.syncfile();
        }
      else
        {
          Cerr << "We do not know to postprocess " << id_du_champ
               << " with the keyword " << localisation << finl;

          return -1;
        }

      os.flush();
      os.syncfile();

      //////////////////////////////////////////////////////////////////////////
    }

  return 1;


}


int Format_Post_Lml::ecrire_item_int_lml(const Nom&   id_item,
                                         const Nom&   id_zone,
                                         const IntVect& vect,
                                         const Nom& nom_fic)
{
  EcrFicPartage os;
  os.ouvrir(nom_fic,ios::app);

  Nom nom_vect(id_item);
  nom_vect += "_elem_dom";

  Nom nom_topologie("Topologie_");
  nom_topologie+= id_zone;

  //Actuellement temp_courant n est pas passe en argument
  ////os << "CHAMPMAILLE " << nom_vect << " "<< nom_topologie << " " << temps_courant << finl;

  if (sub_type(IntTab,vect))
    {
      const IntTab& tab = ref_cast(IntTab,vect);
      os.lockfile();
      os << nom_vect << tab.dimension(1) << " 1 " << " type0 " << tab.dimension(0) << finl;
      os.unlockfile();
      os.syncfile();
      {
        int tab_dimension0_opt=tab.dimension(0);
        os.lockfile();
        for (int i=0; i<tab_dimension0_opt; i++)
          {
            for (int j=0; j<tab.dimension(1); j++)
              {
                int tab_dimension1_opt=tab.dimension(1);
                abort();
                // GF on faisait quoi ????
                for (int j2=0; j2<tab_dimension1_opt; j2++)
                  os << i+1 << " " << tab(i,j2) << " ";
              }
            os << finl;
          }
        os.unlockfile();
        os.syncfile();
      }

    }
  else
    {
      os.lockfile();
      os << nom_vect << " 1" << " 1 " << " type0 " << vect.size() << finl;
      for (int i=0; i<vect.size(); i++)
        os << i+1 << " " << vect(i) << finl;
      os.unlockfile();
      os.syncfile();
    }

  os.flush();
  os.syncfile();

  return 1;
}


