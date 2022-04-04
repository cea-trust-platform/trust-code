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
// File:        Terme_Source_Rappel_T_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Sources
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_Rappel_T_VEF_Face.h>
#include <Champ_Uniforme.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Schema_Temps_base.h>
#include <EFichier.h>
#include <Interprete.h>
#include <TRUSTTab.h>

Implemente_instanciable(Terme_Source_Rappel_T_VEF_Face,"Source_Rappel_T_VEF_P1NC",Source_base);



//// printOn
//

Sortie& Terme_Source_Rappel_T_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Terme_Source_Rappel_T_VEF_Face::readOn(Entree& is )
{
  Motcle mot_lu;
  Motcle acc_ouverte("{");
  Motcle acc_fermee("}");



  alpha_tau=1.;
  fct_ok=-1;

  Motcles les_mots(3);
  {
    les_mots[0]="alpha_tau"; //coeff de relaxation du terme source
    les_mots[1]="force_rappel"; // Expression de la force de rappel
    les_mots[2]="probleme"; // Expression de la force de rappel
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
        case 0:
          is >> alpha_tau;
          break;
        case 1:
          {
            Nom tmp;
            is >> tmp;
            force_rappel.setNbVar(dimension);
            force_rappel.setString(tmp);
            force_rappel.addVar("x");
            force_rappel.addVar("y");
            if (dimension>2)
              force_rappel.addVar("z");
            force_rappel.parseString();
            fct_ok=1;
            break;
          }
        case 2:
          fct_ok=0;
          is >> nom_autre_pb >> nom_inco;
          break;
        default :
          {
            Cerr << mot_lu << " n'est pas un mot compris par Source_Canal_RANS_LES_VEF_P1NC" << finl;
            Cerr << "Les mots compris sont : " << les_mots << finl;
            exit();
          }
        }
      is >> mot_lu;
    }

  return is;

}

void Terme_Source_Rappel_T_VEF_Face::associer_zones(const Zone_dis& zone_dis,
                                                    const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VEF = ref_cast(Zone_VEF, zone_dis.valeur());
  la_zone_Cl_VEF = ref_cast(Zone_Cl_VEF, zone_Cl_dis.valeur());
}

void Terme_Source_Rappel_T_VEF_Face::associer_pb(const Probleme_base& pb)
{
  ;
}
void Terme_Source_Rappel_T_VEF_Face::completer()
{
  Source_base::completer();
  if (fct_ok == 0)
    {
      const Zone_VEF& zone_VEF = ref_cast(Zone_VEF,equation().zone_dis().valeur());
      const Zone& zone = zone_VEF.zone();
      const int nb_elem = zone_VEF.nb_elem();

      Objet_U& ob1=Interprete::objet(nom_autre_pb);
      REF(Probleme_base) pb;
      if( sub_type(Probleme_base, ob1) )
        pb = ref_cast(Probleme_base, ob1);
      else
        {
          Cerr << "On ne trouve pas de probleme de nom " << nom_autre_pb << finl;
          exit();
        }

      REF(Champ_base) rch;
      rch = pb->get_champ(nom_inco);
      l_inconnue=ref_cast(Champ_Inc_base, rch.valeur()) ;

      zone_VEF_autre_pb = ref_cast(Zone_VEF,pb->domaine_dis().zone_dis(0).valeur());
      const int nb_elem_autre_pb = zone_VEF_autre_pb->nb_elem();
      const DoubleTab& xp_autre_pb = zone_VEF_autre_pb->xp() ;
      const DoubleVect& volumes = zone_VEF_autre_pb->volumes();

      // Dimensionnement des tabs
      nb_fin.resize_array(nb_elem);
      corresp_fin_gros.resize_array(nb_elem_autre_pb);
      nb_fin=0;
      corresp_fin_gros=0;
      vol_fin_tot.resize_array(nb_elem);
      vol_fin_tot=0.;

      // Calcul des connectivites entre les maillages des deux pbs
      zone.chercher_elements(xp_autre_pb, corresp_fin_gros);
      for (int i=0; i<nb_elem_autre_pb; i++)
        {
          assert(corresp_fin_gros(i)>0);
          nb_fin[corresp_fin_gros[i]]++;
          vol_fin_tot[corresp_fin_gros[i]] += volumes(i);
        }
    }

}



DoubleTab& Terme_Source_Rappel_T_VEF_Face::ajouter(DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const int nb_faces = zone_VEF.nb_faces();
  const int premiere_face_std=zone_VEF.premiere_face_std();
  const int ndeb = zone_VEF.premiere_face_int();

  const int nb_elem = zone_VEF.nb_elem();
  const DoubleTab& temperature = mon_equation->inconnue().valeurs();
  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();
  const DoubleVect& volumes_entrelaces_Cl = la_zone_Cl_VEF->volumes_entrelaces_Cl();
  const DoubleTab& xv=zone_VEF.xv() ;
  const double dt = mon_equation->schema_temps().pas_de_temps();

  const DoubleVect& volumes_autre_pb = zone_VEF_autre_pb->volumes();

  double vol;
  double force=0.;

  if (fct_ok == 1)
    {
      for(int num_face = 0 ; num_face<premiere_face_std ; num_face++)
        {
          vol = volumes_entrelaces_Cl(num_face);

          for (int i=0; i<dimension; i++)
            force_rappel.setVar(i,xv(num_face,i));

          force = force_rappel.eval();
          resu(num_face) += (force-temperature(num_face))/(alpha_tau*dt)*vol;
        }

      for(int num_face = 0 ; num_face<nb_faces ; num_face++)
        {
          vol = volumes_entrelaces(num_face);

          for (int i=0; i<dimension; i++)
            force_rappel.setVar(i,xv(num_face,i));

          force = force_rappel.eval();
          resu(num_face) += (force-temperature(num_face))/(alpha_tau*dt)*vol;
        }
    }
  else
    {
      const Zone_VEF& zoneVEF_autre_pb = ref_cast(Zone_VEF,l_inconnue->zone_dis_base());
      const int nb_elem_autre_pb = zoneVEF_autre_pb.nb_elem();
      const DoubleTab& xp=zoneVEF_autre_pb.xp() ;

      const IntTab& face_voisins=zone_VEF.face_voisins() ;
      ArrOfDouble val_elem(nb_elem); // valeurs sur les elems du pb courant du champ de l'autre pb.

      DoubleTab valeurs(nb_elem_autre_pb,l_inconnue->nb_comp()); //valeurs sur l'autre pb
      IntTab elems(nb_elem_autre_pb);
      for (int i=0; i<nb_elem_autre_pb; i++)
        elems(i)=i;
      l_inconnue->valeur_aux_elems(xp, elems, valeurs);

      val_elem=0.;
      for (int i=0; i<nb_elem_autre_pb; i++)
        {
          val_elem[corresp_fin_gros[i]] += valeurs(i,0)*volumes_autre_pb(i)/vol_fin_tot[corresp_fin_gros[i]] ; ///nb_fin(corresp_fin_gros(i));
        }


      for(int num_face = 0 ; num_face<ndeb ; num_face++)
        {
          vol = volumes_entrelaces_Cl(num_face);

          force = val_elem[face_voisins(num_face,0)];
          resu(num_face) += (force-temperature(num_face))/(alpha_tau*dt)*vol;
        }
      for(int num_face = ndeb ; num_face<premiere_face_std ; num_face++)
        {
          vol = volumes_entrelaces_Cl(num_face);

          force = 0.5*(val_elem[face_voisins(num_face,0)]+val_elem[face_voisins(num_face,1)]);
          if ((force!=0.))//&& (force-temperature(num_face)>0.))
            resu(num_face) += (force-temperature(num_face))/(alpha_tau*dt)*vol;
        }

      for(int num_face = premiere_face_std ; num_face<nb_faces ; num_face++)
        {
          vol = volumes_entrelaces(num_face);

          if ((val_elem[face_voisins(num_face,0)] !=0.) && (val_elem[face_voisins(num_face,1)] !=0.))
            force = 0.5*(val_elem[face_voisins(num_face,0)]+val_elem[face_voisins(num_face,1)]);
          else
            force = val_elem[face_voisins(num_face,0)]+val_elem[face_voisins(num_face,1)];
          if ((force!=0.))//&& (force-temperature(num_face)>0.))
            {
              //Cout << "force = " << force << finl;
              resu(num_face) += (force-temperature(num_face))/(alpha_tau*dt)*vol;
            }
          //Cout << "force-temperature(num_face) = " << force-temperature(num_face) << finl;
        }



    }


  return resu;
}

DoubleTab& Terme_Source_Rappel_T_VEF_Face::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}

