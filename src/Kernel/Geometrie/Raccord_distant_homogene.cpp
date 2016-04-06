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
// File:        Raccord_distant_homogene.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Raccord_distant_homogene.h>
#include <DoubleTab.h>
#include <Schema_Comm.h>
#include <ArrOfBit.h>
#include <Zone_dis_base.h>
#include <Zone_VF.h>
#include <DoubleTabs.h>
#include <Domaine.h>
#include <communications.h>

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
void Raccord_distant_homogene::trace_elem(const DoubleTab& y, DoubleTab& x) const
{
  assert(est_initialise());
  int nb_compo_=1;
  if (y.nb_dim() == 2)
    nb_compo_ = y.dimension(1);

  const IntTab& send_data = Tab_Envoi();
  const ArrOfInt& recv_data = Tab_Recep();
  const Faces& lesfaces = faces();
  const int n1 = send_data.dimension(0);
  const int n2 = recv_data.size_array();

  // On dimensionne x si ce n'est pas fait
  if (x.size_array()==0 && n2!=0)
    {
      if (y.nb_dim() == 2)
        x.resize(n2,nb_compo_);
      else
        x.resize(n2);
    }
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

      if(y.nb_dim()==2)
        for (int j=0; j<nb_compo_; j++)
          schema.send_buffer(pe_dest) << y(item,j);
      else
        schema.send_buffer(pe_dest) << y(item);
    }
  schema.echange_taille_et_messages();

  for (int i = 0; i < n2; i++)
    {
      const int pe_source = recv_data[i];
      if (x.nb_dim()==2)
        for(int j=0; j<nb_compo_; j++)
          schema.recv_buffer(pe_source) >> x(i,j);
      else
        schema.recv_buffer(pe_source) >> x(i);
    }
  schema.end_comm();
}

// Description:
// Retourne dans le DoubleTab x la trace sur le raccord distant du DoubleTab y localise aux faces
void Raccord_distant_homogene::trace_face(const DoubleTab& y, DoubleTab& x) const
{
  assert(est_initialise());
  int nb_compo_=1;
  if (y.nb_dim() == 2)
    nb_compo_ = y.dimension(1);

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
      const int face  = num_premiere_face() + send_data(i, 1);
      if(y.nb_dim()==2)
        for (int j=0; j<nb_compo_; j++)
          schema.send_buffer(pe_dest) << y(face,j);
      else
        schema.send_buffer(pe_dest) << y(face);
    }
  schema.echange_taille_et_messages();
  for (int i = 0; i < n2; i++)
    {
      const int pe_source = recv_data[i];
      if (x.nb_dim()==2)
        for(int j=0; j<nb_compo_; j++)
          schema.recv_buffer(pe_source) >> x(i,j);
      else
        schema.recv_buffer(pe_source) >> x(i);
    }
  schema.end_comm();
}

// Description:
// Retourne dans le DoubleVect x la trace sur le raccord distant du DoubleVect y localise aux faces du raccord distant
void Raccord_distant_homogene::trace_face(const DoubleVect& y, DoubleVect& x) const
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
      // Define tolerance (this could be calculated once after discretization to compute PrecisionGeom)
      double tolerance=DMAXFLOAT;
      const DoubleTab& coord=opposed_zone_dis.zone().domaine().coord_sommets();
      const IntTab& sommets=opposed_zone_dis_vf.face_sommets();
      int nb_som_par_face=sommets.dimension(1);
      // Compute smaller distance between two items (face center and vertex)
      for (int ind_face=0; ind_face<nb_face1; ind_face++)
        {
          int face1 = ind_face+prem_face1;
          for (int s=0; s<nb_som_par_face; s++)
            {
              int som = sommets(face1,s);
              double distance=0;
              for (int j=0; j<dim; j++)
                {
                  double x1=local_xv(face1,j);
                  double x2=coord(som,j);
                  distance+=(x1-x2)*(x1-x2);
                }
              distance=sqrt(distance);
              // We divide per 10 to have a tolerance
              double local_tolerance=0.1*distance;
              if (tolerance>local_tolerance) tolerance=local_tolerance;
            }
        }
      // Loop on faces on the local boundary:
      for (int ind_face=0; ind_face<nb_face1; ind_face++)
        {
          int face1 = ind_face+prem_face1;
          int temoin=0;
          int p_minimal=-1;
          int ind_face2_minimal=-1;
          double distance_minimal=DMAXFLOAT;
          // Loop on partition:
          for (int p=0; p<parts; p++)
            {
              int size = remote_xv[p].dimension(0);
              for (int ind_face2=0; ind_face2<size; ind_face2++)
                {
                  int remote_face_found=1;
                  double distance=0;
                  for (int j=0; j<dim; j++)
                    {
                      double x1=local_xv(face1,j);
                      double x2=remote_xv[p](ind_face2,j);
                      distance+=(x1-x2)*(x1-x2);
                    }
                  distance=sqrt(distance);
                  if (distance>tolerance) remote_face_found=0;
                  if (distance<distance_minimal)
                    {
                      distance_minimal=distance;
                      ind_face2_minimal=ind_face2;
                      p_minimal=p;
                    }
                  // Si on a passe le test ci-dessus, c'est que les dim coordonnees de la face
                  // sont les memes (d == dim)
                  if (remote_face_found)
                    {
                      Recep(ind_face)=p;
                      racc_vois[p].append_array(ind_face2);
                      temoin=1;
                      //cerr << "[" << Process::me() << "] Find remote face " << ind_face2 << " for local face ind_face " << ind_face << endl;
                      break;
                    }
                }
            }
          if (temoin==0)
            {
              Cerr << "Warning, there is no remote face found on the local boundary " << opposed_boundary.le_nom() << " for the face local number " << ind_face << " (";
              for (int j=0; j<dim; j++) Cerr << " " << local_xv(face1,j);
              Cerr << " )" << finl << "the nearest face on the remote boundary " << raccord_distant.le_nom() << " is the face " << ind_face2_minimal << " (";
              for (int j=0; j<dim; j++) Cerr << " " << remote_xv[p_minimal](ind_face2_minimal,j);
              Cerr << " )" << finl << "which is located at a distance of " << distance_minimal << " and it is above the geometric tolerance " << tolerance << finl;
              Cerr << "Check your mesh or contact TRUST support." << finl;
              Process::exit();
            }
        }
    }
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
          Envoi(ind,1) = facteur(d);
          ind++;
        }
    }
  raccord_distant.e() = 0;
  raccord_distant.est_initialise() = 1;
  raccord_distant.completer();
  Cerr <<"Initialize the remote connection " << zone_dis.zone().domaine().le_nom() << "/" << raccord_distant.le_nom() << "<-" << opposed_zone_dis.zone().domaine().le_nom() << "/" << opposed_boundary.le_nom() << finl;
}
