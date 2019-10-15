#!/usr/bin/python3

import sys
try:
    sys.dont_write_bytecode = True
except:
    pass
import os

FINISH = False
manager = None
zeLock = None

def periodicRefresh():
    """ Periodic refresh launched in a thread to handle jobs killed outside the Sserver """
    global FINISH, manager, zeLock
    import time
    while not FINISH:
        zeLock.acquire()
        manager.refresh()
        zeLock.release()
        time.sleep(2)

def main1():
    global manager, FINISH, zeLock

    import threading
    import socket
    from port import PORT
    import job_manager

    zeLock = threading.Lock()
    manager = job_manager.SJobManager()
    th = threading.Thread(target=periodicRefresh)
    th.start()
    HOST = '' # Symbolic name meaning all available interfaces
    # Arbitrary non-privileged port
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((HOST, PORT))
    s.listen(1)

    ok=1
    while ok:
        conn, addr = s.accept()
        data = conn.recv(job_manager.MAX_SEND_RCV_SIZE)
        zeLock.acquire()
        ok = manager.handle(conn,data.decode("UTF-8"))
        zeLock.release()
    FINISH = True
    th.join()
    s.close()

def main2():
    import socket
    from port import PORT
    import job_manager

    manager = job_manager.SJobManager()
    HOST = '' # Symbolic name meaning all available interfaces
    # Arbitrary non-privileged port
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((HOST, PORT))
    s.listen(1)

    ok=1
    while ok:
        conn, addr = s.accept()
        data = conn.recv(1024)
        try:
            reload(job_manager)  ## [ABN] hmmm, not too sure now this is in a class ...
            ok = manager.handle(conn,data)
        except :
            print ("Error: %s" % data)
            print ("Unexpected error:")
            print(sys.exc_info()[1])

    s.close()
    pass

if  __name__ == '__main__':
    main1()
    pass
