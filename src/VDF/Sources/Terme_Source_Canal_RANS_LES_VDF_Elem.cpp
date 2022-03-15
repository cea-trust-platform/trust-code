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
// File:        Terme_Source_Canal_RANS_LES_VDF_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_Canal_RANS_LES_VDF_Elem.h>
#include <math.h>
#include <Champ_Uniforme.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Pb_Hydraulique.h>
#include <Pb_Thermohydraulique.h>
#include <EFichier.h>
#include <Interprete.h>
#include <SFichier.h>

Implemente_instanciable_sans_destructeur(Terme_Source_Canal_RANS_LES_VDF_Elem,"Canal_RANS_LES_VDF_P0_VDF",Source_base);



Terme_Source_Canal_RANS_LES_VDF_Elem::~Terme_Source_Canal_RANS_LES_VDF_Elem()
{
  //Le destructeur est appele a l'initialisation alors
  //la sauvegarde du champ se fait hors initialisation

  if(umoy.size()!=0)
    {
      SFichier vit_sauv ("Ttemp_sum.dat");
      vit_sauv << utemp_sum;
      SFichier vit_sauv2 ("Tmoy.dat");
      vit_sauv2 << umoy;
    }
}

//// printOn
//

Sortie& Terme_Source_Canal_RANS_LES_VDF_Elem::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Terme_Source_Canal_RANS_LES_VDF_Elem::readOn(Entree& is )
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
  // 2 => moyenne temporelle

  Motcles les_mots(6);
  {
    les_mots[0]="alpha_tau"; //coeff de relaxation du terme source
    les_mots[1]="Ly"; //Hauteur du canal plan (utile pour la moyenne spatiale)
    les_mots[2]="f_start"; //temps a partir duquel le terme source est active
    les_mots[3]="t_av"; //temps de prise de moyenne (moyenne temporelle glissante)
    les_mots[4]="type_moyenne"; //
    les_mots[5]="nom_pb_rans"; //
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
  Cerr << compteur << " motcles ont ete lus dans le readOn du terme de forcage thermique" << finl;

  init();

  return is;

}

void Terme_Source_Canal_RANS_LES_VDF_Elem::associer_zones(const Zone_dis& zone_dis,
                                                          const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur());
}

void Terme_Source_Canal_RANS_LES_VDF_Elem::associer_pb(const Probleme_base& pb)
{
  ;
}

void Terme_Source_Canal_RANS_LES_VDF_Elem::init()
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);
  const double tps = mon_equation->schema_temps().temps_courant();

  int nb_elems = zone_VDF.nb_elem();


  tau.resize(nb_elems);
  U_RANS.resize(nb_elems);
  if(nom_pb_rans == "non_couple")
    {
      SFichier fic_verif("Tverif.RANS");
      EFichier fic_vit("temperature_RANS.dat");

      for(int num_elem=0 ; num_elem<nb_elems ; num_elem++)
        {
          int elem;
          fic_vit >> elem ;
          fic_vit >> U_RANS(elem) ;
          fic_verif << elem << " " << U_RANS(elem) << finl;
        }
    }

  for(int num_elem=0 ; num_elem<nb_elems ; num_elem++)
    {
      tau(num_elem) = alpha_tau;
    }

  utemp_gliss.resize(nb_elems);
  utemp.resize(nb_elems);
  utemp_sum.resize(nb_elems);

  utemp_gliss = 0.;
  utemp = 0.;
  utemp_sum = 0.;

  umoy.resize(nb_elems);

  if (tps > f_start)
    {
      EFichier vit_umoy ("Ttemp_sum.dat");
      EFichier vit_umoy2 ("Tmoy.dat");
      SFichier vit_reprise ("TLES.reprise");
      SFichier vit_reprise2 ("TLES2.reprise");

      int trash;
      vit_umoy >> trash;
      Cerr << "trash = " << trash << finl;
      vit_umoy2 >> trash;
      Cerr << "trash = " << trash << finl;

      for(int num_elem = 0 ; num_elem<nb_elems ; num_elem++)
        {
          vit_umoy >> utemp_sum(num_elem);
          vit_umoy2 >> umoy(num_elem);

          vit_reprise << num_elem << " " << utemp_sum(num_elem) << finl;
          vit_reprise2 << num_elem << " " << umoy(num_elem) << finl;
        }
    }
  cpt = 0;
  compteur_reprise = 0;
}//fin init


///////////// Modif elem a partir d'ici///////////////

void Terme_Source_Canal_RANS_LES_VDF_Elem::mettre_a_jour(double temps)
{
  //Cerr << "Je suis dans le mettre_a_jour" << finl;

  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();

  //vitesse=temperature

  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const double dt = mon_equation->schema_temps().pas_de_temps();
  const double tps = mon_equation->schema_temps().temps_courant();
  int nb_elems = zone_VDF.nb_elem();
  const double dt_min = mon_equation->schema_temps().pas_temps_min();

  int cptbis=0;

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

      U_RANS = pb_rans.valeur().equation(1).inconnue().valeurs();

    }
  //***************************************************

  if(moyenne==2)
    {
      //******************************************************
      //*************** MOYENNE TEMPORELLE *******************
      //******************************************************

      //Calcul d'une premiere moyenne temporelle significative

      if((tps>(f_start-t_av))&&(tps<f_start))
        {
          if (cptbis==0)
            {
              for (int num_elem=0; num_elem<nb_elems; num_elem++)
                {
                  utemp_sum(num_elem) = vitesse(num_elem)*(tps-(f_start-t_av));
                  umoy(num_elem) = vitesse(num_elem);
                }
              cptbis = 3;
            }
          else
            {
              for (int num_elem=0; num_elem<nb_elems; num_elem++)
                {
                  utemp_sum(num_elem) += vitesse(num_elem)*dt;
                  umoy(num_elem) = utemp_gliss(num_elem)/(tps-(f_start-t_av));
                }
            }

        }


      if(tps>=f_start)
        {
          for (int num_elem=0; num_elem<nb_elems; num_elem++)
            {
              utemp_sum(num_elem) += dt*vitesse(num_elem);
              umoy(num_elem)=utemp_sum(num_elem)/(tps-(f_start-t_av));
            }
          cptbis=4;
        }


      //***********************************************************
      //*************** FIN MOYENNE TEMPORELLE  ***************
      //*******************************************************
    }

  else if(moyenne==3)
    {
      if (tps>0)
        {
          if (cptbis==0)
            {
              for (int num_elem=0; num_elem<nb_elems; num_elem++)
                {
                  utemp_sum(num_elem) = vitesse(num_elem)*dt;
                  umoy(num_elem) = vitesse(num_elem);
                }
              cptbis ++;
            }
          else
            {
              for (int num_elem=0; num_elem<nb_elems; num_elem++)
                {
                  utemp_sum(num_elem) += vitesse(num_elem)*dt;
                  umoy(num_elem) = utemp_sum(num_elem)/(tps-dt_min+dt);
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

}//fin mettre_a_jour



DoubleTab& Terme_Source_Canal_RANS_LES_VDF_Elem::ajouter(DoubleTab& resu) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_elems = zone_VDF.nb_elem();
  const DoubleVect& volume = zone_VDF.volumes();
  const double tps = mon_equation->schema_temps().temps_courant();
  const double dt = mon_equation->schema_temps().pas_de_temps();
  const double dt_min = mon_equation->schema_temps().pas_temps_min();

  //vitesse=temperature
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();

  double vol=0.;
  SFichier fic_f("f_temp.dat", ios::app);
  SFichier fic_finst("f_temp_inst.dat", ios::app);

  double mbf = 0.; //maximum body force
  double mbf2 = 0.; //maximum body force

  static int cpt2=0;

  // Calcul de la norme des vitesses au centre des elements

  if(((tps>f_start)&&(compteur_reprise > 1))||((moyenne==3)&&(tps>dt_min)))
    {
      for(int num_elem = 0 ; num_elem<nb_elems ; num_elem++)
        {
          vol = volume(num_elem);

          resu(num_elem) += ((U_RANS(num_elem)-umoy(num_elem))/(tau(num_elem)*dt))*vol;

          mbf2 +=((U_RANS(num_elem)-umoy(num_elem))/(tau(num_elem)*dt))*vol;

          cpt2++;
        }

      mbf2 = mbf2/cpt2;

      int num_elem=12;

      mbf = (U_RANS(num_elem)-umoy(num_elem))
            /(tau(num_elem)*dt)*vol;

      fic_finst << tps << " " << mbf << " " <<  U_RANS(num_elem)
                << " " << umoy(num_elem) << "  " << vitesse(num_elem) << finl;

      fic_f << tps << " " << mbf2 << " "
            << U_RANS(num_elem) << " " << umoy(num_elem) << " "
            << utemp_sum(num_elem) << " " << dt <<finl;
    }//fin if f_start


  return resu;
}

DoubleTab& Terme_Source_Canal_RANS_LES_VDF_Elem::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}
