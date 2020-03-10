from time import sleep

def gen():
    fNameIn = "/export/home/adrien/support/Trio_U/dynamic/long.dt_ev"
    fNameOut = "/export/home/adrien/support/Trio_U/dynamic/long_part.dt_ev"

    with open(fNameIn) as fIn:
        lines = fIn.readlines()
        with open(fNameOut, "w") as fOut:
            cnt = 1
            for lin in lines:
                sleep(0.001)
                le = len(lin)
                cnt = (cnt % 3) + 1
                fOut.write(lin[:le/cnt])
                fOut.flush()
#         sleep(0.01)
                fOut.write(lin[le/cnt:])
                fOut.flush()

if __name__ == "__main__":
    gen()
