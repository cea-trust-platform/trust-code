import unittest

class trusticoco_test(unittest.TestCase):

    def test_simple(self):
        """ Test import of modules and one function in each """

        import medcoupling as mc
        import trusticoco as ti
        print (ti.ICOCO_VERSION)
        pbT = ti.ProblemTrio()
        pbT.setDataFile("test_conduc.data")

        f = mc.ICoCoMEDDoubleField()
        f.setName("toto")
        self.assertEqual(f.getName(), "toto")

    def test_enum(self):
        import trusticoco as ti
        ti.ICoCoValueType.Double
        ti.ICoCoValueType.Int
        ti.ICoCoValueType.String

    def test_small_run(self):
        """ Execute a small run """
        import trusticoco as ti
        import medcoupling as mc

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
                self.assertEqual(ti.ICoCoValueType.Double, pb.getFieldType("PVOL_ELEM_dom"))
                self.assertRaises(RuntimeError, pbT.getFieldType, "XXXX")

                #
                # Play with fields typed as MEDDoubleField:
                #
                fl_asmf = mc.ICoCoMEDDoubleField()
                pb.getOutputMEDDoubleField("PVOL_ELEM_dom", fl_asmf)
                print(fl_asmf.getName())
                toto_asmf = mc.ICoCoMEDDoubleField()
                pb.getInputMEDDoubleFieldTemplate("pvol", toto_asmf)
                totomc1 = toto_asmf.getMCField()
                print(totomc1, toto_asmf.getMCField())
                totomc1 += 1    # From MEDCoupling
                pb.setInputMEDDoubleField("pvol",toto_asmf)

                #
                # Play with fields typed directly as MEDCouplingFieldDouble (nicer in Python ...)
                #
                fl_asmc = pb.getOutputMEDField("PVOL_ELEM_dom")  # WARNING : no "MEDDouble" here!!
                print(fl_asmc.getName())
                totomc2 = pb.getInputMEDFieldTemplate("pvol")    # WARNING : no "MEDDouble" here!!
                print(totomc2)
                totomc2 += 1
                pb.setInputMEDField("pvol",totomc2)              # WARNING : no "MEDDouble" here!!

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
