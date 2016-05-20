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
// File:        Champ_front_recyclage.cpp
// Directory:   $TRUST_ROOT/src/Kernel/VF/Champs
// Version:     /main/33
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_front_recyclage.h>
#include <Interprete.h>
#include <Probleme_base.h>
#include <DoubleTrav.h>
#include <Zone_VF.h>
#include <Equation_base.h>
#include <communications.h>
#include <Octree.h>
#include <IntList.h>
#include <EFichier.h>
#include <EcrFicCollecte.h>
#include <Champ_front_calc.h>
#include <string> // Pour AIX
Implemente_instanciable_sans_constructeur(Champ_front_recyclage,"Champ_front_recyclage",Ch_front_var_instationnaire_dep);

Champ_front_recyclage::Champ_front_recyclage()
{
  int dim = Objet_U::dimension;
  delt_dist.resize(dim,Array_base::NOCOPY_NOINIT);
  delt_dist=0;
  ampli_fluct_.resize(1,Array_base::NOCOPY_NOINIT);
  ampli_fluct_ = 1.;
  ampli_moy_imposee_.resize(1,Array_base::NOCOPY_NOINIT);
  ampli_moy_imposee_ = 1.;
  ampli_moy_recyclee_.resize(1,Array_base::NOCOPY_NOINIT);
  ampli_moy_recyclee_ = 1.;

  methode_moy_impos_ = -1;
  methode_moy_recycl_ = -1;
  ndir = -4;
  diametre = -1.;
  u_tau = -1.;
  visco_cin = -1.;
  fich_recycl_ = "??";
  fich_impos_ = "??";
}

Sortie& Champ_front_recyclage::printOn(Sortie& os) const
{
  return os;
}

//-pb_champ_evaluateur est obligatoire pour specifier le nom du probleme
// et le nom de l inconnue qui vont servir a initialiser la reference
// vers le champ evaluateur. Le nombre de composantes de ce champ doit etre specifie.

Entree& Champ_front_recyclage::readOn(Entree& is)
{

  int dim = Objet_U::dimension;
  int lu_info_evaluateur = 0;
  int nbcomp;

  Motcle motlu, accolade_fermee="}", accolade_ouverte="{";
  Motcles les_mots(8);
  {
    les_mots[0] = "pb_champ_evaluateur";
    les_mots[1] = "distance_plan";
    les_mots[2] = "ampli_fluctuation";
    les_mots[3] = "ampli_moyenne_imposee";
    les_mots[4] = "ampli_moyenne_recyclee";
    les_mots[5] = "moyenne_imposee";
    les_mots[6] = "moyenne_recyclee";
    les_mots[7] = "direction_anisotrope";
  }

  is >> motlu;
  if(motlu!=accolade_ouverte)
    {
      Cerr << "We expected a { while reading of " << que_suis_je() << finl;
      Cerr << "and not : " << motlu << finl;
      exit();
    }

  is >> motlu;
  Cerr<<"word read="<<motlu<<finl;

  while (motlu != accolade_fermee)
    {
      int rang = les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          {
            is >> nom_pb1 >> nom_inco1 >> nbcomp;
            lu_info_evaluateur = 1;
            ampli_fluct_.resize(nbcomp,Array_base::NOCOPY_NOINIT);
            ampli_moy_imposee_.resize(nbcomp,Array_base::NOCOPY_NOINIT);
            ampli_moy_recyclee_.resize(nbcomp,Array_base::NOCOPY_NOINIT);
            ampli_fluct_ = 1.;
            ampli_moy_imposee_ = 1.;
            ampli_moy_recyclee_ = 1.;
            break;
          }
        case 1 :
          {
            for (int i=0; i<dim; i++)
              is >> delt_dist(i) ;
            break;
          }

        case 2 :
          {
            //int nbcomp;
            is >> nbcomp;
            ampli_fluct_.resize(nbcomp);
            for (int i=0; i<nbcomp; i++)
              is >> ampli_fluct_(i) ;
            break;
          }
        case 3 :
          {
            //int nbcomp;
            is >> nbcomp;
            ampli_moy_imposee_.resize(nbcomp);
            for (int i=0; i<nbcomp; i++)
              is >> ampli_moy_imposee_(i) ;
            break;
          }
        case 4 :
          {
            //int nbcomp;
            is >> nbcomp;
            ampli_moy_recyclee_.resize(nbcomp);
            for (int i=0; i<nbcomp; i++)
              is >> ampli_moy_recyclee_(i) ;
            break;
          }

        case 5 :
          {
            methode_moy_impos_ = lire_info_moyenne_imposee(is);
            break;
          }
        case 6 :
          {
            methode_moy_recycl_ = lire_info_moyenne_recyclee(is);
            break;
          }
        case 7 :
          {
            is >> ndir;
            break;
          }

        default :
          {
            Cerr<<"Champ_front_recyclage::readOn: keyword "<<motlu<<" is not recognized."<<finl;
            Cerr<<"The recognized keywords are :"<<les_mots<<finl;
            exit();
          }
        }
      is >> motlu;
    }

  if (lu_info_evaluateur!=1)
    {
      Cerr<<"Keyword pb_champ_evaluateur must be specified with associated data"<<finl;
      Cerr<<"when Champ_front_recyclage field is used."<<finl;
      exit();
    }
  fixer_nb_comp(nbcomp);

  return is;
}

//Lecture des informations necessaires pour evaluer la moyenne imposee
//-profil                : pour imposer un profil analytique
//-interpolation       : lecture dans un fichier et construction d un champ moyen
//                       en realisant une interpolation des donnees lues.
//                       La moyenne est construite pour une direction privilegiee
//                         (direction_anisotrope) et vaut 0 pour les autres directions
//-connexion_approchee : lecture dans un fichier et on retient la valeur de la
//                       variable lue par connexion avec le point le plus proche
//                       de la face de bord consideree
//-connexion_exacte    : lecture dans un fichier geometrie des coordonnees de points
//                         situes dans le plan d evaluation et lecture dans un fichier
//                       distinct des valeurs moyennes. Les valeurs moyennes lues
//                         sont stockees quand la correspondance exacte entre les points
//                         en vis a vis est verifiee.
//- logarithmique      : construction de la moyenne par une loi de paroi (logarithmique)
//
int Champ_front_recyclage::lire_info_moyenne_imposee(Entree& is)
{
  int methode = -1;;
  Motcle choix,motlu;
  Motcles les_mots(5);
  {
    les_mots[0] = "profil";
    les_mots[1] = "interpolation";
    les_mots[2] = "connexion_approchee";
    les_mots[3] = "connexion_exacte";
    les_mots[4] = "logarithmique";
  }

  is >> choix;
  int rang = les_mots.search(choix);
  switch(rang)
    {

    case 0 :
      {
        methode = 1;
        int nc;
        is >> nc;
        fcts_xyz.dimensionner(nc);
        for (int i=0; i<nc; i++)
          is >> fcts_xyz[i];
        break;
      }
    case 1 :
      {
        methode = 2;
        break;
      }
    case 2 :
      {
        methode = 3;
        break;
      }
    case 3 :
      {
        methode = 4;
        break;
      }
    case 4 :
      {
        int compt_mot_lu = 0;
        while (compt_mot_lu!=4)
          {
            is >> motlu;
            if (motlu=="diametre")
              is >> diametre;
            else if (motlu=="u_tau")
              is >> u_tau;
            else if (motlu=="visco_cin")
              is >> visco_cin;
            else if (motlu=="direction")
              is >> ndir;
            else
              {
                Cerr<<"The keyword "<<motlu<<" is not recognized."<<finl;
              }
            compt_mot_lu += 1;
          }
        if (diametre<=0.)
          {
            Cerr<<"Champ_front_recyclage : a positive diameter must be specified when a logarithmique injected averaged is used."<<finl;
            exit();
          }
        if (u_tau<=0.)
          {
            Cerr<<"Champ_front_recyclage : a positive shear velocity must be specified when a logarithmique injected averaged is used."<<finl;
            exit();
          }
        if (visco_cin<=0.)
          {
            Cerr<<"Champ_front_recyclage : a positive kinematic viscosity must be specified when a logarithmique injected averaged is used."<<finl;
            exit();
          }
        if (ndir<-3)
          {
            Cerr<<"Champ_front_recyclage : the direction must be specified when a logarithmique injected averaged is used."<<finl;
            exit();
          }
        methode = 5;
        break;
      }

    default :
      {
        Cerr<<"Champ_front_recyclage::lire_info_moyenne_imposee : keyword "<<choix<<" is not recognized."<<finl;
        Cerr<<"The recognized keywords are :"<<les_mots<<finl;
        exit();
      }
    }

  if ((methode == 2) || (methode == 3))
    {
      Nom nom_fich;
      is >> motlu;
      if (motlu!="fichier")
        {
          Cerr<<"Champ_front_recyclage::lire_info_moyenne_imposee : at least one file must be provided for this method."<<finl;
          exit();
        }
      else
        is >> nom_fich;
      fich_impos_ = nom_fich;
    }

  if (methode == 4)
    {
      Nom nom_fich;
      is >> motlu;
      if (motlu!="fichier")
        {
          Cerr<<"Champ_front_recyclage::lire_info_moyenne_imposee : at least one file must be provided for this method."<<finl;
          exit();
        }
      else
        is >> nom_fich;
      fich_maillage_ = nom_fich;
      is >> nom_fich;
      fich_impos_ = nom_fich;
    }

  if (methode==-1)
    {
      Cerr<<"Champ_front_recyclage::lire_info_moyenne_imposee : no method to make average evaluation has been given."<<finl;
      Cerr<<"Choose among the following keywords : les_mots"<<finl;
      exit();
    }

  return methode;
}

//Lecture des informations necessaires pour evaluer la moyenne recyclee
//-surfacique : moyenne surfacique des valeurs recyclees
//             (la moyenne est faite sur le bord2 ou l on recupere les valeurs)
//-interpolation connexion_approchee connexion_exacte (voir lire_info_moyenne_imposee)
//
int Champ_front_recyclage::lire_info_moyenne_recyclee(Entree& is)
{
  int methode = -1;
  Motcle choix,motlu;
  Motcles les_mots(4);
  {
    les_mots[0] = "surfacique";
    les_mots[1] = "interpolation";
    les_mots[2] = "connexion_approchee";
    les_mots[3] = "connexion_exacte";
  }
  is >> choix;
  int rang = les_mots.search(choix);

  switch(rang)
    {
    case 0 :
      {
        methode = 1;
        break;
      }
    case 1 :
      {
        methode = 2;
        break;
      }
    case 2 :
      {
        methode = 3;
        break;
      }
    case 3 :
      {
        methode = 4;
        break;
      }

    default :
      {
        Cerr<<"Champ_front_recyclage::lire_info_moyenne_recyclee keyword "<<choix<<" is not recognized."<<finl;
        Cerr<<"The recognized keywords are :"<<les_mots<<finl;
        exit();
      }
    }

  if ((methode == 2) || (methode == 3) || (methode == 4))
    {
      Nom nom_fich;
      is >> motlu;
      if (motlu!="fichier")
        {
          Cerr<<"Champ_front_recyclage::lire_info_moyenne_recyclee : a file must be provided for this method."<<finl;
          exit();
        }
      else
        is >> nom_fich;

      fich_recycl_  = nom_fich;
      if (methode == 4)
        is >> nom_fich;
      fich_maillage_ = nom_fich;
    }

  if (methode==-1)
    {
      Cerr<<"Champ_front_recyclage::lire_info_moyenne_recyclee : no method to make average evaluation has been given."<<finl;
      Cerr<<"Choose among the following keywords : les_mots"<<finl;
      exit();
    }

  return methode;
}

//Initialisation de la reference au champ evaluateur (l_inconnue1)
void Champ_front_recyclage::associer_champ_evaluateur(const Nom& un_nom_pb1,const Motcle& un_nom_inco1)
{
  Objet_U& ob1 = Interprete::objet(un_nom_pb1);
  REF(Probleme_base) pb1;
  REF(Champ_base) rch1;

  if(sub_type(Probleme_base,ob1))
    {
      pb1 = ref_cast(Probleme_base,ob1);
    }
  else
    {
      Cerr <<"No problem named "<< un_nom_pb1 <<" has been found."<< finl;
      exit();
    }
  rch1 = pb1->get_champ(un_nom_inco1);

  if(sub_type(Champ_Inc_base,rch1.valeur()))
    l_inconnue1 = ref_cast(Champ_Inc_base,rch1.valeur()) ;
  else
    {
      Cerr<<pb1->le_nom()<<" has no unknown field named "<<un_nom_inco1<<finl;
      exit();
    }
}

//Evaluation de la localisation des points (coords)
//ou l_inconnue1 doit evaluer les valeurs a recycler
//coords = xv2(faces_bor2) + delt_dist
//
void Champ_front_recyclage::get_coord_faces(const Frontiere_dis_base& fr_vf,
                                            DoubleTab& coords,
                                            const DoubleVect& delt_dist)
{
  const Front_VF&   fr_vf2    = ref_cast(Front_VF,fr_vf);
  const Zone_VF&    la_zone2  = ref_cast(Zone_VF,fr_vf2.zone_dis());
  const DoubleTab&  xv2       = la_zone2.xv();
  const int      nb_faces2 = fr_vf2.nb_faces();
  const int      ndeb2     = fr_vf2.num_premiere_face();
  const int      dim       = xv2.dimension(1);

  coords.resize(nb_faces2,dim);
  for (int i = 0; i < nb_faces2; i++)
    for (int j = 0; j < dim; j++)
      coords(i,j) = xv2(i+ndeb2,j) + delt_dist(j);
}

//-appel a Champ_front_calc::initialiser(...)
//-dimensionnement de moyenne_imposee_ et moyenne_recyclee_ par nb_faces_bord2 et nb_compo_
//-constructon des structures assurant le parallelisme :
// inconnues1_coords_to_eval_ inconnues1_elems_ inconnues2_faces_
//-initialisation de moyenne_imposee_ et moyenne_recyclee_
//
int Champ_front_recyclage::initialiser(double temps, const Champ_Inc_base& inco)
{
  if (!Ch_front_var_instationnaire_dep::initialiser(temps,inco))
    return 0;

  // 1: remote
  // 2: local
  const Front_VF& fr_vf2 = ref_cast(Front_VF,la_frontiere_dis.valeur());
  const Nom& nom_bord2 = fr_vf2.frontiere().le_nom();
  int nb_faces_bord2 = fr_vf2.nb_faces();
  associer_champ_evaluateur(nom_pb1,nom_inco1);

  moyenne_imposee_.resize(nb_faces_bord2,nb_compo_,Array_base::COPY_INIT);
  moyenne_recyclee_.resize(nb_faces_bord2,nb_compo_,Array_base::COPY_INIT);

  if (ampli_fluct_.size()!=nb_compo_)
    {
      Cerr<<"The number of fluctuation amplification factors is not coherent with the component number of the field "<<nom_inco1<<finl;
      Cerr<<"This field has "<<nb_compo_<<" components and there is "<<ampli_fluct_.size()<<" amplification factors defined."<<finl;
      Cerr<<"Please check that ampli_fluctuation is well specified in your data set."<<finl;
      exit();
    }

  if (ampli_moy_imposee_.size()!=nb_compo_)
    {
      Cerr<<"The number of the amplification factors for the fixed average is not coherent with the component number of the field "<<nom_inco1<<finl;
      Cerr<<"This field has "<<nb_compo_<<" components and there is "<<ampli_moy_imposee_.size()<<" amplification factors defined."<<finl;
      Cerr<<"Please check that ampli_moyenne_imposee is well specified in your data set."<<finl;
      exit();
    }

  if (ampli_moy_recyclee_.size()!=nb_compo_)
    {
      Cerr<<"The number of the amplification factors for the recycled average is not coherent with the component number of the field "<<nom_inco1<<finl;
      Cerr<<"This field has "<<nb_compo_<<" components and there is "<<ampli_moy_recyclee_.size()<<" amplification factors defined."<<finl;
      Cerr<<"Please check that ampli_moyenne_recyclee is well specified in your data set."<<finl;
      exit();
    }

  Cerr << "Initializing Champ_front_recyclage on the boundary: " << nom_bord2 << finl;

  // Pour chaque point de remote_coord, chercher le processeur dont le domaine1 contient ce point,
  //  et l'indice de l'element:

  const int nprocs = nproc();
  const int moi = me();

  // Remplissage de remote_coords[i]: coordonnees des centres des faces du bord nom_bord2
  // (bord destination) + delt_dist
  // du processeur i (tous les procs possedent le meme tableau remote_coords)
  // Build a VECT(DoubleTab) containing the coordinates of the remote faces on zone 1 for each processor
  DoubleTabs remote_coords(nprocs);
  /*
  if (nom_bord!="??") // Improve this test
  {
     const Front_VF& fr_vf1 = ref_cast(Front_VF,front_dis());
     get_coord_faces(fr_vf1,remote_coords[moi],delt_dist);
  }
  else */
  get_coord_faces(fr_vf2,remote_coords[moi],delt_dist);

  for (int pe = 0; pe < nprocs; pe++)
    envoyer_broadcast(remote_coords[pe],pe);

  // Size the 3 arrays:
  inconnues1_coords_to_eval_.dimensionner(nprocs);
  inconnues1_elems_.dimensionner(nprocs);
  inconnues2_faces_.dimensionner(nprocs);

  VECT(ArrOfInt) indexes_to_recv(nprocs);

  // tmp array for octree results:
  ArrOfInt elem_list;
  elem_list.set_smart_resize(1);

  const Zone& zone1 = l_inconnue1.valeur().equation().zone_dis().valeur().zone();
  const int nb_elem_zone1 = zone1.nb_elem();
  const int dim = remote_coords[moi].dimension(1);

  const Zone_dis_base& zdis = l_inconnue1.valeur().equation().zone_dis().valeur();
  const Zone_VF& zvf = ref_cast(Zone_VF,zdis);
  const DoubleTab& xp = zvf.xp();
  DoubleVect remote_point(3);
  const OctreeRoot& octree = zone1.construit_octree();
  // Loop on processes
  for (int pe = 0; pe < nprocs; pe++)
    {
      // Remote coordinates of the local faces:
      const DoubleTab& remote_coords_on_pe = remote_coords[pe];
      int nb_faces_on_pe = remote_coords_on_pe.dimension(0);
      elem_list.resize_tab(nb_faces_on_pe,ArrOfInt::NOCOPY_NOINIT);

      ArrOfInt elem_list_par_pos;
      // Loop on local faces on the process pe:
      for (int ind_face=0; ind_face<nb_faces_on_pe; ind_face++)
        {
          for (int j=0; j<dimension; j++)
            remote_point(j) = remote_coords_on_pe(ind_face,j);

          // Fill elem_list_par_pos by elements surrounding the remote point:
          octree.rang_elems_sommet(elem_list_par_pos,remote_point(0),remote_point(1),remote_point(2));
          int size_elem_list = elem_list_par_pos.size_array();

          if (size_elem_list==0)
            elem_list(ind_face) = -1;
          else
            {
              DoubleTrav coord_elems(size_elem_list,dimension);
              IntList elems;
              // Fill a list elems and coordinates of the elems
              for (int ind_elem=0; ind_elem<size_elem_list; ind_elem++)
                {
                  int elem = elem_list_par_pos(ind_elem);
                  elems.add_if_not(elem);
                  for (int dir=0; dir<dimension; dir++)
                    coord_elems(ind_elem,dir) = xp(elem,dir);
                }
              int elem_identifie;
              if (size_elem_list==1)
                {
                  // Easy, one element in the list
                  elem_identifie = elems[0];
                }
              else if (size_elem_list==2 && norme_array(delt_dist)==0)
                {
                  // Check first if it is not 2 elements surrounding 2 internal superposed boundary faces, in this
                  // case, discards the element linked to the local boundary face
                  int local_face = fr_vf2.num_premiere_face()+ind_face;
                  int local_elem = zvf.face_voisins(local_face,0);
                  if (local_elem==-1) local_elem = zvf.face_voisins(local_face,1);
                  //Cerr << "[" << Process::me() << "<->" << pe << "] nb_faces=" << fr_vf2.nb_faces() << " face= " << ind_face << " " << local_elem << " " << elems[0] << " " << elems[1] << finl;
                  if (elems[0]==local_elem)
                    elem_identifie=elems[1]; // Remote elem
                  else if (elems[1]==local_elem)
                    elem_identifie=elems[0]; // Remote elem
                  else
                    {
                      // Surely 2 elements surrounding an internal face so select one element:
                      elem_identifie = Zone::identifie_item_unique(elems,coord_elems,remote_point);
                    }
                }
              else
                {
                  // Find a unic remote elem:
                  elem_identifie = Zone::identifie_item_unique(elems,coord_elems,remote_point);
                }
              elem_list(ind_face) = (elem_identifie < nb_elem_zone1 ? elem_identifie : -1);
            }
        }

      // We fill these three arrays:
      DoubleTab& coord1 = inconnues1_coords_to_eval_[pe];
      coord1.set_smart_resize(1);
      ArrOfInt& num_elem = inconnues1_elems_[pe];
      num_elem.set_smart_resize(1);
      ArrOfInt& index_to_recv = indexes_to_recv[pe];
      index_to_recv.set_smart_resize(1);

      int nb_remote_faces = 0;
      // Loop on local faces on the process pe:
      for (int face = 0; face < nb_faces_on_pe; face++)
        {
          const int elem = elem_list(face);
          if (elem < 0 || elem >= nb_elem_zone1)
            {
              // This coordinate is not on this processor...
            }
          else
            {
              coord1.resize(nb_remote_faces + 1, dim);
              for (int j = 0; j < dim; j++)
                coord1(nb_remote_faces, j) = remote_coords_on_pe(face, j);
              nb_remote_faces++;
              num_elem.append_array(elem);
              index_to_recv.append_array(face);
            }
        }
    }

  // Send index_to_recv data to each processor
  envoyer_all_to_all(indexes_to_recv, inconnues2_faces_);

  // Check that each coordinate is received from at least one processor:
  const int nb_faces2 = remote_coords[moi].dimension(0);
  ArrOfDouble count(nb_faces2);
  for (int pe = 0; pe < nprocs; pe++)
    {
      const ArrOfInt index_to_recv = inconnues2_faces_[pe];
      const int n = index_to_recv.size_array();
      for (int i = 0; i < n; i++)
        count[index_to_recv[i]] ++;
      //Cerr << index_to_recv << finl;
    }
  int error_1 = 0;
  int error_2 = 0;
  for (int i = 0; i < nb_faces2; i++)
    {
      if (count[i] < 1)
        {
          Journal() << "Champ_front_recyclage : error face (number "<< i<< ") " << remote_coords[moi](i, 0) << " " << remote_coords[moi](i, 1) << " "
                    << ((dim==3)?remote_coords[moi](i, 2):0.) << " no matching element found" << finl;
          error_1 = 1;
        }
      else if (count[i]>1)
        {
          Journal() << "Champ_front_recyclage : error face (number "<< i<< ") " << remote_coords[moi](i, 0) << " " << remote_coords[moi](i, 1) << " "
                    << ((dim==3)?remote_coords[moi](i, 2):0.) << " " << count[i] << " corresponding points" << finl;
          error_2 = 1;
        }

    }
  if (mp_sum(error_1))
    {
      Cerr << "Error in Champ_front_recyclage.\n"
           << "No matching source element found for some faces of boundary " << nom_bord2
           << " (see log files)." << finl;
      Cerr << "If you are sure that boundary faces match with faces of the other domain, try adding this keyword:" << finl;
      Cerr << "PrecisionGeom 1.e-6" << finl;
      Cerr << "Just after the following keyword in your data file:" << finl;
      Cerr << "dimension " << Objet_U::dimension << finl;
      barrier();
      exit();
    }

  if (mp_sum(error_2))
    {
      Cerr << "Error in Champ_front_recyclage.\n"
           << "At least two elements match for some faces of boundary " << nom_bord2
           << " (see log files)." << finl;

      barrier();
      exit();
    }

  initialiser_moyenne_imposee(moyenne_imposee_);
  initialiser_moyenne_recyclee(moyenne_recyclee_);
  mettre_a_jour(temps);

  return 1;

}

//-Evaluation des valeurs de l_inconnue1 aux points situes
// dans le plan1 (xv(faces_bord2) + delt_dist)
//-Echange des donnees entre processeurs
//-Calcul de moyenne_imposee_ et moyenne_recyclee_ si evaluation dependante du temps
// (faite par le processeur local sur le bord2 a partir de donnees recues d autres
//  processeurs ou specifiees dans le jeu de donnees)
//
void Champ_front_recyclage::mettre_a_jour(double temps)
{

  DoubleTab& tab = valeurs_au_temps(temps);
  const int nprocs = nproc();
  DoubleTabs values_to_send(nprocs);
  DoubleTabs values_to_recv(nprocs);
  // temporary array (because valeur_aux_elems wants intvect and not arrofint)
  IntVect elems;
  elems.set_smart_resize(1);

  for (int pe = 0; pe < nprocs; pe++)
    {
      elems.resize(inconnues1_elems_[pe].size_array());
      elems.inject_array(inconnues1_elems_[pe]);

      const int n = elems.size_array();
      values_to_send[pe].resize(n,nb_compo_);
      l_inconnue1.valeur().valeur_aux_elems(inconnues1_coords_to_eval_[pe],
                                            elems,
                                            values_to_send[pe]);

    }
  envoyer_all_to_all(values_to_send, values_to_recv);
  for (int pe = 0; pe < nprocs; pe++)
    {
      const DoubleTab& values = values_to_recv[pe];
      const ArrOfInt index_to_recv = inconnues2_faces_[pe];
      const int n = index_to_recv.size_array();
      for (int i = 0; i < n; i++)
        {
          const int idx = index_to_recv[i];
          for (int j = 0; j < nb_compo_; j++)
            tab(idx, j) = values(i, j);
        }
    }

  const Front_VF& fr_vf2 = ref_cast(Front_VF,la_frontiere_dis.valeur());
  int nb_faces2 = fr_vf2.nb_faces();

  calcul_moyenne_imposee(tab,temps);
  calcul_moyenne_recyclee(tab,temps);

  for (int i=0; i<nb_faces2; i++)
    for (int dir=0; dir<nb_compo_; dir++)
      tab(i,dir) = ampli_moy_imposee_(dir)*moyenne_imposee_(i,dir) + ampli_fluct_(dir)*(tab(i,dir)-ampli_moy_recyclee_(dir)*moyenne_recyclee_(i,dir));

  tab.echange_espace_virtuel();

}

//Construction de donnees necessaires a l estimation de moyenne_imposee_
//en fonction de la methode d evaluation retenue (methode_moy_impos_)
//
void Champ_front_recyclage::initialiser_moyenne_imposee(DoubleTab& moyenne)
{

  const Front_VF& fr_vf2 = ref_cast(Front_VF,la_frontiere_dis.valeur());

  if (methode_moy_impos_==1)
    {
      profil_2.dimensionner(nb_compo_);
      for (int i=0; i<nb_compo_; i++)
        {
          profil_2[i].setNbVar(4);
          profil_2[i].setString(fcts_xyz[i]);
          profil_2[i].addVar("t");
          profil_2[i].addVar("x");
          profil_2[i].addVar("y");
          profil_2[i].addVar("z");
          profil_2[i].parseString();
        }
    }
  else if (methode_moy_impos_==2)
    {
      lire_fichier_format1(moyenne_imposee_,fr_vf2,fich_impos_);
    }
  else if (methode_moy_impos_==3)
    {
      lire_fichier_format2(moyenne_imposee_,fr_vf2,fich_impos_);
    }
  else if (methode_moy_impos_==4)
    {
      lire_fichier_format3(moyenne_imposee_,fr_vf2,fich_impos_,fich_maillage_);
    }
  else if (methode_moy_impos_==5)
    {
      const Zone_dis_base& zone_dis2 = fr_vf2.zone_dis();
      const Zone_VF& la_zone2        = ref_cast(Zone_VF,zone_dis2);
      const DoubleTab&  xv2          = la_zone2.xv();

      double Kappa = 0.41;
      double d_paroi,y_plus,U_log;

      int nbface2 = fr_vf2.nb_faces();
      int num1 = fr_vf2.num_premiere_face();
      int num2 = num1 + nbface2;
      int num_face_loc;

      for (int num_face2=num1; num_face2<num2; num_face2++)
        {
          num_face_loc = num_face2-num1;
          d_paroi = diametre/2.;

          if(dimension==3)
            {
              if(abs(ndir)==1) d_paroi -= sqrt(xv2(num_face2,1)*xv2(num_face2,1) + xv2(num_face2,2)*xv2(num_face2,2));
              if(abs(ndir)==2) d_paroi -= sqrt(xv2(num_face2,0)*xv2(num_face2,0) + xv2(num_face2,2)*xv2(num_face2,2));
              if(abs(ndir)==3) d_paroi -= sqrt(xv2(num_face2,0)*xv2(num_face2,0) + xv2(num_face2,1)*xv2(num_face2,1));
            }
          else
            {
              if(abs(ndir)==1) d_paroi -= sqrt(xv2(num_face2,1)*xv2(num_face2,1));
              if(abs(ndir)==2) d_paroi -= sqrt(xv2(num_face2,0)*xv2(num_face2,0));
            }

          y_plus = d_paroi*u_tau/visco_cin;
          U_log  = (1./Kappa)*log(y_plus) + 5.1;
          U_log *=  u_tau;
          U_log *= ndir/abs(ndir);

          for (int dir=0; dir<nb_compo_; dir++)
            moyenne(num_face_loc,dir) = ((dir+1)==abs(ndir))*U_log;
        }
    }
}

//Construction de donnees necessaires a l estimation de moyenne_recyclee_
//en fonction de la methode d evaluation retenue (methode_moy_recycl_)
//
void Champ_front_recyclage::initialiser_moyenne_recyclee(DoubleTab& moyenne)
{
  const Front_VF& fr_vf2 = ref_cast(Front_VF,la_frontiere_dis.valeur());
  if (methode_moy_recycl_==2)
    {
      lire_fichier_format1(moyenne_recyclee_,fr_vf2,fich_recycl_);
    }
  else if (methode_moy_recycl_==3)
    {
      lire_fichier_format2(moyenne_recyclee_,fr_vf2,fich_recycl_);
    }
  else if (methode_moy_recycl_==4)
    {
      lire_fichier_format3(moyenne_recyclee_,fr_vf2,fich_recycl_,fich_maillage_);
    }
}


//Evaluation de moyenne_imposee_ en fonction de la methode d evaluation retenue
//-methode_moy_impos_==1 : profil analytique
//-methode_moy_impos_==2 : pas d actualisation (voir initialiser_moyenne_imposee())
//                           lecture a l initialisation et interpolation des donnees lues
//-methode_moy_impos_==3 : pas d actualisation (voir initialiser_moyenne_imposee())
//                           lecture a l initialisation pour realiser une connexion_approchee
//-methode_moy_impos_==4 : pas d actualisation (voir initialiser_moyenne_imposee())
//                         lecture a l initialisation pour realiser une connexion_exacte
//-methode_moy_impos_==5 : pas d actualisation (voir initialiser_moyenne_imposee())
//                           construction a l initialisation d une loi log :
//                           U_log  = ndir/abs(ndir)*u_tau*(1./Kappa)*log(y_plus) + 5.1;
//
void Champ_front_recyclage::calcul_moyenne_imposee(const DoubleTab& tab,double temps)
{
  if (methode_moy_impos_==1)
    {
      const Front_VF& fr_vf2 = ref_cast(Front_VF,la_frontiere_dis.valeur());
      const Zone_dis_base& zone_dis2 = fr_vf2.zone_dis();
      const Zone_VF&  la_zone2 = ref_cast(Zone_VF,zone_dis2);
      const DoubleTab&  xv2 = la_zone2.xv();
      int nb_faces_bord2 = fr_vf2.nb_faces();
      int ndeb = fr_vf2.num_premiere_face();

      for (int i=0; i<nb_faces_bord2; i++)
        for (int j=0; j<nb_compo_; j++)
          {
            profil_2[j].setVar(0,temps);
            for (int d=0; d<dimension; d++)
              profil_2[j].setVar(d+1,xv2(ndeb+i,d));
            moyenne_imposee_(i,j) =  profil_2[j].eval();
          }
    }
  else if (methode_moy_impos_==2)
    {
    }
  else if (methode_moy_impos_==3)
    {
    }
  else if (methode_moy_impos_==4)
    {
    }
  else if (methode_moy_impos_==5)
    {
    }
  else if (methode_moy_impos_!=-1)
    {
      Cerr<<"The method used to calculate moyenne_imposee_ is not definded."<<finl;
      exit();
    }
}

//Evaluation de moyenne_recyclee_ en fonction de la methode d evaluation retenue
//-methode_moy_recycl_==1 : moyenne surfacique sur le bord2 des valeurs estimees
//                          par l_inconnue1 dans plan1
//-methode_moy_recycl_==2 : pas d actualisation (voir initialiser_moyenne_recyclee)
//                            lecture a l initialisation et interpolation des donnees lues
//-methode_moy_recycl_==3 : pas d actualisation (voir initialiser_moyenne_recyclee())
//                            lecture a l initialisation pour realiser une connexion_approchee
//-methode_moy_recycl_==4 : pas d actualisation (voir initialiser_moyenne_recyclee())
//                          lecture a l initialisation pour realiser une connexion_exacte
//
void Champ_front_recyclage::calcul_moyenne_recyclee(const DoubleTab& tab,double temps)
{
  if (methode_moy_recycl_==1)
    {
      const Front_VF& fr_vf2 = ref_cast(Front_VF,la_frontiere_dis.valeur());
      int nb_faces_bord2 = fr_vf2.nb_faces();
      DoubleVect face_surfaces;
      const Faces& les_faces_bord = fr_vf2.frontiere().faces();
      les_faces_bord.calculer_surfaces(face_surfaces);

      DoubleVect moyenne(nb_compo_);
      double somme_si = 0.;
      moyenne = 0.;
      for (int i=0; i<nb_faces_bord2; i++)
        {
          for (int j=0; j<nb_compo_; j++)
            moyenne(j) += tab(i,j)*face_surfaces(i);

          somme_si += face_surfaces(i);
        }
      somme_si = mp_sum(somme_si);

      for (int j=0; j<nb_compo_; j++)
        moyenne(j) = mp_sum(moyenne(j));
      moyenne /= somme_si;

      for (int i=0; i<nb_faces_bord2; i++)
        for (int j=0; j<nb_compo_; j++)
          moyenne_recyclee_(i,j) = moyenne(j);
    }
  else if (methode_moy_recycl_==2)
    {
    }
  else if (methode_moy_recycl_==3)
    {
    }
  else if (methode_moy_recycl_==4)
    {
    }
  else if (methode_moy_recycl_!=-1)
    {
      Cerr<<"The method used to calculate moyenne_recyclee_ is not defined."<<finl;
      exit();
    }
}

//-Lecture du fichier pour dimensionner y_Pb2 et U_Pb2
//-Utilisation des donnees lues dans le fichier pour les trois points
// possedant les cotes les plus proches de y afin de faire une interpolation
// de la variable associee
//
double Champ_front_recyclage::UPb(double y,Nom nom_fich)
{

  int nb_pts;
  DoubleVect y_Pb2;
  DoubleVect U_Pb2;
  EFichier fich_Pb2(nom_fich);

  y_Pb2.resize(1);
  U_Pb2.resize(1);
  int l=0;
  while (!fich_Pb2.eof())
    {
      y_Pb2.resize(l+1);
      U_Pb2.resize(l+1);
      fich_Pb2 >> y_Pb2(l) >> U_Pb2(l);
      l++;
    }
  nb_pts=l-1;
  y_Pb2.resize(nb_pts);
  U_Pb2.resize(nb_pts);
  fich_Pb2.close();

  l=0;
  int l_deb;

  while ((y>y_Pb2(l)) && (l<nb_pts))
    {
      l++;
    }

  if (l<2)
    {
      l_deb=0;
    }
  else
    {
      if (l>(nb_pts-2))
        {
          l_deb=nb_pts-3;
        }
      else
        {
          if ( (y-y_Pb2(l-1)) > (y_Pb2(l)-y) )
            {
              l_deb=l-2;
            }
          else
            {
              l_deb=l-1;
            }
        }
    }

  double ya=y_Pb2(l_deb);
  double Ua=U_Pb2(l_deb);
  double yb=y_Pb2(l_deb+1);
  double Ub=U_Pb2(l_deb+1);
  double yc=y_Pb2(l_deb+2);
  double Uc=U_Pb2(l_deb+2);
  double U;

  U=(y-ya)*(y-yb)/(yc-ya)/(yc-yb)*Uc;
  U+=(y-yb)*(y-yc)/(ya-yb)/(ya-yc)*Ua;
  U+=(y-yc)*(y-ya)/(yb-yc)/(yb-ya)*Ub;

  return U;

}

//-On determine une direction orthogonale a la direction d anisotropie
//-Pour chaque point d evaluation se trouvant dans le plan1 :
//  Estimation de sa cote y dans la direction orthogonale
//  Initialisation de moyenne (dans la direction anisotrope) a partir de
//  la fonction UPb pur la cote y
//
void Champ_front_recyclage::lire_fichier_format1(DoubleTab& moyenne,
                                                 const Frontiere_dis_base& fr_vf,
                                                 const Nom& nom_fich)
{
  const Front_VF& fr_vf2 = ref_cast(Front_VF,fr_vf);
  DoubleTab coords;
  get_coord_faces(fr_vf2,coords,delt_dist);
  const int nb_faces2 = coords.dimension(0);

  int dir_ortho = -1;
  if ((ndir==1) || (ndir==2))
    dir_ortho = ndir;
  else if (ndir==3)
    dir_ortho = 0;
  else
    {
      Cerr<<"For this method the anisotropic direction mut be 1 2 or 3"<<finl;
      exit();
    }

  for (int i=0; i<nb_faces2; i++)
    {
      double y = coords(i,dir_ortho);
      moyenne(i,ndir-1) = UPb(y,nom_fich);
    }
}

//-Pour chaque face du bord2 (coordonnes x2 y2 z2):
//  Lecture dans le fichier nom_fich des coordonnes de points (x1 y1 z1)
//  et de(s) valeur(s) de la variable associee var1 [var2] [var3]
//  Initialisation de moyenne (dans la direction anisotrope si celle_ci est specifiee
//  ou pour toutes les composantes sinon) a partir de var1 [var2] [var3]
//  associe au point x1 y1 z1 qui est a la plus petite distance de x2 y2 z2
//
// Defaut de parallelisme eventuel si (x1 y1 z1) ne parcourent pas les parties virtuels
// ou si deux points sont a egale distance d un point (x2 y2 z2)
//
void Champ_front_recyclage::lire_fichier_format2(DoubleTab& moyenne,
                                                 const Frontiere_dis_base& fr_vf,
                                                 const Nom& nom_fich)
{

  int N1;
  int num_face2,num_face2_loc;
  double x1,y1,z1;
  double x2,y2,z2;
  z2 = 0.;
  ifstream fic_Utargtest(nom_fich);
  if (!fic_Utargtest)
    {
      Cerr <<"No file of name : "<<nom_fich<<" is available."<<finl;
      exit();
    }

  const Front_VF& fr_vf2 = ref_cast(Front_VF,fr_vf);
  int num1 = fr_vf2.num_premiere_face();
  int num2 = num1 + fr_vf2.nb_faces();
  const Zone_dis_base& zone_dis2 = fr_vf2.zone_dis();
  const Zone_VF& zvf2 = ref_cast(Zone_VF,zone_dis2);
  const DoubleTab& xv2 = zvf2.xv();

  for (num_face2=num1; num_face2<num2; num_face2++)
    {
      x2 = xv2(num_face2,0);
      y2 = xv2(num_face2,1);
      if (dimension==3) z2 = xv2(num_face2,2);

      num_face2_loc = num_face2-num1;
      EFichier fic_Utarg(nom_fich);
      fic_Utarg >> N1;
      // Je lis le reste de la ligne au cas ou...
      std::string ligne;
      std::getline(fic_Utarg.get_ifstream(), ligne);
      int compteur=0;
      double dist = 1.E6;
      double dist_eval;

      DoubleVect val(nb_compo_);
      while (!fic_Utarg.eof())
        {
          compteur++;
          if(dimension==3)
            {
              fic_Utarg >> x1 >> y1 >> z1 ;
              for (int comp=0; comp<nb_compo_; comp++)
                fic_Utarg >> val(comp);
              // Je lis le reste de la ligne au cas ou...
              std::getline(fic_Utarg.get_ifstream(), ligne);
              dist_eval = (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1);
              if(dist_eval<dist)
                {
                  dist = dist_eval;
                  if ((abs(ndir)<4) && (abs(ndir)>0))
                    moyenne(num_face2_loc,abs(ndir)-1) = val(abs(ndir)-1);
                  else
                    for (int comp=0; comp<nb_compo_; comp++)
                      moyenne(num_face2_loc,comp) = val(comp);

                }
            }
          else
            {
              fic_Utarg >> x1 >> y1;
              for (int comp=0; comp<nb_compo_; comp++)
                fic_Utarg >> val(comp);
              // Je lis le reste de la ligne au cas ou...
              std::getline(fic_Utarg.get_ifstream(), ligne);
              dist_eval = (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);
              if(dist_eval<dist)
                {
                  dist = dist_eval;
                  if ((abs(ndir)<4) && (abs(ndir)>0))
                    moyenne(num_face2_loc,abs(ndir)-1) = val(abs(ndir)-1);
                  else
                    for (int comp=0; comp<nb_compo_; comp++)
                      moyenne(num_face2_loc,comp) = val(comp);
                }
            }
        }
      if (N1!=compteur-1)
        {
          Cerr << finl;
          Cerr << "Error while reading " << nom_fich << finl;
          Cerr << "The number of lines indicated in the header : " << N1 << finl;
          Cerr << "is not the same than the number of lines detected : " << compteur-1 << finl;
          exit();
        }
      fic_Utarg.close();
    }
}

//-Pour chaque face du bord2 (coordonnes x2 y2 z2) :
//  Lecture dans le fichier nom_fch2 des coordonnes de points (x1 y1 z1)
//  Lecture dans le fichier nom_fich1 des valeurs de la variable associee
//  et initialisation de moyenne a partir de ces valeurs. La lecture est stoppee
//  quand on a lu les valeurs associees au point x1 y1 z1 qui est en connexion
//  exacte avec x2 y2 z2 suivant la direction d anisotropie
//
void Champ_front_recyclage::lire_fichier_format3(DoubleTab& moyenne,
                                                 const Frontiere_dis_base& fr_vf,
                                                 const Nom& nom_fich1,const Nom& nom_fich2)
{
  int Nbfaces,trouve;
  int num_face1,num_face2,num_face2_loc;
  double x1,y1,z1;
  double x2,y2,z2;
  double eps=1.e-4;

  Nom fichier = "VERIF_face_perio.";
  fichier += nom_pb1;
  EcrFicCollecte fic(fichier);

  ifstream fic_geom_test(nom_fich2);
  if (!fic_geom_test)
    {
      Cerr <<"No file of name : geom_face_perio_"<<nom_pb1<<" is available."<<finl;
      exit();
    }

  ifstream fic_Umoy_test(nom_fich1);
  if (!fic_Umoy_test)
    {
      Cerr <<"No file of name : "<<nom_fich1<<" is available."<<finl;
      exit();
    }

  if (abs(ndir)!=1 && abs(ndir)!=2 && abs(ndir)!=3)
    {
      Cerr << finl;
      Cerr << "Error while reading the boundary condition " << fr_vf.le_nom() << finl;
      Cerr << "You must define the keyword \"direction_anisotrope\"" << finl;
      exit();
    }

  const Front_VF& fr_vf2 = ref_cast(Front_VF,fr_vf);
  int num1 = fr_vf2.num_premiere_face();
  int num2 = num1 + fr_vf2.nb_faces();
  const Zone_dis_base& zone_dis2 = fr_vf2.zone_dis();
  const Zone_VF& zvf2 = ref_cast(Zone_VF,zone_dis2);
  const DoubleTab& xv2 = zvf2.xv();
  int nb_faces_bord2 = fr_vf2.nb_faces();

  DoubleTab bidon;
  bidon.resize(nb_faces_bord2,Array_base::COPY_INIT);

  for (num_face2=num1; num_face2<num2; num_face2++)
    {
      x2 = xv2(num_face2,0);
      y2 = xv2(num_face2,1);
      if (dimension==3) z2 = xv2(num_face2,2);

      num_face2_loc = num_face2-num1;

      EFichier fic_geom(nom_fich2);
      EFichier fic_Umoy(nom_fich1);
      fic_geom  >> Nbfaces;
      fic_Umoy >> Nbfaces;
      // Je lis le reste de la ligne au cas ou...
      std::string ligne;
      std::getline(fic_geom.get_ifstream(), ligne);
      std::getline(fic_Umoy.get_ifstream(), ligne);
      int compteur=0;

      trouve = 0;

      while (!fic_geom.eof() && !fic_Umoy.eof())
        {
          compteur++;
          if (trouve==0)
            {
              fic_geom >> num_face1 >> x1 >> y1;
              fic_Umoy >> bidon(num_face2_loc) >> moyenne(num_face2_loc,0) >> moyenne(num_face2_loc,1);
              if (dimension==3)
                {
                  fic_geom >> z1;
                  fic_Umoy >> moyenne(num_face2_loc,2);
                  if(abs(ndir)==1 && (est_egal(y1,y2,eps)) && (est_egal(z1,z2,eps))) trouve=1;
                  if(abs(ndir)==2 && (est_egal(x1,x2,eps)) && (est_egal(z1,z2,eps))) trouve=1;
                  if(abs(ndir)==3 && (est_egal(x1,x2,eps)) && (est_egal(y1,y2,eps))) trouve=1;
                }
              else
                {
                  if(abs(ndir)==1 && (est_egal(y1,y2,eps))) trouve=1;
                  if(abs(ndir)==2 && (est_egal(x1,x2,eps))) trouve=1;
                }
            }
          // Je lis le reste de la ligne au cas ou...
          std::getline(fic_geom.get_ifstream(), ligne);
          std::getline(fic_Umoy.get_ifstream(), ligne);
        }

      if (Nbfaces!=compteur-1)
        {
          Cerr << finl;
          Cerr << "Error while reading " << nom_fich1 << finl;
          Cerr << "The number of lines indicated in the header : " << Nbfaces << finl;
          Cerr << "is not the same than the number of lines detected : " << compteur-1 << finl;
          exit();
        }

      if (trouve==0)
        {
          Cerr << " the face "<<num_face2<<" with coordinates : x="<<x2<<" y="<<y2;
          if (dimension==3)
            Cerr << " z="<<z2;
          Cerr << "\n has no corresponding faces in "<<nom_fich2<<finl;
          exit();
        }

      if (dimension==3)
        {
          fic <<" num_face1 " << num_face1  << " " << x1 << " " << y1 << " " << z1 << finl;
          fic <<" num_face2 " << num_face2  << " " << x2 << " " << y2 << " " << z2 << finl;
          fic <<" "  << finl;
        }
      else
        {
          fic <<" num_face1 " << num_face1  << " " << x1 << " " << y1  << finl;
          fic <<" num_face2 " << num_face2  << " " << x2 << " " << y2  << finl;
          fic <<" "  << finl;
        }

      fic_geom.close();
      fic_Umoy.close();

    }
  fic.close();
}
