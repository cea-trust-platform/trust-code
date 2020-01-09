def rho(T, P):
    return 1000.0

def compute_rho(T, P):
    Ta, Pa = T.getArray(), P.getArray()
    rhoa = Ta.deepCopy()
    Nc = T.getMesh().getNumberOfCells()
    for i in range(Nc):
        rhoa[i] = rho(Ta[i], Pa[i])
    frho = T.clone(True) ; frho.setName("densite")
    frho.setArray(rhoa)
    return frho
