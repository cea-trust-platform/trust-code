#ifndef ReaderFORT21_HeaderFile
#define ReaderFORT21_HeaderFile

#include <fstream>
#include <list>
#include <map>
#include <stdio.h>
#include <string>
#include <vector>

#ifdef WNT

#define file_pos_t __int64
#define open_file fopen
#define get_file_pos _ftelli64
#define set_file_pos _fseeki64

#else

#define open_file fopen64
#define file_pos_t __off64_t
#define get_file_pos ftello64
#define set_file_pos fseeko64

#endif
/*
static inline int findFromEnd( const std::string& theStr, const std::string& theWhat )
{
  size_t pos = theStr.find_last_of( theWhat );
  if ( pos == std::string::npos )
    pos = -1;
  return int(pos);
}
*/
static inline int find(const std::string& theStr, const std::string& theWhat)
{
  size_t pos = theStr.find(theWhat);
  if (pos == std::string::npos)
    pos = -1;
  return int(pos);
}
//! Platform where FORT21 file has been generated. It is similar to GUTHAREplatforms.
enum Platform
{
  Windows_Linux_32 = 0,  // 32-bits Windows/Linux (Intel/AMD architecture)         r = 4 l = 4 d = 4
  Linux_64f90 = 1,       // 64-bits Linux (f90, f95)                               r = 4 l = 8 d = 4
  Linux_64gcc = 2,       // 64-bits Linux (gcc, g77, gnu)                          r = 8 l = 4 d = 4
  Tru64_64f90 = 3,       // Tru64 UNIX (f90)                                       r = 4 l = 8 d = 4
  Unix_64f77 = 4,        // Tru64 UNIX (f77)                                       r = 4 l = 8 d = 8
  SGI_32 = 5,            // 32-bits SGI (big ebdian)                               r = 4 l = 4 d = 4
  IBM_Tech = 6,          // 32-bits IBM RS6000 (OS: AIX)                           r = 4 l = 4 d = 4
  Linux_Tech = 7         // 32-bits IBM Blade server (OS: Red Hat Enterprise AS4)  r = 4 l = 4 d = 8
};

#include <iostream>
#include <vector>
/*! @brief Mesh data stored internally as a BasicMesh object.
 *
 *  This structure contains all the necessary information
 *  for constructing MEDCouplingFieldDouble (with the addition of the MPI
 *  communicator).
 */

//! This class provides services for parsing FORT21 file
class ReaderFORT21
{
public:
  //! CATHARE data types
  enum CathareType
  {
    T_PTR = 0x00,
    T_INT = 0x01,
    T_REAL = 0x02,
    T_CHAR = 0x04,
    T_UNDEF = 0x08
  };
  enum LocalisationField
  {
    L_UNDEF = 0x01,
    L_ELEM = 0x02,
    L_DUAL = 0x04,
    L_GLOBAL = 0x0F,
    L_TWALL = 0x10
  };


  //! Sets information about platform where file was written
  enum Status
  {
    NotDone = -1,
    Done,
    Fail
  };

public:
  // ---------- PUBLIC METHODS ----------

  //! Empty constructor
  explicit ReaderFORT21(int verbosity = 0);

  //! Destructor
  ~ReaderFORT21();

  //! Sets the file to parse
  void setFile(const std::string& theName);

  //! parses the file
  void parse();
  void fix_bad_times();

  //! Returns the status of parsing, see below enum
  ReaderFORT21::Status getStatus() const;

  //! Set platform where FORT21 file has been generated
  void setPlatform(const Platform thePlatform);

protected:
  //! Represents a field in the file; it is written as:
  //!   size - integer, the size of field
  //!   char[size] - field data
  //!   size - once more the same size
  //! The instance of this class is resized as needed to store
  //! the next portion of the file
  class Field
  {
  public:
    Field()
      : myFile(0)
      , myData(0)
      , myCapacity(0)
      , mySize(0)
    {
      setPlatform(Windows_Linux_32);
    }

    ~Field()
    {
      if (myData != 0)
        delete[] myData;
      setFile(0);
    }

    void setFile(FILE* theFile)
    {
      if (myFile != 0)
        fclose(myFile);
      myFile = theFile;

      myFPos = myFile ? get_file_pos(myFile) : 0;
    }

    void setMaxFileInt(int theMaxSizeInt)
    {
      myMaxSizeInt = theMaxSizeInt;
    }

    FILE* getFile() const
    {
      return myFile;
    }

    file_pos_t getFilePos() const
    {
      return myFPos;
    }

    bool read(int theSize = 0);

    int size() const
    {
      return mySize;
    }

    char* getData() const
    {
      return myData;
    }

    void clear()
    {
      mySize = 0;
    }

    void setPlatform(const Platform thePlatform)
    {
      myPlatform = thePlatform;
    }

    Platform getPlatform() const
    {
      return myPlatform;
    }

  private:
    FILE* myFile;
    file_pos_t myFPos;
    char* myData;
    int myCapacity;
    int mySize;
    int myMaxSizeInt;

    Platform myPlatform;
  };

  bool readRecord();
  bool readHeader();
  bool readDescr();
  bool readVaria();
  bool readDesStack(bool theSkip);

public:
  //!  Codes for meaning of variables in FORT21 file.
  enum MeshType
  {
    MESH_Unknown = 0,  //!< Fictive mesh (port, reactor, circuit)
    MESH_Polygone = 1,   //!< for volum, axial
    MESH_Hexa   =2  //!< for threed
  };

  //! Get meaning of CATHARE variable.

private:
  Status myStatus;
  int myRealSize;
  int myMaxSizeInt;
  Field myField;
  std::string myInFile;

  bool myIsC3;
  bool myIsSimpleReal;

  std::vector<double> Times_glob_;
  std::vector<file_pos_t> offset_a_time_;

public:
  class FieldInfo
  {
    friend ReaderFORT21;

  public:
    int size() const
    {
      return size_;
    }
    int nb_parts() const
    {
      return nb_parts_;
    }
    // mettre un enum
    CathareType type_of_field() const
    {
      return type_of_field_;
    }
    LocalisationField localisation_of_field() const
    {
      return localisation_of_field_;
    }

  private:
    int size_;
    CathareType type_of_field_;
    LocalisationField localisation_of_field_;
    file_pos_t offset_rel_stack_;
    int nb_parts_;
  };

private:
  class ElementInfo
  {
  public:
    std::string type_;
    file_pos_t offset_const_;
    std::vector<int> index_times_;
    std::vector<file_pos_t> offset_rel_time_;
    std::map<std::string, FieldInfo> ConstFields_;
    std::map<std::string, FieldInfo> VarFields_;
    file_pos_t sizeblock_ = -1;
  };
  std::map<std::string, ElementInfo> elements_list_;
  int verbosity_;

  const ElementInfo& getElementInfo(const std::string& name_stack) const;
  const FieldInfo& getVarFieldInfo(const ElementInfo& eleminfo, const std::string& name_field) const;
  const FieldInfo& getConstFieldInfo(const ElementInfo& eleminfo, const std::string& name_field) const;

public:
  const FieldInfo& getVarFieldInfo(const std::string& name_stack, const std::string& name_field) const;
  const FieldInfo& getConstFieldInfo(const std::string& name_stack, const std::string& name_field) const;
  std::vector<double> getTimes() const
  {
    return Times_glob_;
  }
  std::vector<double> getTimesStack(const std::string& name_stack) const;
  // std::vector<int> getTimesIndexStack(const std::string& name_stack) const;
  std::vector<std::string> getElementNames() const;
  std::string getElementType(const std::string& name_stack) const;
  std::vector<std::string> getVarFieldNames(const std::string& name_stack) const;
  template <typename _TYPE_> void getValuesVarField(const std::string& name_stack, const std::string& name_field, std::vector<_TYPE_>& data, const int& id_time_field) const;
  template <typename _TYPE_> void getInterpolatedValuesVarField(const std::string& name_stack, const std::string& name_field, std::vector<_TYPE_>& data, const int& global_id_time) const;


  file_pos_t getOffsetVarField(const std::string& name_stack, const std::string& name_field, const int& id_time_field) const;
  std::vector<std::string> getConstFieldNames(const std::string& name_stack) const;
  template <typename _TYPE_> void getValuesConstField(const std::string& name_stack, const std::string& name_field, std::vector<_TYPE_>& data) const;
  class BasicMesh
  {
  public:
    /*! @brief Builds an empty mesh.
     */
    BasicMesh();

    /*! @brief Destructor.
     */
    ~BasicMesh();

    /*! @brief clear and reset all internal data structures.
     *
     * After the call to clear(), all pointers are null
     * Arrays are deleted if necessary
     */
    void clear();

    /*! @brief Used to simulate a 0D geometry (Cathare/Trio for example).
     */
    void dummy_geom();

    /*! @brief Save mesh to a .mesh file
     */
    void save(std::ostream& os) const;

    /*! @brief Restore mesh from a .mesh file
     */
    void restore(std::istream& in);

  public:
    MeshType type_mesh_;                  ///< 0 polygone 1 quadri ....
    int mesh_dim_;                   ///< Mesh dimension
    int space_dim_;                  ///< Space dimension
    int nbnodes_;                    ///< Number of nodes
    int nodes_per_elem_;             ///< Number of nodes for a single element
    int nb_elems_;                   ///< Number of elements
    std::vector<int> connectivity_;  ///< Nodal connectivity array
    std::vector<double> coords_;     ///< Coordinate array
  };
  BasicMesh getMeshStack(const std::string& name_stack) const;
};
#endif
