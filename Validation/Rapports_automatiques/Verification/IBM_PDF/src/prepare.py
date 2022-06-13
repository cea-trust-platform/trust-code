#
# Prepare all the datasets for the form IBM_PDF:
#   test cases are created by subsutiuing parameters in template datasets
#   cases are added via the run.addCase() API
#   and finally the partitionning is triggered
#

import os
from trustutils import run

def misc_param(jdd, meth):
    """ Sets the various dataset parameters according to the chosen interpolation method """
    if meth == "dir":
        jdd = jdd.replace("@EST_DIRICHLET@", "#")
        jdd = jdd.replace("@POINTS_FLUIDES@", "")
        jdd = jdd.replace("@POINTS_SOLIDES@", "")
        jdd = jdd.replace("@ELEMENTS_FLUIDES@", "")
        jdd = jdd.replace("@ELEMENTS_SOLIDES@", "#")
    if meth == "hybrid":
        jdd = jdd.replace("@EST_DIRICHLET@", "")
        jdd = jdd.replace("@POINTS_FLUIDES@", "")
        jdd = jdd.replace("@POINTS_SOLIDES@", "")
        jdd = jdd.replace("@ELEMENTS_FLUIDES@", "")
        jdd = jdd.replace("@ELEMENTS_SOLIDES@", "")
    if meth == "multi_dir":
        jdd = jdd.replace("@EST_DIRICHLET@", "")
        jdd = jdd.replace("@POINTS_FLUIDES@", "#")
        jdd = jdd.replace("@POINTS_SOLIDES@", "")
        jdd = jdd.replace("@ELEMENTS_FLUIDES@", "#")
        jdd = jdd.replace("@ELEMENTS_SOLIDES@", "")
    return jdd

def writeAddAndPartition(baseName, jdd):
    """ Write the dataset for a case, register it, and trigger its partitionning
    """    
    with open(baseName+"/"+baseName+".data", "w") as f:
        f.write(jdd)
    # In the below, we must use ';' to ignore return value of trust -partition (it returns the numb of procs ...)
    run.executeCommand(f"cd {baseName} && trust -partition {baseName}.data; cd ..") 
    os.chdir(run.BUILD_DIRECTORY)
    run.addCase(baseName, "PAR_"+baseName+".data",4)

##
## TAYLOR COUETTE
##
data = {"dir": ["IBM_element_fluide"],
        "multi_dir": ["IBM_gradient_moyen"],
        "hybrid": ["IBM_hybride"]
        }
with open("template_taylor_couette.data", "r") as f:
    templ = f.read()
for meth in ["dir", "hybrid", "multi_dir"]:
    baseName = "taylor_couette_pdf_"+meth
    os.makedirs(baseName, exist_ok=True)
    d = data[(meth)]
    jdd = templ  # Deep copy (str is immutable)
    jdd = jdd.replace("@INTERP_TYPE@", d[0])
    jdd = misc_param(jdd, meth)
    writeAddAndPartition(baseName, jdd)

##
## POISEUILLE ALIGN
##
baseName = "poiseuille_pdf_align"
run.executeCommand(f"cd {baseName} && trust -partition {baseName}.data; cd ..") 
os.chdir(run.BUILD_DIRECTORY)
run.addCase(baseName, "PAR_"+baseName+".data",4)

##
## POISEUILLE TILT
##
data = {"dir": ["IBM_element_fluide"],
        "multi_dir": ["IBM_gradient_moyen"],
        "hybrid": ["IBM_hybride"]
        }
with open("template_poiseuille_tilt.data", "r") as f:
    templ = f.read()
for meth in ["dir", "hybrid", "multi_dir"]:
    for tilt in ["11", "30", "45"]:
        baseName = "poiseuille_pdf_tilt_"+tilt+"_"+meth
        os.makedirs(baseName, exist_ok=True)
        d = data[(meth)]
        jdd = templ  # Deep copy (str is immutable)
        jdd = jdd.replace("@TILT@", tilt)
        jdd = jdd.replace("@INTERP_TYPE@", d[0])
        jdd = misc_param(jdd, meth)
        writeAddAndPartition(baseName, jdd)

##
## CYLINDER STATIC
##
data = {"dir": ["IBM_element_fluide"],
        "multi_dir": ["IBM_gradient_moyen"],
        "hybrid": ["IBM_hybride"]
        }
data_mu = {"20": ["0.05"],
        "100": ["0.01"]
        }
with open("template_cylinder_static.data", "r") as f:
    templ = f.read()
for meth in ["dir", "hybrid", "multi_dir"]:
    for rey in ["20", "100"]:
        baseName = "cylinder_pdf_static_re_"+rey+"_"+meth
        os.makedirs(baseName, exist_ok=True)
        d = data[(meth)]
        d_mu = data_mu[(rey)]
        jdd = templ  # Deep copy (str is immutable)
        jdd = jdd.replace("@REYNOLDS@", rey)
        jdd = jdd.replace("@INTERP_TYPE@", d[0])
        jdd = jdd.replace("@MU_VAL@", d_mu[0])
        jdd = misc_param(jdd, meth)
        writeAddAndPartition(baseName, jdd)

##
## CYLINDER ROTATE
##
data = {"dir": ["IBM_element_fluide"],
        "multi_dir": ["IBM_gradient_moyen"],
        "hybrid": ["IBM_hybride"]
        }
data_mu = {"20": ["0.05"],
        "100": ["0.01"]
        }
with open("template_cylinder_rotate.data", "r") as f:
    templ = f.read()
for meth in ["dir", "hybrid", "multi_dir"]:
    for rey in ["20", "100"]:
        baseName = "cylinder_pdf_rotate_re_"+rey+"_"+meth
        os.makedirs(baseName, exist_ok=True)
        d = data[(meth)]
        d_mu = data_mu[(rey)]
        jdd = templ  # Deep copy (str is immutable)
        jdd = jdd.replace("@REYNOLDS@", rey)
        jdd = jdd.replace("@INTERP_TYPE@", d[0])
        jdd = misc_param(jdd, meth)
        jdd = jdd.replace("@MU_VAL@", d_mu[0])
        writeAddAndPartition(baseName, jdd)

