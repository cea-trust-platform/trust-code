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

#include <Terme_Source_Canal_RANS_LES_VDF_Face.h>
#include <math.h>
#include <Champ_Uniforme.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Pb_Hydraulique.h>
#include <Pb_Thermohydraulique.h>
#include <EFichier.h>
#include <Interprete.h>
#include <SFichier.h>

Implemente_instanciable_sans_destructeur(Terme_Source_Canal_RANS_LES_VDF_Face,"Canal_RANS_LES_VDF_Face",Source_base);

// Terme_Source_Canal_RANS_LES_VDF_Face::Terme_Source_Canal_RANS_LES_VDF_Face()
// {
//   dernier_tps_calc = new double();
// }

Terme_Source_Canal_RANS_LES_VDF_Face::~Terme_Source_Canal_RANS_LES_VDF_Face()
{
  //Le destructeur est appele a l'initialisation alors
  //la sauvegarde du champ se fait hors initialisation

  if(umoy.size()!=0)
    {
      SFichier vit_sauv ("utemp_sum.dat");
      vit_sauv << utemp_sum;
      SFichier vit_sauv2 ("umoy.dat");
      vit_sauv2 << umoy;
    }
}

//// printOn
//

Sortie& Terme_Source_Canal_RANS_LES_VDF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Terme_Source_Canal_RANS_LES_VDF_Face::readOn(Entree& is )
{
  int compteur = 0;
  Motcle mot_lu;
  Motcle acc_ouverte("{");
  Motcle acc_fermee("}");

  nom_pb_rans="non_couple";
  t_av=-1;
  f_start = 0.;
  moyenne = 0;//valeur par defaut
  // 0 => moyenne spatiale
  // 1 => moyenne temporelle glissante (moyenne en alpha)
  // 2 => moyenne temporelle commence a t=f_start-t_av
  // 3 => moyenne temporelle commence a t=0 (mettre valeur positive a t_av dans .data)

  Motcles les_mots(6);
  {
    les_mots[0]="alpha_tau"; //coeff de relaxation du terme source
    les_mots[1]="Ly"; //Hauteur du canal plan (utile pour la moyenne spatiale)
    les_mots[2]="f_start"; //temps a partir duquel le terme source est active
    les_mots[3]="t_av"; //temps de prise de moyenne (moyenne temporelle glissante)
    les_mots[4]="type_moyenne"; //temps de prise de moyenne (moyenne temporelle glissante)
    les_mots[5]="nom_pb_rans";
  }
  is >> mot_lu;
  if(mot_lu != acc_ouverte)
    {
      Cerr << "On attendait { a la place de " << mot_lu
           << " lors de la lecture des parametres de la loi de paroi " << finl;
    }
  is >> mot_lu;
  while(mot_lu != acc_fermee)
    {
      int rang=les_mots.search(mot_lu);
      switch(rang)
        {
        case 0 :
          is >> alpha_tau;
          Cerr << "alpha_tau = " << alpha_tau << finl;
          compteur++;
          break;
        case 1  :
          is >> Ly;
          Cerr << "Ly = "<< Ly << finl;
          compteur++;
          break;
        case 2  :
          is >> f_start;
          Cerr << "f_start = "<< f_start << finl;
          compteur++;
          break;
        case 3  :
          is >> t_av;
          Cerr << "t_av = " << t_av << finl;
          Cerr << "type_moyenne : " << moyenne << finl;
          if((moyenne==1)&&(t_av<=0))
            {
              Cerr << "La periode en temps de la moyenne temporelle glissante est non precisee !"
                   << finl;
              exit();
            }
          compteur++;
          break;
        case 4  :
          is >> moyenne;
          Cerr << "type_moyenne : " << moyenne << finl;
          if((moyenne==1)&&(t_av<=0))
            {
              Cerr << "La periode en temps de la moyenne temporelle glissante est non precisee !"
                   << finl;
              exit();
            }
          compteur++;
          break;
        case 5  :
          is >> nom_pb_rans;

          compteur++;
          break;
        default :
          {
            Cerr << mot_lu << " n'est pas un mot compris" << finl;
            Cerr << "Les mots compris sont : " << les_mots << finl;
            exit();
          }
        }
      is >> mot_lu;
    }
  Cerr << "nom_pb_rans = " << nom_pb_rans << finl;
  Cerr << compteur << " motcles ont ete lus dans le readOn" << finl;

  init();

  return is;

}

void Terme_Source_Canal_RANS_LES_VDF_Face::associer_zones(const Zone_dis& zone_dis,
                                                          const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur());
}

void Terme_Source_Canal_RANS_LES_VDF_Face::associer_pb(const Probleme_base& pb)
{
  ;
}

void Terme_Source_Canal_RANS_LES_VDF_Face::init()
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);
  int nb_faces = zone_VDF.nb_faces();
  const double tps = mon_equation->schema_temps().temps_courant();


  int nb_elems = zone_VDF.nb_elem();
  int nb_faces_y = zone_VDF.nb_faces_Y();
  int Ny = nb_elems/(nb_faces_y-nb_elems);

  SFichier fic_moyx("moyx.dat");
  SFichier fic_moyy("moyy.dat");
  SFichier fic_moyz("moyz.dat");
  SFichier fic_ransx("ransx.dat");
  SFichier fic_ransy("ransy.dat");
  SFichier fic_ransz("ransz.dat");

  tau.resize(nb_faces);
  U_RANS.resize(nb_faces);
  umoy_spat.resize(nb_faces);
  //  champ_spat.resize(nb_faces);
  umoy_spat.resize(nb_faces);
  umoy_spat.resize(nb_faces);

  if (nom_pb_rans == "non_couple")
    {
      SFichier fic_verif("verif.RANS");
      EFichier fic_vit("vitesse_RANS.dat");

      for(int num_face=0 ; num_face<nb_faces ; num_face++)
        {
          int face;
          fic_vit >> face ;
          fic_vit >> U_RANS(face) ;
          fic_verif << face << " " << U_RANS(face) << finl;
        }
    }
  for(int num_face=0 ; num_face<nb_faces ; num_face++)
    {
      tau(num_face) = alpha_tau;
    }


  umoy_x.resize(Ny);
  umoy_y.resize(Ny+1);
  umoy_z.resize(Ny);

  umoy_x = 0.;
  umoy_y = 0.;
  umoy_z = 0.;

  init_calcul_moyenne_spat();

  utemp_gliss.resize(nb_faces);
  utemp.resize(nb_faces);

  utemp_gliss = 0.;

  utemp_gliss.resize(nb_faces);
  utemp.resize(nb_faces);
  utemp_sum.resize(nb_faces);

  utemp_gliss = 0.;
  utemp = 0.;
  utemp_sum = 0.;

  umoy.resize(nb_faces);

  //Reprise
  if (tps > f_start)
    {
      EFichier vit_umoy ("utemp_sum.dat");
      EFichier vit_umoy2 ("umoy.dat");
      SFichier vit_reprise ("LES.reprise");
      SFichier vit_reprise2 ("LES2.reprise");

      int trash;
      vit_umoy >> trash;
      Cerr << "trash = " << trash << finl;
      vit_umoy2 >> trash;
      Cerr << "trash = " << trash << finl;

      for(int num_face = 0 ; num_face<nb_faces ; num_face++)
        {
          vit_umoy >> utemp_sum(num_face);
          vit_umoy2 >> umoy(num_face);

          vit_reprise << num_face << " " << utemp_sum(num_face) << finl;
          vit_reprise2 << num_face << " " << umoy(num_face) << finl;
        }
    }
  cpt = 0;
  compteur_reprise = 0;
}//fin init

void Terme_Source_Canal_RANS_LES_VDF_Face::init_calcul_moyenne_spat()
{
  Cerr << "Debut initialisation des tableaux de moyenne spatiale..." << finl;

  // On va initialiser les differents parametres membres de la classe
  // utiles au calcul des differentes moyennes
  // Initialisation de : Yu,Yv,Yw + compt_x,compt_y,compt_z
  // + corresp_u,corresp_v,corresp_w
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);

  const DoubleTab& xv = zone_VDF.xv();

  int nb_faces = zone_VDF.nb_faces();
  int nb_elems = zone_VDF.zone().nb_elem();
  int nb_faces_y = zone_VDF.nb_faces_Y();

  int Ny; //Nombre de maille en y + 1 (=nombre de faces)
  Ny = nb_elems/(nb_faces_y-nb_elems);
  Nxy = Ny;
  Nyy = Ny+1;
  Nzy = Ny;

  const IntVect& orientation = zone_VDF.orientation();

  int num_face,j,ori,indic,indicv,indicw,trouve;
  double y;
  j=0;
  indic = 0;
  indicv = 0;
  indicw = 0;

  // dimensionnement aux valeurs rentrees dans le jeu de donnees
  Yu.resize(Nxy);
  Yv.resize(Nyy);
  Yw.resize(Nzy);

  compt_x.resize(Nxy);
  compt_y.resize(Nyy);
  compt_z.resize(Nzy);

  //   champ_spat.resize(nb_faces);

  //   champ_spat_x.resize(Nxy);
  //   champ_spat_y.resize(Nyy);
  //   champ_spat_z.resize(Nzy);

  //   champ_spat_x_2.resize(Nxy);
  //   champ_spat_y_2.resize(Nyy);
  //   champ_spat_z_2.resize(Nzy);

  corresp_u.resize(nb_faces);
  corresp_v.resize(nb_faces);
  corresp_w.resize(nb_faces);

  // initialisation
  Yu = -100.;
  Yv = -100.;
  Yw = -100.;

  compt_x = 0;
  compt_y = 0;
  compt_z = 0;

  corresp_u = -1;
  corresp_v = -1;
  corresp_w = -1;

  // remplissage des tableaux ci-dessus

  // Pour le calcul de u, v, w sur les plans d hmogeneite
  for (num_face=0; num_face<nb_faces; num_face++)
    {
      ori = orientation(num_face);
      switch(ori)
        {
        case 0 :
          {
            y = xv(num_face,1);
            trouve = 0;
            for (j=0; j<indic+1; j++)
              {
                if(y==Yu[j])
                  {
                    corresp_u[num_face] = j;
                    compt_x[j] ++;
                    j=indic+1;
                    trouve = 1;
                    break;
                  }
              }
            if (trouve==0)
              {
                corresp_u[num_face] = indic;
                Yu[indic]=y;
                compt_x[indic] ++;
                indic++;
              }
            break;
          }
        case 1 :
          {
            y = xv(num_face,1);
            trouve = 0;
            for (j=0; j<indicv+1; j++)
              {
                if(y==Yv[j])
                  {
                    corresp_v[num_face] = j;
                    compt_y[j] ++;
                    j = indicv+1;
                    trouve = 1;
                    break;
                  }
              }
            if (trouve==0)
              {
                corresp_v[num_face] = indicv;
                Yv[indicv]=y;
                compt_y[indicv] ++;
                indicv++;
              }
            break;
          }
        case 2 :
          {
            y = xv(num_face,1);
            trouve = 0;
            for (j=0; j<indicw+1; j++)
              {
                if(y==Yw[j])
                  {
                    corresp_w[num_face] = j;
                    compt_z[j] ++;
                    j = indicw+1;
                    trouve = 1;
                    break;
                  }
              }
            if (trouve==0)
              {
                Yw[indicw]=y;
                corresp_w[num_face] = indicw;
                compt_z[indicw] ++;
                indicw++;
              }
            break;
          }
        default :
          {
            Cerr << "Cas de figure impossible!!" << finl;
            exit();
            break;
          }
        }
    }
  Nxy = indic;  // nombre de y pour umoy
  Nyy = indicv;
  Nzy = indicw;

  Cerr << "Initialisation des tableaux de moyenne spatiale : OK" << finl;

}

DoubleTab Terme_Source_Canal_RANS_LES_VDF_Face::norme_vit(void) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_elems = zone_VDF.nb_elem();
  int nb_faces = zone_VDF.nb_faces();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  DoubleVect norme_vit_elem(nb_elems);
  norme_vit_elem.resize(nb_faces);
  double vit1, vit2, vit3;
  DoubleTab norme(nb_faces);

  for(int num_elem = 0 ; num_elem<nb_elems ; num_elem++)
    {
      //WARNING 3D ONLY !!!!!!
      int face1 = elem_faces(num_elem,0);
      int face2 = elem_faces(num_elem,3);
      int face3 = elem_faces(num_elem,1);
      int face4 = elem_faces(num_elem,4);
      int face5 = elem_faces(num_elem,2);
      int face6 = elem_faces(num_elem,5);

      //      Cerr << "face1 = "<<face1<<finl;
      //       Cerr << "face2 = "<<face2<<finl;
      //       Cerr << "U_RANS(face1) = "<< U_RANS(face1) <<finl;
      //       Cerr << "U_RANS(face2) = "<< U_RANS(face2) <<finl;


      vit1 = 0.5*(U_RANS(face1)+U_RANS(face2));
      vit2 = 0.5*(U_RANS(face3)+U_RANS(face4));
      vit3 = 0.5*(U_RANS(face5)+U_RANS(face6));

      norme_vit_elem(num_elem) = sqrt(vit1*vit1 + vit2*vit2 + vit3*vit3);
    }

  //Redistribution de la norme sur les faces

  for(int num_face = 0 ; num_face<nb_faces ; num_face++)
    {
      int elem0 = face_voisins(num_face,0);
      int elem1 = face_voisins(num_face,1);

      if(elem0==-1)
        norme(num_face) = norme_vit_elem(elem1);
      else if(elem1==-1)
        norme(num_face) = norme_vit_elem(elem0);
      else
        norme(num_face) = 0.5*(norme_vit_elem(elem0)+norme_vit_elem(elem1));
    }

  return norme;
}

void Terme_Source_Canal_RANS_LES_VDF_Face::moy_spat(DoubleVect& champ, DoubleVect& champ_spat_x,
                                                    DoubleVect& champ_spat_y, DoubleVect& champ_spat_z,
                                                    DoubleVect& champ_spat_x_2, DoubleVect& champ_spat_y_2,
                                                    DoubleVect& champ_spat_z_2)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();
  int nb_faces = zone_VDF.nb_faces();

  int j=-1;

  compt_x=0;
  compt_y=0;
  compt_z=0;

  for (int num_face=0; num_face<nb_faces; num_face++)
    {
      int ori = orientation[num_face];

      if(ori==0)
        {
          j=corresp_u[num_face];
          champ_spat_x[j] += champ(num_face);
          champ_spat_x_2[j] += champ(num_face)*champ(num_face);
          compt_x[j]++;
        }

      if(ori==1)
        {
          j=corresp_v[num_face];
          champ_spat_y[j] += champ(num_face);
          champ_spat_y_2[j] += champ(num_face)*champ(num_face);
          compt_y[j]++;
        }

      if(ori==2)
        {
          j=corresp_w[num_face];
          champ_spat_z[j] += champ(num_face);
          champ_spat_z_2[j] += champ(num_face)*champ(num_face);
          compt_z[j]++;
        }
    }

  for(j=0; j<Nxy; j++)
    {
      champ_spat_x[j] = champ_spat_x[j]/compt_x[j];
      champ_spat_x_2[j] = champ_spat_x_2[j]/compt_x[j];
    }

  for(j=0; j<Nyy; j++)
    {
      champ_spat_y[j] = champ_spat_y[j]/compt_y[j];
      champ_spat_y_2[j] = champ_spat_y_2[j]/compt_y[j];
    }

  for(j=0; j<Nzy; j++)
    {
      champ_spat_z[j] = champ_spat_z[j]/compt_z[j];
      champ_spat_z_2[j] = champ_spat_z_2[j]/compt_z[j];
    }
}

void Terme_Source_Canal_RANS_LES_VDF_Face::calculer_integrale_temporelle(DoubleVect& moy_temp, const DoubleVect& moy)
{
  double dt_v = mon_equation->schema_temps().pas_de_temps();
  if(je_suis_maitre())
    moy_temp.ajoute(dt_v,moy);
}

void Terme_Source_Canal_RANS_LES_VDF_Face::ecriture_moy_spat(DoubleVect& champ_spat_x, DoubleVect& champ_spat_y,
                                                             DoubleVect& champ_spat_z, DoubleVect& champ_spat_x_2,
                                                             DoubleVect& champ_spat_y_2, DoubleVect& champ_spat_z_2)
{
  const double tps = mon_equation->schema_temps().temps_courant();
  Nom temps = Nom(tps);

  Nom nom_ficx ="moy_spat_force_x_";
  nom_ficx+= temps;
  nom_ficx+= ".dat";
  SFichier ficx(nom_ficx);

  Nom nom_ficy ="moy_spat_force_y_";
  nom_ficy+= temps;
  nom_ficy+= ".dat";
  SFichier ficy(nom_ficy);

  Nom nom_ficz ="moy_spat_force_z_";
  nom_ficz+= temps;
  nom_ficz+= ".dat";
  SFichier ficz(nom_ficz);

  for(int i=0 ; i<Nxy ; i++)
    ficx << Yu[i] << " " << champ_spat_x[i] << " " << sqrt(champ_spat_x_2[i]-champ_spat_x[i]*champ_spat_x[i]) << finl;
  for(int i=0 ; i<Nyy ; i++)
    ficy << Yv[i] << " " << champ_spat_y[i] << " " << sqrt(champ_spat_y_2[i]-champ_spat_y[i]*champ_spat_y[i]) << finl;
  for(int i=0 ; i<Nzy ; i++)
    ficz << Yw[i] << " " << champ_spat_z[i] << " " << sqrt(champ_spat_z_2[i]-champ_spat_z[i]*champ_spat_z[i]) << finl;
}

void Terme_Source_Canal_RANS_LES_VDF_Face::ecriture_moy_temp(DoubleVect& champ_temp_x, DoubleVect& champ_temp_y,
                                                             DoubleVect& champ_temp_z, DoubleVect& champ_temp_x_2,
                                                             DoubleVect& champ_temp_y_2, DoubleVect& champ_temp_z_2,
                                                             const double dt)
{
  const double tps = mon_equation->schema_temps().temps_courant();
  Nom temps = Nom(tps);

  Nom nom_ficx ="moy_temp_force_x_";
  nom_ficx+= temps;
  nom_ficx+= ".dat";
  SFichier ficx(nom_ficx);

  Nom nom_ficy ="moy_temp_force_y_";
  nom_ficy+= temps;
  nom_ficy+= ".dat";
  SFichier ficy(nom_ficy);

  Nom nom_ficz ="moy_temp_force_z_";
  nom_ficz+= temps;
  nom_ficz+= ".dat";
  SFichier ficz(nom_ficz);

  for(int i=0 ; i<Nxy ; i++)
    ficx << Yu[i] << " " << champ_temp_x[i]/dt << " " << sqrt(champ_temp_x_2[i]-champ_temp_x[i]*champ_temp_x[i])/dt << finl;
  for(int i=0 ; i<Nyy ; i++)
    ficy << Yv[i] << " " << champ_temp_y[i]/dt << " " << sqrt(champ_temp_y_2[i]-champ_temp_y[i]*champ_temp_y[i])/dt << finl;
  for(int i=0 ; i<Nzy ; i++)
    ficz << Yw[i] << " " << champ_temp_z[i]/dt << " " << sqrt(champ_temp_z_2[i]-champ_temp_z[i]*champ_temp_z[i])/dt << finl;
}

void Terme_Source_Canal_RANS_LES_VDF_Face::mettre_a_jour(double temps)
{
  //Cerr << "Je suis dans le mettre_a_jour" << finl;
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();

  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const double dt = mon_equation->schema_temps().pas_de_temps();
  const double tps = mon_equation->schema_temps().temps_courant();
  const double dt_min = mon_equation->schema_temps().pas_temps_min();

  const IntVect& orientation = zone_VDF.orientation();
  int nb_elems = zone_VDF.nb_elem();
  int nb_faces = zone_VDF.nb_faces();

  int nb_faces_x = zone_VDF.nb_faces_X();
  int nb_faces_y = zone_VDF.nb_faces_Y();
  int nb_faces_z = zone_VDF.nb_faces_Z();

  //static double duree = 0;

  double vit;
  SFichier fic_moyx("moyx.dat");
  SFichier fic_moyy("moyy.dat");
  SFichier fic_moyz("moyz.dat");
  SFichier fic_f("f.dat", ios::app);
  SFichier fic_fface("force_face.dat", ios::app);
  const DoubleTab& xv = zone_VDF.xv();



  //****************************************************
  //******* MaJ de la vitesse cible (RANS) ***********
  //**************************************************
  if(nom_pb_rans != "non_couple")
    {
      REF(Probleme_base) pb_rans;
      Objet_U& obj=Interprete::objet(nom_pb_rans);

      if( sub_type(Probleme_base, obj) )
        {
          pb_rans = ref_cast(Probleme_base, obj);
        }

      U_RANS = pb_rans.valeur().equation(0).inconnue().valeurs();

    }
  //***************************************************

  if(moyenne==0)
    {
      if(tps>(f_start))
        {
          //******************************************************
          //*************** MOYENNE SPATIALE *********************
          //******************************************************

          int num_face,Nx, Ny, Nz, j;
          umoy_x = 0;
          umoy_y = 0;
          umoy_z = 0;

          Nx = nb_elems/(nb_faces_x-nb_elems); //nbre de maille
          Ny = nb_elems/(nb_faces_y-nb_elems);
          Nz = nb_elems/(nb_faces_z-nb_elems);

          for (num_face=0; num_face<nb_faces; num_face++)
            {
              int ori = orientation[num_face];
              vit = vitesse[num_face];
              if (ori==0)
                {
                  j=corresp_u[num_face];
                  umoy_x[j] += vit/((Nx+1)*Nz);
                }
              else if (ori==1)
                {
                  j=corresp_v[num_face];
                  umoy_y[j] += vit/(Nx*Nz);
                }
              else if (ori==2)
                {
                  j=corresp_w[num_face];
                  umoy_z[j] += vit/(Nx*(Nz+1));
                }
            }

          //       // Ecriture des profils moyens
          int i;
          for (i=0 ; i<Ny ; i++)
            {
              fic_moyx << i << " " << umoy_x(i) << finl;
            }
          for (i=0 ; i<Ny+1 ; i++)
            {
              fic_moyy << i << " " << umoy_y(i) << finl;
            }
          for (i=0 ; i<Ny ; i++)
            {
              fic_moyz << i << " " << umoy_z(i) << finl;
            }

          // Redistribution du profil moyen spatial sur tout le champ de vitesse

          for(num_face = 0 ; num_face<nb_faces ; num_face++)
            {
              int ori = orientation(num_face);

              if (ori==0)
                {
                  j = corresp_u(num_face);
                  umoy(num_face) = umoy_x(j);
                }
              else if (ori==1)
                {
                  j = corresp_v(num_face);
                  umoy(num_face) = umoy_y(j);
                }
              else if (ori==2)
                {
                  j = corresp_w(num_face);
                  umoy(num_face) = umoy_z(j);
                }
              else
                Cerr << "Erreur orientation dans la redistribution de la moy. spat. de SouCanRANS_LESVDFFa" << finl;

            }

          //******************************************************
          //************** FIN MOYENNE SPATIALE ******************
          //******************************************************
        }
    }//fin if moyenne = 0

  else if(moyenne==1)
    {
      //****************************************************************
      //*************** MOYENNE TEMPORELLE GLISSANTE *******************
      //****************************************************************

      //Initialisation de la moyenne temp glissante
      //avec une moyenne temporelle classique


      //       if(/*(tps>(f_start-t_av))&&*/(tps<f_start))
      //         {
      //           if (cpt==0)
      //             {
      //               for (int num_face=0;num_face<nb_faces;num_face++)
      //                 {
      //                   utemp_gliss(num_face) = vitesse(num_face)*(tps-(f_start-t_av));
      //                   umoy(num_face) = vitesse(num_face);
      //                 }
      //               cpt = 1;
      //             }
      //           else
      //             {
      //               for (int num_face=0;num_face<nb_faces;num_face++)
      //                 {
      //                   utemp_gliss(num_face) += vitesse(num_face)*dt;
      //                   umoy(num_face) = utemp_gliss(num_face)/(tps-(f_start-t_av));
      //                 }
      //             }

      //         }


      //Calcul d'une premiere moyenne temporelle significative

      //       if((tps>(f_start-t_av))&&(tps<f_start))
      //         {
      //           if (cpt==0)
      //             {
      //               for (int num_face=0;num_face<nb_faces;num_face++)
      //                 {
      //                   utemp_sum(num_face) = vitesse(num_face)*(tps-(f_start-t_av)+dt);
      //                   umoy(num_face) = vitesse(num_face);
      //                 }
      //               cpt = 3;
      //             }
      //           else
      //             {
      //               for (int num_face=0;num_face<nb_faces;num_face++)
      //                 {
      //                   utemp_sum(num_face) += vitesse(num_face)*dt;
      //                   umoy(num_face) = utemp_sum(num_face)/(tps-(f_start-t_av)+dt);
      //                 }
      //             }
      //           //fic_f << tps << " " << utemp_sum(123) << " " << umoy(123) << finl;

      //         }
      //Calcul de la moyenne temporelle glissante
      //t_av est le temps d'integration de la moyenne temp glissante

      //       if(tps>=f_start)
      //         {
      //           for (int num_face=0 ; num_face<nb_faces ; num_face++)
      //             {
      //               if(cpt==1)
      //                 {
      //                   umoy(num_face) = vitesse(num_face);
      //                 }
      //               else if(tps <= t_av)
      //                 {
      //                   umoy(num_face) = (dt/tps)*vitesse(num_face)
      //                     +(1-(dt/tps))*umoy(num_face);
      //                   cpt=2;
      //                 }
      //               else
      //                 {
      //                   umoy(num_face) = (dt/t_av)*vitesse(num_face)
      //                     +(1-(dt/t_av))*umoy(num_face);
      //                 }
      //             }
      //         }


      if(tps>dt_min)
        {
          if (cpt==0)
            {
              for (int num_face=0; num_face<nb_faces; num_face++)
                {
                  utemp_sum(num_face) = vitesse(num_face)*dt;
                  umoy(num_face) = vitesse(num_face);
                }
              cpt++;
            }
          else if (tps <= t_av)
            {
              for (int num_face=0; num_face<nb_faces; num_face++)
                {
                  utemp_sum(num_face) += vitesse(num_face)*dt;
                  umoy(num_face) = utemp_sum(num_face)/(tps-dt_min);
                }
            }
          else
            {
              for (int num_face=0; num_face<nb_faces; num_face++)
                {
                  umoy(num_face) = (dt/t_av)*vitesse(num_face)
                                   +(1-(dt/t_av))*umoy(num_face);
                }
            }

          fic_f << tps << " " << dt << " " << tps-dt_min << " "<< utemp_sum(123);
          fic_f << " " << umoy(123) << " " << vitesse(123) << " " << U_RANS(123) << finl;

        }

      //****************************************************************
      //*************** FIN MOYENNE TEMPORELLE GLISSANTE ***************
      //****************************************************************

    }// fin moyenne = 1
  else if(moyenne==2)
    {
      //******************************************************
      //*************** MOYENNE TEMPORELLE *******************
      //******************************************************

      //Calcul d'une premiere moyenne temporelle significative

      //       if((tps>=f_start)/*&&(tps<f_start)*/)
      //         {
      //           if (cpt==0)
      //             {
      //               for (int num_face=0;num_face<nb_faces;num_face++)
      //                 {
      //                   //utemp_sum(num_face) = vitesse(num_face)*(tps-f_start+dt);
      //                   umoy(num_face) = vitesse(num_face);
      //                 }
      //               cpt = 3;
      //             }
      //           else
      //             {
      //           for (int num_face=0;num_face<nb_faces;num_face++)
      //             {
      //               utemp_sum(num_face) += vitesse(num_face)*dt;
      //               umoy(num_face) = utemp_sum(num_face)/(tps-f_start+dt);
      //             }
      //            }
      //fic_f << tps << " " << utemp_sum(123) << " " << umoy(123) << finl;

      //         }

      if(tps>dt_min)
        {
          if (cpt==0)
            {
              for (int num_face=0; num_face<nb_faces; num_face++)
                {
                  utemp_sum(num_face) = vitesse(num_face)*dt;
                  umoy(num_face) = vitesse(num_face);
                }
              cpt++;
            }
          else
            {
              for (int num_face=0; num_face<nb_faces; num_face++)
                {
                  utemp_sum(num_face) += vitesse(num_face)*dt;
                  umoy(num_face) = utemp_sum(num_face)/(tps-dt_min);
                }
            }
          fic_f << tps << " " << dt << " " << tps-dt_min << " "<< utemp_sum(123);
          fic_f << " " << umoy(123) << " " << vitesse(123) << " " << U_RANS(123) << finl;

        }




      //       if(tps>=f_start)
      //         {
      //           for (int num_face=0;num_face<nb_faces;num_face++)
      //             {
      //                 utemp_sum(num_face) += dt*vitesse(num_face);
      //                 umoy(num_face)=utemp_sum(num_face)/(tps-f_start+dt);
      //             }
      //           cpt=4;
      //         }


      //***********************************************************
      //*************** FIN MOYENNE TEMPORELLE  ***************
      //*******************************************************


    }
  else if(moyenne==3)
    {
      if (tps>0)
        {
          if (cpt==0)
            {
              for (int num_face=0; num_face<nb_faces; num_face++)
                {
                  //                   duree=100*dt;
                  //                   utemp_sum(num_face) = vitesse(num_face)*duree;
                  //                   duree-=dt;
                  //                   umoy(num_face) = utemp_sum(num_face)/(tps-dt_min+duree);
                  //                   Cerr << "nb_faces = "<< nb_faces << finl;
                  //                   Cerr << "num_face = "<< num_face << finl;
                  utemp_sum(num_face) = vitesse(num_face)*dt;
                  umoy(num_face) = vitesse(num_face);
                }
              cpt ++;
            }
          else
            {
              for (int num_face=0; num_face<nb_faces; num_face++)
                {
                  utemp_sum(num_face) += vitesse(num_face)*dt;
                  umoy(num_face) = utemp_sum(num_face)/(tps-dt_min+dt);
                }
            }
        }
    }
  else
    {
      Cerr << "moyenne = " << moyenne << finl;
      Cerr << "Probleme pour le choix du type de moyenne" << finl;
    }

  compteur_reprise++;

  const DoubleVect& porosite_surf = zone_VDF.porosite_face();
  const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();

  DoubleVect force(nb_faces);

  DoubleVect force_x(nb_faces);
  DoubleVect force_y(nb_faces);
  DoubleVect force_z(nb_faces);

  DoubleVect force_x_2(nb_faces);
  DoubleVect force_y_2(nb_faces);
  DoubleVect force_z_2(nb_faces);

  DoubleVect force_x_temp(nb_faces);
  DoubleVect force_y_temp(nb_faces);
  DoubleVect force_z_temp(nb_faces);

  DoubleVect force_x_temp_2(nb_faces);
  DoubleVect force_y_temp_2(nb_faces);
  DoubleVect force_z_temp_2(nb_faces);

  double vol=0;
  DoubleTab norm=norme_vit();

  for(int num_face = 0 ; num_face<nb_faces ; num_face++)
    {
      vol = volumes_entrelaces(num_face)*porosite_surf(num_face);

      force(num_face) = ((U_RANS(num_face)-umoy(num_face))/(dt))
                        *(U_RANS(num_face)/norm(num_face))*vol;

    }

  moy_spat(force, force_x, force_y, force_z, force_x_2, force_y_2, force_z_2);

  calculer_integrale_temporelle(force_x_temp, force_x);
  calculer_integrale_temporelle(force_y_temp, force_y);
  calculer_integrale_temporelle(force_z_temp, force_z);
  calculer_integrale_temporelle(force_x_temp_2, force_x_2);
  calculer_integrale_temporelle(force_y_temp_2, force_y_2);
  calculer_integrale_temporelle(force_z_temp_2, force_z_2);

  // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
  double i, j, epsilon = 1.e-8;
  double dt_post_inst=2.;
  double tps_stats=10.;

  modf((tps+dt)/dt_post_inst + epsilon, &i);
  modf(tps/dt_post_inst + epsilon, &j);
  if(i>j)
    {
      ecriture_moy_spat(force_x, force_y, force_z, force_x_2, force_y_2, force_z_2);
      if(tps>tps_stats)
        ecriture_moy_temp(force_x_temp, force_y_temp, force_z_temp,
                          force_x_temp_2, force_y_temp_2, force_z_temp_2, tps-tps_stats);
    }

  //////////////////////////////////////////
  //developpement a nettoyer apres la these
  //////////////////////////////////////////

  int num_face = 123;
  static int cpt_sonde=0;
  if(cpt_sonde==0)
    {
      for(int face = 0 ; face<nb_faces ; face++)
        {
          if((xv(face,1) > 1.1)&&(xv(face,1) < 0.9)&&(orientation(face)==0))
            num_face=face;
        }
      fic_fface << "# " << num_face << " "<< orientation(num_face)
                << " " << xv(num_face,0) << " " << xv(num_face,1) <<  " " << xv(num_face,2) << finl;
      fic_f << "# " << num_face << " "<< orientation(num_face)
            << " " << xv(num_face,0) << " " << xv(num_face,1) <<  " " << xv(num_face,2) << finl;
      cpt_sonde++;
    }

  fic_fface <<  tps << " " << vitesse(num_face) << " " << force(num_face) << finl;
  //////////////////////////////////////////////////////////

}//fin mettre_a_jour



void Terme_Source_Canal_RANS_LES_VDF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_faces = zone_VDF.nb_faces();
  const DoubleVect& porosite_surf = zone_VDF.porosite_face();
  const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();
  const double tps = mon_equation->schema_temps().temps_courant();
  const double dt = mon_equation->schema_temps().pas_de_temps();
  const double dt_min = mon_equation->schema_temps().pas_temps_min();

  //  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();

  double vol=0;
  SFichier fic_f("f.dat", ios::app);
  SFichier fic_finst("finst.dat", ios::app);

  //double mbf = 0.; //terme de forcage instantanne ponctuel
  //   double mbf2 = 0.; //terme de forcage moyenne

  //   int cpt2=0;
  //  static double beta=1000;

  // Calcul de la norme des vitesses au centre des elements

  DoubleTab norm=norme_vit();

  if((tps>dt_min)&&(tps>f_start))
    {

      for(int num_face = 0 ; num_face<nb_faces ; num_face++)
        {
          vol = volumes_entrelaces(num_face)*porosite_surf(num_face);

          secmem(num_face) += ((U_RANS(num_face)-umoy(num_face))/(tau(num_face)*dt))
                              *(U_RANS(num_face)/norm(num_face))*vol;

          //                mbf2 += (((U_RANS(num_face)-umoy(num_face))
          //                         *(U_RANS(num_face)/norm(num_face)))
          //                        /(tau(num_face)*dt))*vol;
          //                cpt2++;
        }

      //mbf2 = mbf2/cpt2;

    }//fin if f_start

}

DoubleTab& Terme_Source_Canal_RANS_LES_VDF_Face::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}

