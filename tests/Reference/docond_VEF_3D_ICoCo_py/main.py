"""
Authors : Elie Saikali & Adrien Bruneton

Avril 2021
  _____  _____         _____
 |_   _|/ ____|       / ____|
   | | | |      ___  | |      ___
   | | | |     / _ \ | |     / _ \
  _| |_| |____| (_) || |____| (_) |
 |_____|\_____|\___/  \_____|\___/

Supervisor solid/fluid coupling with python

THIS IS A // VERSION => mpi4py PACKAGE IS REQUIRED

"""

import trusticoco as ti
import medcoupling as mc
import ParaMEDMEM as pm
import time
from mpi4py import MPI

def synchronize_dt(val):
    """Synchronize dt and return min"""
    return MPI.COMM_WORLD.allreduce(val, op=MPI.MIN)

def synchronize_bool_or(val):
    """Synchronize bool (or) and return max"""
    return MPI.COMM_WORLD.allreduce(val, op=MPI.MAX)

def synchronize_bool_and(val):
    """Synchronize bool (and) and return min"""
    return MPI.COMM_WORLD.allreduce(val, op=MPI.MIN)

def init_MPI(val):
    """Initialize the MPI variables"""

    print ("Using ICoCo version",ti.ICOCO_VERSION)
    # init MPI and groups
    size = MPI.COMM_WORLD.size
    rank = MPI.COMM_WORLD.rank

    if size !=val:
        print("Error : you must run on %d procs and not %d !" %(size,val))
        return

    return size,rank

def initGroupsProblems(file1, file2, procs1, procs2):
    """
    Init the groups for both codes, create new problem instances and initialize the simulations.
    """

    interface = mc.CommInterface()
    group1 = mc.MPIProcessorGroup(interface,procs1 )
    group2 = mc.MPIProcessorGroup(interface,procs2 )

    if group1.containsMyRank():
        data_file = file1
        mpicomm = group1.getComm()
        # pbT = ti.ProblemTrio()
        # pbT.name = "TRUST"
    elif group2.containsMyRank():
        data_file = file2
        mpicomm = group2.getComm()
        # pbT = ti.ProblemTrio()
        # pbT.name = "TRUST"
    else:
        raise

    pbT = ti.getProblem()
    pbT.name = "TRUST"
    pbT.setDataFile(data_file)
    pbT.setMPIComm(mpicomm)
    pbT.initialize()
    printOutFields(pbT)

    return group1,group2,pbT


def printOutFields(pbT):
    """
    Prints the output field names in both simulations.
    """
    print("OutputFieldsNames : ",pbT.getOutputFieldsNames())
    print("=================================================================")
    print(" ")
    print("Problem initialized ! Simulation and code coupling starts now ...")
    print(" ")
    print("=================================================================")

    return

def getInOutFields(pbT,g1,g2,
                   dom1F1N,dom1F1,dom1F2N,dom1F2,
                   dom2F1N,dom2F1,dom2F2N,dom2F2):
    """
    Get in fields and out templates for both codes.
    Attention to syntax : field name followed by the ICoCoMEDDoubleField !
    """
    # exchange data
    if g1.containsMyRank():
        pbT.getInputMEDDoubleFieldTemplate(dom1F1N,dom1F1)
        pbT.getOutputMEDDoubleField(dom1F2N,dom1F2)
    elif g2.containsMyRank():
        pbT.getOutputMEDDoubleField(dom2F1N,dom2F1)
        pbT.getInputMEDDoubleFieldTemplate(dom2F2N,dom2F2)
    else:
        raise

    return

def prepareDEC(dec1,field1,type1,dec2,field2,type2,init):
    """
    Prepare the interpolator methods (P0 or P1), attach fields, set nature and
    synchronize (i.e. find intersection which is done only once).
    """

    if init:
        dec1.setMethod("P0")
        dec2.setMethod("P0")

    dec1.attachLocalField(field1)
    dec2.attachLocalField(field2)

    if type1 == "maximum":
        dec1.setNature(pm.IntensiveMaximum)
    elif type1 == "conservation":
        dec1.setNature(pm.IntensiveConservation)
    else:
        raise

    if type2 == "maximum":
        dec2.setNature(pm.IntensiveMaximum)
    elif type2 == "conservation":
        dec2.setNature(pm.IntensiveConservation)
    else:
        raise

    if init:
        dec1.synchronize()
        dec2.synchronize()
        init = False

    return init

def interpolateFields(pbT,g1,dec1,dec2,F1N,F1,F2N,F2):
    """
    Interpolate the fields (send/receive)
    """

    if g1.containsMyRank():
        dec1.sendData()
        dec2.recvData()
        pbT.setInputMEDDoubleField(F1N,F1)
    else:
        dec1.recvData()
        dec2.sendData()
        pbT.setInputMEDDoubleField(F2N,F2)

    return

def validateResolution(ok,pbT):
    """
    Validate or abort time step. Checks also if the problem is stationsary.
    """
    if (not ok): # The resolution failed, try with a new time interval.
        pbT.abortTimeStep()
    else: # The resolution was successful, validate and go to the next time step.
        pbT.validateTimeStep()

    return

def finishAllCorrectly(pbT,dec1,dec2,g1,g2):
    """
    Finish the coupled simulation correctly: terminate both problems, release
    DECs, MPI groups and finalize MPI.
    """
    pbT.terminate()
    dec1.release()
    dec2.release()
    g1.release()
    g2.release()
    MPI.COMM_WORLD.Barrier()
    MPI.Finalize()

# THIS IS THE MAIN
if __name__ == "__main__":

    size,rank = init_MPI(4) # 4 because 4 procs

    file1 = "docond_VEF_3D_dom1"
    file2 = "docond_VEF_3D_dom2"
    dom1_procs = [0,1]
    dom2_procs = [2,3]

    dom1_group,dom2_group,pbT = initGroupsProblems(file1,file2,dom1_procs,dom2_procs)

    # some logical values to control the simulation
    stop = False # Does the Problem want to stop ?
    ok = init = True # Is the time interval successfully solved ? first time step ?
    stop = synchronize_bool_or(stop)

    clock0 = time.time()
    compti = 0

    # Create fields and interpolators
    field_flux, field_temperature = ( mc.ICoCoMEDDoubleField() for i in range(2))
    dec_flux, dec_temperature = (mc.InterpKernelDEC(dom1_group, dom2_group) for i in range(2))

    dom1_in = "TEMPERATURE_IN_DOM1"
    dom1_out = "FLUX_SURFACIQUE_OUT_DOM1"
    dom2_in = "FLUX_SURFACIQUE_IN_DOM2"
    dom2_out = "TEMPERATURE_OUT_DOM2"

    # loop on time steps
    while not stop:
        compti +=  1
        clocki =  time.time()
        print (compti, " CLOCK ", clocki - clock0)
        ok = False # Is the time interval successfully solved ?

        # Loop on the time interval tries
        while not ok and not stop:

            # manage timestep
            dt,stop = pbT.computeTimeStep()
            dt = synchronize_dt(dt)
            stop = synchronize_bool_or(stop)
            print ("The actual time is (s) : ",  pbT.presentTime())
            print ("The chosen dt is (s) : ", dt)
            if stop:
                break

            pbT.initTimeStep(dt)

            # prepare & exchange data
            getInOutFields(pbT,dom1_group,dom2_group,
                           dom1_in,field_temperature,dom1_out,field_flux,
                           dom2_out,field_temperature,dom2_in,field_flux)

            init = prepareDEC(dec_flux,field_flux,"maximum",
                              dec_temperature,field_temperature,"maximum",init)

            interpolateFields(pbT,dom1_group,dec_flux,dec_temperature,
                              dom1_in,field_temperature,
                              dom2_in ,field_flux)

            clock_before_resolution = time.time()

            # solve time step
            ok = pbT.solveTimeStep()
            ok = synchronize_bool_and(ok)

            clock_after_resolution = time.time()
            print (compti, " TEMPS DE RESOLUTION DU PB (s) : ", clock_after_resolution - clock_before_resolution)

            # iterate or stop ?
            validateResolution(ok,pbT)

        stat = pbT.isStationary()
        stat = synchronize_bool_and(stat)

        if (stat):
            stop = True


    # finalize correctly
    finishAllCorrectly(pbT,dec_flux,dec_temperature,dom1_group,dom2_group)
