#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <endian.h>

#define PATH_SEPARATOR '/'
#define DEFAULT_EXPORT_BINARY 1

#ifndef __BYTE_ORDER
#error "Byte order not defined."
#endif
#if (__BYTE_ORDER == __BIG_ENDIAN)
const int mymachine_msb =  1;
#elif (__BYTE_ORDER == __LITTLE_ENDIAN)
const int mymachine_msb =  0;
#else
#error "Byte order is neither __BIG_ENDIAN nor __LITTLE_ENDIAN : "
#endif

#define MAX_NAME 1000
#define MAX_COMPONENTS 100

int verbosity = 0; // Level of verbosity (default is "shut up")

enum Elem_som { chkfile_name, elem, som };

typedef struct {
  char name[100];
  int rank; // 0=scalar, 1=vector
  int shape; // Vector size (-1 => dimension of the problem)
  Elem_som elem_som;
} StdComponents;

const StdComponents std_components[] =
{ { "pression", 0, 0, chkfile_name},
  { "vitesse", 1, -1, chkfile_name },
  { "Moyenne_vitesse", 1, -1, chkfile_name },
  { "Ecart_type_vitesse", 1, -1, chkfile_name },
  { "concentration", 0, 0, chkfile_name },
  { "indicatrice", 0, 0, chkfile_name },
  { "temperature", 0, 0, chkfile_name },
  { "Moyenne_temperature", 0, 0, chkfile_name },
  { "Ecart_type_temperature", 0, 0, chkfile_name },
  { "viscosite_turbulente", 0, 0, chkfile_name },
  { "masse_volumique", 0, 0, chkfile_name },
  { "k", 0, 0, chkfile_name },
  { "Critere_Q", 0, 0, chkfile_name },
  { "y_plus", 0, 0, chkfile_name },
  { "vorticite", 1, -1, chkfile_name },
  { "Moyenne_pression", 1, -1, chkfile_name }, 
  { "conductivite", 0, 0, chkfile_name },
  { "tsat", 0, 0, chkfile_name },
  { "delta_h", 0, 0, chkfile_name },
  { "delta_indicatrice", 0, 0, chkfile_name },
  { "surface_interf", 0, 0, chkfile_name },
  { "divergence_u", 0, 0, chkfile_name },
  { "potentiel_chimique_generalise", 0, 0, chkfile_name },
  { "", 0, 0, chkfile_name } // Empty label means end of the table
};

double lata_version = 0;

int interfaces_present = 0;

// Function tests if the lata file version string matches this
// filter version. If not, does not return.
typedef char charray[100];
void TestTrioVersion(char *version)
{
  // List of valid version strings, list ends with ""
  double num[] = {1.302,1.4};
  charray version_string[] =
  {
   "TRUST Version1.3.2",
   "TRUST Version1.4",
   ""}; 
  charray *ptr;
  int i;
  for (ptr = version_string, i=0; strlen(*ptr) > 0; ptr++, i++)
    if (strncmp(version, *ptr, strlen(*ptr)) == 0) {
      lata_version = num[i];
      return; // Ok, we can read this lata file
    }
  fprintf(stderr, "Error : lata version is %s\n", version);
  fprintf(stderr, "This filter can only read the following lata versions\n");
  fprintf(stderr, "(beginning of version string must match one of these) :\n");
  for (ptr = version_string; strlen(*ptr) > 0; ptr++)
    fprintf(stderr, "%s\n", *ptr);
  exit(-1);    
}

void myerror(const char *const s)
{
  fprintf(stderr, s);
  assert(0); // To help debugging
  exit(-1);
}

void* malloctest(unsigned int size)
{
  void *p = 0;
  p = malloc(size);
  if (!p) {
    fprintf(stderr, "Could not allocate %d bytes\n", size);
    assert(0);
    exit(-1);
  }
  return p;
}

#define msg(level, a) {if (level <= verbosity) fprintf(stderr, a);}
#define msg1(level, a, b) {if (level <= verbosity) fprintf(stderr, a, b);}
#define msg2(level, a, b, c) {if (level <= verbosity) fprintf(stderr, a, b, c);}

enum Element {triangle, quad, tetra, hexa, unspecified};

class Domain
{
public:
  char *name;
  char *file;
  int nsom;
  int nelem;
  int binary;
  Element elt_type;
};

class Options
{
public:
  char *basename; // Name of the case
  char *path_prefix; // Path for the case
  int geometry; // Include positions and connections ?
  int inclcomponents; // Include components ?
  int ntimesteps, ndomains, ncomponents;
  int *timesteps; // List of timesteps to import
  char **domains; // List of domains to import
  char **components; // List of components to import
  int maxtimesteps, maxdomains, maxcomponents;
  Element elt_type; // Type of the elements for all domains
  // (not included in the current lata file format)
  int binary; // Are lata files binary ?
  int msb;    // Is the source binary msb ?
  int intsize;  // 4 or 8
  int realsize; // 4 or 8
  int binary_out;
  int forcegroup; // Include fields in a group even if only 1 domain
  int nosuffix; // Suppress _ELEM or _SOM in component names
  Options(int, char **);
  ~Options();
};


Options::~Options()
{
  free (timesteps); timesteps = 0;
  free (domains);   domains = 0;
  free (components);components = 0;
}

// Default options are : import geometry, alltimesteps, 
// alldomains, allcomponents.
Options::Options(int argc, char **argv) :
  basename(0), 
  geometry(1), 
  inclcomponents(1),
  ntimesteps(0), ndomains(0), ncomponents(0),
  timesteps(0), domains(0), components(0),
  maxtimesteps(0), maxdomains(0), maxcomponents(0),
  forcegroup(0), nosuffix(0)
{
  int i;
  elt_type = unspecified;
  binary = 0;
  binary_out = DEFAULT_EXPORT_BINARY;
  // Default endian : current machine, current compile option
  msb = mymachine_msb;

  // Parse options
  for (i = 2; i < argc; i++) {
    if (strncmp(argv[i], "verbosity=", 10) == 0) {
      char *ptr;
      verbosity = strtol(argv[i]+10, &ptr, 10);
      if (ptr == argv[i]+10) {
	fprintf(stderr, "Bad argument : %s\n", argv[i]);
	exit(-1);
      }
    }
  }

  msg1(2,"(Import) Default byte order : %s\n", 
       msb ? "msb (little endian)" : "lsb (big endian)");
  intsize = sizeof(int);
  msg1(2,"(Import) Default int type is signed %d-byte\n", intsize);
  realsize = sizeof(float);
  msg1(2,"(Import) Default real type is %d-byte\n", realsize);

  if (argc < 2) {
    fprintf(stderr,"Usage : lata_dx_filter basename [geometry|components]\n");
    fprintf(stderr," [triangle|quad|tetra|hexa] [binary|ascii] [binout|asciiout]\n");
    fprintf(stderr," [forcegroup] [nosuffix]\n");
    fprintf(stderr," [bigendian|littleendian] [int32|int64]\n");
    fprintf(stderr," [timestep=n][domain=name][component=name][verbosity=n]\n");
    exit(-1);
  }
  //Extract path
  for (i = strlen(argv[1])-1; i>=0; i--)
    if (argv[1][i] == PATH_SEPARATOR)
      break;
  if (i>=0) {
    path_prefix = (char*) malloc(i+2);
    strncpy(path_prefix, argv[1], i+1);
    path_prefix[i+1] = 0;
  } else {
    path_prefix = "";
  }
  // Parse basename : if extension given, remove it
  basename = (char*) malloc(strlen(argv[1]+1));
  if (strlen(argv[1]) > 5 && strcmp(argv[1]+strlen(argv[1])-5, ".lata") == 0) {
      strncpy(basename, argv[1]+i+1, strlen(argv[1]) - (i+1) - 5);
      basename[strlen(argv[1]) - (i+1) - 5] = 0;
  } else {
    strncpy(basename, argv[1]+i+1, strlen(argv[1]) - (i+1));
    basename[strlen(argv[1]) - (i+1)] = 0;
  }
      
  msg1(2, "Basename : %s\n", basename);
  msg1(2, "Prefix   : %s\n", path_prefix);

  // Parse options
  for (i = 2; i < argc; i++) {
    if (strncmp(argv[i], "verbosity=", 10) == 0) {
      // already done
    } else if (strcmp(argv[i], "geometry") == 0) {
      geometry = 1;  // Read geometry only
      inclcomponents = 0;
    } else if (strcmp(argv[i], "components") == 0) {
      geometry = 0;  // Read components only
      inclcomponents = 1;
    } else if (strcmp(argv[i], "nosuffix") == 0) {
      nosuffix=1;
    } else if (strcmp(argv[i], "triangle") == 0) {
      elt_type=triangle;
    } else if (strcmp(argv[i], "quad") == 0) {
      elt_type=quad;
    } else if (strcmp(argv[i], "tetra") == 0) {
      elt_type=tetra;
    } else if (strcmp(argv[i], "hexa") == 0) {
      elt_type=hexa;
    } else if (strcmp(argv[i], "binary") == 0) {
      binary=1;
    } else if (strcmp(argv[i], "ascii") == 0) {
      binary=0;
    } else if (strcmp(argv[i], "binout") == 0) {
      binary_out=1;
    } else if (strcmp(argv[i], "asciiout") == 0) {
      binary_out=0;
    } else if (strcmp(argv[i], "forcegroup") == 0) {
      forcegroup=1;
    } else if (strcmp(argv[i], "bigendian") == 0) {
      msb=0;
    } else if (strcmp(argv[i], "littleendian") == 0) {
      msb=1;
    } else if (strcmp(argv[i], "int32") == 0) {
      intsize=4;
    } else if (strcmp(argv[i], "int64") == 0) {
      intsize=8;
    } else if (strncmp(argv[i], "timestep=", 9) == 0) {
      char *ptr;
      int t = strtol(argv[i]+9, &ptr, 10);
      if (ptr == argv[i]+9) {
	fprintf(stderr, "Bad argument : %s\n", argv[i]);
	exit(-1);
      }
      if (maxtimesteps <= ntimesteps) {
	maxtimesteps += 100;
	timesteps = (int*) realloc(timesteps, sizeof(int) * maxtimesteps);
      }
      timesteps[ntimesteps] = t;
      ntimesteps++;
    } else if (strncmp(argv[i], "domain=", 7) == 0) {
      // Check for duplicate domain name
      int stop = 0;
      for (i=0; i<ndomains; i++)
	if (strcmp(domains[ndomains], argv[i]+7)==0)
	  stop = 1;
      if (stop) {
	fprintf(stderr, "Warning: duplicate option ignored (%s)\n",
		argv[i]);
      } else {
	if (maxdomains <= ndomains) {
	  maxdomains += 100;
	  domains = (char**) realloc(domains, sizeof(char*) * maxdomains);
	}
	domains[ndomains] = argv[i]+7;
	ndomains++;
      }
    } else if (strncmp(argv[i], "component=", 10) == 0) {
      if (maxcomponents <= ncomponents) {
	maxcomponents += 100;
	components = (char**) realloc(components, 
				      sizeof(char*) * maxcomponents);
      }
      components[ncomponents] = argv[i]+10;
      ncomponents++;
    } else {
      fprintf(stderr, "Unknown option : %s\n", argv[i]);
      exit(-1);
    }
  }
  if (verbosity >= 1) {
    fprintf(stderr, "Lata_dx_filter options :\n");
    fprintf(stderr, " Verbosity level = %d\n", verbosity);
    fprintf(stderr, " Forcegroup = %d\n", forcegroup);
    fprintf(stderr, " Geometry is %sincluded\n", geometry ? "" : "not ");
    fprintf(stderr, " Components are %s included\n", 
	    inclcomponents ? "" : "not ");
    fprintf(stderr, " List of timesteps to import :\n");
    if (ntimesteps == 0)
      fprintf(stderr, "  all timesteps\n");
    else
      for (i = 0; i < ntimesteps; i++)
	fprintf(stderr, "  %d\n", timesteps[i]);
    fprintf(stderr, "List of domains to import :\n");
    if (ndomains == 0)
      fprintf(stderr, "  all domains\n");
    else
      for (i = 0; i < ndomains; i++)
	fprintf(stderr, "  %s\n", domains[i]);
    fprintf(stderr, "List of components to import :\n");
    if (ncomponents == 0)
      fprintf(stderr, "  all components\n");
    else
      for (i = 0; i < ncomponents; i++)
	fprintf(stderr, "  %s\n", components[i]);
    fprintf(stderr, "end of options summary\n");
  }
}

class ExportFormat
{
public:
  int binary;
  int msb;
  int intsize;
  int realsize;
  ExportFormat(Options &opt);
};

// Set default values
ExportFormat::ExportFormat(Options &opt)
{
  msb = mymachine_msb;
  binary = opt.binary_out; // Ascii output for debug
  intsize = sizeof(int);
  realsize = sizeof(float);
}

void skip_blocksize(FILE *f, int binary, int intsize)
{
  char s[1000];
  if (binary) {
    fread(s, intsize, 1, f);
    msg(3, "Skip binary bloc size\n");
  } else {
    fgets(s, 1000, f);
    msg1(3, "Skip ascii bloc size : %s", s);
  }
}

void lata_read_positions(Options &opt, FILE *geom, Domain &dom, int &realsize,
			 void * &data)
{
  char s[1000];
  // Read positions
  if (opt.binary == 0) {
    if (lata_version < 1.3) {
      fgets(s, 1000, geom);
      if (strcmp(s, "GRILLE") != 0) {
	fprintf(stderr, "First line of geom file is not GRILLE. Line contains:\n");
	fprintf(stderr, "%s\n", s);
	exit(-1);
      }
      fgets(s, 1000, geom);
      int n1, n2;
      if (sscanf(s, "%*s %d %d\n", &n1, &n2) != 3) {
	fprintf(stderr, "Error on line 2 in geom file. Line contains:\n");
	fprintf(stderr, s);
	exit(-1);
      }
    } else {
      skip_blocksize(geom, opt.binary, opt.intsize);
    }
    realsize = sizeof(float);
    data = (void*) malloctest (sizeof(float) * 3 * dom.nsom);
    float *fltdata = (float*) data;
    int i;
    for (i = 0; i < dom.nsom; i++) {
      fgets(s, 1000, geom);
      if (sscanf(s, "%f%f%f", fltdata, fltdata+1, fltdata+2) != 3) {
	fprintf(stderr, "Error reading point number %d. Line reads:\n%s\n",
		i, s);
	exit(-1);
      }
      fltdata += 3;
    }
    if (dom.elt_type == triangle || dom.elt_type == quad) {
      // 2D geometry has more points. Skip them
      for (i = 0; i < dom.nsom; i++) {
	fgets(s, 1000, geom);
      }
    }
    if (lata_version >= 1.3) {
      skip_blocksize(geom, opt.binary, opt.intsize);
    }
  } else {
    if (lata_version < 1.3) {
      myerror ("Check binary import for version < 1.3\n");
    } else {
      skip_blocksize(geom, opt.binary, opt.intsize);
      realsize = opt.realsize;
      data = (void*) malloctest (opt.realsize * 3 * dom.nsom);
      if (opt.msb == mymachine_msb) {
	int nread;
	nread = fread(data, opt.realsize * 3, dom.nsom, geom);
	if (nread != dom.nsom) {
	  fprintf(stderr, "Error reading binary positions, %d elements read\n", nread);
	  fprintf(stderr, "(%d elts expected).\n", dom.nsom);
	  exit(-1);
	}
      } else {
	myerror("Import positions: byte swap not implemented\n");
      }
      if (dom.elt_type == triangle || dom.elt_type == quad) {
	// 2D geometry has more points. Skip them
	if (fseek(geom, opt.realsize * 3 * dom.nsom, SEEK_CUR) != 0)
	  myerror("Read binary positions : fseek failed\n");
      }
      skip_blocksize(geom, opt.binary, opt.intsize);
    }
  }
}

void lata_read_connections(Options &opt, FILE *geom, Domain &dom,
			   void * &data, int &shape)
{
  char s[1000];
  int elem_nodes = 0;
  switch(dom.elt_type) {
  case triangle: elem_nodes = 3; break;
  case quad: elem_nodes = 4; break;
  case tetra: elem_nodes = 4; break;
  case hexa: elem_nodes = 8; break;
  default: myerror("lata_dx_filter internal error.\n");
  }
  shape = elem_nodes;
  int multiply_alloc = 1;
  if (dom.elt_type == triangle || dom.elt_type == quad)
    multiply_alloc = 2; // 2D cases have twice more nodes per element
  data = (void*) malloctest (sizeof(int) * elem_nodes * dom.nelem * multiply_alloc);

  // Read connections
  if (opt.binary == 0 && lata_version < 1.3) {
    char s[1000];
    fgets(s, 1000, geom);
    if (strcmp(s, "TOPOLOGIE") != 0) {
      fprintf(stderr, "Read connections, expected keyword TOPOLOGIE. Line contains:\n");
      fprintf(stderr, "%s\n", s);
      exit(-1);
    }
    fgets(s, 1000, geom); // Skip line
    fgets(s, 1000, geom); // Read "MAILLE"
    fgets(s, 1000, geom); // Read number of elements
    int n;
    if (sscanf(s, "%d", &n) != 1) {
      fprintf(stderr, 
	      "Read connections, cannot read number of elements. Line contains:\n");
      fprintf(stderr, s);
      exit(-1);
    }
  } else {
    skip_blocksize(geom, opt.binary, opt.intsize);
  }

  if (opt.binary == 0) {
    int *intdata = (int*) data;
    int i, a1, a2, a3, a4, a5, a6, a7, a8;
    switch(dom.elt_type) {
    case triangle: 
      for (i = 0; i < dom.nelem; i++) {
	if (fgets(s, 1000, geom) == 0) myerror("Not enough lines in geom file!\n");
	int nread;
	if (lata_version < 1.3) 
	  { myerror("Erreur !\n"); }
	else
	  nread = sscanf(s, "%d%d%d", &a1, &a2, &a3);
	if (nread != 3) {
	  fprintf(stderr, "Error reading element number %d. Line reads:\n%s\n",
		  i, s);
	  exit(-1);
	}
	intdata[0] = a1-1;
	intdata[1] = a2-1;
	intdata[2] = a3-1;
	intdata += 3;
      }
      break;      
    case quad:
      for (i = 0; i < dom.nelem; i++) {
	if (fgets(s, 1000, geom) == 0) myerror("Not enough lines in geom file!\n");
	int nread;
	if (lata_version < 1.3) 
	  nread = sscanf(s, "VOXEL8%d%d%d%d", &a1, &a2, &a3, &a4);
	else
	  nread = sscanf(s, "%d%d%d%d", &a1, &a2, &a3, &a4);
	if (nread != 4) {
	  fprintf(stderr, "Error reading element number %d. Line reads:\n%s\n",
		  i, s);
	  exit(-1);
	}
	intdata[0] = a1-1;
	intdata[1] = a2-1;
	intdata[2] = a3-1;
	intdata[3] = a4-1;
	intdata += 4;
      }
      break;
    case tetra:
      for (i = 0; i < dom.nelem; i++) {
	if (fgets(s, 1000, geom) == 0) 
	  myerror("Not enough lines in geom file!\n");
	int nread;
	if (lata_version < 1.3) {
	  myerror("Check this feature : import tetra version <1.3\n");
	  nread = sscanf(s, "%d%d%d%d", 
			 &a1, &a2, &a3, &a4);
	} else
	  nread = sscanf(s, "%d%d%d%d", 
			 &a1, &a2, &a3, &a4);
	if (nread != 4) {
	  fprintf(stderr, "Error reading element number %d. Line reads:\n%s\n",
		  i, s);
	  exit(-1);
	}
	intdata[0] = a1-1;
	intdata[1] = a2-1;
	intdata[2] = a3-1;
	intdata[3] = a4-1;
	intdata += 4;
      }      
      break;
    case hexa:
      for (i = 0; i < dom.nelem; i++) {
	if (fgets(s, 1000, geom) == 0) myerror("Not enough lines in geom file!\n");
	int nread;
	if (lata_version < 1.3) {
	  myerror("Check this feature : import hexa version <1.3\n");
	  nread = sscanf(s, "VOXEL8%d%d%d%d%d%d%d%d", 
			 &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8);
	} else
	  nread = sscanf(s, "%d%d%d%d%d%d%d%d", 
			 &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8);
	if (nread != 8) {
	  fprintf(stderr, "Error reading element number %d. Line reads:\n%s\n",
		  i, s);
	  exit(-1);
	}
	intdata[0] = a1-1;
	intdata[1] = a2-1;
	intdata[2] = a3-1;
	intdata[3] = a4-1;
	intdata[4] = a5-1;
	intdata[5] = a6-1;
	intdata[6] = a7-1;
	intdata[7] = a8-1;
	intdata += 8;
      }
      break;
    default: myerror("Read connections: connection type not implemented\n");
    }
  } else {
    if (opt.intsize != sizeof(int)) {
      myerror("Import connections: int size conversion not implemented\n");
    }
    unsigned int nread;
    nread = fread (data, sizeof(int) * elem_nodes * dom.nelem * multiply_alloc,
		   1, geom);
    if (nread != 1) {
      myerror ("Error reading binary connections.\n");
    }
    int i, j;
    int *intdata = (int*) data;
    int *ptr1 = intdata, *ptr2 = intdata;
    if (dom.elt_type == triangle || dom.elt_type == quad) {
      for (i = 0; i < dom.nelem; i++) {
	for (j = 0; j < elem_nodes; j++)
	  ptr1[j] = ptr2[j]-1;
	ptr1 += elem_nodes;
	ptr2 += elem_nodes * 2;
      }
    } else {
      for (i = 0; i < dom.nelem * elem_nodes; i++)
	intdata[i] -= 1;
    }
  }
  if (lata_version >= 1.3 || opt.binary == 1) {
    skip_blocksize(geom, opt.binary, opt.intsize);
  }
}

void export_real_array(ExportFormat &fmt, int &realsize, void * &data,
		       int rank, int shape, int items, int object)
{
  printf("object %d class array\n", object);
  
  printf("type %s rank %d ", 
	 (fmt.realsize == 4) ? "float" : "double",
	 rank);
  if (rank > 1)
    myerror ("export_real_array: rank>1 not implemented\n");
  if (rank > 0)
    printf("shape %d ", shape);
  else
    shape = 1;
  printf("items %d\n", items);
  printf("%s data follows\n",
	 (fmt.binary) ? (fmt.msb ? "msb ieee" : "lsb ieee") : "ascii");

  if (fmt.realsize != realsize) {
    myerror("export_real_array: size conversion not implemented\n");
  }
  if (fmt.binary) {
    if (fmt.msb == mymachine_msb) {
      // Raw dump the data
      fwrite(data, realsize * shape, items, stdout);
    } else {
      fprintf (stderr, "export_real_array: byte swap not implemented.\n");
      exit(-1);
    }
  } else {
    int i, j;
    if (fmt.realsize == 4) {
      float *p = (float*) data;
      for (i = 0; i < items; i++) {
	for (j = 0; j < shape; j++) {
	  printf("%9e ", (double) *p);
	  p++;
	}
	printf("\n");
      }
    } else {
      double *p = (double*) data;
      for (i = 0; i < items; i++) {
	for (j = 0; j < shape; j++) {
	  printf("%17e ", *p);
	  p++;
	}
	printf("\n");
      }
    }
  }
}
void export_int_array(ExportFormat &fmt, void * &data,
		      int rank, int shape, int items, int object)
{
  printf("object %d class array\n", object);
  if (sizeof(int) != 4) {
    myerror("export_int_array: Check int type here !\n");
  }
  printf("type %s rank %d ", 
	 "int", rank);
  if (rank > 1)
    myerror ("export_int_array: rank>1 not implemented\n");
  if (rank > 0)
    printf("shape %d ", shape);
  else
    shape = 1;
  printf("items %d\n", items);
  printf("%s data follows\n",
	 (fmt.binary) ? (fmt.msb ? "msb ieee" : "lsb ieee") : "ascii");

  if (fmt.binary) {
    if (fmt.msb == mymachine_msb) {
      // Raw dump the data
      fwrite(data, sizeof(int) * shape, items, stdout);
    } else {
      fprintf (stderr, "export_int_array: byte swap not implemented.\n");
      exit(-1);
    }
  } else {
    int i, j;
    int *p = (int*) data;
    for (i = 0; i < items; i++) {
      for (j = 0; j < shape; j++) {
	printf("%d ", *p);
	p++;
      }
      printf("\n");
    }
  }
}

void bloc_read_float(FILE *file, Options &opt, int n, void *data)
{
  int i;
  if (opt.binary) {
    int nread = fread(data, opt.realsize, n, file);
    if (nread != n) {
      fprintf(stderr, "Error reading file\n");
      fprintf(stderr, "Could not read %d elements (%d read).\n",
	      n, nread);
      exit(-1);
    }
    if (opt.msb != mymachine_msb)
      myerror ("Import float bloc : Byte swap not implemented\n");
  } else {
    if (opt.realsize == sizeof(float)) {
      float *fltdata = (float*) data;
      for (i = 0; i < n; i++) {
	int nread = fscanf(file, "%f", fltdata+i);
	if (nread != 1) {
	  fprintf(stderr, "Error reading file\n");
	  exit(-1);
	}
      }
    } else {
      double *dbldata = (double*) data;
      for (i = 0; i < n; i++) {
	int nread = fscanf(file, "%lf", dbldata+i);
	if (nread != 1) {
	  fprintf(stderr, "Error reading file\n");
	  exit(-1);
	}
      }
    }
  }
}

void bloc_read_int(FILE *file, Options &opt, int n, void *data)
{
  int i;
  if (opt.binary) {
    int nread = fread(data, opt.intsize, n, file);
    if (nread != n) {
      fprintf(stderr, "Error reading file\n");
      fprintf(stderr, "Could not read %d elements (%d read).\n",
	      n, nread);
      exit(-1);
    }
    if (opt.msb != mymachine_msb)
      myerror ("Import int bloc : Byte swap not implemented\n");
  } else {
    if (opt.intsize == sizeof(int)) {
      int *intdata = (int*) data;
      for (i = 0; i < n; i++) {
	int nread = fscanf(file, "%d", intdata+i);
	if (nread != 1) {
	  fprintf(stderr, "Error reading file\n");
	  exit(-1);
	}
      }
    } else {
      myerror ("Bloc_read_int : cannot read this int type\n");
    }
  }
}

int import_interfaces(Options &opt, char *component_name, 
		      char *file_name, Domain &dom, 
		      ExportFormat &fmt, int &dxobject,
		      char * &label)
{
  char *fname = (char*) malloc(strlen(file_name)+strlen(opt.path_prefix)+1);
  sprintf(fname, "%s%s", opt.path_prefix, file_name);
  msg1(3, "Opening %s\n", fname);
  FILE *file = fopen(fname, "r");
  if (!file) {
    msg1(0, "Component file %s not found. Skipping.\n", fname);
    free (fname);
    return 0;
  }
  // Read nodes
  int dim, nnodes;
  int nread;
  if (opt.binary && opt.intsize==sizeof(int)) {
    nread = fread(&dim, sizeof(int), 1, file);
    nread += fread(&nnodes, sizeof(int), 1, file);
  } else
    nread = fscanf(file, "%d%d", &dim, &nnodes);
  if (nread != 2)
    myerror("Error reading positions (dimension and number)");
  msg1(2, "Interfaces: %d nodes\n", nnodes);

  void *data = malloctest (opt.realsize * nnodes * dim);
  bloc_read_float(file, opt, dim*nnodes, data);
  export_real_array(fmt, opt.realsize, data, 
		    1, dim, nnodes, ++dxobject);
  free(data);
  int pos_object = dxobject;

  // Read elements
  int type, nelem; // type = 2 in 2D, 3 in 3D
  if (opt.binary && opt.intsize==sizeof(int)) {
    nread = fread(&type, sizeof(int), 1, file);
    nread += fread(&nelem, sizeof(int), 1, file);
  } else
    nread = fscanf(file, "%d%d", &type, &nelem);
  if (nread != 2)
    myerror("Error reading connections (type and number)");
  msg1(2, "Interfaces: %d elements\n", nelem);
  data = malloctest (opt.intsize * nelem * type);
  bloc_read_int(file, opt, type*nelem, data);
  export_int_array(fmt, data, 1, type, nelem, ++dxobject);
  free(data);
  printf ("attribute \"ref\" string \"positions\"\n");
  printf ("attribute \"element type\" string \"%s\"\n",
	  (type==2)?"lines":"triangles");
  
  int connect_object = dxobject;

  // Read field data
  int nfields;
  const int maxfields = 10;
  char fieldname[maxfields][50];
  char s[50];
  int field_som[maxfields]; // 1 : som, 0 : dom
  int field_vect[maxfields]; // 0 : scalar, 1 : vector
  int field_object = dxobject + 1;
  int toomany = 1;
  int n_scan;
  for (nfields=0; ; nfields++) {
    do {
      if (!fgets(s, 50, file)) { // Read string + "\n"
	toomany = 0;
	break; // End of file ?
      }
      msg1(5, "field string read : \"%s\"\n", s);
      int n = strlen(s);
      if (s[n-1] != '\n') {
	fprintf(stderr, "Error in import_interfaces :\n field name too long (%s...)\n", s);
	exit(-1);
      }
      field_som[nfields] = (strncmp(s, "som ", 4) == 0); // SOM or DOM ?
      n_scan = sscanf(s, "%*s %d %s", &(field_vect[nfields]), fieldname[nfields]);
    } while (n_scan != 2);
    if (toomany==0)
      break;
    // Read the trailing null char
    if (opt.binary)
      fgetc(file);
    msg1(4, " found interfaces component : %s\n", fieldname[nfields]);
    msg2(4, " component is of type : %d %s\n", 
	 field_vect[nfields], field_som[nfields] ? "SOM" : "DOM");
    int nread = field_som[nfields] ? nnodes : nelem;
    nread *= field_vect[nfields];
    msg1(5, " reading %d floats...\n", nread);
    void *data = malloctest (opt.realsize * nread);
    bloc_read_float(file, opt, nread, data);
    export_real_array(fmt, opt.realsize, data, 
		      (field_vect[nfields] > 1) ? 1 : 0, 
		      field_vect[nfields], nread, ++dxobject);
    if (field_som[nfields])
      printf ("attribute \"dep\" string \"positions\"\n");
    else
      printf ("attribute \"dep\" string \"connections\"\n");
    free(data);
  }
  if (toomany) 
    myerror("Error : too many interface fields (increase maxfields)\n");
    
  printf ("object %d class field\n", ++dxobject);
  printf (" component \"positions\" value %d\n", pos_object);
  printf (" component \"connections\" value %d\n", connect_object);
  int i;
  for (i = 0; i < nfields; i++)
    printf (" component \"%s\" value %d\n", fieldname[i], field_object + i);

  free(fname);
  strcpy(label, "INTERFACES");

  interfaces_present++;

  return 1;
}

int import_component(Options &opt, char *component_name, 
		     char *file_name, Domain &dom, 
		     ExportFormat &fmt, int &dxobject,
		     char * &label)
{
  int i;
  // Analyse component name
  int compo;
  for (compo = 0; strlen(std_components[compo].name) > 0; compo++) {
    if (strcmp(component_name, std_components[compo].name) == 0) {
      msg1(3, "Known component name found : %s\n", component_name);
      break;
    }
  }
  if (strlen(std_components[compo].name) == 0) {
    if (strcmp(component_name, "INTERFACES") == 0) {
      return import_interfaces(opt, component_name, 
			       file_name, dom, 
			       fmt, dxobject,
			       label);
    }
    fprintf (stderr, "Unknown component name : %s\n", component_name);
    fprintf (stderr, "Update StdComponents in lata_dx_filter\n");
    exit(-1);
  }
  // Check for elem / som
  Elem_som elem_som;
  if (std_components[compo].elem_som == chkfile_name) {
//    if (strstr(file_name, ".I.ELEM.") != 0)
    if (strstr(file_name, ".ELEM.") != 0)
      elem_som = elem;
//    else if (strstr(file_name, ".I.SOM.") != 0)
    else if (strstr(file_name, ".SOM.") != 0)
      elem_som = som;
    else {
      fprintf(stderr, "Error reading component %s\n", component_name);
      fprintf(stderr, "Could not deduce ELEM or SOM from file name :\n");
      fprintf(stderr, "%s\n", file_name);
      exit(-1);
    }
  } else
    elem_som = std_components[compo].elem_som;
  if (opt.nosuffix)
    sprintf(label, "%s", component_name);
  else
    sprintf(label, "%s_%s", component_name, 
  	  (elem_som == elem) ? "ELEM" : "SOM");
  // Check if this component is wanted
  if (opt.ncomponents > 0) {
    for (i = 0; i < opt.ncomponents; i++) {
      if (strstr(opt.components[i], label) != 0)
	break;
    }
    if (i == opt.ncomponents) {
      msg1(3, "Component rejected : %s\n", label);
      return 0;
    }
  }
  // Ok, import this.
  char *fname = (char*) malloc(strlen(file_name)+strlen(opt.path_prefix)+1);
  sprintf(fname, "%s%s", opt.path_prefix, file_name);
  msg1(3, "Opening %s\n", fname);
  FILE *file = fopen(fname, "r");
  if (!file) {
    msg1(0, "Component file %s not found. Skipping.\n", fname);
    free (fname);
    return 0;
  }

  int datasize; // Number of elements to read
  int shape = 1;
  datasize = (elem_som == elem) ? dom.nelem : dom.nsom;
  if (std_components[compo].rank == 1) {
    if (std_components[compo].shape == -1) {
      if (elem_som == elem && (opt.elt_type == triangle || opt.elt_type == quad))
	shape = 2;
      else
	shape = 3;
    } else
      shape = std_components[compo].shape;
  } else
    shape = 1;
  datasize *= shape;
  // Allocate memory
  void* data = malloctest (opt.realsize * datasize);

  // Import data
  int reorder;
  if (std_components[compo].rank == 0)
    reorder = 0;
  else if (elem_som == som && lata_version < 1.4)
    reorder = 0;
  else
    reorder = 1; // Vectors at elements are writen x1 x2 x3... y1 y2 y3... z1 z2 z3...
  if (reorder == 0) {
    skip_blocksize(file, opt.binary, opt.intsize);
    if (opt.binary) {
      int nread = fread(data, opt.realsize, datasize, file);
      if (nread != datasize) {
	fprintf(stderr, "Error reading file %s\n", fname);
	fprintf(stderr, "Could not read %d elements (%d read).\n",
		datasize, nread);
	exit(-1);
      }
      if (opt.msb != mymachine_msb)
	myerror ("Import component : Byte swap not implemented\n");
    } else {
      if (opt.realsize == sizeof(float)) {
	float *fltdata = (float*) data;
	for (i = 0; i < datasize; i++) {
	  int nread = fscanf(file, "%f", fltdata+i);
	  if (nread != 1) {
	    fprintf(stderr, "Error reading ascii file %s (wanted=%d,read=%d)\n", 
		    fname, datasize, i);
	    exit(-1);
	  }
	}
      } else {
	double *dbldata = (double*) data;
	for (i = 0; i < datasize; i++) {
	  int nread = fscanf(file, "%lf", dbldata+i);
	  if (nread != 1) {
	    fprintf(stderr, "Error reading ascii file %s (wanted=%d,read=%d)\n", 
		    fname, datasize, i);
	    exit(-1);
	  }
	}
      }
    }
  } else {
    void *data2 = 0;
    if (opt.binary)
      data2 = malloctest (opt.realsize * datasize / shape);
    skip_blocksize(file, opt.binary, opt.intsize);
    for (i = 0; i < shape; i++) {
      if (opt.binary) {
	// binary file
	int nread = fread(data2, opt.realsize, datasize / shape, file);
	if (nread != datasize / shape) {
	  fprintf(stderr, "Error reading file %s, bloc %d\n", fname, i);
	  fprintf(stderr, "Could not read %d elements (%d read).\n",
		  datasize / shape, nread);
	  exit(-1);
	}
	if (opt.msb != mymachine_msb)
	  myerror ("Import component : Byte swap not implemented\n");
	// Reorder
	int j, k;
	if (opt.realsize == sizeof(float)) {
	  float *f1 = (float*) data;
	  float *f2 = (float*) data2;
	  for (j = 0, k = i; j < datasize / shape; j++, k+=shape)
	    f1[k] = f2[j];
	} else {
	  double *f1 = (double*) data;
	  double *f2 = (double*) data2;
	  for (j = 0, k = i; j < datasize / shape; j++, k+=shape)
	    f1[k] = f2[j];
	}
      } else {
	// Ascii
	int j;
	for (j = 0; j < datasize / shape; j++) {
	  int nread;
	  if (opt.realsize == sizeof(float)) {
	    nread = fscanf(file, "%f", 
			       ((float*)data) + i + shape * j);
	  } else {
	    nread = fscanf(file, "%lf", 
			   ((double*)data) + i + shape * j);
	  }
	  if (nread != 1) {
	    fprintf(stderr, "Error reading file %s\n", fname);
	    exit(-1);
	  }
	}
      }
    }
    skip_blocksize(file, opt.binary, opt.intsize);
    if (opt.binary)
      free(data2);
  }
  fclose(file);

  // Export data
  dxobject++;
  export_real_array(fmt, opt.realsize, data, (shape < 2) ? 0 : 1, shape, datasize/shape,
		    dxobject);
  if (elem_som == elem)
    printf("attribute \"dep\" string \"connections\"\n");
  else
    printf("attribute \"dep\" string \"positions\"\n");

  free(data);

  free(fname);
  return 1;
}


void do_import(Options &opt)
{
  int i;
  int dxobjects = 0;
  int *dxpositions = 0, *dxconnections = 0, *dxfield = 0, *dxgroup = 0;
  int *dxcomponents = 0, *ncomponents;
  char **dxcomp_label;
  int dxtime = 0;

  // Open base lata file
  char *lata_name = (char*) malloc(strlen(opt.path_prefix)+strlen(opt.basename)+10);
  sprintf(lata_name, "%s%s.lata", opt.path_prefix, opt.basename);
  msg1(2, "Opening lata file : %s\n", lata_name);
  FILE *lata_file = fopen(lata_name, "r");
  if (!lata_file) {
    fprintf(stderr, "Error opening lata file : %s\n", lata_name);
    exit(-1);
  }

  // Read header
  char trio_version[200];
  char bname[200];
  char signature[200];
  if (!fgets(trio_version, 200, lata_file))
    myerror("Error reading trio_version");
  msg1(2, "Trio version : %s\n", trio_version);
  TestTrioVersion(trio_version);
  if (!fgets(bname, 200, lata_file))
    myerror("Error reading base name");
  msg1(2, "Base name of the case : %s\n", bname);
  if (!fgets(signature, 200, lata_file))
    myerror("Error reading signature");
  msg1(2, "Signature : %s\n", signature);
  if (!strcmp(signature, "Trio_U"))
    myerror("Bad signature");
  
  Domain *domains = 0;
  int ndomains = 0, ndomainsmax = 0;

  // Read domain list and check against requested domains
  msg(2, "Domains found (*=imported)\n");
  do {
    char s[1000], dom[1000], dfile[1000];
    int nsom, nelem;
    fpos_t pos;
    fgetpos(lata_file, &pos);
    fgets(s, 1000, lata_file);
    int nread = sscanf(s, " Geometrie%s%s", dom, dfile);
    if (nread != 2) {
      // End of domain list
      fsetpos(lata_file, &pos); // Go back to reread this line
      break;
    }
    fgets(s, 1000, lata_file);
    fgets(s, 1000, lata_file);
    if (sscanf(s, "nb_som_tot=%d", &nsom) != 1) {
      fprintf(stderr, "Error reading nb_som_tot for domain %s\n", dom);
      exit(-1);
    }
    fgets(s, 1000, lata_file);
    if (sscanf(s, "nb_elem_tot=%d", &nelem) != 1) {
      fprintf(stderr, "Error reading nb_elem_tot for domain %s\n", dom);
      exit(-1);
    }
    // Do we want this domain ?
    int ok = (opt.ndomains == 0);
    for (i=0; i<opt.ndomains; i++)
      if (strcmp(opt.domains[i], dom) == 0)
	ok = 1;
    if (verbosity >= 2) 
      fprintf(stderr, "%s %s %s nb_som_tot=%d nb_elem_tot=%d\n", 
	      ok?"*":" ", dom, dfile, nsom, nelem);
    if (!ok)
      break;
    // Add this domain to the import list
    if (ndomainsmax <= ndomains) {
      ndomainsmax += 10;
      domains = (Domain*) realloc(domains,sizeof(Domain)*ndomainsmax);
    }
    if (strlen(dom) > MAX_NAME)
      myerror("Increase MAX_NAME and recompile filter !\n");
    domains[ndomains].name = (char*) malloc(strlen(dom)+1);
    strcpy(domains[ndomains].name, dom);
    domains[ndomains].file = (char*) malloc(strlen(dfile)+1);
    strcpy(domains[ndomains].file, dfile);
    domains[ndomains].nsom = nsom;
    domains[ndomains].nelem = nelem;
    // Default values
    if (opt.elt_type == unspecified) {
      fprintf(stderr,
	      "Lata version %f : you must specify element type on command line.\n",
	    lata_version);
      exit(-1);
    }
    domains[ndomains].elt_type = opt.elt_type;
    domains[ndomains].binary = opt.binary;
    ndomains++;
  } while (1);
  if (opt.ndomains > 0 && opt.ndomains != ndomains) {
    fprintf(stderr, 
	    "Some requested domains were not found in the lata file.\n");
    fprintf(stderr,
	    "Import aborted.\n");
    exit(-1);
  }

  // Setting up export format
  ExportFormat exfmt(opt);
  if (verbosity >= 1) {
    fprintf(stderr, "Exporting format :\n");
    fprintf(stderr, "  %s\n", exfmt.binary ? "Binary" : "Ascii");
    if (exfmt.binary) {
      if (exfmt.msb) fprintf(stderr, "  msb (little endian)\n");
      else fprintf(stderr, "  lsb (big endian)\n");
      fprintf(stderr, "  ints are signed %d-byte\n", exfmt.intsize);
      fprintf(stderr, "  real are %d-byte\n", exfmt.realsize);
    }
  }

  // Exporting domain list
  {  
    // Compute length of longest string
    unsigned int maxlen = 0;
    for (i=0; i<ndomains; i++)
      if (maxlen < strlen(domains[i].name))
	maxlen = strlen(domains[i].name);
    maxlen++;
    printf("object %d class array\n", ++dxobjects);
    printf(" type string rank 1 shape %d items %d", 
	   maxlen, ndomains);
    printf(" data follows\n");
    for (i=0; i<ndomains; i++)
      printf("\"%s\"\n", domains[i].name);
  }

  dxpositions = (int*)malloc(sizeof(int)*ndomains);
  dxconnections = (int*)malloc(sizeof(int)*ndomains);
  dxgroup = (int*)malloc(sizeof(int)*ndomains);
  dxfield = (int*)malloc(sizeof(int)*ndomains);
  dxcomponents = (int*)malloc(sizeof(int)*ndomains*MAX_COMPONENTS);
  dxcomp_label = (char**) malloc(sizeof(char*)*ndomains*MAX_COMPONENTS);
  ncomponents = (int*)malloc(sizeof(int)*ndomains);
  for (i = 0; i < ndomains; i++)
    ncomponents[i] = 0;
  for (i = 0; i < ndomains*MAX_COMPONENTS; i++)
    dxcomp_label[i] = (char*)malloc(100);

  // Exporting geometry
  for (i=0; i<ndomains; i++) {
    if (opt.geometry) {
      // Open geometry file
      FILE *geom;
      {
	char *fname = (char*) 
	  malloc(strlen(domains[i].file)+strlen(opt.path_prefix)+1);
	sprintf(fname, "%s%s", opt.path_prefix, domains[i].file);
	msg1(1, "Opening file : %s\n", fname);
	geom = fopen(fname, "r");
	free(fname);
      }
      if (!geom) {
	fprintf(stderr,"Couldn't open geometry file %s\n", domains[i].file);
	exit(-1);
      }
      // Import positions data
      int rsize;
      void* data;
      lata_read_positions(opt, geom, domains[i], rsize, data);
      // Export positions data
      dxpositions[i] = ++dxobjects;
      export_real_array(exfmt, rsize, data, 1, 3, domains[i].nsom, dxobjects);
      free(data);
      // Extract connections
      int shape;
      dxconnections[i] = ++dxobjects;
      lata_read_connections(opt, geom, domains[i], data, shape);
      export_int_array(exfmt, data, 1, shape, domains[i].nelem, dxobjects);
      free(data);
      fclose(geom);
      char *eltstring;
      switch (domains[i].elt_type) {
      case triangle: eltstring = "triangles"; break;
      case quad: eltstring = "quads"; break;
      case tetra: eltstring = "tetrahedra"; break;
      case hexa: eltstring = "cubes"; break;
      default: myerror("Bad element type !\n");
      }
      printf("attribute \"element type\" string \"%s\"\n", eltstring);
      printf("attribute \"ref\" string \"positions\"\n");
    }

    // Build domain attribute information
    dxgroup[i] = ++dxobjects;
    printf("object %d constantarray type int rank 0 items %d\n",
	   dxgroup[i], domains[i].nelem);
    printf("ascii data follows\n %d\n", i);
    printf("attribute \"dep\" string \"connections\"\n");
  }

  double time = 0.;

  // If exporting components, going through timesteps
  if (opt.inclcomponents && opt.ntimesteps > 0) {
    int tstep = -1;
    do {
      // For the moment, only grab one timestep
      // First line should read TEMPS %f
      char s[1000];
      if (fgets(s, 1000, lata_file) ==  0) {
	msg(2, "End of lata_file\n");
	break;
      }
      double newtime;
      if (sscanf(s, "TEMPS%lf", &newtime) == 1) {
	msg1(3, "Timestep found : %20.15e\n", time);	
	tstep++;
	// Stop here if last timestep was the one we want
	if (tstep - 1 == opt.timesteps[0])
	  break;
      } else if (strncmp(s, "FIN", 3) == 0) {
	break;
      } else {
	time = newtime;
	// Scan components
	char component_name[1000];
	char file_name[1000];
	char champ[10];
	int nread = sscanf(s, "%s%s%s", champ, component_name, file_name);
	if (nread != 3) {
	  fprintf(stderr, 
		  "Error reading lata file (expecting Champ ...or Stat ...). Line reads :\n");
	  fprintf(stderr, s);
	  exit(-1);
	}
	if (tstep != opt.timesteps[0])
	  continue; // This timestep is not wanted
	// To which domain does this component belong ?
	int d;
	for (d = 0; d < ndomains; d++) {
	  char s[MAX_NAME+3];
	  sprintf(s, ".%s.", domains[d].name);
	  msg1(3, "Checking against %s\n", s);
	  if (strstr(file_name, s) != 0)
	    break;
	}
	if (d >= ndomains) {
	  msg1(2, "Domain not wanted : %s\n", file_name);
	  break;
	}
	if (import_component(opt, component_name, file_name, 
			     domains[d], exfmt, dxobjects,
			     dxcomp_label[d*MAX_COMPONENTS+ncomponents[d]])
	    ) {
	  if (ncomponents[d] == MAX_COMPONENTS)
	    myerror("Error : increase MAX_COMPONENTS and recompile filter!\n");
	  dxcomponents[d * MAX_COMPONENTS + ncomponents[d]] = dxobjects;
	  ncomponents[d]++;
	}
      }
    } while (1);
    if (tstep == -1)
      myerror("Timestep not found\n");
  }
  dxtime = ++dxobjects;
  printf("object %d class array type float items 1\n", dxtime);
  printf(" data follows %20.12g\n", time);
   
  // Export geometry
  for (i = 0; i < ndomains; i++) {
    if (!opt.geometry) {
      dxobjects++;
      dxpositions[i]=dxobjects;
      printf("object %d class gridpositions counts %d\n", 
	     dxobjects, domains[i].nsom);
      printf("origin 0 delta 1\n");
      dxobjects++;
      dxconnections[i]=dxobjects;
      printf("object %d class gridconnections counts %d\n", 
	     dxobjects, domains[i].nelem+1);
      printf("attribute \"element type\" string \"lines\"\n");
      printf("attribute \"ref\" string \"positions\"\n");
    }
    dxfield[i] = ++dxobjects;
    printf("object %d class field\n", dxobjects);
    printf(" component \"positions\" %d\n", dxpositions[i]);
    printf(" component \"connections\" %d\n", dxconnections[i]);
    printf(" component \"domain\" %d\n", dxgroup[i]);
    printf(" component \"TIME\" %d\n", dxtime);
    if (opt.inclcomponents) {
      int j;
      for (j = 0; j < ncomponents[i]; j++) 
	if (strcmp(dxcomp_label[i*MAX_COMPONENTS+j], "INTERFACES") != 0) 
	  printf(" component \"%s\" %d\n", 
		 dxcomp_label[i*MAX_COMPONENTS+j],
		 dxcomponents[i*MAX_COMPONENTS+j]);
    }
  }
  if (opt.forcegroup || ndomains > 1 || interfaces_present) { 
    // Group all domains in a "group object"
    dxobjects++;
    printf("object %d class group\n", dxobjects);
    for (i = 0; i < ndomains; i++) {
      printf(" member \"%s\" value %d\n", domains[i].name, dxfield[i]);
      int j;
      for (j = 0; j < ncomponents[i]; j++) {
	 if (strcmp(dxcomp_label[i*MAX_COMPONENTS+j], "INTERFACES") == 0)
	    printf(" member \"INTERFACES\" value %d\n", 
		   dxcomponents[i*MAX_COMPONENTS+j]);
      }
    }
  }
  printf("END\n");

  fclose(lata_file);
}

int main(int argc, char **argv)
{
  Options opt(argc, argv);

  do_import(opt);

  return 0;
}
