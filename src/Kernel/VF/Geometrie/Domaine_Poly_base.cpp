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

#include <Linear_algebra_tools_impl.h>
#include <Connectivite_som_elem.h>
#include <MD_Vector_composite.h>
#include <Domaine_Poly_base.h>
#include <MD_Vector_tools.h>
#include <Comm_Group_MPI.h>
#include <Quadrangle_VEF.h>
#include <communications.h>
#include <Matrice_Morse.h>
#include <Segment_poly.h>
#include <Statistiques.h>
#include <Hexaedre_VEF.h>
#include <Matrix_tools.h>
#include <unordered_map>
#include <Array_tools.h>
#include <Quadri_poly.h>
#include <Tetra_poly.h>
#include <TRUSTLists.h>
#include <Tetraedre.h>
#include <Rectangle.h>
#include <PE_Groups.h>
#include <Hexa_poly.h>
#include <Tri_poly.h>
#include <Hexaedre.h>
#include <Triangle.h>
#include <EFichier.h>
#include <SFichier.h>
#include <Segment.h>
#include <Domaine.h>
#include <Scatter.h>
#include <EChaine.h>
#include <unistd.h>
#include <Lapack.h>
#include <numeric>
#include <vector>
#include <cfloat>
#include <tuple>
#include <cmath>
#include <cfenv>
#include <set>
#include <map>

Implemente_base(Domaine_Poly_base,"Domaine_Poly_base",Domaine_VF);

Sortie& Domaine_Poly_base::ecrit(Sortie& os) const
{
  Domaine_VF::printOn(os);
  os << "____ h_carre "<<finl;
  os << h_carre << finl;
  os << "____ type_elem_ "<<finl;
  os << type_elem_ << finl;
  os << "____ nb_elem_std_ "<<finl;
  os << nb_elem_std_ << finl;
  os << "____ volumes_entrelaces_ "<<finl;
  volumes_entrelaces_.ecrit(os);
  os << "____ face_normales_ "<<finl;
  face_normales_.ecrit(os);
  os << "____ nb_faces_std_ "<<finl;
  os << nb_faces_std_ << finl;
  os << "____ rang_elem_non_std_ "<<finl;
  rang_elem_non_std_.ecrit(os);
  return os;
}

Sortie& Domaine_Poly_base::printOn(Sortie& os) const
{
  Domaine_VF::printOn(os);

  os << h_carre << finl;
  os << type_elem_ << finl;
  os << nb_elem_std_ << finl;
  os << volumes_entrelaces_ << finl;
  os << face_normales_ << finl;
  os << xp_ << finl;
  os << xv_ << finl;
  os << nb_faces_std_ << finl;
  os << rang_elem_non_std_ << finl;
  return os;
}

Entree& Domaine_Poly_base::readOn(Entree& is)
{
  Domaine_VF::readOn(is);
  is >> h_carre;
  is >> type_elem_;
  is >> nb_elem_std_ ;
  is >> volumes_entrelaces_ ;
  is >> face_normales_ ;
  is >> xp_;
  is >> xv_;
  is >> nb_faces_std_ ;
  is >> rang_elem_non_std_ ;
  return is;
}

/*! @brief Methode appelee par Domaine_VF::discretiser apres la creation des faces reelles.
 *
 *   On reordonne les faces de sorte a placer les faces "non standard"
 *   au debut de la liste des faces. Les faces non standard sont celles
 *   dont les volumes de controles sont modifies par les conditions aux
 *   limites.
 *
 */
void Domaine_Poly_base::reordonner(Faces& les_faces)
{
  Cerr << "Domaine_Poly_base::reordonner faces " << finl;

  // Construction de rang_elem_non_std_ :
  //  C'est un vecteur indexe par les elements du domaine.
  //  size() = nb_elem()
  //  size_tot() = nb_elem_tot()
  //  Valeurs dans le tableau :
  //   rang_elem_non_std_[i] = -1 si l'element i est standard,
  //  sinon
  //   rang_elem_non_std_[i] = j, ou j est l'indice de l'element dans
  //   les tableaux indexes par les elements non standards (par exemple
  //   le tableau Domaine_Cl_Poly_base::type_elem_Cl_).
  // Un element est non standard s'il est voisin d'une face frontiere.
  {
    const Domaine& dom = domaine();
    const int nb_elements = nb_elem();
    const int nb_faces_front = domaine().nb_faces_frontiere();
    dom.creer_tableau_elements(rang_elem_non_std_);
    //    rang_elem_non_std_.resize(nb_elements);
    //    Scatter::creer_tableau_distribue(dom, JOINT_ITEM::ELEMENT, rang_elem_non_std_);
    rang_elem_non_std_ = -1;
    int nb_elems_non_std = 0;
    // D'abord on marque les elements non standards avec rang_elem_non_std_[i] = 0
    for (int i_face = 0; i_face < nb_faces_front; i_face++)
      {
        const int elem = les_faces.voisin(i_face, 0);
        if (rang_elem_non_std_[elem] < 0)
          {
            rang_elem_non_std_[elem] = 0;
            nb_elems_non_std++;
          }
      }
    nb_elem_std_ = nb_elements - nb_elems_non_std;
    rang_elem_non_std_.echange_espace_virtuel();
    int count = 0;
    const int size_tot = rang_elem_non_std_.size_totale();
    // On remplace le marqueur "0" par l'indice j.
    for (int elem = 0; elem < size_tot; elem++)
      {
        if (rang_elem_non_std_[elem] == 0)
          {
            rang_elem_non_std_[elem] = count;
            count++;
          }
      }
  }

  renumeroter(les_faces);

}

void Domaine_Poly_base::typer_elem(Domaine& domaine_geom)
{
  const Elem_geom_base& type_elem_geom = domaine_geom.type_elem().valeur();

  if (sub_type(Rectangle,type_elem_geom))
    {
      domaine_geom.typer("Quadrangle");
    }
  else if (sub_type(Hexaedre,type_elem_geom))
    domaine_geom.typer("Hexaedre_VEF");

  const Elem_geom_base& elem_geom = domaine_geom.type_elem().valeur();
  type_elem_.typer(elem_geom.que_suis_je());
}

void Domaine_Poly_base::discretiser()
{

  Domaine& domaine_geom=domaine();
  typer_elem(domaine_geom);
  Elem_geom_base& elem_geom = domaine_geom.type_elem().valeur();
  Domaine_VF::discretiser();

  // Correction du tableau facevoisins:
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
    const int fin   = nb_faces_tot();
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


  if (sub_type(Segment_poly,type_elem_.valeur()))
    {
      if (!sub_type(Segment,elem_geom))
        {
          Cerr << " The type of the element " << elem_geom.que_suis_je() << " is incorrect" << finl;
          Process::exit();
        }
    }
  else if (sub_type(Tri_poly,type_elem_.valeur()))
    {
      if (!sub_type(Triangle,elem_geom))
        {
          Cerr << " The type of the element " << elem_geom.que_suis_je() << " is incorrect" << finl;
          Cerr << " Only the Triangle type is compatible with the PolyMAC_P0 discretisation in dimension 2" << finl;
          Cerr << " You must triangulate the domain when using the TRUST mesher" ;
          Cerr << " This can be done by adding : Trianguler nom_dom" << finl;
          Process::exit();
        }
    }
  else if (sub_type(Tetra_poly,type_elem_.valeur()))
    {
      if (!sub_type(Tetraedre,elem_geom))
        {
          Cerr << " The type of the element " << elem_geom.que_suis_je() << " is incorrect" << finl;
          Cerr << " Only the Tetrahedral type is compatible with the PolyMAC_P0 discretisation in dimension 3" << finl;
          Cerr << " You must tetrahedrize the domain when using the TRUST mesher" ;
          Cerr << " This can be done by adding : Tetraedriser nom_dom" << finl;
          Process::exit();
        }
    }

  else if (sub_type(Quadri_poly,type_elem_.valeur()))
    {

      if (!sub_type(Quadrangle_VEF,elem_geom))
        {
          Cerr << " The type of the element " << elem_geom.que_suis_je() << " is incorrect" << finl;
          Process::exit();
        }
    }
  else if (sub_type(Hexa_poly,type_elem_.valeur()))
    {

      if (!sub_type(Hexaedre_VEF,elem_geom))
        {
          Cerr << " The type of the element " << elem_geom.que_suis_je() << " is incorrect" << finl;
          Process::exit();
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
    // const Domaine & dom = domaine();
    //    Scatter::creer_tableau_distribue(dom, JOINT_ITEM::FACE, face_normales_);
    creer_tableau_faces(face_normales_);
    const IntTab& face_som = face_sommets();
    IntTab& face_vois = face_voisins();
    const IntTab& elem_face = elem_faces();
    const int n_tot = nb_faces_tot();
    for (num_face = 0; num_face < n_tot; num_face++)
      {
        type_elem_->normale(num_face,
                            face_normales_,
                            face_som, face_vois, elem_face, domaine_geom);
      }

    DoubleTab old(face_normales_);
    face_normales_.echange_espace_virtuel();
    for (num_face = 0; num_face < n_tot; num_face++)
      {
        int id=1;
        for (int d=0; d<dimension; d++)
          if (!est_egal(old(num_face,d),face_normales_(num_face,d)))
            {
              id=0;
              if (!est_egal(old(num_face,d),-face_normales_(num_face,d)))
                {
                  Cerr<<"pb in faces_normales" <<finl;
                  Process::exit();
                }
            }
        if (id==0)
          {
            // on a change le sens de la normale, on inverse elem1 elem2
            std::swap(face_vois(num_face,0),face_vois(num_face,1));
          }
      }
  }

  /* recalcul de xv pour avoir les vrais CG des faces */
  const DoubleTab& coords = domaine().coord_sommets();
  for (int face = 0; dimension == 3 && face < nb_faces(); face++)
    {
      double xs[3] = { 0, }, S = 0;
      for (int k = 0; k < face_sommets_.dimension(1); k++)
        if (face_sommets_(face, k) >= 0)
          {
            int s0 = face_sommets_(face, 0), s1 = face_sommets_(face, k),
                s2 = k + 1 < face_sommets_.dimension(1) && face_sommets_(face, k + 1) >= 0 ? face_sommets_(face, k +1) : s0;
            double s = 0;
            for (int r = 0; r < 3; r++)
              for (int i = 0; i < 2; i++)
                s += (i ? -1 : 1) * face_normales_(face, r) * (coords(s2, (r + 1 +  i) % 3) - coords(s0, (r + 1 +  i) % 3))
                     * (coords(s1, (r + 1 + !i) % 3) - coords(s0, (r + 1 + !i) % 3));
            for (int r = 0; r < 3; r++) xs[r] += s * (coords(s0, r) + coords(s1, r) + coords(s2, r)) / 3;
            S += s;
          }
      if (S == 0 && sub_type(Hexa_poly,type_elem_.valeur()))
        {
          Cerr << "===============================" << finl;
          Cerr << "Error in your mesh for " << que_suis_je() << "!" << finl;
          Cerr << "Add this keyword before discretization in your data file to create polyedras:" << finl;
          Cerr << "Polyedriser " << domaine().le_nom() << finl;
          Process::exit();
        }
      for (int r = 0; r < 3; r++) xv_(face, r) = xs[r] / S;
    }
  xv_.echange_espace_virtuel();

  detecter_faces_non_planes();

  calculer_volumes_entrelaces();

  /* ordre canonique dans elem_faces_ */
  std::map<std::array<double, 3>, int> xv_fsa;
  for (int e = 0, i, j, f; e < nb_elem_tot(); e++)
    {
      for (i = 0, j = 0, xv_fsa.clear(); i < elem_faces_.dimension(1) && (f = elem_faces_(e, i)) >= 0; i++)
        xv_fsa[ {{ xv_(f, 0), xv_(f, 1), dimension < 3 ? 0 : xv_(f, 2) }}] = f;
      for (auto &&c_f : xv_fsa) elem_faces_(e, j) = c_f.second, j++;
    }

  //calculer_h_carre();
}

void Domaine_Poly_base::detecter_faces_non_planes() const
{
  const IntTab& f_e = face_voisins(), &f_s = face_sommets_;
  const DoubleTab& xs = domaine().coord_sommets(), &nf = face_normales_;
  const DoubleVect& fs = face_surfaces();
  int i, j, f, s, rk = Process::me(), np = Process::nproc();
  double sin2;
  ArrOfDouble val(np); //sur chaque proc : { cos^2 max, indice de face, indices d'elements }
  IntTab face(np), elem1(np), elem2(np);

  //sur chaque proc : on cherche l'angle le plus grand entre un sommet et le plan de sa face
  for (f = 0; f < nb_faces(); f++)
    for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
      if ((sin2 = std::pow(dot(&xs(s, 0), &nf(f, 0), &xv_(f, 0)) / fs(f), 2) / dot(&xs(s, 0), &xs(s, 0), &xv_(f, 0), &xv_(f, 0))) > val[rk])
        val[rk] = sin2, face(rk) = f, elem1(rk) = f_e(f, 0), elem2(rk) = f_e(f, 1);
  envoyer_all_to_all(val, val), envoyer_all_to_all(face, face), envoyer_all_to_all(elem1, elem1), envoyer_all_to_all(elem2, elem2);

  for (i = j = 0, sin2=0.0; i < Process::nproc(); i++)
    if (val[i] > sin2) sin2 = val[i], j = i;
  double theta = asin(sqrt(sin2)) * 180 / M_PI;
  Cerr << "Domaine_Poly_base : angle sommet/face max " << theta << " deg (proc " << j << " , face ";
  Cerr << face(j) << " , elems " << elem1(j) << " / " << elem2(j) << " )" << finl;
  if (theta > 1)
    {
      Cerr << "Domaine_Poly_base : non-planar face detected ! Please fix your mesh or call 911 ..." << finl;
      Process::exit();
    }
}

void Domaine_Poly_base::discretiser_aretes()
{
  //diverses quantites liees aux aretes
  if (dimension > 2)
    {
      domaine().creer_aretes();
      md_vector_aretes_ = domaine().aretes_som().get_md_vector();

      /* ordre canonique dans aretes_som */
      IntTab& a_s = domaine().set_aretes_som(), &e_a = domaine().set_elem_aretes();
      const DoubleTab& xs = domaine().coord_sommets();
      std::map<std::array<double, 3>, int> xv_fsa;
      for (int a = 0, i, j, s; a < a_s.dimension_tot(0); a++)
        {
          for (i = 0, j = 0, xv_fsa.clear(); i < a_s.dimension(1) && (s = a_s(a, i)) >= 0; i++) xv_fsa[ {{ xs(s, 0), xs(s, 1), xs(s, 2) }}] = s;
          for (auto &&c_s : xv_fsa) a_s(a, j) = c_s.second, j++;
        }

      //remplissage de som_aretes
      som_arete.resize(domaine().nb_som_tot());
      for (int i = 0; i < a_s.dimension_tot(0); i++)
        for (int j = 0; j < 2; j++) som_arete[a_s(i, j)][a_s(i, !j)] = i;

      //remplissage de xa (CGs des aretes), de ta_ (vecteur tangent aux aretes) et de longueur_arete_ (longueurs des aretes)
      xa_.resize(0, 3), ta_.resize(0, 3);
      creer_tableau_aretes(xa_), creer_tableau_aretes(ta_), creer_tableau_aretes(longueur_aretes_);
      for (int i = 0, k; i < domaine().nb_aretes_tot(); i++)
        {
          int s1 = a_s(i, 0), s2 = a_s(i, 1);
          longueur_aretes_(i) = sqrt( dot(&xs(s2, 0), &xs(s2, 0), &xs(s1, 0), &xs(s1, 0)));
          for (k = 0; k < 3; k++) xa_(i, k) = (xs(s1, k) + xs(s2, k)) / 2, ta_(i, k) = (xs(s2, k) - xs(s1, k)) / longueur_aretes_(i);
        }

      /* ordre canonique dans elem_aretes_ */
      for (int e = 0, i, j, a; e < nb_elem_tot(); e++)
        {
          for (i = 0, j = 0, xv_fsa.clear(); i < e_a.dimension(1) && (a = e_a(e, i)) >= 0; i++) xv_fsa[ {{ xa_(a, 0), xa_(a, 1), xa_(a, 2) }}] = a;
          for (auto &&c_a : xv_fsa) e_a(e, j) = c_a.second, j++;
        }
    }

  //MD_vector pour Champ_Elem_PolyMAC_P0P1NC (elems + faces)
  MD_Vector_composite mdc_ef;
  mdc_ef.add_part(domaine().md_vector_elements()), mdc_ef.add_part(md_vector_faces());
  mdv_elems_faces.copy(mdc_ef);

  //MD_vector pour Champ_Face_PolyMAC_P0P1NC (faces + aretes)
  MD_Vector_composite mdc_fa;
  mdc_fa.add_part(md_vector_faces()), mdc_fa.add_part(dimension < 3 ? domaine().md_vector_sommets() : md_vector_aretes());
  mdv_faces_aretes.copy(mdc_fa);
}

void Domaine_Poly_base::orthocentrer()
{
  const IntTab& f_s = face_sommets(), &e_s = domaine().les_elems(), &e_f = elem_faces();
  const DoubleTab& xs = domaine().coord_sommets(), &nf = face_normales_;
  const DoubleVect& fs = face_surfaces();
  int i, j, e, f, s, np;
  DoubleTab M(0, dimension + 1), X(dimension + 1, 1), S(0, 1), vp; //pour les systemes lineaires

  IntTrav b_f_ortho, b_e_ortho; // b_{f,e}_ortho(f/e) = 1 si la face / l'element est orthocentre
  creer_tableau_faces(b_f_ortho), domaine().creer_tableau_elements(b_e_ortho);

  /* 1. orthocentrage des faces (en dimension 3) */
  Cerr << domaine().le_nom() << " : ";
  if (dimension < 3) b_f_ortho = 1; //les faces (segments) sont deja orthcentrees!
  else for (f = 0; f < nb_faces_tot(); f++)
      {
        //la face est-elle deja orthocentree?
        double d2min = DBL_MAX, d2max = 0, d2;
        for (i = 0, np = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++, np++)
          d2min = std::min(d2min, d2 = dot(&xs(s, 0), &xs(s, 0), &xv_(f, 0), &xv_(f, 0))), d2max = std::max(d2max, d2);
        if ((b_f_ortho(f) = (d2max / d2min - 1 < 1e-8))) continue;

        //peut-on l'orthocentrer?
        M.resize(np + 1, 4), S.resize(np + 1, 1);
        for (i = 0; i < np; i++)
          for (j = 0, S(i, 0) = 0, M(i, 3) = 1; j < 3; j++)
            S(i, 0) += 0.5 * std::pow(M(i, j) = xs(f_s(f, i), j) - xv_(f, j), 2);
        for (j = 0, S(np, 0) = M(np, 3) = 0; j < 3; j++) M(np, j) = nf(f, j) / fs(f);
        if (kersol(M, S, 1e-12, nullptr, X, vp) > 1e-8) continue; //la face n'a pas d'orthocentre

        //contrainte : ne pas diminuer la distance entre xv et chaque arete de plus de 50%
        double r2min = DBL_MAX;
        for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
          {
            int s2 = f_s(f, i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0),
                a = som_arete[s].at(s2);
            std::array<double, 3> vec = cross(3, 3, &xv_(f, 0), &ta_(a, 0), &xs(s, 0));
            r2min = std::min(r2min, dot(&vec[0], &vec[0]));
          }
        if (dot(&X(0, 0), &X(0, 0)) > 0.25 * r2min) continue; //on s'eloigne trop du CG

        for (i = 0, b_f_ortho(f) = 1; i < dimension; i++)
          if (std::fabs(X(i, 0)) > 1e-8) xv_(f, i) += X(i, 0);
      }
  Cerr << 100. * mp_somme_vect(b_f_ortho) / Process::mp_sum(nb_faces()) << "% orthocentered faces " << finl;

  /* 2. orthocentrage des elements */
  Cerr << domaine().le_nom() << " : ";
  for (e = 0; e < nb_elem_tot(); e++)
    {
      //l'element est-il deja orthocentre?
      double d2min = DBL_MAX, d2max = 0, d2;
      for (i = 0, np = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++, np++)
        d2min = std::min(d2min, d2 = dot(&xs(s, 0), &xs(s, 0), &xp_(e, 0), &xp_(e, 0))), d2max = std::max(d2max, d2);
      if ((b_e_ortho(e) = (d2max / d2min - 1 < 1e-8))) continue;

      //pour qu'on puisse l'orthocentrer, il faut que ses faces le soient
      for (i = 0, j = 1; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) j = j && b_f_ortho(f);
      if (!j) continue;

      //existe-il un orthocentre?
      M.resize(np, dimension + 1), S.resize(np, 1);
      for (i = 0; i < np; i++)
        for (j = 0, S(i, 0) = 0, M(i, dimension) = 1; j < dimension; j++)
          S(i, 0) += 0.5 * std::pow(M(i, j) = xs(e_s(e, i), j) - xp_(e, j), 2);
      if (kersol(M, S, 1e-12, nullptr, X, vp) > 1e-8) continue; //l'element n'a pas d'orthocentre

      //contrainte : ne pas diminuer la distance entre xp et chaque face de plus de 50%
      double rmin = DBL_MAX;
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        rmin = std::min(rmin, std::fabs(dot(&xp_(e, 0), &nf(f, 0), &xv_(f, 0)) / fs(f)));
      if (dot(&X(0, 0), &X(0, 0)) > 0.25 * rmin * rmin) continue; //on s'eloigne trop du CG

      for (i = 0, b_e_ortho(e) = 1; i < dimension; i++)
        if (std::fabs(X(i, 0)) > 1e-8) xp_(e, i) += X(i, 0);
    }
  Cerr << 100. * mp_somme_vect(b_e_ortho) / Process::mp_sum(nb_elem()) << "% d'elements orthocentres" << finl;
}

void Domaine_Poly_base::calculer_h_carre()
{
  // Calcul de h_carre
  h_carre = 1.e30;
  if (h_carre_.size()) return; // deja fait
  h_carre_.resize(nb_faces());
  // Calcul des surfaces
  const DoubleVect& surfaces=face_surfaces();
  const int nb_faces_elem=domaine().nb_faces_elem();
  const int nbe=nb_elem();
  for (int num_elem=0; num_elem<nbe; num_elem++)
    {
      double surf_max = 0;
      for (int i=0; i<nb_faces_elem; i++)
        {
          double surf = surfaces(elem_faces(num_elem,i));
          surf_max = (surf > surf_max)? surf : surf_max;
        }
      double vol = volumes(num_elem)/surf_max;
      vol *= vol;
      h_carre_(num_elem) = vol;
      h_carre = ( vol < h_carre )? vol : h_carre;
    }
}

void disp_dt(const DoubleTab& A)
{
  int i, j, k, l;
  if (A.nb_dim() == 4)
    for (i = 0; i < A.dimension_tot(0); i++)
      for (j = 0, fprintf(stderr, i ? "}}},{" : "{{"); j < A.dimension(1); j++)
        for (k = 0, fprintf(stderr, j ? "}},{" : "{"); k < A.dimension(2); k++)
          for (l = 0, fprintf(stderr, k ? "},{" : "{"); l < A.dimension(3); l++)
            fprintf(stderr, "%.10E%s ", A.addr()[A.dimension(3) * (A.dimension(2) * (i * A.dimension(1) + j) + k) + l], l + 1 < A.dimension(3) ? "," : "");
  else if (A.nb_dim() == 3)
    for (i = 0; i < A.dimension_tot(0); i++)
      for (j = 0, fprintf(stderr, i ? "}},{" : "{{"); j < A.dimension(1); j++)
        for (k = 0, fprintf(stderr, j ? "},{" : "{"); k < A.dimension(2); k++)
          fprintf(stderr, "%.10E%s ", A.addr()[A.dimension(2) * (i * A.dimension(1) + j) + k], k + 1 < A.dimension(2) ? "," : "");
  else if (A.nb_dim() == 2)
    for (i = 0; i < A.dimension_tot(0); i++)
      for (j = 0, fprintf(stderr, i ? "},{" : "{{"); j < A.dimension(1); j++)
        fprintf(stderr, "%.10E%s ", A.addr()[i * A.dimension(1) + j], j + 1 < A.dimension(1) ? "," : "");
  else for (i = 0, fprintf(stderr, "{"); i < A.dimension_tot(0); i++)
      fprintf(stderr, "%.10E%s ", A.addr()[i], i + 1 < A.dimension_tot(0) ? "," : "");
  fprintf(stderr, A.nb_dim() == 4 ? "}}}}\n" : A.nb_dim() == 3 ? "}}}\n" : A.nb_dim() == 2 ? "}}\n" : "}\n");
}

void disp_da(const ArrOfDouble& A)
{
  int i;
  for (i = 0, fprintf(stderr, "{"); i < A.size_array(); i++)
    fprintf(stderr, "%.10E%s ", A.addr()[i], i + 1 < A.size_array() ? "," : "");
  fprintf(stderr, "}\n");
}

void disp_it(const IntTab& A)
{
  int i, j, k, l;
  if (A.nb_dim() == 4)
    for (i = 0; i < A.dimension_tot(0); i++)
      for (j = 0, fprintf(stderr, i ? "}}},{" : "{{"); j < A.dimension(1); j++)
        for (k = 0, fprintf(stderr, j ? "}},{" : "{"); k < A.dimension(2); k++)
          for (l = 0, fprintf(stderr, k ? "},{" : "{"); l < A.dimension(3); l++)
            fprintf(stderr, "%d%s ", (True_int)A.addr()[A.dimension(3) * (A.dimension(2) * (i * A.dimension(1) + j) + k) + l], l + 1 < A.dimension(3) ? "," : "");
  else if (A.nb_dim() == 3)
    for (i = 0; i < A.dimension_tot(0); i++)
      for (j = 0, fprintf(stderr, i ? "}},{" : "{{"); j < A.dimension(1); j++)
        for (k = 0, fprintf(stderr, j ? "},{" : "{"); k < A.dimension(2); k++)
          fprintf(stderr, "%d%s ", (True_int)A.addr()[A.dimension(2) * (i * A.dimension(1) + j) + k], k + 1 < A.dimension(2) ? "," : "");
  else if (A.nb_dim() == 2)
    for (i = 0; i < A.dimension_tot(0); i++)
      for (j = 0, fprintf(stderr, i ? "},{" : "{{"); j < A.dimension(1); j++)
        fprintf(stderr, "%d%s ", (True_int)A.addr()[i * A.dimension(1) + j], j + 1 < A.dimension(1) ? "," : "");
  else for (i = 0, fprintf(stderr, "{"); i < A.dimension_tot(0); i++)
      fprintf(stderr, "%d%s ", (True_int)A.addr()[i], i + 1 < A.dimension_tot(0) ? "," : "");
  fprintf(stderr, A.nb_dim() == 4 ? "}}}}\n" : A.nb_dim() == 3 ? "}}}\n" : A.nb_dim() == 2 ? "}}\n" : "}\n");
}

void disp_ia(const ArrOfInt& A)
{
  int i;
  for (i = 0, fprintf(stderr, "{"); i < A.size_array(); i++)
    fprintf(stderr, "%d%s ", (True_int)A.addr()[i], i + 1 < A.size_array() ? "," : "");
  fprintf(stderr, "}\n");
}

void disp_m(const Matrice_Base& M_in)
{
  Matrice_Morse M;
  Matrix_tools::convert_to_morse_matrix(M_in, M);
  DoubleTab A(M.nb_lignes(), M.nb_colonnes());
  for (int i = 0; i < A.dimension(0); i++)
    for (int k = M.get_tab1().addr()[i] - 1; k < M.get_tab1().addr()[i + 1] - 1; k++)
      A(i, M.get_tab2().addr()[k] - 1) = M.get_coeff().addr()[k];
  disp_dt(A);
}

void disp_mt(const tabs_t& mvect)
{
  for (auto &&n_v : mvect)
    fprintf(stderr, "%s:\n", n_v.first.c_str()), disp_dt(n_v.second), fprintf(stderr, "\n");
}

void disp_mm(const std::map<std::string, Matrice_Morse>& mmat)
{
  for (auto &&n_v : mmat)
    fprintf(stderr, "%s:\n", n_v.first.c_str()), disp_m(n_v.second), fprintf(stderr, "\n");
}

void disp_mmp(const matrices_t& mmat)
{
  for (auto &&n_v : mmat)
    fprintf(stderr, "%s:\n", n_v.first.c_str()), disp_m(*n_v.second), fprintf(stderr, "\n");
}

DoubleVect& Domaine_Poly_base::dist_norm_bord(DoubleVect& dist, const Nom& nom_bord) const
{
  for (int n_bord=0; n_bord<les_bords_.size(); n_bord++)
    {
      const Front_VF& fr_vf = front_VF(n_bord);
      if (fr_vf.le_nom() == nom_bord)
        {
          dist.resize(fr_vf.nb_faces());
          int ndeb = fr_vf.num_premiere_face();
          for (int face=ndeb; face<ndeb+fr_vf.nb_faces(); face++)
            dist(face-ndeb) = dist_norm_bord(face);
        }
    }
  return dist;
}

const IntTab& Domaine_Poly_base::equiv() const
{
  if (equiv_.nb_dim() == 3) return equiv_;
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  const DoubleTab& nf = face_normales();
  const DoubleVect& fs = face_surfaces();//, &vf = volumes_entrelaces();
  int i, j, e1, e2, f, f1, f2, d, D = dimension, ok;

  IntTrav ntot, nequiv;
  creer_tableau_faces(ntot), creer_tableau_faces(nequiv);
  equiv_.resize(nb_faces_tot(), 2, e_f.dimension(1));
  Cerr << domaine().le_nom() << " : intializing equiv... ";
  for (f = 0, equiv_ = -1; f < nb_faces_tot(); f++)
    if ((e1 = f_e(f, 0)) >= 0 && (e2 = f_e(f, 1)) >= 0)
      for (i = 0; i < e_f.dimension(1) && (f1 = e_f(e1, i)) >= 0; i++)
        for (j = 0, ntot(f)++; j < e_f.dimension(1) && (f2 = e_f(e2, j)) >= 0; j++)
          {
            if (std::fabs(std::fabs(dot(&nf(f1, 0), &nf(f2, 0)) / (fs(f1) * fs(f2))) - 1) > 1e-6) continue; //normales colineaires?
            for (ok = 1, d = 0; d < D; d++) ok &= std::fabs((xv_(f1, d) - xp_(e1, d)) - (xv_(f2, d) - xp_(e2, d))) < 1e-12; //xv - xp identiques?
            if (!ok) continue;
            equiv_(f, 0, i) = f2, equiv_(f, 1, j) = f1, nequiv(f)++; //si oui, on a equivalence
          }
  Cerr << mp_somme_vect(nequiv) * 100. / mp_somme_vect(ntot) << "% equivalent faces!" << finl;
  return equiv_;
}

const Static_Int_Lists& Domaine_Poly_base::som_elem() const
{
  if (som_elem_.get_nb_lists() >= 0) return som_elem_;
  construire_connectivite_som_elem(domaine().nb_som_tot(), domaine().les_elems(), som_elem_, 1);
  return som_elem_;
}

const IntTab& Domaine_Poly_base::elem_som_d() const
{
  if (elem_som_d_.size()) return elem_som_d_;
  const IntTab& e_s = domaine().les_elems();
  elem_som_d_.resize(nb_elem_tot() + 1);
  for (int e = 0, i; e < nb_elem_tot(); e++)
    for (elem_som_d_(e + 1) = elem_som_d_(e), i = 0; i < e_s.dimension(1) && e_s(e, i) >= 0; i++)
      elem_som_d_(e + 1)++;
  return elem_som_d_;
}

const IntTab& Domaine_Poly_base::elem_arete_d() const
{
  if (elem_arete_d_.size()) return elem_arete_d_;
  const IntTab& e_a = dimension < 3 ? elem_faces() : domaine().elem_aretes();
  elem_arete_d_.resize(nb_elem_tot() + 1);
  for (int e = 0, i; e < nb_elem_tot(); e++)
    for (elem_arete_d_(e + 1) = elem_arete_d_(e), i = 0; i < e_a.dimension(1) && e_a(e, i) >= 0; i++)
      elem_arete_d_(e + 1)++;
  return elem_arete_d_;
}

const DoubleTab& Domaine_Poly_base::vol_elem_som() const
{
  if (vol_elem_som_.size()) return vol_elem_som_; //deja fait
  const IntTab& es_d = elem_som_d(), &e_f = elem_faces(), &f_s = face_sommets(), &e_s = domaine().les_elems();
  const DoubleTab& xs = domaine().coord_sommets();
  DoubleTab& vol = vol_elem_som_;
  vol.resize(es_d(nb_elem_tot()));
  int e, s, f, i, j, k, D = dimension;
  for (e = 0; e < nb_elem_tot(); e++)
    for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
      for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++)
        {
          int sb = D < 3 ? -1 : f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0); //sommet suivant sur l'arete (3D)
          auto vec = cross(D, D, &xv_(f, 0), &xs(s, 0), &xp_(e, 0), &xp_(e, 0));
          double x = std::abs(D < 3 ? vec[2] : dot(&xs(sb, 0), &vec[0], &xp_(e, 0))); //volume du parallelepipede
          for (k = 0; k < D - 1; k++) //contribution au volume de s (2D) ou a ceux de s / sb (3D)
            vol(es_d(e) + (int)(std::find(&e_s(e, 0), &e_s(e, 0) + es_d(e + 1) - es_d(e), k ? sb : s) - &e_s(e, 0))) += x / (D < 3 ? 2 : 12);
        }
  return vol;
}

const DoubleTab& Domaine_Poly_base::pvol_som(const DoubleVect& porosite_elem) const
{
  if (pvol_som_.size()) return pvol_som_; //deja fait
  const IntTab& es_d = elem_som_d(), &e_s = domaine().les_elems();
  const DoubleTab& v_es = vol_elem_som();
  domaine().creer_tableau_sommets(pvol_som_);
  for (int e = 0; e < nb_elem_tot(); e++)
    for (int i = 0, j = es_d(e); j < es_d(e + 1); i++, j++)
      pvol_som_(e_s(e, i)) += porosite_elem(e) * v_es(j);
  pvol_som_.echange_espace_virtuel();
  return pvol_som_;
}
