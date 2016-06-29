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
// File:        Sonde.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/77
//
//////////////////////////////////////////////////////////////////////////////


#include <Sonde.h>
#include <Postraitement.h>
#include <Zone_VF.h>
#include <sys/stat.h>
#include <communications.h>
#include <Champ_Generique_Interpolation.h>
#include <Entree_complete.h>
//#include <Ch_Fonc_P0.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(Sonde,"Sonde",Objet_U);
Sonde::Sonde() {  }

static int fichier_sondes_cree=0;

// Description:
//    Imprime le type de l'objet sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Sonde::printOn(Sortie& s ) const
{
  return s << que_suis_je();
}

void Sonde::completer()
{
  // Recherche du champ sonde
  // Remplissage de la reference au champ

  //On devrait acceder au domaine par le champ generique
  //Mais reference pas encore faite
  Probleme_base& Pb = mon_post->probleme();
  const Motcle& nom_domaine = Pb.domaine().le_nom();

  Motcle nom_champ_ref;
  Noms liste_noms;
  Pb.get_noms_champs_postraitables(liste_noms);
  Motcles mots_compare(liste_noms.size());
  for (int i=0; i<liste_noms.size(); i++)
    mots_compare[i] = liste_noms[i];

  Motcle nom_macro=nom_champ_lu_;
  Motcle nom_macro_test, nom1("_not_def"), nom2("_not_def");
  if (!(mots_compare.contient_(nom_champ_lu_)||mon_post->comprend_champ_post(nom_champ_lu_)))
    {
      if (nom_champ_lu_.debute_par("MOYENNE_"))
        nom_macro = nom_macro.suffix("MOYENNE_");
      else if (nom_champ_lu_.debute_par("ECART_TYPE_"))
        nom_macro = nom_macro.suffix("ECART_TYPE_");

      if (nom_champ_lu_.debute_par("CORRELATION_"))
        {
          nom_macro_test = nom_champ_lu_;
          nom_macro_test = nom_macro_test.suffix("CORRELATION_");
          for (int i=0; i<mots_compare.size(); i++)
            {
              nom_macro_test = nom_macro_test.suffix(mots_compare[i]);
              if (nom_macro_test.debute_par("_"))
                {
                  nom1 = mots_compare[i];
                  nom2 = nom_macro_test.suffix("_");
                }
            }
        }
    }

  if (mots_compare.contient_(nom_macro))
    {
      if (nom_macro==Motcle(nom_champ_lu_))
        {
          REF(Champ_base) champ_ref = Pb.get_champ(nom_champ_lu_);
          const Nom& nom_champ_base = champ_ref->le_nom();
          const Noms& compos_base = champ_ref->noms_compo();
          int ncomposante = Champ_Generique_base::composante(nom_champ_lu_,nom_champ_base,compos_base,champ_ref->get_synonyms());

          if (ncomposante==-1)
            nom_champ_ref = nom_champ_base +"_natif_"+ nom_domaine;
          else
            {
              Nom nume(ncomposante);
              nom_champ_ref = nom_champ_base+"_natif_"+ nom_domaine+nume;
            }
        }
      else
        nom_champ_ref = nom_champ_lu_+"_natif_"+nom_domaine;
    }
  else
    {
      //Cas des correlations
      if ((mots_compare.contient_(nom1)) && (mots_compare.contient_(nom2)))
        {
          nom_champ_ref = "Correlation_";
          nom_champ_ref += nom1+"_natif_"+nom_domaine+"_"+nom2+"_natif_"+nom_domaine;
        }
      else
        //Cas d un champ generique general
        nom_champ_ref = nom_champ_lu_;
    }

  mon_champ = mon_post->get_champ_post(nom_champ_ref);

  // Remplissage de l'attribut ncomp (il vaut -1 par defaut)

  const Noms nom_champ = mon_champ->get_property("nom");
  const Noms noms_comp = mon_champ->get_property("composantes");
  ncomp = Champ_Generique_base::composante(nom_champ_ref,nom_champ[0],noms_comp,mon_champ->get_property("synonyms"));

  initialiser();
}
// Description:
//    Lit les specifications d'une sonde a partir d'un flot d'entree.
//    Format:
//    Sondes
//     {
//       [nom_sonde nom_champ Periode dts Points n x1 y1 [z1] ... xn yn [zn]
//       [nom_sonde nom_champ Periode dts Segment ns x1 y1 [z1] x2 y2 [z2]
//       ...
//     }
// Precondition: le postraitement doit avoir ete associe
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: donnees de la sonde non definies
// Exception: erreur de format, mot clef inconnus
// Exception: donnees de la sonde pas definies correctement
// Effets de bord:
// Postcondition:
Entree& Sonde::readOn(Entree& is)
{
  assert(mon_post.non_nul());
  Motcle motlu;
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  int nbre_points;
  // initialisation de periode par defaut
  periode = 1.e10;
  // initialisation de periode-nodes-chsom-grav par defaut
  nodes=0;
  chsom=0;
  grav=0;
  som=0;
  dim=-1;
  numero_elem_=-1;
  is >> motlu;
  // Lecture d'un mot cle qui n'est pas le champ
  if (motlu=="nodes")
    {
      nodes=1;
      is >> motlu;
    }
  else if (motlu=="chsom")
    {
      chsom=1;
      is >> motlu;
    }
  else if (motlu=="grav")
    {
      grav=1;
      is >> motlu;
    }
  else if (motlu=="som")
    {
      som=1;
      is >> motlu;
    }
  // Affectation du nom du champ
  nom_champ_lu_ = motlu;

  //Creation des Champ_Generique_refChamp necessaire pour l initialisation de la REF a Champ_Generique_base
  //Si le champ demande est un Champ_base connu du probleme on cree le Champ_Generique_refChamp correspondant

  Noms liste_noms;
  Probleme_base& Pb = mon_post->probleme();
  Pb.get_noms_champs_postraitables(liste_noms);

  Motcles mots_compare(liste_noms.size());
  for (int i=0; i<liste_noms.size(); i++)
    mots_compare[i] = liste_noms[i];

  if (mots_compare.contient_(nom_champ_lu_))
    {
      Pb.creer_champ(nom_champ_lu_);
      //On va creer un Champ_Generique_refChamp dont le nom a pour base
      //le nom du champ auquel on fait reference et non pas une composante de ce champ
      REF(Champ_base) champ_ref = Pb.get_champ(nom_champ_lu_);
      const Nom& le_nom_champ = champ_ref->le_nom();
      const Motcle& nom_domaine = Pb.domaine().le_nom();
      Motcle identifiant;

      identifiant = Motcle(le_nom_champ)+"_natif_"+nom_domaine;
      if (!mon_post->comprend_champ_post(identifiant))
        {
          mon_post->creer_champ_post(le_nom_champ,"natif",is);
        }
    }
  else
    {
      Nom expression;
      int is_champ_predefini = Pb.expression_predefini(motlu,expression);
      if ((is_champ_predefini) && (!mon_post->comprend_champ_post(nom_champ_lu_)))
        {
          Champ_Generique champ;
          Entree_complete s_complete(expression,is);
          s_complete>>champ;
          mon_post->complete_champ(champ,nom_champ_lu_);
        }
    }

  // Lecture des caracteristiques de la sonde
  IntVect fait(2);

  Motcles les_motcles(12);
  {
    les_motcles[0] = "periode";
    les_motcles[1] = "point";
    les_motcles[2] = "points";
    les_motcles[3] = "segment";
    les_motcles[4] = "plan";
    les_motcles[5] = "volume";
    les_motcles[6] = "segmentxdx";
    les_motcles[7] = "planxdxdy";
    les_motcles[8] = "circle";
    les_motcles[9] = "position_like";
    les_motcles[10] = "numero_elem_sur_maitre";
    les_motcles[11] = "segmentpoints";
  }

  while ((fait(0) != 1) || (fait(1) != 1))
    {
      is >> motlu;
      if (motlu == accolade_fermee)
        {
          Cerr << "Error while reading the probe " << nom_ <<finl;
          Cerr << "The data of the probe were not defined" << finl;
          exit();
        }
      int rang=les_motcles.search(motlu);
      if (rang == -1)
        {
          Cerr << "Error while reading the probe " << nom_ <<finl;
          Cerr << motlu << " is not understood; the keywords understood are : " << finl;
          Cerr << les_motcles;
          exit();
        }

      switch(rang)
        {
        case 0:
          {
            is >> periode;
            break;
          }
        case 1:
        case 2:
        case 11:
          {
            type_ = les_motcles[rang];
            rang = 1;
            dim = 0;
            is >> nbre_points;
            les_positions_.resize(nbre_points,dimension);

            for (int i=0; i<nbre_points; i++)
              for (int j=0; j<dimension; j++)
                is >> les_positions_(i,j);

            break;
          }
        case 10:
          {
            type_ = les_motcles[rang];
            rang=1;
            dim=0;
            is >> numero_elem_;
            les_positions_.resize(1,dimension);
            break;
          }
        case 3:
        case 6:
          {
            type_ = les_motcles[rang];
            int rang2=rang;
            rang = 1;
            dim = 1;
            DoubleVect origine(dimension);
            DoubleVect extremite(dimension);
            DoubleVect dx(dimension);
            int i=0,j=0;
            is >> nbre_points;
            les_positions_.resize(nbre_points,dimension);

            for (; i<dimension; i++)
              {
                is >> origine(i);
                type_+=" ";
                type_+=(Nom)origine(i);
              }
            for (i=0; i<dimension; i++)
              {
                is >> extremite(i);
                type_+=" ";
                type_+=(Nom)extremite(i);
              }
            for (i=0; i<dimension; i++)
              if (rang2==6)
                dx(i)=(extremite(i))/(nbre_points-1);
              else
                dx(i)=(extremite(i)-origine(i))/(nbre_points-1);
            for (i=0; i<nbre_points; i++)
              for (j=0; j<dimension; j++)
                les_positions_(i,j)=origine(j)+i*dx(j);
            break;
          }
        case 4:
        case 7:
          {
            type_ = les_motcles[rang];
            int rang2=rang;
            rang = 1;
            dim = 2;
            DoubleVect origine(dimension);
            DoubleVect extremite1(dimension);
            DoubleVect extremite2(dimension);
            DoubleVect dx1(dimension);
            DoubleVect dx2(dimension);
            int i=0,j=0,k=0;
            is >> nbre_points1;
            is >> nbre_points2;
            nbre_points=nbre_points1*nbre_points2;
            les_positions_.resize(nbre_points,dimension);

            for (; i<dimension; i++)
              is >> origine(i);
            for (i=0; i<dimension; i++)
              is >> extremite1(i);
            for (i=0; i<dimension; i++)
              is >> extremite2(i);
            if (rang2==7)
              {
                for (i=0; i<dimension; i++)
                  dx1(i)=(extremite1(i))/(nbre_points1-1);
                for (i=0; i<dimension; i++)
                  dx2(i)=(extremite2(i))/(nbre_points2-1);
              }
            else
              {
                for (i=0; i<dimension; i++)
                  dx1(i)=(extremite1(i)-origine(i))/(nbre_points1-1);
                for (i=0; i<dimension; i++)
                  dx2(i)=(extremite2(i)-origine(i))/(nbre_points2-1);
              }
            for (i=0; i<nbre_points1; i++)
              for (j=0; j<nbre_points2; j++)
                for (k=0; k<dimension; k++)
                  les_positions_(i*nbre_points2+j,k)=origine(k)+i*dx1(k)+j*dx2(k);
            break;
          }

        case 5:
          {
            type_ = les_motcles[rang];
            rang = 1;
            dim = 3;
            ArrOfDouble origine(dimension);
            ArrOfDouble extremite1(dimension);
            ArrOfDouble extremite2(dimension);
            ArrOfDouble extremite3(dimension);
            ArrOfDouble dx1(dimension);
            ArrOfDouble dx2(dimension);
            ArrOfDouble dx3(dimension);
            int i=0,j=0,k=0;
            is >> nbre_points1;
            is >> nbre_points2;
            is >> nbre_points3;
            nbre_points=nbre_points1*nbre_points2*nbre_points3;
            les_positions_.resize(nbre_points,dimension);

            for (; i<dimension; i++)
              is >> origine(i);
            for (i=0; i<dimension; i++)
              is >> extremite1(i);
            for (i=0; i<dimension; i++)
              is >> extremite2(i);
            for (i=0; i<dimension; i++)
              is >> extremite3(i);
            for (i=0; i<dimension; i++)
              dx1(i)=(extremite1(i)-origine(i))/(nbre_points1-1);
            for (i=0; i<dimension; i++)
              dx2(i)=(extremite2(i)-origine(i))/(nbre_points2-1);
            for (i=0; i<dimension; i++)
              dx3(i)=(extremite3(i)-origine(i))/(nbre_points3-1);
            for (i=0; i<nbre_points1; i++)
              for (j=0; j<nbre_points2; j++)
                for (int m=0; m<nbre_points3; m++)
                  for (k=0; k<dimension; k++)
                    les_positions_(i+j*nbre_points1+m*nbre_points1*nbre_points2,k)=origine(k)+i*dx1(k)+j*dx2(k)+m*dx3(k);
            break;
          }
        case 8:
          {
            type_ = les_motcles[rang];
            // circle nbre_points x0 y0 [z0 dir] radius teta1 teta2
            rang = 1;
            dim = 1;
            int dir;
            double radius, teta1, teta2;
            DoubleVect origine(dimension);
            is >> nbre_points;
            les_positions_.resize(nbre_points,dimension);
            for (int i=0; i<dimension; i++)
              is >> origine(i);
            if (dimension==3) is >> dir;
            is >> radius >> teta1 >> teta2;
            // Ajout des informations
            for (int i=0; i<dimension; i++)
              {
                type_+=" ";
                type_+=(Nom)origine(i);
              }
            type_+=" ";
            type_+=(Nom)radius;
            type_+=" ";
            type_+=(Nom)teta1;
            type_+=" ";
            type_+=(Nom)teta2;
            // We calculate the positions
            for (int i=0; i<nbre_points; i++)
              {
                double angle=teta1+(teta2-teta1)*i/(nbre_points-1);
                angle*=M_PI/180;
                if (dimension==2)
                  {
                    les_positions_(i,0)=origine(0)+radius*cos(angle);
                    les_positions_(i,1)=origine(1)+radius*sin(angle);
                  }
                else if (dimension==3)
                  {
                    if (dir==0)
                      {
                        les_positions_(i,0)=origine(0);
                        les_positions_(i,1)=origine(1)+radius*cos(angle);
                        les_positions_(i,2)=origine(2)+radius*sin(angle);
                      }
                    else if (dir==1)
                      {
                        les_positions_(i,0)=origine(0)+radius*cos(angle);
                        les_positions_(i,1)=origine(1);
                        les_positions_(i,2)=origine(2)+radius*sin(angle);
                      }
                    else if (dir==2)
                      {
                        les_positions_(i,0)=origine(0)+radius*cos(angle);
                        les_positions_(i,1)=origine(1)+radius*sin(angle);
                        les_positions_(i,2)=origine(2);
                      }
                  }
              }
            break;
          }
        case 9:
          {
            Motcle autre_sonde;
            is >> autre_sonde;
            // on cherche la sonde correspondante
            int m=-1;
            const Sondes& les_sondes=mon_post->les_sondes();
            for (int i=0; i<les_sondes.size(); i++)
              if (les_sondes(i).get_nom()==autre_sonde)
                {
                  m=i;
                  break;
                }
            //else Cerr<<les_sondes(i).get_nom()<<finl;
            if (m==-1)
              {
                Cerr<<" The probe name "<<autre_sonde<< " was not found"<<finl;
                exit();
              }
            // on recupere  les_positions_
            const Sonde& la_sonde_ref=les_sondes(m);
            type_ = la_sonde_ref.get_type();
            les_positions_=la_sonde_ref.les_positions();
            dim =  la_sonde_ref.get_dim();
            rang=1;
            break;
          }
        default:
          {
            Cerr << motlu <<"is not yet understood!" << finl;
            exit();
          }
        }
      fait(rang) = 1;
    }
  if ( (fait[0] == 0) || (fait[1] == 0) || (dim==-1))
    {
      Cerr << "Error while reading the probe " << nom_ << finl;
      Cerr << "The data of the probe have not been properly defined" << finl;
      exit();
    }

  // Construction du fichier associe a la sonde
  nom_fichier_=nom_du_cas();
  nom_fichier_+= "_";
  nom_fichier_+= nom_;
  if (dim==2)
    nom_fichier_+= ".plan";
  else
    nom_fichier_+= ".son";

  if (Process::je_suis_maitre())
    {
      // Ajout du nom du fichier sonde dans le fichier listant les sondes
      // Ce fichier sera utilise par Run_sonde
      SFichier fichier_sondes;
      Nom nom_fich=nom_du_cas();
      nom_fich+=".sons";
      if (!fichier_sondes_cree)
        fichier_sondes.ouvrir(nom_fich);
      else
        fichier_sondes.ouvrir(nom_fich,ios::app);
      fichier_sondes << nom_fichier_ << finl;
      fichier_sondes_cree=1;
    }
  return is;
}


// Description:
//    Associer le postraitement a la sonde.
// Precondition:
// Parametre: Postraitement& le_post
//    Signification: le postraitement a associer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la sonde a un postraitement associe
void Sonde::associer_post(const Postraitement& le_post)
{
  mon_post=le_post;
  if (mon_post->noms_fichiers_sondes().contient(nom_)==0)
    mon_post->noms_fichiers_sondes().add(nom_);
  else
    {
      Cerr << "The filename " << nom_ << " is used several times for probes." << finl;
      Cerr << "A probe must have a unique filename to avoid writing conflicts." << finl;
      exit();
    }
}


// Description:
//    Initialise la sonde. Dimensionne les tableaux,
//    de valeurs, verifie si les points specifies sont
//    bien dans la zone de calcul.
// Precondition:
// Parametre: Zone& zone_geom
//    Signification: la zone de calcul qui sera sondee
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception: point de sondage en dehors de la zone de calcul
// Effets de bord:
// Postcondition: la sonde est initialisee
void Sonde::initialiser()
{
  nb_bip = 0.;
  // Dimension the elem_ array:
  int nbre_points = les_positions_.dimension(0);
  if(elem_.size() != nbre_points)
    elem_.resize(nbre_points);

  const Zone& zone_geom = mon_champ->get_ref_domain().zone(0);

  if (numero_elem_==-1)
    {
      // Location of probes is given by coordinates in the les_positions_ array:
      int nb_som = zone_geom.type_elem().nb_som();
      int nb_coord = les_positions_.dimension(1);
      if (nb_coord+1>nb_som)
        {
          Cerr << "You can't specify the probe named " << nom_ << " with "<< nb_coord << " coordinates on the domain named " <<zone_geom.domaine().le_nom()<<finl;
          Cerr << "which is constituted with cells of kind " << zone_geom.type_elem().valeur().que_suis_je() << "." << finl;
          Cerr << "Change the probe coordinates or use numero_elem_sur_maitre keyword (see documentation)" << finl;
          Cerr << "to specify a cell containing the probe and not its coordinates." << finl;
          exit();
        }
      // Fill the elem_ array (which list cells containing all the probes):
      zone_geom.chercher_elements(les_positions_,elem_);
    }
  else
    {
      // Location of the probe is given by an element number:
      elem_[0]= numero_elem_;
      const IntTab& les_elems=mon_champ->get_ref_zone_dis_base().zone().les_elems();
      if (numero_elem_<les_elems.dimension_tot(0))
        {
          const DoubleTab& coord=mon_champ->get_ref_zone_dis_base().zone().domaine().les_sommets();
          int nb_som=les_elems.dimension(1);
          // Fill les_positions_ with the cog of the cell numero_elem_
          for (int s=0; s<nb_som; s++)
            {
              int soml=les_elems(numero_elem_,s);
              for (int dir=0; dir<dimension; dir++)
                les_positions_(0,dir)+=coord(soml,dir)/nb_som;
            }
        }
      else
        {
          if (je_suis_maitre())
            {
              Cerr<<" On the probe named " << nom_ << " , the element number "<<numero_elem_<<" does not exist on the master processor, we put the position to zero"<<finl;
              les_positions_=0;
            }
          elem_[0]=-1;
        }
    }
  // Check if some probes are outside the domain:
  for (int i=0; i<nbre_points; i++)
    if (mp_max(elem_[i])==-1)
      Cerr << "WARNING: The point number " << i+1 << " of the probe named " << nom_ << " is outside the computational domain " << zone_geom.domaine().le_nom() << finl;

  if (je_suis_maitre()&&(nproc()>1))
    {
      if (ncomp == -1)
        {
          const Noms noms_comp = mon_champ->get_property("composantes");
          int nb_comp = noms_comp.size();
          if (nb_comp == 1)
            valeurs_sur_maitre.resize(nbre_points);
          else
            valeurs_sur_maitre.resize(nbre_points,nb_comp);
        }
      else
        valeurs_sur_maitre.resize(nbre_points);
    }

  // Probes may be moved to cog, face, vertex:
  const Zone& zone = mon_champ->get_ref_domain().zone(0);
  const Noms nom_champ = mon_champ->get_property("nom");
  if (grav==1)
    {
      Cerr<<"The location of the probe named "<<nom_<<" are modified (to centers of gravity). Check the .log files to see the new location."<<finl;
      const Zone_VF& zoneVF = ref_cast(Zone_VF,mon_champ->get_ref_zone_dis_base());
      const DoubleTab xp = zoneVF.xp();
      for (int i=0; i<nbre_points; i++)
        {
          if(elem_[i]!=-1)
            {
              Journal()<<"The point " << i << " of the probe "<<nom_<<" is moved:";
              for (int dir=0; dir<dimension; dir++)
                {
                  Journal() << " x(" << dir << "): " << les_positions_(i,dir) << " -> " << xp(elem_[i],dir);
                  les_positions_(i,dir)=xp(elem_[i],dir);
                }
              Journal() << finl;
            }
        }
    }
  else if (nodes==1)
    {
      const Zone_VF& zoneVF = ref_cast(Zone_VF,mon_champ->get_ref_zone_dis_base());
      const DoubleTab xv = zoneVF.xv();
      const IntTab& elem_faces = zoneVF.elem_faces();
      if (mp_max(elem_faces.size_array())==0)
        {
          Cerr << "Error: the domain " << zoneVF.zone().domaine().le_nom() << " is not discretized." << finl;
          exit();
        }
      if (sub_type(Champ_Generique_Interpolation,mon_champ.valeur()))
        {
          Motcle dom_interp=mon_champ->get_ref_domain().le_nom();
          Cerr << finl;
          Cerr << "Error in your probe : " << nom_ << finl;
          Cerr << "You can not project to nodes, the field " << nom_champ[0] << finl;
          Cerr << "which is interpolated on the domain " << dom_interp << finl;
          exit();
        }
      Cerr<<"The location of the probe named "<<nom_<<" are modified (to faces). Check the .log files to see the new location."<<finl;
      const int nfaces_par_element = zone.nb_faces_elem() ;
      for (int i=0; i<nbre_points; i++)
        {
          double dist_min=DMAXFLOAT;
          int face_min=-1;
          if(elem_[i]!=-1)
            {
              Journal()<<"The point " << i << " of the probe "<<nom_<<" is moved:";
              for(int fac=0; fac<nfaces_par_element; fac++)
                {
                  int face=elem_faces(elem_[i],fac);
                  double dist=0.;

                  for (int dir=0; dir<dimension; dir++)
                    dist+=(xv(face,dir)-les_positions_(i,dir))*(xv(face,dir)-les_positions_(i,dir));

                  if(dist<=dist_min)
                    {
                      dist_min=dist;
                      face_min=face;
                    }
                }

              for (int dir=0; dir<dimension; dir++)
                {
                  Journal() << " x(" << dir << "): " << les_positions_(i,dir) << " -> " << xv(face_min,dir);
                  les_positions_(i,dir)=xv(face_min,dir);
                }
              Journal() << " (" << (face_min < zoneVF.premiere_face_int() ? "boundary face " : "internal face ") << face_min<<")" << finl;
            }
        }
    }
  else if (som==1)
    {
      if (sub_type(Champ_Generique_Interpolation,mon_champ.valeur()))
        {
          Motcle dom_interp=mon_champ->get_ref_domain().le_nom();
          Cerr << finl;
          Cerr << "Error in your probe : " << nom_ << finl;
          Cerr << "You can not project to vertexes, the field " << nom_champ[0] << finl;
          Cerr << "which is interpolated on the domain " << dom_interp << finl;
          exit();
        }
      Cerr<<"The location of the probe named "<<nom_<<" are modified (to vertexes). Check the .log files to see the new location."<<finl;
      const IntTab& sommet_elem = zone.les_elems();
      const int sommets_par_element = zone.les_elems().dimension(1);
      const DoubleTab& coord = zone.domaine().les_sommets();
      for (int i=0; i<nbre_points; i++)
        {
          double dist_min=DMAXFLOAT;
          int sommet_min=-1;
          if(elem_[i]!=-1)
            {
              Journal()<<"The point " << i << " of the probe "<<nom_<<" is moved:";
              for(int isom=0; isom<sommets_par_element; isom++)
                {
                  int sommet=sommet_elem(elem_[i],isom);
                  double dist=0.;
                  for (int dir=0; dir<dimension; dir++)
                    dist+=(coord(sommet,dir)-les_positions_(i,dir))*(coord(sommet,dir)-les_positions_(i,dir));

                  if(dist<=dist_min)
                    {
                      dist_min=dist;
                      sommet_min=sommet;
                    }
                }

              for (int dir=0; dir<dimension; dir++)
                {
                  Journal() << " x(" << dir << "): " << les_positions_(i,dir) << " -> " << coord(sommet_min,dir);
                  les_positions_(i,dir)=coord(sommet_min,dir);
                }
              Journal() << finl;
            }
        }
    }

  // chaque processeur a regarder si il avait le point
  // le maitre construit un tableau (prop) determinant qui
  // va donner la valeur au maitre

  // Le maitre construit aussi le vect(ArrOfInt) participant
  // lui donnant pour un proc les differents elements de celui-ci
  IntVect prop(elem_);
  if (je_suis_maitre())
    {
      IntVect elems2(elem_);

      prop=0;
      // Par defaut c'est le maitre le proprio (en particulier pour les sondes en dehors)
      IntVect elem_prov(elem_);
      DoubleTab positions_prov(les_positions_);
      int p;
      int nbproc=Process::nproc();
      for(p=1; p<nbproc; p++)
        {
          recevoir(elem_prov,p,0,2002+p);
          recevoir(positions_prov,p,0,2001+p);
          for (int el=0; el<nbre_points; el++)
            if (elem_prov[el]!=-1)
              {
                if  (elems2[el]==-1)
                  {
                    elems2[el]=elem_prov[el];
                    prop[el]=p;
                    for (int j=0; j<dimension; j++)
                      les_positions_(el,j)=positions_prov(el,j);
                  }
              }
        }
      // OK On a rempli le tableau prop;

      // le maitre dimensionne participant;
      participant.dimensionner(nbproc);
      for(p=0; p<nbproc; p++)
        {
          int size=0;
          for (int el=0; el<nbre_points; el++)
            if (prop[el]==p) size++;
          participant[p].resize_array(size);
          participant[p]=-1;
          int pos=0;
          for (int el=0; el<nbre_points; el++)
            if (prop[el]==p)
              {
                participant[p][pos]=el;
                pos++;
              }
          assert((size==0)||(min_array(participant[p])>-1));
        }
    }
  else
    {
      envoyer(elem_,Process::me(),0,2002+Process::me());
      envoyer(les_positions_,Process::me(),0,2001+Process::me());
    }

  envoyer_broadcast(prop, 0);

  reprise=0;
  les_positions_sondes_=les_positions_;

  nbre_points_tot=nbre_points;

  //
  // on redimensionne les tableaux a la taille reel a partir de prop
  nbre_points=0;
  for (int el=0; el<nbre_points_tot; el++)
    if (prop(el)==me()) nbre_points++;
  DoubleTab new_positions(nbre_points,dimension);
  IntVect new_elem(nbre_points);
  int comp=0;
  for (int el=0; el<nbre_points_tot; el++)
    if (prop(el)==me())
      {
        new_elem(comp)=elem_(el);
        for (int dir=0; dir<dimension; dir++)
          new_positions(comp,dir)=les_positions_(el,dir);
        comp++;
      }
  int test=nbre_points;
  test=mp_sum(test);
  assert(test==nbre_points_tot);
  elem_=new_elem;
  les_positions_=new_positions;
  //

  // on dimensionne le tableau valeurs_locales
  if (ncomp == -1)
    {
      const Noms noms_comp = mon_champ->get_property("composantes");
      int nb_comp = noms_comp.size();

      if (nb_comp == 1)
        valeurs_locales.resize(nbre_points);
      else
        valeurs_locales.resize(nbre_points,nb_comp);
    }
  else
    valeurs_locales.resize(nbre_points);

}


// Description:
//    Ouvre le fichier associe a la sonde. (*.son)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le fichier associe est ouvert
void Sonde::ouvrir_fichier()
{
  if(je_suis_maitre())
    {
      if(le_fichier_.is_open())
        le_fichier_.close();
      //char *sonde_file;
      struct stat f;
      const char* sonde_file=nom_fichier_;
      if (stat(sonde_file,&f))
        reprise = 0;
      else if (reprise==0)
        reprise = mon_post->probleme().reprise_effectuee();

      if (reprise==0)
        le_fichier_.ouvrir(nom_fichier_);
      else
        le_fichier_.ouvrir(nom_fichier_,ios::app);

      SFichier& s = le_fichier_;
      s.setf(ios::scientific);
      s.precision(8);

      // Ecriture de l'en tete des fichiers sondes :
      if ((dim==0 || dim==1) && reprise==0)
        {
          reprise=1;
          const DoubleTab& p=les_positions_sondes();
          int nbre_points = les_positions_sondes_.dimension(0);
          s << "# " << nom_fichier_ << finl;
          s << "# Temps";
          for(int i=0; i<nbre_points; i++)
            {
              s << " x= " << p(i,0) << " y= " << p(i,1) ;
              if (dimension==3) fichier() << " z= " << p(i,2) ;
            }
          s << finl;
          // La 1.5.6 ajoute des informations supplementaires
          s << "# Champ " << nom_champ_lu_ << finl;
          s << "# Type " << get_type() << finl;
        }
      // Ecriture de l'en tete des fichiers plan :
      if (dim>1 && reprise==0)
        {
          reprise=1;
          s << "TRUST   Version1  01/09/96" << finl;
          s << nom_du_cas() << finl;
          s << "TRUST" << finl;
          s << "GRILLE";
          const DoubleTab& p = les_positions_sondes();
          // Nouveau on ajoute des informations pour Run_sonde
          s << " " << get_type() << " " << nom_champ_lu_ << " " << nbre_points1 << " " << nbre_points2;
          for (int j=0; j<dimension; j++) s << " " << p(0,j);
          for (int j=0; j<dimension; j++) s << " " << p((nbre_points1-1)*nbre_points2,j);
          for (int j=0; j<dimension; j++) s << " " << p(nbre_points2-1,j);
          s << finl;
          Nom nom_grille("Grille");
          Nom nom_topologie("Topologie");
          nom_grille += "_";
          nom_grille += nom_;
          nom_topologie += "_";
          nom_topologie += nom_;
          int k,kn;
          int nbre_points = les_positions_sondes_.dimension(0);
          if (dim==2)
            {
              double xn=0.,yn=0.,zn=0.,norme=0.;
              int p1=1 ,p_nbre_points2=nbre_points2;
              while (p(0,0)==p(p1,0) && p(0,1)==p(p1,1) && p(0,2)==p(p1,2))
                {
                  p1+=1;
                  assert(p1<nbre_points);
                }
              while (p(0,0)==p(p_nbre_points2,0) && p(0,1)==p(p_nbre_points2,1) && p(0,2)==p(p_nbre_points2,2))
                {
                  p_nbre_points2+=1;
                  assert(p_nbre_points2<nbre_points);
                }
              while (p(p1,0)==p(p_nbre_points2,0) && p(p1,1)==p(p_nbre_points2,1) && p(p1,2)==p(p_nbre_points2,2))
                {
                  p_nbre_points2+=1;
                  assert(p_nbre_points2<nbre_points);
                }

              if (dimension==3)
                {
                  xn=(p(p1,2)-p(0,2))*(p(p_nbre_points2,1)-p(0,1))
                     -(p(p1,1)-p(0,1))*(p(p_nbre_points2,2)-p(0,2));
                  yn=(p(p1,0)-p(0,0))*(p(p_nbre_points2,2)-p(0,2))
                     -(p(p1,2)-p(0,2))*(p(p_nbre_points2,0)-p(0,0));
                }
              else if (dimension==2)
                {
                  xn=0.;
                  yn=0.;
                }
              zn=(p(p1,1)-p(0,1))*(p(p_nbre_points2,0)-p(0,0))
                 -(p(p1,0)-p(0,0))*(p(p_nbre_points2,1)-p(0,1));
              norme=dabs(xn)+dabs(yn)+dabs(zn);
              xn/=norme;
              yn/=norme;
              zn/=norme;
              s << nom_grille << " 3 " << 2*nbre_points << finl;
              int i;
              for(i=0; i<nbre_points; i++)
                {
                  s << p(i,0) << " " << p(i,1) ;
                  if (dimension==3) fichier() << " " << p(i,2) << finl;
                  else if (dimension==2) s << " 0." << finl;
                }
              for(i=0; i<nbre_points; i++)
                {
                  s << p(i,0)+xn << " " << p(i,1)+yn ;
                  if (dimension==3) s << " " << p(i,2)+zn << finl;
                  else if (dimension==2) s << " " << zn << finl;
                }
              s << "TOPOLOGIE" << finl;
              s << nom_topologie << " " << nom_grille << finl;
              s << "MAILLE" << finl;
              s << (nbre_points1-1)*(nbre_points2-1) << finl;
              for(int j=0; j<nbre_points2-1; j++)
                for(i=0; i<nbre_points1-1; i++)
                  {
                    k=j*nbre_points1+i+1;
                    kn=k+nbre_points;
                    s << "VOXEL8 " << k << " " << k+1 << " "
                      << k+nbre_points1 << " " << k+nbre_points1+1
                      << " " << kn << " " << kn+1 << " " << kn+nbre_points1
                      << " " << kn+nbre_points1+1 << finl;
                  }
              s << "FACE" << finl;
              s << "0" << finl;
            }
          else if (dim==3)
            {
              const DoubleTab& pbis=les_positions_sondes();
              s << nom_grille << " 3 " << nbre_points << finl;
              int i;
              for(i=0; i<nbre_points; i++)
                {
                  s << pbis(i,0) << " " << pbis(i,1) ;
                  s << " " << pbis(i,2) << finl;
                }
              s << "TOPOLOGIE" << finl;
              s << nom_topologie << " " << nom_grille << finl;
              s << "MAILLE" << finl;
              s << (nbre_points1-1)*(nbre_points2-1)*(nbre_points3-1) << finl;
              for(int m=0; m<nbre_points3-1; m++)
                for(int j=0; j<nbre_points2-1; j++)
                  for(i=0; i<nbre_points1-1; i++)
                    {
                      k=m*nbre_points2*nbre_points1+j*nbre_points1+i+1;
                      kn=k+nbre_points2*nbre_points1;
                      s << "VOXEL8 " << k << " " << k+1 << " "
                        << k+nbre_points1 << " " << k+nbre_points1+1
                        << " " << kn << " " << kn+1 << " " << kn+nbre_points1
                        << " " << kn+nbre_points1+1 << finl;
                    }
              s << "FACE" << finl;
              s << "0" << finl;
            }
        }
      s.close();
    }
}


// Description:
//    Effectue une mise a jour en temps de la sonde
//    effectue le postraitement.
// Precondition:
// Parametre: double temps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: double tinit
//    Signification: le temps initial de la sonde
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Sonde::mettre_a_jour(double un_temps, double tinit)
{
  // La mise a jour du champ est a supprimer car elle doit deja etre faite dans Post::mettre_a_jour()
  double temps_courant = mon_champ->get_time();

  if ( temps_courant != un_temps )
    {
      Champ espace_stockage;
      Champ_base& ma_source_mod = ref_cast_non_const(Champ_base,mon_champ->get_champ(espace_stockage));
      ma_source_mod.mettre_a_jour(un_temps);
    }
  double dt=mon_post->probleme().schema_temps().pas_de_temps();
  double nb;
  // Le *(1+Objet_U::precision_geom) est pour eviter des erreurs d'arrondi selon les machines
  if (periode<=dt)
    nb=nb_bip+1.;
  else
    modf(temps_courant*(1+Objet_U::precision_geom)/periode, &nb);

  // On doit ecrire les sondes
  if (nb>nb_bip)
    {
      // Si le maillage est deformable il faut reconstruire les sondes
      if (mon_post->probleme().domaine().deformable()) initialiser();
      nb_bip=nb;
      reprise=1;
      postraiter();
    }
}


// Description:
//    Effectue un postraitement.
//    Calcul les valeurs du champ aux position demandees
//    et les imprime sur le fichier associe.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Sonde::postraiter()
{
  // Pour sonde de Champ_Generique_Interpolation
  // Attention le get_champ est important. Il permet
  // de calculer les valeurs du champ
  Champ espace_stockage;
  mon_champ->fixer_identifiant_appel(nom_champ_lu_);
  const Champ_base& ma_source = ref_cast(Champ_base,mon_champ->get_champ(espace_stockage));

  if (chsom==1)
    {
      Champ_base& ma_source_mod =ref_cast_non_const(Champ_base,ma_source);
      if (ncomp == -1)
        ma_source_mod.valeur_aux_elems_smooth(les_positions_,elem_, valeurs_locales);
      else
        ma_source_mod.valeur_aux_elems_compo_smooth(les_positions_,elem_,valeurs_locales, ncomp);
    }
  else
    {
      if (ncomp == -1)
        ma_source.valeur_aux_elems(les_positions_,elem_, valeurs_locales);
      else
        ma_source.valeur_aux_elems_compo(les_positions_,elem_,valeurs_locales, ncomp);
    }

  //int i;
  int nb_compo=valeurs_locales.nb_dim();
  //  int nbre_points = les_positions_.dimension(0);

  // le maitre reconstruit le tableau valeurs
  // a partir des differents contributeurs

  if(je_suis_maitre())
    {
      ouvrir_fichier();
      fichier().ouvrir(nom_fichier_,ios::app);
      fichier().setf(ios::scientific);
      fichier().precision(8);
      int p;
      int nbproc = Process::nproc();
      DoubleTab valeurs_pe;


      DoubleTab& valeurs=(nbproc==1?valeurs_locales:valeurs_sur_maitre);
      if (nbproc==1)
        ;//valeurs=valeurs_locales;
      else
        {
          int nb_val=valeurs_locales.dimension(0);
          if(nb_compo==1)
            {
              for(int i=0; i<nb_val; i++)
                {
                  valeurs(participant[0][i])=valeurs_locales(i);
                }
            }
          else
            {
              int nb_val2=valeurs_locales.dimension(1);
              int k;
              for(int i=0; i<nb_val; i++)
                for(k=0; k<nb_val2; k++)
                  {
                    valeurs(participant[0][i],k)=valeurs_locales(i,k);
                  }
            }
        }

      for(p=1; p<nbproc; p++)
        {
          // le message n'est envoye que si le proc participe
          if (participant[p].size_array()!=0)
            {
              recevoir(valeurs_pe,p,0,2002+p);
              if(nb_compo==1)
                {
                  int nb_val=valeurs_pe.dimension(0);
                  for(int i=0; i<nb_val; i++)
                    {
                      valeurs(participant[p][i])=valeurs_pe(i);
                      //           val_max = max(dabs(valeurs(i)),dabs(valeurs_pe(i)));
                      //                   if(val_max==(dabs(valeurs_pe(i))))
                      //                     valeurs(i)=valeurs_pe(i);
                    }
                }
              else
                {
                  int nb_val1=valeurs_pe.dimension(0);
                  int nb_val2=valeurs_pe.dimension(1);
                  int k;
                  for(int i=0; i<nb_val1; i++)
                    for(k=0; k<nb_val2; k++)
                      {
                        valeurs(participant[p][i],k)=valeurs_pe(i,k);
                        //  val_max = max(dabs(valeurs(i,k)),dabs(valeurs_pe(i,k)));
                        //                     if(val_max==(dabs(valeurs_pe(i,k))))
                        //                       valeurs(i,k)=valeurs_pe(i,k);
                      }
                }
            }
        }

      double temps_courant = mon_post->probleme().schema_temps().temps_courant();

      if (dim==0 || dim==1)
        {
          fichier() << temps_courant;
          for(int i=0; i<valeurs.dimension(0); i++)
            if (nb_compo==2)
              for(int k=0; k<valeurs.dimension(1); k++)
                fichier() << " " << valeurs(i,k);
            else
              fichier() << " " << valeurs(i);
          fichier() << finl;
        }
      // Pour les sondes type plan, impression au format lml :
      // num_sommet comp1 [comp2] [comp3]
      // et dans la troisieme direction :
      else if (dim==2 || dim==3)
        {
          Nom nom_post;
          int nbre_points = les_positions_.dimension(0);
          nbre_points =nbre_points_tot;
          const Noms noms_comp = mon_champ->get_property("composantes");
          int nb_comp = noms_comp.size();
          const Noms unites = mon_champ->get_property("unites");
          if (ncomp==-1)
            {
              const Noms noms_champ = mon_champ->get_property("nom");
              nom_post = noms_champ[0];
            }
          else
            nom_post = noms_comp[ncomp];

          Nom nom_topologie("Topologie");
          nom_topologie += "_";
          nom_topologie += nom_;

          fichier() << "TEMPS " << temps_courant << "\n";
          fichier() << "CHAMPPOINT " << nom_post << " " << nom_topologie
                    << " " << temps_courant << "\n";
          fichier() << nom_post << " " << nb_comp << " " << unites[0] << "\n";

          int nbp=nbre_points;
          if (dim==2) nbp*=2;
          if (nb_comp>1)
            fichier() << "type1 " << nbp << "\n";
          else fichier() << "type0 " << nbp << "\n";
          int i;
          for(i=0; i<nbre_points; i++)
            {
              fichier() << i+1;
              if (nb_compo==2)
                for(int j=0; j<valeurs.dimension(1); j++)
                  fichier() << " " << valeurs(i,j);
              else
                fichier() << " " << valeurs(i);
              // Pour ne pas flusher :
              fichier() << "\n";
            }
          // Pour le 2D, on rajoute une direction
          if (dim==2)
            {
              for(i=0; i<nbre_points; i++)
                {
                  fichier() << nbre_points+i+1;
                  if (nb_compo==2)
                    for(int j=0; j<valeurs.dimension(1); j++)
                      fichier() << " " << valeurs(i,j);
                  else
                    fichier() << " " << valeurs(i);
                  // Pour ne pas flusher :
                  fichier() << "\n";
                }
            }
        }
      fichier().flush();
      fichier().close();
    }
  else
    {
      // le processeur envoye un message que si il participe
      if (valeurs_locales.dimension(0)!=0)
        envoyer(valeurs_locales,Process::me(),0,2002+Process::me());
    }
}

