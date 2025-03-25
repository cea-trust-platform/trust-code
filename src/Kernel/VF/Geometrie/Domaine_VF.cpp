/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Linear_algebra_tools_impl.h>
#include <Dirichlet_paroi_defilante.h>
#include <Build_Map_to_Structured.h>
#include <Connectivite_som_elem.h>
#include <Dirichlet_loi_paroi.h>
#include <Dirichlet_homogene.h>
#include <Static_Int_Lists.h>
#include <MD_Vector_tools.h>
#include <MD_Vector_base.h>
#include <Faces_builder.h>
#include <Analyse_Angle.h>
#include <DeviceMemory.h>
#include <Domaine_VF.h>
#include <Periodique.h>
#include <Conds_lim.h>
#include <Symetrie.h>
#include <Domaine.h>
#include <Scatter.h>
#include <Device.h>
#include <Navier.h>
#include <iomanip>
#include <utility>
#include <set>

#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDCouplingMemArray.hxx>
#include <MEDLoader.hxx>

using namespace MEDCoupling;
#endif

#ifdef __APPLE__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif /* __APLE__*/

Implemente_base(Domaine_VF,"Domaine_VF",Domaine_dis_base);

Sortie& Domaine_VF::printOn(Sortie& os) const
{
  os << "volumes_ : " << finl;
  volumes_.ecrit(os);

  os << "volumes_entrelaces_ : " << finl;
  volumes_entrelaces_.ecrit(os);

  os << "face_voisins_ : " << finl;
  face_voisins_.ecrit(os);

  os << "face_surfaces_ : "<<face_surfaces_<< finl;
  face_surfaces_.ecrit(os);

  os << "xp_ : " << finl;
  xp_.ecrit(os);

  os << "xv_ : " << finl;
  xv_.ecrit(os);

  os << "elem_faces_ : " << finl;
  elem_faces_.ecrit(os);

  //  os << "faces_doubles_ : " << finl; // PQ : 12/10/05 : a voir ce qu'il faut faire ici ???
  //  faces_doubles_.ecrit(os);

  os << "face_sommets_ : " << finl; // PQ : 12/10/05 : a voir ce qu'il faut faire ici ???
  face_sommets_.ecrit(os);

  os << "nb_faces_ : " << finl;
  os << nb_faces() <<finl;

  os << "les_bords_ : " << finl;
  os << les_bords_ <<finl;

  return os ;
}

//// readOn
//

Entree& Domaine_VF::readOn(Entree& is)
{
  volumes_.lit(is);
  volumes_entrelaces_.lit(is);
  face_voisins_.lit(is);
  face_surfaces_.lit(is);
  xp_.lit(is);
  xv_.lit(is);
  elem_faces_.lit(is);
  //  faces_doubles_.lit(is); // PQ : 12/10/05 : a voir ce qu'il faut faire ici ???
  face_sommets_.lit(is);
  int nb_faces_unused;
  is >> nb_faces_unused;
  is >> les_bords_;
  return is ;
}

/*! @brief cette methode ne fait rien elle est surchargee par Domaine_VDF par ex.
 *
 */
void Domaine_VF::reordonner(Faces&)
{
}

void Domaine_VF::renumeroter(Faces& les_faces)
{

  // Construction du tableau de renumerotation des faces. Ce tableau,
  // une fois trie dans l'ordre croissant donne l'ordre des faces dans
  // le domaine_VF. La cle de tri est construite de sorte a pouvoir retrouver
  // l'indice de la face a partir de la cle par la formule :
  //  indice_face = cle % nb_faces
  const int nbfaces = les_faces.nb_faces();
  ArrOfInt sort_key(nbfaces);

  {
    nb_faces_std_ = 0;
    const int nb_faces_front = domaine().nb_faces_frontiere();
    // Attention : face_voisins_ n'est pas encore initialise, il
    // faut passer par les_faces.voisins() :
    const IntTab& facevoisins = les_faces.voisins();
    // On place en premier les faces de bord:
    int i_face;
    for (i_face = 0; i_face < nb_faces_front; i_face++)
      // Si la face est au bord, elle doit etre placee au debut
      // (en fait elle ne doit pas etre renumerotee)
      sort_key[i_face] = i_face;

    for (i_face=nb_faces_front; i_face < nbfaces; i_face++)
      {
        const int elem0 = facevoisins(i_face, 0);
        const int elem1 = facevoisins(i_face, 1);
        // Ces faces ont toujours deux voisins.
        assert(elem0 >= 0 && elem1 >= 0);
        if (rang_elem_non_std_[elem0] >= 0 || rang_elem_non_std_[elem1] >= 0)
          {
            // Si la face est voisine d'un element non standard, elle
            // doit etre classee juste apres les faces de bord:
            sort_key[i_face] = i_face;
          }
        else
          {
            // Face standard : a la fin du tableau
            sort_key[i_face] = i_face + nbfaces;
            nb_faces_std_++;
          }
      }

    sort_key.ordonne_array();

    // On transforme a nouveau la cle en numero de face:
    for (i_face = 0; i_face < nbfaces; i_face++)
      {
        const int key = sort_key[i_face] % nbfaces;
        sort_key[i_face] = key;
      }
  }
  // On reordonne les faces:
  {
    IntTab& faces_sommets = les_faces.les_sommets();
    IntTab old_tab(faces_sommets);
    const int nb_som_faces = faces_sommets.dimension(1);
    for (int i = 0; i < nbfaces; i++)
      {
        const int old_i = sort_key[i];
        for (int j = 0; j < nb_som_faces; j++)
          faces_sommets(i, j) = old_tab(old_i, j);
      }
  }

  {
    IntTab& faces_voisins = les_faces.voisins();
    IntTab old_tab(faces_voisins);
    for (int i = 0; i < nbfaces; i++)
      {
        const int old_i = sort_key[i];
        faces_voisins(i, 0) = old_tab(old_i, 0);
        faces_voisins(i, 1) = old_tab(old_i, 1);
      }
  }

  // Calcul de la table inversee: reverse_index[ancien_numero] = nouveau numero
  ArrOfInt reverse_index(nbfaces);
  {
    for (int i = 0; i < nbfaces; i++)
      {
        const int j = sort_key[i];
        reverse_index[j] = i;
      }
  }
  // Renumerotation de elem_faces:
  {
    // Nombre d'indices de faces dans le tableau
    const int nb_items = elem_faces_.size();
    ArrOfInt& array = elem_faces_;
    for (int i = 0; i < nb_items; i++)
      {
        const int old = array[i];
        if (old<0)
          array[i] = -1;
        else
          array[i] = reverse_index[old];
      }
  }
  // Mise a jour des indices des faces de joint:
  {
    Joints&      joints    = domaine().faces_joint();
    const int nbjoints = joints.size();
    for (int i_joint = 0; i_joint < nbjoints; i_joint++)
      {
        Joint&     un_joint         = joints[i_joint];
        ArrOfInt& indices_faces = un_joint.set_joint_item(JOINT_ITEM::FACE).set_items_communs();
        const int nbfaces2    = indices_faces.size_array();
        assert(nbfaces2 == un_joint.nb_faces()); // renum_items_communs rempli ?
        for (int i = 0; i < nbfaces2; i++)
          {
            const int old = indices_faces[i]; // ancien indice local
            indices_faces[i] = reverse_index[old];
          }
        // Les faces de joint ne sont plus consecutives dans le
        // tableau: num_premiere_face n'a plus ne sens
        un_joint.fixer_num_premiere_face(-1);
      }
  }
  // Mise a jour des indices des groupes de faces:
  {
    Groupes_Faces&      groupes_faces    = domaine().groupes_faces();
    groupes_faces.renumerote(reverse_index);
  }

}


/*! @brief Genere les faces construits les frontieres
 *
 */
void Domaine_VF::discretiser()
{
  // ToDo reordering, best place here ?
  if (getenv("TRUST_MESH_REORDERING")!=nullptr) domaine().reordering();

  Cerr << "<<<<<<<<<< Discretization VF >>>>>>>>>>" << finl;

  Domaine_dis_base::discretiser();

  Domaine& ledomaine=domaine();
  histogramme_angle(ledomaine,Cerr);
  Faces* les_faces_ptr=creer_faces();
  Faces& les_faces= *les_faces_ptr;
  {
    {
      Type_Face type_face = domaine().type_elem()->type_face(0);
      les_faces.typer(type_face);
      les_faces.associer_domaine(domaine());

      Static_Int_Lists connectivite_som_elem;
      const int     nb_sommets_tot = domaine().nb_som_tot();
      const IntTab&    elements       = domaine().les_elems();

      construire_connectivite_som_elem(nb_sommets_tot,
                                       elements,
                                       connectivite_som_elem,
                                       1 /* include virtual elements */);

      Faces_builder faces_builder;
      faces_builder.creer_faces_reeles(domaine(),
                                       connectivite_som_elem,
                                       les_faces,
                                       elem_faces_);
    }

    reordonner(les_faces);

    // Les faces sont dans l'ordre definitif, on peut remplir
    // renum_items_communs des faces:
    Scatter::calculer_renum_items_communs(ledomaine.faces_joint(), JOINT_ITEM::FACE);

    // Remplissage de face_voisins des frontieres:
    // a factoriser avec DomaineCut.cpp
    {
      const IntTab& facevoisins = les_faces.voisins();
      const int nb_frontieres = ledomaine.nb_front_Cl();
      for (int i_frontiere = 0; i_frontiere < nb_frontieres; i_frontiere++)
        {
          Frontiere&   fr               = ledomaine.frontiere(i_frontiere);
          IntTab&      mes_face_voisins = fr.faces().voisins();
          const int nbfaces         = fr.nb_faces();
          const int premiere_face    = fr.num_premiere_face();
          mes_face_voisins.resize(nbfaces, 2);
          for (int i = 0; i < nbfaces; i++)
            {
              mes_face_voisins(i, 0) = facevoisins(premiere_face + i, 0);
              mes_face_voisins(i, 1) = facevoisins(premiere_face + i, 1);
            }
        }
    }

    Scatter::calculer_espace_distant_faces(domaine(), les_faces.nb_faces(), elem_faces_);
    // Apres la methode suivante, on aura le droit d'utiliser creer_tableau_faces() :
    Scatter::construire_md_vector(domaine(), les_faces.nb_faces(), JOINT_ITEM::FACE, md_vector_faces_);


    const MD_Vector& md_vect_sommets = domaine().les_sommets().get_md_vector();
    const MD_Vector& md_vect_elements = domaine().les_elems().get_md_vector();
    // Construction de l'espace virtuel du tableau face_sommets
    Scatter::construire_espace_virtuel_traduction(md_vector_faces_, md_vect_sommets, les_faces.les_sommets());

    // Idem pour face_voisins_
    // Certaines faces des elements virtuels les plus externes n'ont pas d'element local voisin => erreurs non fatales
    Scatter::construire_espace_virtuel_traduction(md_vector_faces_, md_vect_elements, les_faces.voisins(), 0 /* error not fatal */);

    // Idem pour elem_faces_
    Scatter::construire_espace_virtuel_traduction(md_vect_elements, md_vector_faces_, elem_faces_);

    ledomaine.init_faces_virt_bord(md_vector_faces_, md_vector_faces_front_);

    // Assignation de face_voisins et face_sommets
    face_voisins().ref(les_faces.voisins());
    face_sommets().ref(les_faces.les_sommets());

    // Calcul des surfaces:
    les_faces.calculer_surfaces(face_surfaces_);
    // Calcul de la surface des faces virtuelles
    MD_Vector md_nul;
    creer_tableau_faces(face_surfaces_);
    face_surfaces_.echange_espace_virtuel();
    face_surfaces_.set_md_vector(md_nul); // Detache la structure parallele

    // Changement a la v1.5.7 beta: xv_ a maintenant un descripteur parallele: dimension(0)=nb_faces
    les_faces.calculer_centres_gravite(xv_);

    // Calcul des volumes
    ledomaine.calculer_volumes(volumes_, inverse_volumes_);
  }
  {
    int i=0, derniere=ledomaine.nb_bords();
    les_bords_.dimensionner(domaine().nb_front_Cl()+domaine().nb_groupes_faces());
    for(; i<derniere; i++)
      {
        les_bords_[i].associer_frontiere(ledomaine.bord(i));
        les_bords_[i].associer_Domaine_dis(*this);
      }
    int decal=derniere;
    derniere+=domaine().nb_raccords();
    for(; i<derniere; i++)
      {
        int j=i-decal;
        les_bords_[i].associer_frontiere(ledomaine.raccord(j).valeur());
        les_bords_[i].associer_Domaine_dis(*this);
      }
    decal=derniere;
    derniere+=domaine().nb_frontieres_internes();
    for(; i<derniere; i++)
      {
        int j=i-decal;
        les_bords_[i].associer_frontiere(ledomaine.bords_interne(j));
        les_bords_[i].associer_Domaine_dis(*this);
      }
    decal=derniere;
    derniere+=domaine().nb_groupes_faces();
    for(; i<derniere; i++)
      {
        int j=i-decal;
        les_bords_[i].associer_frontiere(ledomaine.groupe_faces(j));
        les_bords_[i].associer_Domaine_dis(*this);
      }
  }
  // Centre de gravite des elements (tableau xp_)
  ledomaine.calculer_centres_gravite(xp_);
  // Centre de gravite du domaine
  ArrOfDouble c(dimension);
  ledomaine.calculer_mon_centre_de_gravite(c);

  // On cree les domaines frontieres
  ledomaine.creer_mes_domaines_frontieres(*this);

  delete les_faces_ptr;

  // Fill in the table face_numero_bord
  remplir_face_numero_bord();

  faces_doubles_.resize_array(nb_faces());
  est_face_bord_.resize_array(nb_faces_tot());


  ///////////////////////
  // On imprime des infos
  ///////////////////////
  ledomaine.imprimer();        // Extremas du domaine et volumes
  infobord();                        // Aires des bords
  info_elem_som();                // Nombre elements et sommets
  Cerr << "<<<<<< End of Discretization VF >>>>>>>>>>" << finl;
}

void Domaine_VF::discretiser_no_face()
{
  Domaine& dom = domaine();
  typer_elem(dom);
  // Calcul du volume du domaine discretisee
  dom.calculer_volumes(volumes(), inverse_volumes());
}

void Domaine_VF::typer_discretiser_ss_domaine(int i)
{
  Domaine& dom = domaine();

  auto& sds = les_sous_domaines_dis[i];
  sds.typer("Sous_domaine_VF");
  sds->associer_sous_domaine(dom.ss_domaine(i));
  sds->associer_domaine_dis(*this);
  sds->discretiser();
}

void Domaine_VF::remplir_face_voisins_fictifs(const Domaine_Cl_dis_base& )
{
  Cerr <<" Domaine_VF::remplir_face_voisins_fictifs(const Domaine_Cl_dis_base& ) must be overloaded by" << que_suis_je()<<finl;
  exit();
  assert(0);
}



/*! @brief renvoie new(Faces) ! elle est surchargee par Domaine_VDF par ex.
 *
 */
Faces* Domaine_VF::creer_faces()
{
  Faces* les_faces=new(Faces);
  return les_faces;
}

void Domaine_VF::modifier_pour_Cl(const Conds_lim& conds_lim)
{
  Cerr << " Domaine_VF::modifier_pour_Cl" << finl;
  for (auto& itr : conds_lim)
    {
      // for( cl ...
      const Cond_lim_base& cl=itr.valeur();
      if (sub_type(Periodique, cl))
        {
          // if (perio ...
          // Modifications du tableau face_voisins
          // Une face de type periodicite doit avoir les memes voisins que
          // sa face associee
          //      face_voisins(face,j) = face_voisins(face_associee(face),j)  j=1,2
          //

          const Periodique& la_cl_period = ref_cast(Periodique,cl);
          const Front_VF& frontieredis = ref_cast(Front_VF,la_cl_period.frontiere_dis());

          int nb_face_bord = frontieredis.nb_faces();
          int ndeb =0;
          int nfin = frontieredis.nb_faces_tot();
#ifndef NDEBUG
          int num_premiere_face = frontieredis.num_premiere_face();
          int num_derniere_face = num_premiere_face+nfin;
#endif
          int face;

          for (int ind_face=ndeb; ind_face<nfin; ind_face++)
            {
              face = frontieredis.num_face(ind_face);
              int faassociee = la_cl_period.face_associee(ind_face);
              faassociee = frontieredis.num_face(faassociee);
              if (ind_face<nb_face_bord)
                {
                  assert(faassociee>=num_premiere_face);
                  assert(faassociee<num_derniere_face);
                }

              if (face_voisins_(face,0) == -1)
                face_voisins_(face,0) = face_voisins_(faassociee,0);
              else if (face_voisins_(face,1) == -1)
                face_voisins_(face,1) = face_voisins_(faassociee,1);
            }
        }
    }


  // PQ : 10/10/05 : les faces periodiques etant a double contribution
  //                      l'appel a marquer_faces_double_contrib s'effectue dans cette methode
  //                      afin de pouvoir beneficier de conds_lim.


  marquer_faces_double_contrib(conds_lim);
  // Construction du tableau num_fac_loc_
  construire_num_fac_loc();
}

// Methode pour la construction du tableau num_fac_loc_
void Domaine_VF::construire_num_fac_loc()
{
  const int size = nb_faces_tot();
  num_fac_loc_.resize(size,2);
  num_fac_loc_=-1;
  for(int face=0; face<size; face++)
    for (int voisin=0; voisin<2; voisin++)
      {
        int elem = face_voisins_(face,voisin);
        if(elem!=-1)
          {
            int face_loc = numero_face_local(face,elem);
            if (face_loc != -1) num_fac_loc_(face,voisin) = face_loc;
            else
              {
                // Cas periodique
                int autre_voisin = 1 - voisin;
                int elem2 = face_voisins(face,autre_voisin);
                int nb_faces_elem = elem_faces_.dimension(1);
                for (face_loc=0; face_loc<nb_faces_elem; face_loc++)
                  {
                    const int face_glob = elem_faces(elem,face_loc);
                    const int elem_voisin1 = face_voisins(face_glob,0);
                    const int elem_voisin2 = face_voisins(face_glob,1);
                    if (elem_voisin1==elem2 || elem_voisin2==elem2)
                      {
                        num_fac_loc_(face,voisin) = face_loc;
                        break;
                      }
                  }
              }
          }
      }
}

// Renvoie le numero local de face a partir des numeros globaux face et elem
// Utiliser si possible Domaine_VF::num_fac_loc(face,voisin) car la methode est
// optimisee, celle ci non.
int Domaine_VF::numero_face_local(int face, int elem) const
{
  //int nfe=domaine().nb_faces_elem();
  // GF pour le cas ou on a plusieurs types de faces....
  // nb_faces_elem() renvoit le nbre de sommet par face du premier type de faces
  int nfe=elem_faces_.dimension(1);
  for(int face_loc=0; face_loc<nfe; face_loc++)
    if(elem_faces_(elem, face_loc)==face)
      return face_loc;
  return -1;
}

/*! @brief Remplissage du tableau face_virt_pe_num_ (voir commentaire dans Domaine_VF.
 *
 * h)
 *
 */
void Domaine_VF::construire_face_virt_pe_num()
{
  int i;

  const int nf = nb_faces();
  const int nf_tot = nb_faces_tot();
  const int nb_faces_virt = nf_tot - nf;

  IntTab tmp(0, 2);
  creer_tableau_faces(tmp, RESIZE_OPTIONS::NOCOPY_NOINIT);
  const int moi = me();
  for (i = 0; i < nf; i++)
    {
      tmp(i, 0) = moi;
      tmp(i, 1) = i;
    }
  tmp.echange_espace_virtuel();

  face_virt_pe_num_.resize(nb_faces_virt, 2);
  // Copie de la partie virtuelle de tmp dans face_virt_pe_num_
  face_virt_pe_num_.inject_array(tmp, nb_faces_virt*2, 0 /* dest offset*/, nf * 2 /* source offset*/);
}

const IntTab& Domaine_VF::face_virt_pe_num() const
{
  // On verifie que le tableau a ete construit (si ca plante, c'est qu'on a
  // oublie d'appeler construire_face_virt_pe_num()).
  assert(face_virt_pe_num_.dimension(1) == 2);
  return face_virt_pe_num_;
}

/*! @brief Compute the normalized boundary outward vector associated to the face global_face_number and eventually scaled by scale_factor
 *
 */
DoubleTab Domaine_VF::normalized_boundaries_outward_vector(int global_face_number, double scale_factor) const
{
  const IntTab& neighbor_faces = face_voisins();
  DoubleTab normal_vector(dimension); //result
  for (int d = 0; d < dimension; d++)
    normal_vector(d) = face_normales(global_face_number, d) / face_surfaces(global_face_number);

  //now we have a normalized normal vector
  //but it remains to orient it correctly (outwards)
  //for this we build a vector that links middle of the face to the center of the element
  //and then we do the inner product between this new vector and normal vector
  //So, if the result is positive, we change the sign of normal vector
  //else we do nothing
  int neighbor_elem=neighbor_faces(global_face_number,0);
  if( neighbor_elem == -1 )
    neighbor_elem = neighbor_faces(global_face_number,1);
  //DoubleTab vector_face_elem(dimension);
  double inner_product=0;
  for(int i=0; i<dimension; i++)
    {
      //ith component of the vector
      double vec_face_elem = xp(neighbor_elem,i)-xv(global_face_number,i);
      inner_product+=vec_face_elem*normal_vector(i);
    }

  if( inner_product > 0 )
    {
      for(int i=0; i<dimension; i++)
        {
          normal_vector(i)*=-1;
        }
    }

  //now the normal vector is well oriented (outwards)
  //we can multiply it by a given scale factor
  for(int i=0; i<dimension; i++)
    {
      normal_vector(i)*=scale_factor;
    }

  return normal_vector;
}

void Domaine_VF::marquer_faces_double_contrib(const Conds_lim& conds_lim)
{
  Journal() << " Domaine_VF::marquer_faces_double_contrib" << finl;
  // marquage des faces periodiques
  ////////////////////////////////////////////////

  est_face_bord_=0; // init for inner faces.
  for (auto& itr : conds_lim)
    {
      const Cond_lim_base& cl=itr.valeur();
      int flag = sub_type(Periodique, cl) ? 2 : 1;
      const Front_VF& le_bord = ref_cast(Front_VF, cl.frontiere_dis());
      for (int ind_face = 0; ind_face < le_bord.nb_faces_tot(); ind_face++)
        {
          int num_face = le_bord.num_face(ind_face);
          est_face_bord_[num_face]=flag;
        }
    }
  for (auto& itr : conds_lim)
    {
      const Cond_lim_base& cl=itr.valeur();
      if (sub_type(Periodique, cl))
        {
          const Periodique& la_cl_period = ref_cast(Periodique,cl);
          const Front_VF& frontieredis = ref_cast(Front_VF,la_cl_period.frontiere_dis());
          int ndeb = frontieredis.num_premiere_face();
          int nfin = ndeb + frontieredis.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            faces_doubles_[face]=1;
        }
    }

  // marquage des faces items communs
  ////////////////////////////////////////////////
  int nbjoints=nb_joints();

  for(int njoint=0; njoint<nbjoints; njoint++)
    {
      const Joint& joint_temp = joint(njoint);
      const IntTab& indices_faces_joint = joint_temp.joint_item(JOINT_ITEM::FACE).renum_items_communs();
      const int nbfaces = indices_faces_joint.dimension(0);
      for (int j = 0; j < nbfaces; j++)
        {
          // Pour acceder au face de joint, il existe desormais un tableau dedie renum_items_communs()
          // car l'ancienne facon d'acceder a ces faces etait de passer par
          // joint.num_premiere_face() et joint.nb_faces() ce qui n'est plus valable !
          // En effet, la numerotation des faces de joint n'est plus continue desormais !
          // joint.num_premiere_face() retourne -1 desormais pour detecter les anciens codages.
          int face_de_joint = indices_faces_joint(j, 1);
          faces_doubles_[face_de_joint] = 1;
        }
    }
}

void Domaine_VF::infobord()
{
  Cerr << "==============================================" << finl;
  Cerr << "The boundary areas of the domain " << domaine().le_nom() << " are:" << finl;
  DoubleVect surfaces;

  // Raccords
  Raccords& raccords=domaine().faces_raccord();
  for (int i=0; i<raccords.nb_raccords(); i++)
    {
      Faces& faces=raccords(i)->faces();
      faces.associer_domaine(domaine());
      faces.calculer_surfaces(surfaces);
      double s=0;
      for (int j=0; j<faces.nb_faces(); j++)
        s=s+surfaces(j);
      s=mp_sum(s);
      raccords(i)->set_aire(s);
      Cerr<<"Area of "<<raccords(i).le_nom()<<"  \t= "<<s<<finl;
    }

  // Bords
  Bords& bords=domaine().faces_bord();
  for (int i=0; i<bords.nb_bords(); i++)
    {
      Faces& faces=bords(i).faces();
      faces.associer_domaine(domaine());
      faces.calculer_surfaces(surfaces);
      double s=0;
      for (int j=0; j<faces.nb_faces(); j++)
        s=s+surfaces(j);
      s=mp_sum(s);
      bords(i).set_aire(s);
      Cerr<<"Area of "<<bords(i).le_nom()<<"  \t= "<<s<<finl;
    }
  Cerr << "==============================================" << finl;
}

void Domaine_VF::info_elem_som()
{
  const trustIdType nbelem = domaine().les_elems().get_md_vector()->nb_items_seq_tot();
  const trustIdType nbsom = domaine().les_sommets().get_md_vector()->nb_items_seq_tot();
  const trustIdType nbfaces = face_voisins().get_md_vector()->nb_items_seq_tot();
  Cerr<<"Calculation of elements and nodes on " << domaine().le_nom() << " :" << finl;
  Cerr<<"Total number of elements = "<<nbelem<<finl;
  Cerr<<"Total number of nodes = "<<nbsom<<finl;
  Cerr<<"Total number of faces = "<<nbfaces<<finl;
  Raccords& raccords=domaine().faces_raccord();
  for (int i=0; i<raccords.nb_raccords(); i++)
    {
      trustIdType nb_boundary_faces = mp_sum(ref_cast(Frontiere,raccords(i).valeur()).nb_faces());
      Cerr<< nb_boundary_faces << " of them on boundary "<<raccords(i).le_nom()<<finl;

    }
  Bords& bords=domaine().faces_bord();
  for (int i=0; i<bords.nb_bords(); i++)
    {
      trustIdType nb_boundary_faces = mp_sum(ref_cast(Frontiere,bords(i)).nb_faces());
      Cerr<< nb_boundary_faces << " of them on boundary "<<bords(i).le_nom()<<finl;
    }
  Cerr<<"=============================================="<<finl;
  trustIdType internal_item = std::min(nbelem, nbfaces);
  internal_item = std::min(internal_item, nbsom);
  // premiere_face_int()*dimension+1 pour ne pas alerter sur flux vectoriels aux faces frontieres:
  DeviceMemory::internal_items_size_ = std::max((trustIdType)(premiere_face_int()*dimension+1),internal_item);
}

void Domaine_VF::creer_tableau_faces(Array_base& t, RESIZE_OPTIONS opt) const
{
  MD_Vector_tools::creer_tableau_distribue(md_vector_faces_, t, opt);
}

void Domaine_VF::creer_tableau_aretes(Array_base& t, RESIZE_OPTIONS opt) const
{
  const MD_Vector& md = md_vector_aretes();
  MD_Vector_tools::creer_tableau_distribue(md, t, opt);
}

void Domaine_VF::creer_tableau_faces_bord(Array_base& t, RESIZE_OPTIONS opt) const
{
  const MD_Vector& md = md_vector_faces_bord();
  MD_Vector_tools::creer_tableau_distribue(md, t, opt);
}

void Domaine_VF::remplir_face_numero_bord()
{
  Cerr << "Domaine_VF::remplir_face_numero_bord" << finl;
  face_numero_bord_.resize(nb_faces());
  face_numero_bord_=-1; // init for inner faces.
  Domaine& ledomaine=domaine();
  int ndeb, nfin;
  const int nb_bords = ledomaine.nb_bords();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Bord& le_bord = ledomaine.bord(n_bord);
      ndeb = le_bord.num_premiere_face();
      nfin = ndeb + le_bord.nb_faces();
      for (int num_face=ndeb; num_face<nfin; num_face++)
        face_numero_bord_[num_face] = n_bord;
    }

  const int nb_raccords = ledomaine.nb_raccords() ;
  for (int n_racc=0; n_racc<nb_raccords; n_racc++)
    {
      const Raccord& le_racc = ledomaine.raccord(n_racc);
      ndeb = le_racc -> num_premiere_face();
      nfin = ndeb + le_racc -> nb_faces();
      for (int num_face=ndeb; num_face<nfin; num_face++)
        face_numero_bord_[num_face] = n_racc + nb_bords;
    }
}

const DoubleTab& Domaine_VF::xv_bord() const
{
  if (xv_bord_.get_md_vector() == md_vector_faces_bord()) return xv_bord_; //deja cree
  xv_bord_.resize(0, dimension), creer_tableau_faces_bord(xv_bord_);
  std::copy(xv_.addr(), xv_.addr() + dimension * premiere_face_int(), xv_bord_.addr()); //faces reelles : le debut de xv_
  xv_bord_.echange_espace_virtuel();
  return xv_bord_;
}

/*! @brief calcul le tableau xgr pour le calcul des moments des forces aux bords :
 *
 *
 */
DoubleTab Domaine_VF::calculer_xgr() const
{
  const DoubleTab& xgrav = xv();
  const ArrOfDouble& c_grav=domaine().cg_moments();
  int nb_faces = premiere_face_int();
  DoubleTab xgr(nb_faces, dimension);
  for (int num_face=0; num_face <nb_faces; num_face++)
    for (int i=0; i<dimension; i++)
      xgr(num_face,i)=xgrav(num_face,i)-c_grav[i];
  return xgr;
}

/** Build internal MEDCoupling cartesian mesh and correspondances
 *
 */
void Domaine_VF::build_map_mc_Cmesh(const bool with_faces)
{
#ifdef MEDCOUPLING_

  if (domaine().type_elem()->que_suis_je() != "Quadrangle" && domaine().type_elem()->que_suis_je() != "Rectangle"
      && domaine().type_elem()->que_suis_je() != "Hexaedre")
    {
      Cerr << "Error in Domaine_VF::build_mc_Cmesh ! You use the interpret Build_Map_to_Structured but your elem type is " << domaine().type_elem()->que_suis_je() << " !!!" << finl;
      Cerr << "This option is only available for Quadrangle/Rectangle/Hexaedre types !!! Remove this interpret from your data file." << finl;
      Process::exit();
    }

  Cerr << finl;
  Cerr << "###########################################" << finl;
  Cerr << "@@@@@@@@ Building Structured infos @@@@@@@@" << finl;
  Cerr << "###########################################" << finl;

  /* Step 1. build mesh */
  build_mc_Cmesh();

  /* Step 2. build nodes correspondance : indx est i + nx * (j + ny * k) */
  build_mc_Cmesh_nodesCorrespondence();

  /* Step 2. build elem and face correspondance : indx est i + (nx - 1) * (j + (ny - 1) * k) */
  build_mc_Cmesh_correspondence(with_faces);

  mc_Cmesh_with_faces_corr_ = with_faces;
  mc_Cmesh_ready_ = true;

  Cerr << "##################################################" << finl;
  Cerr << "@@@@@@@@ End of Building Structured infos @@@@@@@@" << finl;
  Cerr << "##################################################" << finl;
  Cerr << finl;

#else
  Cerr << "Error in Domaine_VF::build_mc_Cmesh ! You use the interpret Build_Map_to_Structured but your TRUST version is compiled without MEDCOUPLING !!!" << finl;
  Process::exit();
#endif
}

#ifdef MEDCOUPLING_

/** Build internal MEDCoupling cartesian mesh for various postprocessings.
 */
void Domaine_VF::build_mc_Cmesh()
{
  using DAD = MCAuto<DataArrayDouble>;

  Cerr << "Domaine_VF::build_mc_Cmesh() ... " << finl;
  const auto& u_mesh = domaine().get_mc_mesh(); // will be built
  const auto& coords = u_mesh->getCoords();
  trustIdType nb_soms_tmp = coords->getNumberOfTuples(), nb_elems_tmp = u_mesh->getNumberOfCells();

  const int nb_soms = Process::check_int_overflow(nb_soms_tmp);
  const int nb_elems = Process::check_int_overflow(nb_elems_tmp);
  const int mesh_dim = u_mesh->getMeshDimension();

  Cerr << "Domaine_VF: Creating a MEDCouplingCMesh object for the domaine_VF '" << que_suis_je() << "' & the domaine '" << domaine().le_nom() << "' ..." << finl;
  mc_Cmesh_ = MEDCouplingCMesh::New("TRUST_CMesh");

  const double eps = Objet_U::precision_geom;
  DAD coo[3], uniq[3];
  for (unsigned d=0; d < (unsigned)mesh_dim; d++)
    {
      coo[d] = coords->keepSelectedComponents({d});
      uniq[d] = coo[d]->getDifferentValues(eps);
      std::sort(uniq[d]->rwBegin(), uniq[d]->rwEnd());
    }
  mc_Cmesh_x_coords_.resize(uniq[0]->getNumberOfTuples());
  std::copy(uniq[0]->begin(), uniq[0]->end(),mc_Cmesh_x_coords_.begin());
  mc_Cmesh_y_coords_.resize(uniq[1]->getNumberOfTuples());
  std::copy(uniq[1]->begin(), uniq[1]->end(),mc_Cmesh_y_coords_.begin());
  if (mesh_dim>2)
    {
      mc_Cmesh_z_coords_.resize(uniq[2]->getNumberOfTuples());
      std::copy(uniq[2]->begin(), uniq[2]->end(),mc_Cmesh_z_coords_.begin());
    }

  const int nx = (int)mc_Cmesh_x_coords_.size(), ny = (int)mc_Cmesh_y_coords_.size();
  const int nz = (mesh_dim > 2) ? (int)mc_Cmesh_z_coords_.size() : 1;

  const int nb_som_cart = nx * ny * nz;
  const int nb_elem_cart = (mesh_dim > 2) ? (nx - 1) * (ny - 1) * (nz - 1) : (nx - 1) * (ny - 1);

  if (nb_som_cart != nb_soms || nb_elem_cart != nb_elems)
    {
      Cerr << "Issue in converting the TRUST mesh to MEDCouplingCMesh ... It seems that the mesh is not an IJK-like mesh !!!" << finl;
      Cerr << "Nb soms : " << nb_soms << " for TRUST mesh vs " << nb_som_cart << " for the MEDCouplingCMesh" << finl;
      Cerr << "Nb elems : " << nb_elems << " for TRUST mesh vs " << nb_elem_cart << " for the MEDCouplingCMesh" << finl;
      Process::exit();
    }

  Cerr << "Structured mesh dimensions (nodes) : " << nx << " x " << ny;
  if (mesh_dim > 2) Cerr << " x " << nz;
  Cerr << finl;

  if (mesh_dim > 2)
    mc_Cmesh_->setCoords(uniq[0], uniq[1], uniq[2]);
  else
    mc_Cmesh_->setCoords(uniq[0], uniq[1]);

  Cerr << "Domaine_VF::build_mc_Cmesh() ... OK !" << finl;
}

/** Correspondance between TRUST_CMesh elements (and faces) and TRUST
 *
 */
void Domaine_VF::build_mc_Cmesh_correspondence(bool withFace)
{
  using DAI = MCAuto<DataArrayIdType>;
  using DAD = MCAuto<DataArrayDouble>;

  Cerr << "Domaine_VF::build_mc_Cmesh_correspondence() building elem (and potentially face) correspondence ... " << finl;

  // Temporary unstruct  version of the CMesh to establish correspondences
  // The exact same numbering as in the original CMesh is preserved:
  MCAuto<MEDCouplingUMesh> mc_unstr = mc_Cmesh_->buildUnstructured();
  const MEDCouplingUMesh * mc_mesh = domaine().get_mc_mesh();

  // Renumber nodes of mc_unstr to use original TRUST node indices:
  mc_unstr->setCoords(mc_mesh->getCoords());
  DAI renumb(DataArrayIdType::New());
  mcIdType n_nod = mc_unstr->getCoords()->getNumberOfTuples();
  renumb->alloc(n_nod);
  std::copy(mc_Cmesh_nodesCorrespondence_.data(),mc_Cmesh_nodesCorrespondence_.data()+n_nod, renumb->rwBegin());

  mc_unstr->renumberNodesInConn(renumb->begin()); // only in connectivity

  // Identify elements
  DataArrayIdType * mP;
  mc_unstr->areCellsIncludedIn(mc_mesh, 2, mP);  // 2: same nodal connectivity

  // Check that we have covered all cells exactly once:
  DAI outliers = mP->findIdsNotInRange(0, mc_unstr->getNumberOfCells());
  if (outliers->getNumberOfTuples() != 0)
    Process::exit("ERROR: Issue in converting the TRUST mesh to MEDCouplingCMesh ... It seems that the mesh is not an IJK-like mesh!!!");

  mc_Cmesh_elemCorrespondence_.resize(mP->getNumberOfTuples());
  std::copy(mP->begin(), mP->end(), mc_Cmesh_elemCorrespondence_.data());

//  // Debug
//  Cerr << "ELEM CORRESPONDANCE" << finl;
//  int elem_idx = 0;
//  for (const auto& cor: mc_Cmesh_elemCorrespondence_)
//    Cerr << "Elem struct. " << elem_idx++ << " --> Elem non struct (original TRUST) " << cor << finl;

  if (withFace)
    {
      int mesh_dim = mc_unstr->getMeshDimension();
      get_mc_face_mesh();

      DAI desc(DataArrayIdType::New()), descIndx(DataArrayIdType::New()), revDesc(DataArrayIdType::New()), revDescIndx(DataArrayIdType::New());
      MCAuto<MEDCouplingUMesh> mc_unstr_fac(mc_unstr->buildDescendingConnectivity(desc, descIndx, revDesc, revDescIndx));
      mcIdType nb_fac = mc_unstr_fac->getNumberOfCells();

      DataArrayIdType * mapFac;
      mc_unstr_fac->areCellsIncludedIn(mc_face_mesh_, 2, mapFac);  // 2: same nodal connectivity

      DAD faceBary = mc_face_mesh_->computeCellCenterOfMass();
      const double * faceBaryP = faceBary->getConstPointer();
      DAI permu(DataArrayIdType::New());
      permu->alloc(nb_fac);
      permu->iota();

      auto comp_func_2d = [&](const int& i1, const int& i2) -> bool
      {
        std::array<double, 2> tup1 = { faceBaryP[i1*2+1], faceBaryP[i1*2+0] }; // Y, X
        std::array<double, 2> tup2 = { faceBaryP[i2*2+1], faceBaryP[i2*2+0] };
        return tup1 < tup2;  // use **reverse** lexicographic ordering natively given by std::array
      };

      auto comp_func_3d = [&](const int& i1, const int& i2) -> bool
      {
        std::array<double, 3> tup1 = { faceBaryP[i1*3+2], faceBaryP[i1*3+1], faceBaryP[i1*3+0] }; // Z, Y, X
        std::array<double, 3> tup2 = { faceBaryP[i2*3+2], faceBaryP[i2*3+1], faceBaryP[i2*3+0] }; // Z, Y, X
        return tup1 < tup2;  // use **reverse** lexicographic ordering natively given by std::array
      };

      if (mesh_dim == 2)
        std::sort(permu->rwBegin(), permu->rwEnd(), comp_func_2d);
      else
        std::sort(permu->rwBegin(), permu->rwEnd(), comp_func_3d);
      for (mcIdType pp: *permu) // loop on face following lexicographic ordering
        {
          int p = static_cast<int>(pp);
          const int ori = orientation(p); // normal
          if (ori == 0) mc_Cmesh_facesXCorrespondence_.push_back(p);
          if (ori == 1) mc_Cmesh_facesYCorrespondence_.push_back(p);
          if (mesh_dim > 2 && ori == 2) mc_Cmesh_facesZCorrespondence_.push_back(p);
        }

      Cerr << "   - Built mc_Cmesh_facesXCorrespondence_ with size " << (int)mc_Cmesh_facesXCorrespondence_.size() << finl;
      Cerr << "   - Built mc_Cmesh_facesYCorrespondence_ with size " << (int)mc_Cmesh_facesYCorrespondence_.size() << finl;
      if (mesh_dim > 2)
        Cerr << "   - Built mc_Cmesh_facesZCorrespondence_ with size " << (int)mc_Cmesh_facesZCorrespondence_.size() << finl;
    }
  Cerr << "Domaine_VF::build_mc_Cmesh_correspondence() - done!" << finl;
}


// magic !
static int findCartIndex(const std::vector<double>& vect, const double value)
{
  if (value < vect.front() || value > vect.back())
    return -1; // pas dedans

  auto it = std::lower_bound(vect.begin(), vect.end(), value);

  if (it == vect.end())
    return -1; // pas dedans

  int index = static_cast<int>(std::distance(vect.begin(), it));

  if (index > 0 && value < vect[index])
    index--; // juste avant !!

  return index;
}

/** Correspondance between TRUST_CMesh nodes and TRUST
 */
void Domaine_VF::build_mc_Cmesh_nodesCorrespondence()
{
  Cerr << "Domaine_VF::build_mc_Cmesh_nodesCorrespondence() ... " << finl;
  const auto& u_mesh = domaine().get_mc_mesh();
  const double* coordsP = u_mesh->getCoords()->getConstPointer();
  const int space_dim = static_cast<int>(u_mesh->getCoords()->getNumberOfComponents());
  const int nb_soms = static_cast<int>(u_mesh->getNumberOfNodes());
  const int mesh_dim = u_mesh->getMeshDimension();
  const int nx = (int)mc_Cmesh_x_coords_.size(), ny = (int)mc_Cmesh_y_coords_.size();

  mc_Cmesh_nodesCorrespondence_.resize(nb_soms, -1);

  for (int node = 0; node < nb_soms; ++node)
    {
      int i = findCartIndex(mc_Cmesh_x_coords_, coordsP[node*space_dim + 0]);
      int j = findCartIndex(mc_Cmesh_y_coords_, coordsP[node*space_dim + 1]);
      int k = (mesh_dim > 2) ? findCartIndex(mc_Cmesh_z_coords_, coordsP[node*space_dim + 2]) : 0;

      if (i == -1 || j == -1 || (mesh_dim > 2 && k == -1))
        {
          Cerr << "Node " << node << " not contained in the structured mesh !!!" << finl;
          Process::exit();
        }

      const int ijk_idx = i + nx * (j + ny * k); // Indexing for structured mesh nodes
      mc_Cmesh_nodesCorrespondence_[ijk_idx] = node;
//      Cerr << "Noeud non struct. " << node << " --> Noeud struct. " << ijk_idx << finl;
    }
  Cerr << "Domaine_VF::build_mc_Cmesh_nodesCorrespondence() ... OK !" << finl;
}

#endif

void Domaine_VF::get_position(DoubleTab& positions) const
{
  positions.resize(nb_elem(), xp_.dimension(1));
  CDoubleTabView xp = xp_.view_ro();
  DoubleTabView positions_v = positions.view_wo();
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), range_2D({0,0}, {nb_elem(), xp_.dimension(1)}), KOKKOS_LAMBDA(const int i, const int j)
  {
    positions_v(i,j) = xp(i,j);
  });
  end_gpu_timer(__KERNEL_NAME__);
  // Don't work with simply: ToDo fix
  // positions = zvf.xp();
}

double Domaine_VF::compute_L1_norm(const DoubleVect& val_source) const
{
  double sum = 0.;
  const int ne = nb_elem();
  for (int i=0; i<ne; i++)
    {
      sum+=std::fabs(val_source(i))*volumes(i);
    }
  return sum;
}

double Domaine_VF::compute_L2_norm(const DoubleVect& val_source) const
{
  double sum = 0.;
  const int ne = nb_elem();
  for (int i=0; i<ne; i++)
    {
      sum+=val_source(i)*val_source(i)*volumes(i);
    }
  return sum;
}

void Domaine_VF::get_nb_integ_points(IntTab& ) const
{
  Process::exit("The function should not be used in domaine_VF, related to quadrature points (DG discretization)");
  //surcharge dans domaine_DG mais qui n'est pas dans kernel
}

void Domaine_VF::get_ind_integ_points(IntTab& ) const
{
  Process::exit("The function should not be used in domaine_VF, related to quadrature points (DG discretization)");
  //surcharge dans domaine_DG mais qui n'est pas dans kernel
}

/*! Methode inspiree de Raccord_distant_homogene::initialise
 */
void Domaine_VF::init_dist_paroi_globale(const Conds_lim& conds_lim)
{
  if(dist_paroi_initialisee_) return;

  const Domaine_VF& domaine_ = *this;
  int D=Objet_U::dimension, nf = domaine_.nb_faces(), ne = domaine_.nb_elem();
  const IntTab& f_s = face_sommets();
  const DoubleTab& xs = domaine_.domaine().coord_sommets();

  // On initialise les tables y_faces_ et y_elem_
  domaine_.creer_tableau_faces(y_faces_);
  domaine_.domaine().creer_tableau_elements(y_elem_);

  n_y_elem_.resize(0,D);
  n_y_faces_.resize(0,D);

  MD_Vector_tools::creer_tableau_distribue(y_elem_.get_md_vector(), n_y_elem_);
  MD_Vector_tools::creer_tableau_distribue(y_faces_.get_md_vector(), n_y_faces_);

  // On va identifier les faces par leur centres de gravite
  int parts = Process::nproc();
  int moi = Process::me();
  DoubleTabs remote_xv(parts);

  // On initialise la table de faces/sommets/aretes de bords locale, on cree une table de sommets locale et on compte les aretes
  int nb_faces_bord_ = 0;
  int nb_aretes = 0;
  std::set<int> soms;
  for (auto& itr : conds_lim)
    if ( sub_type(Dirichlet_paroi_defilante, itr.valeur()) || sub_type(Dirichlet_homogene, itr.valeur()) ||
         (sub_type(Navier, itr.valeur()) && !sub_type(Symetrie, itr.valeur()) ) || sub_type(Dirichlet_loi_paroi, itr.valeur()))
      {
        int num_face_1_cl = itr->frontiere_dis().frontiere().num_premiere_face();
        int nb_faces_cl   = itr->frontiere_dis().frontiere().nb_faces();

        nb_faces_bord_ += itr->frontiere_dis().frontiere().nb_faces();

        for (int f=num_face_1_cl ; f < nb_faces_cl+num_face_1_cl ; f++)
          {
            int nb_som_loc = 0;
            while ( (nb_som_loc < nb_som_face()) && (f_s(f, nb_som_loc) != -1))
              {
                soms.insert(f_s(f, nb_som_loc));
                nb_som_loc++;
              }
            nb_aretes += (D == 3 ? nb_som_loc : 0)  ; // Autant d'aretes autour d'une face que de sommets !
          }
      }

  if (Process::mp_max(nb_faces_bord_) == 0) /* test all procs */
    Process::exit(que_suis_je() + " : at least one boundary must be solid for the distance to the edge to be calculated !!!");

  remote_xv[moi].resize(nb_faces_bord_ + (int)soms.size() + nb_aretes,D);

  // On remplit les coordonnes des faces et aretes de bord locales
  int ind_tab = 0 ; // indice de la face/sommet/arete dans le tableau
  for (int ind_cl = 0 ; ind_cl < conds_lim.size() ; ind_cl++)
    if ( sub_type(Dirichlet_paroi_defilante, conds_lim[ind_cl].valeur()) || sub_type(Dirichlet_homogene, conds_lim[ind_cl].valeur()) || (sub_type(Navier, conds_lim[ind_cl].valeur()) && !sub_type(Symetrie, conds_lim[ind_cl].valeur()) ) )
      {
        int num_face_1_cl = conds_lim[ind_cl]->frontiere_dis().frontiere().num_premiere_face();
        int nb_faces_cl   = conds_lim[ind_cl]->frontiere_dis().frontiere().nb_faces();

        for (int f=num_face_1_cl ; f < nb_faces_cl+num_face_1_cl ; f++)
          {
            for (int d=0 ; d<D ; d++)
              remote_xv[moi](ind_tab,d) = domaine_.xv(f, d); // Remplissage des faces
            ind_tab++;

            if (D==3) // Remplissage des aretes
              {
                int id_som = 1 ;
                while ( (id_som < nb_som_face()) && (f_s(f, id_som) != -1))
                  {
                    for (int d=0 ; d<D ; d++)
                      remote_xv[moi](ind_tab,d) = (xs(f_s(f, id_som), d) + xs(f_s(f, id_som-1), d)) / 2;
                    id_som++;
                    ind_tab++;
                  }
                for (int d=0 ; d<D ; d++)
                  remote_xv[moi](ind_tab,d) = (xs(f_s(f, 0), d) + xs(f_s(f, id_som-1), d)) / 2;
                ind_tab++;
              }
          }
      }

  for (auto som:soms) // Remplissage des sommets
    {
      for (int d=0 ; d<D ; d++)
        remote_xv[moi](ind_tab,d) = xs(som, d);
      ind_tab++;
    }

  // Puis on echange les tableaux des centres de gravites
  // envoi des tableaux
  for (int p = 0; p < parts; p++)
    envoyer_broadcast(remote_xv[p], p);

#ifdef MEDCOUPLING_
  // On traite les informations, chaque proc connait tous les XV

  // On boucle sur toutes les faces puis tous les elems
  const DoubleTab& local_xv = domaine_.xv(),
                   & local_xp = domaine_.xp();

  //DataArrayDoubles des xv locaux et de tous les remote_xv (a la suite)
  std::vector<MCAuto<DataArrayDouble> > vxv(parts);
  std::vector<const DataArrayDouble*> cvxv(parts);
  for (int p = 0; p < parts; p++)
    {
      vxv[p] = DataArrayDouble::New();
      vxv[p]->useExternalArrayWithRWAccess(remote_xv[p].addr(), remote_xv[p].dimension(0), remote_xv[p].dimension(1));
      cvxv[p] = vxv[p];
    }
  MCAuto<DataArrayDouble> remote_xvs(DataArrayDouble::Aggregate(cvxv)), local_xs(DataArrayDouble::New());
  local_xs->alloc(nf+ne, D);
  for (int f = 0; f < nf; f++)
    for (int d = 0; d < D; d++)
      local_xs->setIJ(f, d, local_xv(f, d));
  for (int e = 0; e < ne; e++)
    for (int d = 0; d < D; d++)
      local_xs->setIJ(nf+e, d, local_xp(e, d));

  //indices des points de remote_xvs les plus proches de chaque point de local_xv
  MCAuto<DataArrayIdType> glob_idx(DataArrayIdType::New());
  glob_idx = remote_xvs->findClosestTupleId(local_xs);

  //pour chaque element et face de local_xs : remplissage des tableaux
  for (int fe = 0; fe<nf+ne; fe++)
    {
      //retour de l'indice global (glob_idx(ind_face)) au couple (proc, ind_face2)
      int proc = 0;
      mcIdType fe2_big = glob_idx->getIJ(fe, 0);
      while (fe2_big >= remote_xv[proc].dimension(0))
        {
          fe2_big -= remote_xv[proc].dimension(0);
          proc++;
        }
      assert(fe2_big < std::numeric_limits<int>::max());
      int fe2 = (int)fe2_big;
      assert(fe2 <  remote_xv[proc].dimension(0));

      double distance2 = 0;
      for (int d=0; d<D; d++)
        {
          double x1 = 0 ;
          if (fe<nf) x1=local_xv(fe,d);
          else if (fe<nf+ne) x1=local_xp(fe-nf,d);
          else { Cerr<<"Domaine_Poly_base::init_dist_bord : problem in the ditance to the edge calculation. Contact TRUST support."<<finl; Process::exit();}
          double x2=remote_xv[proc](fe2,d);
          distance2 += (x1-x2)*(x1-x2);
        }
      if (fe<nf)
        {
          y_faces_(fe) = std::sqrt(distance2);
          if (y_faces_(fe) > Objet_U::precision_geom)
            for (int d = 0 ; d<D ; d++)
              n_y_faces_(fe, d) = ( local_xv(fe,d)-remote_xv[proc](fe2,d) )/ y_faces_(fe);
        }
      else
        {
          y_elem_(fe-nf)  = std::sqrt(distance2);
          for (int d = 0 ; d<D ; d++)
            n_y_elem_(fe-nf, d) = ( local_xp(fe-nf,d)-remote_xv[proc](fe2,d) )/ y_elem_(fe-nf);
        }
    }

#else
  Cerr<<"Domaine_Poly_base::init_dist_bord needs TRUST compiled with MEDCoupling."<<finl;
  exit();
#endif

  // Pour les elems de bord, on calcule la distance de facon propre avec le produit scalaire
  for (int ind_cl = 0 ; ind_cl < conds_lim.size() ; ind_cl++)
    if ( sub_type(Dirichlet_paroi_defilante, conds_lim[ind_cl].valeur()) || sub_type(Dirichlet_homogene, conds_lim[ind_cl].valeur()) || (sub_type(Navier, conds_lim[ind_cl].valeur()) && !sub_type(Symetrie, conds_lim[ind_cl].valeur()) ))
      {
        int num_face_1_cl = conds_lim[ind_cl]->frontiere_dis().frontiere().num_premiere_face();
        int nb_faces_cl   = conds_lim[ind_cl]->frontiere_dis().frontiere().nb_faces();

        for (int f=num_face_1_cl ; f < nb_faces_cl+num_face_1_cl ; f++)
          {
            const int ind = (face_voisins(f,0)>=0) ? face_voisins(f,0) : face_voisins(f,1) ;
            const double dist_ef_loc = (face_voisins(f,0)>=0) ? dist_face_elem0(f, face_voisins(f,0)) : dist_face_elem1(f, face_voisins(f,1));
            if ( dist_ef_loc < y_elem_(ind) ) // Prise en compte du cas ou l'element a plusieurs faces de bord
              {
                y_elem_(ind) = dist_ef_loc ;
                for (int d = 0 ; d < D ; d++)
                  n_y_elem_(ind, d) = -face_normales(f,  d)/face_surfaces(f);
              }
            for (int d = 0 ; d<D ; d++)
              n_y_faces_(f, d)                 = -face_normales(f,  d)/face_surfaces(f); // Coherent normal vector for border faces
          }
      }

  n_y_faces_.echange_espace_virtuel();
  n_y_elem_.echange_espace_virtuel();
  y_faces_.echange_espace_virtuel();
  y_elem_.echange_espace_virtuel();
  dist_paroi_initialisee_ = 1;
  Cerr <<"Initialize the y table " << domaine_.domaine().le_nom();
}

/*! @brief Build the MEDCoupling **face** mesh.
 *  It is always made of polygons (in 3D) for simplicity purposes.
 *  Face numbers (and node numbers) are the same as in TRUST.
 *
 *  It unfortunately needs a Domaine_dis_base since this is at this level that the face_sommets relationship is held ...
 *  As a consequence also the faces of a Domaine can not be postprocessed before discretisation.
 *  Another consequence is that it is not available for 64 bits domains.
 */
void Domaine_VF::build_mc_face_mesh() const
{
#ifdef MEDCOUPLING_
  using MEDCoupling::DataArrayIdType;
  using MEDCoupling::DataArrayDouble;

  using DAId = MCAuto<DataArrayIdType>;

  const MEDCouplingUMesh* mc_mesh = domaine().get_mc_mesh();

  // Build descending connectivity and convert it to polygons
  DAId desc(DataArrayIdType::New()), descIndx(DataArrayIdType::New()), revDesc(DataArrayIdType::New()), revDescIndx(DataArrayIdType::New());
  mc_face_mesh_ = mc_mesh->buildDescendingConnectivity(desc, descIndx, revDesc, revDescIndx);
  if (Objet_U::dimension == 3) mc_face_mesh_->convertAllToPoly();

  // Build second temporary mesh (with shared nodes!) having the TRUST connectivity
  MCAuto<MEDCouplingUMesh> faces_tmp = mc_face_mesh_->deepCopyConnectivityOnly();
  const IntTab& faces_sommets = face_sommets();
  int nb_fac = faces_sommets.dimension(0);
  int max_som_fac = faces_sommets.dimension_int(1);
  assert((int)mc_face_mesh_->getNumberOfCells() == nb_fac);
  DAId c(DataArrayIdType::New()), cI(DataArrayIdType::New());
  c->alloc(0,1);
  cI->alloc(nb_fac+1, 1);
  mcIdType *cIP = cI->getPointer();
  cIP[0] = 0; // better not forget this ...
  mcIdType typ = Objet_U::dimension == 3 ? INTERP_KERNEL::NormalizedCellType::NORM_POLYGON : INTERP_KERNEL::NormalizedCellType::NORM_SEG2;

  for (int fac=0; fac<nb_fac; fac++)  // Fills the two MC arrays c and cI describing the connectivity of the face mesh
    {
      c->pushBackSilent(typ);
      int i=0, s=-1;
      for (; i <  max_som_fac && (s = faces_sommets(fac, i)) >= 0; i++)
        c->pushBackSilent(s);
      cIP[fac+1] = cIP[fac] + i + 1; // +1 because of type
    }
  faces_tmp->setConnectivity(c, cI);

  // Then we can simply identify cells by their nodal connectivity:
  DataArrayIdType * mP;
  mc_face_mesh_->areCellsIncludedIn(faces_tmp,2, mP);    // 2: same nodal connectivity
  // DAId renum(mP); //useful to automatically free memory allocated in mP
  DAId renum2(mP->invertArrayN2O2O2N(nb_fac));
#ifndef NDEBUG
  // All cells should be found:
  DAId outliers = renum2->findIdsNotInRange(0, nb_fac);
  if (outliers->getNumberOfTuples() != 0)
    Process::exit("Invalid renumbering arrays! Should not happen. Some faces could not be matched between the TRUST face domain and the buildDescendingConnectivity() version.");
#endif

#ifdef NDEBUG
  bool check = false;
#else
  bool check = true;
#endif
  // Apply the renumbering so that final mc_face_mesh_ has the same face number as in TRUST
  mc_face_mesh_->renumberCells(renum2->begin(), check);
#ifndef NDEBUG
  mc_face_mesh_->checkConsistency();
#endif
  mP->decrRef();

  mc_face_mesh_ready_ = true;
#endif // MEDCOUPLING_
}

/** @brief Build the dual mesh of the domain for post-processing of face fields.
 *
 * For each face of each element, a polyhedron is built with faces being made of
 *   - the original face
 *   - triangles buit with the element center of mass and each of the segment of the original face
 * Thus an inner face of the domain has two associated dual polyhedral elements, and a boundary
 * face has only one associated dual element.
 * The member face_dual_ is also completed and has the same logic and exact same ordering as face_voisins_.
 */
void Domaine_VF::build_mc_dual_mesh() const
{
#ifdef MEDCOUPLING_
  using DAId = MCAuto<DataArrayIdType>;
  using DAD = MCAuto<DataArrayDouble>;

  const MEDCouplingUMesh* mc_face_mesh = get_mc_face_mesh();
  const int dim = Objet_U::dimension;

  Cerr << "   Domaine: Creating **dual** mesh as a MEDCouplingUMesh object for the domain '" << le_nom() << "'" << finl;

  // Fills in connectivity for the dual mesh, and correspondance array 'face_dual_'
  int nb_fac = Process::check_int_overflow(mc_face_mesh->getNumberOfCells());
  int nb_elem = domaine().nb_elem(); // real only
  int nb_coo = domaine().nb_som();
  const mcIdType *fc = mc_face_mesh->getNodalConnectivity()->getConstPointer(),
                  *fcI= mc_face_mesh->getNodalConnectivityIndex()->getConstPointer();
  // Init face_dual_
  face_dual_.resize(nb_fac, 2);
  face_dual_=-1;
  // Prepare raw MC connectivity of the dual mesh:
  DAId c(DataArrayIdType::New()), cI(DataArrayIdType::New());
  cI->alloc(nb_fac*2, 1); // Conservative pre-allocation (too large because of bound faces)
  c->alloc(0,1);
  mcIdType *cIP = cI->getPointer();
  cIP[0] = 0; // better not forget this ...
  int gi = 0; // global index of dual elements created

  for(int f=0; f<nb_fac; f++)    // For all the (real) faces
    {
      int e1=face_voisins_(f, 0), e2=face_voisins(f, 1);
      for (int e: {e1, e2})
        {
          if (e==-1 || e >= nb_elem) continue;  // skip boundary or virtual
          mcIdType c_sz = c->getNumberOfTuples();
          if (dim == 2)  // easy, just triangles to build
            {
              c->reAlloc(c_sz + 3 + 1); // will add a triangle
              mcIdType *cP = c->getPointer();
              cP[c_sz++] = INTERP_KERNEL::NormalizedCellType::NORM_TRI3;
              // Triangle connectivity:
              const mcIdType *p = fc + fcI[f] + 1;
              cP[c_sz++] = *p;
              cP[c_sz++] = *(p+1);
              cP[c_sz++] = e+nb_coo; // index of the barycenter in the final coord array
            }
          else // 3D
            {
              mcIdType nb_pts = fcI[f+1]-fcI[f]-1;  // nb of points in face f (-1 to skip type)
              // Increase size of c to fit for the next poyhedron to be inserted:
              // +1 : for the type
              // (nb_pts-1)=nb of segs in face,
              // (nb_pts-1)*(3+1)=nb of points for all triangles (segs in 2D) built on top of those segments
              //    +1 : to account for the '-1' separator between face in polyhedr conn,
              // final -1 because no '-1' at the end of the last face... pfffeww!
              c->reAlloc(c_sz + 1 + nb_pts + (nb_pts-1)*(3+1));

              mcIdType *cP = c->getPointer();
              cP[c_sz++] = INTERP_KERNEL::NormalizedCellType::NORM_POLYHED;
              // Add the face itself:
              for(const mcIdType *p = fc + fcI[f] + 1; p < fc+fcI[f + 1]; p++)
                cP[c_sz++] = *p;
              // Add the new faces containing the barycenter:
              const mcIdType *p2 = fc + fcI[f] + 1;
              for (mcIdType i = 0; i<nb_pts-1; i++)  // loop on all segs of the face - in 2D this will loop once only
                {
                  cP[c_sz++] = -1; // -1 to separate from the prev face in the polyhedron
                  cP[c_sz++] = e+nb_coo; // index of the barycenter in the final coord array
                  cP[c_sz++] = *(p2+i);
                  cP[c_sz++] = *(p2+(i+1)%nb_pts);
                }
            }
          assert(c_sz == c->getNumberOfTuples());  // check the awful realloc from above ...
          cIP[gi+1] = c_sz;
          face_dual_(f, e==e1 ? 0:1) = gi;
          gi++;  // next dual element
        }
    }

  // Strip cI that might be too big - surely a down-sizing
  cI->reAlloc(gi+1);
  // Prepare final coords - no choice here (sticking coords and xp_ together on TRUST side is a bad idea because of virtuals)
  // TODO - save/use coords for other (primal) meshes too
  DAD coo2 = mc_face_mesh->getCoords()->deepCopy();
  coo2->reAlloc(nb_coo+xp_.dimension(0));
  std::copy(xp_.addr(), xp_.addr() + nb_elem*dim, coo2->getPointer()+nb_coo*dim);

  const std::string dual_nam = domaine().get_mc_mesh()->getName() + "_dual";
  mc_dual_mesh_ = MEDCouplingUMesh::New(dual_nam, dim);
  mc_dual_mesh_->setCoords(coo2);
  mc_dual_mesh_->setConnectivity(c,cI);
  mc_dual_mesh_ready_ = true;
#endif // MEDCOUPLING_
}

