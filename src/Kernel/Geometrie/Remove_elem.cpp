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

#include <NettoieNoeuds.h>
#include <Remove_elem.h>
#include <TRUSTLists.h>
#include <TRUSTArray.h>
#include <Scatter.h>
#include <Param.h>

Implemente_instanciable(Remove_elem, "Remove_elem", Interprete_geometrique_base);
// XD remove_elem interprete remove_elem -1 Keyword to remove element from a VDF mesh (named domaine_name), either from an explicit list of elements or from a geometric condition defined by a condition f(x,y)>0 in 2D and f(x,y,z)>0 in 3D. All the new borders generated are gathered in one boundary called : newBord (to rename it, use RegroupeBord keyword. To split it to different boundaries, use DecoupeBord_Pour_Rayonnement keyword). Example of a removed zone of radius 0.2 centered at (x,y)=(0.5,0.5): NL2 Remove_elem dom { fonction $0.2*0.2-(x-0.5)^2-(y-0.5)^2>0$ } NL2 Warning : the thickness of removed zone has to be large enough to avoid singular nodes as decribed below : \includepng{{removeelem.jpeg}}{{11.234}}
// XD  attr domaine ref_domaine domain 0 Name of domain
// XD  attr bloc remove_elem_bloc bloc 0 not_set

Sortie& Remove_elem::printOn(Sortie& os) const { return Interprete::printOn(os); }

Entree& Remove_elem::readOn(Entree& is) { return Interprete::readOn(is); }

int Remove_elem::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  int retval = 1;
  if (mot == "liste")
    {
      int nb_elem, elem;
      is >> nb_elem;
      for (int i = 0; i < nb_elem; i++)
        {
          is >> elem;
          listelem.add(elem);
        }
    }
  else
    retval = -1;

  return retval;
}

// XD remove_elem_bloc objet_lecture nul 1 not_set
Entree& Remove_elem::interpreter_(Entree& is)
{
  associer_domaine(is);
  Param param(que_suis_je());
  Nom fonction;
  param.ajouter_non_std("liste", this); // XD_ADD_P listentier not_set
  param.ajouter("fonction", &fonction); // XD_ADD_P chaine not_set
  param.lire_avec_accolades_depuis(is);
  if (fonction == "??")
    f_ok = 0;
  else
    {
      Cerr << "Reading and interpretation of the function " << fonction << " ... ";
      f.setNbVar(3);
      f.setString(fonction);
      f.addVar("x");
      f.addVar("y");
      f.addVar("z");
      f.parseString();
      f_ok = 1;
      Cerr << " Ok" << finl;
    }

  Scatter::uninit_sequential_domain(domaine());
  remove_elem_(domaine());
  Scatter::init_sequential_domain(domaine());
  NettoieNoeuds::nettoie(domaine());
  Cerr << "Refinement... OK" << finl;
  return is;
}

void Remove_elem::recreer_faces(Domaine& domaine, Faces& faces, IntTab& som_face) const
{
  IntTab& sommets = faces.les_sommets();
  int nb_faces = sommets.dimension(0);
  int nbs = (dimension == 2) ? 2 : 4;  // nombre de sommets par face
  IntTab faces_recreees(nb_faces, nbs);

  int ii = 0;

  for (int i = 0; i < nb_faces; i++)
    {
      ArrOfInt ind(4);
      ind[0] = sommets(i, 0);
      ind[1] = sommets(i, 1);
      ind[2] = (dimension == 3) ? sommets(i, 2) : -1;
      ind[3] = (dimension == 3) ? sommets(i, 3) : -1;
      ind.ordonne_array(); // ordonnancement des indices pour eviter les mauvaises surprises de numerotations d'indices

      int trouve = 0;

      int j = 0;
      for (; j < som_face.dimension(2); j++)
        {
          if (som_face(ind[3], 0, j) == ind[2] && som_face(ind[3], 1, j) == ind[1] && som_face(ind[3], 2, j) == ind[0])
            {
              trouve = 1;
              break;
            }
        }

      if (trouve == 1) // on "supprime" la face en reinitialisant a -1 les indices
        {
          som_face(ind[3], 0, j) = -1;
          som_face(ind[3], 1, j) = -1;
          som_face(ind[3], 2, j) = -1;
        }
      else
        {
          faces_recreees(ii, 0) = sommets(i, 0);
          faces_recreees(ii, 1) = sommets(i, 1);
          if (dimension == 3)
            {
              faces_recreees(ii, 2) = sommets(i, 2);
              faces_recreees(ii, 3) = sommets(i, 3);
            }
          ii++;
        }
    }

  faces_recreees.resize(ii, nbs);

  sommets.reset();
  sommets.ref(faces_recreees);
}

void Remove_elem::creer_faces(Domaine& dom, Faces& faces, IntTab& som_face) const
{
  faces.dimensionner(1);
  IntTab& sommets = faces.les_sommets();
  int nbsom = domaine().les_sommets().dimension(0);
  int nbs = (dimension == 2) ? 2 : 4;  // nombre de sommets par face
  IntTab faces_recreees(1, nbs);

  int ii = 0;

  for (int i = 0; i < nbsom; i++)
    {
      for (int j = 0; j < som_face.dimension(2); j++)
        {
          if (som_face(i, 0, j) != -1)
            {
              faces_recreees.resize(ii + 1, nbs);
              faces_recreees(ii, 0) = i;
              faces_recreees(ii, 1) = som_face(i, 0, j);
              if (dimension == 3)
                {
                  faces_recreees(ii, 2) = som_face(i, 1, j);
                  faces_recreees(ii, 3) = som_face(i, 2, j);
                }
              ii++;
            }
        }
    }

  faces.dimensionner(ii);
  sommets.ref(faces_recreees);
}

void Remove_elem::remplir_liste(IntTab& som_face, int ind1, int ind2, int ind3, int ind4) const
{
  ArrOfInt ind(4);
  ind[0] = ind1;
  ind[1] = ind2;
  ind[2] = ind3;
  ind[3] = ind4;
  ind.ordonne_array(); // ordonnancement des indices pour eviter les mauvaises surprises de numerotations d'indices (rencontrees prealablement)

  int trouve = 0;

  int j = 0;
  for (; j < som_face.dimension(2); j++)
    {
      if (som_face(ind[3], 0, j) == ind[2] && som_face(ind[3], 1, j) == ind[1] && som_face(ind[3], 2, j) == ind[0])
        {
          trouve = 1;
          break;
        }
    }

  if (trouve == 1) // on "supprime" la face en reinitialisant a -1 les indices concernes
    {
      som_face(ind[3], 0, j) = -1;
      som_face(ind[3], 1, j) = -1;
      som_face(ind[3], 2, j) = -1;
    }
  else // on "ajoute" la face en affectant les indices aux premieres cases "libres" (=-1)
    {
      for (j = 0; j < som_face.dimension(2); j++)
        {
          if (som_face(ind[3], 0, j) == -1)
            break;
        }
      som_face(ind[3], 0, j) = ind[2];
      som_face(ind[3], 1, j) = ind[1];
      som_face(ind[3], 2, j) = ind[0];
    }
}

void Remove_elem::remove_elem_(Domaine& dom)
{
  if (dom.type_elem()->que_suis_je() == "Rectangle" || dom.type_elem()->que_suis_je() == "Hexaedre")
    {

      IntTab& les_elems = dom.les_elems();
      int oldsz = les_elems.dimension(0);
      ArrOfInt marq_remove(oldsz);
      int nbsom = domaine().les_sommets().dimension(0);

      int nbs = (dimension == 2) ? 4 : 8;  // nombre de sommets par element
      IntTab new_elems(oldsz, nbs);

      int nbfacesom = (dimension == 2) ? 4 : 4 * 3; // nbre de faces connectees a un sommet
      IntTab som_face(nbsom, 3, nbfacesom);
      som_face = -1;

      if (f_ok)
        {
          DoubleTab xg(oldsz, dimension);
          dom.type_elem()->calculer_centres_gravite(xg);
          for (int i = 0; i < oldsz; i++)
            {
              f.setVar(0, xg(i, 0));
              f.setVar(1, xg(i, 1));
              if (dimension == 3)
                f.setVar(2, xg(i, 2));
              //if(f.eval()) listelem.add(i);
              if ((int) (f.eval() + 0.5))
                marq_remove[i] = 1; //listelem.add(i); // pour etre conforme a ce qui est fait dans DecoupeBord
            }
        }
      else
        {
          for (int i = 0; i < listelem.size(); i++)
            {
              marq_remove[listelem[i]] = 1;
            }

        }

      int j = 0;
      Cerr << "-> " << listelem.size() << " elements will be removed from the domain " << domaine().le_nom() << finl;
      /*    if (listelem.size()==0)
       {
       Cerr << "May be an error when applying Remove_elem : no elements found." << finl;
       Process::exit();
       } */
      for (int i = 0; i < oldsz; i++)
        {
          if (marq_remove[i] == 0)
            {
              for (int k = 0; k < nbs; k++)
                new_elems(j, k) = les_elems(i, k);
              j++;
            }
          else
            {
              if (dimension == 2)
                {
                  int i0 = les_elems(i, 0);
                  int i1 = les_elems(i, 1);
                  int i2 = les_elems(i, 2);
                  int i3 = les_elems(i, 3);

                  remplir_liste(som_face, i0, i1, -1, -1);
                  remplir_liste(som_face, i0, i2, -1, -1);
                  remplir_liste(som_face, i1, i3, -1, -1);
                  remplir_liste(som_face, i2, i3, -1, -1);
                }
              else
                {
                  int i0 = les_elems(i, 0);
                  int i1 = les_elems(i, 1);
                  int i2 = les_elems(i, 2);
                  int i3 = les_elems(i, 3);
                  int i4 = les_elems(i, 4);
                  int i5 = les_elems(i, 5);
                  int i6 = les_elems(i, 6);
                  int i7 = les_elems(i, 7);

                  remplir_liste(som_face, i0, i1, i2, i3);
                  remplir_liste(som_face, i0, i1, i4, i5);
                  remplir_liste(som_face, i0, i4, i2, i6);
                  remplir_liste(som_face, i1, i5, i3, i7);
                  remplir_liste(som_face, i2, i3, i6, i7);
                  remplir_liste(som_face, i4, i5, i6, i7);
                }
            }
        }

      new_elems.resize(j, nbs);
      les_elems.ref(new_elems);

      // Reconstruction de l'octree
      dom.invalide_octree();
      dom.construit_octree();
      dom.reordonner();

      {
        Cerr << " Regeneration of boundaries" << finl;
        for (auto &itr : dom.faces_bord())
          {
            Faces& les_faces = itr.faces();
            if (dimension == 2)
              les_faces.typer(Type_Face::segment_2D);
            else
              les_faces.typer(Type_Face::quadrangle_3D);
            recreer_faces(dom, les_faces, som_face);
          }
        Cerr << " addition of a new boundary issued from removed elements" << finl;
        Bord& new_bord = dom.faces_bord().add(Bord());
        new_bord.nommer("newBord");
        if (dimension == 2)
          new_bord.typer_faces(Type_Face::segment_2D);
        else
          new_bord.typer_faces(Type_Face::quadrangle_3D);
        Faces& les_faces = new_bord.faces();
        creer_faces(dom, les_faces, som_face);
      }

      {
        // Les Bords internes
        Cerr << "Regeneration of internal faces" << finl;
        for (auto &itr : dom.bords_int())
          {
            Faces& les_faces = itr.faces();
            if (dimension == 2)
              les_faces.typer(Type_Face::segment_2D);
            else
              les_faces.typer(Type_Face::quadrangle_3D);
            recreer_faces(dom, les_faces, som_face);
          }
      }

      Cerr << "END of Remove_elem..." << finl;
      Cerr << "  1 NbElem=" << dom.les_elems().dimension(0) << "  NbNod=" << dom.nb_som() << finl;
    }

  else

    {
      Cerr << "We do not yet know how to Remove_elem the " << dom.type_elem()->que_suis_je() << "s" << finl;
      exit();
    }
}


