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

#include <Traitement_particulier_NS_EC.h>
#include <Navier_Stokes_std.h>
#include <Zone_VF.h>
#include <Terme_Source_Acceleration.h>
#include <Milieu_base.h>
#include <Ref_ArrOfDouble.h>
#include <Ref_DoubleTab.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <sys/stat.h>

Implemente_base_sans_constructeur_ni_destructeur(Traitement_particulier_NS_EC,"Traitement_particulier_NS_EC",Traitement_particulier_NS_base);


// Description:
//
// Precondition:
// Parametre: Sortie& is
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Traitement_particulier_NS_EC::printOn(Sortie& is) const
{
  return is;
}


// Description:
//
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Traitement_particulier_NS_EC::readOn(Entree& is)
{
  return is;
}

Entree& Traitement_particulier_NS_EC::lire(Entree& is)
{
  Motcle accouverte = "{" , accfermee = "}" ;
  Motcle valec="Ec";
  Motcle motbidon, motlu;
  nb_bip=0;
  periode = mon_equation->probleme().schema_temps().pas_temps_min();
  is >> motbidon ;
  if (motbidon == accouverte)
    {
      Motcles les_mots(2);
      les_mots[0] = "Ec";
      les_mots[1] = "Ec_dans_repere_fixe";
      {
        is >> motlu;
        while(motlu != accfermee)
          {
            int rang=les_mots.search(motlu);
            switch(rang)
              {
              case 0 :
              case 1 :
                {
                  if (rang==1)
                    repere_mobile_=1;
                  is >> motlu;
                  if (motlu!="periode")
                    {
                      Cerr<<"On attendait le mot cle periode apres " << les_mots[rang] << "."<<finl;
                      exit();
                    }
                  is >> periode;
                  break;
                }
              default :
                {
                  Cerr << "Erreur dans la lecture de Traitement_particulier_NS_EC";
                  Cerr << "Les mots cles possibles sont "<< les_mots <<" { et }" << finl;
                  Cerr << "Vous avez lu :" << motlu << finl;
                  exit();
                  break;
                }
              }
            is >> motlu;
          }
        is >> motlu;
        if (motlu != accfermee)
          {
            Cerr << "Erreur dans la lecture de Traitement_particulier_NS_EC";
            Cerr << "On attendait une }" << finl;
            exit();
          }
      }
    }
  else
    {
      Cerr << "Erreur dans la lecture de Traitement_particulier_NS_EC";
      Cerr << "On attendait une {" << finl;
      exit();
    }
  return is;
}

void Traitement_particulier_NS_EC::ouvrir_fichier(SFichier& s) const
{
  // Ouverture du fichier Nom_du_cas_EC.son
  Nom nom_fich(nom_du_cas());
  nom_fich += "_EC";
  if (repere_mobile_)
    nom_fich+="_dans_repere_fixe";
  nom_fich+=".son";

  const Probleme_base& pb=mon_equation->probleme();
  struct stat f;
  if (stat(nom_fich,&f) || (nb_bip==0 && !pb.reprise_effectuee()))
    s.ouvrir(nom_fich);
  else
    s.ouvrir(nom_fich,ios::app);
  s.setf(ios::scientific);
  s.precision(8);
}

void Traitement_particulier_NS_EC::preparer_calcul_particulier()
{
  tinit = mon_equation->inconnue().temps();
  nb_bip=0;
  double Ec;
  calculer_Ec(Ec);
  if(je_suis_maitre())
    {
      SFichier le_fichier;
      ouvrir_fichier(le_fichier);
      // Ecriture premiere ligne
      le_fichier<<"# Temps        Energie_cinetique_totale"<<finl;
      le_fichier<<tinit<<" "<<Ec<<finl;
    }
}
void Traitement_particulier_NS_EC::post_traitement_particulier()
{
  double temps = mon_equation->inconnue().temps();
  double nb = floor((temps - tinit) / periode);
  if (nb > nb_bip + 0.5)
    {
      nb_bip=nb;
      double Ec;
      calculer_Ec(Ec);
      if(je_suis_maitre())
        {
          SFichier le_fichier;
          ouvrir_fichier(le_fichier);
          le_fichier<<temps<<" "<<Ec<<finl;
        }
    }
}

// Description:
//  Fonction outil utilisee dans calculer_Ec. Calcule la somme
//  de 0.5*v^2*rho*volumes_entrelaces.
static double trait_part_calculer_ec_faces(const int         face_debut,
                                           const int         nb_faces,
                                           const int                frontiere,
                                           const DoubleTab&         vitesse,
                                           const DoubleVect&         volumes_entrelaces,
                                           const DoubleTab&         xv,
                                           const DoubleTab&         masse_volumique,
                                           const ArrOfDouble&         translation,
                                           const ArrOfDouble&         rotation,
                                           const int         repere_mobile_,
                                           const ArrOfInt&        faces_doubles
                                          )
{
  const int face_fin = face_debut + nb_faces;
  double ec = 0.;
  double rho = 0.;
  const int nb_dim_1 = (vitesse.line_size() == 1);
  const int dim      = Objet_U::dimension;
  ArrOfDouble ve(Objet_U::dimension);
  for (int face = face_debut; face < face_fin; face++)
    {
      // Calcul de la vitesse d'entrainement
      if (repere_mobile_)
        {
          ve[0]=translation[0];
          ve[1]=translation[1];
          if (Objet_U::dimension==3)
            {
              ve[2]=translation[2];
              ve[0]+=rotation[1]*xv(face,2)-rotation[2]*xv(face,1);
              ve[1]+=rotation[2]*xv(face,0)-rotation[0]*xv(face,2);
              ve[2]+=rotation[0]*xv(face,1)-rotation[1]*xv(face,0);
            }
        }
      else
        ve=0;

      double v2;
      double volume;
      if (nb_dim_1)
        {
          // Une composante de vitesse a la face (VDF)
          const double v = vitesse(face);
          if (repere_mobile_)
            {
              Cerr << "Le codage de l'energie cinetique calculee dans un repere fixe" <<finl;
              Cerr << "n'est pas fait en VDF." << finl;
              Process::exit(); // En effet probleme de conception, il faudrait avoir l'orientation des faces VDF
            }
          v2 = v * v;
          // En VDF, sur les frontieres, on ne prend que le 1/2 volume entrelace
          volume = (frontiere ? 0.5 : 1) * volumes_entrelaces(face);
        }
      else
        {
          // Deux ou trois composantes (VEFP1B)
          v2 = 0.;
          for (int i = 0; i < dim; i++)
            {
              const double v_i = vitesse(face, i);
              v2 += (v_i + ve[i]) * (v_i + ve[i]);
            }
          // En VEF, cela est incorrect, il faudrait les volumes etendus:
          volume = volumes_entrelaces(face);
        }
      const int k = (masse_volumique.dimension(0)==1) ? 0 : face;
      rho = masse_volumique(k, 0);
      double contribution = (faces_doubles[face]==1) ? 0.5 : 1 ;
      ec += contribution * 0.5 * v2 * volume * rho;
    }
  return ec;
}

// Description:
//  Meme methode de calcul en VDF et en VEF.
//
//  Si aucun champ de masse volumique n'a ete associe, on calcule
//   INTEGRALE de 1/2 * u^2 sur le domaine.
//  Si un champ de masse volumique a ete associe, il doit etre
//  de type "champ aux faces". On calcule INTEGRALE de 1/2 * rho * u^2.
//  rho est un champ P0 aux elements, u est P0 sur les volumes entrelaces
// Valeur de retour:
//  La somme sur tous les processeurs de l'integrale de 1/2*rho*u*u.
void Traitement_particulier_NS_EC::calculer_Ec(double& energie_cinetique)
{

  const Zone_dis_base& zdisbase        = mon_equation->inconnue().zone_dis_base();
  const Zone_VF&    zone_VF            = ref_cast(Zone_VF, zdisbase);
  const DoubleVect& volumes_entrelaces = zone_VF.volumes_entrelaces();
  const DoubleTab&  xv                 = zone_VF.xv();
  const DoubleTab&  vitesse            = mon_equation->inconnue().valeurs();
  REF(ArrOfDouble) translation(xv);
  REF(ArrOfDouble) rotation(xv);
  REF(DoubleTab) rho(xv);
  DoubleVect rotation_nulle(dimension);
  rotation_nulle=0;
  if (repere_mobile_)
    {
      int ok=0;
      // Verification de l'existence d'un terme source Acceleration dans Navier Stokes
      const Sources& les_sources=mon_equation->sources();
      CONST_LIST_CURSEUR(Source) curseur(les_sources);
      while(curseur)
        {
          if (sub_type(Terme_Source_Acceleration,curseur->valeur()))
            {
              const Terme_Source_Acceleration& terme_source_acceleration=ref_cast(Terme_Source_Acceleration,curseur->valeur());
              // Verification que les vitesses de translation du repere
              // mobiles sont bien definies et association des tableaux translation et
              // eventuellement rotation
              if (terme_source_acceleration.champ_vitesse().non_nul())
                {
                  translation=terme_source_acceleration.champ_vitesse().valeurs();
                  if (terme_source_acceleration.omega().non_nul())
                    rotation=terme_source_acceleration.omega().valeurs();
                  else
                    rotation=rotation_nulle;
                  ok=1;
                }
            }
          ++curseur;
        }
      if (!ok)
        {
          Cerr << "Vous ne pouvez calculer l'energie cinetique dans un repere fixe" << finl;
          Cerr << "que si le repere de calcul est mobile, c'est a dire que vous avez" << finl;
          Cerr << "defini un terme source d'acceleration dans l'equation Navier Stokes." << finl;
          Cerr << "Ou bien il manque dans ce terme source la definition de la vitesse" << finl;
          Cerr << "du repere mobile dans le repere fixe. Modifier votre jeu de donnees." << finl;
          exit();
        }
    }
  double ec = 0.;

  if (has_champ_masse_volumique())
    {
      const Champ_base& champ_rho = get_champ_masse_volumique();
      rho = champ_rho.valeurs();
      if (rho->dimension(0) != zone_VF.nb_faces() || rho->line_size() != 1)
        {
          Cerr << "Erreur dans Traitement_particulier_NS_EC::calculer_Ec" << finl;
          Cerr << "le champ de masse volumique n'est pas un champ scalaire aux faces" << finl;
          Process::exit();
        }
    }
  else
    {
      rho = mon_equation->milieu().masse_volumique().valeurs();
    }
  const int nb_front = zone_VF.nb_front_Cl();
  const ArrOfInt& faces_doubles = zone_VF.faces_doubles();
  // Faces de bord
  for (int i = 0; i < nb_front; i++)
    {
      const Frontiere& fr = zone_VF.front_VF(i).frontiere();
      const int debut    = fr.num_premiere_face();
      const int nb_faces = fr.nb_faces();
      ec += trait_part_calculer_ec_faces(debut, nb_faces, 1,
                                         vitesse, volumes_entrelaces, xv, rho, translation, rotation, repere_mobile_, faces_doubles);
    }
  // Faces a l'interieur du domaine (y compris les faces de joint)
  {
    const int debut    = zone_VF.premiere_face_int();
    const int nb_faces = zone_VF.nb_faces_internes();
    ec += trait_part_calculer_ec_faces(debut, nb_faces, 0,
                                       vitesse, volumes_entrelaces, xv, rho, translation, rotation, repere_mobile_, faces_doubles);
  }
  ec = Process::mp_sum(ec);
  energie_cinetique = ec;
}
