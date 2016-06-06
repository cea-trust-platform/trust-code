def main2():
    ok=1
   
    class Sjob_info:
        pass
    sjob_info=Sjob_info()
    sjob_info.nb_proc=0
    sjob_info.id=0
    sjob_info.nb_proc_max=10000

    
    liste_action=[]

    import socket

    import sys
    try: 
        sys.dont_write_bytecode = True
    except:
        pass

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
            ok,n,id=server.traite(conn,data,liste_action,sjob_info)
            assert (id==sjob_info.id)
            assert (n==sjob_info.nb_proc)
            
        except :
            print "error", data 
            import sys
            print "Unexpected error:", sys.exc_info()[1]

            
    s.close()
    pass

if  __name__ == '__main__':
    main2()
    pass
