/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Perte_Charge_Singuliere.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Perte_Charge_Singuliere.h>
#include <Motcle.h>
#include <stdlib.h>
#include <Domaine.h>
#include <Zone_VF.h>
#include <Sous_Zone.h>
#include <Octree.h>
#include <Source_base.h>
#include <DoubleVect.h>
#include <DoubleTrav.h>
#include <Param.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Discretisation_base.h>
#include <EChaine.h>

extern void convert_to(const char *s, double& ob);

// Description:
//    Lit les specifications d'une perte de charge singuliere
//    a partir d'un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: mot cle inattendu, on attendait "KX","KY", "KZ" ou "K"
// Effets de bord:
// Postcondition:
Entree& Perte_Charge_Singuliere::lire_donnees(Entree& is)
{
  Motcle motlu;
  Motcle acc_ouverte("{");
  Motcles les_motcles(4);
  les_motcles[0] = "KX";
  les_motcles[1] = "KY";
  les_motcles[2] = "KZ";
  les_motcles[3] = "K";
  regul_ = 0;
  is >> motlu;
  if (motlu != acc_ouverte)
    {
      Cerr << "On attendait le mot cle" << acc_ouverte << " a la place de " << motlu << finl;
      Process::exit();
    }
  is >> motlu;
  if (motlu != "dir")
    {
      Cerr << "On attendait le mot cle dir a la place de " << motlu << finl;
      Process::exit();
    }
  else
    {
      is >> motlu;
      int rang = les_motcles.search(motlu);
      if (rang == -1)
        {
          Cerr << "Erreur a la lecture des donnees de Perte_Charge_Singuliere" << finl;
          Cerr << "On attendait l'un des mots cles " << les_motcles << finl;
          Cerr << "a la place de " << motlu << finl;
          Process::exit();
        }
      direction_perte_charge_ = rang == 3 ? -1 : rang;

    }
  is >> motlu;
  if (motlu == "regul")
    {
      regul_ = 1;
      Nom eps_str, deb_str;
      Param param("regul");
      param.ajouter("K0", &K_, Param::REQUIRED);
      param.ajouter("deb", &deb_str, Param::REQUIRED);
      param.ajouter("eps", &eps_str, Param::REQUIRED);
      param.lire_avec_accolades(is);
      deb_cible_.setNbVar(1), eps_.setNbVar(1);
      deb_cible_.setString(deb_str), eps_.setString(eps_str);
      deb_cible_.addVar("t"), eps_.addVar("t");
      deb_cible_.parseString(), eps_.parseString();
    }
  else if (motlu == "coeff")
    {
      is >> motlu;
      convert_to(motlu.getChar(), K_);
    }
  else
    {
      Cerr << "On attendait le mot cle coeff ou regul a la place de " << motlu << finl;
      Process::exit();
    }

  Cerr << " direction_perte_charge_ " << direction_perte_charge_ << finl;
  Cerr << " perte de charge K_ " << K_ << finl;

  return is;
}

void Perte_Charge_Singuliere::lire_surfaces(Entree& is, const Domaine& le_domaine,
                                            const Zone_dis_base& zone_dis, IntVect& les_faces, IntVect& sgn)
{
  const Zone_VF& zvf = ref_cast(Zone_VF,zone_dis);
  const IntTab& elem_faces = zvf.elem_faces();
  const DoubleTab& xv = zvf.xv();
  int nfe = zvf.zone().nb_faces_elem();
  IntTab face_tab; //1 for faces in the surface
  zvf.creer_tableau_faces(face_tab);
  Champ_Don orientation;

  int algo=-1;
  Motcle method;

  Motcle motlu;
  Motcle acc_ouverte("{");
  Motcle acc_fermee("}");

  Nom nom_ss_zone, nom_surface;
  double position;

  /* Read plan */
  is >> motlu;
  if (motlu != "surface")
    {
      Cerr << "On attendait le mot cle surface a la place de " << motlu << finl;
      Process::exit();
    }
  is >> motlu;
  if (motlu != acc_ouverte)
    {
      Cerr << "On attendait le mot cle" << acc_ouverte << " a la place de " << motlu << finl;
      Process::exit();
    }
  is >> method;
  if (method=="X" || method=="Y" || method=="Z")
    {
      /* Subzone algorithm */
      algo=0;
      Nom direction=method, egal;
      if (  ((direction=="X") && (direction_perte_charge()!=0))
            || ((direction=="Y") && (direction_perte_charge()!=1))
            || ((direction=="Z") && (direction_perte_charge()!=2)))
        {
          Nom dir,sect;
          if (direction_perte_charge() == 0)
            {
              dir="KX";
              sect="X";
            }
          else if (direction_perte_charge() == 1)
            {
              dir="KY";
              sect="Y";
            }
          else if (direction_perte_charge() == 2)
            {
              dir="KZ";
              sect="Z";
            }
          Cerr << "The section specified " << direction << " is not coherent with the coefficient pressure loss direction indicated by " << dir << finl;
          Cerr << "Here the section to specify is " << sect << "." << finl;
          Process::exit();
        }
      is >> egal >> position;
      is >> nom_ss_zone;
      Cerr << " position " << direction << " " << position << finl;
      identifiant_ = nom_ss_zone + "_" + direction + "=" + Nom(position);
    }

  else if (method=="Surface")
    {
      /* Surface algorithm */
      algo=1;
      /*
            if (zvf.dimension!=3)
              {
                Cerr << "Error in Perte_Charge_Singuliere::lire_surfaces" << finl;
                Cerr << "The keyword " << method << " specified for plan definition is only possible in 3D !" << finl;
                Cerr << "You must used the method of intersection between subzone and location of plane." << finl;
                Process::exit();
              }
      */
      if (zone_dis.que_suis_je().debute_par("Zone_VDF"))
        {
          Cerr << "Error in Perte_Charge_Singuliere::lire_surfaces" << finl;
          Cerr << "The keyword " << method << " specified for plan definition is only possible in VEF discretization !" << finl;
          Cerr << "You must used the method of intersection between subzone and location of plane." << finl;
          Process::exit();
        }
      is >> nom_surface;
      Cerr << " surface " << nom_surface << finl;
      identifiant_ = nom_surface;
    }

  else
    {
      Cerr << "Error in Perte_Charge_Singuliere::lire_surfaces" << finl;
      Cerr << "The keyword " << method << " specified for plan definition is not coherent !" << finl;
      Process::exit();
    }
  is >> motlu;
  if (motlu == "orientation") is >> orientation, is >> motlu; //on lit un champ pour orienter la surface

  if (motlu != acc_fermee)
    {
      Cerr << "On attendait le mot cle" << acc_fermee << " a la place de " << motlu << finl;
      Process::exit();
    }
  is >> motlu;
  if (motlu != acc_fermee)
    {
      Cerr << "On attendait le mot cle" << acc_fermee << " a la place de " << motlu << finl;
      Process::exit();
    }
  /* Found faces */
  int compteur=0;

  if (algo==0)
    {
      /* Subzone algorithm */
      const Sous_Zone& ssz = le_domaine.ss_zone(nom_ss_zone);
      for (int poly=0; poly<ssz.nb_elem_tot(); poly++)
        {
          for (int j=0; j<nfe; j++)
            {
              int numfa = elem_faces(ssz(poly),j);
              // numfa might be negative in case of polyhedron where number of faces for an elem varies (padding)
              if (numfa >= 0 && est_egal(xv(numfa,direction_perte_charge()),position) )
                {
                  bool trouve=0;
                  for (int i=0; i<compteur; i++)
                    if (les_faces[i]==numfa)
                      {
                        trouve=1;
                        break;
                      }
                  if (trouve==0) les_faces[compteur++] = numfa, face_tab(numfa) = 1;
                }
            }
        }
    }

  else if (algo==1)
    {
      /* Surface algorithm */
      const Domaine& le_domaine2D = ref_cast(Domaine,le_domaine.interprete().objet(nom_surface));
      if (le_domaine2D.nb_zones() == 0)
        {
          Cerr << "Error in Perte_Charge_Singuliere::lire_surfaces" << finl;
          Cerr << "You must correct your data file because the " << nom_surface << " surface is not well defined in parallel !" << finl;
          Process::exit();
        }
      const Zone& zone_2D = le_domaine2D.zone(0);
      const DoubleTab& coord_sommets_2D=le_domaine2D.coord_sommets();

      DoubleTab xv2D;
      zone_2D.calculer_centres_gravite(xv2D);
      const int nb_elem_2D = zone_2D.nb_elem_tot();
      int nse2D = zone_2D.nb_som_elem();

      const DoubleTab& coord_sommets=le_domaine.coord_sommets();
      const IntTab& face_sommets = zvf.face_sommets();
      const Zone& zone_vol = le_domaine.zone(0);
      const OctreeRoot& octree_vol = zone_vol.construit_octree();

      // Loop on faces on the surface domain
      Cerr << " Surface " << nom_surface << " with " << nb_elem_2D << " faces" << finl;
      for (int ind_face=0; ind_face<nb_elem_2D; ind_face++)
        {
          // Fill elem_list_vol by elements surrounding the center of gravity
          ArrOfInt elem_list_vol;
          double zv2d=0;
          if (Objet_U::dimension>2)
            zv2d=xv2D(ind_face,2);
          octree_vol.rang_elems_sommet(elem_list_vol,xv2D(ind_face,0),xv2D(ind_face,1),zv2d);
          int nb_elem_vol = elem_list_vol.size_array();
          // Loop on elements on the volume domain
          for (int ind_elem=0; ind_elem<nb_elem_vol; ind_elem++)
            {
              int elem = elem_list_vol(ind_elem);
              // Loop on faces of each element on the volume domain
              for (int j=0, numfa; j < nfe && (numfa = elem_faces(elem,j)) >= 0; j++)
                {
                  // Test if the same face between surface domain and volum domain
                  int coincide=0;
                  // Loop on vertex of each face of each element on the volume domain
                  for (int k=0; k<nse2D; k++)
                    {
                      int numso = face_sommets(numfa,k);
                      double xcoord_vol=coord_sommets(numso,0);
                      double ycoord_vol=coord_sommets(numso,1);
                      //double zcoord_vol=coord_sommets(numso,2);
                      double  zcoord_vol=0;
                      if (Objet_U::dimension>2)
                        zcoord_vol=coord_sommets(numso,2);

                      for (int i=0; i<nse2D ; i++)
                        {
                          int numso2D = zone_2D.sommet_elem(ind_face,i);
                          double xcoord_2D=coord_sommets_2D(numso2D,0);
                          double ycoord_2D=coord_sommets_2D(numso2D,1);
                          double zcoord_2D=0;
                          if (Objet_U::dimension>2)
                            zcoord_2D=coord_sommets_2D(numso2D,2);
                          if ( est_egal(xcoord_vol,xcoord_2D)
                               && est_egal(ycoord_vol,ycoord_2D)
                               && est_egal(zcoord_vol,zcoord_2D))
                            {
                              coincide=coincide+1;
                              break;
                            }
                        }
                    }
                  if (coincide==nse2D)
                    {
                      bool trouve=0;
                      for (int i=0; i<compteur ; i++)
                        if (les_faces[i]==numfa)
                          {
                            trouve=1;
                            break;
                          }
                      if (trouve==0) les_faces[compteur++] = numfa, face_tab(numfa) = 1;
                    }
                }
            }
        }
    }

  int faces_found=mp_somme_vect(face_tab);
  if (faces_found==0)
    {
      Cerr << "Error in Perte_Charge_Singuliere::lire_surfaces" << finl;
      Cerr << "No faces has been found !" << finl;
      Process::exit();
    }
  else
    Cerr << " " << faces_found << " faces have been found for the section." << finl;

  les_faces.resize(compteur);
  if (orientation.non_nul())
    {
      sgn.resize(compteur);
      DoubleTrav xvf(compteur, Objet_U::dimension), ori(compteur, Objet_U::dimension);
      for (int i = 0; i < compteur; i++) for (int j = 0; j < Objet_U::dimension; j++) xvf(i, j) = xv(les_faces(i), j);
      orientation.valeur().valeur_aux(xvf, ori);
      for (int i = 0; i < compteur; i++)
        {
          double scal = 0;
          for (int j = 0; j < Objet_U::dimension; j++) scal += ori(i, j) * zvf.face_normales(les_faces(i), j);
          sgn(i) = (scal > 0 ? 1 : -1);
        }
    }
}

double Perte_Charge_Singuliere::calculate_Q(const Equation_base& eqn, const IntVect& num_faces, const IntVect& sgn) const
{
  const DoubleTab& vit = eqn.inconnue().futur();
  const Zone_VF& zvf = ref_cast(Zone_VF, eqn.zone_dis().valeur());
  const DoubleTab& rho = eqn.milieu().masse_volumique().valeurs();

  DoubleTrav deb_vect;
  zvf.creer_tableau_faces(deb_vect); //pour bien sommer les debits en parallele
  for (int i = 0; i < num_faces.size(); i++)
    {
      int f = num_faces(i), sgn_loc = sgn.size() ? sgn(i) : 1;
      if (vit.nb_dim() == 2) for (int j = 0; j < vit.dimension(1); j++) deb_vect(f) += sgn_loc * zvf.face_normales(f, j) * vit(f, j);
      else deb_vect(f) = zvf.face_surfaces(f) * sgn_loc * vit(f);
      //on passe du produit "vitesse normale * surface" au debit a travers la face (signe si l'orientation est definie, non signe sinon)
      deb_vect(f) = zvf.porosite_face(f) * (sgn.size() ? deb_vect(f) : dabs(deb_vect(f))) * rho.addr()[rho.dimension(0) > 1 ? zvf.face_voisins(f, deb_vect(f) > 0) : 0];
    }
  return mp_somme_vect(deb_vect);
}

void Perte_Charge_Singuliere::update_K(const Equation_base& eqn, double deb, DoubleVect& bilan)
{
  if (!regul_) return;
  double t = eqn.probleme().schema_temps().temps_courant(), dt = eqn.probleme().schema_temps().pas_de_temps();
  deb_cible_.setVar(0, t), eps_.setVar(0, t);
  double deb_cible = deb_cible_.eval(), eps = eps_.eval(), f_min = std::pow(1 - eps, dt), f_max = std::pow(1 + eps, dt); //bande de variation de K
  K_ *= min(max(std::pow(dabs(deb) / deb_cible, 2), f_min), f_max);

  //pour le fichier de suivi : seulement sur le maitre, car Source_base::imprimer() fait une somme sur les procs
  if (!Process::me()) bilan(0) = K_, bilan(1) = deb, bilan(2) = deb_cible;
}

