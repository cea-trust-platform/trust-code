/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Interpolation_IBM_elem_fluid.h>
#include <TRUSTTrav.h>
#include <Domaine.h>
#include <Param.h>

Implemente_instanciable( Interpolation_IBM_elem_fluid, "Interpolation_IBM_element_fluide|IBM_element_fluide", Interpolation_IBM_base ) ;
// XD interpolation_ibm_elem_fluid interpolation_ibm_base ibm_element_fluide 1 Immersed Boundary Method (IBM): fluid element interpolation.

Sortie& Interpolation_IBM_elem_fluid::printOn( Sortie& os ) const
{
  Interpolation_IBM_base::printOn( os );
  return os;
}

void Interpolation_IBM_elem_fluid::set_param(Param& param)
{
  Interpolation_IBM_base::set_param(param);
  param.ajouter("points_fluides",&fluid_points_lu_,Param::REQUIRED); // XD_ADD_P field_base Node field giving the projection of the point below (points_solides) falling into the pure cell fluid
  param.ajouter("points_solides",&solid_points_lu_,Param::REQUIRED); // XD_ADD_P field_base Node field giving the projection of the node on the immersed boundary
  param.ajouter("elements_fluides",&fluid_elems_lu_,Param::REQUIRED);   // XD_ADD_P field_base Node field giving the number of the element (cell) containing the pure fluid point
  param.ajouter("correspondance_elements",&corresp_elems_lu_,Param::REQUIRED);   // XD_ADD_P field_base Cell field giving the SALOME cell number
}

Entree& Interpolation_IBM_elem_fluid::readOn( Entree& is )
{
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Interpolation_IBM_elem_fluid::discretise(const Discretisation_base& dis, Domaine_dis_base& le_dom_EF)
{
  int nb_comp = Objet_U::dimension;
  Noms units(nb_comp);
  Noms c_nam(nb_comp);

  dis.discretiser_champ("champ_sommets",le_dom_EF,"fluid_elems","none",1,0., fluid_elems_);
  fluid_elems_.valeur().affecter(fluid_elems_lu_);
  if ((&corresp_elems_lu_)->non_nul())
    {
      dis.discretiser_champ("champ_elem",le_dom_EF,"corresp_elems","none",1,0., corresp_elems_);
      corresp_elems_.valeur().affecter(corresp_elems_lu_);
    }
  dis.discretiser_champ("vitesse",le_dom_EF,vectoriel,c_nam,units,nb_comp,0.,fluid_points_);
  fluid_points_.valeur().affecter(fluid_points_lu_);
  dis.discretiser_champ("vitesse",le_dom_EF,vectoriel,c_nam,units,nb_comp,0.,solid_points_);
  solid_points_.valeur().affecter(solid_points_lu_);
  computeFluidElems(le_dom_EF);
}

void Interpolation_IBM_elem_fluid::computeFluidElems(Domaine_dis_base& le_dom_EF)
{
  double eps = 1e-12;
  int nb_som = le_dom_EF.nb_som();
  int nb_som_tot = le_dom_EF.nb_som_tot();
  int nb_elem = le_dom_EF.nb_elem();
  int nb_elem_tot = le_dom_EF.nb_elem_tot();
  const DoubleTab& coordsDom = le_dom_EF.domaine().coord_sommets();
  // const IntTab& elems = le_dom_EF.domaine().les_elems();

  DoubleTab& elems_fluid_ref = fluid_elems_.valeur().valeurs();
  DoubleTab& fluid_points_ref = fluid_points_.valeur().valeurs();
  DoubleTab& solid_points_ref = solid_points_.valeur().valeurs();

  // La numerotation des elements peut avoir changee entre l'etape pre_pro et
  // l'etape calcul (cas du pre_pro Salome)
  // On utilise un champ d'etiquette pour fluid_elems_ (par exemple le no elem Salome)
  // et un champ d'element reprenant ces etiquettes
  if ((&corresp_elems_)->non_nul())
    {
      DoubleTab& corresp_elems_ref = corresp_elems_.valeur().valeurs();
      int nb_tag_max = -1;
      for (int i = 0 ; i < nb_elem_tot ; i++)
        {
          int int_cor_elem = (int)lrint(corresp_elems_ref(i));
          if (int_cor_elem > nb_tag_max) nb_tag_max = int_cor_elem;
        }
      int dimtag = nb_tag_max+1;
      DoubleTrav elems_fluid_trust(dimtag);
      elems_fluid_trust = -1.e+10;
      for (int i = 0 ; i < nb_elem_tot ; i++)
        {
          int indextag = (int)lrint(corresp_elems_ref(i)) ;
          //Cerr << indextag << ", " << dimtag << ", " << nb_elem << ", " << nb_elem_tot << ", " << nb_som << ", " << nb_som_tot << finl;
          //abort();
          if (indextag < dimtag && indextag >= 0)
            {
              elems_fluid_trust(indextag) = i*1.0;
            }
          else
            {
              Cerr<<"erreur in computeFluidElems : elem corresp_elems_ref(elem) = "<<i<<" "<<corresp_elems_ref(i)<<" ; indextag = "<<indextag<<" < 0 ou >= "<<dimtag<<finl;
              exit();
            }
        }
      for (int i = 0 ; i < nb_som ; i++)
        {
          if (elems_fluid_ref(i) >= 0.0)
            {
              int indexr = (int)lrint(elems_fluid_ref(i));
              if (indexr < dimtag && indexr >= 0)
                {
                  if (elems_fluid_trust(indexr) >= 0.)
                    {
                      elems_fluid_ref(i) = elems_fluid_trust(indexr);
                    }
                  else
                    {
                      double x = fluid_points_ref(i,0);
                      double y = fluid_points_ref(i,1);
                      double z = fluid_points_ref(i,2);
                      double xs = solid_points_ref(i,0);
                      double ys = solid_points_ref(i,1);
                      double zs = solid_points_ref(i,2);
                      double d = (x-xs)*(x-xs)+(y-ys)*(y-ys)+(z-zs)*(z-zs);
                      if (d < eps)
                        // A ne pas considerer si points fluide et solide sont confondus (erreur prepro)
                        {
                          elems_fluid_ref(i) = elems_fluid_trust(indexr);
                        }
                      else
                        {
                          Cerr << __FILE__ << (int)__LINE__ << "Interpolation_IBM_elem_fluid::computeFluidElems : ERROR : joint width too low?" << finl;
                          Cerr<<"node elems_fluid_ref(node) index = "<<i<<" "<<elems_fluid_ref(i)<<" "<<indexr<<finl;
                          Cerr<<"elems_fluid_trust(index) < 0. = "<<elems_fluid_trust(indexr)<<finl;
                          Cerr<<"nb som som_tot elem elem_tot = "<<nb_som<<" "<<nb_som_tot<<" "<<nb_elem<<" "<<nb_elem_tot<<finl;
                          Cerr<<"coords_point(node) : x y z    = "<<coordsDom(i,0)<<" "<<coordsDom(i,1)<<" "<<coordsDom(i,2)<<finl;
                          Cerr<<"fluid_points(node) : xf yf zf = "<<x<<" "<<y<<" "<<z<<finl;
                          Cerr<<"solid_points(node) : xs ys zs = "<<xs<<" "<<ys<<" "<<zs<<finl;
                          int elem_found = le_dom_EF.domaine().chercher_elements(x,y,z);
                          Cerr<<"chercher_elements(xf,yf,zf) = "<<elem_found<<finl;
                          exit();
                        }
                    }
                }
              else
                {
                  Cerr << __FILE__ << (int)__LINE__ << "Interpolation_IBM_elem_fluid::computeFluidElems : ERROR : joint width too low?" << finl;
                  Cerr<<"node elems_fluid_ref(node) = "<<i<<" "<<elems_fluid_ref(i)<<finl;
                  Cerr<<"index = "<<indexr<<" < 0 ou >= "<<dimtag<<finl;
                  exit();
                }
            }
        }
    }
}
