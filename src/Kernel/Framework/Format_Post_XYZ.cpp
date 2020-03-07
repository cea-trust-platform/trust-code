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
// File:        Format_Post_XYZ.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////
#include <Format_Post_XYZ.h>
#include <communications.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Format_Post_XYZ,"Format_Post_XYZ",Format_Post_base);


/////////////////////////////////////////////////////////////////////////////
//Parametre: xyz_basename_
// Signification: debut du nom du fichier
//Extension xyz

//Specificites de ce format
//Ecriture dans un seul fichier (sequentiel ou parallele)
///////////////////////////////////////////////////////////////////////////

// Description:
//  Constructeur par defaut:
//Specifier dans commentaire ce qui est fixe par defaut

Format_Post_XYZ::Format_Post_XYZ()
{
  reset();
}

// Description:
//  Remet l'objet dans l'etat obtenu par le constructeur par defaut.
void Format_Post_XYZ::reset()
{
  xyz_basename_ = "??";
}

// Description: erreur => exit
Sortie& Format_Post_XYZ::printOn(Sortie& os) const
{
  Cerr << "Format_Post_XYZ::printOn : error" << finl;
  exit();
  return os;
}

// Description: Lecture des parametres du postraitement au format "jeu de donnees"
//  Le format attendu est le suivant:
//  {
//    nom_fichier basename_sans_extension
//  }
Entree& Format_Post_XYZ::readOn(Entree& is)
{
  Format_Post_base::readOn(is);
  return is;
}

void Format_Post_XYZ::set_param(Param& param)
{
  param.ajouter("nom_fichier",&xyz_basename_,Param::REQUIRED);
}

// Description: Initialisation de la classe avec des parametres par
//  defaut
int Format_Post_XYZ::initialize_by_default(const Nom& file_basename)
{
  xyz_basename_= file_basename;
  return 1;
}

int Format_Post_XYZ::initialize(const Nom& file_basename, const int& format, const Nom& option_para)
{

  xyz_basename_= file_basename;
  return 1;
}

int Format_Post_XYZ::ecrire_entete(double temps_courant,int reprise,const int& est_le_premier_post)
{

  Nom nom_fich(xyz_basename_);
  nom_fich +=".";
  Nom format="xyz";
  nom_fich += format;

  ecrire_entete_xyz(nom_fich,est_le_premier_post);
  return 1;

}

int Format_Post_XYZ::finir(int& est_le_dernier_post)
{

  Nom nom_fich(xyz_basename_);
  nom_fich +=".";
  Nom format="xyz";
  nom_fich += format;

  finir_xyz(nom_fich,est_le_dernier_post);
  return 1;
}


int Format_Post_XYZ::completer_post(const Domaine& dom,const int is_axi,
                                    const Nature_du_champ& nature,const int nb_compo,const Noms& noms_compo,
                                    const Motcle& loc_post,const Nom& le_nom_champ_post)
{

  completer_post_xyz();
  return 1;

}

int Format_Post_XYZ::preparer_post(const Nom& id_du_domaine,const int& est_le_premier_post,
                                   const int& reprise,
                                   const double& t_init)
{

  preparer_post_xyz();
  return 1;

}

// Description:
//  voir Format_Post_base::ecrire_domaine
int Format_Post_XYZ::ecrire_domaine(const Domaine& domaine,const int& est_le_premier_post)
{
  //Appel de la methode statique specifique au format xyz
  Nom nom_fich(xyz_basename_);
  nom_fich +=".";
  Nom format="xyz";
  nom_fich += format;
  ecrire_domaine_xyz(domaine,nom_fich);
  return 1; // ok tout va bien
}

// Description: commence l'ecriture d'un nouveau pas de temps
//  En l'occurence pour le format XYZ:
//  Ouvre le fichier maitre en mode APPEND et ajoute une ligne
//   "TEMPS xxxxx"
int Format_Post_XYZ::ecrire_temps(const double temps)
{

  Nom nom_fich(xyz_basename_);
  nom_fich +=".";
  Nom format="xyz";
  nom_fich += format;

  ecrire_temps_xyz(temps,nom_fich);

  return 1;
}

// Description:
//  voir Format_Post_base::ecrire_champ
int Format_Post_XYZ::ecrire_champ2(const Domaine& domaine,const Noms& unite_, const Noms& noms_compo,
                                   int ncomp,double temps_,double temps_courant,
                                   const Nom& id_champ,
                                   const Nom& id_du_domaine,
                                   const Nom& localisation,
                                   const Nom&   nature,
                                   const DoubleTab& valeurs,
                                   const DoubleTab& coord)

{

  //Appel de la methode statique specifique au format XYZ

  Nom nom_fich = construire_nom_xyz(temps_,id_champ,id_du_domaine,localisation);

  ecrire_champ_xyz(domaine,unite_,noms_compo,ncomp,temps_,id_champ,id_du_domaine,localisation,valeurs,nom_fich,coord);

  return 1;
}

// Description:
int Format_Post_XYZ::ecrire_item_int(const Nom&     id_item,
                                     const Nom&     id_du_domaine,
                                     const Nom&     id_zone,
                                     const Nom&     localisation,
                                     const Nom&     reference,
                                     const IntVect& valeurs,
                                     const int   reference_size)
{
  Nom nom_fich(xyz_basename_);
  nom_fich +=".";
  Nom format="xyz";
  nom_fich += format;
  Nom id_zone_dom(id_zone);
  id_zone_dom+="_";
  id_zone_dom+=id_du_domaine;
  ecrire_item_int_xyz(id_item,id_zone,valeurs,nom_fich);

  return 1;
}
Nom Format_Post_XYZ::construire_nom_xyz(double temps_,
                                        const Nom& id_champ,
                                        const Nom& id_du_domaine,
                                        const Nom& localisation)
{
  // recup de lata  pour construire le nom du fichier
  // Un fichier xyz sera cree par champs et par pas de temps de post-traitement
  Nom id_du_champ_modifie(id_champ);
  Nom iddomaine(id_du_domaine);
  //On utilise prefix avec un argument en majuscule
  if ((Motcle)localisation=="SOM")
    {
      id_du_champ_modifie.prefix(id_du_domaine);
      id_du_champ_modifie.prefix(iddomaine);
      ////id_du_champ_modifie.prefix("_som_");
      id_du_champ_modifie.prefix("_SOM_");
    }
  else if ((Motcle)localisation=="ELEM")
    {
      id_du_champ_modifie.prefix(id_du_domaine);
      id_du_champ_modifie.prefix(iddomaine);
      ////id_du_champ_modifie.prefix("_elem_");
      id_du_champ_modifie.prefix("_ELEM_");
    }
  else if ((Motcle)localisation=="FACES")
    {
      id_du_champ_modifie.prefix(id_du_domaine);
      id_du_champ_modifie.prefix(iddomaine);
      id_du_champ_modifie.prefix("_FACES_");
    }
  // Construction du nom du fichier
  Nom extension_champ(id_du_champ_modifie);
  extension_champ += ".";
  extension_champ += localisation;
  extension_champ += ".";
  extension_champ += id_du_domaine;
  extension_champ += ".";
  char str_temps[100] = "0.0";
  if (temps_ >= 0.)
    sprintf(str_temps, "%.10f", temps_);
  extension_champ += str_temps;
  Nom nom_fich(xyz_basename_);
  nom_fich += "." ;
  nom_fich += extension_champ;
  nom_fich +=".";
  Nom format="xyz";
  nom_fich += format;

  return nom_fich;
}

int Format_Post_XYZ::ecrire_entete_xyz(Nom& nom_fich,const int& est_le_premier_post)
{
  return 1;
}

int Format_Post_XYZ::finir_xyz(Nom& nom_fich, const int& est_le_dernier_post)
{
  return 1;
}

int Format_Post_XYZ::completer_post_xyz()
{
  return 1;
}

int Format_Post_XYZ::preparer_post_xyz()
{
  return 1;
}

int Format_Post_XYZ::ecrire_domaine_xyz(const Domaine& domaine,Nom& nom_fich)
{
  return 1;
}

int Format_Post_XYZ::ecrire_temps_xyz(const double temps,Nom& nom_fich)
{
  return 1;
}

int Format_Post_XYZ::ecrire_champ_xyz(const Domaine& domaine,const Noms& unite_,const Noms& noms_compo,
                                      int ncomp,double temps_,
                                      const Nom&   id_du_champ,
                                      const Nom&   id_du_domaine,
                                      const Nom&   localisation,
                                      const DoubleTab& data,Nom& nom_fich,
                                      const DoubleTab& coord)
{
  //////////////////////////////////////////////////////////////////////////////////////
  EcrFicPartage s;
// J'ecris tout en une seule fois donc pas la peine d'ouvrir en mode append
  s.ouvrir(nom_fich);
  int dim = coord.dimension(1);
  int nb_compo_=1;
  if (data.nb_dim()>1)
    nb_compo_=data.dimension(1);
  // On ecrit uniquement le n nombre de lignes contenues dans le fichier xyz
  // il correspond au nombre de coordonees
  int n = data.dimension(0);
  if(je_suis_maitre())
    s << n << finl;
  for (int j=0; j<n; j++)
    {
      // Les coord du point j
      for (int ncomp2=0; ncomp2<dim; ncomp2++)
        s << coord(j, ncomp2) << " " ;
      // Les val du champs au point j
      if (nb_compo_==1)
        s << data(j) << " ";
      else
        for (int ncomp2=0; ncomp2<nb_compo_; ncomp2++)
          s << data(j,ncomp2) << " ";
      s << finl;
    }
  s.unlockfile();
  s.syncfile();
  //////////////////////////////////////////////////////////////

  s.flush();
  s.syncfile();

  return 1;
}

int Format_Post_XYZ::ecrire_item_int_xyz(const Nom&   id_item,
                                         const Nom&   id_zone,
                                         const IntVect& vect,
                                         const Nom& nom_fic)
{
  return 1;
}
