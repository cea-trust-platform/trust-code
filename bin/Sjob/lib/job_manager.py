import os

def sendall_str(conn, s):
    conn.sendall(bytes(s, "UTF-8"))

class Job(object):
    """ A single job in the queue """
    def __init__(self):
        self.id = -1      # Internal ID of the job
        self.pid = -1     # PID of the job
        self.pere = -1    # PID of the father in case of recursive call to Salloc
        self.status="W"   # W: Waiting, R:Running, S:Suspended
        self.children = []  # Children IDs of current job in case it has been suspended.
        self.nb=1         # nb of procs requested by the job
        self.host="-"     #
        self.cmd = ""     # The command line passed to Salloc - just for info
        self.cmd_short = "" # Short version

    def __str__(self):
        """ Short representation of a job """
        msg = "#%d (PID %d) -- %s" % (self.id, self.pid, self.cmd_short)
        return msg

    def get_long_status(self):
        if self.status != "S":
            return self.status
        c = self.children
        c.sort()
        # nice formatting to have packs instead of long lists...
        # "1,2,3,4,5,6" becomes "1->6"
        l = []
        while c != []:
            if len(c) == 1:
                l.append("%d" % c[0])
                break
            prev, ln, i  = c[0], len(c[1:]), -1
            end_done = False
            for i, cc in enumerate(c[1:]):
                if cc != prev+1:
                    if i == ln-1:  end_done = True
                    if i==0:
                        l.append("%d" % c[0])
                        c = c[1:]
                        break
                    if c[0]+1 == prev:  l.extend([str(c[0]), str(prev)])
                    else:               l.append("%d->%d" % (c[0], prev))
                    c = c[i+1:]
                    break
                else:
                    prev = cc
            if i == ln-1 and not end_done:
                if c[0]+1 == prev:  l.extend([str(c[0]), str(prev)])
                else:               l.append("%d->%d" % (c[0], prev))
                break

        return "S[%s]" % (",".join(l))

    def gen_short_com(self):
        a = self.cmd.split()
        l = []
        for aa in a:
            b = aa.split("/")
            l.append(b[-1])
        self.cmd_short = " ".join(l)

class SJobManager(object):
    """ Main class doing the allocations """
    NB_PROC_MAX_DFLT = 1000

    def __init__(self):
      self.nb_proc = 0           # Number of proc allocated to Sjob at startup
      self.nb_proc_max = SJobManager.NB_PROC_MAX_DFLT  # Max number of procs a job can request
      self.id_cnter = 0          # Internal counter to generate current job IDs
      self.liste_action = []     # List of current jobs submitted

    def generate_id(self):
      """ A unique ID for a job. """
      v = self.id_cnter
      self.id_cnter+= 1
      return v

    def get_job_with_id(self, i):
      for j in self.liste_action:
          if j.id == i:
              return j
      return None

    def get_job_with_pid(self, i):
      for j in self.liste_action:
          if j.pid == i:
              return j
      return None

    def remove_and_resume_father(self, job, warn=True):
      # Resume father, if any:
      if (job.pere != -1):
        father = self.get_job_with_id(job.pere)
        if father is None:
            if warn: print("WARNING - job #%d has a father (#%d) but it cannot be found!" %(job.id, job.pere))
        else:
            if not job.id in father.children:
                print("WARNING -> could not find job #%d in the children of job #%d" % (job.id, father.id))
            else:
                father.children.remove(job.id)
            # Resume father when all its children have terminated:
            if father.children == []:
                print ("Resuming job #%d (status was %s)" % (father.id,father.status))
                if father.status[0] != "S":
                    raise Exception("INTERNAL ERROR - trying to resume #%d which is not suspended (status=%s)" % (father.id,father.status))
                father.status = "R"
      self.liste_action.remove(job)

    def srun(self, conn, args):
        """ Schedule a job for running. The core of the mechanism is that no data is sent back 
        to the client.py script as long as the job is not effectively running. This effectively blocks
        the Salloc script until the job is scheduled to run. """
        job = Job()
        job.id = self.generate_id()
        job.pid = int(args[1])
        job.nb=int(args[0])
        job.conn=conn
        pere = int(args[2])
        job.cmd = " ".join(args[3:])
        job.gen_short_com()

        if (pere >= 0):   # PERE information was provided
#             print ("srun invoked with father PID %s" % pere)
            father = self.get_job_with_pid(pere)
            if father is None:
                raise Exception("ERROR: srun invoked with parent PID which cannot be found (%d)" % pere)
            job.pere=father.id
            if father.status != "S":
                father.status="S"
                print("Suspending -> %s" % str(father))
            # Append the job to the list that the father will have to wait for ...
            father.children.append(job.id)
        self.liste_action.append(job)

    def ping_PID(self, pid):
        """ Test if a process is still alive """
        try:
            os.kill(pid, 0)
        except OSError:
            return False
        return True

    def kill_process_and_children_DNU(self, pid):
        """ This version does not properly kill child processes ..."""
        GRACE_TIME_SEC = 2    # time before sending SIGKILL
        MAX_TIMEOUT_SEC = 30  # after this we consider that we could not kill the process
        import time
        import signal

        os.kill(pid, signal.SIGTERM)
        time.sleep(GRACE_TIME_SEC)
        if self.ping_PID(pid):
            cnt = 0
            # Still there?
            if not self.ping_PID(pid):
                return True
            # More aggressive now:
            while cnt < MAX_TIMEOUT_SEC:
                os.kill(pid, signal.SIGKILL)
                time.sleep(1)
                if not self.ping_PID(pid):
                    break
                cnt += 1
                if cnt == MAX_TIMEOUT_SEC:
                    print("WARNING - could not kill properly PID %d" % pid)
                    return False
        return True

    def _safe_terminate(self, proc):
        import psutil
        try:
            proc.terminate()
        except psutil.NoSuchProcess:
            pass

    def _safe_kill(self, proc):
        import psutil
        try:
            proc.kill()
        except psutil.NoSuchProcess:
            pass

    def kill_process_and_children(self, pid):
        GRACE_TIME_SEC = 1    # time before sending SIGKILL
        MAX_TIMEOUT_SEC = 30  # after this we consider that we could not kill the process
        import psutil

        try:
            mainp = psutil.Process(pid)
        except psutil.NoSuchProcess:
            return True       # Process already dead - all good.
        procs = mainp.children(recursive=True)
        all_procs = procs + [mainp]
        for p in all_procs:
            # print("(Inclusive) child of %d is: %d" % (pid, p.pid))
            self._safe_terminate(p)
        _, alive = psutil.wait_procs(all_procs, timeout=GRACE_TIME_SEC)
        if alive == []:
            return True
        cnt = 0
        while cnt < MAX_TIMEOUT_SEC:
            if alive == []:
                break
            for p in alive:
                self._safe_kill(p)
            _, alive = psutil.wait_procs(all_procs, timeout=1)
            cnt += 1
            if cnt == MAX_TIMEOUT_SEC:
                print("WARNING - could not kill properly PID %d" % pid)
                return False
        return True

    def kill_and_delete(self, a, warn=True):
        if a.status[0]=="W":
            try:
                # Message has to start with KILL - this will trigger exit(1) in client.py
                sendall_str(a.conn,"EXIT: Killed and deleted job ID #%d" % a.id)
                a.conn.close()
            except:
                pass # In case we are killing a tree, the children might already have died.
        self.kill_process_and_children(a.pid)
        print("Killed and deleted -> %s" % str(a))
        self.remove_and_resume_father(a, warn)

    def sdelete(self, conn, args):
        try:
            idj=int(args[0])
        except:
            sendall_str(conn,"sdelete - invalid job id: '%s'" % str(args))
            conn.close()
            return
        OK="NO"
        a = self.get_job_with_id(idj)
        if a is None:
            OK = "could not find job!"
        else:
            # If the process is part of a tree, we kill the whole tree:
            if a.pere != -1:
              b = self.get_job_with_id(a.pere)
              if not b is None:
                a = b
            chld = a.children
            self.kill_and_delete(a)
            # Also kill children if any:
            for c in chld:
                jb = self.get_job_with_id(c)
                if not jb is None:
                    self.kill_and_delete(jb)
            OK = "OK"
        sendall_str(conn,"sdelete "+OK)
        conn.close()

    def kill_all(self, conn):
        for a in self.liste_action[:]:  # copy of the list since kill_and_delete() removes from the list...
            self.kill_and_delete(a)
        sendall_str(conn,"Killed all jobs in queue!")
        conn.close()

    def finish(self, conn, args):
        pidj=int(args[0])
        OK="NO"

        a = self.get_job_with_pid(pidj)
        if self.get_job_with_pid(pidj) is None:
            print("WARNING: 'finish' invoked with invalid PID (%d)" % pidj)
            OK="invalid PID (%d)" % pidj
        else:
            if a.status[0]=="W":
                sendall_str(a.conn,"Deleted job with PID %d" % pidj) # this is not a kill
                a.conn.close()
            self.remove_and_resume_father(a)
            OK = "OK %d" % a.id
            print("Finished -> %s" % str(a))
        sendall_str(conn,"finish "+OK)
        conn.close()

    def squeue(self, conn, args):
        """ Print current job queue """
        msg="JOBID\tPARENT\tSTATUS\tNB_PROC\tPID\tSHORT_COMMAND\n"
        use=0
        l = []
        for job in self.liste_action:
            s = job.get_long_status()
            per = {-1: "N/A"}.get(job.pere, str(job.pere))
            l.append([job.id, per, s, job.nb, job.pid, job.cmd_short])
            if job.status == "R":
                use+=job.nb
        key_fun = lambda sub_l: (sub_l[2], sub_l[0])
        l.sort(key=key_fun)
        ll = map(lambda x: "%d\t%s\t%s\t%d\t%d\t%s" % (x[0], x[1], x[2], x[3], x[4], x[5]), l)
        msg += "\n".join(ll)
        msg_head = "%d used out of %d (with an exceptional max of %d)\n"  % (use, self.nb_proc, self.nb_proc_max)
        sendall_str(conn,msg_head + msg)
        conn.close()

    def launch_next_job(self):
        if not (self.nb_proc and len(self.liste_action)):
            return

        use=0
        for job in self.liste_action:
            if job.status=="R" :
                use+=job.nb
        for job in self.liste_action:
            if job.status=="W":
                if (job.nb>self.nb_proc_max):
                    print("WARNING: killing job because too many procs requested -> %s" % str(job))
                    sendall_str(job.conn,"EXIT: killing #%d - too many proc requested!!" % job.id)
                    self.kill_process_and_children(job.pid)
                    self.remove_and_resume_father(job)
                    continue
                if (use+min(job.nb,self.nb_proc)<=self.nb_proc):  # Allowing jobs requesting more than what's available
                    if self.kill_if_dead_father(job):
                        continue
                    job.status="R"
                    print("Run -> %s" % str(job))
                    sendall_str(job.conn,"Running #%d" % job.id)  # This unlocks the script client.py called from Salloc
                    job.conn.close()
                    use+=job.nb
#             else:
#               if job.status!="R" :
#                 print("Supsend job ? %d" % job.id)

    def kill_if_dead_father(self, job):
      if job.pere >= 0:
          if self.get_job_with_id(job.pere) is None:
              print ("WARNING: Job #%d has non-existent father (#%d)! Deleting it." % (job.id, job.pere))
              self.kill_and_delete(job, warn=False)
              return True
      return False

    def refresh(self):
        """ Detect jobs that were killed abnormally outside of the framework """
        remove_ajob=True
        while remove_ajob:
            remove_ajob=False
            for job in self.liste_action:
                # if job.status=="R":
                if not self.ping_PID(job.pid):
                    print ("Job killed outside? -> %s" % str(job))
                    self.remove_and_resume_father(job)
                    remove_ajob=True
                    break
                if self.kill_if_dead_father(job):
                    break

    def handle(self, conn,data):
        """ Main routine """
#         print ("[INCOMING]: '%s'" % data)
        self.refresh()
        cmd = data.split()[0]
        args = data.split()[1:]
        if (cmd == "srun"):
            self.srun(conn, args)
        elif (cmd=="sdelete") :
            self.sdelete(conn, args)
        elif (cmd=="finish") :
            self.finish(conn, args)
        elif (cmd=="squeue"):
            self.squeue(conn, args)
        elif (cmd=="up"):
            self.nb_proc+=1
            sendall_str(conn,"up %d"%self.nb_proc)
            conn.close()
        elif (cmd=="down"):
            self.nb_proc-=1
            sendall_str(conn,"down %d"%self.nb_proc)
            conn.close()
        elif (cmd=="set_nb_proc"):
            self.nb_proc=int(args[0])
            sendall_str(conn,"Setting nb_proc to %d" % self.nb_proc)
            conn.close()
        elif (cmd=="set_nb_proc_max"):
            self.nb_proc_max=int(args[0])
            sendall_str(conn,"Setting nb_proc_max to %d"%self.nb_proc_max)
            conn.close()
        elif (cmd=="stop"):
            sendall_str(conn,"stop")
            conn.close()
            return 0
        elif (cmd=="kill_and_stop"):
            self.kill_all(conn)
            return 0
        elif (cmd=="kill_all"):
            self.kill_all(conn)
        else:
            print("ERROR command not understood: " + data)
            sendall_str(conn,'Command not understood!')
            conn.close()
            return 1

        self.launch_next_job()
        return 1
