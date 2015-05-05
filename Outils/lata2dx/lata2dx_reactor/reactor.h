#ifndef _REACTOR_H_
#define _REACTOR_H_

#include <iostream>
#include <map>

#include "LataStructures.h"
#include "ArrOfInt.h"
#include "ArrOfDouble.h"
#include "ArrOfFloat.h"
#include "ArrOfBit.h"
#include "Parser.h"
#include "reactor_component.h"
#include <LataFilter.h>
#include <LataWriter.h>


using namespace std;

class Domain;
class LataField_base;

class reactor;
class building_criterion;
class reactor_post_treatment;
class reactor_post_treatments;

/****************************************/
/****************************************/
/* Definition of class geometry_reactor */
/****************************************/
/****************************************/



/* This class describes the geometry of a RPA and its main functions*/
/* We thus have access and i/o functions */
class geometry_reactor {

  friend class reactor;

  /* i/o functions */
  friend istream& operator>>(istream&, geometry_reactor&);
  friend ostream& operator<<(ostream&, const geometry_reactor&);




  /* Access functions */
 public :

   //Functions that give read only access to geometry_reactor attributes
   const ArrOfBit& elements() const;
   const ArrOfInt& faces() const;
   const ArrOfInt& vertices() const;
   const ArrOfInt& boundary_faces() const;
   const DoubleTab& exterior_normal_surface() const;

   unsigned int nb_elements() const;
   unsigned int nb_faces() const;
   unsigned int nb_vertices() const;
   unsigned int nb_boundary_faces() const;
   unsigned int global_number(unsigned int) const;
   unsigned int local_number(unsigned int) const;

   bool is_element(const int&) const;
   bool is_face(const int&) const;
   bool is_vertex(const int&) const;
   bool is_boundary_face(const int&) const;

   double volume() const;
   double volume_calculation() const;
   double volume_calculation(const int&) const;
   vector<double> gravity_center(int) const;

   //Function that prints objet geometry_reactor in ASCII
   void print(ostream&, bool is_complete_data_wanted=false) const;

/*  const reactor& reactor() const; */


  /* Functions modifying attributes of class reactor */
 protected :

   //Functions that initialize or destroy geometry_reactor object
  geometry_reactor();
  ~geometry_reactor();
  geometry_reactor(reactor&);

  void build(reactor&);
  void clear();

  //Functions that give a read only access to geometry_reactor
  //But used only in class geometry_reactor
  //Very useful when using functions on constant objects
   const reactor* reactorPtr() const;

  //Functions that modify geometry_reactor attributes
   ArrOfBit& elements();
   ArrOfInt& faces();
   ArrOfInt& vertices();
   ArrOfInt& boundary_faces();
   DoubleTab& exterior_normal_surface();

   unsigned int& nb_elements();
   unsigned int& nb_faces();
   unsigned int& nb_vertices();
   unsigned int& nb_boundary_faces();

   double& volume();



  /* Attributes */
 private :

  //Copies are forbidden
  geometry_reactor(const geometry_reactor&) { }
  geometry_reactor& operator=(const geometry_reactor&) { return *this; }

  //Functions used only in class geometry_reactor
  bool build_elements();
  bool build_faces();
  bool build_vertices();
  bool build_boundary_faces();
  bool build_exterior_normal_surface();
  int opposite_vertex(int,int) const;

  //Attributes
  double volume_;

  unsigned int number_of_elements_;
  ArrOfBit elements_;

  unsigned int number_of_faces_;
  ArrOfInt faces_;

  unsigned int number_of_vertices_;
  ArrOfInt vertices_;

  unsigned int number_of_boundary_faces_;
  ArrOfInt boundary_faces_;
  DoubleTab exterior_normal_surface_;

  reactor *reactor_;
};




inline unsigned int geometry_reactor::nb_elements() const { return number_of_elements_; }
inline unsigned int& geometry_reactor::nb_elements() { return number_of_elements_; }

inline unsigned int geometry_reactor::nb_faces() const { return number_of_faces_; }
inline unsigned int& geometry_reactor::nb_faces() { return number_of_faces_; }

inline unsigned int geometry_reactor::nb_vertices() const { return number_of_vertices_; }
inline unsigned int& geometry_reactor::nb_vertices() { return number_of_vertices_; }

inline unsigned int geometry_reactor::nb_boundary_faces() const { return number_of_boundary_faces_; }
inline unsigned int& geometry_reactor::nb_boundary_faces() { return number_of_boundary_faces_; }

inline double geometry_reactor::volume() const { return volume_; }
inline double& geometry_reactor::volume() { return volume_; }

inline const ArrOfBit& geometry_reactor::elements() const { return elements_; }
inline ArrOfBit& geometry_reactor::elements() { return elements_; }

inline const ArrOfInt& geometry_reactor::faces() const { return faces_; }
inline const ArrOfInt& geometry_reactor::vertices() const { return vertices_; }

inline const reactor* geometry_reactor::reactorPtr() const { return reactor_; }

inline const ArrOfInt& geometry_reactor::boundary_faces() const { return boundary_faces_; }
inline ArrOfInt& geometry_reactor::boundary_faces() { return boundary_faces_; }

inline const DoubleTab& geometry_reactor::exterior_normal_surface() const { return exterior_normal_surface_; }
inline DoubleTab& geometry_reactor::exterior_normal_surface() { return exterior_normal_surface_; }

inline unsigned int geometry_reactor::global_number(unsigned int local_number) const
{
  //Checking condition
  assert(local_number<= number_of_elements_);

  const int nb_elements = elements_.size_array();

  unsigned int global_number=0;
  int iter=-1;

  //Calculation of global_number
  for (unsigned int i=0; iter!=(int) local_number && i<(unsigned int) nb_elements; i++)
    if (elements_[i]) { iter++; global_number=i; }

  //Verifying validity of function
  assert(iter>=0);
  assert((unsigned int) iter==local_number);

  return global_number;
}

inline unsigned int geometry_reactor::local_number(unsigned int global_number) const
{
  assert((int) global_number<elements_.size_array());

  int local_number = -1;
  for (unsigned int i=0; i<global_number+1; i++)
    if (elements_[i]) local_number++;

  assert(local_number>=0);
  return (unsigned int) local_number;
}

inline bool geometry_reactor::is_element(const int& i) const
{
  assert(i>=0 && i<elements_.size_array());
  return (bool) elements_[i];
}

inline bool geometry_reactor::is_face(const int& i) const
{
  assert(i>=0);
  for (unsigned int j=0; j<number_of_faces_; j++)
    if (faces_[j]==i) return true;

  return false;
}

inline bool geometry_reactor::is_vertex(const int& i) const
{
  assert(i>=0);
  for (unsigned int j=0; j<number_of_vertices_; j++)
    if (vertices_[j]==i) return true;

  return false;
}

inline bool geometry_reactor::is_boundary_face(const int& i) const
{
  assert(i>=0);
  for (unsigned int j=0; j<number_of_boundary_faces_; j++)
    if (boundary_faces_[j]==i) return true;

  return false;
}

inline istream& operator>>(istream& is, geometry_reactor& geometry)
{
  return is; //to do
}

inline ostream& operator<<(ostream& os, const geometry_reactor& geometry)
{
  return os; //to do
}




/***************************************/
/***************************************/
/* Definition of class physics_reactor */
/***************************************/
/***************************************/


/* This class describes a RPA and its main functions*/
/* We thus have access and i/o functions */
class physics_reactor {

  friend class reactor;

  /* i/o functions */
  friend istream& operator>>(istream&, physics_reactor&);
  friend ostream& operator<<(ostream&, const physics_reactor&);




  /* Access functions */ 
 public :

   //Functions that give read only access to physics_reactor attributes
  const vector<reactor_component>& reactor_components() const;
  vector<double> velocity_flux() const;

  //Function that prints object physics_reactor in ASCII
  void print(ostream&) const;

/*   const reactor& reactor() const; */
  



  /* Functions modifying attributes of class reactor */
 protected :

  //Functions that initialize or destroy the object physics_reactor
  physics_reactor(reactor&);
  physics_reactor();
  ~physics_reactor();

  void build(reactor&);
  void clear();


  //Functions that give a read only access to geometry_reactor
  //But used only in class geometry_reactor
  //Very useful when using functions on constant objects 
  const reactor* reactorPtr() const;

  //Functions that calculate the average or volumic average
  //of a component with a given number in reactor_->domain
  //its flux through the boundary faces with a given number
  //in reactor_->domain
  vector<double> physics_average(unsigned int) const; 
  vector<double> physics_volumic_average(unsigned int) const;
  vector<double> component_flux(unsigned int) const;
  
  //Functions that modify physics_reactor attributes
  vector<reactor_component>& reactor_components();


  /* Attributes */
 private :
  
  //Copies are forbidden
  physics_reactor(const physics_reactor&) {}
  physics_reactor& operator=(const physics_reactor&) { return *this; }
  
  //Functions used only in class physics_reactor
  bool build_reactor_components();

  //Attributes
  vector<reactor_component> reactor_components_;

  reactor* reactor_;
};


inline const vector<reactor_component>& physics_reactor::reactor_components() const { return reactor_components_; }
inline vector<reactor_component>& physics_reactor::reactor_components() { return reactor_components_; }

inline const reactor* physics_reactor::reactorPtr() const { return reactor_; }

inline istream& operator>>(istream& is, physics_reactor& physics)
{
  return is;
}

inline ostream& operator<<(ostream& os, const physics_reactor& physics)
{
  return os;
}



/*******************************/
/*******************************/
/* Definition of class reactor */
/*******************************/
/*******************************/


/* This class describes a RPA and its main functions*/
/* We thus have access and i/o functions */
class reactor {

  friend class reactors;

  /* i/o functions */
  friend istream& operator>>(istream&, reactor&);
  friend ostream& operator<<(ostream&, const reactor&);




  /* Access functions */
 public :

   //Functions that initialize or destroy reactor object
  reactor();
  reactor(const Domain&, vector<Field<FloatTab> >&, building_criterion&);
  reactor(const Domain&, vector<Field<FloatTab> >&, building_criterion&, reactor_post_treatment&);
  ~reactor();

  void build(const Domain&, vector<Field<FloatTab> >&, building_criterion&);
  void build(const Domain&, vector<Field<FloatTab> >&, building_criterion&, reactor_post_treatment&);
  void clear();  

  //Functions that give read only access to reactor attributes  
  const Domain& domain() const;
  const geometry_reactor& geometry() const;
  const physics_reactor& physics() const;  
  const building_criterion& criterion() const;
  const reactor_post_treatment& post_treatment() const;

  unsigned int search_component_name(const string&) const;

  const vector<Field<FloatTab> >& unknown_data() const;
  const Field<FloatTab>& unknown_data(unsigned int) const;

  unsigned int number() const;

  bool is_defined() const;
  bool is_domain_defined() const;
  bool is_criterion_defined() const;
  bool is_post_treatment_defined() const;
  bool is_geometry_defined() const;
  bool is_physics_defined() const;
  bool is_post_treatment_ok() const;


  //Function that prints object reactor in ASCII
  void print(ostream&, bool is_complete_data_wanted=false) const; 



  /* Functions modifying attributes of class reactor */
 protected :

   //Functions that modify reactor attributes
  geometry_reactor& geometry();
  physics_reactor& physics();

  building_criterion* criterionPtr();
  reactor_post_treatment* post_treatmentPtr();
  const Domain* domainPtr();

  unsigned int& number();

  bool& is_geometry_defined();
  bool& is_physics_defined();

  //Functions that give a read only access to geometry_reactor
  //But used only in class geometry_reactor
  //Very useful when using functions on constant objects 
  const Domain* domainPtr() const;
  const building_criterion* criterionPtr() const;
  const reactor_post_treatment* post_treatmentPtr() const;


  /* Attributes */
 private :
  
  //Copies are forbidden
  reactor(const reactor&) {}
  reactor& operator=(const reactor&) { return *this; }

  //Functions used only in reactor
  void build_components_names();

  //Attributes
  bool is_geometry_defined_;
  bool is_physics_defined_;
  unsigned int number_;

  geometry_reactor geometry_reactor_;
  physics_reactor physics_reactor_;
  building_criterion *building_criterion_;
  reactor_post_treatment *post_treatment_;
  const Domain *domain_;
  vector<Field<FloatTab> > *latafields_;

  map<string, unsigned int> components_names_;
};



inline istream& operator>>(istream& is, reactor& Reactor) 
{
  cerr << "Reading reactor data" << endl;
  is >> Reactor.number_ >> Reactor.geometry_reactor_ 
     >> Reactor.physics_reactor_ ;//to do : writing building criterion
  cerr << "End of reading reactor data" << endl;

  return is;
}

inline ostream& operator<<(ostream& os, const reactor& Reactor)
{
  cerr << "Writing reactor data" << endl;
  os << Reactor.number_ << Reactor.geometry_reactor_ 
     << Reactor.physics_reactor_ ;//to do : printing building criterion
  cerr << "End of writing reactor data" << endl;

  return os;
}

inline const vector<Field<FloatTab> >& reactor::unknown_data() const { return *latafields_ ; }
inline const Field<FloatTab>& reactor::unknown_data(unsigned int i) const { return (*latafields_)[i]; }

inline unsigned int reactor::number() const { return number_; }
inline unsigned int& reactor::number() { return number_; }

inline bool reactor::is_defined() const 
{ 
  if (domain_!=NULL && building_criterion_!=NULL) return true;
  return false;
}

inline bool reactor::is_domain_defined() const { if (domain_!=NULL) return true; return false; }
inline bool reactor::is_criterion_defined() const { if (building_criterion_!=NULL) return true; return false; }
inline bool reactor::is_post_treatment_defined() const { if (post_treatment_!=NULL) return true; return false; }

inline bool reactor::is_geometry_defined() const { return is_geometry_defined_; }
inline bool& reactor::is_geometry_defined() { return is_geometry_defined_; }

inline bool reactor::is_physics_defined() const { return is_physics_defined_; }
inline bool& reactor::is_physics_defined() { return is_physics_defined_; }

inline const geometry_reactor& reactor::geometry() const { return geometry_reactor_; }
inline geometry_reactor& reactor::geometry() { return geometry_reactor_; }

inline const physics_reactor& reactor::physics() const { return physics_reactor_; }
inline physics_reactor& reactor::physics() { return physics_reactor_; }

inline const building_criterion& reactor::criterion() const  { return *building_criterion_; }
inline building_criterion* reactor::criterionPtr() { return building_criterion_; }
inline const building_criterion* reactor::criterionPtr() const { return building_criterion_; }

inline const reactor_post_treatment& reactor::post_treatment() const { return *post_treatment_; }
inline reactor_post_treatment* reactor::post_treatmentPtr() { return post_treatment_; }
inline const reactor_post_treatment* reactor::post_treatmentPtr() const { return post_treatment_; }

inline const Domain& reactor::domain() const { return *domain_; }
inline const Domain* reactor::domainPtr() { return domain_; }
inline const Domain* reactor::domainPtr() const { return domain_; }



/******************************************/
/******************************************/
/* Definition of class building criterion */
/******************************************/
/******************************************/


/* This class describes criteria used to define and build RPAs */
/* We thus have access and i/o functions */
class building_criterion {


  friend class building_criterions;

  /* i/o functions */
  friend istream& operator>>(istream&,building_criterion&);
  friend ostream& operator<<(ostream&,const building_criterion&);




 public :

   //Functions that build and destroy building_criterion objet 
  building_criterion();
  ~building_criterion();

  void clear();
  void initPtr(reactor& Reactor);

  //Functions that give read only access to building_criterion object 
  unsigned int number() const;
  unsigned int& number(); //be careful when use this function
  unsigned int search_type(const string&) const;
  unsigned int search_coordinate_type(const string&) const;

  string type() const;
  string coordinate_type() const;

  double bound_min() const;
  double bound_max() const;

  const vector<string>& vector_of_parsers_string() const;  

  const vector<Parser*>& parsers() const;

  const reactor* reactorPtr() const;

  //Functions that test building_criterion attributes  
  bool is_satisfied(const int&) const;

  //Function that prints object building_criterion in ASCII
  void print(ostream&) const;



 protected :

   //Functions that modify building_criterion attributes
  string& type();
  string& coordinate_type();

  double& bound_min();
  double& bound_max();

  bool& replacing_bound_max();
  bool& replacing_bound_min();

  vector<string>& vector_of_parsers_string();
  vector<Parser*>& parsers();

  reactor* reactorPtr();

  void deletePtr();

  

 private :
  
  //Copies are forbidden
  building_criterion(const building_criterion&) { }
  building_criterion& operator=(const building_criterion&) { return *this; }  

  //Functions that are only used in class building_criterion
  void build_type_keywords();
  void build_coordinate_type_keywords();
  void build_parsers();
  void build_parsers_string(const vector<string>&);
  void build_bounds(double,double);

  void replace_bounds(const LataField_base&) const;


  //Attributes
  unsigned int number_;

  string type_;
  string coordinate_type_;
  vector<string> vector_of_parsers_string_;
  vector<Parser*> parsers_;

  mutable double bound_max_;
  mutable double bound_min_;

  mutable bool replacing_bound_max_;
  mutable bool replacing_bound_min_;

  reactor* reactor_;

  //Dictionnaries of some important keywords 
  map<string, unsigned int> type_keywords_;
  map<string, unsigned int> coordinate_type_keywords_;
};


inline istream& operator>>(istream& is, building_criterion& criterion)
{
  criterion.type_ = "";//to do
  is >> criterion.bound_max_ >> criterion.bound_min_;
  return is;
}

inline ostream& operator<<(ostream& os,const building_criterion& criterion)
{
  os << "TYPE : " << criterion.type_ << endl;
/*   os << "COORDINATE TYPE : " << criterion.coordinate_type_ << endl; */

  os << "Maximum and minimum bounds : " << criterion.bound_max_ << ", " 
     << criterion.bound_min_ << endl;

  return os;
}


inline unsigned int& building_criterion::number() { return number_; }
inline unsigned int building_criterion::number() const { return number_; }

inline string building_criterion::type() const { return type_; }
inline string& building_criterion::type() { return type_; }

inline string building_criterion::coordinate_type() const { return coordinate_type_; }
inline string& building_criterion::coordinate_type() { return coordinate_type_; }

inline reactor* building_criterion::reactorPtr() { return reactor_; }
inline const reactor* building_criterion::reactorPtr() const { return reactor_; }

inline void building_criterion::initPtr(reactor& Reactor) { reactor_=&Reactor; }
inline void building_criterion::deletePtr() { reactor_=NULL; }

inline double building_criterion::bound_min() const { return bound_min_; }
inline double& building_criterion::bound_min() { return bound_min_; }

inline double building_criterion::bound_max() const { return bound_max_; }
inline double& building_criterion::bound_max() { return bound_max_; }

inline const vector<string>& building_criterion::vector_of_parsers_string() const { return vector_of_parsers_string_; }
inline vector<string>& building_criterion::vector_of_parsers_string() { return vector_of_parsers_string_; }

inline vector<Parser*>& building_criterion::parsers() { return parsers_; }
inline const vector<Parser*>& building_criterion::parsers() const { return parsers_; }

inline bool& building_criterion::replacing_bound_max() { return replacing_bound_max_; }
inline bool& building_criterion::replacing_bound_min() { return replacing_bound_min_; }



/******************************************/
/******************************************/
/* Definition of class building_criterions */
/******************************************/
/******************************************/

class building_criterions : public vector<building_criterion*>
{
  friend istream& operator>>(istream&, building_criterions&);
  friend ostream& operator<<(ostream&, const building_criterions&);

 public :

   //Functions that build or destroy building_criterions object
  building_criterions(int n=0);
  ~building_criterions();
  
  void build_criterions();
  void clear_criterions();

  //Functions that give read only access to building_criterions attributes
  const vector<double>& bounds_min() const;
  const vector<double>& bounds_max() const;
  
  unsigned int number_of_reactors() const;
  unsigned int search_keyword(const string&) const;

  const vector<string>& types() const;
  const vector<string>& coordinate_types() const;
  


 protected :
  
  //Functions that modify building_criterions attributes
  vector<double>& bounds_min();
  vector<double>& bounds_max();




 private : 
  
  //Copies are forbidden
  building_criterions(const building_criterions&) { }
  building_criterions& operator=(const building_criterions&) { return *this; }  

  //Functions used only when creating component_builder object with operator>>
  void skip(istream&, string);
  void read_type(istream&); 
  void read_coordinate_type(istream&);
  void read_parsers_data(istream&);
  void read_bounds(istream&);
  void read_number_of_reactors(istream&); 
  
  //Function that build map(s) and check data
  void build_keywords();
  unsigned int search_keyword_without_stop(const string&) const;

  //Attributes
  vector<string> types_;
  vector<string> coordinate_types_;
  vector< vector<string> > vector_of_parsers_string_;
  
  unsigned int number_of_reactors_;

  vector<double> bounds_min_;
  vector<double> bounds_max_;

  map<string, unsigned int> keywords_;
};


inline istream& operator>>(istream& is, building_criterions& criterions)
{
  string read_word;
  unsigned int where_in_keywords = 0;

  while (is >> read_word)
    {
      where_in_keywords = criterions.search_keyword(read_word);//to do to_upper

      switch(where_in_keywords)
	{
	case 1 : //TYPE of building_criterions

	  criterions.read_type(is);
	  break;

	case 2 : //NUMBER of reactor(s) to create

	  criterions.read_number_of_reactors(is);
	  break;

	case 3 : //COMMENTARY

	  criterions.skip(is,"#"); 
	  break;

	case 6 : //TYPE of coordinates

	  criterions.read_coordinate_type(is);
	  break;

	default : //WORD found in keywords_ but not used here

	  assert(where_in_keywords>=5);

	  cerr << "Error in building_criterions::operator>>" << endl;
	  cerr << "Keyword " << read_word << " must not be used here" << endl;
	  cerr << "Exiting program" << endl;
	  exit(-1);

	  break;

	}//end of switch on keywords_

    }//fin du while 

  return is; 
}

inline ostream& operator<<(ostream& os, const building_criterions& criterions)
{
  //Writing size of building_criterions
  os << "NUMBER_OF_REACTORS " << criterions.number_of_reactors_ << endl;;

  //Writing type of building_criterions
  for (unsigned int i=0; i<criterions.types_.size(); i++)
    os << "TYPE " << criterions.types_[i] << endl;

/*   //Writing coordinate type of building_criterions */
/*   for (unsigned int i=0; i<criterions.types_.size(); i++) */
/*     os << "COORDINATE TYPE " << criterions.coordinate_types_[i] << endl; */

  //Writing 
  for (unsigned int i=0; i<criterions.number_of_reactors_; i++)
    {
      assert(criterions[i]!=NULL);
      os << *(criterions[i]);    
    }

  return os;
}

inline const vector<double>& building_criterions::bounds_min() const { return bounds_min_; }
inline vector<double>& building_criterions::bounds_min() { return bounds_min_; }

inline const vector<double>& building_criterions::bounds_max() const { return bounds_max_; }
inline vector<double>& building_criterions::bounds_max() { return bounds_max_; }

inline unsigned int building_criterions::number_of_reactors() const { return number_of_reactors_; }

inline const vector<string>&  building_criterions::types() const { return types_; }
inline const vector<string>&  building_criterions::coordinate_types() const { return coordinate_types_; }



/***********************************************/
/***********************************************/
/* Definition of class reactor_post_treatment */
/***********************************************/
/***********************************************/




class reactor_post_treatment {

  friend class reactor_post_treatments;

  friend istream& operator>>(istream&, reactor_post_treatment&);
  friend ostream& operator<<(ostream&, const reactor_post_treatment&);

 public :

  //Functions that build and destroy reactor_post_treatment objet 
  reactor_post_treatment();
  ~reactor_post_treatment();

  void clear();

  //Functions that give read only access to reactor_post_treatment object
  const vector<string>& names_of_special_calculation() const;
  const vector< vector<string> >& vector_of_parameters_names() const;

  unsigned int search_special_calculation(const string&) const;
  bool is_special_calculation_defined(const string&) const;

  //Function that prints object reactor_post_treatement in ASCII
  void print(ostream&) const; 

 protected :

  //Functions that modify reactor_post_treatment attributes
  vector<string>& names_of_special_calculation();
  vector< vector<string> >& vector_of_parameters_names();
  

 private :

  //Copies are forbidden
  reactor_post_treatment(const reactor_post_treatment&) { }
  reactor_post_treatment& operator=(const reactor_post_treatment&) { return *this; }

  //Functions used only in class reactor_post_treatment
  void build_names_of_special_calculation(const vector<string>&);
  void build_parameters_names(const vector< vector<string> >&);
  void build_special_calculation_keywords();

  void synchronize_keywords_with_special_calculation();

  //Attributes

  //Attributes for post-treatment data
  vector<string> names_of_special_calculation_;
  vector< vector<string> > vector_of_parameters_names_; 

  //Dictionnary of some important keywords 
  map<string, unsigned int> special_calculation_keywords_; 
};


inline vector<string>& reactor_post_treatment::names_of_special_calculation() { return names_of_special_calculation_; }
inline const vector<string>& reactor_post_treatment::names_of_special_calculation() const { return names_of_special_calculation_; }

inline vector< vector<string> >& reactor_post_treatment::vector_of_parameters_names() { return  vector_of_parameters_names_; }
inline const vector< vector<string> >& reactor_post_treatment::vector_of_parameters_names() const { return  vector_of_parameters_names_; } 


inline istream& operator>>(istream& is, reactor_post_treatment& post_treatment)
{
  return is;//to do
}

inline ostream& operator<<(ostream& os,const reactor_post_treatment& post_treatment)
{
  assert(post_treatment.names_of_special_calculation_.size()==post_treatment.vector_of_parameters_names_.size());

  os << "Names of special calculation : ";
  for (unsigned int i=0; i<post_treatment.names_of_special_calculation_.size(); i++)
    os << post_treatment.names_of_special_calculation_[i] << ", ";
  os << endl;

  os << "Parameters needed for special calculation : ";
  for (unsigned int i=0; i<post_treatment.vector_of_parameters_names_.size(); i++)
    {
      os << "{ ";
      for (unsigned int j=0; j<post_treatment.vector_of_parameters_names_[i].size(); j++)
	os << post_treatment.vector_of_parameters_names_[i][j] << ", ";
      os << " } ; ";
    }
  os << endl;

  return os;
}



/***********************************************/
/***********************************************/
/* Definition of class reactor_post_treatments */
/***********************************************/
/***********************************************/



class reactor_post_treatments : public vector<reactor_post_treatment*>
{

  friend istream& operator>>(istream&, reactor_post_treatments&);
  friend ostream& operator<<(ostream&, const reactor_post_treatments&);

 public :

   //Functions that build or destroy reactor_post_treatments object
  reactor_post_treatments(int n=0);
  ~reactor_post_treatments();
  
  void build_post_treatments();
  void clear_post_treatments();

  //Functions that give read only access to reactor_post_treatments attributes
  unsigned int search_keyword(const string&) const;

  const vector<string>& names_of_special_calculation() const;
  const vector< vector<string> > & vector_of_parameters_names() const;
  


 protected :
  


 private : 
  
  //Copies are forbidden
  reactor_post_treatments(const reactor_post_treatments&) { }
  reactor_post_treatments& operator=(const reactor_post_treatments&) { return *this; }  

  //Functions used only when creating component_builder object with operator>>
  void skip(istream&, string);
  void read_name_of_special_calculation(istream&);
  void read_special_calculation_data(istream&);
  void read_parameters(istream&,vector< vector<string> >&);
  void read_label(istream&,vector<string>&);
  
  //Function that build map(s) and check data
  void build_keywords();
  unsigned int search_keyword_without_stop(const string&) const;

  //Attributes
  vector<string> names_of_special_calculation_;
  vector< vector<string> > vector_of_parameters_names_;
  
  map<string, unsigned int> keywords_;
};


inline const vector<string>&  reactor_post_treatments::names_of_special_calculation() const { return names_of_special_calculation_; }
inline const vector< vector<string> >&  reactor_post_treatments::vector_of_parameters_names() const { return vector_of_parameters_names_; }



inline istream& operator>>(istream& is, reactor_post_treatments& post_treatments)
{
  string read_word;
  unsigned int where_in_keywords = 0;

  while (is >> read_word)
    {
      where_in_keywords = post_treatments.search_keyword(read_word);//to do to_upper

      switch(where_in_keywords)
	{
	case 1 : //NAME of special calculation 
	  
	  post_treatments.read_name_of_special_calculation(is);
	  post_treatments.read_special_calculation_data(is);
	  break;

	case 2 : //COMMENTARY

	  post_treatments.skip(is,"#"); 
	  break; 

	default : //WORD found in keywords_ but not used here

	  assert(where_in_keywords>=5);

	  cerr << "Error in reactor_post_treatments::operator>>" << endl;
	  cerr << "Keyword " << read_word << " must not be used here" << endl;
	  cerr << "Exiting program" << endl;
	  exit(-1);

	  break;

	}//end of switch on keywords_

    }//fin du while 

  return is; 
}

inline ostream& operator<<(ostream& os, const reactor_post_treatments& post_treatments)
{
  //Writing 
  for (unsigned int i=0; i<post_treatments.size(); i++)
    {
      assert(post_treatments[i]!=NULL);
      os << *(post_treatments[i]);    
    }

  return os;
}


/********************************/
/********************************/
/* Definition of class reactors */
/********************************/
/********************************/

class reactors : public vector<reactor*>
{
  /* i/o functions */
  friend istream& operator>>(istream&, reactors&);
  friend ostream& operator<<(ostream&, const reactors&);
  


  /* Access functions */
 public :
  
   //Functions that build or destroy reactors object
  reactors(int n=0);
  reactors(const Domain&, vector<Field<FloatTab> >&, building_criterions&);
  reactors(const Domain&, vector<Field<FloatTab> >&, building_criterions&, reactor_post_treatments&);
  ~reactors();

  void build(const Domain&, vector<Field<FloatTab> >&, building_criterions&);
  void build(const Domain&, vector<Field<FloatTab> >&, building_criterions&, reactor_post_treatments&);
  void clear();

  //Functions that give read only access to reactors attributes
  const reactor* reactorsPtr(unsigned int) const;
  unsigned int nb_reactors() const;

  //Functions that print reactors objet or ractors attributes in ASCII
  void print(ostream&, bool is_complete_data_wanted=false) const;
  void print_velocity_flux(ostream&) const;
  void print_components_fluxes(ostream&) const;

  //Functions that test reactors object
  bool are_reactors_ok(unsigned int&) const;

  void write_components(LataFilter & , const int binary_out , vector<Field<FloatTab> > &);

 protected:

  //Functions that modify reactors object
  reactor* reactorsPtr(unsigned int);
  unsigned int& nb_reactors();



 private : 

  //Copies are forbidden
  reactors(const reactors&) { }
  reactors& operator=(const reactors&) { return *this; }  

  //Attributes 
  unsigned int number_of_reactors_;
};



inline istream& operator>>(istream& is, reactors& Reactors)
{
  for (unsigned int i=0; i<Reactors.number_of_reactors_; i++)
    {
      assert(Reactors[i]!=NULL);
      is >> *(Reactors[i]);
    }

  return is;
}

inline ostream& operator<<(ostream& os, const reactors& Reactors)
{
   for (unsigned int i=0; i<Reactors.number_of_reactors_; i++)
     {
       assert(Reactors[i]!=NULL);
       os << *(Reactors[i]);
     }

  return os; 
}

inline void reactors::build(const Domain& domain, vector<Field<FloatTab> >& latafields, building_criterions& criterions)
{
  //Tests
  assert(number_of_reactors_>0);
  assert(criterions.size()==number_of_reactors_);

  //Building of reactors
  for (unsigned int i=0; i<number_of_reactors_; i++)
    {
      assert((*this)[i]!=NULL);
      assert(criterions[i]!=NULL);

      reactor& Reactor = *((*this)[i]);
      Reactor.build(domain,latafields,*(criterions[i]));
    }
}

inline void reactors::build(const Domain& domain, vector<Field<FloatTab> >& latafields, building_criterions& criterions, reactor_post_treatments& post_treatments)
{
  //Tests
  assert(number_of_reactors_>0);
  assert(criterions.size()==number_of_reactors_);

  //Building of reactors
  for (unsigned int i=0; i<number_of_reactors_; i++)
    {
      assert((*this)[i]!=NULL);
      assert(criterions[i]!=NULL);
      assert(post_treatments[i]!=NULL);

      reactor& Reactor = *((*this)[i]);
      Reactor.build(domain,latafields,*(criterions[i]),*(post_treatments[i]));
    }
}

inline void reactors::clear()
{
  for (unsigned int i=0; i<number_of_reactors_; i++)
    (*this)[i]->clear();
}

inline const reactor* reactors::reactorsPtr(unsigned int i) const
{
  assert(i<(*this).size());
  return (*this)[i];
}

inline reactor* reactors::reactorsPtr(unsigned int i)
{
  assert(i<(*this).size());
  return (*this)[i];
}

inline unsigned int reactors::nb_reactors() const
{ 
  return number_of_reactors_;
}

inline unsigned int& reactors::nb_reactors()
{ 
  return number_of_reactors_;
}

#endif
