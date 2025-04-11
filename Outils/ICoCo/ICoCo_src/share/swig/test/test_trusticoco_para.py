import unittest
from mpi4py import MPI

class trusticoco_test(unittest.TestCase):

    def test_set_comm(self):
        return
        """ Set a MPI communicator created from Python """
        import trusticoco as ti

        pbT = ti.ProblemTrio()
        pbT.name = "TRUST"
        com2 = MPI.Comm(MPI.COMM_WORLD)
        pbT.setMPIComm(com2)

    def test_repeated_run_parallel(self):    
        """ Same as test_repeated_run() (in  test_trusticoco.py) but in //
        """
        import trusticoco as ti
        import medcoupling as mc
       
        def run():
            pbT = ti.ProblemTrio()
            pbT.name = "TRUST"
            com2 = MPI.Comm(MPI.COMM_WORLD)
            pbT.setMPIComm(com2)
            pbT.setDataFile("PAR_test_conduc_para.data")
            pb = pbT
            pb.initialize()
            dt, stop = pb.computeTimeStep()
            pb.initTimeStep(dt)                            
            ok = pb.solveTimeStep()
            pb.validateTimeStep()
            pbT.terminate()

        run()
        run()    

if __name__ == "__main__":
    unittest.main()
