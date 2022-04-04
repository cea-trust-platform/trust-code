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
// File:        Raccord_distant_homogene.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Raccord_distant_homogene.h>
#include <TRUSTTab.h>
#include <Schema_Comm.h>
#include <ArrOfBit.h>
#include <Zone_dis_base.h>
#include <Zone_VF.h>
#include <TRUSTTabs.h>
#include <Domaine.h>
#include <communications.h>

#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDCouplingMemArray.hxx>

using namespace MEDCoupling;
#endif

Implemente_instanciable_sans_constructeur(Raccord_distant_homogene,"Raccord_distant_homogene",Raccord_distant);

// Description:
//    Simple appel a: Raccord_distant::printOn(Sortie& )
//    (+ finl)
// Precondition:
// Parametre: Sortie& s
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
Sortie& Raccord_distant_homogene::printOn(Sortie& s ) const
{
  Raccord_distant::printOn(s) << finl;
  s << (int)1 <<finl;
  s << tab_envoi << finl;
  s << tab_recep << finl;
  s << nom_frontiere_voisine_ << finl;
  s << est_initialise_ << finl;
  s << e_ << finl;
  return s;
}




// Description:
//    Simple appel a: Raccord_distant::readOn(Entree& )
// Precondition:
// Parametre: Entree& s
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
Entree& Raccord_distant_homogene::readOn(Entree& s)
{
  Raccord_distant::readOn(s) ;
  int flag;
  s >> flag;
  if(flag==1)
    {
      s >> tab_envoi;
      s >> tab_recep;
      s >> nom_frontiere_voisine_;
      s >> est_initialise_;
      s >> e_;
    }
  return s;
}

// Description:
//    Constructeur
// Precondition:
// Parametre: Aucun
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Aucun
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Raccord_distant_homogene::Raccord_distant_homogene() : est_initialise_(0),e_(0)
{}

// Description:
// Retourne dans le DoubleTab x la trace sur le raccord distant du DoubleTab y localise aux elements du domaine distant
void Raccord_distant_homogene::trace_elem_distant(const DoubleTab& y, DoubleTab& x) const
{
  assert(est_initialise());
  const int nb_compo_= y.line_size();

  const IntTab& send_data = Tab_Envoi();
  const ArrOfInt& recv_data = Tab_Recep();
  const Faces& lesfaces = faces();
  const int n1 = send_data.dimension(0);
  const int n2 = recv_data.size_array();

  // On dimensionne x si ce n'est pas fait
  if (x.size_array()==0 && n2!=0)
    x.resize(n2, nb_compo_);
  else if (x.dimension(0) != n2)
    {
      Cerr << "Call to Frontiere::trace_elem with a DoubleTab x not located on boundary faces." << finl;
      Process::exit();
    }
  Schema_Comm schema;
  schema.set_send_recv_pe_list(send_pe_list_, recv_pe_list_);
  schema.begin_comm();
  for (int i = 0; i < n1; i++)
    {
      const int pe_dest = send_data(i, 0);
      const int face  = send_data(i, 1);
      int item = lesfaces.voisin(face,0);
      if (item == -1)
        item = lesfaces.voisin(face,1);

      for (int j=0; j<nb_compo_; j++)
        schema.send_buffer(pe_dest) << y(item,j);
    }
  schema.echange_taille_et_messages();

  for (int i = 0; i < n2; i++)
    {
      const int pe_source = recv_data[i];
      for(int j=0; j<nb_compo_; j++)
        schema.recv_buffer(pe_source) >> x(i,j);
    }
  schema.end_comm();
}

// Description:
// Retourne dans le DoubleTab x la trace sur le raccord distant du DoubleTab y localise aux faces
void Raccord_distant_homogene::trace_face_distant(const DoubleTab& y, DoubleTab& x) const
{
  assert(est_initialise());
  int n, N = y.line_size();

  const IntTab& send_data = Tab_Envoi();
  const ArrOfInt& recv_data = Tab_Recep();
  const int n1 = send_data.dimension(0);
  const int n2 = recv_data.size_array();

  // On dimensionne x si ce n'est pas fait
  if (x.size_array()==0 && n2!=0)
    {
      if (y.nb_dim() == 2) x.resize(n2, y.dimension(1));
      else if (y.nb_dim() == 3) x.resize(n2, y.dimension(1), y.dimension(2));
      else if (y.nb_dim() == 4) x.resize(n2, y.dimension(1), y.dimension(2), y.dimension(3));
    }

  Schema_Comm schema;
  schema.set_send_recv_pe_list(send_pe_list_, recv_pe_list_);
  schema.begin_comm();
  for (int i = 0; i < n1; i++)
    {
      const int pe_dest = send_data(i, 0);
      const int face  = num_premiere_face() + send_data(i, 1);
      for (n = 0; n < N; n++) schema.send_buffer(pe_dest) << y.addr()[N * face + n];
    }
  schema.echange_taille_et_messages();
  for (int i = 0; i < n2; i++)
    {
      const int pe_source = recv_data[i];
      for (n = 0; n < N; n++) schema.recv_buffer(pe_source) >> x.addr()[N * i + n];
    }
  schema.end_comm();
}

// Description:
// Retourne dans le DoubleVect x la trace sur le raccord distant du DoubleVect y localise aux faces du raccord distant
void Raccord_distant_homogene::trace_face_distant(const DoubleVect& y, DoubleVect& x) const
{
  // Verifie que l'on passe bien un tableau aux faces du raccord
  assert(y.size()==nb_faces());
  assert(est_initialise());
  const IntTab& send_data = Tab_Envoi();
  const ArrOfInt& recv_data = Tab_Recep();
  const int n1 = send_data.dimension(0);
  const int n2 = recv_data.size_array();

  // On dimensionne x si ce n'est pas fait
  if (x.size_array()==0 && n2!=0)
    x.resize(n2);

  Schema_Comm schema;
  schema.set_send_recv_pe_list(send_pe_list_, recv_pe_list_);
  schema.begin_comm();
  for (int i = 0; i < n1; i++)
    {
      const int pe_dest = send_data(i, 0);
      const int face  = send_data(i, 1);
      schema.send_buffer(pe_dest) << y(face);
    }
  schema.echange_taille_et_messages();
  for (int i = 0; i < n2; i++)
    {
      const int pe_source = recv_data[i];
      schema.recv_buffer(pe_source) >> x(i);
    }
  schema.end_comm();
}

void Raccord_distant_homogene::completer()
{
  const IntTab& send_data = Tab_Envoi();
  const ArrOfInt& recv_data = Tab_Recep();
  const int n1 = send_data.dimension(0);
  const int n2 = recv_data.size_array();
  send_pe_list_.set_smart_resize(1);
  recv_pe_list_.set_smart_resize(1);
  const int nbproc = Process::nproc();
  ArrOfBit flags(nbproc);
  flags = 0;
  for (int i = 0; i < n1; i++)
    {
      const int pe = send_data(i, 0);
      if (!flags.testsetbit(pe))
        send_pe_list_.append_array(pe);
    }
  flags = 0;
  for (int i = 0; i < n2; i++)
    {
      const int pe = recv_data[i];
      if (!flags.testsetbit(pe))
        recv_pe_list_.append_array(pe);
    }
  send_pe_list_.ordonne_array();
  recv_pe_list_.ordonne_array();
}

// Description:
// Initialise le raccord distant avec la frontiere et la zone discretisee opposees au raccord distant, et la zone discretisee du raccord distant
void Raccord_distant_homogene::initialise(const Frontiere& opposed_boundary, const Zone_dis_base& opposed_zone_dis, const Zone_dis_base& zone_dis)
{
  Raccord_distant_homogene& raccord_distant = *this;
  if(raccord_distant.est_initialise())
    {
      Cerr << "Remote connection " << raccord_distant.le_nom() << " already initialized." << finl;
      Cerr << "Error, contact TRUST support." << finl;
      exit();
    }
  int dim=Objet_U::dimension;
  raccord_distant.nom_frontiere_voisine()=opposed_boundary.le_nom();
  const Zone_VF& zone_dis_vf = ref_cast(Zone_VF, zone_dis);
  // On va identifier les faces par leur centres de gravite
  int parts = Process::nproc();
  DoubleTabs remote_xv(parts);
  int prem_face2 = raccord_distant.num_premiere_face();
  int nb_face2   = raccord_distant.nb_faces();
  int moi = Process::me();
  remote_xv[moi].resize(nb_face2,dim);
  for (int ind_face=0 ; ind_face < nb_face2 ; ind_face++)
    {
      int face = prem_face2 + ind_face;
      for (int j=0 ; j<dim ; j++)
        remote_xv[moi](ind_face,j) = zone_dis_vf.xv(face,j);
    }

  // Puis on echange les tableaux des centres de gravites
  // envoi des tableaux
  for (int p = 0; p < parts; p++)
    envoyer_broadcast(remote_xv[p], p);

  VECT(ArrOfInt) racc_vois(parts);
  for (int p = 0; p < parts; p++)
    racc_vois[p].set_smart_resize(1);

#ifdef MEDCOUPLING_
  // On traite les informations, chaque proc connait tous les XV
  // Si le proc porte un morceau du raccord_distant
  int prem_face1 = opposed_boundary.num_premiere_face();
  int nb_face1   = opposed_boundary.nb_faces();
  if (nb_face1>0)
    {
      const Zone_VF& opposed_zone_dis_vf = ref_cast(Zone_VF,opposed_zone_dis);
      const DoubleTab& local_xv = opposed_zone_dis_vf.xv();

      ArrOfInt& Recep=raccord_distant.Tab_Recep();
      Recep.resize_array(nb_face1);
      //double tolerance = 1e-8; //not very tolerant, are we?
      double tolerance = Objet_U::precision_geom * 100 ; //default value 1e-8 not very tolerant, are we?

      //DataArrayDoubles des xv locaux et de tous les remote_xv (a la suite)
      std::vector<MCAuto<DataArrayDouble> > vxv(parts);
      std::vector<const DataArrayDouble*> cvxv(parts);
      for (int p = 0; p < parts; p++)
        {
          vxv[p] = DataArrayDouble::New();
          vxv[p]->useExternalArrayWithRWAccess(remote_xv[p].addr(), remote_xv[p].dimension(0), remote_xv[p].dimension(1));
          cvxv[p] = vxv[p];
        }
      MCAuto<DataArrayDouble> remote_xvs(DataArrayDouble::Aggregate(cvxv)), local_xvs(DataArrayDouble::New());
      local_xvs->alloc(nb_face1, dim);
      for (int ind_face = 0; ind_face < nb_face1; ind_face++) for (int j = 0; j < dim; j++)
          local_xvs->setIJ(ind_face, j, local_xv(prem_face1 + ind_face, j));

      //indices des points de remote_xvs les plus proches de chaque point de local_xv
      MCAuto<DataArrayInt> glob_idx(DataArrayInt::New());

      glob_idx = remote_xvs->findClosestTupleId(local_xvs);

      //pour chaque face de local_xv : controle de la tolerance, remplissage de tableau
      for (int ind_face = 0, face1 = prem_face1; ind_face<nb_face1; ind_face++, face1++)
        {
          //retour de l'indice global (glob_idx(ind_face)) au couple (proc, ind_face2)
          int proc = 0, ind_face2 = glob_idx->getIJ(ind_face, 0);
          while (ind_face2 >= remote_xv[proc].dimension(0)) ind_face2 -= remote_xv[proc].dimension(0), proc++;
          assert(ind_face2 < remote_xv[proc].dimension(0));

          //controle de la tolerance
          double distance2 = 0;
          for (int j=0; j<dim; j++)
            {
              double x1=local_xv(face1,j);
              double x2=remote_xv[proc](ind_face2,j);
              distance2 += (x1-x2)*(x1-x2);
            }
          if (distance2 > tolerance * tolerance)
            {
              Cerr << "Warning, there is no remote face found on the local boundary " << opposed_boundary.le_nom() << " for the face local number " << ind_face << " (";
              for (int j=0; j<dim; j++) Cerr << " " << local_xv(face1,j);
              Cerr << " )" << finl << "the nearest face on the remote boundary " << raccord_distant.le_nom() << " is the face " << ind_face2 << " (";
              for (int j=0; j<dim; j++) Cerr << " " << remote_xv[proc](ind_face2,j);
              Cerr << " )" << finl << "which is located at a distance of " << sqrt(distance2) << " and it is above the geometric tolerance " << tolerance << finl;
              Cerr << "Check your mesh or contact TRUST support." << finl;
              Process::exit();
            }

          //remplissage des tableaux
          Recep[ind_face]=proc;
          racc_vois[proc].append_array(ind_face2);
        }
    }
#else
  Cerr<<"Raccord_distant_homogene needs TRUST compiled with MEDCoupling."<<finl;
  exit();
#endif
  VECT(ArrOfInt) facteurs(Process::nproc());
  envoyer_all_to_all(racc_vois, facteurs);

  IntTab& Envoi = raccord_distant.Tab_Envoi();
  if (nb_face2 > 0)
    Envoi.resize(nb_face2, 2);
  int ind = 0;
  for (int p = 0; p < parts; p++)
    {
      const ArrOfInt& facteur = facteurs[p];
      const int sz = facteur.size_array();
      for (int d=0 ; d<sz ; d++)
        {
          Envoi(ind,0) = p;
          Envoi(ind,1) = facteur[d];
          ind++;
        }
    }
  raccord_distant.e() = 0;
  raccord_distant.est_initialise() = 1;
  raccord_distant.completer();
  Cerr <<"Initialize the remote connection " << zone_dis.zone().domaine().le_nom() << "/" << raccord_distant.le_nom() << "<-" << opposed_zone_dis.zone().domaine().le_nom() << "/" << opposed_boundary.le_nom() << finl;
}
