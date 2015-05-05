#include <sstream>
#include <math.h>
#include <iostream>
#include <fstream>
#include <LataDB.h>
#include <LataFilter.h>
#include <LmlReader.h>
#include <LataWriter.h>
#include <stdlib.h>
#include "reactor.h"
#include "component_builder.h"
extern string IntToString(int);
extern char* c_string(const string&);
extern string extract_label(const string&);
extern string delete_label(const string&);
extern char* to_upper(const char*);
extern char* to_lower(const char*);
extern string& to_upper(string&);
extern string& to_lower(string&);
extern char next_character(istream&);
extern double max(const vector<double>&);
extern double min(const vector<double>&);

int check_data(const LataFilter& filter, const Domain_Id& id)
{
  Domain_Id requested_id(id);

  // Get the real timestep where this domain is stored:
  const LataGeometryMetaData & geom_metadata = filter.get_geometry_metadata(id.name_);
  
  if (geom_metadata.source_ == "latadb") {
    // Request for a native domain : load it from lataDB
    // If reconnect and loading all subdomains, go ! Don't store the operator in cache since it's
    //  not required to process fields.
    
    // Is it a mesh with FACES field?
    if (!filter.get_lataDB().field_exists(0, requested_id.name_, "FACES"))
      {
	cerr << "Error: No face field found in the input file !" << endl;
	exit(-1);
      }

    // Is it a mesh with INTERFACES_ELEM field ?
    filter.get_lataDB().get_field(1, requested_id.name_, "INDICATRICE_INTERF","ELEM");
  }

  return 1;
}

// #include "building_criterion.cpp"


/******************************************/
/******************************************/
/* Definition of class building criterion */
/******************************************/
/******************************************/

building_criterion::building_criterion() 
{ 
  type_=""; 
  coordinate_type_="CARTESIAN";
  bound_max_=0.; 
  bound_min_=0.; 
  replacing_bound_max_=false;
  replacing_bound_min_=false;
  reactor_=NULL; 

  //Function that build map<string, unsigned int> attributes
  build_type_keywords();
  build_coordinate_type_keywords();
}

void building_criterion::build_type_keywords()
{
  type_keywords_["GEOMETRIC"]=1;
  type_keywords_["MIXTE"]=2;
}

void building_criterion::build_coordinate_type_keywords()
{
  coordinate_type_keywords_["CARTESIAN"]=1;
  coordinate_type_keywords_["CYLINDRICAL"]=2;
  coordinate_type_keywords_["SPHERICAL"]=3;
}

building_criterion::~building_criterion() 
{ 
  bound_max_=0.; 
  bound_min_=0.; 
  replacing_bound_max_=false;
  replacing_bound_min_=false;
  type_=""; 
  coordinate_type_="CARTESIAN";
  reactor_=NULL; 

  vector_of_parsers_string_.clear();
  type_keywords_.clear();
  coordinate_type_keywords_.clear();

  //Special treatment for attribute parsers_
  for (unsigned int i=0; i<parsers_.size(); i++)
    delete parsers_[i];

  parsers_.clear();
}

void building_criterion::clear()
{
  bound_max_=0.; 
  bound_min_=0.; 
  replacing_bound_max_=false;
  replacing_bound_min_=false;
  type_=""; 
  coordinate_type_="CARTESIAN";
  reactor_=NULL; 

  vector_of_parsers_string_.clear();
  type_keywords_.clear();
  coordinate_type_keywords_.clear();

  //Special treatment for attribute parsers_
  for (unsigned int i=0; i<parsers_.size(); i++)
    delete parsers_[i];

  parsers_.clear();
}

bool building_criterion::is_satisfied(const int& element) const 
{
  assert(reactor_!=NULL);

  const vector<Field<FloatTab> >& latafields = reactorPtr()->unknown_data();
  vector<double> gravity_center;

  const unsigned int where = search_type(type_);
  const unsigned int which_coordinate = search_coordinate_type(coordinate_type_);
  unsigned int where_in_components = 0;

  double theta=0.;
  double phi=0.;
  double r=0.,r_xy=0.;

  bool is_criterion_ok = true;

  //Different results in accordance with the type of
  //the building criterion
  switch(where)
    {
    case 1 :

      assert(parsers_.size()!=0);

      gravity_center.resize(3);
      gravity_center = reactorPtr()->geometry().gravity_center(element);

      switch(which_coordinate)
	{
	case 1 :

	  for (unsigned int i=0; i<parsers_.size(); i++)
	    {
	      parsers_[i]->setVar( c_string("x"),gravity_center[0]);
	      parsers_[i]->setVar( c_string("y"),gravity_center[1]);
	      parsers_[i]->setVar( c_string("z"),gravity_center[2]);
	      is_criterion_ok &= (parsers_[i]->eval()==1.) ? true : false;
	    }
	  break;

	case 2 :

	  r = gravity_center[0]*gravity_center[0];
	  r += gravity_center[1]*gravity_center[1];
	  r = sqrt(r);

	  theta = 0.;
	  if (r>0.)
	    {
	      if (gravity_center[1]>=0.)
		theta = acos(gravity_center[0]/r);
	      else
		theta = 2*3.14159265-acos(gravity_center[0]/r);
	    }
	  
	  for (unsigned int i=0; i<parsers_.size(); i++)
	    {
	      parsers_[i]->setVar( c_string("r"),r);
	      parsers_[i]->setVar( c_string("theta"),theta);
	      parsers_[i]->setVar( c_string("z"),gravity_center[2]);
	      is_criterion_ok &= (parsers_[i]->eval()==1.) ? true : false;
	    }
	  
	  break;

	case 3 :

	  r_xy = gravity_center[0]*gravity_center[0];
	  r_xy += gravity_center[1]*gravity_center[1];

	  r = r_xy;
	  r += gravity_center[2]*gravity_center[2];

	  r_xy=sqrt(r_xy);
	  r = sqrt(r);

	  theta = 0.;
	  if (r_xy>0.)
	    {
	      if (gravity_center[1]>=0.)
		theta = acos(gravity_center[0]/r_xy);
	      else
		theta = 2*3.14159265-acos(gravity_center[0]/r_xy);
	    }
	  
	  phi = 0.;
	  if (r>0.)
	    phi = acos(gravity_center[2]/r);

	  for (unsigned int i=0; i<parsers_.size(); i++)
	    {
	      parsers_[i]->setVar( c_string("r"),r);
	      parsers_[i]->setVar( c_string("theta"),theta);
	      parsers_[i]->setVar( c_string("phi"),phi);
	      is_criterion_ok &= (parsers_[i]->eval()==1.) ? true : false;
	    }

	  break;

	default :

	  cerr<<"Error in building_criterion::is_satisfied()" << endl;
	  cerr<<"Unknown type of coordinate"<<endl;
	  cerr<<"Exiting program"<<endl;
	  exit(-1);

	  break;
	}

      return is_criterion_ok;
      break;

    case 2 :

      cerr << "Error in building_criterion::is_satisfied()" << endl;
      cerr << "Not coded yet" << endl;
      cerr << "Exiting programm" << endl;
      exit(-1); 
      
      return false;
      break;

    default :

      assert(where==0);
      const string component_name = type_+"_ELEM"; 
      where_in_components = reactorPtr()->search_component_name(component_name);

      if (where_in_components<0)
	{ 
	  cerr << "Error in building_criterion::is_satisfied()" << endl;
	  cerr << "Word " << type_ << " is not a known type" << endl;
	  cerr << "Known type are the following : ";
	  
	  typedef map<string, unsigned int>::const_iterator CI;
	  for (CI p=type_keywords_.begin(); p!=type_keywords_.end(); p++)
	    cerr << p->first << ", ";
	  cerr << endl;
	
	  cerr << "The type " << type_ << " of building criterion is not a known component." << endl;
	  cerr << "Exiting the programm." << endl;
	  exit(-1);
	}

      assert(where_in_components>=0);

      const Field<FloatTab> * latafld_ptr = dynamic_cast<const Field<FloatTab> *> (&latafields[where_in_components]);
      assert(latafld_ptr->data_.dimension(1)==1);

      //If one wants to have the maximum and minimum of unknown "type_"
      replace_bounds(latafields[where_in_components]);

      if ( (float(bound_min_)<=latafld_ptr->data_(element,0)) &&
           (latafld_ptr->data_(element,0)<float(bound_max_)) ) return true;
      else return false;
      break;
    } 

  return true;
}

void building_criterion::replace_bounds(const LataField_base& latafield) const
{
  //Tolerance to avoid non concording comparison
  const double tolerance = 1;

  assert((*dynamic_cast<const Field<FloatTab> *> (&latafield)).data_.dimension(1)==1);
  const ArrOfFloat & arr = (*dynamic_cast<const Field<FloatTab> *> (&latafield)).data_;

  if (replacing_bound_max_) 
    {
      bound_max_=double(max_array(arr))+tolerance;
      replacing_bound_max_=false;
    }
  if (replacing_bound_min_) 
    {
      bound_min_=double(min_array(arr))-tolerance;
      replacing_bound_min_=false;
    }
}

unsigned int building_criterion::search_type(const string& s) const
{
  typedef map<string, unsigned int>::const_iterator CI;
  CI is_found = type_keywords_.find(s);

  const unsigned int where = (is_found==type_keywords_.end()) ? 0 : is_found->second;
  assert(where>=0 && where< type_keywords_.size()+1);

  return where;
}

unsigned int building_criterion::search_coordinate_type(const string& s) const
{
  typedef map<string, unsigned int>::const_iterator CI;
  CI is_found = coordinate_type_keywords_.find(s);

  const unsigned int where = (is_found==coordinate_type_keywords_.end()) ? 0 : is_found->second;
  assert(where>=0 && where< coordinate_type_keywords_.size()+1);

  return where;
}

void building_criterion::print(ostream& os) const
{
  os << "Number of building criterion : " << number_ << endl;
  os << "Type of building criterion : " << type_ << endl;
//   os << "Type of used coordinates : " << coordinate_type_ << endl;
  os << "Bound min and bound max : " << bound_min_ << ", " << bound_max_ << endl;

  os << "Parsers string : " ;
  for (unsigned int i=0; i<vector_of_parsers_string_.size(); i++)
    os << vector_of_parsers_string_[i] << "; ";
  os << endl;

  os << "Known type_keywords : ";
  typedef map<string, unsigned int>::const_iterator CI;
  for (CI p=type_keywords_.begin(); p!=type_keywords_.end(); p++)
    os << p->first << ", ";
  os << endl;

//   os << "Known coordinate_type_keywords : ";
//   typedef map<string, unsigned int>::const_iterator CI;
//   for (CI p=coordinate_type_keywords_.begin(); p!=coordinate_type_keywords_.end(); p++)
//     os << p->first << ", ";
//   os << endl;
}


void building_criterion::build_parsers()
{
  const unsigned int size = vector_of_parsers_string_.size();
  assert(size!=0);

  parsers_.resize(size);

  for (unsigned int i=0; i<size; i++)
    {
      String parser_String( c_string(vector_of_parsers_string_[i]) );
      parsers_[i] = new Parser(parser_String,6);
      parsers_[i]->addVar( c_string("x") );
      parsers_[i]->addVar( c_string("y") );
      parsers_[i]->addVar( c_string("z") );
      parsers_[i]->addVar( c_string("r") );
      parsers_[i]->addVar( c_string("theta") );
      parsers_[i]->addVar( c_string("phi") );
      parsers_[i]->parseString();
    }
}

void building_criterion::build_bounds(double bound_min, double bound_max)
{
  bound_min_=bound_min;
  bound_max_=bound_max;
}

void building_criterion::build_parsers_string(const vector<string>& parsers_string)
{
  const unsigned int size = parsers_string.size();
  vector_of_parsers_string_.resize(size);
  
  for (unsigned int i=0; i<size; i++)
    vector_of_parsers_string_[i]=parsers_string[i];
}


/****************************************/
/****************************************/
/* Definition of class geometry_reactor */
/****************************************/
/****************************************/

void build_faces_elem(const DomainUnstructured & dom, IntTab & faces_elem)
{
  if (!dom.faces_ok()) {
    cerr << "Error: input mesh has not description of the faces" << endl;
  }
  faces_elem.resize(dom.nb_faces(), 2);
  ((ArrOfInt&)faces_elem) = -1;
  const int nb_elem = dom.elem_faces_.dimension(0);
  const int nb_faces_elem = dom.elem_faces_.dimension(1);
  for (int i = 0; i < nb_elem; i++) {
    for (int j = 0; j < nb_faces_elem; j++) {
      const int face = dom.elem_faces_(i, j);
      if (faces_elem(face, 0) == i || faces_elem(face, 1) == i ) {
	// attention, pour un calcul parallele et un domaine reconnecte, les faces de joint apparaissent 2 fois
	// ne rien faire
      } else if (faces_elem(face, 0) < 0) {
	faces_elem(face, 0) = i;
      } else if (faces_elem(face, 1) < 0) {
	faces_elem(face, 1) = i; 
      } else {
	cerr << "Error building faces_element connectivity: face " << face << " has more than 2 connected elements" << endl;
	cerr << faces_elem(face, 0) << " " << faces_elem(face, 1) << " " << i << endl;
	exit(-1);
      }
    }
  }
}


/* Build the class geometry_reactor */
/* Fill the different attributes with the parameter given */
geometry_reactor::geometry_reactor(reactor& Reactor)
{
  //Fill attribute *reactor_
  reactor_=&Reactor;

  //Build and fill the array elements_
  //Fill attribute number_of_elements_
  //Calculation of the reactor volume
  const bool is_elements_built = build_elements();
  volume_ = volume_calculation();

  //Build and fill the array faces_
  const bool is_faces_built = build_faces();

  //Build and fill the array vertices_
  const bool is_vertices_built = build_vertices();

  //Build and fill the array boundary_faces_
  const bool is_boundary_faces_built = build_boundary_faces();

  //Build and fill the array exterior_normal_surfaces_
  const bool is_exterior_normal_surface_built = build_exterior_normal_surface();

  if (is_elements_built && is_faces_built && 
	 is_vertices_built && is_boundary_faces_built && 
	 is_exterior_normal_surface_built)
  {
  }
  else
  {
     Cerr << "Something wrong in geometry_reactor::geometry_reactor" << finl;
     exit(-1);
  }
}

bool geometry_reactor::build_elements()
{
  assert(reactor_!=NULL);
  assert(reactorPtr()->is_defined());

  const Domain& domain = reactorPtr()->domain();
  const building_criterion& criterion = reactorPtr()->criterion();

  //Build and fill the array elements_
  //Fill attribute number_of_elements_
  const int nb_elements = domain.nb_elements();

  elements_.resize_array(nb_elements);
  elements_=0;
  number_of_elements_=0;
  
  cerr << "Treatment of reactor number : " << reactorPtr()->number() << endl;
  for (int element=0; element<nb_elements; element++)
    if (criterion.is_satisfied(element)) 
      { elements().setbit(element); number_of_elements_++; }
  cerr << "End of treatment of reactor number : " << reactorPtr()->number() << endl;

  return true;
}

bool geometry_reactor::build_faces()
{
  assert(reactor_!=NULL);
  assert(reactorPtr()->is_defined());
  assert(elements_.size_array()!=0);
  assert(elements_.size_array()==reactorPtr()->domain().nb_elements());

  const Domain& domain = reactorPtr()->domain();

  //Build and fill the array faces_
  //Fill attribute number_of_faces_
  const int nb_elements = domain.nb_elements();
  const int nb_faces = domain.nb_faces();
  number_of_faces_ = 0;

  const DomainUnstructured * domainUnstructured_Ptr = dynamic_cast<const DomainUnstructured *>(&domain);

  if (!domainUnstructured_Ptr)
  {
    Journal() << "Error : domain " << domain.id_.name_ << " is not domain unstructured" << endl;
    throw;
  }

  const IntTab& faces_per_element = domainUnstructured_Ptr->elem_faces_;
  const int nb_faces_per_element = faces_per_element.dimension(1);

  ArrOfBit is_face_in_reactor(nb_faces);
  is_face_in_reactor=0;


  // Detection of faces that belong to reactor
  for (int element=0; element<nb_elements; element++)
    for (int face_loc=0; face_loc<nb_faces_per_element; face_loc++)
      if (elements_[element]) is_face_in_reactor.setbit(faces_per_element(element,face_loc));


  // Resizing and filling array faces_
  unsigned int faces_size = 0;
  for (int face=0; face<nb_faces; face++) 
    if (is_face_in_reactor[face]) faces_size++;

  unsigned int place = 0;
  number_of_faces_ = faces_size; //fill attribute number_of_faces_
  faces_.resize_array(faces_size);
  for (int face=0; face<nb_faces; face++)
    if (is_face_in_reactor[face]) faces_[place++]=face;

  assert(place==faces_size);

  return true;
}

bool geometry_reactor::build_vertices()
{
  assert(reactor_!=NULL);
  assert(reactorPtr()->is_defined());;
  assert(elements_.size_array()!=0);
  assert(elements_.size_array()==reactorPtr()->domain().nb_elements());

  const Domain& domain = reactorPtr()->domain();

  //Build and fill the array vertices_
  //Fill attribute number_of_vertices_
  const int nb_elements = domain.nb_elements();
  const int nb_vertices = domain.nb_nodes();
  number_of_vertices_ = 0;

  const DomainUnstructured * domainUnstructured_Ptr = dynamic_cast<const DomainUnstructured *>(&domain);

  if (!domainUnstructured_Ptr)
  {
    Journal() << "Error : domain " << domain.id_.name_ << " is not domain unstructured" << endl;
    throw;
  }

  const IntTab& vertices_per_element = domainUnstructured_Ptr->elements_;
  const int nb_vertices_per_element = vertices_per_element.dimension(1);

  ArrOfBit is_vertex_in_reactor(nb_vertices);
  is_vertex_in_reactor=0;


  // Detection of vertices that belong to reactor
  for (int element=0; element<nb_elements; element++)
    for (int vertex_loc=0; vertex_loc<nb_vertices_per_element; vertex_loc++)
      if (elements_[element]) is_vertex_in_reactor.setbit(vertices_per_element(element,vertex_loc));


  // Resizing and filling array vertices_
  unsigned int vertices_size = 0;
  for (int vertex=0; vertex<nb_vertices; vertex++) 
    if (is_vertex_in_reactor[vertex]) vertices_size++;

  unsigned int place = 0;
  number_of_vertices_ = vertices_size; //fill attribute number_of_vertices_
  vertices_.resize_array(vertices_size);
  for (int vertex=0; vertex<nb_vertices; vertex++)
    if (is_vertex_in_reactor[vertex]) vertices_[place++]=vertex;

  assert(place==vertices_size);

  return true;
}
 
bool geometry_reactor::build_boundary_faces()
{
  assert(reactor_!=NULL);
  assert(reactorPtr()->is_defined());;
  assert(elements_.size_array()!=0);

  const Domain& domain = reactorPtr()->domain();

  //Build and fill the array boundary_faces_
  const int nb_faces = domain.nb_faces();
  const int nb_elements = domain.nb_elements();
  number_of_boundary_faces_ = 0;

  const DomainUnstructured * domainUnstructured_Ptr = dynamic_cast<const DomainUnstructured *>(&domain);

  if (!domainUnstructured_Ptr)
  {
    Journal() << "Error : domain " << domain.id_.name_ << " is not domain unstructured" << endl;
    throw;
  }

  const IntTab& faces_per_element = domainUnstructured_Ptr->elem_faces_;
  const int nb_faces_per_element = faces_per_element.dimension(1);

  /* contains the number of element that are neighbors of a face : */
  /* if a face is a boundary one, it has only one neighbor */
  /* if a face is an internal one, it has only two neighbors */
  ArrOfInt face_neighbors(nb_faces);
  face_neighbors=0;

  for (int element=0; element<nb_elements; element++)
    if (elements_[element])
      for (int face_loc=0; face_loc<nb_faces_per_element; face_loc++)
	face_neighbors[faces_per_element(element,face_loc)]++;

  for (int face=0; face<nb_faces; face++)
    if (face_neighbors[face]==1) number_of_boundary_faces_++;

  unsigned int place = 0;
  boundary_faces_.resize_array(number_of_boundary_faces_);
  for (int face=0; face<nb_faces; face++)
    if (face_neighbors[face]==1) { boundary_faces_[place++]=face; }

  assert(place==number_of_boundary_faces_);

  return true;
}

int geometry_reactor::opposite_vertex(int element, int face) const
{
  assert(reactor_!=NULL);
  assert(reactorPtr()->is_defined());;

  const Domain& domain = reactorPtr()->domain();

  const DomainUnstructured * domainUnstructured_Ptr = dynamic_cast<const DomainUnstructured *>(&domain);

  if (!domainUnstructured_Ptr)
  {
    Journal() << "Error : domain " << domain.id_.name_ << " is not domain unstructured" << endl;
    throw;
  }

  const IntTab& vertices_per_element = domainUnstructured_Ptr->elements_;
  const IntTab& vertices_per_face = domainUnstructured_Ptr->faces_;
  const int nb_vertices_per_element = vertices_per_element.dimension(1);
  const int nb_vertices_per_face = vertices_per_face.dimension(1);


  for (int i=0; i<nb_vertices_per_element; i++)
    {
      const int vertex = vertices_per_element(element,i);
      bool is_opposite_to_face = true;

      for (int j=0; j<nb_vertices_per_face; j++)
	{
	  const int vertex_face = vertices_per_face(face,j);
	  if (vertex_face==vertex) is_opposite_to_face &= false;
	}//end for on j

      if (is_opposite_to_face) return vertex;

    }//end for on i

  return -1;
}

bool geometry_reactor::build_exterior_normal_surface()
{
  assert(reactor_!=NULL);
  assert(reactorPtr()->is_defined());;
  assert(reactorPtr()->domain().elt_type_>Domain::quadri);

  const Domain& domain = reactorPtr()->domain();
  
  //Build and fill the array exterior_normal_surfaces_
  //Available only in 3D : inspired of TRUST code 
  exterior_normal_surface_.resize(number_of_boundary_faces_,3);

  const DomainUnstructured * domainUnstructured_Ptr = dynamic_cast<const DomainUnstructured *>(&domain);

  if (!domainUnstructured_Ptr)
  {
    Journal() << "Error : domain " << domain.id_.name_ << " is not domain unstructured" << endl;
    throw;
  }

  const FloatTab& positions = domainUnstructured_Ptr->nodes_;
  const IntTab& vertex_per_face = domainUnstructured_Ptr->faces_;
  IntTab faces_to_elements;
  build_faces_elem(*domainUnstructured_Ptr, faces_to_elements);

  const double coeff = (domain.elt_type_==Domain::tetra)? 2. : 1.;

  for (unsigned int face_loc=0; face_loc<number_of_boundary_faces_; face_loc++)
    {
      const int face = boundary_faces_[face_loc];
      const int reactor_element = is_element(faces_to_elements(face,0)) ? 
	faces_to_elements(face,0) : faces_to_elements(face,1);
      
      const int opposite_vertex = geometry_reactor::opposite_vertex(reactor_element,face);
      const int first_vertex = vertex_per_face(face,0);
      const int second_vertex = vertex_per_face(face,1);
      const int third_vertex = vertex_per_face(face,2);
      
      //Evaluation of the cross-product between to edge of ¨face¨ :
      //we thus have a vector that is orhtogonal to ¨face¨ 
      const double x1 = positions(first_vertex,0)-positions(second_vertex,0);  
      const double y1 = positions(first_vertex,1)-positions(second_vertex,1);  
      const double z1 = positions(first_vertex,2)-positions(second_vertex,2);  
      
      const double x2 = positions(third_vertex,0)-positions(second_vertex,0);  
      const double y2 = positions(third_vertex,1)-positions(second_vertex,1);  
      const double z2 = positions(third_vertex,2)-positions(second_vertex,2);
      
      const double nx = exterior_normal_surface_(face_loc,0)=(y1*z2-y2*z1)/coeff;
      const double ny = exterior_normal_surface_(face_loc,1)=(x2*z1-x1*z2)/coeff;
      const double nz = exterior_normal_surface_(face_loc,2)=(x1*y2-x2*y1)/coeff;
      
      //Evaluation of the dot-product between the previous calculated vector
      //and the vector built with one vertex of ¨face¨ and the opposite vertex of ¨face¨
      //We use the sign of the dot-product to choose appropriate orientation of 
      //¨exterior_normal_surfaces_¨
      const double x_opposite = positions(opposite_vertex,0)-positions(first_vertex,0);
      const double y_opposite = positions(opposite_vertex,1)-positions(first_vertex,1);
      const double z_opposite = positions(opposite_vertex,2)-positions(first_vertex,2);
      
      if (nx*x_opposite+ny*y_opposite+nz*z_opposite>0)
	{
	  exterior_normal_surface_(face_loc,0)*=-1.;
	  exterior_normal_surface_(face_loc,1)*=-1.;
	  exterior_normal_surface_(face_loc,2)*=-1.;
	}
    }//end for "face_loc"
  
  return true;
}

/* Build the class geometry_reactor */
geometry_reactor::geometry_reactor() : elements_(), faces_(), vertices_(), 
				       boundary_faces_(), exterior_normal_surface_() 
{
  number_of_elements_=0;
  number_of_faces_=0;
  number_of_vertices_=0;
  number_of_boundary_faces_=0;

  volume_=0.;

  reactor_=NULL;
}

/* Destroy the class geometry_reactor */
geometry_reactor::~geometry_reactor() 
{
  number_of_elements_=0;
  elements_.resize_array(0);
  volume_=0.;

  number_of_faces_=0;
  faces_.reset();

  number_of_vertices_=0;
  vertices_.reset();

  number_of_boundary_faces_=0;
  boundary_faces_.reset();
  exterior_normal_surface_.reset();

  reactor_=NULL;
}

void geometry_reactor::clear()
{
  number_of_elements_=0;
  elements_.resize_array(0);
  volume_=0.;

  number_of_faces_=0;
  faces_.reset();

  number_of_vertices_=0;
  vertices_.reset();

  number_of_boundary_faces_=0;
  boundary_faces_.reset();
  exterior_normal_surface_.reset();

  reactor_=NULL;
}

/* Initialize the class geometry_reactor when already build */
/* with default constructor */
void geometry_reactor::build(reactor& Reactor)
{
  //Fill attribute *reactor_
  reactor_=&Reactor;

  //Build and fill the array elements_
  //Fill attribute number_of_elements_
  //Calculation of the reactor volume

  const bool is_elements_built = build_elements();
  volume_ = volume_calculation();

  //Build and fill the array faces_
  const bool is_faces_built = build_faces();

  //Build and fill the array vertices_
  const bool is_vertices_built = build_vertices();

  //Build and fill the array boundary_faces_
  const bool is_boundary_faces_built = build_boundary_faces();

  //Build and fill the array exterior_normal_surfaces_
  //Available only in 3D : inspired of TRUST code 
  const bool is_exterior_normal_surface_built = build_exterior_normal_surface();

  if (is_elements_built && is_faces_built && 
	 is_vertices_built && is_boundary_faces_built &&
	 is_exterior_normal_surface_built) {}
  else { Cerr << "Something wrong in geometry_reactor::build..." << finl; exit(-1); }
}

/* Calculation of the reactor volume */
double geometry_reactor::volume_calculation() const
{
  assert(reactor_!=NULL);
  assert(reactorPtr()->is_defined());;

  double total_volume = 0.;

  for (int i=0; i<reactorPtr()->domain().nb_elements(); i++)
    if (elements_[i]) total_volume+=volume_calculation(i);
    
  return total_volume;
}

double geometry_reactor::volume_calculation(const int& element) const
{
  assert(reactor_!=NULL);
  assert(reactorPtr()->is_defined());;
  assert(reactorPtr()->domain().elt_type_>Domain::quadri);

  const Domain& domain = reactorPtr()->domain();

  const DomainUnstructured * domainUnstructured_Ptr = dynamic_cast<const DomainUnstructured *>(&domain);

  if (!domainUnstructured_Ptr)
  {
    Journal() << "Error : domain " << domain.id_.name_ << " is not domain unstructured" << endl;
    throw;
  }

  const FloatTab& positions = domainUnstructured_Ptr->nodes_;
  const IntTab& face_per_element = domainUnstructured_Ptr->elem_faces_;
  const IntTab& vertex_per_face = domainUnstructured_Ptr->faces_;

  double coeff = 0.;

  switch(domain.elt_type_)
    {
    case Domain::tetra : //TETRAHEDRON
      
      coeff=1./6;
      break;

    case Domain::hexa : //HEXAHEDRON

      coeff=1.;
      break;

    default : 

      cerr << "Error in geometry_reactor::volume_calculation()" << endl;
      cerr << "Not coded yet" << endl;
      cerr << "Exiting program" << endl;
      exit(-1);

      break;

    }//end of switch

  const int face = face_per_element(element,0);
  const int first_vertex = vertex_per_face(face,0);
  const int second_vertex = vertex_per_face(face,1);
  const int third_vertex = vertex_per_face(face,2);
  const int fourth_vertex = opposite_vertex(element,face);

  const double x0 = positions(first_vertex,0)-positions(second_vertex,0);  
  const double y0 = positions(first_vertex,1)-positions(second_vertex,1);  
  const double z0 = positions(first_vertex,2)-positions(second_vertex,2);  
  
  const double x1 = positions(first_vertex,0)-positions(third_vertex,0);  
  const double y1 = positions(first_vertex,1)-positions(third_vertex,1);  
  const double z1 = positions(first_vertex,2)-positions(third_vertex,2);
  
  const double x2 = positions(first_vertex,0)-positions(fourth_vertex,0);  
  const double y2 = positions(first_vertex,1)-positions(fourth_vertex,1);  
  const double z2 = positions(first_vertex,2)-positions(fourth_vertex,2);
  
  double volume_per_element = x0*(y1*z2-y2*z1);
  volume_per_element-=x1*(y0*z2-y2*z0);
  volume_per_element+=x2*(y0*z1-y1*z0);
  volume_per_element*=coeff;
  volume_per_element=fabs(volume_per_element);

  return volume_per_element;
}

vector<double> geometry_reactor::gravity_center(int element) const
{
  assert(reactor_!=NULL);
  assert(reactorPtr()->is_defined());

  const Domain& domain = reactorPtr()->domain();

  const DomainUnstructured * domainUnstructured_Ptr = dynamic_cast<const DomainUnstructured *>(&domain);

  if (!domainUnstructured_Ptr)
  {
    Journal() << "Error : domain " << domain.id_.name_ << " is not domain unstructured" << endl;
    throw;
  }

  const IntTab& vertices_per_element = domainUnstructured_Ptr->elements_;
  const FloatTab& positions = domainUnstructured_Ptr->nodes_;

  const int shape = positions.dimension(1);
  const int nb_vertices_per_element = vertices_per_element.dimension(1);
  
  assert(element>=0 && element<reactorPtr()->domain().nb_elements());

  vector<double> gravity_center(shape);

  for (int som_loc=0; som_loc<nb_vertices_per_element; som_loc++)
    {
      const int som_glob = vertices_per_element(element,som_loc);
      
      for (int dim=0; dim<shape; dim++)
	gravity_center[dim]+=positions(som_glob,dim)/nb_vertices_per_element;
    }
    
  return gravity_center;
}



void geometry_reactor::print(ostream& os, bool is_complete_data_wanted) const
{
  os << "Volume of reactor : " << volume_ << endl;

  os << "Number of elements in reactor : " << number_of_elements_ << endl;
  if (is_complete_data_wanted)
    {
      os << "Printing list of elements in reactor : ";
      for (int i=0; i<reactorPtr()->domain().nb_elements(); i++)
	if (elements_[i]) os << i << ", ";
      os << endl;
    }

  os << "Number of faces in reactor : " << number_of_faces_ << endl;
  if (is_complete_data_wanted)
    {  
      os << "Printing list of faces in reactor : ";
      for (unsigned int i=0; i<number_of_faces_; i++)
	os << faces_[i] << ", ";
      os << endl;
    }

  os << "Number of vertices in reactor : " << number_of_vertices_ << endl;
  if (is_complete_data_wanted)
    {
      os << "Printing list of vertices in reactor : ";
      for (unsigned int i=0; i<number_of_vertices_; i++)
	os << vertices_[i] << ", ";
      os << endl;
    }

  os << "Number of boundary_faces in reactor : " << number_of_boundary_faces_ << endl;
  if (is_complete_data_wanted)
    {
      os << "Printing list of boundary_faces in reactor : ";
      for (unsigned int i=0; i<number_of_boundary_faces_; i++)
	os << boundary_faces_[i] << ", ";
      os << endl;
    }

  if (is_complete_data_wanted)
    {
      os << "Printing list of exterior_normal_surface in reactor : ";
      for (int i=0; i<exterior_normal_surface_.dimension(0); i++)
	{
	  os << "(";
	  for (int j=0; j<exterior_normal_surface_.dimension(1); j++)
	    os << exterior_normal_surface_(i,j) << ",";
	  os << ") ;";
	}
      os << endl;
    }
}


/***************************************/
/***************************************/
/* Definition of class physics_reactor */
/***************************************/
/***************************************/


/* Build the class physics_reactor */
/* Fill the different attributes with the parameter given */
physics_reactor::physics_reactor(reactor& Reactor)
{
  //Fill attribute *reactor_
  reactor_=&Reactor;
  assert(reactorPtr()->is_defined());
  assert(reactorPtr()->is_geometry_defined());

  //Build mean of some components
  if (reactorPtr()->is_post_treatment_defined()) build_reactor_components();
}

physics_reactor::physics_reactor()
{
  reactor_=NULL;
}

physics_reactor::~physics_reactor()
{
  reactor_components_.clear();
  reactor_=NULL;
}

void physics_reactor::clear()
{
  reactor_components_.clear();
  reactor_=NULL;
}

vector<double> physics_reactor::physics_average(unsigned int i) const
{
  assert(reactor_!=NULL);
  assert(reactorPtr()->is_defined());
  assert(reactorPtr()->is_geometry_defined());
  assert(i<reactorPtr()->unknown_data().size());

  const LataField_base& latafield = reactorPtr()->unknown_data(i);

  const Domain& domain = reactorPtr()->domain();

  const DomainUnstructured * domainUnstructured_Ptr = dynamic_cast<const DomainUnstructured *>(&domain);

  if (!domainUnstructured_Ptr)
  {
    Journal() << "Error : domain " << domain.id_.name_ << " is not domain unstructured" << endl;
    throw;
  }

  const IntTab& vertex_per_element = domainUnstructured_Ptr->elements_;
  const IntTab& face_per_element = domainUnstructured_Ptr->elem_faces_;

  const Nom label = LataField_base::localisation_to_string(latafield.localisation_);

  const Field<FloatTab> * latafld_ptr = dynamic_cast<const Field<FloatTab> *> (&latafield);

  const int shape = latafld_ptr->data_.dimension(1);
  const int nb_elements = reactorPtr()->domain().nb_elements();

  int domain_total_dof = -1;
  if (label=="ELEM") domain_total_dof = reactorPtr()->domain().nb_elements();
  if (label=="SOM") domain_total_dof = reactorPtr()->domain().nb_nodes();
  if (label=="FACES") domain_total_dof = reactorPtr()->domain().nb_faces();
  assert(domain_total_dof!=-1);

  int reactor_total_dof = -1;
  if (label=="ELEM") reactor_total_dof = reactorPtr()->geometry().nb_elements();
  if (label=="SOM") reactor_total_dof = reactorPtr()->geometry().nb_vertices();
  if (label=="FACES") reactor_total_dof = reactorPtr()->geometry().nb_faces();
  assert(reactor_total_dof!=-1);

  int dof_by_element = -1;
  if (label=="ELEM") dof_by_element = 1;
  if (label=="SOM") dof_by_element = vertex_per_element.dimension(1);
  if (label=="FACES") dof_by_element = face_per_element.dimension(1);
  assert(dof_by_element!=-1);

  ArrOfBit is_dof_seen(domain_total_dof);
  is_dof_seen=0;

  vector<double> average(shape);
  
  for (int element=0; element<nb_elements; element++)
    {
      if (reactorPtr()->geometry().is_element(element))
	{
	  for (int dof=0; dof<dof_by_element; dof++)
	    {	  
	      int place = -1;
	      if (label=="ELEM") place = element;
	      if (label=="SOM") place = vertex_per_element(element,dof);
	      if (label=="FACES") place = face_per_element(element,dof);
	      assert(place!=-1);
	      assert(place<domain_total_dof);

	      if (!is_dof_seen[place]) //to count a dof only one time
		{
		  is_dof_seen.setbit(place); //now dof "place" has been counted

		  for (int k=0; k<shape; k++)
		      average[k]+=latafld_ptr->data_(place,k);

		}//end if

	    }//end for on dof

	}//end if

    }//end for on element

  for (unsigned int j=0; j<average.size(); j++)
    average[j]/=reactor_total_dof;

  return (reactor_total_dof==0) ? vector<double>(shape) : average;
}

vector<double> physics_reactor::physics_volumic_average(unsigned int i) const
{
  assert(reactor_!=NULL);
  assert(reactorPtr()->is_defined());
  assert(reactorPtr()->is_geometry_defined());
  assert(i<reactorPtr()->unknown_data().size());
  
  const LataField_base& latafield = reactorPtr()->unknown_data(i);

  const Domain& domain = reactorPtr()->domain();

  const DomainUnstructured * domainUnstructured_Ptr = dynamic_cast<const DomainUnstructured *>(&domain);

  if (!domainUnstructured_Ptr)
  {
    Journal() << "Error : domain " << domain.id_.name_ << " is not domain unstructured" << endl;
    throw;
  }

  const IntTab& vertex_per_element = domainUnstructured_Ptr->elements_;
  const IntTab& face_per_element = domainUnstructured_Ptr->elem_faces_;

  const Nom label = LataField_base::localisation_to_string(latafield.localisation_);

  const Field<FloatTab> * latafld_ptr = dynamic_cast<const Field<FloatTab> *> (&latafield);

  const int shape = latafld_ptr->data_.dimension(1);

  const int nb_elements = reactorPtr()->domain().nb_elements();
  const int nb_of_reactor_elements = reactorPtr()->geometry().nb_elements();

  int dof_by_element = -1;
  if (label=="ELEM") dof_by_element = 1;
  if (label=="SOM") dof_by_element = vertex_per_element.dimension(1);
  if (label=="FACES") dof_by_element = face_per_element.dimension(1);
  assert(dof_by_element!=-1);

  const double total_volume = reactorPtr()->geometry().volume();

  vector<double> volumic_average(shape);
  
  for (int element=0; element<nb_elements; element++)
    {
      if (reactorPtr()->geometry().is_element(element))
	{
	  const double volume = reactorPtr()->geometry().volume_calculation(element);

	  for (int j=0; j<dof_by_element; j++)
	    {	  
	      int place = -1;
	      if (label=="ELEM") place = element;
	      if (label=="SOM") place = vertex_per_element(element,j);
	      if (label=="FACES") place = face_per_element(element,j);
	      assert(place!=-1);

	      for (unsigned int k=0; k<volumic_average.size(); k++)
		volumic_average[k]+=latafld_ptr->data_(place,k)*volume/dof_by_element;

	    }//end for on j

	}//end if

    }//end for on element

  for (unsigned int j=0; j<volumic_average.size(); j++)
    volumic_average[j]/=total_volume;

  return (nb_of_reactor_elements==0) ? vector<double>(shape) : volumic_average;
}

//Calculation of the flux of component velocity through the boundary_faces
//REMARK : a flux through a boundary face "f" is null, if elements that are
//        neighbors of "f" are not TOTALLY filled with liquid 
vector<double> physics_reactor::velocity_flux() const
{
  assert(reactor_!=NULL);
  assert(reactorPtr()->is_defined());
  assert(reactorPtr()->is_geometry_defined());

  const int nb_boundary_faces = reactorPtr()->geometry().nb_boundary_faces(); 

  const DoubleTab& exterior_normal_surface = reactorPtr()->geometry().exterior_normal_surface();
  const ArrOfInt& boundary_faces = reactorPtr()->geometry().boundary_faces();

  const Domain& domain = reactorPtr()->domain();

  const DomainUnstructured * domainUnstructured_Ptr = dynamic_cast<const DomainUnstructured *>(&domain);

  if (!domainUnstructured_Ptr)
  {
    Journal() << "Error : domain " << domain.id_.name_ << " is not domain unstructured" << endl;
    throw;
  }

  IntTab faces_to_elements;
  build_faces_elem(*domainUnstructured_Ptr, faces_to_elements);

  const vector<Field<FloatTab> >& latafields = reactorPtr()->unknown_data();

  vector<double> velocity_flux(nb_boundary_faces);

  const unsigned int velocity_place = reactorPtr()->search_component_name("VITESSE_FACES");
  const int dimension = exterior_normal_surface.dimension(1);

  const Field<FloatTab> * latafld_ptr = dynamic_cast<const Field<FloatTab> *> (&latafields[velocity_place]);

  const int shape = latafld_ptr->data_.dimension(1);

  const unsigned int indicatrice_place = reactorPtr()->search_component_name("INDICATRICE_INTERF_ELEM");
  const LataField_base& indicatrice = reactorPtr()->unknown_data(indicatrice_place);

  const Field<FloatTab> * indic_ptr = dynamic_cast<const Field<FloatTab> *> (&indicatrice);

  assert(indic_ptr->data_.dimension(0)==reactorPtr()->domain().nb_elements());
  assert(indic_ptr->data_.dimension(1)==1);
  
  switch(shape)
    {      
    case 1 : //VDF case : to modify

      for (int i=0; i<nb_boundary_faces; i++)
	{
	  bool is_liquid_present = true;
	  const int neighbor1 = faces_to_elements(boundary_faces[i],0);
	  const int neighbor2 = faces_to_elements(boundary_faces[i],1);
	  if (neighbor1!=-1) is_liquid_present &= (indic_ptr->data_(neighbor1,0)==1);
	  if (neighbor2!=-1) is_liquid_present &= (indic_ptr->data_(neighbor2,0)==1);

	  if (is_liquid_present)
	    for (int j=0; j<dimension; j++)
	      if ( fabs(exterior_normal_surface(i,j)) > 0 )
		velocity_flux[i]+=latafld_ptr->data_(boundary_faces[i],0)*exterior_normal_surface(i,j);

	}//end of for on i

      break;

    case 2 : case 3 : //VEF case

      for (int i=0; i<nb_boundary_faces; i++)
	{
	  bool is_liquid_present = true;
	  const int neighbor1 = faces_to_elements(boundary_faces[i],0);
	  const int neighbor2 = faces_to_elements(boundary_faces[i],1);
	  if (neighbor1!=-1) is_liquid_present &= (indic_ptr->data_(neighbor1,0)==1);
	  if (neighbor2!=-1) is_liquid_present &= (indic_ptr->data_(neighbor2,0)==1);

	  if (is_liquid_present)
	    for (int j=0; j<shape; j++)
	      velocity_flux[i]+=latafld_ptr->data_(boundary_faces[i],j)*exterior_normal_surface(i,j);

	}//end of for on i

      break;

    default :

      cerr << "Error in physics_reactor::velocity_flux()" << endl;
      cerr << "This case does not exist" << endl;
      cerr << "Exiting programm." << endl;
      exit(-1);

      break;

    }//end switch


  for (int i=0; i<nb_boundary_faces; i++)
    for (int j=0; j<shape; j++)
      if (velocity_flux[i]<0) velocity_flux[i]=0;

  return velocity_flux;
}

//Calculation of the flux of component number "i" through the boundary_faces
//REMARK : the component number "i" HAS TO be of vector type.
//         if not, use object component_builder to build appropriate component
//REMARK : a flux through a boundary face "f" is null, if elements that are
//        neighbors of "f" are not TOTALLY filled with liquid 
vector<double> physics_reactor::component_flux(unsigned int i) const
{
  assert(reactor_!=NULL);
  assert(reactorPtr()->is_defined());
  assert(reactorPtr()->is_geometry_defined());
  assert(i< reactorPtr()->unknown_data().size());

  const LataField_base& latafield = reactorPtr()->unknown_data(i);

  const ArrOfInt& boundary_faces = reactorPtr()->geometry().boundary_faces();
  const DoubleTab& exterior_normal_surface = reactorPtr()->geometry().exterior_normal_surface();

  const Domain& domain = reactorPtr()->domain();

  const DomainUnstructured * domainUnstructured_Ptr = dynamic_cast<const DomainUnstructured *>(&domain);

  if (!domainUnstructured_Ptr)
  {
    Journal() << "Error : domain " << domain.id_.name_ << " is not domain unstructured" << endl;
    throw;
  }

  IntTab faces_to_elements;
  build_faces_elem(*domainUnstructured_Ptr, faces_to_elements);

  const Field<FloatTab> * latafld_ptr = dynamic_cast<const Field<FloatTab> *> (&latafield);

  const int shape = latafld_ptr->data_.dimension(1);

  const int dimension = exterior_normal_surface.dimension(1);
  const int nb_of_boundary_faces = reactorPtr()->geometry().nb_boundary_faces();

  const unsigned int indicatrice_place = reactorPtr()->search_component_name("INDICATRICE_INTERF_ELEM");
  const LataField_base& indicatrice = reactorPtr()->unknown_data(indicatrice_place);

  const Field<FloatTab> * indic_ptr = dynamic_cast<const Field<FloatTab> *> (&indicatrice);

  assert(indic_ptr->data_.dimension(0)==reactorPtr()->domain().nb_elements());
  assert(indic_ptr->data_.dimension(1)==1);

  vector<double> component_flux(nb_of_boundary_faces);

  switch(shape)
    {      
    case 1 : //VDF case : to modify

      for (int i=0; i<nb_of_boundary_faces; i++)
	{
	  bool is_liquid_present = true;
	  const int neighbor1 = faces_to_elements(boundary_faces[i],0);
	  const int neighbor2 = faces_to_elements(boundary_faces[i],1);
	  if (neighbor1!=-1) is_liquid_present &= (indic_ptr->data_(neighbor1,0)==1);
	  if (neighbor2!=-1) is_liquid_present &= (indic_ptr->data_(neighbor2,0)==1);

	  if (is_liquid_present)
	    for (int j=0; j<dimension; j++)
	      if ( fabs(exterior_normal_surface(i,j))>0)
		component_flux[i]=latafld_ptr->data_(boundary_faces[i],0)*exterior_normal_surface(i,j);
	}

      break;

    case 2 : case 3 : //VEF case

      for (int i=0; i<nb_of_boundary_faces; i++)
	{
	  bool is_liquid_present = true;
	  const int neighbor1 = faces_to_elements(boundary_faces[i],0);
	  const int neighbor2 = faces_to_elements(boundary_faces[i],1);
	  if (neighbor1!=-1) is_liquid_present &= (indic_ptr->data_(neighbor1,0)==1);
	  if (neighbor2!=-1) is_liquid_present &= (indic_ptr->data_(neighbor2,0)==1);

	  if (is_liquid_present)
	    for (int j=0; j<shape; j++)
	      component_flux[i]+=latafld_ptr->data_(boundary_faces[i],j)*exterior_normal_surface(i,j);
	}
 
      break;

    default :

      cerr << "Error in physics_reactor::component_flux()" << endl;
      cerr << "This case does not exist" << endl;
      cerr << "Exiting programm." << endl;
      exit(-1);

      break;

    }//end switch

  return component_flux; 
}

/* Initialize the class physics_reactor when already build */
/* with default constructor */
void physics_reactor::build(reactor& Reactor)
{
  //Fill attribute *reactor_
  reactor_=&Reactor;
  assert(reactorPtr()->is_defined());
  assert(reactorPtr()->is_geometry_defined());
  
  //Build mean of some components
  if (reactorPtr()->is_post_treatment_defined()) build_reactor_components();
}

bool physics_reactor::build_reactor_components()
{
  //Calculation of reactor_components size
  const vector<string>& calculation_names = reactorPtr()->post_treatment().names_of_special_calculation();
  const vector< vector<string> >& parameters_names = reactorPtr()->post_treatment().vector_of_parameters_names();
  const unsigned int special_size = calculation_names.size();

  assert(calculation_names.size()==parameters_names.size());

  unsigned int size_to_calculate = 0;

  for (unsigned int special_calculation=0; special_calculation<special_size; special_calculation++)
    size_to_calculate+=parameters_names[special_calculation].size();

  //Initialization of reactor_components
  reactor_components_.resize(size_to_calculate);

  unsigned int max_size = 0;
  for (unsigned int special_calculation=0; special_calculation<special_size; special_calculation++)
    {
      const string& calculation_name = calculation_names[special_calculation];
      const unsigned int parameter_size = parameters_names[special_calculation].size();

      for (unsigned int parameter=0; parameter<parameter_size; parameter++)
	{
	  const string& parameter_name = parameters_names[special_calculation][parameter];
	  const string label = extract_label(parameter_name);
	  const string parameter_name_without_label = delete_label(parameter_name);

	  reactor_components_[parameter+max_size].name_of_special_calculation() = calculation_name;
	  reactor_components_[parameter+max_size].name() = parameter_name_without_label+"_"+calculation_name+"_"+label;


	  if (calculation_name=="MOYENNE_GEOMETRIQUE")
	    {
	      const unsigned int component_place = reactorPtr()->search_component_name(parameter_name);
	      reactor_components_[parameter+max_size].values() = physics_average(component_place);

	      continue;
	    }//end if 

	  if (calculation_name=="MOYENNE_VOLUMIQUE")
	    {
	      const unsigned int component_place = reactorPtr()->search_component_name(parameter_name);
	      reactor_components_[parameter+max_size].values() = physics_volumic_average(component_place);

	      continue;
	    }//end if

	  if (calculation_name=="FLUX")
	    {
	      assert(label=="FACES");
	      const unsigned int component_place = reactorPtr()->search_component_name(parameter_name);
	      reactor_components_[parameter+max_size].values() = component_flux(component_place);

	      continue;
	    }//end if

	  else
	    {
	      cerr << "Error in build_reactor_components()" << endl;
	      cerr << "Special calculation " << calculation_name << " not coded yet" << endl;
	      cerr << "Exiting programm" << endl;
	      exit(-1);
	    }
	  
	}//end for on parameter
      
      max_size += parameters_names[special_calculation].size();

    }//end for on special_calculation
  
  assert(max_size==size_to_calculate);
  return true;
}


void physics_reactor::print(ostream& os) const
{
  os << "Printing reactor_components : " << endl;
  for (unsigned int i=0; i<reactor_components_.size(); i++)
    {
      os << "Name : " << reactor_components_[i].name() << endl;
      os << "Special calculation name : " << reactor_components_[i].name_of_special_calculation() << endl;
      os << "Values : (";
      for (unsigned int j=0; j<reactor_components_[i].values().size(); j++)
	os << reactor_components_[i].values()[j] << ",";
      os << ")" << endl;
    }
  os << endl;
}



/*******************************/
/*******************************/
/* Definition of class reactor */
/*******************************/
/*******************************/


/* Build the class reactor */
/* Fill the different attributes with the parameter given */
reactor::reactor() : geometry_reactor_(), physics_reactor_(), components_names_()
{
  domain_=NULL;
  building_criterion_=NULL;
  post_treatment_=NULL;

  number_=0;

  is_geometry_defined_=false;
  is_physics_defined_=false;
}

/* Build the class reactor */
/* Fill the different attributes with the parameter given */
reactor::reactor(const Domain& domain, vector<Field<FloatTab> >& latafields, building_criterion& criterion)
{
  domain_=&domain;
  latafields_=&latafields;
  building_criterion_=&criterion;

  assert(domain_!=NULL);
  assert(building_criterion_!=NULL);
  post_treatment_=NULL;

  //Init attribute components_names_
  build_components_names();

  //Initialization of reactor pointer 
  //of class building_criterion
  criterion.initPtr(*this);

  number_=criterion.number();

  geometry_reactor_.build(*this); 
  is_geometry_defined_=true;

  physics_reactor_.build(*this);
  is_physics_defined_=true;
} 

reactor::reactor(const Domain& domain, vector<Field<FloatTab> >& latafields, building_criterion& criterion, reactor_post_treatment& post_treatment)
{
  domain_=&domain;
  latafields_=&latafields;
  building_criterion_=&criterion;

  post_treatment_=&post_treatment;
  assert(domain_!=NULL);
  assert(building_criterion_!=NULL);
  assert(post_treatment_!=NULL);

  //Init attribute components_names_
  build_components_names();

  //Initialization of reactor pointer 
  //of class building_criterion
  criterion.initPtr(*this);

  number_=criterion.number();

  geometry_reactor_.build(*this); 
  is_geometry_defined_=true;

  physics_reactor_.build(*this);
  is_physics_defined_=true;
} 

/* Destroy the class reactor */
reactor::~reactor() 
{
  domain_=NULL;
  building_criterion_=NULL;
  post_treatment_=NULL;

  number_=0;

  components_names_.clear();

  is_geometry_defined_=false;
  is_physics_defined_=false;
}

void reactor::clear()
{
  physics_reactor_.clear();
  is_physics_defined_=false;

  geometry_reactor_.clear();
  is_geometry_defined_=false;

  number_=0;

  components_names_.clear();

  domain_=NULL;
  building_criterion_=NULL;
  post_treatment_=NULL;
}

/* Initialize the class reactor when already build */
/* with default constructor */
void reactor::build(const Domain& domain, vector<Field<FloatTab> >& latafields, building_criterion& criterion)
{
  domain_=&domain;
  latafields_=&latafields;
  building_criterion_=&criterion;
  post_treatment_=NULL;

  //Init attribute components_names_
  build_components_names();

  //Initialization of reactor pointer 
  //of class building_criterion
  criterion.initPtr(*this);

  unsigned int number =criterionPtr()->number();
  number_= number;
  
  geometry_reactor().build(*this);
  is_geometry_defined_=true;

  physics_reactor().build(*this);
  is_physics_defined_=true;
}

void reactor::build(const Domain& domain, vector<Field<FloatTab> >& latafields, building_criterion& criterion, reactor_post_treatment& post_treatment)
{
  domain_=&domain;
  latafields_=&latafields;
  building_criterion_=&criterion;
  post_treatment_=&post_treatment;

  //Init attribute components_names_
  build_components_names();

  //Initialization of reactor pointer 
  //of class building_criterion
  criterion.initPtr(*this);

  unsigned int number =criterionPtr()->number();
  number_= number;
  
  geometry_reactor().build(*this);
  is_geometry_defined_=true;

  physics_reactor().build(*this);
  is_physics_defined_=true;
}

void reactor::build_components_names()
{
  assert(domain_!=NULL);

  const vector<Field<FloatTab> >& latafields = *latafields_;
  const unsigned int components_size = latafields.size();

  for (unsigned int i=0; i<components_size; i++)
    components_names_[custom_field_name(latafields[i])]=i+1;
}

unsigned int reactor::search_component_name(const string& s) const
{
  typedef map<string, unsigned int>::const_iterator CI;
  CI is_found = components_names_.find(s);

  const unsigned int where = (is_found==components_names_.end()) ? 0 : is_found->second;
  assert(where>=0 && where< components_names_.size()+1);

  if (!where) 
    {
      cerr << "Error in reactor::search_component_name()" << endl;
      cerr << "Word " << s << " is not a known component name" << endl;
      cerr << "Known component name are the following : ";

      for (CI p=components_names_.begin(); p!=components_names_.end(); p++)
	cerr << p->first << ", ";
      cerr << endl;
      
      cerr << "Exiting program" << endl;
      exit(-1);
    }

  return where-1;
}

bool reactor::is_post_treatment_ok() const
{
  assert(domain_!=NULL);
  assert(post_treatment_!=NULL);

  const vector<string>& names_of_special_calculation = post_treatmentPtr()->names_of_special_calculation();
  const vector < vector<string> >& vector_of_parameters_names = 
    post_treatmentPtr()->vector_of_parameters_names();

  assert(vector_of_parameters_names.size()==names_of_special_calculation.size());

  bool test = true;

  const unsigned int first_size = names_of_special_calculation.size();

  for (unsigned int i=0; i<first_size; i++)
    {
      const unsigned int second_size = vector_of_parameters_names[i].size();
      
      for (unsigned int j=0; j<second_size; j++)
	{
	  const string& parameter_name = vector_of_parameters_names[i][j];

	  if (search_component_name(parameter_name)>=0) test &= true; 
	  else test &= false;

	}//end for on j

    }//end for on i

  return test;
}


void reactor::print(ostream& os, bool is_complete_data_wanted) const
{
  os << "Printing information about reactor" << endl;
  os << "Number of reactor : " << number_ << endl;

  os << "Known components names : ";
  typedef map<string, unsigned int>::const_iterator CI;
  for (CI p=components_names_.begin(); p!=components_names_.end(); p++)
    os << p->first << ", ";
  os << endl;

  os << "Printing criterion" << endl;
  os << "*************************************" << endl;
  building_criterion_->print(os);
  os << "*************************************" << endl;

  os << "Is geometry_reactor defined ? : " << is_geometry_defined_ << endl;
  os << "Printing geometry" << endl;
  os << "*************************************" << endl;
  geometry_reactor_.print(os,is_complete_data_wanted);
  os << "*************************************" << endl;

  os << "Is physics_reactor defined ? : " << is_physics_defined_ << endl;
  os << "Printing physics" << endl;
  os << "*************************************" << endl;
  physics_reactor_.print(os);
  os << "*************************************" << endl;

  os << endl;
} 




/******************************************/
/******************************************/
/* Definition of class building_criterions */
/******************************************/
/******************************************/

building_criterions::building_criterions(int n) : vector<building_criterion*>(n), number_of_reactors_(n)
{
  //Init attribute keywords_
  build_keywords();

  //Building the objects building_criterion
  for (unsigned int i=0; i<number_of_reactors_; i++)
    (*this)[i] = new building_criterion();
}

void building_criterions::build_criterions()
{
  //Building object pointed elements and their attributes
  assert(number_of_reactors_>0);

  resize(number_of_reactors_);
  for (unsigned int i=0; i<number_of_reactors_; i++)
    {
      (*this)[i] = new building_criterion();
      (*this)[i]->number()=i;
    }

  assert(types_.size()!=0);
  for (unsigned int i=0; i<number_of_reactors_; i++)
    (*this)[i]->type() = types_[0];//to modify

  if (coordinate_types_.size()==0)
    for (unsigned int i=0; i<number_of_reactors_; i++)
      (*this)[i]->coordinate_type() = "CARTESIAN";//default value
  else
    for (unsigned int i=0; i<number_of_reactors_; i++)
      (*this)[i]->coordinate_type() = coordinate_types_[0];//to modify

  assert(bounds_min_.size()==bounds_max_.size());
  unsigned int size = bounds_min_.size();
  double maximum_of_bounds_max = max(bounds_max_);
  double minimum_of_bounds_min = min(bounds_min_);
  if (size!=0) assert(size==number_of_reactors_);
  for (unsigned int i=0; i<size; i++)
    {
      (*this)[i]->build_bounds(bounds_min_[i],bounds_max_[i]);
      if (maximum_of_bounds_max==bounds_max_[i]) (*this)[i]->replacing_bound_max()=true;
      if (minimum_of_bounds_min==bounds_min_[i]) (*this)[i]->replacing_bound_min()=true;
    }

  size = vector_of_parsers_string_.size();
  if (size!=0) assert(size==number_of_reactors_);
  for (unsigned int i=0; i<size; i++)
    {
      (*this)[i]->build_parsers_string(vector_of_parsers_string_[i]);
      (*this)[i]->build_parsers();
    }
}


building_criterions::~building_criterions()
{
  //Deleting the objects building_criterion
  for (unsigned int i=0; i<number_of_reactors_; i++)
    delete (*this)[i];

  clear();

  //Deleting attributes
  number_of_reactors_=0;

  keywords_.clear();
  types_.clear();
  coordinate_types_.clear();
  vector_of_parsers_string_.clear();
  bounds_min_.clear();
  bounds_max_.clear();
}


void building_criterions::clear_criterions()
{
  //Deleting the objects building_criterion
  for (unsigned int i=0; i<number_of_reactors_; i++)
    (*this)[i]->clear();

  //Deleting attributes
  number_of_reactors_=0;

  keywords_.clear();
  types_.clear();
  coordinate_types_.clear();
  vector_of_parsers_string_.clear();
  bounds_min_.clear();
  bounds_max_.clear();
}

void building_criterions::build_keywords()
{
  keywords_["TYPE"]=1;
  keywords_["NUMBER_OF_REACTORS"]=2;
  keywords_["#"]=3;
  keywords_["GEOMETRIC"]=4; //Must be used only when keyword "TYPE" has been detected
  keywords_["MIXTE"]=5; //Must be used only when keyword "TYPE" has been detected
  keywords_["COORDINATE"]=6;
  keywords_["CARTESIAN"]=7; //Must be used only when keyword "COORDINATE" has been detected
  keywords_["CYLINDRICAL"]=8; //Must be used only when keyword "COORDINATE" has been detected
  keywords_["SPHERICAL"]=9; //Must be used only when keyword "COORDINATE" has been detected
}

unsigned int building_criterions::search_keyword(const string& s) const
{
  typedef map<string, unsigned int>::const_iterator CI;
  CI is_found = keywords_.find(s);

  const unsigned int where = (is_found==keywords_.end()) ? 0 : is_found->second;
  assert(where>=0 && where< keywords_.size()+1);

  if (!where) 
    {
      cerr << "Error in building_criterions::search_keyword()" << endl;
      cerr << "Word " << s << " is not a known keyword" << endl;
      cerr << "Known keywords are the following : ";
      
      for (CI p=keywords_.begin(); p!=keywords_.end(); p++)
	cerr << p->first << ", ";
      cerr << endl;
      
      cerr << "Exiting program" << endl;
      exit(-1);
    }

  return where;
}

unsigned int building_criterions::search_keyword_without_stop(const string& s) const
{
  typedef map<string, unsigned int>::const_iterator CI;
  CI is_found = keywords_.find(s);

  const unsigned int where = (is_found==keywords_.end()) ? 0 : is_found->second;
  assert(where>=0 && where< keywords_.size()+1);

  return where;
}




void building_criterions::skip(istream& is, string s)
{
  string read_word;

  is >> read_word;
  while (read_word!=s)  is >> read_word;
}

void building_criterions::read_type(istream& is)
{
  assert(number_of_reactors_>0);

  string read_word;
  unsigned int size = 0;

  is >> read_word;
  types_.push_back( to_upper(read_word) );
  const unsigned int where_in_keywords = search_keyword_without_stop(read_word);

  //Reading of data
  is >> read_word;
  if (read_word!="{")
    {
      cerr << "Error in  building_criterions::read_data()" << endl;
      cerr << "You forget a parenthesis {" << endl;
      cerr << "Exiting programm" << endl;
      exit(-1);
    }
  
  switch(where_in_keywords)
    {
    case 4 :

      //Resizing attribute vector_of_parsers_string_
      vector_of_parsers_string_.reserve(number_of_reactors_);

      while(next_character(is)!='}')
	{
	  read_parsers_data(is);
	  size++;
	}

      if (size!=number_of_reactors_)
	{
	  cerr << "Error in building_criterions::read_type()" << endl;
	  cerr << "You try to build " << number_of_reactors_ << " reactors" << endl;
	  cerr << "But you only have defined " << size << " reactors building criterions" << endl;
	  cerr << "Exiting programm" << endl;
	  exit(-1);
	}

      assert(vector_of_parsers_string_.size()==number_of_reactors_);

      break;

    case 5 :

      cerr << "Not coded yet" << endl;
      cerr << "Exiting programm" << endl;
      exit(-1);
      break;

    default :

      //Resizing attributes bounds_min_ and bounds_max_
      bounds_min_.reserve(number_of_reactors_);
      bounds_max_.reserve(number_of_reactors_);

      while (next_character(is)!='}')
	{
	  read_bounds(is);
	  size++;
	}

      if (size!=number_of_reactors_)
	{
	  cerr << "Error in building_criterions::read_type()" << endl;
	  cerr << "You try to build " << number_of_reactors_ << " reactors" << endl;
	  cerr << "But you only have defined " << size << " reactors building criterions" << endl;
	  cerr << "Exiting programm" << endl;
	  exit(-1);
	}

      assert(bounds_min_.size()==number_of_reactors_);
      assert(bounds_max_.size()==number_of_reactors_);

      break;

    }//end of switch

  is >> read_word;
  if (read_word!="}")
    {
      cerr << "Error in  building_criterions::read_data()" << endl;
      cerr << "You forget a parenthesis }" << endl;
      cerr << "Exiting programm" << endl;
      exit(-1);
    }
}

void building_criterions::read_coordinate_type(istream& is)
{
  assert(number_of_reactors_>0);

  string read_word;

  is >> read_word;
  coordinate_types_.push_back( to_upper(read_word) );
}

void building_criterions::read_parsers_data(istream& is)
{
  string read_word;
  vector<string> parsers_string;

  is >> read_word;
  if (read_word!="(")
    {
      cerr << "Error in  building_criterions::read_data()" << endl;
      cerr << "You forget a parenthesis (" << endl;
      cerr << "Exiting program" << endl;
      exit(-1);
    }

  
  while ( (is >> read_word) && (read_word!=")") )
    {
      if (read_word!="(") parsers_string.push_back(read_word);
      else 
	{
	  cerr << "Error in  building_criterions::read_data()" << endl;
	  cerr << "You forget a parenthesis )" << endl;
	  cerr << "Exiting program" << endl;
	  exit(-1);
	}

    }//end while

  vector_of_parsers_string_.push_back(parsers_string);
}

void building_criterions::read_bounds(istream& is)
{
  string read_word;
  double bound_min = 0, bound_max=0;

  is >> read_word;
  if (read_word!="(")
    {
      cerr << "Error in  building_criterions::read_bounds()" << endl;
      cerr << "You forget a parenthesis (" << endl;
      cerr << "Exiting program" << endl;
      exit(-1);
    }

  is >> bound_min >> bound_max >> read_word;

  if (read_word!=")")
    {
      cerr << "Error in  building_criterions::read_bounds()" << endl;
      cerr << "You forget a parenthesis ) or you try to impose " 
	   << "more/less than 2 bounds" << endl;
      cerr << "Exiting program" << endl;
      exit(-1);
    }

  assert(bound_min<=bound_max);

  bounds_min_.push_back(bound_min);
  bounds_max_.push_back(bound_max);
}

void building_criterions::read_number_of_reactors(istream& is)
{
  is >> number_of_reactors_;
} 

/********************************/
/********************************/
/* Definition of class reactors */
/********************************/
/********************************/

reactors::reactors(int n) : vector<reactor*>(n),  number_of_reactors_(n)  { }

reactors::reactors(const Domain& domain, vector<Field<FloatTab> >& latafields, building_criterions& criterions) 
{
  number_of_reactors_=criterions.size();

  //To create the vector
  resize(number_of_reactors_);

  //To init the different reactors
  for (unsigned int i=0; i<number_of_reactors_; i++)
    {
      assert(criterions[i]!=NULL);
      (*this)[i] = new reactor(domain,latafields,*(criterions[i]));
    }

  //Test to see if all elements of domain are in one 
  //(and only one) reactor
  unsigned int error_code=0;
  if (!are_reactors_ok(error_code))
    {
      switch(error_code)
	{
	case 0 :
	  cerr << "One reactor don't possess any element" << endl;
	  break; 

	case 1 :
	  cerr << "At least one element belongs to any reactor" << endl;
	  break;

	case 2 :
	  cerr << "At least one element belongs to more than one reactor" << endl;
	  break;

	default :
	  cerr << "Error in reactors::reactors()" << endl;
	  cerr << "Case not coded yet" << endl;
	  break;

	}//end of switch

      cerr << "Exiting programm" << endl;
      exit(-1);

    }//end of if
}

reactors::reactors(const Domain& domain, vector<Field<FloatTab> >& latafields, building_criterions& criterions, reactor_post_treatments& post_treatments)
{
  number_of_reactors_=criterions.size();

  //To create the vector
  resize(number_of_reactors_);

  //To init the different reactors
  for (unsigned int i=0; i<number_of_reactors_; i++)
    {
      assert(criterions[i]!=NULL);
      (*this)[i] = new reactor(domain,latafields,*(criterions[i]),*(post_treatments[i]));
      (*this)[i]->is_post_treatment_ok();
    }

  //Test to see if all elements of domain are in one 
  //(and only one) reactor
  unsigned int error_code = 0;
  if (!are_reactors_ok(error_code))
    {
      switch(error_code)
	{
	case 0 :
	  cerr << "One reactor don't possess any element" << endl;
	  break; 

	case 1 :
	  cerr << "At least one element belongs to any reactor" << endl;
	  break;

	case 2 :
	  cerr << "At least one element belongs to more than one reactor" << endl;
	  break;

	default :
	  cerr << "Error in reactors::reactors()" << endl;
	  cerr << "Case not coded yet" << endl;
	  break;

	}//end of switch

      cerr << "Exiting programm" << endl;
      exit(-1);

    }//end of if
}

reactors::~reactors()
{
  number_of_reactors_=0;

  //Destroying the different reactors
  for (unsigned int i=0; i<number_of_reactors_; i++)
    delete (*this)[i];

  //Destroying the object reactors
  clear();
}

void reactors::print_velocity_flux(ostream& os) const
{
  assert( (*this)[0]!=NULL );
  assert( (*this)[0]->is_defined() );

  const Domain& domain = reactorsPtr(0)->domain();

  //To compute faces that belong to 2 different reactors
  ArrOfInt face_neighbors(domain.nb_faces());

  for (unsigned int i=0; i<number_of_reactors_; i++)
    {
      assert( (*this)[i]!=NULL );
      assert( (*this)[i]->is_defined() );
      assert( (*this)[i]->is_geometry_defined() );
      assert( (*this)[i]->is_physics_defined() );

      const reactor& reactor1 = *((*this)[i]);
      const ArrOfInt& boundary_faces_reactor1 = reactor1.geometry().boundary_faces(); 
      const unsigned int nb_boundary_faces_reactor1 = reactor1.geometry().nb_boundary_faces();

      vector<double> flux(number_of_reactors_);
      vector<double> velocity_flux_reactor1(nb_boundary_faces_reactor1);
      velocity_flux_reactor1=reactor1.physics().velocity_flux();

      face_neighbors=0;
      for (unsigned int boundary_face=0; boundary_face<nb_boundary_faces_reactor1; boundary_face++)
	face_neighbors[ boundary_faces_reactor1[boundary_face] ] = boundary_face+1; //to avoid having 0
	  
      for (unsigned int j=(i+1)%number_of_reactors_; j!=i; j=(j+1)%number_of_reactors_)
	{
	  assert( (*this)[j]!=NULL );
	  assert( (*this)[j]->is_defined() );
	  assert( (*this)[j]->is_geometry_defined() );
	  assert( (*this)[j]->is_physics_defined() );
	  
	  const reactor& reactor2 = *((*this)[j]);
	  const ArrOfInt& boundary_faces_reactor2 = reactor2.geometry().boundary_faces(); 
	  const unsigned int nb_boundary_faces_reactor2 = reactor2.geometry().nb_boundary_faces();
	  
	  for (unsigned int boundary_face=0; boundary_face<nb_boundary_faces_reactor2; boundary_face++)
	    {
	      const unsigned int face1 = face_neighbors[ boundary_faces_reactor2[boundary_face] ];
	      if (face1 > 0)
		{
		  assert(face1<=nb_boundary_faces_reactor1);
 		  if ( velocity_flux_reactor1[face1-1]>0 ) flux[j]+=velocity_flux_reactor1[face1-1];
		}//end if
	      
	    }//end for on boundary_face

	}//end for on j

      if (i==0) os << "Velocity flux" << endl;
      for (unsigned int m=0; m<flux.size(); m++)
	os << flux[m] << " ";
      os << endl;

    }//end for on i
}

void reactors::print_components_fluxes(ostream& os) const
{
  assert( (*this)[0]!=NULL );
  assert( (*this)[0]->is_defined() );
  assert( (*this)[0]->is_geometry_defined() );
  assert( (*this)[0]->is_physics_defined() );

  const Domain& domain = reactorsPtr(0)->domain();
  const unsigned int nb_reactor_components = reactorsPtr(0)->physics().reactor_components().size();

  //To compute faces that belong to 2 different reactors
  ArrOfInt face_neighbors(domain.nb_faces());

  for (unsigned int component=0; component<nb_reactor_components; component++)
    {
      for (unsigned int i=0; i<number_of_reactors_; i++)
	{
	  assert( (*this)[i]!=NULL );
	  assert( (*this)[i]->is_defined() );
	  assert( (*this)[i]->is_geometry_defined() );
	  assert( (*this)[i]->is_physics_defined() );

	  const reactor& reactor1 = *((*this)[i]);
	  const geometry_reactor& geometry1 = reactor1.geometry();
	  const physics_reactor& physics1 = reactor1.physics();
	  assert(physics1.reactor_components().size()==nb_reactor_components);

	  const reactor_component& reactor_component1 = physics1.reactor_components()[component];
	  const ArrOfInt& boundary_faces_reactor1 = geometry1.boundary_faces(); 
	  const unsigned int nb_boundary_faces_reactor1 = geometry1.nb_boundary_faces();
	  
	  vector<double> flux(number_of_reactors_);

	  //To work only on FLUX
	  if (reactor_component1.name_of_special_calculation()!="FLUX") break; //to go to next component

	  face_neighbors=0;
	  for (unsigned int boundary_face=0; boundary_face<nb_boundary_faces_reactor1; boundary_face++)
	    face_neighbors[ boundary_faces_reactor1[boundary_face] ] = boundary_face+1; //to avoid having 0

	  for (unsigned int j=(i+1)%(number_of_reactors_); j!=i; j=(j+1)%number_of_reactors_)
	    {
	      assert( (*this)[j]!=NULL );
	      assert( (*this)[j]->is_defined() );
	      assert( (*this)[j]->is_geometry_defined() );
	      assert( (*this)[j]->is_physics_defined() );
	      
	      const reactor& reactor2 = *((*this)[j]);
	      const geometry_reactor& geometry2 = reactor2.geometry();
	      const physics_reactor& physics2 = reactor2.physics();
	      assert(physics2.reactor_components().size()==nb_reactor_components);

	      const reactor_component& reactor_component2 = physics2.reactor_components()[component];
	      const ArrOfInt& boundary_faces_reactor2 = geometry2.boundary_faces(); 
	      const unsigned int nb_boundary_faces_reactor2 = geometry2.nb_boundary_faces();
	      
	      //Are we working on the same unknown ?
	      if (reactor_component1.name()!=reactor_component2.name())
	      {
		      Cerr << "Different unknown in reactors::print_components_fluxes: " << reactor_component1.name() << " " << reactor_component2.name() << finl;
		      exit(-1);
	      }
	    
	      for (unsigned int boundary_face=0; boundary_face<nb_boundary_faces_reactor2; boundary_face++)
		{
		  const unsigned int face1 = face_neighbors[ boundary_faces_reactor2[boundary_face] ];
		  if ( face1 > 0 )
		    {
		      assert(face1<=nb_boundary_faces_reactor1);
		      if ( reactor_component1.values()[face1-1]>0 ) flux[j]+=reactor_component1.values()[face1-1];
		    }//end if
		  
		}//end for on boundary_face
  
	    }//end for on j
	  
	  if (i==0) os << "Name of component : " << reactor_component1.name() << endl;
	  for (unsigned int m=0; m<flux.size(); m++)
	    os << flux[m] << " ";
	  os << endl;

	}//end for on i

    }//end for on component
}


//Error_code = 0 if and only if at least one reactor don't possess any element
//Error_code = 1 if and only if at least one element belongs to any reactor
//Error_code = 2 if and only if at least one element belongs to more thant one reactor
bool reactors::are_reactors_ok(unsigned int& error_code) const
{
  //Test 
  for (unsigned int reactor=0; reactor<number_of_reactors_; reactor++)
    {
      const unsigned int next_reactor = (reactor+1)%number_of_reactors_;
      if (reactorsPtr(reactor)->domainPtr()!=reactorsPtr(next_reactor)->domainPtr())
      {
	 Cerr << "Error in reactors::are_reactors_ok" << finl;
         exit(-1);
      }
    }

  const int nb_elements = reactorsPtr(0)->domain().nb_elements();
  ArrOfInt nb_reactors_for_element(nb_elements);
  nb_reactors_for_element = 0;

  //To see if one reactor have no element
  for (unsigned int reactor=0; reactor<number_of_reactors_; reactor++) 
    if (reactorsPtr(reactor)->geometry().nb_elements()==0) { error_code=0; return false; }

  //To see if one element belongs to more than one reactor
  for (unsigned int reactor=0; reactor<number_of_reactors_; reactor++) 
    for (int element=0; element<nb_elements; element++)
      if (reactorsPtr(reactor)->geometry().is_element(element)) 
	nb_reactors_for_element[element]+=1;

  for (int element=0; element<nb_elements; element++)
    {
      if (nb_reactors_for_element[element]==0)
	{ error_code=1; return false; }

      if (nb_reactors_for_element[element]>1)
	{ error_code=2; return false; }
    }

  return true;
}

void reactors::print(ostream& os, bool is_complete_data_wanted) const
{
  for (unsigned int i=0; i<number_of_reactors_; i++)
    (*this)[i]->print(os,is_complete_data_wanted);
}



void reactors::write_components(LataFilter & filter, const int binary_out, vector<Field<FloatTab> >& new_fields)
{
  Nom new_prefix ="";
  Nom new_basename="new_post";

  bool cfait=false;

  LataDBDataType default_int;
  if (binary_out)
    default_int.msb_ = LataDBDataType::machine_msb_;
  else
    default_int.msb_ = LataDBDataType::ASCII;
  default_int.type_ = LataDBDataType::INT32;
  default_int.array_index_ = LataDBDataType::F_INDEXING; //use_fortran_indexing
  default_int.data_ordering_ = LataDBDataType::C_ORDERING; //!use_fortran_data_ordering
  default_int.fortran_bloc_markers_ = LataDBDataType::BLOC_MARKERS_SINGLE_WRITE; //fortran_blocs
  default_int.bloc_marker_type_ = LataDBDataType::INT32;
  default_int.file_offset_ = 0;

  LataWriter lata_writer;
  lata_writer.init_file(new_prefix, new_basename, default_int, LataDBDataType::REAL32);
  
  const entier ntimesteps = filter.get_nb_timesteps();
  
  // Build list of geometries to export
  Noms geometries;
  {
    Noms names = filter.get_exportable_geometry_names();
    for (entier i = 0; i < names.size(); i++)
	geometries.add(names[i]);
  }
  
  for (entier tstep = 0; tstep < ntimesteps; tstep++)
  {
      if (tstep > 0) {
	// Real timestep (timestep 0 contains global definitions)
	double t = filter.get_timestep(tstep);
	lata_writer.write_time(t);
      }

      // Find geometry that is not interface
      entier i = 0; 
      for (;i<geometries.size();i++)
	{
	  const LataGeometryMetaData & geom_metadata = filter.get_geometry_metadata(geometries[i]);
	  if (!geom_metadata.dynamic_)
	    break;
	}

	{
	  const LataGeometryMetaData & md = filter.get_geometry_metadata(geometries[i]);
	  if ((md.dynamic_ && tstep > 0) || ((!md.dynamic_) && tstep == 0)) 
	    {
	      // Output geometry data defined at this timestep:
	      const Domain & domain = filter.get_geometry(Domain_Id(md.internal_name_, tstep, -1));
	      lata_writer.write_geometry(domain);
	      filter.release_geometry(domain);
	    }
	  if (tstep > 0) {
	    // Output fields
	    LataVector<Field_UName> field_names = filter.get_exportable_field_unames(md.internal_name_);
	    const entier n = field_names.size();

            const entier nnn= new_fields.size();

	    for (entier iii = 0; iii < nnn; iii++)
	    {

		// for each new field

		cfait=false;

		for (entier j = 0; j < n; j++)
		{
		const LataField_base & field = filter.get_field(Field_Id(field_names[j], tstep, -1));
		const Field<FloatTab> * fld_ptr = dynamic_cast<const Field<FloatTab> *> (&field);

		if ((!cfait) && (tstep == (ntimesteps-1)))

		{
		    Field<FloatTab> pole(*fld_ptr);
		    ArrOfFloat & data_array=pole.data_;

		    const Field<FloatTab> * latafld_ptr = dynamic_cast<const Field<FloatTab> *> (&(new_fields[iii]));
		    const ArrOfFloat & data_latachamps=latafld_ptr->data_;

		    if ((pole.nature_==latafld_ptr->nature_) &&
			(pole.data_.dimension(0)==latafld_ptr->data_.dimension(0)) &&
			(pole.data_.dimension(1)==latafld_ptr->data_.dimension(1)) &&
			(pole.localisation_==latafld_ptr->localisation_))
			{

			cfait=true;

			const entier sz=data_latachamps.size_array();
			for (entier k=0; k<sz; k++)
			   data_array[k]=data_latachamps[k];

			pole.id_.uname_.set_field_name(latafld_ptr->component_names_[0]);
			
			lata_writer.write_component(pole);

			}
		}
		}

	    } //iii

	  }
	}
    }

  lata_writer.finish();
}



/***********************************************/
/***********************************************/
/* Definition of class reactor_post_treatment */
/***********************************************/
/***********************************************/


reactor_post_treatment::reactor_post_treatment() 
{ 
  //  reactor_=NULL; 

  //Function that build map<string, unsigned int> attributes
  build_special_calculation_keywords();
}


void reactor_post_treatment::build_special_calculation_keywords()
{
  special_calculation_keywords_["MOYENNE_GEOMETRIQUE"]=1;
  special_calculation_keywords_["MOYENNE_VOLUMIQUE"]=2;
  special_calculation_keywords_["FLUX"]=3;
}


reactor_post_treatment::~reactor_post_treatment() 
{ 
  //  reactor_=NULL; 

  names_of_special_calculation_.clear();
  vector_of_parameters_names_.clear();
  special_calculation_keywords_.clear();
}


void reactor_post_treatment::clear()
{
//   reactor_=NULL; 

  names_of_special_calculation_.clear();
  vector_of_parameters_names_.clear();
  special_calculation_keywords_.clear();
}


unsigned int reactor_post_treatment::search_special_calculation(const string& s) const
{
  typedef map<string, unsigned int>::const_iterator CI;
  CI is_found = special_calculation_keywords_.find(s);

  const unsigned int where = (is_found==special_calculation_keywords_.end()) ? 0 : is_found->second;
  assert(where>=0 && where< special_calculation_keywords_.size()+1);

  if (!where) 
    {
      cerr << "Error in reactor_post_treatment::search_special_calculation()" << endl;
      cerr << "Word " << s << " is not a known special calculation" << endl;
      cerr << "Known special calculations are the following : ";
      
      for (CI p=special_calculation_keywords_.begin(); p!=special_calculation_keywords_.end(); p++)
	cerr << p->first << ", ";
      cerr << endl;
      
      cerr << "Exiting program" << endl;
      exit(-1);
    }

  return where-1;
}


bool reactor_post_treatment::is_special_calculation_defined(const string& s) const
{
  typedef map<string, unsigned int>::const_iterator CI;
  CI is_found = special_calculation_keywords_.find(s);
  
  const unsigned int where = (is_found==special_calculation_keywords_.end()) ? 0 : is_found->second;
  assert(where>=0 && where< special_calculation_keywords_.size()+1);

  return (where>0) ? true : false;
}


void reactor_post_treatment::print(ostream& os) const
{
//   os << "Number of building criterion : " << number_ << endl;

  os << "Names of special calculation : " ;
  for (unsigned int i=0; i<names_of_special_calculation_.size(); i++)
    os << names_of_special_calculation_[i] << "; ";
  os << endl;

  os << "Functions that overcome special calculation : " ;
  for (unsigned int i=0; i<vector_of_parameters_names_.size(); i++)
    {
      os << "{ ";
      for (unsigned int j=0; j<vector_of_parameters_names_[i].size(); j++)
	os << vector_of_parameters_names_[i][j] << ", ";
      os << "} ;; "; 
    }
  os << endl;
  
  os << "Known special_calculation_keywords : ";
  typedef map<string, unsigned int>::const_iterator CI;
  for (CI p=special_calculation_keywords_.begin(); p!=special_calculation_keywords_.end(); p++)
    os << p->first << ", ";
  os << endl;
}


void reactor_post_treatment::build_names_of_special_calculation(const vector<string>& special_calculation)
{
  const unsigned int size = special_calculation.size();
  names_of_special_calculation_.resize(size);

  for (unsigned int i=0; i<size; i++)
    names_of_special_calculation_[i]=special_calculation[i];

  //Very important
  synchronize_keywords_with_special_calculation();
}

void reactor_post_treatment::build_parameters_names(const vector< vector<string> >& parameters_names)
{
  const unsigned int size = parameters_names.size();
  vector_of_parameters_names_.resize(size);

  for (unsigned int i=0; i<size; i++)
    {
      const unsigned int second_size = parameters_names[i].size(); 
      vector_of_parameters_names_[i].resize(second_size);
      
      for (unsigned int j=0; j<second_size; j++)
	vector_of_parameters_names_[i][j]=parameters_names[i][j];
      
    }//end for on i
}


void reactor_post_treatment::synchronize_keywords_with_special_calculation()
{
  assert(names_of_special_calculation_.size()!=0);
  typedef map<string, unsigned int>::const_iterator CI;

  const unsigned int size = names_of_special_calculation_.size();

  //Is special_calculation_keywords given throuhg file already known ?
  for (unsigned int i=0; i<size; i++)
    {
      const string& special_calculation = names_of_special_calculation_[i];
      CI p=special_calculation_keywords_.find(special_calculation);

      if (p==special_calculation_keywords_.end())
	{
	  cerr << "Error in reactor_post_treatment::synchronize_keywords_with_special_calculation()"
	       << endl;
	  cerr << "Keyword " << special_calculation << " is not a known special calculation"
	       << endl;
	  
	  cerr << "Known special calculations are : ";
	  for (CI q=special_calculation_keywords_.begin(); q!=special_calculation_keywords_.end(); q++)
	    cerr << q->first << ",";
	  cerr << endl;

	  cerr << "Exiting program" << endl;
	  exit(-1);
	}
    }

  //Update special_calculation_keywords
  special_calculation_keywords_.clear();

  for(unsigned int i=0; i<size; i++)
    {
      const string& special_calculation = names_of_special_calculation_[i]; 
      special_calculation_keywords_[special_calculation]=i+1;
    }
}


/***********************************************/
/***********************************************/
/* Definition of class reactor_post_treatments */
/***********************************************/
/***********************************************/


reactor_post_treatments::reactor_post_treatments(int n) : vector<reactor_post_treatment*>(n)
{
  //Init attribute keywords_
  build_keywords();

  //Building the objects reactor_post_treatment
  for (unsigned int i=0; i<size(); i++)
    (*this)[i] = new reactor_post_treatment();
}

void reactor_post_treatments::build_post_treatments()
{
  //Building object pointed elements and their attributes
  assert(size()>0);

  //Special attributes for post-treatment
  for (unsigned int i=0; i<size(); i++)
    {
      (*this)[i]->build_names_of_special_calculation(names_of_special_calculation_);
      (*this)[i]->build_parameters_names(vector_of_parameters_names_);
    }
}


reactor_post_treatments::~reactor_post_treatments()
{
  //Deleting the objects reactor_post_treatment
  for (unsigned int i=0; i<size(); i++)
    delete (*this)[i];

  clear();

  //Deleting attributes
  keywords_.clear();
  names_of_special_calculation_.clear();
  vector_of_parameters_names_.clear();
}


void reactor_post_treatments::clear_post_treatments()
{
  //Deleting the objects reactor_post_treatment
  for (unsigned int i=0; i<size(); i++)
    (*this)[i]->clear();

  //Deleting attributes
  keywords_.clear();
  names_of_special_calculation_.clear();
  vector_of_parameters_names_.clear();
}

void reactor_post_treatments::build_keywords()
{
  keywords_["NAME"]=1;
  keywords_["#"]=2;
  keywords_["LABEL"]=3; //Must be used only when keyword "NAME" has been detected
  keywords_["PARAMETERS"]=4; //Must be used only when keyword "NAME" has been detected
}

unsigned int reactor_post_treatments::search_keyword(const string& s) const
{
  typedef map<string, unsigned int>::const_iterator CI;
  CI is_found = keywords_.find(s);

  const unsigned int where = (is_found==keywords_.end()) ? 0 : is_found->second;
  assert(where>=0 && where< keywords_.size()+1);

  if (!where) 
    {
      cerr << "Error in reactor_post_treatments::search_keyword()" << endl;
      cerr << "Word " << s << " is not a known keyword" << endl;
      cerr << "Known keywords are the following : ";
      
      for (CI p=keywords_.begin(); p!=keywords_.end(); p++)
	cerr << p->first << ", ";
      cerr << endl;
      
      cerr << "Exiting program" << endl;
      exit(-1);
    }

  return where;
}

unsigned int reactor_post_treatments::search_keyword_without_stop(const string& s) const
{
  typedef map<string, unsigned int>::const_iterator CI;
  CI is_found = keywords_.find(s);

  const unsigned int where = (is_found==keywords_.end()) ? 0 : is_found->second;
  assert(where>=0 && where< keywords_.size()+1);

  return where;
}

void reactor_post_treatments::skip(istream& is, string s)
{
  string read_word;

  is >> read_word;
  while (read_word!=s)  is >> read_word;
}

void reactor_post_treatments::read_name_of_special_calculation(istream& is)
{
  string read_word;

  is >> read_word;
  names_of_special_calculation_.push_back( to_upper(read_word) );
}

void reactor_post_treatments::read_special_calculation_data(istream& is)
{
  vector<string> local_labels;
  vector < vector<string> > local_parameters;

  string read_word;

  is >> read_word;
  if (read_word!="{")
    {
      cerr << "Error in reactor_post_treatments::read_special_calculation_data()" << endl;
      cerr << "You forget this parenthesis {" << endl;
      cerr << "Exiting programm" << endl;
      exit(-1);
    }

  while (is >> read_word && read_word!="}" )
    {
      const unsigned int where_in_keywords = search_keyword( to_upper(read_word) );

      switch(where_in_keywords)
	{
	case 2 : //COMMENTARY 

	  skip(is,"#");
	  break;

	case 3 : //LABEL of parameters which are applied special calculation
	  
	  read_label(is,local_labels);
	  break;
  
	case 4 : //PARAMETERS which are applied special calculation
	  
	  read_parameters(is,local_parameters);
	  break;

	default :

	  cerr << "Error in reactor_post_treatments::read_special_calculation_data()" << endl;
	  cerr << "Keyword " << read_word << " must not be used here" << endl;
	  cerr << "Exiting program" << endl;
	  exit(-1);
	  
	  break;

	}//end of switch

    }//end of while

  //Renaming of local_parameters with given labels
  assert(local_labels.size()==local_parameters.size());
  vector<string> local_parameters_concatenated;

  for (unsigned int label=0; label<local_labels.size(); label++)
    {
      for (unsigned int parameter=0; parameter<local_parameters[label].size(); parameter++)
	{
	  const string& string_label = "_"+local_labels[label];
	  const string& parameter_name = local_parameters[label][parameter]+string_label;
	  local_parameters_concatenated.push_back(parameter_name);
	}//end for on parameter

    }//end for on label

  vector_of_parameters_names_.push_back(local_parameters_concatenated);      
} 

void reactor_post_treatments::read_parameters(istream& is, vector< vector<string> >& local_parameters)
{
  string read_word;
  vector<string> parameters;

  is >> read_word;
  assert(read_word=="(");

  while ( (is >> read_word) && (read_word!=")") ) 
    parameters.push_back( to_upper(read_word) ); 

  local_parameters.push_back(parameters);
}

void reactor_post_treatments::read_label(istream& is, vector<string>& local_labels)
{
  string read_word;
  
  is >> read_word;
  local_labels.push_back( to_upper(read_word) );
}

class Lata2dxOptions: public LataOptions
{
public:
  Lata2dxOptions();
  void describe();
  entier parse_option(const Nom & s);
  void parse_options(int argc, char **argv);

  // Input filter: when reading the input file, timesteps, domains
  //  and components not mentionned in these vectors are not read.
  // If empty list, write all items.
  ArrOfInt input_timesteps_filter;
  Noms     input_domains_filter;
  Noms     input_components_filter;

  // Output file will be written in binary format
  int binary_out;
  Nom output_filename;

};

Lata2dxOptions::Lata2dxOptions()
{
  binary_out = 1;
}

void Lata2dxOptions::describe()
{
  cerr << "Usage : lata2dx_reactor input_file_name" << endl;
  cerr << " [timestep=n]" << endl;
  cerr << " [domain=name]" << endl;
  cerr << " [binout|asciiout]" << endl;
  cerr << endl;
  cerr << "Database filtering options" << endl;
  cerr << " Filtering of input database, applied to the input file before data processing:" << endl;
  cerr << "  timestep=n      : Load the specified timestep (mandatory if several timesteps in the file)" << endl;
  cerr << "  domain=label    : Read the specified domain from the input file." << endl;
  cerr << "                    (mandatory if input file contains several domains)" << endl;
  cerr << "  component=label : Loads only specified components (speed up processing if some components are not needed)" << endl;
  cerr << endl;
  cerr << "Output formatting options: define the format of output data" << endl;
  cerr << " binout|asciiout  : default is binary" << endl;
  cerr << endl;
  cerr << "verbosity=n : The greater n, the more debugging info you get." << endl;
  cerr << endl;
  cerr << "This executable will need the following files:" << endl;
  cerr << "  ./component_builder.txt" << endl;
  cerr << "  ./criterions.txt" << endl;
  cerr << "  ./post_treatment.txt" << endl;
  cerr << endl;
  // Do not describe Filter options... (not parsed)
}

entier Lata2dxOptions::parse_option(const Nom & s)
{
  if (s == "binout") {
    binary_out = 1;
  } else if (s == "asciiout") {
    binary_out = 0;
  } else if (s.debute_par("writelata=")) {
    output_filename = read_string_opt(s);
  } else if (s.debute_par("timestep=")) {
    // Internally, first timestep is 1.
    input_timesteps_filter.set_smart_resize(1);
    input_timesteps_filter.append_array(read_int_opt(s)+1);    
  } else if (s.debute_par("domain=")) {
    input_domains_filter.add(read_string_opt(s));
  } else if (s.debute_par("component=")) {
    input_components_filter.add(read_string_opt(s));
  } 
  // do not parse LataFilter options (use the lata2dx tool if needed)
  return 1;
}

void Lata2dxOptions::parse_options(int argc, char **argv) 
{
  if (argc < 2) {
    describe();
    exit(-1);
  }
  extract_path_basename(argv[1], path_prefix, basename);

  Journal(3) << "Basename : " << basename << endl
	     << "Prefix   : " << path_prefix << endl;

  for (int i = 2; i < argc; i++) {
    if (!parse_option(argv[i])) {
      cerr << "Lata2dx : unknown command line option: " << argv[i] << endl;
      describe();
      exit(-1);
    }
  }
}

int main(int argc, char **argv) 
{
  Lata2dxOptions opt;
  Journal(8) << "command line ";
  for (int i=0; i<argc; i++)
    Journal(8) << argv[i] << " ";
  Journal(8) << endl;

  // force some options:
  opt.export_fields_at_faces_ = true;
  //opt.reconnect = true;
  //opt.reconnect_tolerance = 1e-9; 

  opt.parse_options(argc, argv);
  read_any_format_options(argv[1], opt);

  // Reading input file into this
  LataDB lata_db_input;
  read_any_format(argv[1], opt.path_prefix, lata_db_input);
    
  // Apply input filtering:
  LataDB lata_db;
  LataDB_apply_input_filter(lata_db_input, lata_db, 
			    opt.input_timesteps_filter,
			    opt.input_domains_filter,
			    opt.input_components_filter);

  LataFilter filter;
  filter.initialize(opt, lata_db);

  /* Opening files */
  ifstream infile_builder("./component_builder.txt",ios::in);
  ifstream infile_criterions("./criterions.txt",ios::in);
  ifstream infile_post_treatement("./post_treatment.txt",ios::in);
  ofstream outfile_flux("./flux.txt",ios::out);
  ofstream outfile_reactors("./reactors_data.txt",ios::out);

  if (!infile_builder)
    { 
      cerr << "Error main() : file component_builder.txt not found" << endl; 
      exit(-1);
    }

  if (!infile_criterions)
    { 
      cerr << "Error main() : file criterions.txt not found" << endl; 
      exit(-1);
    }

  if (!infile_post_treatement)
    { 
      cerr << "Error main() : file post_treatment.txt not found" << endl; 
      exit(-1);
    }

  if (!outfile_flux)
    { 
      cerr << "Error main() : file flux.txt not found" << endl; 
      exit(-1);
    }
      
  if (!outfile_reactors)
    { 
      cerr << "Error main() : file reactors_data.txt not found" << endl; 
      exit(-1);
    }

  if (filter.get_nb_timesteps() != 2) {
    int n = lata_db_input.nb_timesteps() - 1; // number of real timesteps (timestep 0 contains global domains)
    cerr << "Error: you must select timestep=N "
	 << "with 0 <= N < " << n << endl;
    exit(-1);
  }


  /* Creating objects */
  Noms geoms = filter.get_exportable_geometry_names();
  // Search for the fixed volume mesh (ignore interfaces)
  Nom domain_name(""); 
  {
    int error = 0;
    for (int j = 0; j < geoms.size(); j++) {
      const LataGeometryMetaData & md = filter.get_geometry_metadata(geoms[j]);
      if (md.dynamic_) 
	continue; // ignore this domain (interfaces)
      if (domain_name != "") {
	domain_name += " ";
	error = 1;
      }
      domain_name += md.internal_name_;
    }
    if (error) {
      cerr << "Error : Found several domains in the lata file, you must choose one with domain=DOMAIN_NAME" << endl
	   << " Available domains: " << domain_name << endl;
      exit(-1);
    }
    if (domain_name == "") {
      cerr << "Error: No fixed mesh found in the input file !" << endl;
      exit(-1);
    }
  }
  Domain_Id id(domain_name, 0, -1 /* load all blocks */);
  const Domain & domain = filter.get_geometry(id);
  check_data(filter,id);

  LataVector<Field_UName> field_names = filter.get_exportable_field_unames(domain_name);

  const entier n = field_names.size();

  vector< Field<FloatTab> > provisoire;

  for (entier j = 0; j < n; j++) {
    const LataField_base & field = filter.get_field(Field_Id(field_names[j], 1 /* timestep */, -1 /* load all blocks */));
    const Field<FloatTab> * fld_ptr = dynamic_cast<const Field<FloatTab> *> (&field);
    provisoire.push_back(*fld_ptr);
    filter.release_field(field);
  }

  vector< Field<FloatTab> > & latachamps = provisoire;

  component_builder builder(domain,latachamps);
  building_criterions criterions;

  /* Reading files */
  infile_builder >> builder;
  infile_criterions >> criterions;

  /* Building objects */
  builder.build_components();

  criterions.build_criterions();
  reactor_post_treatments post_treatments(criterions.size());
  infile_post_treatement >> post_treatments;
  post_treatments.build_post_treatments();
  reactors Reactors(domain,latachamps,criterions,post_treatments);
  Reactors.print_velocity_flux(outfile_flux);
  Reactors.print_components_fluxes(outfile_flux);
  builder.build_components(Reactors);
  Reactors.print(outfile_reactors);

  int n_new=latachamps.size(); // Number of new components to be written
  // These new components are
  // - the components defined in component_builder.txt and
  // - one component REACTORS_NUMBERS built after the reactors definition in criterions.txt

  vector< Field<FloatTab> > latafields_a_postraiter;

  for (int iii=n; iii<n_new; iii++)
    {
      latafields_a_postraiter.push_back(latachamps[iii]);
    }

  Reactors.write_components(filter, opt.binary_out, latafields_a_postraiter);
}
