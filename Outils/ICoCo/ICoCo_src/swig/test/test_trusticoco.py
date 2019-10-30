import unittest

class trusticoco_test(unittest.TestCase):

    def test_simple(self):
        """ Test import of modules and one function in each """
        import trusticoco as ti
        pbT = ti.ProblemTrio()
        pbT.setDataFile("test_conduc.data")

        f = ti.TrioField()
        f.setName("toto")
        self.assertEqual(f.getName(), "toto")

    def test_small_run(self):
        return
        """ Execute a small run """
        import trusticoco as ti

        pbT = ti.ProblemTrio()
        pbT.name = "TRUST"
        pbT.setDataFile("test_conduc.data")
        pbT.initialize()

        def run(pb, tmax):
            # pb.initialize()
            stop = False
            t= 0
            while (t < tmax) and (not stop):
                dt, stop = pb.computeTimeStep()
                if stop:
                    return
                pb.initTimeStep(dt)
                # List of available fields:
                print("out",pb.getOutputFieldsNames())
                print("in",pb.getInputFieldsNames())

                #
                # Play with fields typed as TrioField:
                #
                fl_astf = pb.getOutputField("PVOL_ELEM_dom")
                print(fl_astf.getName())
                fl2_astf = ti.TrioField()
                pb.getInputFieldTemplate("pvol", fl2_astf)
                pb.setInputField("pvol",fl2_astf)

                # Main time loop:
                ok = pb.solveTimeStep()
                if ok:
                    pb.validateTimeStep()
                    t = pb.presentTime()
                    if pb.isStationary():
                        print("pb stationary -> stop")
                        stop = True
                else:
                    pb.abortTimeStep()
                    dt2, stop2 = pb.computeTimeStep()
                    print("pb abortTimeStep", dt, dt2)
                    assert dt != dt2

        run(pbT,8.e-5)
        pbT.terminate()

if __name__ == "__main__":
    unittest.main()
