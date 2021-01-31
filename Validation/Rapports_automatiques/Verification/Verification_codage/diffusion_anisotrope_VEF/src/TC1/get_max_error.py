import medcoupling as mc

def getFieldErrValues (fileName,fieldName,meshName):
    """
    Read the error field and returns the max and average values.
    """
    lst_dt = mc.GetAllFieldIterations(fileName,fieldName)
    ff_err = mc.ReadFieldCell(fileName,meshName,0,fieldName,lst_dt[-1][0],lst_dt[-1][1])
    myMax=ff_err.getArray().normMax()
    myAv=ff_err.getArray().getAverageValue()

    return myMax,myAv

if __name__ == "__main__":
    myMax,myAv=getFieldErrValues("fields.med","ERROR_ELEM_ELEM_DOM","DOM")
    ff = open('Error_values.txt', 'wb')
    ff.write("%.4g %.4g".encode() %(myMax,myAv))
    ff.close()
