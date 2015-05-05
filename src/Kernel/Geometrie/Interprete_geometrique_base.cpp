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
// File:        Interprete_geometrique_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Interprete_geometrique_base.h>
#include <Domaine.h>
#include <Scatter.h>
#include <EChaine.h>
#include <Sous_Zone.h>

Implemente_base(Interprete_geometrique_base,"Interprete_geometrique_base",Interprete);

Implemente_liste(REF(Domaine));

Sortie& Interprete_geometrique_base::printOn(Sortie& os) const
{
  return os;
}


Entree& Interprete_geometrique_base::readOn(Entree& is)
{
  return is;
}

void Interprete_geometrique_base::associer_domaine(Nom& nom_dom)
{
  EChaine is(nom_dom);
  associer_domaine(is);
}

void Interprete_geometrique_base::associer_domaine(Entree& is)
{
  Cerr << "Association of the domain on which the interpreter applies " << que_suis_je() << finl;
  Nom nom_dom;
  is >> nom_dom;
  if(!sub_type(Domaine, objet(nom_dom)))
    {
      Cerr << nom_dom << " is of type " << objet(nom_dom).que_suis_je() << finl;
      Cerr << "we know to treat only one domain with the interpreter " << que_suis_je() << finl;
      exit();
    }
  domains_.add(ref_cast(Domaine, objet(nom_dom)));
}

Entree& Interprete_geometrique_base::interpreter(Entree& is)
{
  Cerr << "Execution of the interpreter " << que_suis_je() << finl;
  Process::imprimer_ram_totale();
  // Appel a l'interprete fils
  interpreter_(is);

  // On met a jour un certain nombre de choses
  // suite a la modification des domaines
  for (int j=0; j<domains_.size(); j++)
    for(int i=0; i<domaine(j).nb_zones() ; i++)
      {
        Zone& zone = domaine(j).zone(i);
        zone.invalide_octree();
        zone.faces_bord().associer_zone(zone);
        zone.faces_joint().associer_zone(zone);
        zone.faces_raccord().associer_zone(zone);
        zone.faces_int().associer_zone(zone);
        zone.type_elem().associer_zone(zone);
        zone.fixer_premieres_faces_frontiere();
        zone.associer_domaine(domaine(j));
      }
  return is;
}

// Je rajoute dans les sous-zones les nouveaux elements
void Interprete_geometrique_base::mettre_a_jour_sous_zone(Zone& zone, int& elem, int num_premier_elem, int nb_elem) const
{
  Domaine& dom=zone.domaine();
  for (int ssz=0; ssz<dom.nb_ss_zones(); ssz++)
    {
      Sous_Zone& sous_zone=dom.ss_zone(ssz);
      if ((que_suis_je()=="Raffiner_anisotrope")
          || (que_suis_je()=="Raffiner_isotrope")
          || (que_suis_je()=="Remove_elem")
          || (que_suis_je()=="Tetraedriser_homogene")
          || (que_suis_je()=="Tetraedriser_homogene_compact")
          || (que_suis_je()=="Tetraedriser_homogene_fin")
          || (que_suis_je()=="Tetraedriser_par_prisme")
          || (que_suis_je()=="Tetraedriser")
          || (que_suis_je()=="Trianguler")
          || (que_suis_je()=="Trianguler_fin")
          || (que_suis_je()=="Trianguler_H")
          || (que_suis_je()=="VerifierCoin"))
        {
          const int& nb_poly=sous_zone.nb_elem_tot();
          for (int nb_p=0; nb_p<nb_poly; nb_p++)
            {
              if (elem==sous_zone[nb_p])
                {
                  Cerr << "   The element " << elem << " which will be modified, is included to the subarea \"" << sous_zone.le_nom() << "\". So the element";
                  if (nb_elem>1) Cerr << "s";
                  Cerr << " generated [";
                  for(int num_elem=num_premier_elem; num_elem<num_premier_elem+nb_elem; num_elem++)
                    {
                      sous_zone.add_poly(num_elem);
                      if (num_elem!=num_premier_elem) Cerr << " ";
                      Cerr << num_elem;
                    }
                  Cerr << "] will be added to this subarea." << finl;
                }
            }
        }
      else
        {
          Cerr << finl;
          Cerr << "You have defined the subarea \"" << sous_zone.le_nom() << "\" before using the interpreter \"" << que_suis_je() << "\"." << finl;
          Cerr << "To correctly mark the good elements, set the subarea after to have used the interpreter." << finl;
          exit();
        }
    }
}

