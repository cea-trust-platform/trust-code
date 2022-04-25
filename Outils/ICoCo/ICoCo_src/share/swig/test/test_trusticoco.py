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

    def test_exceptions(self):
        import trusticoco as ti

        pbT = ti.ProblemTrio()
        self.assertRaises(ti.ICoCoNotImplemented, pbT.getFieldUnit, "XXX")
#         pbT.setDataFile("test_conduc.data")
#         pbT.initialize()
#         dt, stop = pbT.computeTimeStep()
#         pbT.initTimeStep(dt)
#         self.assertRaises(ti.ICoCoWrongArgument, pbT.getFieldType, "XXXX")
#         self.assertRaises(ti.ICoCoWrongContext, pbT.terminate)
#         pbT.solveTimeStep()
#         pbT.validateTimeStep()
#         pbT.terminate()

    def test_small_run(self):
        """ Execute a small run """
        import trusticoco as ti
        import medcoupling as mc

        pbT = ti.ProblemTrio()
        pbT.name = "TRUST"
        pbT.setDataFile("test_conduc.data")
        pbT.initialize()

        equation_non_resolue = pbT.getOutputIntValue("non_resolue");
        pbT.setInputIntValue("non_resolue",equation_non_resolue);

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
                expected_outs = ('FLUX_DROIT', 'TMAX', 'TEMPERATURE_SOM_dom', 'TEMPERATURE_ELEM_dom', 'PVOL_ELEM_dom')
                expected_ins = ('Tdroit', 'pvol')
                self.assertEqual(pb.getOutputFieldsNames(), expected_outs)
                self.assertEqual(pb.getInputFieldsNames(), expected_ins)
                self.assertEqual(ti.ICoCoValueType.Double, pb.getFieldType("PVOL_ELEM_dom"))
                self.assertRaises(ti.ICoCoWrongArgument, pb.getFieldType, "XXXX")

                #
                # Play with fields typed as MEDDoubleField:
                #
                fl_asmf = mc.ICoCoMEDDoubleField()
                pb.getOutputMEDDoubleField("PVOL_ELEM_dom", fl_asmf)
                print("@@@ NAME:", fl_asmf.getName())
                toto_asmf = mc.ICoCoMEDDoubleField()
                pb.getInputMEDDoubleFieldTemplate("pvol", toto_asmf)
                totomc1 = toto_asmf.getMCField()
                print(totomc1, toto_asmf.getMCField())
                totomc1 += 1    # From MEDCoupling
                pb.setInputMEDDoubleField("pvol",toto_asmf)

                #
                # Play with fields typed directly as MEDCouplingFieldDouble (nicer in Python ...)
                #
                fl_asmc = pb.getOutputMEDField("PVOL_ELEM_dom")  # WARNING : no "MEDFieldDouble" here!!
                print(fl_asmc.getName())
                totomc2 = pb.getInputMEDFieldTemplate("pvol")    # WARNING : no "MEDFieldDouble" here!!
                print(totomc2)
                totomc2 += 1
                pb.setInputMEDField("pvol",totomc2)              # WARNING : no "MEDFieldDouble" here!!

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
