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
// File:        Create_domain_from_sous_zone.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#include <Create_domain_from_sous_zone.h>
#include <Domaine.h>
#include <Param.h>
#include <DomaineCutter.h>
#include <Scatter.h>
#include <Sous_Zone.h>

Implemente_instanciable(Create_domain_from_sous_zone,"Create_domain_from_sous_zone",Interprete_geometrique_base);

Sortie& Create_domain_from_sous_zone::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}
Entree& Create_domain_from_sous_zone::readOn(Entree& is)
{
  return Interprete::readOn(is);
}







Entree& Create_domain_from_sous_zone::interpreter_(Entree& is)
{
  Nom nom_dom,nom_dom_org;
  Nom nom_sous_zone;

  Param param(que_suis_je());
  param.ajouter("domaine_final",&nom_dom,Param::REQUIRED);
  param.ajouter("par_sous_zone",&nom_sous_zone,Param::REQUIRED);
  param.ajouter("domaine_init",&nom_dom_org,Param::REQUIRED);

  param.lire_avec_accolades_depuis(is);



  associer_domaine(nom_dom);
  Domaine& dom=domaine();


  if (nproc()>1)
    {
      Cerr<<"option par_sous_zone of  "<<que_suis_je()<<" is for sequential" <<finl;
      exit();
    }
  const Sous_Zone& ssz=ref_cast(Sous_Zone,objet(nom_sous_zone));
  int nb_poly=ssz.nb_elem_tot();

  const Domaine& domaine_org=ref_cast(Domaine, objet(nom_dom_org));
  IntTab marq_elem(domaine_org.zone(0).nb_elem());
  for (int pol=0; pol<nb_poly; pol++)
    marq_elem(ssz[pol])=1;


  DomaineCutter cutter;
  Noms vide;
  cutter.initialiser(domaine_org,marq_elem,2,1,vide,1);
  cutter.construire_sous_domaine(1,dom);


  Zone& zone=dom.zone(0);
  Bords& bords=zone.faces_bord();

  if (cutter.bords_internes().size()>0)
    zone.faces_joint().vide();
  else
    {
      // on transforme les joints en bord
      Joints& joints= zone.faces_joint();
      for (int j=joints.size()-1; j>=0; j--)
        {
          Cout <<"The joint"<<j<<" becomes a boundary"<<finl;
          Bord b;
          ref_cast(Frontiere,b)=ref_cast(Frontiere,joints(j));
          b.nommer(Nom("Couture_")+Nom(j));
          bords.add(b);

          joints.suppr(joints(j));
        }

    }

  for (int b=bords.size()-1; b>=0; b--)
    if (bords(b).nb_faces()==0)
      {
        Cout << bords(b).le_nom()<<" deleted"<<finl;
        bords.suppr(bords(b));
      }
  Raccords& listrac=zone.faces_raccord();

  for (int b=bords.size()-1; b>=0; b--)
    {
      if (cutter.bords_internes().rang(bords(b).le_nom())>=0)
        {
          Cout << bords(b).le_nom() <<" becomes a connection "<<finl;
          Raccord racc_base;
          racc_base.typer("Raccord_local_homogene");
          Raccord_base& racc=racc_base.valeur();
          // on caste en Frontiere pour pouvoir faire la copie ...
          ref_cast(Frontiere,racc)=ref_cast(Frontiere,bords(b));
          listrac.add(racc_base);

          bords.suppr(bords(b));
        }
    }

  dom.nommer(nom_dom);
  Scatter::init_sequential_domain(dom);

  return is;

}
