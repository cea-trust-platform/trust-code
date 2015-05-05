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
// File:        Distanceparoi.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/26
//
//////////////////////////////////////////////////////////////////////////////

#include <Distanceparoi.h>
#include <SFichierBin.h>
#include <Static_Int_Lists.h>
#include <Connectivite_som_elem.h>
#include <Faces_builder.h>



Implemente_instanciable(Distanceparoi,"Distance_paroi",Interprete_geometrique_base);

Sortie& Distanceparoi::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Distanceparoi::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

Entree& Distanceparoi::interpreter_(Entree& is)
{
  if(Process::nproc()>1)
    {
      Cerr << " Distance_paroi : the calculation of the distance to the wall is made in sequential mode !! " << finl;
      exit();
    }

  int nb_paroi;
  Motcle format;
  associer_domaine(is);
  is >> nb_paroi;

  Noms nom_paroi(nb_paroi);

  for (int i=0; i< nb_paroi; i++) is >> nom_paroi[i];

  is >> format;
  if( (format != "binaire") && (format != "formatte"))
    {
      Cerr<<"Define the writing's format 'formatte' to write the file Wall_length.xyz"<<finl;
      Cerr<<"both in binary and ascii format, otherwise specify 'binaire'"<<finl;
      exit();
    }


  Domaine& dom=domaine();
  Nom fichier = dom.le_nom();
  fichier += "_Wall_length.xyz";

  Zone& la_zone=dom.zone(0);

  if (Process::je_suis_maitre())
    Cerr << "In Distanceparoi::interpreter : Generate faces" << finl;
  Faces* les_faces_ptr=new(Faces);
  Faces& les_faces= *les_faces_ptr;

  //la_zone.creer_faces(les_faces);

  {
    // bloc a factoriser avec Zone_VF.cpp :
    Type_Face type_face = la_zone.type_elem().type_face(0);
    les_faces.typer(type_face);
    les_faces.associer_zone(la_zone);

    Static_Int_Lists connectivite_som_elem;
    const int     nb_sommets_tot = la_zone.domaine().nb_som_tot();
    const IntTab&    elements       = la_zone.les_elems();

    construire_connectivite_som_elem(nb_sommets_tot,
                                     elements,
                                     connectivite_som_elem,
                                     1 /* include virtual elements */);

    Faces_builder faces_builder;
    IntTab elem_faces; // Tableau dont on aura pas besoin
    faces_builder.creer_faces_reeles(la_zone,
                                     connectivite_som_elem,
                                     les_faces,
                                     elem_faces);
  }
  if (Process::je_suis_maitre())
    Cerr << "In Distanceparoi::interpreter : Generate faces finished" << finl;

  DoubleTab xp;                                     // centres de gravite des elements
  DoubleTab xv;                                     // centres de gravite des faces
  const DoubleTab& xs=dom.coord_sommets();        // coordonnees des sommets

  les_faces.calculer_centres_gravite(xv);
  la_zone.calculer_centres_gravite(xp);

  int nb_elem_tot = xp.dimension(0);
  int dim = xv.dimension(1);

  DoubleVect wall_length(nb_elem_tot);
  wall_length=10e+15;
  DoubleVect dist_min(wall_length);

  ifstream fic1(fichier);
  if (fic1)
    {
      Cerr << " file " << fichier << " already exits : to generate a new one, this file has to be deleted" << finl;
      exit();
    }
  else
    {
      if (Process::je_suis_maitre())
        Cerr << " Generation of the external file named : " << fichier << finl;

      for (int b=0; b<nb_paroi; b++)
        {
          int rang=la_zone.rang_frontiere(nom_paroi[b]);
          const Frontiere& la_frontiere=la_zone.frontiere(rang);

          // pas de methode const pour Frontiere::les_sommets_des_faces();
          const IntTab& sommets_face = (ref_cast(Frontiere,la_frontiere)).les_sommets_des_faces();

          int ndeb = la_frontiere.num_premiere_face();
          int nb_faces = la_frontiere.nb_faces();
          int nfin = ndeb + nb_faces;

          int pourcent=0;

          for (int face=ndeb; face<nfin; face++)
            {

              if((((face-ndeb)*100)/nb_faces)>pourcent)
                {
                  pourcent=((face-ndeb)*100)/nb_faces;
                  Cerr <<"\r"<<  pourcent << " % performs of " << nom_paroi[b] ;
                }

              if (dim==2)
                {
                  double compx_0=(xv(face,0));
                  double compy_0=(xv(face,1));

                  int som1=sommets_face(face-ndeb,0);
                  int som2=sommets_face(face-ndeb,1);

                  double nx=  xs(som1,1)-xs(som2,1);
                  double ny=-(xs(som1,0)-xs(som2,0));

                  double norme=sqrt(nx*nx+ny*ny);
                  if (norme==0)
                    {
                      Cerr << "Error in Distanceparoi::interpreter_" << finl;
                      Cerr << "Two nodes " << som1 << " and " << som2 << " seems to have same coordinates:" << finl;
                      Cerr << "xs(" << som1 << ")=" << xs(som1,0) <<" ys(" << som1 << ")= " << xs(som1,1) << finl;
                      Cerr << "xs(" << som2 << ")=" << xs(som2,0) <<" ys(" << som2 << ")= " << xs(som2,1) << finl;
                      Cerr << "Check your mesh and suppress same nodes or contact TRUST support." << finl;
                      exit();
                    }

                  if ( (la_zone.chercher_elements(compx_0+nx, compy_0+ny)) < 0 ) norme*=-1.;

                  nx/=norme;
                  ny/=norme;


                  for (int num_elem=0; num_elem<nb_elem_tot; num_elem++)
                    {
                      double min_loc=dist_min[num_elem];

                      double compx=(compx_0-xp(num_elem,0));
                      if (compx<min_loc)
                        {
                          double compy=(compy_0-xp(num_elem,1));
                          if (compy<min_loc)
                            {

                              double w_dist=sqrt(compx*compx+compy*compy);

                              if (w_dist<min_loc)
                                {
                                  double compx_vect=compx*nx;
                                  double compy_vect=compy*ny;
                                  //   double compz_vect=compz*nz;

                                  double w_distproj=sqrt(compx_vect*compx_vect+compy_vect*compy_vect);
                                  wall_length[num_elem]=w_distproj;
                                  dist_min[num_elem]=w_dist;
                                }

                            }
                        }
                    }
                }

              else if (dim==3)
                {
                  double compx_0=(xv(face,0));
                  double compy_0=(xv(face,1));
                  double compz_0=(xv(face,2));

                  int som1=sommets_face(face-ndeb,0);
                  int som2=sommets_face(face-ndeb,1);
                  int som3=sommets_face(face-ndeb,2);

                  double x1= xs(som2,0)-xs(som1,0);
                  double y1= xs(som2,1)-xs(som1,1);
                  double z1= xs(som2,2)-xs(som1,2);

                  double x2= xs(som3,0)-xs(som1,0);
                  double y2= xs(som3,1)-xs(som1,1);
                  double z2= xs(som3,2)-xs(som1,2);

                  double nx= y1*z2 - z1*y2 ;
                  double ny= z1*x2 - x1*z2 ;
                  double nz= x1*y2 - y1*x2 ;

                  double norme=sqrt(nx*nx+ny*ny+nz*nz);

                  if ( (la_zone.chercher_elements(compx_0+nx, compy_0+ny, compz_0+nz)) < 0 ) norme*=-1.;

                  nx/=norme;
                  ny/=norme;
                  nz/=norme;


                  for (int num_elem=0; num_elem<nb_elem_tot; num_elem++)
                    {
                      double min_loc=dist_min[num_elem];

                      double compx=(compx_0-xp(num_elem,0));
                      if (compx<min_loc)
                        {
                          double compy=(compy_0-xp(num_elem,1));
                          if (compy<min_loc)
                            {
                              double compz=(compz_0-xp(num_elem,2));
                              if (compz<min_loc)
                                {

                                  double w_dist=sqrt(compx*compx+compy*compy+compz*compz);

                                  if (w_dist<min_loc)
                                    {
                                      double compx_vect=compx*nx;
                                      double compy_vect=compy*ny;
                                      double compz_vect=compz*nz;

                                      double w_distproj=sqrt(compx_vect*compx_vect+compy_vect*compy_vect+compz_vect*compz_vect);
                                      wall_length[num_elem]=w_distproj;
                                      dist_min[num_elem]=w_dist;
                                    }
                                }
                            }
                        }
                    }
                }
              else
                {
                  Cerr << "ERROR : Dimension 2 or 3 are required to calculate wall distance "<< finl;
                  exit();
                }
            }// fin face
          Cerr<<finl;
        }// fin nb_paroi
    }//(!fic1)

  Cerr<<" Max distance"<< local_max_vect(dist_min)<<" "<< local_max_vect(wall_length)<<finl;



  SFichierBin fic_sauv(fichier);

  fic_sauv<<nom_paroi<<finl;
  int ntot=(nb_elem_tot*(1+Objet_U::dimension));
  // entete pour  pouvoir passer les blocs
  //    fic_sauv<<1 <<finl<<ntot << finl<< 1 <<ntot<<finl<<ntot<<finl;
  fic_sauv << 1 <<ntot <<  1 << ntot << finl << ntot << finl;
  //    fic_sauv << 1 << ntot << 1 << ntot << ntot;
  for (int p=0; p<nb_elem_tot; p++)
    {
      fic_sauv<<xp(p,0)<<" "<<xp(p,1);
      if (Objet_U::dimension==3) fic_sauv<<" "<<xp(p,2);
      fic_sauv<<" "<<wall_length(p)<<finl;
    }

  fic_sauv << ntot << 0 << 0 << 0 <<1 << 0 << finl << ntot << finl << 1 << 0 << 0 <<finl;
  //    fic_sauv<<ntot<<finl<<0<<finl<< 0<<finl<< 0<<finl<<1 <<finl<<0 <<finl<<ntot<<finl<<1 <<finl<<0<<finl<< 0<<finl;

  if (format=="formatte")
    {
      fichier = dom.le_nom();
      fichier += "_Wall_length_formatted.xyz";
      SFichier fic_sauv_format(fichier);
      fic_sauv_format<<nom_paroi<<finl;
      int ntot2=(nb_elem_tot*(1+Objet_U::dimension));
      // entete pour  pouvoir passer les blocs
      fic_sauv_format << 1 <<ntot2 <<  1 << ntot2 << finl << ntot2 << finl;
      for (int p=0; p<nb_elem_tot; p++)
        {
          fic_sauv_format<<xp(p,0)<<" "<<xp(p,1);
          if (Objet_U::dimension==3) fic_sauv_format<<" "<<xp(p,2);
          fic_sauv_format<<" "<<wall_length(p)<<finl;
        }
      fic_sauv_format << ntot2 << 0 << 0 << 0 <<1 << 0 << finl << ntot2 << finl << 1 << 0 << 0 <<finl;
    }

  delete les_faces_ptr;
  return is;
}

/* lecture


EFichier fic(nomfic);
fic>>nom_paroi;
DoubleTab walllength(nbelem);
Ecriture_Lecture_Special:::lecture(zvf,fic,nbelem,wallelngth,0);
*/
