
#ifndef LataLoader_H
#define LataLoader_H


#include <vector>
#include "LataFilter.h"
namespace ParaMEDMEM {
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

    ParaMEDMEM::MEDCouplingMesh     *GetMesh(const char *varname,int timestate, int block=-1);
    //ParaMEDMEM::DataArray  *GetVar(int, int, const char *);
    ParaMEDMEM::DataArray  *GetVectorVar(int, int, const char *);
    void GetTimes(std::vector<double>& times);
    ParaMEDMEM::MEDCouplingFieldDouble*  GetFieldDouble(const char *varname,int timestate, int block=-1);
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
