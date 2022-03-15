/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Zone_VF.cpp
// Directory:   $TRUST_ROOT/src/Kernel/VF/Zones
// Version:     /main/48
//
//////////////////////////////////////////////////////////////////////////////

#include <Zone_VF.h>
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


Implemente_base(Zone_VF,"Zone_VF",Zone_dis_base);

Sortie& Zone_VF::printOn(Sortie& os) const
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

  porosite_elem_.ecrit(os);
  porosite_face_.ecrit(os);
  return os ;
}

//// readOn
//

Entree& Zone_VF::readOn(Entree& is)
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
  porosite_elem_.lit(is);
  porosite_face_.lit(is);
  return is ;
}

// Description:
// cette methode ne fait rien
// elle est surchargee par Zone_VDF par ex.
void Zone_VF::reordonner(Faces& )
{
}


// Description:
// Genere les faces
// construits les frontieres
void Zone_VF::discretiser()
{
  Cerr << "<<<<<< Discretization VF >>>>>>>>>>" << finl;

  Zone& lazone=zone();
  histogramme_angle(lazone.domaine(),Cerr);
  Faces* les_faces_ptr=creer_faces();
  Faces& les_faces= *les_faces_ptr;
  {
    {
      Type_Face type_face = zone().type_elem().type_face(0);
      les_faces.typer(type_face);
      les_faces.associer_zone(zone());

      Static_Int_Lists connectivite_som_elem;
      const int     nb_sommets_tot = zone().domaine().nb_som_tot();
      const IntTab&    elements       = zone().les_elems();

      construire_connectivite_som_elem(nb_sommets_tot,
                                       elements,
                                       connectivite_som_elem,
                                       1 /* include virtual elements */);

      Faces_builder faces_builder;
      faces_builder.creer_faces_reeles(zone(),
                                       connectivite_som_elem,
                                       les_faces,
                                       elem_faces_);
    }

    reordonner(les_faces);

    // Les faces sont dans l'ordre definitif, on peut remplir
    // renum_items_communs des faces:
    Scatter::calculer_renum_items_communs(lazone.faces_joint(), Joint::FACE);

    // Remplissage de face_voisins des frontieres:
    // a factoriser avec DomaineCut.cpp
    {
      const IntTab& facevoisins = les_faces.voisins();
      const int nb_frontieres = lazone.nb_front_Cl();
      for (int i_frontiere = 0; i_frontiere < nb_frontieres; i_frontiere++)
        {
          Frontiere&   fr               = lazone.frontiere(i_frontiere);
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

    Scatter::calculer_espace_distant_faces(zone(), les_faces.nb_faces(), elem_faces_);
    // Apres la methode suivante, on aura le droit d'utiliser creer_tableau_faces() :
    Scatter::construire_md_vector(zone().domaine(), les_faces.nb_faces(), Joint::FACE, md_vector_faces_);


    const MD_Vector& md_vect_sommets = zone().domaine().les_sommets().get_md_vector();
    const MD_Vector& md_vect_elements = zone().les_elems().get_md_vector();
    // Construction de l'espace virtuel du tableau face_sommets
    Scatter::construire_espace_virtuel_traduction(md_vector_faces_, md_vect_sommets, les_faces.les_sommets());

    // Idem pour face_voisins_
    // Certaines faces des elements virtuels les plus externes n'ont pas d'element local voisin => erreurs non fatales
    Scatter::construire_espace_virtuel_traduction(md_vector_faces_, md_vect_elements, les_faces.voisins(), 0 /* error not fatal */);

    // Idem pour elem_faces_
    Scatter::construire_espace_virtuel_traduction(md_vect_elements, md_vector_faces_, elem_faces_);

    lazone.init_faces_virt_bord(md_vector_faces_, md_vector_faces_front_);

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
    lazone.calculer_volumes(volumes_, inverse_volumes_);
  }
  {
    int i=0, derniere=lazone.nb_bords();
    les_bords_.dimensionner(zone().nb_front_Cl());
    for(; i<derniere; i++)
      {
        les_bords_[i].associer_frontiere(lazone.bord(i));
        les_bords_[i].associer_Zone_dis(*this);
      }
    int decal=derniere;
    derniere+=zone().nb_raccords();
    for(; i<derniere; i++)
      {
        int j=i-decal;
        les_bords_[i].associer_frontiere(lazone.raccord(j).valeur());
        les_bords_[i].associer_Zone_dis(*this);
      }
    decal=derniere;
    derniere+=zone().nb_frontieres_internes();
    for(; i<derniere; i++)
      {
        int j=i-decal;
        les_bords_[i].associer_frontiere(lazone.faces_interne(j));
        les_bords_[i].associer_Zone_dis(*this);
      }
  }
  // Centre de gravite des elements (tableau xp_)
  lazone.calculer_centres_gravite(xp_);
  // Centre de gravite de la zone
  ArrOfDouble c(dimension);
  lazone.calculer_mon_centre_de_gravite(c);

  // On cree les domaines frontieres
  lazone.domaine().creer_mes_domaines_frontieres(*this);

  // Attention : ca ne remplit pas elem_faces, ca fait
  // creer_faces_virtuelles_non_std() en VEF :
  remplir_elem_faces();

  delete les_faces_ptr;

// Fill in the table face_numero_bord
  remplir_face_numero_bord();

  ///////////////////////
  // On imprime des infos
  ///////////////////////
  lazone.domaine().imprimer();        // Extremas du domaine et volumes
  infobord();                        // Aires des bords
  info_elem_som();                // Nombre elements et sommets
  Cerr << "<<<<<< End of Discretization VF >>>>>>>>>>" << finl;
}

void Zone_VF::remplir_face_voisins_fictifs(const Zone_Cl_dis_base& )
{
  Cerr <<" Zone_VF::remplir_face_voisins_fictifs(const Zone_Cl_dis_base& ) must be overloaded by" << que_suis_je()<<finl;
  exit();
  assert(0);
}



// Description:
// renvoie new(Faces) !
// elle est surchargee par Zone_VDF par ex.
Faces* Zone_VF::creer_faces()
{
  Faces* les_faces=new(Faces);
  return les_faces;
}

void Zone_VF::modifier_pour_Cl(const Conds_lim& conds_lim)
{
  Cerr << " Zone_VF::modifier_pour_Cl" << finl;
  int nb_cond_lim=conds_lim.size();
  int i=0;
  for (; i<nb_cond_lim; i++)
    {
      // for( cl ...
      const Cond_lim_base& cl=conds_lim[i].valeur();
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
void Zone_VF::construire_num_fac_loc()
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
// Utiliser si possible Zone_VF::num_fac_loc(face,voisin) car la methode est
// optimisee, celle ci non.
int Zone_VF::numero_face_local(int face, int elem) const
{
  //int nfe=zone().nb_faces_elem();
  // GF pour le cas ou on a plusieurs types de faces....
  // nb_faces_elem() renvoit le nbre de sommet par face du premier type de faces
  int nfe=elem_faces_.dimension(1);
  for(int face_loc=0; face_loc<nfe; face_loc++)
    if(elem_faces_(elem, face_loc)==face)
      return face_loc;
  return -1;
}


// remplissage des porosites
void Zone_VF::calculer_porosites()
{
  // Les porosites volumiques et surfacique valent 1

  zone().creer_tableau_elements(porosite_elem_, Array_base::NOCOPY_NOINIT);
  porosite_elem_ = 1;

  creer_tableau_faces(porosite_face_, Array_base::NOCOPY_NOINIT);
  porosite_face_ = 1;
}


// remplissage des diametres hydrauliques
void Zone_VF::calculer_diametres_hydrauliques()
{
  // les diametres hydrauliques valent 0
  creer_tableau_faces(diametre_hydraulique_face_, Array_base::NOCOPY_NOINIT);
  diametre_hydraulique_face_ = 0.;

  // diametre_hydraulique_elem_.resize(0,Objet_U::dimension);
  // Desormais diametre_hydraulique_elem_ scalaire
  zone().creer_tableau_elements(diametre_hydraulique_elem_, Array_base::NOCOPY_NOINIT);
  diametre_hydraulique_elem_ = 0.;
}

// remplissage des coefficients d'echange_thermique
void Zone_VF::calculer_coefficient_echange_thermique()
{
  // coefficient d'echange_thermique a 0
  int nbfaces = xv_.dimension(0);
  coefficient_echange_thermique_.resize(nbfaces);
  coefficient_echange_thermique_ = 0;
}

// remplissage des coefficient de frottement
void Zone_VF::calculer_coefficient_frottement()
{
  // coefficient de frottement a 0
  int nbfaces = xv_.dimension(0);
  coefficient_frottement_.resize(nbfaces);
  coefficient_frottement_= 0;
}

// Description:
//  Remplissage du tableau face_virt_pe_num_ (voir commentaire dans Zone_VF.h)
void Zone_VF::construire_face_virt_pe_num()
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

const IntTab& Zone_VF::face_virt_pe_num() const
{
  // On verifie que le tableau a ete construit (si ca plante, c'est qu'on a
  // oublie d'appeler construire_face_virt_pe_num()).
  assert(face_virt_pe_num_.dimension(1) == 2);
  return face_virt_pe_num_;
}

// Description:
// Compute the normalized boundary outward vector associated to the face global_face_number
// and eventually scaled by scale_factor
DoubleTab Zone_VF::normalized_boundaries_outward_vector(int global_face_number, double scale_factor) const
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

void Zone_VF::marquer_faces_double_contrib(const Conds_lim& conds_lim)
{
  Journal() << " Zone_VF::marquer_faces_double_contrib" << finl;
  faces_doubles_.resize_array(nb_faces());
  // marquage des faces periodiques
  ////////////////////////////////////////////////

  int nb_cond_lim=conds_lim.size();

  for (int i=0; i<nb_cond_lim; i++)
    {
      const Cond_lim_base& cl=conds_lim[i].valeur();
      if (sub_type(Periodique, cl))
        {
          const Periodique& la_cl_period = ref_cast(Periodique,cl);
          const Front_VF& frontieredis = ref_cast(Front_VF,la_cl_period.frontiere_dis());
          int ndeb = frontieredis.num_premiere_face();
          int nfin = ndeb + frontieredis.nb_faces();

          for (int face=ndeb; face<nfin; face++)
            {
              faces_doubles_(face)=1;
            }
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
          faces_doubles_(face_de_joint) = 1;
        }
    }
}
void Zone_VF::typer_elem(Zone& azone)
{
  //Ne fait rien
}

void Zone_VF::infobord()
{
  Cerr << "==============================================" << finl;
  Cerr << "The boundary areas of the domain " << zone().domaine().le_nom() << " are:" << finl;
  DoubleVect surfaces;

  // Raccords
  Raccords& raccords=zone().faces_raccord();
  for (int i=0; i<raccords.nb_raccords(); i++)
    {
      Faces& faces=raccords(i).valeur().faces();
      faces.associer_zone(zone());
      faces.calculer_surfaces(surfaces);
      double s=0;
      for (int j=0; j<faces.nb_faces(); j++)
        s=s+surfaces(j);
      s=mp_sum(s);
      raccords(i).valeur().set_aire(s);
      Cerr<<"Area of "<<raccords(i).le_nom()<<"  \t= "<<s<<finl;
    }

  // Bords
  Bords& bords=zone().faces_bord();
  for (int i=0; i<bords.nb_bords(); i++)
    {
      Faces& faces=bords(i).faces();
      faces.associer_zone(zone());
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

void Zone_VF::info_elem_som()
{
  const int nbelem = zone().les_elems().get_md_vector().valeur().nb_items_seq_tot();
  const int nbsom = zone().domaine().les_sommets().get_md_vector().valeur().nb_items_seq_tot();
  const int nbfaces = face_voisins().get_md_vector().valeur().nb_items_seq_tot();
  Cerr<<"Calculation of elements and nodes on " << zone().domaine().le_nom() << " :" << finl;
  Cerr<<"Total number of elements = "<<nbelem<<finl;
  Cerr<<"Total number of nodes = "<<nbsom<<finl;
  Cerr<<"Total number of faces = "<<nbfaces<<finl;
  Raccords& raccords=zone().faces_raccord();
  for (int i=0; i<raccords.nb_raccords(); i++)
    {
      int nb_boundary_faces = mp_sum(ref_cast(Frontiere,raccords(i).valeur()).nb_faces());
      Cerr<< nb_boundary_faces << " of them on boundary "<<raccords(i).le_nom()<<finl;

    }
  Bords& bords=zone().faces_bord();
  for (int i=0; i<bords.nb_bords(); i++)
    {
      int nb_boundary_faces = mp_sum(ref_cast(Frontiere,bords(i)).nb_faces());
      Cerr<< nb_boundary_faces << " of them on boundary "<<bords(i).le_nom()<<finl;
    }
  Cerr<<"=============================================="<<finl;
}

void Zone_VF::creer_tableau_faces(Array_base& t, Array_base::Resize_Options opt) const
{
  MD_Vector_tools::creer_tableau_distribue(md_vector_faces_, t, opt);
}

void Zone_VF::creer_tableau_aretes(Array_base& t, Array_base::Resize_Options opt) const
{
  const MD_Vector& md = md_vector_aretes();
  MD_Vector_tools::creer_tableau_distribue(md, t, opt);
}

void Zone_VF::creer_tableau_faces_bord(Array_base& t, Array_base::Resize_Options opt) const
{
  const MD_Vector& md = md_vector_faces_bord();
  MD_Vector_tools::creer_tableau_distribue(md, t, opt);
}

void Zone_VF::remplir_face_numero_bord()
{
  Cerr << " Zone_VF::remplir_face_numero_bord" << finl;
  face_numero_bord_.resize(nb_faces());
  face_numero_bord_=-1; // init for inner faces.
  Zone& lazone=zone();
  int ndeb, nfin, num_face;
  const int nb_bords = lazone.nb_bords();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Bord& le_bord = lazone.bord(n_bord);
      ndeb = le_bord.num_premiere_face();
      nfin = ndeb + le_bord.nb_faces();
      for (num_face=ndeb; num_face<nfin; num_face++)
        {
          face_numero_bord_[num_face] = n_bord;
        }
    }
}

const DoubleTab& Zone_VF::xv_bord() const
{
  if (xv_bord_.get_md_vector() == md_vector_faces_bord()) return xv_bord_; //deja cree
  xv_bord_.resize(0, dimension), creer_tableau_faces_bord(xv_bord_);
  std::copy(xv_.addr(), xv_.addr() + dimension * premiere_face_int(), xv_bord_.addr()); //faces reelles : le debut de xv_
  xv_bord_.echange_espace_virtuel();
  return xv_bord_;
}

// Description:
// renvoie le nombre total de faces sur lesquelles
// sont appliquees les conditions limites :
// bords, raccords, plaques.
inline int Zone_VF::nb_faces_bord_tot() const
{
  return md_vector_faces_bord().valeur().get_nb_items_tot();
}
