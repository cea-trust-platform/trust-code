
class Job:
    def __init__(self):
        self.status="W"
        self.pere=-1
    def affiche(self):
        
        return str(self.id)+" "+self.name+" "+str(self.nb)+" "+self.status+" "+self.pid+" \n"


def remove_job(job,liste_action):
    if (job.pere!=-1):
        for a in  liste_action:
            if a.id==job.pere:
                print "fin suspend ",a.id,a.status
                a.status="R"
    liste_action.remove(job)
def traite(conn,data,liste_action,nb_proc,id):
    
    print data
    for job in liste_action:
        # if job.status=="R":
        import os
        te=os.system("kill -18 "+job.pid+"  2>/dev/null")
        if te:
            print job.affiche(),job.pid,"Job killed ?"
            remove_job(job,liste_action)
            break
   
    if (data.split()[0]=="srun"):
        job=Job()
        job.id=id
    
        job.pid=data.split()[2]
        if (len(data.split())>3):
            print "pere",len(data.split())  ,data.split()[3]
            pere=data.split()[3]
            
            for a in liste_action:
                
                if a.pid==pere:
                    a.status="S_"
                    job.pere=a.id
                    a.status+=str(id)
            
        id+=1
        job.name="srun"
        job.nb=int(data.split()[1])
        job.conn=conn
        liste_action.append(job)
       
    elif (data.split()[0]=="sdelete") :
        idj=int(data.split()[1])
        OK="NO"
        for a in liste_action:
            if a.id==idj:
                if a.status=="W":
                    a.conn.sendall("deleted "+data.split()[1])
                    a.conn.close()
                import os
                os.system("kill -9 "+a.pid)
                remove_job(a,liste_action)
                OK="OK"
                break
        conn.sendall(data.split()[0]+" "+OK)
        conn.close()
    elif (data.split()[0]=="finish") :
        pidj=(data.split()[1])
        OK="NO"
        for a in liste_action:
            # print "iii",a.pid
            if a.pid==pidj:
                if a.status=="W":
                    a.conn.sendall("deleted "+data.split()[1])
                    a.conn.close()
                remove_job(a,liste_action)
                OK="OK "+str(a.id)
                break
        conn.sendall(data.split()[0]+" "+OK)
        conn.close()

    elif (data.split()[0]=="squeue"):
        msg="JOBID NAME NB STATUS PID\n"
        use=0
        for job in liste_action:
            msg+=job.affiche()
            if job.status=="R":
                use+=job.nb
        msg+=str(use)+" used of "+ str(nb_proc)
        conn.sendall(msg)
        conn.close()
    elif (data.split()[0]=="up"):
        nb_proc+=1
        conn.sendall("up %s"%nb_proc)
        conn.close()
    elif (data.split()[0]=="down"):
        nb_proc-=1
        conn.sendall("down %s"%nb_proc)
        conn.close()
    elif (data.split()[0]=="set_nb_proc"):
        nb_proc=int(data.split()[1])
        conn.sendall("nb_proc %s"%nb_proc)
        conn.close()  
    elif (data.split()[0]=="stop"):
        conn.sendall("stop")
        conn.close()
        return 0,nb_proc,id
    else:
        print "error",data
        conn.sendall('iii')
        conn.close()
        return 1,nb_proc,id


    use=0
    if nb_proc and len(liste_action):
        for job in liste_action:
            if job.status=="W":
                if (use+min(job.nb,nb_proc)<=nb_proc):
                    job.status="R"
                    job.conn.sendall("Running "+str(job.id))
                    job.conn.close()
                    use+=job.nb
            elif job.status=="R" :
                use+=job.nb
            else:
                print "supsend job ?",job.id
        pass
    return 1,nb_proc,id

def main():
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
        ok,nb_proc,id=traite(conn,data,liste_action,nb_proc,id)
        
    s.close()
    pass

if  __name__ == '__main__':
    main()
    pass
