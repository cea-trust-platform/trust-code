
#ifndef LataLoader_H
#define LataLoader_H


#include <vector>
#include "LataFilter.h"
namespace MEDCoupling {
class MEDCouplingMesh;
class DataArray;
class  MEDCouplingFieldDouble;
}

// ****************************************************************************
//  Class: LataLoader
//
//  Purpose:
//
//
// ****************************************************************************

class LataLoader 
{
  public:
    LataLoader(const char *) throw(LataDBError);
    ~LataLoader();

    int            GetNTimesteps(void);

    const char    *GetType(void)   { return "lata"; };
    void           FreeUpResources(void); 

    MEDCoupling::MEDCouplingMesh     *GetMesh(const char *varname,int timestate, int block=-1);
    //MEDCoupling::DataArray  *GetVar(int, int, const char *);
    MEDCoupling::DataArray  *GetVectorVar(int, int, const char *);
    void GetTimes(std::vector<double>& times);
    inline std::vector<double> getTimes() { std::vector<double> a; GetTimes(a);return a; };
    MEDCoupling::MEDCouplingFieldDouble*  GetFieldDouble(const char *varname,int timestate, int block=-1);
    std::vector<std::string> GetMeshNames();
    
    std::vector<std::string> GetFieldNames();
    std::vector<std::string> GetFieldNamesOnMesh(const std::string& );
  protected:
    // DATA MEMBERS

    void    PopulateDatabaseMetaData( int);

    void register_fieldname(const char *visit_name, const Field_UName &, int component);
    void register_meshname(const char *visit_name, const char *latafilter_name);
    void get_field_info_from_visitname(const char *varname, Field_UName &, int & component) const;

    LataDB      lata_db_; // Source database
    LataFilter  filter_; // Data processor and cache
    Noms field_username_;
    Field_UNames field_uname_;

    Noms mesh_username_;
    Noms mesh_latafilter_name_;
    Nom filename;
    // For each name, which component is it in the source field:
    LataVector<int> field_component_;
};


#endif
