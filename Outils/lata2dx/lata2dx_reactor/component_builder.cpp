#include <sstream>
#include <stdlib.h>
#include "component_builder.h"
#include "reactor_component.h"

extern string IntToString2(int);
extern char* c_string(const string&);
extern string delete_label(const string&);
extern string extract_label(const string&);
extern char* to_upper(const char*);
extern char* to_lower(const char*);
extern string& to_upper(string&);
extern string& to_lower(string&);


/* Functions of class component_builder */
component_builder::component_builder()
{
  domain_=NULL;

  build_keywords();

  assert(vector_of_parameters_names_.size()==0);
  assert(vector_of_parsersPtr_.size()==0);
  assert(vector_of_nbplaces_.size()==0);
  assert(vector_of_shape_.size()==0);
  assert(names_.size()==0);
  assert(labels_.size()==0);
  assert(types_.size()==0);
}

component_builder::component_builder(const Domain& domain, vector<Field<FloatTab> >& latafields)
{
  domain_=&domain;
  latafields_=&latafields;

  build_keywords();
  build_components_names();

  assert(vector_of_parameters_names_.size()==0);
  assert(vector_of_parsersPtr_.size()==0);
  assert(vector_of_nbplaces_.size()==0);
  assert(vector_of_shape_.size()==0);
  assert(names_.size()==0);
  assert(labels_.size()==0);
  assert(types_.size()==0);
}
 
void component_builder::init(const Domain& domain, vector<Field<FloatTab> >& latafields)
{
  domain_=&domain;
  latafields_=&latafields;

  build_components_names();

  assert(vector_of_parameters_names_.size()==0);
  assert(vector_of_parsersPtr_.size()==0);
  assert(vector_of_nbplaces_.size()==0);
  assert(vector_of_shape_.size()==0);
  assert(names_.size()==0);
  assert(labels_.size()==0);
  assert(types_.size()==0);
}

component_builder::~component_builder()
{
  domain_=NULL;

  names_.clear();
  labels_.clear();
  types_.clear();

  keywords_.clear();
  components_names_.clear();

  vector_of_nbplaces_.clear();
  vector_of_shape_.clear();
  vector_of_parameters_names_.clear();

  /* Special treatment when considering parsers */
  for (unsigned int i=0; i<vector_of_parsersPtr_.size(); i++)
    for (unsigned int j=0; j<vector_of_parsersPtr_[i].size(); j++)
      delete vector_of_parsersPtr_[i][j];

  vector_of_parsersPtr_.clear();
}

string custom_field_name(const LataField_base & f)
{
  return string(f.id_.uname_.get_field_name()) 
    + string("_") 
    + string(f.id_.uname_.get_localisation());
}

void component_builder::build_keywords()
{
  keywords_["NAME"]=1;
  keywords_["TYPE"]=2;
  keywords_["LABEL"]=3;
  keywords_["PARAMETERS"]=4;
  keywords_["#"]=5;
} 

void component_builder::build_components_names()
{
  assert(domain_!=NULL);

  const vector<Field<FloatTab> >& latafields = *latafields_;
  const unsigned int components_size = latafields.size();

  for (unsigned int i=0; i<components_size; i++)
    components_names_[custom_field_name(latafields[i])]=i+1;
}

unsigned int component_builder::search_keyword(const string& s) const
{
  typedef map<string, unsigned int>::const_iterator CI;
  CI is_found = keywords_.find(s);

  const unsigned int where = (is_found==keywords_.end()) ? 0 : is_found->second;
  assert(where>=0 && where< keywords_.size()+1);

  if (!where) 
    {
      cerr << "Error in component_builder::search_keyword()" << endl;
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

unsigned int component_builder::search_component_name(const string& s) const
{
  typedef map<string, unsigned int>::const_iterator CI;
  CI is_found = components_names_.find(s);

  const unsigned int where = (is_found==components_names_.end()) ? 0 : is_found->second;
  assert(where>=0 && where< components_names_.size()+1);

  if (!where) 
    {
      cerr << "Error in component_builder::search_component_name()" << endl;
      cerr << "Word " << s << " is not a known component name" << endl;
      cerr << "Known component names are the following : ";

      for (CI p=components_names_.begin(); p!=components_names_.end(); p++)
	cerr << p->first << ", ";
      cerr << endl;
      
      cerr << "Exiting program" << endl;
      exit(-1);
    }

  return where-1;
}

void component_builder::skip(istream& is, string s)
{
  string read_word;

  is >> read_word;
  while (read_word!=s)  is >> read_word;
}

void component_builder::read_function_name(istream& is)
{
  string read_word;

  is >> read_word;
  names_.push_back( to_upper(read_word) );
}

void component_builder::read_function_type(istream& is)
{
  string read_word;
  
  is >> read_word;
  types_.push_back( to_upper(read_word) );
}

void component_builder::read_function_label(istream& is)
{
  string read_word;

  is >> read_word;
  labels_.push_back( to_upper(read_word) );
}

void component_builder::read_function_parameters(istream& is)
{
  string read_word;
  vector<string> names_used;

  is >> read_word;
  while (read_word!="{") 
    { 
      names_used.push_back( to_upper(read_word) ); 
      is>>read_word;
    }
  vector_of_parameters_names_.push_back(names_used);
}

void component_builder::read_function_definition(istream& is)
{
  unsigned int size = 0;

  const DomainUnstructured * domainUnstructured_Ptr = dynamic_cast<const DomainUnstructured *>(domain_);

  if (!domainUnstructured_Ptr)
  {
    Journal() << "Error : domain " << domain_->id_.name_ << " is not domain unstructured" << endl;
    throw;
  }

  const int dimension = (types_.back()=="SCALAR") ? 1 : domainUnstructured_Ptr->nodes_.dimension(1);
  const unsigned int maximum_number_of_unknown = 100;
  const unsigned int data_size = vector_of_parameters_names_.back().size();
 
  string read_function;
  string unknown_sign;
  vector<Parser*> parsersPtr;

  while ( (is >> read_function) && (read_function!="}") ) 
    {
      String2 read_function_String2( c_string(read_function) );
      parsersPtr.push_back( new Parser(read_function_String2,maximum_number_of_unknown) );

      for (unsigned int j=0; j<data_size; j++)
	{
	  unknown_sign = "x"+IntToString2(j);
	  parsersPtr.back()->addVar( c_string(unknown_sign) );
	}//end for on j

      parsersPtr.back()->parseString2();
      size++;
    }//end of while loop

  if ((int) size!=dimension)
    {
      cerr << "Error in component_builder::read_function_definition()" << endl;
      cerr << "Function " << names_.back() << " requires " << dimension 
	   << " components" << endl;
      cerr << "You try to define a function with " << size << " components"
	   << endl;
      cerr << "Exiting programm" << endl;
      exit(-1);
    }

  vector_of_parsersPtr_.push_back(parsersPtr);
}

bool component_builder::are_names_unknown() const
{ 
//   assert(domain_!=NULL);
//   assert(names_.size()!=0);
//   assert(labels_.size()!=0);
//   assert(names_.size()==labels_.size());

//   for (unsigned int i=0; i<names_.size(); i++)
//     {
//       const string& name = names_[i];
//       const string& label = labels_[i];
//       if ( search_component_name(name+"_"+label) >=0 ) return false;
//     }
 
  return true;
}

bool component_builder::are_function_parameters_unknown() const
{ 
  assert(domain_!=NULL);
  assert(names_.size()==labels_.size());
  assert(names_.size()==vector_of_parameters_names_.size());

  bool test = false;

  for (unsigned int i=0; i<labels_.size(); i++) 
    {
      const string& label = labels_[i];
      const unsigned int names_used_size = vector_of_parameters_names_[i].size();
      
      for (unsigned int j=0; j<names_used_size; j++)
	{
	  const string& name = vector_of_parameters_names_[i][j];
	  const unsigned int where = search_component_name(name+"_"+label);
	  if (where>=0) test|=false;
	  else test|=true;
	}//end for on j
      
    }//end for on i

  return test;
}

bool component_builder::are_information_ok() const
{
  assert(domain_!=NULL);
  assert(names_.size()==vector_of_parameters_names_.size());
  assert(labels_.size()==vector_of_parameters_names_.size());
  assert(names_.size()==types_.size());  

  const vector<Field<FloatTab> >& latafields = *latafields_;

  const DomainUnstructured * domainUnstructured_Ptr = dynamic_cast<const DomainUnstructured *>(domain_);

  if (!domainUnstructured_Ptr)
  {
    Journal() << "Error : domain " << domain_->id_.name_ << " is not domain unstructured" << endl;
    throw;
  }

  const int dimension = domainUnstructured_Ptr->nodes_.dimension(1);

  vector< vector<int> > local_vector_of_nbplaces(names_.size());
  vector< vector<int> > local_vector_of_shape(names_.size());
  
  for (unsigned int i=0; i<vector_of_parameters_names_.size(); i++)
    {
      const string& label = labels_[i];
      const unsigned int names_used_size = vector_of_parameters_names_[i].size();
      
      local_vector_of_nbplaces[i].resize(names_used_size);
      local_vector_of_shape[i].resize(names_used_size);
      
      for (unsigned int j=0; j<names_used_size; j++)
	{
	  const string& name = vector_of_parameters_names_[i][j];
	  const unsigned int where = search_component_name(name+"_"+label);

	  if (where>=0)
	    {

	  const Field<FloatTab> * latafld_ptr = dynamic_cast<const Field<FloatTab> *> (&(latafields[where]));

	      local_vector_of_nbplaces[i][j] = latafld_ptr->data_.dimension(0);
	      local_vector_of_shape[i][j] = latafld_ptr->data_.dimension(1);		

	    }
	  else
	    {
	      cerr << "Error in component_builder::are_information_ok()" << endl;
	      cerr << "Exiting programm" << endl;
	      exit(-1);
	    }

	}//end for on j
      
    }//end for on i
  

  bool test_nbplaces = true;
  unsigned int size = local_vector_of_nbplaces.size();
  for (unsigned int k=0; k<size; k++)
    {
      const unsigned int second_size = local_vector_of_nbplaces[k].size();
      
      for (unsigned int l=0; l<second_size; l++)
	{
	  const int& first_nbplaces = local_vector_of_nbplaces[k][l];
	  const int& second_nbplaces = local_vector_of_nbplaces[k][(l+1)%second_size]; 
	  test_nbplaces &= (first_nbplaces==second_nbplaces);
	}//end for on l
      
    }//end for on k
  
  bool test_shape = true;
  size = local_vector_of_shape.size();
  assert(size==types_.size());
  for (unsigned int k=0; k<size; k++)
    {
      const string& type = types_[k];
      const unsigned int second_size = local_vector_of_shape[k].size();
      
      for (unsigned int l=0; l<second_size; l++)
	{
	  const int& shape = local_vector_of_shape[k][l];
	  if (type=="SCALAR") test_shape &= (shape==1);
	  else test_shape &= (shape==dimension) || (shape==1);
	}//end for on l
      
    }//end for on k
  
  return test_nbplaces && test_shape;
}

bool component_builder::are_data_ok() const
{
  bool test_names;
  bool test_function_parameters; 
  bool test_information;

  if (test_names = !are_names_unknown() )
    {
      cerr << "Error in component_builder::operator>>" << endl;
      cerr << "One of the new function you try to build already exists" << endl;
      cerr << "Please check data file" << endl;
    }
  if (test_function_parameters = are_function_parameters_unknown()) 
    {
      cerr << "Error in component_builder::operator>>" << endl;
      cerr << "One of the component parameters are not known" << endl;
      cerr << "Please check data file" << endl;
    }
  if (test_information = !are_information_ok())
    {
      cerr << "Error in component_builder::operator>>" << endl;
      cerr << "Components used to create one component" 
	   << " are either not of the same type or "
	   << "not discretized in the same way." << endl;
      cerr << "Please check definition of components." << endl;
    }

  return !test_function_parameters && !test_information;
}

void component_builder::build_components()
{
  assert(domain_!=NULL);
  assert(names_.size()==types_.size());
  assert(names_.size()==vector_of_parameters_names_.size());
  assert(names_.size()==vector_of_parsersPtr_.size());

  const unsigned int data_size = names_.size();
  vector<Field<FloatTab> > new_components(data_size);

  for (unsigned int i=0; i<data_size; i++)
    {
      new_components[i].localisation_ = identity(labels_[i]);
      //new_components[i].component_names_.add(names_[i] + "_" + labels_[i]);
      new_components[i].component_names_.add(names_[i]);
      new_components[i].id_ = Field_Id(Field_UName(domain_->id_.name_, names_[i].c_str(), labels_[i].c_str()),
				      1, /* timestep */
				      -1 /* block */);
      const int nbplaces = vector_of_nbplaces_[i];
      const int shape = vector_of_shape_[i];

      if (shape>1) new_components[i].nature_= LataDBField::VECTOR;
      if (shape==1) new_components[i].nature_= LataDBField::SCALAR;

      LataField_base& new_component_i=new_components[i];

      Field<FloatTab> * latafld_ptr = dynamic_cast<Field<FloatTab> *> (&new_component_i);

      latafld_ptr->data_.resize(nbplaces, shape);

      assert((int) vector_of_parsersPtr_[i].size()==shape);

      const int size_of_components = vector_of_parameters_names_[i].size();
      for (int j=0; j<shape; j++)
	{
	  assert(vector_of_parsersPtr_[i][j]!=NULL);
	  Parser& parser = *(vector_of_parsersPtr_[i][j]);

	  for (int k=0; k<nbplaces; k++)
	    {
	      for (int l=0; l<size_of_components;l++)
		{
		  const string& parameter_name = vector_of_parameters_names_[i][l];

		  const unsigned int where = search_component_name(parameter_name+"_"+labels_[i]);
		  assert(where>=0);

		  const LataField_base& other_field = (*latafields_)[where];

		  const Field<FloatTab> * otherfld_ptr = dynamic_cast<const Field<FloatTab> *> (&other_field);

		  const int parameter_shape = otherfld_ptr->data_.dimension(1);
		  const FloatTab& parameter_values = otherfld_ptr->data_;
		  const string parameter_sign = "x"+IntToString2(l);

		  //If function to build is of VECTOR type and parameters 
		  //are a mixing of SCALAR and VECTOR types
		  if (parameter_shape!=shape) 
		    { 
		      //Following test implies that function to build is of VECTOR type
		      //and parameters are of SCALAR type 
		      assert(parameter_shape==1);
		      parser.setVar( c_string(parameter_sign),parameter_values(k,0) );
		    }

		  //If function to build and parameters are only of one same type
		  else parser.setVar( c_string(parameter_sign),parameter_values(k,j) );
		  
		}//end for on l

		latafld_ptr->data_(k,j)=parser.eval();

	    }//end for on k
 
	}//end for on j

    }//end for on i

   for (unsigned int i=0; i<new_components.size(); i++)
      (*latafields_).push_back(new_components[i]);

  update_components_names();
}

//Funtion that builds component with attributes reactor_components of 
//the different reactors
//ATTENTION : if a reactor_component is of type FLUX, this function does not
//make it as a component readable by open_dx
void component_builder::build_components(const reactors& Reactors)
{
  assert(domain_!=NULL);

  const unsigned int reactors_size = Reactors.size();
  const int nelem = domain_->nb_elements();

  /* Creating new components */
  /* with the attributes reactor_component */
  /* of each reactor */
  const unsigned int reactor_components_size = Reactors.reactorsPtr(0)->physics().reactor_components().size();

  for (unsigned int reactor_compo=0; reactor_compo<reactor_components_size; reactor_compo++)
    {
      Field<FloatTab> component_to_build;
      entier add_compo = 0;

      for (unsigned int reactor=0; reactor<reactors_size; reactor++)
	{
	  const reactor_component& partial_component = 
	    Reactors.reactorsPtr(reactor)->physics().reactor_components()[reactor_compo]; 

	  const string& special_calculation = partial_component.name_of_special_calculation();
	  if (special_calculation=="FLUX") break; //to directly go to next component

	  const unsigned int shape = partial_component.values().size();
	  add_compo = 1;
	  component_to_build.localisation_ = identity( extract_label(partial_component.name()) );
	  component_to_build.component_names_.add(partial_component.name());

	  if (shape>1) component_to_build.nature_= LataDBField::VECTOR;
	  if (shape==1) component_to_build.nature_= LataDBField::SCALAR;

          LataField_base& other_field = component_to_build;

          Field<FloatTab> * latafld_ptr = dynamic_cast<Field<FloatTab> *> (&other_field);

          latafld_ptr->data_.resize(nelem,shape);

	  for (int elem=0; elem<nelem; elem++)      
	    if (Reactors.reactorsPtr(reactor)->geometry().is_element(elem))
	      for (unsigned int dim=0; dim<shape; dim++)
		latafld_ptr->data_(elem,dim) = partial_component.values()[dim];
	
	}//end for on reactor

      if (add_compo) 
	(*latafields_).push_back(component_to_build);

    }//end for on reactor_component



  /* Creating new components */
  /* with reactors number */
  Field<FloatTab> component_to_build;

  component_to_build.localisation_ = LataField_base::ELEM;
  //component_to_build.component_names_.add("REACTORS_NUMBERS_ELEM");
  component_to_build.component_names_.add("REACTORS_NUMBERS");
  component_to_build.id_ = Field_Id(Field_UName(domain_->id_.name_,
						"REACTORS_NUMBERS", 
						"ELEM"),
				    1 /* timestep */,
				    -1 /* block */);

  component_to_build.nature_= LataDBField::SCALAR;

      LataField_base& component_to_build_ref=component_to_build;

      Field<FloatTab> * latafld_ptr = dynamic_cast<Field<FloatTab> *> (&component_to_build_ref);

      latafld_ptr->data_.resize(nelem,1);

  for (unsigned int reactor=0; reactor<reactors_size; reactor++)
    {
      const ArrOfBit& reactor_elements = Reactors.reactorsPtr(reactor)->geometry().elements();

      for (int elem=0; elem<nelem; elem++)
	if (reactor_elements[elem]) 
	  latafld_ptr->data_(elem,0) = Reactors.reactorsPtr(reactor)->number();

    }//end for on reactor

  (*latafields_).push_back(component_to_build);



  /* Updating components names known */
  update_components_names();

}


void component_builder::add_nbplaces()
{
  assert(domain_!=NULL);

  const unsigned int size = names_.size();
  assert(vector_of_parameters_names_.size()==size);
  assert(labels_.size()==size);

  vector_of_nbplaces_.resize(size);

  for (unsigned int i=0; i<size; i++)
    {
      const string& label = labels_[i];
      assert(vector_of_parameters_names_[i].size()!=0);
      
      const string& component_name = vector_of_parameters_names_[i][0];
      const unsigned int where = search_component_name(component_name+"_"+label);
      assert(where>=0);

      LataField_base& other_field=(*latafields_)[where];

      const Field<FloatTab> * latafld_ptr = dynamic_cast<const Field<FloatTab> *> (&other_field);
      
      vector_of_nbplaces_[i] = latafld_ptr->data_.dimension(0);
    }
}
 
void component_builder::add_shape()
{
  assert(domain_!=NULL);

  const unsigned int size = names_.size();
  assert(vector_of_parameters_names_.size()==size);
  assert(labels_.size()==size);  

  vector_of_shape_.resize(size);

  for (unsigned int i=0; i<size; i++)
    {
      const string& label = labels_[i];
      assert(vector_of_parameters_names_[i].size()!=0);
      
      const string& component_name = vector_of_parameters_names_[i][0];
      const unsigned int where = search_component_name(component_name+"_"+label);
      assert(where>=0);

      LataField_base& other_field=(*latafields_)[where];

      const Field<FloatTab> * latafld_ptr = dynamic_cast<const Field<FloatTab> *> (&other_field);
      
      vector_of_shape_[i] = latafld_ptr->data_.dimension(1);
    }
}

void component_builder::update_components_names() const
{
  assert(domain_!=NULL);
  
  const vector<Field<FloatTab> >& latafields = *latafields_;
  const unsigned int components_size = latafields.size();
  
  for (unsigned int i=0; i<components_size; i++)
    components_names_[custom_field_name((*latafields_)[i])]=i+1;
}

LataField_base::Elem_som component_builder::identity(const string& s) const
{
  string lower_s = s;
  to_lower(lower_s);

  if (lower_s=="elem") return LataField_base::ELEM;
  if (lower_s=="som") return LataField_base::SOM;
  if (lower_s=="faces") return LataField_base::FACES;

  cerr << "Error in component_builder::identity()" << endl;
  cerr << "String2 " << lower_s << " is not a known Elem_som members" << endl;
  cerr << "Elem_som members known are : elem, som, faces" << endl;
  cerr << "Exiting program" << endl;
  exit(-1);
}
