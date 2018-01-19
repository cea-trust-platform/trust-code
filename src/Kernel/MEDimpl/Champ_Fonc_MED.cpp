/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Champ_Fonc_MED.cpp
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_MED.h>
#include <LireMED.h>
#include <Char_ptr.h>


Implemente_instanciable_sans_constructeur(Champ_Fonc_MED,"Champ_Fonc_MED",Champ_Fonc_base);

Champ_Fonc_MED::Champ_Fonc_MED():Champ_Fonc_base::Champ_Fonc_base()
{
  last_time_only_=0;
}

//     printOn()
/////

Sortie& Champ_Fonc_MED::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Champ_Fonc_MED::readOn(Entree& s)
{
  Nom nom_fic2,nom_dom,nom_champ;
  Motcle localisation;
  double un_temps;
  s>>nom_fic2;
  int use_existing_domain=0;
  last_time_only_=0;
  if (nom_fic2=="use_existing_domain")
    {
      use_existing_domain=1;
      s>>nom_fic2;
    }
  if (nom_fic2=="last_time")
    {
      last_time_only_=1;
      s>>nom_fic2;
    }

  lire_nom_med(nom_dom,s);
  lire_nom_med(nom_champ,s);


  s>>localisation;
  s>>un_temps;
  traite_nom_fichier_med(nom_fic2);
#ifdef MED_
  test_version(nom_fic2);
#endif

  // Ajout d'un test bloquant pour empecher la lecture de fichiers 000n.med car Champ_Fonc_MED ne marche
  // pas encore voir http://dibona.intra.cea.fr:8090/trio_u/issue2605
  Nom tmp(nom_fic2);
  tmp.prefix("0001.med");
  if (tmp!=nom_fic2 && Process::nproc()>1)
    {
      Cerr << "Error, Champ_Fonc_MED can't read, for the moment, partitioned MED files." << finl;
      Cerr << "Try to build a single MED file by:" << finl;
      Cerr << "A) Using LATA format for the previous calculation." << finl;
      Cerr << "B) Convert the LATA files into a single MED file with Lata_to_MED keyword." << finl;
      Cerr << "C) Use this single MED file with Champ_Fonc_MED keyword." << finl;
      exit();
    }
  nommer(nom_champ);

  bool domain_exist=((interprete().objet_existant(nom_dom))&& ( sub_type(Domaine, interprete().objet(nom_dom))));
  if (( use_existing_domain) && (!domain_exist))
    {
      Cerr<<" The domain "<< nom_dom << "doesnt exist !!!! in Champ_Fonc_MED "<< nom_fic2<<" "<<nom_dom<<" "<<nom_champ<<" "<<localisation<<" "<<un_temps <<finl;
      exit();
    }
  if (domain_exist&& use_existing_domain )
    {
      Cerr<<nom_dom<<" was not read into the med file because it already exists" <<finl;

      Domaine& un_dom=ref_cast(Domaine, interprete().objet(nom_dom));
#ifndef NDEBUG
      // on va verifier que l'on a le meme
      if (1)
        {
          LireMED liremed;
          dom.nommer(nom_dom);
          // ne pas initialiser le nom du domaine va conduire a ne pas creer les fichiers sous zones
          Nom nom_dom_trio_non_nomme;
          liremed.lire_geom(nom_fic2,dom,nom_dom,nom_dom_trio_non_nomme);
          dom.les_sommets().set_md_vector(un_dom.les_sommets().get_md_vector());
          dom.les_sommets()-=un_dom.les_sommets();
          dom.les_sommets().set_md_vector(MD_Vector());
          dom.zone(0).les_elems().set_md_vector(un_dom.zone(0).les_elems().get_md_vector());
          dom.zone(0).les_elems()-=un_dom.zone(0).les_elems();
          dom.zone(0).les_elems().set_md_vector(MD_Vector());
          if ((max_abs_array(dom.les_sommets())>1e-5)||(max_abs_array(dom.zone(0).les_elems())>0))
            {
              Cerr<<"Error the domain in the file "<<nom_fic2<<" and domain "<<nom_dom<<" are not the same (coords,conn)"<<finl;
              exit();
            }
          //
          dom=Domaine();
        }
#endif
      creer(nom_fic2,un_dom,localisation,temps_sauv_);
      //exit();
    }
  else
    {
      if (domain_exist)
        {
          Cerr<<"INFO: You can use Champ_fonc_med use_existing_domain "<< nom_fic2<<" "<<nom_dom<<" "<<nom_champ<<" "<<localisation<<" "<<un_temps<<" to optimize"<<finl;
          //  exit();
        }
      LireMED liremed;
      dom.nommer(nom_dom);
      // ne pas initialiser le nom du domaine va conduire a ne pas creer les fichiers sous zones
      Nom nom_dom_trio_non_nomme;
      liremed.lire_geom(nom_fic2,dom,nom_dom,nom_dom_trio_non_nomme);
      // MODIF ELI LAUCOIN (06/03/2012) :
      // j'ajoute l'attribut temps_sauv_
      creer(nom_fic2,dom,localisation,temps_sauv_);
    }
  if (last_time_only_)
    {
      un_temps=temps_sauv_(temps_sauv_.size_array()-1);
      Cout << "The resumption time is "<<un_temps<<finl;
    }
  // FIN MODIF ELI LAUCOIN (06/03/2012)
  le_champ().nommer(nom_champ);
  le_champ().corriger_unite_nom_compo();
  mettre_a_jour(un_temps);

  return s ;
}

void Champ_Fonc_MED::mettre_a_jour(double t)
{
  double t2=-1;
  //  Cerr<<" Champ_Fonc_MED::mettre_a_jour("<<t<<" "<<-t-1<<finl;
  if (t<0) lire(t2,-(int)t-1);
  else
    lire(t);
  //  assert(0);exit();
  // Champ_Fonc_base::mettre_a_jour(t);
}
void  Champ_Fonc_MED::lire(double t,int nn)
{
#ifdef MED_
  int fid=MEDfileOpen(nom_fic,MED_ACC_RDONLY);
  if (fid<0)
    {
      Cerr <<"Problem while opening the file "<<nom_fic<<finl;
      exit();
    }


  Char_ptr pflname;
  pflname.allocate(MED_NAME_SIZE);
  if (nn!=-1)
    {
      //  Cerr<<"on lit le "<<nn+1<< " pas de temps du champ "<<le_nom()<<finl;
    }

  Char_ptr dt_unit;
  dt_unit.allocate(MED_SNAME_SIZE);
  // on cherche le numero du pas de temps
  med_int numdt,numo;
  double dt=-2;
  int ret;
  Char_ptr maa_ass;
  maa_ass.allocate(MED_NAME_SIZE);
  //Cerr<<"nb_dt"<<nb_dt<<finl;
  Nom nom_dom(mon_dom.le_nom());
  int i2=0;
  Nom& le_nom_du_champ=nom_champ_dans_fichier_med_;
#ifdef MED30
  med_field_type typcha;
  //  Nom meshname; dimensionne_nom_taille(meshname,MED_NAME_SIZE);
  Char_ptr dtunit;
  dtunit.allocate(MED_SNAME_SIZE);
  med_bool localmesh;
  med_int nbofcstp;
  int ncomp = MEDfieldnComponentByName(fid,le_nom_du_champ);
  Char_ptr comp;
  comp.allocate(MED_SNAME_SIZE*ncomp);
  Char_ptr unit;
  unit.allocate(MED_SNAME_SIZE*ncomp);
  ret=MEDfieldInfoByName(fid,le_nom_du_champ,maa_ass,&localmesh,&typcha,comp,unit,dtunit,&nbofcstp);
  int nb_dt_tot=nbofcstp;
#else
  int nb_dt_tot=MEDnPasdetemps(fid,le_nom_du_champ,type_ent,type_geo);
#endif
  double tmax=-1;
  for (int j=0; j<nb_dt_tot; j++)
    {
#ifdef MED30
      med_int meshnumdt,meshnumit;
      MEDfieldComputingStepMeshInfo(fid,le_nom_du_champ,j+1,&numdt,&numo,&dt,&meshnumdt,&meshnumit);
#else
      med_int ngauss;
      MEDpasdetempsInfo(fid,le_nom_du_champ,type_ent,type_geo,
                        j+1, maa_ass,&ngauss, &numdt,dt_unit, &dt, &numo);
#endif
      tmax=dt;
    }
  for (int j=0; j<nb_dt_tot; j++)
    {
      if (i2>=nb_dt) exit();
      //Cerr<<"ici "<<j<<finl;

#ifdef MED30
      med_int meshnumdt,meshnumit;
      MEDfieldComputingStepMeshInfo(fid,le_nom_du_champ,j+1,&numdt,&numo,&dt,&meshnumdt,&meshnumit);
#else
      med_int ngauss;
      MEDpasdetempsInfo(fid,le_nom_du_champ,type_ent,type_geo,
                        j+1, maa_ass,&ngauss, &numdt,dt_unit, &dt, &numo);
#endif
      Nom Nmaa_ass(maa_ass);
      //Cerr<<" dt "<<dt<<" "<<t <<finl;
      //Cerr<<dt_unit<<finl;
      //Cerr<<j<<" la "<<nom_dom<<Nmaa_ass<<" numdt"<<numdt<<finl;

      if (est_egal(dt,t))
        {
          //if (nom_dom!=Nmaa_ass) { Cerr<<"pb de coherence noms dom"<<finl;exit();}
#ifdef MED30
          int size=MEDfieldnValue(fid,le_nom_du_champ,numdt,numo,type_ent,type_geo);
#else
          int size=1;
#endif
          if ((size!=0)&&(nom_dom==Nmaa_ass) )
            break;
        }
      if (Nmaa_ass==nom_dom)
        {
          if (nn==i2)
            {
              t=dt;
              nn=j;
              break;
            }
          i2++;
        }
    }
  //dt=t;
  //numdt=1;
  //numo=MED_NONOR;
  //  Cerr<<"dt - t"<<dt -t <<finl;
  //Cerr<<ret<<" ici "<<nn<<finl;
  if (((nb_dt==1)&&(!est_egal(dt,t))) || ((last_time_only_==1)&& (!est_egal(tmax,t))))
    {
      Cout<< " We assume that the field "<< le_nom_du_champ<<" is stationary" <<finl;
      // exit();
    }
  else
    {
      if (!est_egal(dt,t))
        {
          Cerr.precision(12);
          Cerr<<" we did not find the time " <<t<<finl;
          Cerr<<"time available for "<<le_nom_du_champ<<" dom "<<nom_dom;
          for (int j=0; j<nb_dt; j++)
            {
#ifdef MED30
              med_int meshnumdt,meshnumit;
              MEDfieldComputingStepMeshInfo(fid,le_nom_du_champ,j+1,&numdt,&numo,&dt,&meshnumdt,&meshnumit);
#else
              int ngauss;
              MEDpasdetempsInfo(fid,le_nom_du_champ,type_ent,type_geo,
                                j+1, nom_dom, &ngauss, &numdt,dt_unit, &dt, &numo);
#endif
              Cerr<<" "<<dt<<finl;
            }
          exit();
        }
      //Cerr<<"la"<<finl;
      Cerr<<"reading of the "<<(int)numdt<< " time step of field "<<le_nom_du_champ<<" time "<<t<<" type ent "<<(int)type_ent<<finl;
#ifdef MED30
      ret=MEDfieldValueRd(fid,le_nom_du_champ,numdt,numo,type_ent,type_geo,MED_FULL_INTERLACE,MED_ALL_CONSTITUENT,(unsigned char*)le_champ().valeurs().addr());
#else
      ret=MEDchampLire(fid,nom_dom,le_nom_du_champ,(unsigned char*)le_champ().valeurs().addr(),MED_FULL_INTERLACE,MED_ALL,pflname,type_ent,type_geo,numdt,numo);
#endif
      //Cerr<<"valeurs"<<valeurs_<<finl;
      if (ret<0)
        {
          Cerr<<"Problem while reading field "<<le_nom_du_champ<<" name of domain "<< mon_dom.le_nom()<<finl;
          exit();
        }
      MEDfileClose(fid);
    }
  //  Cerr<<valeurs()<<finl;
  Champ_Fonc_base::mettre_a_jour(t);
  le_champ().Champ_Fonc_base::mettre_a_jour(t);
#else
  med_non_installe();
#endif
}
const Domaine& Champ_Fonc_MED::domaine() const
{
  return mon_dom;
}

#ifdef MED_
med_geometry_type type_geo_trio_to_type_med(const Nom& type);
#endif

void Champ_Fonc_MED::creer(const Nom& nomfic,const Domaine& un_dom,const Motcle& localisation,ArrOfDouble& temps_sauv)
{
#ifdef MED_

  mon_dom=un_dom;
  nom_fic=nomfic;
  int nbcomp,size;
  //med_entity_type type_ent;
  //med_geometry_type type_geo;
  const Nom& type_elem=un_dom.zone(0).type_elem()->que_suis_je();
  type_geo=type_geo_trio_to_type_med(type_elem);
  int verifie_type=0;
  if (localisation!=Nom())
    {
      verifie_type=1;
      if (localisation=="som")
        type_ent=MED_NODE;
      else  if (localisation=="elem")
        type_ent=MED_CELL;
      else
        {
          Cerr<<localisation<<"localisation not understood "<<finl;
          exit();
        }
    }
  //ArrOfDouble temps_sauv;
  nom_champ_dans_fichier_med_=le_nom();
  nom_champ_dans_fichier_med_=medinfo1champ(nomfic,nom_champ_dans_fichier_med_,numero_ch,nbcomp,nb_dt,type_ent,type_geo,size,un_dom.le_nom(),verifie_type,temps_sauv);
  Nom type_champ;
  if (type_ent==MED_NODE)
    {
      if ((type_geo==MED_QUAD4)||(type_geo==MED_HEXA8))
        type_champ = "Champ_Fonc_Q1_MED";
      else
        type_champ = "Champ_Fonc_P1_MED";
    }
  else if (type_ent==MED_CELL)
    {
      type_champ = "Champ_Fonc_P0_MED";
    }
  vrai_champ_.typer(type_champ);
  fixer_nb_comp(nbcomp);
  le_champ().fixer_nb_comp(nbcomp);
  zonebidon_inst.associer_zone(un_dom.zone(0));
  le_champ().associer_zone_dis_base(zonebidon_inst);
  le_champ().fixer_nb_valeurs_nodales(size);
  if (size != le_champ().valeurs().dimension(0))
    {
      Cerr << "Error in Champ_Fonc_MED: nb_ddl incorrect" << finl;
      exit();
    }
  //pour forcer la lecture lors du mettre a jour
  changer_temps(-1e3);

  //corriger_unite_nom_compo();
  le_champ().nommer(le_nom());
  //  le_champ().corriger_unite_nom_compo();
#else
  med_non_installe();
#endif
}

const Zone_dis_base& Champ_Fonc_MED::zone_dis_base() const
{
  return zonebidon_inst;
  Cerr<<"Champ_Fonc_MED::zone_dis_base not coded "<<finl;

  assert(0);
  exit();

}

const ArrOfDouble& Champ_Fonc_MED::get_saved_times(void) const
{
  return temps_sauv_;
}
