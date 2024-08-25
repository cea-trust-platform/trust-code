/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Connectivite_som_elem.h>
#include <Check_espace_virtuel.h>
#include <MD_Vector_composite.h>
#include <VEF_discretisation.h>
#include <EcrFicPartageBin.h>
#include <Static_Int_Lists.h>
#include <MD_Vector_tools.h>
#include <Domaine_Cl_VEF.h>
#include <Quadrangle_VEF.h>
#include <Octree_Double.h>
#include <Hexaedre_VEF.h>
#include <Domaine_VEF.h>
#include <EFichierBin.h>
#include <Quadri_VEF.h>
#include <Periodique.h>
#include <TRUSTLists.h>
#include <Periodique.h>
#include <Rectangle.h>
#include <Tetra_VEF.h>
#include <Conds_lim.h>
#include <Tetraedre.h>
#include <ArrOfBit.h>
#include <Hexaedre.h>
#include <Hexa_VEF.h>
#include <Triangle.h>
#include <Tri_VEF.h>
#include <Domaine.h>
#include <Debog.h>

Implemente_instanciable(Domaine_VEF, "Domaine_VEF", Domaine_VF);

Sortie& Domaine_VEF::ecrit(Sortie& os) const
{
  Domaine_VF::printOn(os);
  os << "____ h_carre " << finl;
  os << h_carre << finl;
  os << "____ type_elem_ " << finl;
  os << type_elem_ << finl;
  os << "____ nb_elem_std_ " << finl;
  os << nb_elem_std_ << finl;
  os << "____ volumes_entrelaces_ " << finl;
  volumes_entrelaces_.ecrit(os);
  os << "____ face_normales_ " << finl;
  face_normales_.ecrit(os);
  os << "____ facette_normales_ " << finl;
  facette_normales_.ecrit(os);
  os << "____ nb_faces_std_ " << finl;
  os << nb_faces_std_ << finl;
  os << "____ rang_elem_non_std_ " << finl;
  rang_elem_non_std_.ecrit(os);
  return os;
}

Sortie& Domaine_VEF::printOn(Sortie& os) const
{
  Domaine_VF::printOn(os);

  os << h_carre << finl;
  os << type_elem_.valeur() << finl;
  os << nb_elem_std_ << finl;
  os << volumes_entrelaces_ << finl;
  os << face_normales_ << finl;
  os << facette_normales_ << finl;
  os << xp_ << finl;
  os << xv_ << finl;
  os << nb_faces_std_ << finl;
  os << rang_elem_non_std_ << finl;
  return os;
}

Entree& Domaine_VEF::readOn(Entree& is)
{
  Domaine_VF::readOn(is);
  is >> h_carre;

  /* read type_elem */
  {
    Nom type;
    is >> type;
    if (type == "Tri_VEF")
      type_elem_ = Tri_VEF();
    else if (type == "Tetra_VEF")
      type_elem_ = Tetra_VEF();
    else if (type == "Quadri_VEF")
      type_elem_ = Quadri_VEF();
    else if (type == "Hexa_VEF")
      type_elem_ = Hexa_VEF();
    else
      {
        Cerr << type << " n'est pas un Elem_VEF" << finl;
        Process::exit();
      }
  }

  is >> nb_elem_std_;
  is >> volumes_entrelaces_;
  is >> face_normales_;
  is >> facette_normales_;
  is >> xp_;
  is >> xv_;
  is >> nb_faces_std_;
  is >> rang_elem_non_std_;
  return is;
}

void exemple_champ_non_homogene(const Domaine_VEF& domaine_VEF, DoubleTab& tab)
{
  const DoubleTab& xp = domaine_VEF.xp();
  const Domaine& domaine=domaine_VEF.domaine();
  const DoubleTab& coord=domaine.coord_sommets();
  const DoubleTab& xa=domaine_VEF.xa();
  const ArrOfInt& renum_arete_perio=domaine_VEF.get_renum_arete_perio();
  // Verification du tableau xa des coordonnees arete
  if (xa.size_array()) Debog::verifier("xa=",xa);
  int nb_elem=domaine.nb_elem();
  int nb_elem_tot=domaine.nb_elem_tot();
  int nb_som=domaine.nb_som();
  int nb_som_tot=domaine.nb_som_tot();
  int nb_aretes=domaine.nb_aretes();
  for (int I=0 ; I<nb_elem; I++)
    {
      tab(I)=(1.1+xp(I,0))*(1.1+2*xp(I,1));
      if (Objet_U::dimension==3) tab(I)*=(1.1+3*xp(I,2));
    }
  for (int I=0; I<nb_som; I++)
    {
      tab(nb_elem_tot+I)=(1.1+coord(I,0))*(1.1+2*coord(I,1));
      if (Objet_U::dimension==3) tab(nb_elem_tot+I)*=(1.1+3*coord(I,2));
      // On applique la periodicite:
      tab(nb_elem_tot+I)=tab(nb_elem_tot+domaine.get_renum_som_perio(I));
    }

#ifndef NDEBUG
  const IntVect& ok_arete = domaine_VEF.get_ok_arete();
#endif
  for (int I=0; I<nb_aretes; I++)
    {
      tab(nb_elem_tot+nb_som_tot+I)=(1.1+xa(I,0))*(1.1+2*xa(I,1))*(1.1+3*xa(I,2));
      // On applique la periodicite:
      tab(nb_elem_tot+nb_som_tot+I)=tab[nb_elem_tot+nb_som_tot+renum_arete_perio[I]];
      // On verifie ok_arete au passage
      assert(ok_arete(I)==ok_arete(renum_arete_perio[I]));
    }
  tab.echange_espace_virtuel();
}

void Domaine_VEF::swap(int fac1, int fac2, int nb_som_faces)
{

}

/*! @brief Methode appelee par Domaine_VF::discretiser apres la creation des faces reelles.
 *
 *   On reordonne les faces de sorte a placer les faces "non standard"
 *   au debut de la liste des faces. Les faces non standard sont celles
 *   dont les volumes de controles sont modifies par les conditions aux
 *   limites.
 *
 */
void Domaine_VEF::reordonner(Faces& les_faces)
{
  Cerr << "Domaine_VEF::reordonner les_faces " << finl;

  // Construction de rang_elem_non_std_ :
  //  C'est un vecteur indexe par les elements du domaine.
  //  size() = nb_elem()
  //  size_tot() = nb_elem_tot()
  //  Valeurs dans le tableau :
  //   rang_elem_non_std_[i] = -1 si l'element i est standard,
  //  sinon
  //   rang_elem_non_std_[i] = j, ou j est l'indice de l'element dans
  //   les tableaux indexes par les elements non standards (par exemple
  //   le tableau Domaine_Cl_VEF::type_elem_Cl_).
  // Un element est non standard s'il est voisin d'une face frontiere.
  {
    const int nb_faces_front = domaine().nb_faces_frontiere();
    domaine().creer_tableau_elements(rang_elem_non_std_, RESIZE_OPTIONS::NOCOPY_NOINIT);
    rang_elem_non_std_ = -1;
    // D'abord on marque les elements non standards avec rang_elem_non_std_[i] = 0
    for (int i_face = 0; i_face < nb_faces_front; i_face++)
      {
        const int elem = les_faces.voisin(i_face, 0);
        rang_elem_non_std_[elem] = 0;
      }
    rang_elem_non_std_.echange_espace_virtuel();

    // On construit le md_vector des elements non standards, mais il n'est pas utilise.
    // L'important, c'est que rang_elem_non_std_ soit rempli correctement pour etre
    //  utilise par creer_md_vect_renum() dans Domaine_Cl_VEF::associer()
    MD_Vector md_vect_elems_non_std;
    MD_Vector_tools::creer_md_vect_renum_auto(rang_elem_non_std_, md_vect_elems_non_std);
  }

  renumeroter(les_faces);
}

void Domaine_VEF::discretiser()
{
  Domaine& domaine_geom = domaine();
  typer_elem(domaine_geom);
  Elem_geom_base& elem_geom = domaine_geom.type_elem().valeur();
  Domaine_VF::discretiser();

  // Correction du tableau face_voisins:
  //  A l'issue de Domaine_VF::discretiser(), les elements voisins 0 et 1 d'une
  //  face sont les memes sur tous les processeurs qui possedent la face.
  //  Si la face est virtuelle et qu'un des deux elements voisins n'est
  //  pas connu (il n'est pas dans l'epaisseur du joint), l'element voisin
  //  vaut -1. Cela peut etre un voisin 0 ou un voisin 1.
  //  On corrige les faces virtuelles pour que, si un element voisin n'est
  //  pas connu, alors il est voisin1. Le voisin0 est donc toujours valide.
  {
    IntTab& face_vois = face_voisins();
    const int debut = nb_faces();
    const int fin = nb_faces_tot();
    for (int i = debut; i < fin; i++)
      {
        if (face_voisins(i, 0) == -1)
          {
            face_vois(i, 0) = face_vois(i, 1);
            face_vois(i, 1) = -1;
          }
      }
  }

  // Verification de la coherence entre l'element geometrique et
  //l'elemnt de discretisation

  if (sub_type(Tri_VEF, type_elem_.valeur()))
    {
      if (!sub_type(Triangle, elem_geom))
        {
          Cerr << " Le type de l'element geometrique " << elem_geom.que_suis_je() << " est incorrect" << finl;
          Cerr << " Seul le type Triangle est compatible avec la discretisation VEF en dimension 2" << finl;
          Cerr << " Il faut trianguler le domaine lorsqu'on utilise le mailleur interne";
          Cerr << " en utilisant l'instruction: Trianguler nom_dom" << finl;
          exit();
        }
    }
  else if (sub_type(Tetra_VEF, type_elem_.valeur()))
    {
      if (!sub_type(Tetraedre, elem_geom))
        {
          Cerr << " Le type de l'element geometrique " << elem_geom.que_suis_je() << " est incorrect" << finl;
          Cerr << " Seul le type Tetraedre est compatible avec la discretisation VEF en dimension 3" << finl;
          Cerr << " Il faut tetraedriser le domaine lorsqu'on utilise le mailleur interne";
          Cerr << " en utilisant l'instruction: Tetraedriser nom_dom" << finl;
          exit();
        }
    }

  else if (sub_type(Quadri_VEF, type_elem_.valeur()))
    {

      if (!sub_type(Quadrangle_VEF, elem_geom))
        {
          Cerr << " Le type de l'element geometrique " << elem_geom.que_suis_je() << " est incorrect" << finl;
          exit();
        }
    }
  else if (sub_type(Hexa_VEF, type_elem_.valeur()))
    {

      if (!sub_type(Hexaedre_VEF, elem_geom))
        {
          Cerr << " Le type de l'element geometrique " << elem_geom.que_suis_je() << " est incorrect" << finl;
          exit();
        }
    }

  int num_face;

  // On remplit le tableau face_normales_;
  //  Attention : le tableau face_voisins n'est pas exactement un
  //  tableau distribue. Une face n'a pas ses deux voisins dans le
  //  meme ordre sur tous les processeurs qui possedent la face.
  //  Donc la normale a la face peut changer de direction d'un
  //  processeur a l'autre, y compris pour les faces de joint.
  {
    const int n = nb_faces();
    face_normales_.resize(n, dimension);
    creer_tableau_faces(face_normales_, RESIZE_OPTIONS::NOCOPY_NOINIT);
    const IntTab& face_som = face_sommets();
    const IntTab& face_vois = face_voisins();
    const IntTab& elem_face = elem_faces();
    const int n_tot = nb_faces_tot();
    for (num_face = 0; num_face < n_tot; num_face++)
      {
        type_elem_->normale(num_face, face_normales_, face_som, face_vois, elem_face, domaine_geom);
      }
  }

  // Calcul de facette_normales_
  type_elem_->creer_facette_normales(domaine_geom, facette_normales(), rang_elem_non_std());

  calculer_volumes_entrelaces();
  Cerr << "Informations of the Domaine VEF of the domain " << domaine().le_nom() << " : " << finl;

  calculer_h_carre();

  domaine().creer_tableau_sommets(volumes_som, RESIZE_OPTIONS::NOCOPY_NOINIT);

  double coeff=1./3.;
  if (dimension==3)
    coeff=1./4.;

  const IntTab& elements = domaine().les_elems();
  const int nb_som_elem = elements.dimension(1);
  // Boucle sur tous les elements car on ajoute des contributions aux sommets de joints:
  const int n = nb_elem_tot();
  // On a besoin de l'espace virtuel des volumes
  const DoubleVect& volume_elem = volumes();
  assert_espace_virtuel_vect(volume_elem);

  // Annule tout le tableau car on va faire += sur des items virtuels
  // (sinon acces a des cases non initialisees)
  operator_egal(volumes_som, 0., VECT_ALL_ITEMS);
  for(int k=0; k<n; k++)
    {
      double volume = coeff * volume_elem(k);
      for(int isom=0; isom<nb_som_elem; isom++)
        {
          int som = elements(k, isom);
          volumes_som(som)+=volume;
        }
    }
  volumes_som.echange_espace_virtuel();
}

void Domaine_VEF::discretiser_suite(const VEF_discretisation& discr)
{
  // Recuperation des parametres de la discretisation
  alphaE = discr.get_alphaE();
  alphaS = discr.get_alphaS();
  alphaA = discr.get_alphaA();
  alphaRT = discr.get_alphaRT();
  P1Bulle = discr.get_P1Bulle();
  modif_div_face_dirichlet= discr.get_modif_div_face_dirichlet();
  cl_pression_sommet_faible = discr.get_cl_pression_sommet_faible();


  if (alphaA)
    discretiser_arete();

  // Construction du descripteur pour les tableaux p1bulle
  {
    MD_Vector_composite md_p1b;
    if (alphaE)
      {
        const MD_Vector& md = domaine().md_vector_elements();
        md_p1b.add_part(md, 0, "P0");
      }
    if (alphaS)
      {
        const MD_Vector& md = domaine().md_vector_sommets();
        md_p1b.add_part(md, 0, "P1");
      }
    if (alphaA)
      {
        const MD_Vector& md = md_vector_aretes();
        md_p1b.add_part(md, 0, "Pa");
      }
    md_vector_p1b_.copy(md_p1b);
  }
  Cerr << "le Domaine_VEF a ete rempli avec succes" << finl;
}

/*
  static int arete(const IntTab& som_aretes,
  const IntTab& aretes_som,
  int S1, int S2, const Domaine& dom)
  {
  //Cout << "S1,S2 = " << S1 << " " << S2 << finl;
  for(int i=0; i<nb_max_aretes_som; i++)
  {
  const int& a=som_aretes(S1,i);
  if( (dom.get_renum_som_perio(aretes_som(a,0))==S2)
  ||
  (dom.get_renum_som_perio(aretes_som(a,1))==S2) )
  return a;
  }
  {
  Cerr << "arete pas Trouvee!!" << finl;
  Cout << "S1,S2 = " << S1 << " " << S2 << finl;
  Cout << "som_aretes " << som_aretes << finl;
  Cout << "aretes_som " << aretes_som << finl;
  Process::exit();
  }
  return -1;
  } */

static int next(int S,
                const ArrOfInt& contenu)
{
  int nb_som=contenu.size_array();
  int i=S+1;
  while(i<nb_som)
    if(contenu[i]==1)
      return i;
    else i++;
  i=0;
  while(i<S)
    if(contenu[i]==1)
      return i;
    else i++;
  return -1;
}

void Domaine_VEF::discretiser_arete()
{
  const Domaine& dom = domaine();

  // Creation des aretes reelles (informations geometriques construites et stockees dans le domaine)
  domaine().creer_aretes();
  md_vector_aretes_ = domaine().aretes_som().get_md_vector();

  // Calcul des centres de gravite des aretes xa_ stockes dans le Domaine_VF
  const IntTab& aretes_som = domaine().aretes_som();
  const int nb_aretes = aretes_som.dimension(0);
  const DoubleTab& coord = dom.les_sommets();
  const int dim = coord.dimension(1);
  xa_.resize(0, dim);
  creer_tableau_aretes(xa_, RESIZE_OPTIONS::NOCOPY_NOINIT);
  for (int i = 0; i < nb_aretes; i++)
    {
      const int s0 = aretes_som(i, 0);
      const int s1 = aretes_som(i, 1);
      for (int j = 0; j < dim; j++)
        xa_(i, j) = (coord(s0, j) + coord(s1, j)) * 0.5;
    }
  xa_.echange_espace_virtuel();

  const IntTab& elem_aretes = domaine().elem_aretes();

  // Calcul du volume des aretes
  // Creation d'un tableau initialise a zero:
  creer_tableau_aretes(volumes_aretes);

  const int nbr_elem = domaine().nb_elem();
  const int nb_aretes_elem = elem_aretes.dimension(1);
  // facteur 6 pour le calcul des volumes des aretes, est-ce correct pour autre chose qu'un tetra ?
  assert(nb_aretes_elem == 6);
  // Essai d'une autre facon de coder: calcul des contributions aux aretes par les elements reels,
  // puis sommation des contributions des aretes partagees:
  for (int elem = 0; elem < nbr_elem; elem++)
    {
      double vol = volumes(elem) / 6.0;
      for(int j = 0; j < nb_aretes_elem; j++)
        {
          int arete = elem_aretes(elem, j);
          volumes_aretes[arete] += vol;
        }
    }
  // Sommation des contributions des aretes joint et echange des espaces virtuels
  MD_Vector_tools::echange_espace_virtuel(volumes_aretes, MD_Vector_tools::EV_SOMME_ECHANGE);
}

void Domaine_VEF::construire_ok_arete()
{
  Cerr << "Build array ok_arete..." << finl;
  const Domaine& dom=domaine();
  const IntTab& aretes_som=domaine().aretes_som();
  const int nb_som_reel=nb_som();

  // Connectivite sommets-aretes (pour chaque sommet, liste des aretes adjacentes)
  // B.M.: je remplace IntTab(n, 64) par une Static_Int_List et je reutilise
  //  la methode de calcul des connectivites... plus econome en memoire !
  // som_aretes contient une connectivite modifiee pour les sommets periodiques
  // (les aretes sont toujours rattachees au sommet get_renum_som_perio(),
  //  les aretes periodiques opposees sont remplacees par une arete [nb_som_reel,nb_som_reel]
  //  fictive, et les sommets perio opposes ne sont rattaches a aucune arete)
  const int nb_aretes = aretes_som.dimension(0);
  Static_Int_Lists som_aretes;
  {
    // Creation d'un tableau d'aretes ou les sommets sont remplaces par le sommet
    //  periodique associe et sans les aretes periodiques opposees
    IntTab aretes_som2;
    aretes_som2.copy(aretes_som, RESIZE_OPTIONS::NOCOPY_NOINIT);

    for (int i = 0; i < nb_aretes; i++)
      {
        if (renum_arete_perio[i] == i)
          {
            aretes_som2(i, 0) = dom.get_renum_som_perio(aretes_som(i,0));
            aretes_som2(i, 1) = dom.get_renum_som_perio(aretes_som(i,1));
          }
        else
          {
            // arete periodique opposee, non conservee, on cree une arete fictive pour
            // construire_connectivite_som_elem: le sommet d'indice nb_som_reel
            // sera connecte a toutes les aretes periodiques supprimees
            aretes_som2(i, 0) = nb_som_reel;
            aretes_som2(i, 1) = nb_som_reel;
          }
      }
    // On donne un sommet de plus (le sommet fictif connecte aux aretes supprimees)
    construire_connectivite_som_elem(nb_som_reel+1, aretes_som2, som_aretes, 0 /* do not include virtual items */);
  }

  // Creation et initialisation du tableau contenu, initialise a zero par defaut
  ArrOfInt contenu(nb_som_reel);

  // Initialisation pour le parallele: contenu est mis a 2
  // pour les sommets communs recus d'un autre processeur
  {
    ArrOfBit flags;
    MD_Vector_tools::get_sequential_items_flags(dom.les_sommets().get_md_vector(), flags);
    for (int i = 0; i < nb_som_reel; i++)
      {
        if (!flags[i])
          {
            // Ce sommet est recu d'un autre processeur
            const int i2 = dom.get_renum_som_perio(i);
            contenu[i] = 2;
            if (i2 != i)
              contenu[i2] = 2;
          }
      }
  }

  // Estimation du nombre d'aretes superflues a trouver sur le domaine
  // en comptant les sommets reels non periodiques dont le contenu vaut 0
  int nombre_aretes_superflues_prevues_sur_le_dom=0;
  for (int i=0; i<nb_som_reel; i++)
    if (i==dom.get_renum_som_perio(i) && contenu[i]==0)
      nombre_aretes_superflues_prevues_sur_le_dom++;

  ok_arete = -1;

  // On boucle tant qu'il y'a des sommets avec le contenu 0
  while (min_array(contenu)==0)
    {
      // On cherche le premier sommet avec contenu 0 en bouclant sur les aretes
      int Aroot=-1,Sroot=-1;
      int S0,S1;
      do
        {
          while (ok_arete(++Aroot)==0) {};
          S0=dom.get_renum_som_perio(aretes_som(Aroot,0));
          S1=dom.get_renum_som_perio(aretes_som(Aroot,1));
        }
      while (Aroot<nb_aretes && contenu[S0]!=0 && contenu[S1]!=0);

      assert(Aroot<nb_aretes);

      Aroot=renum_arete_perio[Aroot];

      ok_arete(Aroot)=0; // lockee
      if (!contenu[S0])
        {
          contenu[S0]=1;
          Sroot=S0;
        }
      else if (!contenu[S1])
        {
          contenu[S1]=1;
          Sroot=S1;
        }

      // Boucle sur les sommets
      do
        {
          const int nb_aretes_voisines = som_aretes.get_list_size(Sroot);
          for(int i=0; i<nb_aretes_voisines; i++)
            {
              int A=renum_arete_perio[som_aretes(Sroot,i)];
              if(ok_arete(A)==-1)
                {
                  int S=dom.get_renum_som_perio(aretes_som(A,0));
                  if(S==Sroot)
                    {
                      S=dom.get_renum_som_perio(aretes_som(A,1));
                    }
                  if(!contenu[S])
                    {
                      ok_arete(A)=0; // lockee
                      contenu[S]=1;
                    }
                  else
                    {
                      ok_arete(A)=1; // libre
                    }
                }
            }
          contenu[Sroot]=2;
        }
      while((Sroot=next(Sroot, contenu))!=-1);

      // Correction pour des tableaux ok_arete et contenu pour la periodicite
      for(int i=0; i<nb_aretes; i++)
        ok_arete(i)=ok_arete[renum_arete_perio[i]];
      for(int i=0; i<nb_som_reel; i++)
        contenu[i]=contenu[dom.get_renum_som_perio(i)];
    }

  // Mise a jour des parties virtuelles du tableau ok_arete
  ok_arete.echange_espace_virtuel();

  // Verification des aretes superflues
  verifie_ok_arete(nombre_aretes_superflues_prevues_sur_le_dom);

  // Ecriture des aretes superflues dans un fichier nom_du_cas.ok_arete afin de le relire la fois suivante
  Nom fichier(nom_du_cas());
  fichier+="_";
  fichier+=domaine().le_nom()+".ok_arete";

  Cerr << "Writing file " << fichier << finl;
  EcrFicPartageBin fic_ok_arete_;
  if (!fic_ok_arete_.ouvrir(fichier,ios::out))
    {
      Cerr << "Error: cannot open file " << fichier << " for writing." << finl;
      exit();
    }

  ArrOfBit marqueurs_aretes;
  const MD_Vector& md_aretes = md_vector_aretes();
  MD_Vector_tools::get_sequential_items_flags(md_aretes, marqueurs_aretes);
  const int nb_aretes_seq = md_aretes->nb_items_seq_tot();

  if (Process::je_suis_maitre())
    fic_ok_arete_ << nb_aretes_seq << finl;

  // Chaque processeur ecrit ses aretes non communes:
  const int n = marqueurs_aretes.size_array();
  for (int i = 0; i < n; i++)
    {
      if (marqueurs_aretes[i])
        fic_ok_arete_ << xa_(i,0) << tspace << xa_(i,1) << tspace << xa_(i,2) << tspace << ok_arete(i) << finl;
    }
  fic_ok_arete_.syncfile();
  fic_ok_arete_.close();
}

void Domaine_VEF::construire_renum_arete_perio(const Conds_lim& conds_lim)
{
  Cerr << "Build array renum_arete_perio..." << finl;
  const IntTab& aretes_som=domaine().aretes_som();
  const int nb_aretes_tot=domaine().nb_aretes_tot();
  const Domaine& dom=domaine();

  // Initialisation de renum_arete_perio
  renum_arete_perio.resize_array(nb_aretes_tot);
  for (int i=0; i<nb_aretes_tot; i++)
    renum_arete_perio[i]=i;

  const IntTab& elem_aretes=domaine().elem_aretes();
  ArrOfInt aretes1(6);
  ArrOfInt aretes2(6);
  // Premiere etape: faire pointer tous les aretes periodiques liees entre elles vers la meme arete
  for (auto& itr : conds_lim)
    {
      //for cl

      const Cond_lim_base& cl = itr.valeur();
      if (sub_type(Periodique, cl))
        {
          //if Perio
          const Periodique& la_cl_perio = ref_cast(Periodique,cl);
          const Front_VF& le_bord = ref_cast(Front_VF,cl.frontiere_dis());

          int nf_bord_tot = le_bord.nb_faces_tot();
          IntVect fait(nf_bord_tot);
          fait = 0;
          for(int ind_face=0; ind_face<nf_bord_tot; ind_face++)
            if(!fait(ind_face))
              {
                int face=le_bord.num_face(ind_face);
                int ind_faassociee=la_cl_perio.face_associee(ind_face);
                int face_assciee=le_bord.num_face(ind_faassociee);
                fait(ind_faassociee)=fait(ind_face)=1;

                int elem1 = face_voisins_(face,0);
                int elem2 = face_voisins_(face,1);

                for(int j=0; j<6; j++)
                  {
                    aretes1[j]=elem_aretes(elem1,j);
                    aretes2[j]=elem_aretes(elem2,j);
                  }

                for(int j1=0; j1<6; j1++)
                  {
                    int ar1=aretes1[j1];
                    int& ar1_perio = renum_arete_perio[ar1];
                    // On verifie que l'arete appartient a la face (ok==2)
                    int som11=aretes_som(ar1, 0);
                    int som12=aretes_som(ar1, 1);
                    int ok=0;
                    int nbf = domaine().type_elem()->nb_som_face();
                    const IntTab& sommet = face_sommets();
                    for (int k=0; k<nbf; k++)
                      if (sommet(face,k)==som11 || sommet(face,k)==som12) ok++;
                    assert(ok>0);
                    if (ok==2)
                      {
                        int s11=dom.get_renum_som_perio(som11);
                        int s12=dom.get_renum_som_perio(som12);
                        for(int j2=0; j2<6; j2++)
                          {
                            int ar2=aretes2[j2];
                            int& ar2_perio = renum_arete_perio[ar2];
                            int som21=aretes_som(ar2, 0);
                            int som22=aretes_som(ar2, 1);
                            ok=0;
                            // On verifie que l'arete appartient a la face_assciee (ok==2)
                            for (int k=0; k<nbf; k++)
                              if (sommet(face_assciee,k)==som21 || sommet(face_assciee,k)==som22) ok++;
                            assert(ok>0);
                            if (ok==2)
                              {
                                int s21=dom.get_renum_som_perio(som21);
                                int s22=dom.get_renum_som_perio(som22);
                                assert(ar1!=ar2);
                                //Les aretes sont donc periodiques si on rentre dans le "if"
                                if ( ( (s21==s11)||(s22==s11) ) && ( (s22==s12)||(s21==s12) ) )
                                  {
                                    // Critere I : choix de l'arete perio en fonction d'un critere geometrique
                                    int dir_perio = la_cl_perio.direction_periodicite();
                                    int arete_perio = (xa(ar1_perio,dir_perio)<=xa(ar2_perio,dir_perio)) ? ar1_perio : ar2_perio;
                                    /*
                                    // Critere II : choix de l'arete perio en fonction des sommets periodiques
                                    int arete_perio;
                                    if (som11!=s11 && som12!=s12) // Les sommets de l'arete 1 sont periodiques
                                    arete_perio = ar2_perio;
                                    else if (som21!=s21 && som22!=s22) // Les sommets de l'arete 2 sont periodiques
                                    arete_perio = ar1_perio;
                                    else
                                    {
                                    Cerr << "Cas non prevu." << finl;
                                    exit();
                                    } */
                                    ar2_perio = arete_perio;
                                    ar1_perio = arete_perio;
                                    volumes_aretes(ar2)+=volumes_aretes(ar1);
                                    volumes_aretes(ar1)+=volumes_aretes(ar2);
                                  }//fin du if sur "s21==s11"
                              }
                          }//fin du for sur "j2"
                      }
                  }//fin du if sur "renum_arete_perio(aretes1)" et du for sur "j1"
              }//fin du if sur "fait"
        }//fin if Perio
    }// fin for cl

  // Deuxieme etape: faire pointer tous les aretes periodiques liees entre elles vers la meme arete
  for (int i = 0; i < nb_aretes_tot; i++)
    {
      int j = renum_arete_perio[i];
      // Parcourir les aretes reliees pour cette chaine:
      while (j != renum_arete_perio[j])
        j = renum_arete_perio[j];
      renum_arete_perio[i] = j;
    }

  if (Debog::active())
    {
      IntVect tmp;
      creer_tableau_aretes(tmp, RESIZE_OPTIONS::NOCOPY_NOINIT);
      const int n = tmp.size_array();
      for (int i=0; i<n; i++)
        tmp[i] = renum_arete_perio[i];
      Debog::verifier_indices_items("renum_arete_perio",tmp.get_md_vector(),tmp);
    }
}

void Domaine_VEF::verifie_ok_arete(int nombre_aretes_superflues_prevues_sur_le_dom) const
{
  Cerr << "Check array ok_arete..." << finl;
  // Algorithme de verification du tableau des aretes superflues
  // ok_arete(i)==0 : i arete superflue
  // ok_arete(i)==2 : i arete necessaire
  // ...
  // contenu doit contenir uniquement 2 ce qui implique que tous les
  // sommets ont ete analyses.
  const Domaine& dom=domaine();
  const int nb_som_reel=nb_som();
  const IntTab& aretes_som=domaine().aretes_som();
  int nb_aretes_pour_verbose=60; // Pour verbose
  ArrOfInt sommet_relie_arete_superflue(nb_som_reel);
  sommet_relie_arete_superflue=0;
  for (int i=0; i<nb_som_reel; i++) // Si i est un sommet periodique:
    if (dom.get_renum_som_perio(i)!=i) // il suffit de faire la verification sur le sommet dom.get_renum_som_perio(i)
      sommet_relie_arete_superflue[i]=1; // donc on ne verifie pas i

  double nombre_aretes_reelles_superflues=0;
  // On parcourt toutes les aretes mais on ne regarde que les sommets reels
  int nb_aretes_tot=domaine().nb_aretes_tot();
  int nb_aretes_reelles=domaine().nb_aretes();
  for (int i=0; i<nb_aretes_tot; i++)
    {
      if (!ok_arete(i)) // arete superflue
        {
          int S0=aretes_som(i,0);
          if (S0<nb_som_reel)
            {
              sommet_relie_arete_superflue[S0]=1;
              sommet_relie_arete_superflue[dom.get_renum_som_perio(S0)]=1;
            }
          int S1=aretes_som(i,1);
          if (S1<nb_som_reel)
            {
              sommet_relie_arete_superflue[S1]=1;
              sommet_relie_arete_superflue[dom.get_renum_som_perio(S1)]=1;
            }
          if (renum_arete_perio[i]==i) // Pour ne compter les aretes periodiques qu'une fois
            {
              if (nb_aretes_tot<nb_aretes_pour_verbose)
                {
                  if (renum_arete_perio[i]==i) Cerr << "[" << Process::me() << "] Arete " << i << " superflue non perio: " << S0 << " " << S1 << finl;
                  else Cerr << "[" << Process::me() << "] Arete " << i << " superflue perio: " << S0 << " " << S1 << " Periodique avec " << renum_arete_perio[i] << finl;
                }
              int aretes_superflues_communes=1;
              // Si l'arete superflue est commune on en tient compte
              for (int j=0; j<dom.faces_joint().size(); j++)
                {
                  int nb_aretes_sur_le_joint = dom.faces_joint()(j).joint_item(JOINT_ITEM::ARETE).items_communs().size_array();
                  for (int k=0; k<nb_aretes_sur_le_joint; k++)
                    if (dom.faces_joint()(j).joint_item(JOINT_ITEM::ARETE).items_communs()[k]==i) aretes_superflues_communes++;
                }
              // On compte les aretes reelles superflues
              if (i<nb_aretes_reelles)
                nombre_aretes_reelles_superflues+=1./aretes_superflues_communes;
            }
        }
    }

  if (nb_som_reel > 0 && min_array(sommet_relie_arete_superflue)==0)
    {
      Cerr << finl << "[" << Process::me() << "] Il y'a au moins un sommet qui n'est pas relie a une arete superflue :" << finl;
      for (int i=0; i<sommet_relie_arete_superflue.size_array(); i++)
        if (sommet_relie_arete_superflue[i]==0) Cerr << "Sommet " << i << finl;
      Process::exit();
    }
  Cerr << "[" << Process::me() << "] Verification que chaque sommet non periodique est bien relie a au moins une arete superflue: OK!" << finl;
  // On compte le nombre total de sommets en tenant compte de la periodicite et des sommets communs
  double nb_sommets_non_periodiques=0;
  for (int i=0; i<nb_som_reel; i++)
    if (dom.get_renum_som_perio(i)==i) // Sommet non periodique
      {
        int sommets_communs=1;
        // On tient compte des sommets communs
        for (int j=0; j<dom.faces_joint().size(); j++)
          for (int k=0; k<dom.faces_joint()(j).joint_item(JOINT_ITEM::SOMMET).items_communs().size_array(); k++)
            if (dom.faces_joint()(j).joint_item(JOINT_ITEM::SOMMET).items_communs()[k]==i) sommets_communs++;
        nb_sommets_non_periodiques+=1./sommets_communs;
      }
  double total_nombre_aretes_superflues=mp_sum(nombre_aretes_reelles_superflues);
  double somme_nombre_aretes_superflues_prevues_par_domaine=mp_sum(nombre_aretes_superflues_prevues_sur_le_dom);
  double total_nb_sommets_non_periodiques=mp_sum(nb_sommets_non_periodiques);


  // Cerr << "Nombre de sommets non periodiques           = " << total_nb_sommets_non_periodiques << finl;
  // Cerr << "Nombre de sommets periodiques               =  " << nb_som_reel-total_nb_sommets_non_periodiques << finl;
  //int nb_aretes_periodiques=0;
  //int nb_aretes_perio_superflues=0;
  for (int i=0; i<nb_aretes_tot; i++)
    {
      if (renum_arete_perio[i]!=i)
        {
          //nb_aretes_periodiques++;
          assert(ok_arete(i)==ok_arete(renum_arete_perio[i]));
          //if (!ok_arete(i)) nb_aretes_perio_superflues++;
        }
    }
  // Cerr << "Nombre d'aretes non periodiques             = " << nb_aretes_tot-nb_aretes_periodiques << finl;
  // Cerr << "Nombre d'aretes periodiques                 =  " << nb_aretes_periodiques << finl;

  // Cerr << "Nombre d'aretes superflues                  =  " << total_nombre_aretes_superflues << finl;
  // Cerr << "Nombre d'aretes superflues periodiques      =  " << nb_aretes_perio_superflues << finl;
  // Cerr << "Nombre d'aretes non superflues periodiques  =  " << nb_aretes_periodiques-nb_aretes_perio_superflues << finl;

  if (Process::is_sequential()) // On se limite au sequentiel car il y'a un soucis pour le calcul de total_nombre_aretes_superflues (les items communs pour les aretes n'est pas construit!)
    if (!est_egal(somme_nombre_aretes_superflues_prevues_par_domaine,total_nombre_aretes_superflues) && je_suis_maitre())
      {
        Cerr << "La somme des aretes superflues prevues par domaine n'est pas egale au nombre d'aretes superflues trouvees sur le domaine." << finl;
        Cerr << somme_nombre_aretes_superflues_prevues_par_domaine << " != " << total_nombre_aretes_superflues << finl;
        Process::exit();
      }
  Cerr << "Nombre total d'aretes superflues: " << somme_nombre_aretes_superflues_prevues_par_domaine << finl;
  Cerr << "Nombre total de sommets non periodiques = " << total_nb_sommets_non_periodiques << finl;
  Cerr << "Verification de l'egalite du nombre total d'aretes superflues et du nombre total de sommets: ";
  // Verification que le nombre d'aretes superflues et egal au nombre de sommets
  if (!est_egal(somme_nombre_aretes_superflues_prevues_par_domaine,total_nb_sommets_non_periodiques) && je_suis_maitre())
    {
      Cerr << "Non correspondance. Echec de l'algorithme de recherche des aretes superflues." << finl;
      Process::exit();
    }
  Cerr << "OK!" << finl << "Verification correcte des aretes superflues." << finl;
}

// Valeur de retour:
//  1: ok
//  0: fichier inexistant
//  -1: fichier existe mais pas le bon nombre d'aretes
int Domaine_VEF::lecture_ok_arete()
{
  Nom fichier(nom_du_cas());
  fichier+="_";
  fichier+=domaine().le_nom()+".ok_arete";

  Cerr << "Trying to read file " << fichier << " (edges to remove from the set of degrees of freedom)" << finl;
  EFichierBin fic_ok_arete_;
  // Lecture de ok_arete dans un fichier pour comparaison sequentiel-parallele
  if (!fic_ok_arete_.ouvrir(fichier,ios::out))
    {
      Cerr << "File " << fichier << " does not exist." << finl;
      return 0;
    }

  int n;
  fic_ok_arete_ >> n;
  if (n != md_vector_aretes()->nb_items_seq_tot())
    {
      Cerr << "File " << fichier << " is not compatible with the current mesh." << finl;
      return 0;
    }

  Octree_Double octree;
  octree.build_nodes(xa_, 1 /* include virtual nodes */);
  const double eps = precision_geom;
  ArrOfInt liste_aretes;


  IntVect marqueurs;
  creer_tableau_aretes(marqueurs); // initialise a zero par defaut

  for (int i = 0; i < n; i++)
    {
      double x, y, z;
      int flag;
      fic_ok_arete_ >> x >> y >> z >> flag;
      octree.search_elements_box(x-eps, y-eps, z-eps, x+eps, y+eps, z+eps, liste_aretes);
      octree.search_nodes_close_to(x, y, z, xa_, liste_aretes, eps);
      const int m = liste_aretes.size_array();
      if (m > 1)
        {
          Cerr << "File " << fichier << " is not compatible with the current mesh." << finl;
          return 0;
        }
      if (m == 1)
        {
          const int arete = liste_aretes[0];
          ok_arete[arete] = flag;
          marqueurs[arete]++;
        }
      else
        {
          // Normal en parallele: on n'a pas toutes les aretes sur tous les procs...
        }
    }

  // On doit avoir trouve exactement une fois toutes les aretes:
  if (max_array(marqueurs) > 1 || min_array(marqueurs) < 1)
    {
      Cerr << "File " << fichier << " is not compatible with the current mesh." << finl;
      return 0;
    }
  else
    Cerr << "File " << fichier << " is OK." << finl;
  fic_ok_arete_.close();
  assert_espace_virtuel_vect(ok_arete);

  return 1;
}

void Domaine_VEF::creer_tableau_p1bulle(Array_base& x, RESIZE_OPTIONS opt) const
{
  const MD_Vector& md = md_vector_p1b();
  MD_Vector_tools::creer_tableau_distribue(md, x, opt);
}


void Domaine_VEF::calculer_h_carre()
{
  const int nbe = nb_elem();
  // Calcul de h_carre
  h_carre = 1.e30;
  h_carre_.resize(nbe);
  // Calcul des surfaces
  const DoubleVect& surfaces = face_surfaces();
  const int nb_faces_elem = domaine().nb_faces_elem();
  for (int num_elem = 0; num_elem < nbe; num_elem++)
    {
      double surf_max = 0;
      for (int i = 0; i < nb_faces_elem; i++)
        {
          double surf = surfaces(elem_faces(num_elem, i));
          surf_max = (surf > surf_max) ? surf : surf_max;
        }
      double vol = volumes(num_elem) / surf_max;
      vol *= vol;
      h_carre_(num_elem) = vol;
      h_carre = (vol < h_carre) ? vol : h_carre;
    }
  // Modif BM: je pense qu'avec mp_min(h_carre) on va eviter beaucoup de differences seq/par
  h_carre = mp_min(h_carre);
  Cerr << "Lowest cell size h=(Volume/max(Surface))= " << sqrt(h_carre) << finl;
  double moyenne = 0.;
  for (int i = 0; i < nbe; i++)
    moyenne += h_carre_(i);
  moyenne = mp_sum(moyenne);
  double h_carre_moyen = moyenne / mp_sum((double) nbe);
  Cerr << "Average cell size <h>= " << sqrt(h_carre_moyen) << finl;
  if (h_carre * 1e6 < h_carre_moyen)
    Cerr << "Warning, a cell size is more than 1000 times smaller than the average cell size of the domain. Check your mesh." << finl;
  Cerr << "==============================================" << finl;
}

void Domaine_VEF::calculer_volumes_entrelaces()
{
  //  Cerr << "les normales aux faces " << face_normales() << finl;
  // On calcule les volumes entrelaces;

  // Si domaine dynamique, le tableau peut eventuellement deja avoir la bonne structure, ou pas.
  // S'il n'est pas deformable, on n'est pas cense passer ici deux fois.
  assert(domaine().deformable() || !(volumes_entrelaces_.get_md_vector().non_nul()));
  if (!(volumes_entrelaces_.get_md_vector() == md_vector_faces()))
    {
      volumes_entrelaces_.reset();
      creer_tableau_faces(volumes_entrelaces_, RESIZE_OPTIONS::NOCOPY_NOINIT);
    }
  int nb_faces_elem = domaine().nb_faces_elem();
  int num_face;
  int nmax = premiere_face_int();
  const double facteur = 1. / nb_faces_elem;
  for (num_face = 0; num_face < nmax; num_face++)
    {
      int elem1 = face_voisins_(num_face, 0);
      volumes_entrelaces_[num_face] = volumes(elem1) * facteur;
    }
  nmax = nb_faces();
  for (; num_face < nmax; num_face++)
    {
      int elem1 = face_voisins_(num_face, 0);
      int elem2 = face_voisins_(num_face, 1);
      volumes_entrelaces_[num_face] = (volumes(elem1) + volumes(elem2)) * facteur;
    }
  volumes_entrelaces_.echange_espace_virtuel();
}

void Domaine_VEF::remplir_elem_faces()
{
  creer_faces_virtuelles_non_std();
}

void Domaine_VEF::modifier_pour_Cl(const Conds_lim& conds_lim)
{
  Journal() << "Domaine_VEF::Modifier_pour_Cl" << finl;
  for (auto &itr : conds_lim)
    {
      //for cl
      const Cond_lim_base& cl = itr.valeur();
      if (sub_type(Periodique, cl))
        {
          //if Perio
          const Periodique& la_cl_period = ref_cast(Periodique, cl);
          int nb_faces_elem = domaine().nb_faces_elem();
          const Front_VF& la_front_dis = ref_cast(Front_VF, cl.frontiere_dis());
          int ndeb = 0;
          int nfin = la_front_dis.nb_faces_tot();
#ifndef NDEBUG
          int num_premiere_face = la_front_dis.num_premiere_face();
          int num_derniere_face = num_premiere_face + nfin;
#endif
          int nbr_faces_bord = la_front_dis.nb_faces();
          assert((nb_faces() == 0) || (ndeb < nb_faces()));
          assert(nfin >= ndeb);
          int elem1, elem2, k;
          int face;
          // Modification des tableaux face_voisins_ , face_normales_ , volumes_entrelaces_
          // On change l'orientation de certaines normales
          // de sorte que les normales aux faces de periodicite soient orientees
          // de face_voisins(la_face_en_question,0) vers face_voisins(la_face_en_question,1)
          // comme le sont les faces internes d'ailleurs

          DoubleVect C1C2(dimension);
          double vol, psc = 0;

          for (int ind_face = ndeb; ind_face < nfin; ind_face++)
            {
              //for ind_face
              face = la_front_dis.num_face(ind_face);
              if ((face_voisins_(face, 0) == -1) || (face_voisins_(face, 1) == -1))
                {
                  int faassociee = la_front_dis.num_face(la_cl_period.face_associee(ind_face));
                  if (ind_face < nbr_faces_bord)
                    {
                      assert(faassociee >= num_premiere_face);
                      assert(faassociee < num_derniere_face);
                    }

                  elem1 = face_voisins_(face, 0);
                  elem2 = face_voisins_(faassociee, 0);
                  vol = (volumes(elem1) + volumes(elem2)) / nb_faces_elem;
                  volumes_entrelaces_[face] = vol;
                  volumes_entrelaces_[faassociee] = vol;
                  face_voisins_(face, 1) = elem2;
                  face_voisins_(faassociee, 0) = elem1;
                  face_voisins_(faassociee, 1) = elem2;
                  psc = 0;
                  for (k = 0; k < dimension; k++)
                    {
                      C1C2[k] = xv_(face, k) - xp_(face_voisins_(face, 0), k);
                      psc += face_normales_(face, k) * C1C2[k];
                    }

                  if (psc < 0)
                    for (k = 0; k < dimension; k++)
                      face_normales_(face, k) *= -1;

                  for (k = 0; k < dimension; k++)
                    face_normales_(faassociee, k) = face_normales_(face, k);
                }
            }
        }
    }

  // PQ : 10/10/05 : les faces periodiques etant a double contribution
  //                      l'appel a marquer_faces_double_contrib s'effectue dans cette methode
  //                      afin de pouvoir beneficier de conds_lim.
  Domaine_VF::marquer_faces_double_contrib(conds_lim);
  // Construction du tableau num_fac_loc_
  construire_num_fac_loc();

  static DoubleVect* ptr=0;
  if(ptr!=&volumes_som)
    {
      const Domaine& dom=domaine();
      int i;
      const int ns = nb_som();
      for(i=0; i<ns; i++)
        {
          int j=(dom.get_renum_som_perio(i));
          if(i!=j)
            volumes_som(j)+=volumes_som(i);
        }
      for(i=0; i<ns; i++)
        {
          int j=(dom.get_renum_som_perio(i));
          if(i!=j)
            volumes_som(i)=volumes_som(j);
        }
      volumes_som.echange_espace_virtuel();
      ptr = &volumes_som;
    }

  // Verification du tableau renum_som_perio
  if (Debog::active())
    {
      IntVect tmp;
      const Domaine& dom = domaine();
      dom.creer_tableau_sommets(tmp, RESIZE_OPTIONS::NOCOPY_NOINIT);
      const int n = tmp.size_array();
      for (int i=0; i<n; i++)
        tmp[i] = dom.get_renum_som_perio(i);
      Debog::verifier_indices_items("renum_som_perio",tmp.get_md_vector(),tmp);
    }

  if(get_alphaA())
    {
      // Construction de renum_arete_perio
      construire_renum_arete_perio(conds_lim);

      // Creation d'un tableau distribue pour ok_arete
      creer_tableau_aretes(ok_arete, RESIZE_OPTIONS::NOCOPY_NOINIT);

      // Si P1 alors on doit trouver les aretes superflues:
      if (get_alphaS())
        {
          // On essaie de lire un fichier .ok_arete d'un precedant calcul
          // S'il n'existe pas ou est incoherent avec le maillage, on reconstruit ok_arete
          if (!lecture_ok_arete())
            construire_ok_arete();
        }
      else
        {
          // Toutes les aretes sont necessaires si pas support P1
          ok_arete=1;
        }
      Debog::verifier_getref("ok_arete", ok_arete, ok_arete);

      //Debog::verifier("ok_arete (identique seulement si ok_arete relu dans le fichier .ok_arete):",ok_arete);
    }// fin if 3D
}

void Domaine_VEF::typer_elem(Domaine& domaine_geom)
{
  const Elem_geom_base& elem_geom = domaine_geom.type_elem().valeur();
  if (sub_type(Rectangle, elem_geom))
    domaine_geom.typer("Quadrangle");
  else if (sub_type(Hexaedre, elem_geom))
    domaine_geom.typer("Hexaedre_VEF");

  const Nom& type_elem_geom = domaine_geom.type_elem()->que_suis_je();

  if (Motcle(type_elem_geom) != "Segment")
    {
      Nom type;
      if (type_elem_geom == "Triangle")
        type = "Tri_VEF";
      else if (type_elem_geom == "Tetraedre")
        type = "Tetra_VEF";
      else if (type_elem_geom == "Quadrangle")
        type = "Quadri_VEF";
      else if (type_elem_geom == "Hexaedre_VEF")
        type = "Hexa_VEF";
      else
        {
          Cerr << "probleme de typage dans Elem_VEF::typer" << finl;
          Process::exit();
        }
      type_elem_.typer(type);
    }
}

/*! @brief creation de l'espace distant pour les faces virtuelles; creation du tableau des faces virtuelles de bord
 *
 */
void Domaine_VEF::creer_faces_virtuelles_non_std()

{
  ind_faces_virt_non_std_.resize_array(314);
  ind_faces_virt_non_std_ = -999;
}

DoubleTab& Domaine_VEF::vecteur_face_facette()
{
  // On construit si de taille nul
  // ou si le maillage est deformable
  if (vecteur_face_facette_.size() == 0 || domaine().deformable())
    {
      // Taille 8*n*4*3*2=192n
      const int nfa7 = type_elem().nb_facette();
      const int nb_poly_tot = nb_elem_tot();
      vecteur_face_facette_.resize(nb_poly_tot, nfa7, dimension, 2);
      const IntTab& KEL = type_elem().KEL();
      const IntTab& les_Polys = domaine().les_elems();
      const DoubleTab& coord = domaine().coord_sommets();
      const DoubleTab& xg = xp();
      int nb_som_facette=dimension;
      for (int poly = 0; poly < nb_poly_tot; poly++)
        {
          // Boucle sur les facettes du polyedre non standard:
          for (int fa7 = 0; fa7 < nfa7; fa7++)
            {
              int num1 = elem_faces(poly, KEL(0, fa7));
              int num2 = elem_faces(poly, KEL(1, fa7));

              // Calcul des rx0 et rx1 :
              for (int i = 0; i < dimension; i++)
                {
                  // Calcul de la ieme coordonnee du centre de la fa7
                  double coord_centre_fa7 = xg(poly, i);
                  for (int num_som_fa7 = 0; num_som_fa7 < nb_som_facette - 1; num_som_fa7++)
                    {
                      int isom_loc = KEL(num_som_fa7 + 2, fa7);
                      int isom_glob = les_Polys(poly, isom_loc);
                      coord_centre_fa7 += coord(isom_glob, i);
                    }
                  coord_centre_fa7 /= nb_som_facette;
                  // Fin calcul de la ieme coordonnee du centre de la fa7
                  vecteur_face_facette_(poly,fa7,i,0) = coord_centre_fa7 - xv_(num1,i);
                  vecteur_face_facette_(poly,fa7,i,1) = coord_centre_fa7 - xv_(num2,i);
                }
              // Fin de Calcul des rx0 et rx1
            }
        }
    }
  return vecteur_face_facette_;
}
