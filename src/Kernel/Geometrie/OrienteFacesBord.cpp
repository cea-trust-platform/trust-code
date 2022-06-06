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

#include <OrienteFacesBord.h>
#include <Motcle.h>

#include <SFichier.h>

Implemente_instanciable(OrienteFacesBord,"OrienteFacesBord",Interprete_geometrique_base);


Sortie& OrienteFacesBord::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& OrienteFacesBord::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

Entree& OrienteFacesBord::interpreter_(Entree& is)
{
  associer_domaine(is);
  oriente_faces_bord(domaine().le_nom());
  return is;
}


void OrienteFacesBord::oriente_faces_bord(const Nom& nom_dom)
{

  Domaine& dom=ref_cast(Domaine, objet(nom_dom));

  Zone& zone=dom.zone(0);
  Nom type=dom.zone(0).type_elem()->que_suis_je();


  double e=0.001;
  IntVect elem(1);
  DoubleTab pos(1,dimension);
  const DoubleTab& coords=dom.coord_sommets();
  int nombre_faces_rayonnantes=zone.nb_bords()+zone.nb_raccords();
  for (int iface=0; iface<nombre_faces_rayonnantes; iface++)
    {
      const IntTab& som=(iface<zone.nb_bords()?zone.bord(iface).faces().les_sommets():zone.raccord(iface-zone.nb_bords()).valeur().faces().les_sommets());
      int nb_faces=som.dimension(0);
      IntTab new_faces(nb_faces,som.dimension(1));

      for (int j=0; j<nb_faces; j++)
        {
          if (dimension==2)
            {
              if (axi)
                {
                  double r0,t0,r1,t1;
                  r0=coords(som(j,0),0);
                  t0=coords(som(j,0),1);
                  r1=coords(som(j,1),0);
                  t1=coords(som(j,1),1);
                  if (t1<t0) t1+=2*M_PI;
                  if (est_egal(r0,r1))
                    {
                      pos(0,1)=0.5*(t0+t1);
                      if (t0<t1)
                        pos(0,0)=(1-e)*r0;
                      else
                        pos(0,0)=(1+e)*r0;
                    }
                  else if (est_egal(t0,t1))
                    {
                      pos(0,0)=0.5*(r0+r1);
                      if (r0<r1)
                        pos(0,1)=(1+e)*t0;
                      else
                        pos(0,1)=(1-e)*t0;
                    }
                }
              else
                {
                  double x0,y0,x1,y1;
                  x0=coords(som(j,0),0);
                  y0=coords(som(j,0),1);
                  x1=coords(som(j,1),0);
                  y1=coords(som(j,1),1);
                  double nx=y0-y1;
                  double ny=x1-x0;
                  double alpha=10*Objet_U::precision_geom/(sqrt(nx*nx+ny*ny));
                  pos(0,0)=0.5*(x0+x1)+alpha*nx;
                  pos(0,1)=0.5*(y0+y1)+alpha*ny;
                }
              dom.zone(0).chercher_elements(pos,elem);
              if (elem(0)!=-1)
                {
                  new_faces(j,0) = som(j,0);
                  new_faces(j,1) = som(j,1);
                }
              else
                {
                  new_faces(j,0) = som(j,1);
                  new_faces(j,1) = som(j,0);
                }
            }
          else
            {
              if (axi)
                {
                  double r0,t0,z0,r3,t3,z3;
                  r0=coords(som(j,0),0);
                  t0=coords(som(j,0),1);
                  z0=coords(som(j,0),2);
                  r3=coords(som(j,3),0);
                  t3=coords(som(j,3),1);
                  z3=coords(som(j,3),2);
                  // Cas ou on repasse a teta=0;
                  if (t3<t0) t3+=2*M_PI;

                  if (est_egal(r0,r3))
                    {
                      pos(0,1)=0.5*(t0+t3);
                      if (t0<t3)
                        pos(0,0)=(1+e)*r0;
                      else
                        pos(0,0)=(1-e)*r0;
                      pos(0,2)=0.5*(z0+z3);
                    }
                  else if (est_egal(z0,z3))
                    {
                      pos(0,0)=0.5*(r0+r3);
                      pos(0,1)=0.5*(t0+t3);
                      if (r0<r3)
                        pos(0,2)=(1+e)*z0;
                      else
                        pos(0,2)=(1-e)*z0;
                    }
                  else if (est_egal(t0,t3))
                    {
                      pos(0,0)=0.5*(r0+r3);
                      pos(0,2)=0.5*(z0+z3);
                      if (r0<r3)
                        pos(0,1)=(1+e)*t0;
                      else
                        pos(0,1)=(1-e)*t0;
                    }
                  dom.zone(0).chercher_elements(pos,elem);
                  if (elem(0)!=-1)
                    {
                      new_faces(j,0) = som(j,0);
                      new_faces(j,1) = som(j,2);
                      new_faces(j,2) = som(j,3);
                      new_faces(j,3) = som(j,1);
                    }
                  else
                    {
                      new_faces(j,0) = som(j,0);
                      new_faces(j,1) = som(j,1);
                      new_faces(j,2) = som(j,3);
                      new_faces(j,3) = som(j,2);
                    }
                }
              else
                {
                  double x0,y0,z0,x1,y1,z1,x2,y2,z2;
                  x0=coords(som(j,0),0);
                  y0=coords(som(j,0),1);
                  z0=coords(som(j,0),2);
                  x1=coords(som(j,1),0);
                  y1=coords(som(j,1),1);
                  z1=coords(som(j,1),2);
                  x2=coords(som(j,2),0);
                  y2=coords(som(j,2),1);
                  z2=coords(som(j,2),2);
                  double nx=(y1-y0)*(z2-z0)-(z1-z0)*(y2-y0);
                  double ny=(z1-z0)*(x2-x0)-(x1-x0)*(z2-z0);
                  double nz=(x1-x0)*(y2-y0)-(y1-y0)*(x2-x0);
                  double alpha=100*Objet_U::precision_geom/(sqrt(nx*nx+ny*ny+nz*nz));
                  pos(0,0)=(x0+x1+x2)/3.+alpha*nx;
                  pos(0,1)=(y0+y1+y2)/3.+alpha*ny;
                  pos(0,2)=(z0+z1+z2)/3.+alpha*nz;
                  dom.zone(0).chercher_elements(pos,elem);
                  /*
                    if (x0>0 && y0>0 && x1>0 && y1>0 && x2>0 && y2>0)
                    {
                    Cout << "nx = " << nx << " " << ny << " " << nz << finl;
                    Cout << "pos = " << pos << finl;
                    Cout << elem(0) << finl;
                    } */
                  if (elem(0)!=-1)
                    {
                      //Cout << "non inversion" << finl;
                      if (type=="Tetraedre")
                        {
                          new_faces(j,0) = som(j,0);
                          new_faces(j,1) = som(j,1);
                          new_faces(j,2) = som(j,2);

                        }
                      else
                        {
                          new_faces(j,0) = som(j,0);
                          new_faces(j,1) = som(j,1);
                          new_faces(j,2) = som(j,3);
                          new_faces(j,3) = som(j,2);
                        }
                    }
                  else
                    {
                      //Cout << "inversion" << finl;
                      if (type=="Tetraedre")
                        {
                          new_faces(j,0) = som(j,0);
                          new_faces(j,1) = som(j,2);
                          new_faces(j,2) = som(j,1);
                        }
                      else
                        {
                          new_faces(j,0) = som(j,0);
                          new_faces(j,1) = som(j,2);
                          new_faces(j,2) = som(j,3);
                          new_faces(j,3) = som(j,1);
                        }
                    }
                  if (elem(0)!=-1)
                    {
                      int el=elem(0);
                      pos(0,0)=(x0+x1+x2)/3.-alpha*nx;
                      pos(0,1)=(y0+y1+y2)/3.-alpha*ny;
                      pos(0,2)=(z0+z1+z2)/3.-alpha*nz;
                      dom.zone(0).chercher_elements(pos,elem);
                      if (elem(0)!=-1)
                        {
                          Cerr << "Case not implemented in the OrienteFacesBord algorithm! " << finl;
                          Cerr << "The face " << j << " is surrounded by the two elements " << el << " " << elem(0) << finl;
                          Cerr << alpha*nx << finl;
                          Cerr << alpha*ny << finl;
                          Cerr << alpha*nz << finl;
                          Cerr << "Contact TRUST support." <<finl;
                          exit();
                        }
                    }
                }
            }
        }
      if (iface<zone.nb_bords())
        zone.bord(iface).faces().les_sommets().ref(new_faces);
      else
        zone.raccord(iface-zone.nb_bords()).valeur().faces().les_sommets().ref(new_faces);
    }

}

