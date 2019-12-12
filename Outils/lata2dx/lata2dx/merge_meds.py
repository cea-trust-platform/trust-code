from __future__ import print_function
import medcoupling as mc
import MEDLoader as ml
import time
from optparse import OptionParser
import sys
import os


def write_mesh():
    mn = ml.GetMeshNames(FilesList[0])
    wfs = True
    for n in mn:
        print ("Mesh : %s, proc :" %format(n), end=" ")
        meshes = []
        for i, f in enumerate(FilesList):
            print (i, end=" "),
            mesh = ml.ReadUMeshFromFile(f, n, 0)
            meshes.append(mesh)
        mesh_m = ml.MEDCouplingUMesh.MergeUMeshes(meshes)
        mesh_m.setName(meshes[0].getName())
        ml.WriteUMesh(OutputFile, mesh_m, wfs); wfs = False
        print ("done")

def write_fields():
    fields = {}
    for n in fn:
        print ("Field : %s" %(n))
        for t in range(n_dt):
            print ("    Timestep %s/%s, proc :" %(t + 1, n_dt), end=" "),
            f_dt = []
            for i, f in enumerate(FilesList):
                print (i, end=" "),
                f_m = ml.MEDFileFieldMultiTS.New(f, n)
                f_dt.append(f_m[t].getFieldAtLevel(mc.ON_CELLS,0))
            print ("done")
            f_dt_merge = mc.MEDCouplingFieldDouble.MergeFields(f_dt)
            f_dt_merge.setTime(f_dt[0].getTime()[0], f_dt[0].getTime()[1], 0)
            f_dt_merge.setName(f_dt[0].getName())
            fnp = ml.MEDFileField1TS.New()
            fnp.setFieldNoProfileSBT(f_dt_merge)
            fnp.write(OutputFile, 0)

if __name__=="__main__":
    parser = OptionParser()
    parser.set_usage("Merge into a single .med file MED files generated from TRUST parallel computation.\n  python merge_meds.py MEDFilesToMerge.med NB_PROCS [MergedFileName.med]")
    (opts, args) = parser.parse_args()
    parser.print_usage()

    if len(args) < 2:
        print ("Fatal error: You should specify MEDFilesToMerge.med and NB_PROCS !!!")
        exit(1)

    # get files names to merge
    InputFile = sys.argv[1]
    if InputFile.endswith('.med'):
        InputFile = os.path.splitext(os.path.basename(InputFile))[0]
    InputFile = InputFile[:-4] 

    nb_procs = int(sys.argv[2])

    OutputFile = 'merged.med'
    if len(args) == 3:
        OutputFile = sys.argv[3]
        if not OutputFile.endswith('.med'):
            OutputFile += '.med'

    FilesList = ['{}{:04d}.med'.format(InputFile, i) for i in range(nb_procs)]

    for i in range(nb_procs):
        if not (os.path.isfile(FilesList[i])):
           print(("\nFatal error: %s file does not exist!" %(FilesList[i])))
           exit(1)

    mff = ml.MEDFileFields(FilesList[0])
    fn = mff.getFieldsNames()
    n_dt = len(ml.MEDFileFieldMultiTS.New(FilesList[0], fn[0]))

    write_mesh()
    write_fields()

    print ("\nParallel Med file have been merged into %s" %(OutputFile))
