/****************************************************************************
* Copyright (c) 2021, CEA
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

#include <Redresser_hexaedres_vdf.h>
#include <Octree_Double.h>
#include <Domaine.h>
#include <Rectangle.h>
#include <Hexaedre.h>
#include <Quadrangle_VEF.h>
#include <Hexaedre_VEF.h>
Implemente_instanciable(Redresser_hexaedres_vdf,"Redresser_hexaedres_vdf",Interprete_geometrique_base);

Sortie& Redresser_hexaedres_vdf::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Redresser_hexaedres_vdf::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

Entree& Redresser_hexaedres_vdf::interpreter_(Entree& is)
{
  if (Process::nproc()>1)
    {
      Cerr << "Redresser_hexaedres_vdf can't be used during a parallel calculation." << finl;
      exit();
    }
  associer_domaine(is);
  DoubleTab& les_sommets = domaine().les_sommets();
  int nb_sommets = les_sommets.dimension(0);
  double epsilon = Objet_U::precision_geom;
  int ok=0;
  // On type en rectangle ou hexaedre
  const Elem_geom_base& elem=domaine().zone(0).type_elem().valeur();
  Cerr << "Attempt on the mesh ";
  if                 (sub_type(Rectangle,elem)) Cerr << "to correct rectangles..." << finl;
  else if          (sub_type(Quadrangle_VEF,elem)) Cerr << "to correct VEF quadrangles into rectangles..." << finl;
  else if          (sub_type(Hexaedre,elem)) Cerr << "to correct hexahedra..." << finl;
  else if          (sub_type(Hexaedre_VEF,elem)) Cerr << "to correct VEF hexahedra into regular hexahedra..." << finl;
  else
    {
      Cerr << "This type of element ("<<elem.que_suis_je()<<") is not supported by Redresser_hexaedres_vdf" << finl;
      exit();
    }
  if (dimension==2) domaine().zone(0).typer("Rectangle");
  if (dimension==3) domaine().zone(0).typer("Hexaedre");
  Elem_geom_base& new_elem=domaine().zone(0).type_elem().valeur();
  do
    {
      double correction_max = 0;
      Octree_Double octree;
      octree.build_nodes(les_sommets, 0 /* do not include virtual nodes */);
      // direction indique la coordonnee qu'on va corriger
      for (int direction = 0; direction < dimension; direction++)
        {
          ArrOfInt marqueurs(nb_sommets); // initialise a zero
          ArrOfInt liste_sommets;
          liste_sommets.set_smart_resize(1);
          int prochain_sommet = 0;
          while (prochain_sommet < nb_sommets)
            {
              // Cherche le prochain sommet non marque:
              for (; prochain_sommet < nb_sommets; prochain_sommet++)
                if (! marqueurs[prochain_sommet])
                  break;
              if (prochain_sommet == nb_sommets)
                break;
              // Trouver tous les sommets qui sont dans une boite tres grande
              // en dehors de la direction dir:
              double x = les_sommets(prochain_sommet, 0);
              double y = les_sommets(prochain_sommet, 1);
              double z = (dimension == 3) ? les_sommets(prochain_sommet, 2) : 0.;
              double xmin = (direction==0) ? (x-epsilon) : (-DMAXFLOAT);
              double xmax = (direction==0) ? (x+epsilon) : (+DMAXFLOAT);
              double ymin = (direction==1) ? (y-epsilon) : (-DMAXFLOAT);
              double ymax = (direction==1) ? (y+epsilon) : (+DMAXFLOAT);
              double zmin = (direction==2) ? (z-epsilon) : (-DMAXFLOAT);
              double zmax = (direction==2) ? (z+epsilon) : (+DMAXFLOAT);
              octree.search_elements_box(xmin, ymin, zmin, xmax, ymax, zmax, liste_sommets);
              // Pour chaque sommet, corriger deux des coordonnees
              // (on met les coordonnees du point trouve)
              const int n = liste_sommets.size_array();
              for (int i = 0; i < n; i++)
                {
                  const int j = liste_sommets[i];
                  // le premier sommet sert de reference, ne pas le corriger:
                  if (j == prochain_sommet)
                    continue;
                  // coordonnee de reference (on aligne tous les autres points du
                  // plan de maillage sur celui-la:
                  const double coord_ref = les_sommets(prochain_sommet, direction);

                  // verifier que le sommet est bien dans la boite (l'octree
                  // renvoie les sommets "potentiellement" a l'interieur
                  const double coord = les_sommets(j, direction);
                  if (std::fabs(coord-coord_ref) > epsilon)
                    continue;

                  correction_max=std::max(correction_max,std::fabs(coord_ref-les_sommets(j, direction)));
                  // Corriger:
                  les_sommets(j, direction) = coord_ref;
                  marqueurs[j] = 1; // sommet traite !
                }
              marqueurs[prochain_sommet] = 1;
            }
        }
      epsilon*=10;
      Cerr << "Redresser_hexaedres_vdf, epsilon=" << epsilon << " maximum_correction=" << correction_max << finl;
      if (epsilon>1)
        {
          Cerr << "Redresser_hexaedres_vdf failed" << finl;
          Cerr << "it has failed to make regular hexahedra on the mesh." << finl;
          exit();
        }
      if (dimension==2)
        ok=ref_cast(Rectangle,new_elem).reordonner_elem();
      else if (dimension==3)
        ok=ref_cast(Hexaedre,new_elem).reordonner_elem();
    }
  while (ok==-1);
  Cerr << "End and success of Redresser_hexaedres_vdf" << finl;
  return is;
}
