def main2():
    ok=1
    nb_proc=0
    id=0
    liste_action=[]

    import socket


    from port import PORT
    HOST = ''                 # Symbolic name meaning all available interfaces
    # Arbitrary non-privileged port
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((HOST, PORT))
    s.listen(1)
    
    while ok:
        
        conn, addr = s.accept()
        # print 'Connected by', addr
        # while 1:
        data = conn.recv(1024)
        # if  not data: 
        #    break
        try:
            import server
            reload(server)
            ok,nb_proc,id=server.traite(conn,data,liste_action,nb_proc,id)
        except :
            print "error", data 
            import sys
            print "Unexpected error:", sys.exc_info()[1]

            
    s.close()
    pass

if  __name__ == '__main__':
    main2()
    pass
