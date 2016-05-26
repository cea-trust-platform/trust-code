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
// File:        Format_Post_Lata_V1.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////
#include <Format_Post_Lata_V1.h>
#include <Fichier_Lata.h>
#include <communications.h>
#include <SFichier.h>
#include <LataV1_field_definitions.h>

Implemente_instanciable_sans_constructeur(Format_Post_Lata_V1,"Format_Post_Lata_V1",Format_Post_Lata);

Format_Post_Lata_V1::Format_Post_Lata_V1()
{
  ref_faces_sommets = 0;
}

void Format_Post_Lata_V1::reset()
{
  ref_faces_sommets = 0;
}

// Description: les options sont identiques a celles du format V2
//  Voir Format_Post_Lata::readOn()
Entree& Format_Post_Lata_V1::readOn(Entree& is)
{
  return Format_Post_Lata::readOn(is);
}

Sortie& Format_Post_Lata_V1::printOn(Sortie& os) const
{
  return Format_Post_Lata::printOn(os);
}

// Description: ecriture de l'entete format LataV1
int Format_Post_Lata_V1::ecrire_entete(double temps_courant,int reprise,const int& est_le_premier_post)
{
  ecrire_entete_lata_V1(lata_basename_,options_para_,est_le_premier_post);

  return 1;
}

int Format_Post_Lata_V1::finir(int& est_le_dernier_post)
{
  finir_lata_V1(lata_basename_,options_para_,est_le_dernier_post);
  return 1;
}

// Description:
//  1) Voir documentation de Format_Post_base::ecrire_domaine()
//  2) Specificites: format LATA ancienne version:
//   Ecriture du maillage compatible avec le postraitement Data-Visualizer (DV).
//   DV est un post 3D uniquement. Si le cas traite est 2D on
//   le rend 3D de la maniere suivante :
//    - On dedouble tous les noeuds du plan 2D
//    - On transforme les triangles en prismes
//    - On transforme les quadrangles en hexaedres
//   Rq :la numerotation des elements dans DV commence a 1.


int Format_Post_Lata_V1::ecrire_domaine(const Domaine& domaine,const int& est_le_premier_post)
{
  const Nom& id_domaine = domaine.le_nom();
  Motcle type_elem = domaine.zone(0).type_elem()->que_suis_je();
  if (dimension==3)
    {
      if ((type_elem=="TRIANGLE") || (type_elem=="QUADRANGLE"))
        type_elem+="_3D";
    }
  const DoubleTab& sommets = domaine.les_sommets();
  const int dim = domaine.les_sommets().dimension(1);
  const IntTab& elements = domaine.zone(0).les_elems();

  ecrire_domaine_lata_V1(id_domaine,type_elem,dim,sommets,elements,status,lata_basename_,format_,options_para_);

  return 1; // ok tout va bien
}

int Format_Post_Lata_V1::ecrire_champ(const Domaine& domaine, const Noms& unite_, const Noms& noms_compo,
                                      int ncomp,double temps_,double temps_courant,
                                      const Nom&   id_du_champ,
                                      const Nom&   id_du_domaine,
                                      const Nom&   localisation,
                                      const Nom&   nature,
                                      const DoubleTab& data)
{

  ecrire_champ_lata_V1(id_du_champ,id_du_domaine,localisation,data,temps_,lata_basename_,format_,options_para_);

  return 1;
}

// Description: l'ecriture des bords n'est pas supportee par ce format: return 0
int Format_Post_Lata_V1::ecrire_bords(const Nom&     id_du_domaine,
                                      const Motcle& type_faces,
                                      const DoubleTab& sommets,
                                      const IntTab& faces_sommets,
                                      const IntTab& faces_num_bord,
                                      const Noms&    bords_nom)
{
  return 0;
}

// On intercepte uniquement les tableaux FACES et ELEM_FACES
//  qui sont ecrits dans le meme fichier que le domaine.
int Format_Post_Lata_V1::ecrire_item_int(const Nom&   id_item,
                                         const Nom&   id_du_domaine,
                                         const Nom&   id_zone,
                                         const Nom&   localisation,
                                         const Nom&   reference,
                                         const IntVect& val,
                                         const int reference_size)
{
  const IntTab& data = ref_cast(IntTab,val);

  if (id_item == "FACES")
    {
      ref_faces_sommets = &data;
      ref_nb_som = reference_size;
      return 1;
    }
  else if (id_item == "ELEM_FACES")
    {
      // continue
    }
  else
    {
      ref_faces_sommets = 0;
      // not supported
      return 0;
    }

  ecrire_faces_lata_V1(lata_basename_,
                       format_,
                       options_para_,
                       id_du_domaine,
                       ref_cast(IntTab,*ref_faces_sommets),
                       ref_cast(IntTab,val),
                       ref_nb_som);

  return 0;
}

// On intercepte uniquement les tableaux FACES et ELEM_FACES
//  qui sont ecrits dans le meme fichier que le domaine.
int Format_Post_Lata_V1::ecrire_faces_lata_V1(const Nom& basename,
                                              const Format& format,
                                              const Options_Para& option,
                                              const Nom&   id_du_domaine,
                                              const IntTab& faces,
                                              const IntTab& elem_faces,
                                              const int nb_sommets)
{
  Nom nom_fichier_geom;
  int nb_faces_tot = -1;
  int decalage_faces = 1;
  {
    Nom basename_geom(basename);
    Nom extension_geom(extension_lata());
    extension_geom += ".";
    extension_geom += id_du_domaine;
    Fichier_Lata fichier_geom(basename_geom, extension_geom,
                              Fichier_Lata::APPEND, format, option);
    nom_fichier_geom = fichier_geom.get_filename();

    // Faces
    // Les indices de sommets, elements et autres dans le fichier lata commencent a 1 :
    int nb_col;
    int decal;
    if (option == SINGLE_FILE && Process::nproc() > 1)
      {
        // Decalage a ajouter aux indices pour avoir une numerotation globale.
        decal = mppartial_sum(nb_sommets) + 1;
        decalage_faces = mppartial_sum(faces.dimension(0)) + 1;
      }
    else
      {
        decal = 1;
        decalage_faces = 1;
      }
    nb_faces_tot = Format_Post_Lata::write_inttab(fichier_geom,
                                                  1, decal,
                                                  faces,
                                                  nb_col,
                                                  option);

    Format_Post_Lata::write_inttab(fichier_geom,
                                   1, decalage_faces,
                                   elem_faces,
                                   nb_col,
                                   option);
  }

  Fichier_Lata_maitre fichier_lata(basename, extension_lata(),
                                   Fichier_Lata::APPEND, option);
  SFichier& sfichier = fichier_lata.get_SFichier();

  Nom filename_joint_faces;
  if (option == SINGLE_FILE && Process::nproc() > 1)
    {
      Nom extension;
      extension = extension_lata();
      extension += ".";
      extension += "decal_faces_proc";
      extension += ".";
      extension += id_du_domaine;
      {
        Fichier_Lata fichier_champ(basename, extension,
                                   Fichier_Lata::ERASE, format, option);
        filename_joint_faces = fichier_champ.get_filename();
        int nb_col = -1; // non utilise
        IntTab data(1,2);
        data(0, 0) = decalage_faces;
        data(0, 1) = faces.dimension(0);
        Format_Post_Lata::write_inttab(fichier_champ,0,  0 /* ne pas ajouter de decalage */,
                                       data, nb_col, option);
      }
    }

  if (fichier_lata.is_master())
    {
      sfichier << "nb_faces_tot=" << nb_faces_tot << finl;
      if (option == SINGLE_FILE && Process::nproc() > 1)
        {
          const int nproc = Process::nproc();
          sfichier << "joints_faces nproc=" << nproc << " decalage=" << remove_path(filename_joint_faces) << finl;
        }
    }
  fichier_lata.syncfile();

  return 1; // ok tout va bien
}


int Format_Post_Lata_V1::ecrire_domaine_lata_V1(const Nom& id_domaine,
                                                const Motcle& type_elem,
                                                const int dim,
                                                const DoubleTab& sommets,
                                                const IntTab& elements,
                                                const Status& stat, const Nom& basename, const Format&
                                                format, const Options_Para& option)
{

  if (stat > INITIALIZED)
    {
      return 0; // Ecriture tardive des domaines dynamiques non supportee
    }

  // Construction du nom du fichier de geometrie
  Nom basename_geom(basename);
  Nom extension_geom(extension_lata());
  extension_geom += ".";
  extension_geom += id_domaine;

  Nom nom_fichier_geom;
  // Indice global attribue au premier sommet de ce processeur:
  int decalage_sommets = -1;
  int decalage_elements = -1;
  {
    Fichier_Lata fichier_geom(basename_geom, extension_geom,
                              Fichier_Lata::ERASE, format,option);

    nom_fichier_geom = fichier_geom.get_filename();
    int nb_col;
    // Coordonnees des sommets
    const int nb_sommets = sommets.dimension(0);

    if (dim==2)
      {
        // Ecriture en deux fois : tous les processeurs ecrivent une premiere rangee
        // de sommets, puis tous les processeurs ecrivent une deuxieme rangee.
        for (int bloc_number = 0; bloc_number < 2; bloc_number++)
          {
            DoubleTab sommets2(nb_sommets, 3);
            const double z = (bloc_number == 0) ? 0. : 1.;
            for (int i = 0; i < nb_sommets; i++)
              {
                double x = sommets(i,0);
                double y = sommets(i,1);
                if (axi)
                  {
                    double r = x;
                    double theta = y;
                    x = r * cos(theta);
                    y = r * sin(theta);
                  }
                sommets2(i,0) = x;
                sommets2(i,1) = y;
                sommets2(i,2) = z;
              }
            Format_Post_Lata::write_doubletab(fichier_geom, sommets2, nb_col, option,
                                              bloc_number, 2);
          }

      }
    else
      {

        if (axi)
          {
            DoubleTab sommets2(sommets);
            for (int i = 0; i < nb_sommets; i++)
              {
                double r = sommets(i,0);
                double theta = sommets(i,1);
                double z = sommets(i,2);
                sommets2(i,0) = r*cos(theta);
                sommets2(i,1) = r*sin(theta);
                sommets2(i,2) = z;
              }
            Format_Post_Lata::write_doubletab(fichier_geom, sommets2, nb_col,option);
          }
        else
          {
            Format_Post_Lata::write_doubletab(fichier_geom, sommets, nb_col,option);
          }
      }
    // Elements
    // Les indices de sommets, elements et autres dans le fichier lata commencent a 1 :
    const int nb_elem = elements.dimension(0);
    if (option == SINGLE_FILE && Process::nproc() > 1)
      {
        // Tous les processeurs ecrivent dans le meme fichier. Il faut renumeroter les
        // sommets des elements pour avoir une numerotation globale.
        // On ajoute un pour numerotation fortran
        decalage_sommets = 1 + mppartial_sum(nb_sommets);
        decalage_elements = 1 + mppartial_sum(nb_elem);
      }
    else
      {
        // Chaque processeur ecrit dans un fichier separe avec des indices locaux.
        // On ajoute un pour numerotation fortran
        decalage_sommets = 1;
        decalage_elements = 1;
      }
    if (dim==2)
      {
        // Decalage a ajouter a un indice de sommet pour passer de la rangee z=0
        // a la rangee z=1
        int decalage_z1 = nb_sommets;
        if (option == SINGLE_FILE && Process::nproc() > 1)
          decalage_z1 = mp_sum(nb_sommets);
        const int nsomelem = (elements.nb_dim() > 1) ? elements.dimension(1) : 1;
        IntTab elem2(nb_elem, nsomelem*2);
        for (int i = 0; i < nb_elem; i++)
          {
            for (int j = 0; j < nsomelem; j++)
              {
                const int s = elements(i,j);
                elem2(i,j) = s;
                elem2(i,j + nsomelem) = s + decalage_z1;
              }
          }
        Format_Post_Lata::write_inttab(fichier_geom, 1,decalage_sommets, elem2, nb_col, option);
      }
    else
      {
        Format_Post_Lata::write_inttab(fichier_geom, 1,decalage_sommets, elements, nb_col, option);
      }
  }
  // En mode parallele, on ecrit en plus des fichiers contenant les donnees paralleles
  // sur les sommets, les elements et les faces...
  // On ecrit dans le fichier maitre ceci :
  //  joints_sommets NPROC FICHIER_NB_SOMMETS FICHIER_NB_SOMMETS_JOINTS FICHIER_JOINTS
  // NPROC est le nombre de parties
  // FICHIER_NB_SOMMETS est le nom du fichier contenant, pour chaque proc, le nombre de
  //  sommets sur ce processeur
  // FICHIER_JOINTS est le nom du fichier contenant les joints
  // Dans FICHIER_NB_SOMMETS, on a un tableau contenant NPROC entiers, le nombre de sommets
  //  ecrits par chaque processeur
  // Dans FICHIER_NB_SOMMETS_JOINTS, on a un tableau de NPROC entiers contenant pour chaque
  //  proc le nombre de sommets de joint
  // Dans FICHIER_JOINTS on a un tableau contenant les sommets de joint de chaque processeur,
  //  en numerotation globale : pour chaque sommet, indice du sommet local, indice du sommet
  //  equivalent sur une autre sous-partie, numero de la sous-partie
  Nom filename_decal_som;
  Nom filename_decal_elem;
  if (option == SINGLE_FILE && Process::nproc() > 1)
    {
      Nom extension;
      extension = extension_lata();
      extension += ".";
      extension += "decal_som_proc";
      extension += ".";
      extension += id_domaine;

      {
        Fichier_Lata fichier_champ(basename, extension,
                                   Fichier_Lata::ERASE, format, option);
        filename_decal_som = fichier_champ.get_filename();
        int nb_col = -1; // non utilise
        IntTab data(1,2);
        data(0, 0) = decalage_sommets;
        data(0, 1) = sommets.dimension(0);
        Format_Post_Lata::write_inttab(fichier_champ,0, 0 /* ne pas ajouter de decalage */,
                                       data, nb_col, option);
      }

      extension = extension_lata();
      extension += ".";
      extension += "decal_elem_proc";
      extension += ".";
      extension += id_domaine;

      {
        Fichier_Lata fichier_champ(basename, extension,
                                   Fichier_Lata::ERASE, format, option);
        filename_decal_elem = fichier_champ.get_filename();
        int nb_col = -1; // non utilise
        IntTab data(1,2);
        data(0, 0) = decalage_elements;
        data(0, 1) = elements.dimension(0);
        Format_Post_Lata::write_inttab(fichier_champ,0, 0 /* ne pas ajouter de decalage */,
                                       data, nb_col, option);
      }
    }

  Fichier_Lata_maitre fichier_lata(basename, extension_lata(),
                                   Fichier_Lata::APPEND,option);
  SFichier& sfichier = fichier_lata.get_SFichier();

  int nb_som_tot = sommets.dimension(0);
  int nb_elem_tot = elements.dimension(0);
  if (option == SINGLE_FILE && Process::nproc() > 1)
    {
      nb_som_tot = mp_sum(nb_som_tot);
      nb_elem_tot = mp_sum(nb_elem_tot);
    }
  if (fichier_lata.is_master())
    {
      sfichier << "Geometrie " << id_domaine;
      sfichier << " " << remove_path(nom_fichier_geom) << finl;
      switch(format)
        {
        case ASCII:
          sfichier << "Format=ascii" << finl;
          break;
        case BINAIRE:
          sfichier << "Format=binary" << finl;
          break;
        default:
          Cerr << "Error in Format_Post_Lata_V1::ecrire_entete\n"
               << " format not supported" << finl;
          exit();
        }
      sfichier << "nb_som_tot=" << nb_som_tot << finl;
      sfichier << "type_elem=" << type_elem << finl;
      sfichier << "nb_elem_tot=" << nb_elem_tot << finl;
      if (option == SINGLE_FILE && Process::nproc() > 1)
        {
          const int nproc = Process::nproc();
          sfichier << "joints_sommets nproc=" << nproc << " decalage=" << remove_path(filename_decal_som) << finl;
          sfichier << "joints_elements nproc=" << nproc << " decalage=" << remove_path(filename_decal_elem) << finl;
        }
    }
  fichier_lata.syncfile();
  return 1;
}

int Format_Post_Lata_V1::ecrire_entete_lata_V1(const Nom& basename,const Options_Para& option,const int& est_le_premier_post)
{

  if (est_le_premier_post)
    {

      // Effacement du fichier .lata et ecriture de l'entete
      Fichier_Lata_maitre fichier(basename, extension_lata(),
                                  Fichier_Lata::ERASE,
                                  option);

      SFichier& sfichier = fichier.get_SFichier();
      if (fichier.is_master())
        {
          sfichier << "Trio_U Version1.4.9 06/2005" << finl;
          sfichier << Objet_U::nom_du_cas() << finl;
          sfichier << "Trio_U" << finl;
        }
      fichier.syncfile();

    }

  return 1;
}

int Format_Post_Lata_V1::finir_lata_V1(const Nom& basename,const Options_Para& option,const int& est_le_dernier_post)
{

  if (est_le_dernier_post)
    {
      Fichier_Lata_maitre fichier(basename, extension_lata(),
                                  Fichier_Lata::APPEND,
                                  option);
      SFichier& sfichier = fichier.get_SFichier();
      if (fichier.is_master())
        {
          sfichier << "FIN" << finl;

        }
      fichier.syncfile();
    }
  return 1;
}

int Format_Post_Lata_V1::ecrire_champ_lata_V1(const Nom&   id_du_champ,
                                              const Nom&   id_du_domaine,
                                              const Nom&   localisation,
                                              const DoubleTab& data,const double& temps,const Nom& basename,
                                              const Format& format, const Options_Para& option)
{
  // Verification que le nom du champ correspond bien au
  // nombre de coposante du champ grace a latav1_component_shape()
  // Cette partie de codage pourra etre supprime quand le plugin
  // LATA sera en mesure de gerer cela lui-meme (V2 ?)
  int nb_comp,nb_comp_du_champ;
  {
    int shape=1;
    shape=latav1_component_shape(id_du_champ);

    Nom avec_s;
    avec_s="";
    nb_comp=1;
    if (shape<0)
      {
        avec_s="s";
        if (shape==-1)
          nb_comp=dimension;
        else if (dimension==3)
          nb_comp=dimension;
      }
    else
      nb_comp=shape;

    nb_comp_du_champ=data.nb_dim();
    if (nb_comp_du_champ>1)
      nb_comp_du_champ=data.dimension(1);

    // On exclus tous les cas ou le champ qui est postraite aux faces, est scalaire mais interprete comme vectoriel
    // Typiquement le cas de la vitesse aux faces en VDF : nb_comp = 2 ou 3 et nb_comp_du_champ = 1 car 1 seul vitesse normal par face.
    if ((Motcle)localisation=="FACES" && nb_comp_du_champ<nb_comp)
      nb_comp_du_champ=nb_comp;

    Nom avec_ss;
    avec_ss="";
    if (nb_comp_du_champ>1)
      avec_ss="s";
  }

  //Modification de id_du_champ car l interpolation modifie le nom (ajout de elem_nom_du_domaine
  //ou som_nom_du_domaine, on supprime cette extension)
  Motcle id_du_champ_modifie(id_du_champ);
  Motcle iddomaine(id_du_domaine);

  if (nb_comp!=nb_comp_du_champ)
    {
      id_du_champ_modifie=Nom(nb_comp_du_champ);
      id_du_champ_modifie+="_";
      id_du_champ_modifie+=id_du_champ;
    }
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

  if ((Motcle)id_du_champ_modifie=="VITESSEX")
    id_du_champ_modifie="VITESSEX";
  if ((Motcle)id_du_champ_modifie=="VITESSEY")
    id_du_champ_modifie="VITESSEY";

  if ((Motcle)id_du_champ_modifie=="EPS")
    id_du_champ_modifie="EPS";

  const char * point = ".";
  // Construction du nom du fichier
  Nom basename_champ(basename);
  Nom extension_champ(extension_lata());
  extension_champ += point;
  extension_champ += id_du_champ_modifie;
  extension_champ += point;
  Motcle loc = localisation;

  if (loc.debute_par("SOM"))
    {
      extension_champ += "SOM";
    }
  else if (loc.debute_par("ELEM"))
    {
      extension_champ += "ELEM";
    }
  else if (loc.debute_par("FACES"))
    {
      extension_champ += "FACES";
    }
  else
    {
      return 0;
    }
  extension_champ += point;
  extension_champ += id_du_domaine;
  extension_champ += point;
  extension_champ += "pb";
  extension_champ += point;
  char str_temps[100] = "0.0";
  if (temps >= 0.)
    sprintf(str_temps, "%.10f", temps);
  extension_champ += str_temps;

  Nom filename_champ;
  {
    Fichier_Lata fichier_champ(basename_champ, extension_champ,
                               Fichier_Lata::ERASE, format,option);
    filename_champ = fichier_champ.get_filename();

    if (data.nb_dim() == 1)
      {
        int nb_colonnes;
        Format_Post_Lata::write_doubletab(fichier_champ,
                                          data,
                                          nb_colonnes,
                                          option);
      }
    else
      {
        // Pour compatibilite avec la version precedente du module lata :
        // si le champ a plusieurs composantes,
        // on ecrit :
        //   valeurs(0,0) valeurs(1,0) valeurs(2,0) ...
        //   valeurs(0,1) valeurs(1,1) valeurs(2,1) ...
        const int nb_lignes = data.dimension(0);
        const int nb_colonnes = data.dimension(1);
        DoubleTab data_slice(nb_lignes);
        // Pour voir data comme un tableau unidimensionnel :
        if (data.nb_dim() != 2)
          {
            Cerr << "Error : Format_Post_Lata_V1::ecrire_champ_lata_V1 not coded for dimensions > 2" << finl;
            exit();
          }
        for (int colonne = 0; colonne < nb_colonnes; colonne++)
          {
            for (int i = 0; i < nb_lignes; i++)
              data_slice[i] = data(i, colonne);
            int dummy;
            Format_Post_Lata::write_doubletab(fichier_champ,
                                              data_slice,
                                              dummy,
                                              option,
                                              colonne, nb_colonnes);
          }
      }
  }
  // Ouverture du fichier .lata en mode append.
  // Ajout de la reference au champ

  Fichier_Lata_maitre fichier(basename, extension_lata(),
                              Fichier_Lata::APPEND, option);
  SFichier& sfichier = fichier.get_SFichier();
  if (fichier.is_master())
    {
      sfichier << "Champ ";
      sfichier << id_du_champ_modifie << " ";
      sfichier << remove_path(filename_champ) << finl;
    }
  fichier.syncfile();
  return 1;
}

