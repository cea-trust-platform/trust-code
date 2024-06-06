import unittest
from mpi4py import MPI

class trusticoco_test(unittest.TestCase):

    def test_set_comm(self):
        """ Set a MPI communicator created from Python """
        import trusticoco as ti

        pbT = ti.ProblemTrio()
        pbT.name = "TRUST"
        com2 = MPI.Comm(MPI.COMM_WORLD)
        pbT.setMPIComm(com2)

if __name__ == "__main__":
    unittest.main()
