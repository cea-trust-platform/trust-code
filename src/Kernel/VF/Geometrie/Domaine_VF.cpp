/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Domaine_VF.h>
#include <Periodique.h>
#include <Conds_lim.h>
#include <Static_Int_Lists.h>
#include <Connectivite_som_elem.h>
#include <Faces_builder.h>
#include <Domaine.h>
#include <MD_Vector_tools.h>
#include <Scatter.h>
#include <MD_Vector_base.h>
#include <Analyse_Angle.h>
#include <Linear_algebra_tools_impl.h>

#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDCouplingMemArray.hxx>

using namespace MEDCoupling;
#endif
#include <Dirichlet_homogene.h>
#include <Dirichlet_paroi_defilante.h>
#include <Navier.h>
#include <Symetrie.h>
#include <Dirichlet_loi_paroi.h>

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
void Domaine_VF::reordonner(Faces& )
{
}


/*! @brief Genere les faces construits les frontieres
 *
 */
void Domaine_VF::discretiser()
{
  Cerr << "<<<<<<<<<< Discretization VF >>>>>>>>>>" << finl;

  Domaine_dis_base::discretiser();

  Domaine& ledomaine=domaine();
  histogramme_angle(ledomaine,Cerr);
  Faces* les_faces_ptr=creer_faces();
  Faces& les_faces= *les_faces_ptr;
  {
    {
      Type_Face type_face = domaine().type_elem().type_face(0);
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
    Scatter::calculer_renum_items_communs(ledomaine.faces_joint(), Joint::FACE);

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
    Scatter::construire_md_vector(domaine(), les_faces.nb_faces(), Joint::FACE, md_vector_faces_);


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

    // Calcul des surfaces:
    les_faces.calculer_surfaces(face_surfaces_);
    // Calcul de la surface des faces virtuelles
    MD_Vector md_nul;
    creer_tableau_faces(face_surfaces_);
    face_surfaces_.echange_espace_virtuel();
    face_surfaces_.set_md_vector(md_nul); // Detache la structure parallele

    // Changement a la v1.5.7 beta: xv_ a maintenant un descripteur parallele: dimension(0)=nb_faces
    les_faces.calculer_centres_gravite(xv_);

    face_voisins().ref(les_faces.voisins());
    face_sommets().ref(les_faces.les_sommets());

    // Calcul des volumes
    ledomaine.calculer_volumes(volumes_, inverse_volumes_);
  }
  {
    int i=0, derniere=ledomaine.nb_bords();
    les_bords_.dimensionner(domaine().nb_front_Cl()+domaine().nb_groupes_int());
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
    derniere+=domaine().nb_groupes_int();
    for(; i<derniere; i++)
      {
        int j=i-decal;
        les_bords_[i].associer_frontiere(ledomaine.groupe_interne(j));
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

  // Attention : ca ne remplit pas elem_faces, ca fait
  // creer_faces_virtuelles_non_std() en VEF :
  remplir_elem_faces();

  delete les_faces_ptr;

  // Fill in the table face_numero_bord
  //remplir_face_numero_bord();

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

  sous_domaine_dis(i).typer("Sous_domaine_VF");
  sous_domaine_dis(i).associer_sous_domaine(dom.ss_domaine(i));
  sous_domaine_dis(i).associer_domaine_dis(*this);
  sous_domaine_dis(i).discretiser();
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
  creer_tableau_faces(tmp, Array_base::NOCOPY_NOINIT);
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
  DoubleTab vector_face_elem(dimension);
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
      const IntTab& indices_faces_joint = joint_temp.joint_item(Joint::FACE).renum_items_communs();
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
      Faces& faces=raccords(i).valeur().faces();
      faces.associer_domaine(domaine());
      faces.calculer_surfaces(surfaces);
      double s=0;
      for (int j=0; j<faces.nb_faces(); j++)
        s=s+surfaces(j);
      s=mp_sum(s);
      raccords(i).valeur().set_aire(s);
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
  const int nbelem = domaine().les_elems().get_md_vector().valeur().nb_items_seq_tot();
  const int nbsom = domaine().les_sommets().get_md_vector().valeur().nb_items_seq_tot();
  const int nbfaces = face_voisins().get_md_vector().valeur().nb_items_seq_tot();
  Cerr<<"Calculation of elements and nodes on " << domaine().le_nom() << " :" << finl;
  Cerr<<"Total number of elements = "<<nbelem<<finl;
  Cerr<<"Total number of nodes = "<<nbsom<<finl;
  Cerr<<"Total number of faces = "<<nbfaces<<finl;
  Raccords& raccords=domaine().faces_raccord();
  for (int i=0; i<raccords.nb_raccords(); i++)
    {
      int nb_boundary_faces = mp_sum(ref_cast(Frontiere,raccords(i).valeur()).nb_faces());
      Cerr<< nb_boundary_faces << " of them on boundary "<<raccords(i).le_nom()<<finl;

    }
  Bords& bords=domaine().faces_bord();
  for (int i=0; i<bords.nb_bords(); i++)
    {
      int nb_boundary_faces = mp_sum(ref_cast(Frontiere,bords(i)).nb_faces());
      Cerr<< nb_boundary_faces << " of them on boundary "<<bords(i).le_nom()<<finl;
    }
  Cerr<<"=============================================="<<finl;
}

void Domaine_VF::creer_tableau_faces(Array_base& t, Array_base::Resize_Options opt) const
{
  MD_Vector_tools::creer_tableau_distribue(md_vector_faces_, t, opt);
}

void Domaine_VF::creer_tableau_aretes(Array_base& t, Array_base::Resize_Options opt) const
{
  const MD_Vector& md = md_vector_aretes();
  MD_Vector_tools::creer_tableau_distribue(md, t, opt);
}

void Domaine_VF::creer_tableau_faces_bord(Array_base& t, Array_base::Resize_Options opt) const
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
  const int nb_bords = ledomaine.nb_bords();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Bord& le_bord = ledomaine.bord(n_bord);
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      for (int num_face=ndeb; num_face<nfin; num_face++)
        face_numero_bord_[num_face] = n_bord;
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

/*! @brief renvoie le nombre total de faces sur lesquelles sont appliquees les conditions limites :
 *
 *  bords, raccords, plaques.
 *
 */
inline int Domaine_VF::nb_faces_bord_tot() const
{
  return md_vector_faces_bord().valeur().get_nb_items_tot();
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

void Domaine_VF::init_dist_paroi_globale(const Conds_lim& conds_lim) // Methode inspiree de Raccord_distant_homogene::initialise
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
        int num_face_1_cl = itr.frontiere_dis().frontiere().num_premiere_face();
        int nb_faces_cl   = itr.frontiere_dis().frontiere().nb_faces();

        nb_faces_bord_ += itr.frontiere_dis().frontiere().nb_faces();

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
        int num_face_1_cl = conds_lim[ind_cl].frontiere_dis().frontiere().num_premiere_face();
        int nb_faces_cl   = conds_lim[ind_cl].frontiere_dis().frontiere().nb_faces();

        for (int f=num_face_1_cl ; f < nb_faces_cl+num_face_1_cl ; f++)
          {
            for (int d=0 ; d<D ; d++) remote_xv[moi](ind_tab,d) = domaine_.xv(f, d); // Remplissage des faces
            ind_tab++;

            if (D==3) // Remplissage des aretes
              {
                int id_som = 1 ;
                while ( (id_som < nb_som_face()) && (f_s(f, id_som) != -1))
                  {
                    for (int d=0 ; d<D ; d++) remote_xv[moi](ind_tab,d) = (xs(f_s(f, id_som), d) + xs(f_s(f, id_som-1), d)) / 2;
                    id_som++;
                    ind_tab++;
                  }
                for (int d=0 ; d<D ; d++) remote_xv[moi](ind_tab,d) = (xs(f_s(f, 0), d) + xs(f_s(f, id_som-1), d)) / 2;
                ind_tab++;
              }
          }
      }

  for (auto som:soms) // Remplissage des sommets
    {
      for (int d=0 ; d<D ; d++) remote_xv[moi](ind_tab,d) = xs(som, d);
      ind_tab++;
    }

  // Puis on echange les tableaux des centres de gravites
  // envoi des tableaux
  for (int p = 0; p < parts; p++)
    envoyer_broadcast(remote_xv[p], p);

  VECT(ArrOfInt) racc_vois(parts);
  for (int p = 0; p < parts; p++)
    racc_vois[p].set_smart_resize(1);

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
  MCAuto<DataArrayInt> glob_idx(DataArrayInt::New());
  glob_idx = remote_xvs->findClosestTupleId(local_xs);

  //pour chaque element et face de local_xs : remplissage des tableaux
  for (int fe = 0; fe<nf+ne; fe++)
    {
      //retour de l'indice global (glob_idx(ind_face)) au couple (proc, ind_face2)
      int proc = 0, fe2 = glob_idx->getIJ(fe, 0);
      while (fe2 >= remote_xv[proc].dimension(0)) fe2 -= remote_xv[proc].dimension(0), proc++;
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
          if (y_faces_(fe)>1.e-8)
            for (int d = 0 ; d<D ; d++) n_y_faces_(fe, d) = ( local_xv(fe,d)-remote_xv[proc](fe2,d) )/ y_faces_(fe);
        }
      else
        {
          y_elem_(fe-nf)  = std::sqrt(distance2);
          for (int d = 0 ; d<D ; d++) n_y_elem_(fe-nf, d) = ( local_xp(fe-nf,d)-remote_xv[proc](fe2,d) )/ y_elem_(fe-nf);
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
        int num_face_1_cl = conds_lim[ind_cl].frontiere_dis().frontiere().num_premiere_face();
        int nb_faces_cl   = conds_lim[ind_cl].frontiere_dis().frontiere().nb_faces();

        for (int f=num_face_1_cl ; f < nb_faces_cl+num_face_1_cl ; f++)
          {
            const int ind = face_voisins(f, 0) > -1 ? face_voisins(f, 0) : face_voisins(f, 1);
            if ( dist_face_elem0(f, ind) < y_elem_(ind) ) // Prise en compte du cas ou l'element a plusieurs faces de bord
              {
                y_elem_(ind) = dist_face_elem0(f, ind) ;
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
