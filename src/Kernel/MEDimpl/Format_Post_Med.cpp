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
// File:        Format_Post_Med.cpp
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////
#include <Format_Post_Med.h>
#include <Comm_Group.h>
#include <EcrMED.h>
#include <EcrFicPartage.h>
#include <EFichier.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Format_Post_Med,"Format_Post_Med",Format_Post_base);


/////////////////////////////////////////////////////////////////////////////
//Parametre: med_basename_
// Signification: debut du nom du fichier
//Extension med

//Specificites de ce format
//Generation d un fichier format.med.data exploitable pour reecrire les donnees a un autre format
//-Methodes completer_post_med(...) et preparer_post_med(...) pour generer les fichiers
//-Metode finir_med(...) pour concatener les fichiers crees
///////////////////////////////////////////////////////////////////////////////////////

// Description:
//  Constructeur par defaut:

//Specifier dans commentaire ce qui est fixe par defaut
Format_Post_Med::Format_Post_Med()
{
  reset();
}

// Description:
//  Remet l'objet dans l'etat obtenu par le constructeur par defaut.
void Format_Post_Med::reset()
{
  med_basename_ = "??";
}

// Description: erreur => exit
Sortie& Format_Post_Med::printOn(Sortie& os) const
{
  Cerr << "Format_Post_Meshtv::printOn : error" << finl;
  exit();
  return os;
}

// Description: Lecture des parametres du postraitement au format "jeu de donnees"
//  Le format attendu est le suivant:
//  {
//    nom_fichier filename_sans_extension
//  }
Entree& Format_Post_Med::readOn(Entree& is)
{
  Format_Post_base::readOn(is);
  return is;
}

void Format_Post_Med::set_param(Param& param)
{
  param.ajouter("nom_fichier",&med_basename_,Param::REQUIRED);
}

// Description: Initialisation de la classe avec des parametres par
//  defaut
int Format_Post_Med::initialize_by_default(const Nom& file_basename)
{
  med_basename_= file_basename;

  return 1;
}

int Format_Post_Med::initialize(const Nom& file_basename, const int& format, const Nom& option_para)
{
  med_basename_= file_basename;

  return 1;
}

EcrMED Format_Post_Med::getEcrMED() const
{
  EcrMED e;
  e.setMajorMode(false);
  return e;
}

int Format_Post_Med::ecrire_entete(double temps_courant,int reprise,const int& est_le_premier_post)
{

  Nom nom_fich(med_basename_);
  nom_fich +=".";
  Nom format="med";
  nom_fich += format;
  nom_fich += ".index";

  ecrire_entete_med(nom_fich,est_le_premier_post);
  return 1;

}

int Format_Post_Med::finir(int& est_le_dernier_post)
{
  return 1;
  Nom nom_fich(med_basename_);
  nom_fich +=".";
  Nom format="med";
  nom_fich += format;
  nom_fich += ".index";

  finir_med(nom_fich,est_le_dernier_post);

  return 1;
}


int Format_Post_Med::completer_post(const Domaine& dom,const int is_axi,
                                    const Nature_du_champ& nature,const int nb_compo,const Noms& noms_compo,
                                    const Motcle& loc_post,const Nom& le_nom_champ_post)
{
  return 1;
  Nom nom_fich2(med_basename_);
  Nom format="med";
  nom_fich2 += ".";
  nom_fich2 += format;
  nom_fich2 += ".med1";

  Nom nom1(le_nom_champ_post);
  nom1 += "_";
  Nom nom2(Nom(loc_post).majuscule());
  nom1 += nom2;

  Nom nom_post(le_nom_champ_post);
  if (loc_post=="SOM")
    {
      nom_post.prefix(dom.le_nom());
      nom_post.prefix("_som_");
      nom_post.prefix("_SOM_");
    }
  else if (loc_post=="ELEM")
    {
      nom_post.prefix(dom.le_nom());
      nom_post.prefix("_ELEM_");
      nom_post.prefix("_elem_");
    }
  nom_post+="_";
  nom_post+=nom2;

  completer_post_med(nom_fich2,nom_post,nom2);

  return 1;
}

int Format_Post_Med::preparer_post(const Nom& id_du_domaine,const int& est_le_premier_post,
                                   const int& reprise,
                                   const double& t_init)
{
  return 1;
  Nom nom_fich1(med_basename_);
  Nom nom_fich2(med_basename_);
  Nom nom_fich3(med_basename_);


  Nom format="med";
  nom_fich1 += ".";
  nom_fich1 += format;
  nom_fich1 += ".med0";

  nom_fich2 += ".";
  nom_fich2 += format;
  nom_fich2 += ".med1";

  nom_fich3 += ".";
  nom_fich3 += format;
  nom_fich3 += ".med2";

  preparer_post_med(nom_fich1,nom_fich2,nom_fich3,id_du_domaine,est_le_premier_post);
  return 1;

}


// Description:
//  voir Format_Post_base::ecrire_domaine
int Format_Post_Med::ecrire_domaine(const Domaine& domaine,const int& est_le_premier_post)
{
  Nom nom_fich(med_basename_);
  nom_fich +=".";
  Nom format="med";
  nom_fich += format;
  nom_fich += ".index";

  Nom nom_fic_base(med_basename_);
  nom_fic_base += ".";
  nom_fic_base += format;
  Nom nom_fic=nom_fic_base.nom_me(Process::me());

  ecrire_domaine_med(domaine,nom_fic,est_le_premier_post,nom_fich);

  return 1; // ok tout va bien
}

// Description:
//  voir Format_Post_base::ecrire_domaine
int Format_Post_Med::ecrire_faces_domaine(const Domaine& domaine, const Zone_dis_base& zone_dis_base)
{
  Nom format="med";
  Nom nom_fic_base(med_basename_);
  nom_fic_base += ".";
  nom_fic_base += format;
  Nom nom_fic=nom_fic_base.nom_me(Process::me());

  EcrMED ecr_med(getEcrMED());
  ecr_med.ecrire_faces_domaine(nom_fic,domaine,zone_dis_base);
  return 1; // ok tout va bien
}

// Description: commence l'ecriture d'un nouveau pas de temps
//  Ouvre le fichier maitre en mode APPEND et ajoute une ligne
//   "TEMPS xxxxx" si ce temps n'a pas encore ete ecrit
int Format_Post_Med::ecrire_temps(const double temps)
{
  Nom nom_fich(med_basename_);
  nom_fich +=".";
  Nom format="med";
  nom_fich += format;
  nom_fich += ".index";

  ecrire_temps_med(temps,nom_fich);

  return 1;
}

// Description:
//  voir Format_Post_base::ecrire_champ
int Format_Post_Med::ecrire_champ(const Domaine& domaine,const Noms& unite_,const Noms& noms_compo,
                                  int ncomp,double temps_,double temps_courant,
                                  const Nom& id_champ,
                                  const Nom& id_du_domaine,
                                  const Nom& localisation,
                                  const Nom&   nature,
                                  const DoubleTab& valeurs)
{

  //Attention au sens du compteur

  Nom nom_fich_index(med_basename_);
  nom_fich_index +=".";
  Nom format="med";
  nom_fich_index += format;
  nom_fich_index += ".index";

  Nom nom_fich_ecrit(med_basename_);
  nom_fich_ecrit += ".";
  nom_fich_ecrit += format;

  ecrire_champ_med(domaine,unite_,noms_compo,ncomp,temps_,nom_fich_ecrit,id_champ,id_du_domaine,localisation,valeurs,nom_fich_index);

  return 1;
}

// Description:

int Format_Post_Med::ecrire_item_int(const Nom&     id_item,
                                     const Nom&     id_du_domaine,
                                     const Nom&     id_zone,
                                     const Nom&     localisation,
                                     const Nom&     reference,
                                     const IntVect& valeurs,
                                     const int   reference_size)
{

  return 1;
}

int Format_Post_Med::ecrire_entete_med(Nom& nom_fich,const int& est_le_premier_post)
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

int Format_Post_Med::finir_med(Nom& nom_fich,int& est_le_dernier_post)
{
  ////Nom nom_fichier_base = nom_fich.prefix(".med.index");
  Nom nom_fichier_base = nom_fich;
  nom_fichier_base.prefix(".med.index");
  Nom nom_fichier(nom_fichier_base);
  Nom format = "med";
  nom_fichier += ".";
  nom_fichier += format;

  if (est_le_dernier_post)
    {
      if (je_suis_maitre())
        {

          // on veut concatainer les 3 fichiers
          SFichier file3(nom_fichier+".data");
          SFichier file("postmed.data");
          for (int i=0; i<3; i++)
            {
              Nom num(i);
              Nom fic2(nom_fichier);
              fic2+=".med";
              fic2+=num;
              {
                EFichier file2(fic2);
                Nom motlu;
                file2>>motlu;
                while (!file2.eof())
                  {
                    file<<motlu<<" "<<finl;
                    if (motlu!="#")
                      file3<<motlu<<" "<<finl;
                    file2>>motlu;

                  }
              }

            }

        }

      EcrFicPartage s;
      s.ouvrir(nom_fich,ios::app);

      if (Process::je_suis_maitre())
        {
          s << "FIN" << finl;
        }

      s.flush();
      s.syncfile();

    }

  return 1;
}

int Format_Post_Med::completer_post_med(const Nom& nom_fich2,const Nom& nom1,const Nom& nom2)
{

  if (je_suis_maitre())
    {
      SFichier file;
      file.ouvrir(nom_fich2,ios::app);
      file <<nom1<<" "<<nom2<<finl;
    }

  return 1;
}

int Format_Post_Med::preparer_post_med(const Nom& nom_fich1,const Nom& nom_fich2,const Nom& nom_fich3,
                                       const Nom& id_du_domaine,const int& est_le_premier_post)
{
  Nom nom_fich(nom_fich1);
  nom_fich.prefix(".med0");

  if (je_suis_maitre())
    {

      if (est_le_premier_post)
        {
          SFichier file(nom_fich1);
          file<<"{ Dimension "<<dimension<<finl<<"# export Domaine "<<id_du_domaine<<finl;
        }
      else
        {
          SFichier file(nom_fich1,ios::app);
          file<<" export Domaine "<<id_du_domaine<<finl;
        }

      SFichier file;
      if (est_le_premier_post)
        {
          file.ouvrir(nom_fich2);
          file<<"# \n Pbc_MED pbmed\n Lire pbmed { "<<finl;
        }
      else
        {
          file.ouvrir(nom_fich2,ios::app);
          file<<"} } } , "<<finl;
        }
      file<<nom_fich<<" "<<id_du_domaine<<"  { Postraitement { Champs dt_post 1e-9 {"<<finl;

      if (est_le_premier_post)
        {
          SFichier file2(nom_fich3);
          file2<<"} } } } }"<<finl;
        }
    }


  return 1;
}

int Format_Post_Med::ecrire_domaine_med(const Domaine& domaine,const Nom& nom_fic,const int& est_le_premier_post,Nom& nom_fich)
{

  //Cerr<<"We want to postprocess with MED "<<est_le_premier_post<<finl;
  EcrMED ecr_med(getEcrMED());

  // Cerr<<"We want to postprocess with MED in domain"<<nom_fic<<finl;
  // Cerr<<"We want to postprocess with MED"<<domaine.le_nom()<<finl;

  EcrFicPartage s;
  s.ouvrir(nom_fich,ios::app);

  int dim = domaine.les_sommets().dimension(1);

  int mode=-1;
  if (est_le_premier_post==0)  mode=0;
  s<<"dimension: "<<dim<<finl;
  s<<"domaine: "<<domaine.le_nom()<<finl;
  s<<"nb_proc: "<<Process::nproc()<<finl;
  ecr_med.ecrire_domaine(nom_fic,domaine,domaine.le_nom(),mode);
  // Cerr<<"We want to postprocess with MED"<<domaine.le_nom()<<"end"<<finl;

  s.flush();
  s.syncfile();

  return 1;

}

int Format_Post_Med::ecrire_temps_med(const double temps,Nom& nom_fich)
{
  EcrFicPartage s;
  s.ouvrir(nom_fich,ios::app);

  if (je_suis_maitre())
    s << "TEMPS " << temps << finl;

  s.flush();
  s.syncfile();

  return 1;
}

int Format_Post_Med::ecrire_champ_med(const Domaine& dom,const Noms& unite_, const Noms& noms_compo,
                                      int ncomp, double temps_,const Nom& nom_pdb,
                                      const Nom&   id_du_champ,
                                      const Nom&   id_du_domaine,
                                      const Nom&   loc_post,
                                      const DoubleTab& valeurs,Nom& nom_fich)
{
  //compteur est declare en variable locale car toujours fixe a 1
  int compteur;
  //Ouverture du fichier

  EcrFicPartage os;
  os.ouvrir(nom_fich, ios::app);

  Nom fic = nom_pdb.nom_me(me());

  //Ce compteur est en fait anciennement passe sous est_le_premier_post_pour_nom_fich_
  //et n est pas reellement utilise
  compteur = 1;
  Nom nom_post(id_du_champ);
  if (ncomp != -1)
    {
      nom_post = noms_compo[ncomp];
    }
  nom_post.prefix(dom.le_nom());
  if (loc_post == "SOM")
    {
      nom_post.prefix("_som_");
      nom_post.prefix("_SOM_");
    }
  else if (loc_post == "ELEM")
    {
      nom_post.prefix("_ELEM_");
      nom_post.prefix("_elem_");
    }
  if (je_suis_maitre())
    os << "champ: " << nom_post << " " << dom.le_nom() << " " << loc_post << finl;
  os.syncfile();
  EcrMED ecr_med(getEcrMED());
  Nom nom_dom = dom.le_nom();
  Nom nom_dom_inc = dom.le_nom();
  Nom type_elem = dom.zone(0).type_elem()->que_suis_je();;

  if (loc_post == "SOM")
    ecr_med.ecrire_champ("CHAMPPOINT", fic, dom, id_du_champ, valeurs, unite_, type_elem, temps_, compteur);
  else if (loc_post == "ELEM")
    ecr_med.ecrire_champ("CHAMPMAILLE", fic, dom, id_du_champ, valeurs, unite_, type_elem, temps_, compteur);
  else if (loc_post == "FACES")
    ecr_med.ecrire_champ("CHAMPFACES", fic, dom, id_du_champ, valeurs, unite_, type_elem, temps_, compteur);
  else if (loc_post == "FACE")
    {
      if (nom_dom_inc != nom_dom)
        Cerr << "We do not know to postprocess " << id_du_champ
             << " with the keyword " << loc_post << " on different domains " << finl;
    }
  else
    {
      Cerr << "We do not know to postprocess " << id_du_champ
           << " with the keyword " << loc_post << finl;
      return -1;
    }
  os.syncfile();
  return 1;
}





