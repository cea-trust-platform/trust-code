	program fft_test
c	test the execution speed of FFT
c	Note: dtime is a bsd call used to get the timings

	parameter (nsize=256*256)
	complex a(nsize),s(512*3)
	dimension is(512),m(3)
	integer wm(4*256+20),wn(4*256+20)
	real*4 timer(2),aa(2*nsize)
	ntimes=10
c	256x256 sizes for harm
	m(1)=8
	m(2)=8
	m(3)=0
c	setup tables
	ifset=0
	do j=1,nsize
		a(j)=0.1d0*cos(j*0.01)*sin(j*0.1)*sin(j*0.0333)
	enddo
	call harm(a,m,is,s,ifset,iferr)
c	initialize timer
	err=dtime(timer)
	do j=1,ntimes
	ifset=-2
	call harm(a,m,is,s,ifset,iferr)
	ifset=2
	call harm(a,m,is,s,ifset,iferr)
	enddo
	err=dtime(timer)
	call tprint(timer,'harm, 256x256',ntimes)
	
c	Now do the same for C2FFT
	do j=1,nsize
		a(j)=0.1d0*cos(j*0.01)*sin(j*0.1)*sin(j*0.0333)
	enddo
	id=256
	nm=256
	nn=256
	ISIG=0
	iord=1
	call C2FFT(A,ID,NM,NN,WM,WN,ISIG,IORD,IS,IERR)
	if(ierr.ne.0)then
		write(6,*)'Error Code ',ierr,' in C2FFT'
		stop
		endif
c	initialize timer
	err=dtime(timer)
	do j=1,ntimes
	ISIG=-1
	call C2FFT(A,ID,NM,NN,WM,WN,ISIG,IORD,IS,IERR)
	if(ierr.ne.0)then
		write(6,*)'Error Code ',ierr,' in C2FFT'
		stop
		endif
	isig=1
c	normalize
	do k=1,nm*nn
		a(k)=a(k)/(nm*nn)
		enddo
	enddo
	call C2FFT(A,ID,NM,NN,WM,WN,ISIG,IORD,IS,IERR)
	if(ierr.ne.0)then
		write(6,*)'Error Code ',ierr,' in C2FFT'
		stop
		endif
	err=dtime(timer)
	call tprint(timer,'C2FFT 256x256',ntimes)

c	setup tables
	ifset=0
	do j=1,nsize,2
		aa(j)=0.1d0*cos(j*0.01)*sin(j*0.1)*sin(j*0.0333)
		aa(j+1)=0.0d0
	enddo
	id=256
	nm=256
	nn=256
c	initialize timer
	err=dtime(timer)
	do j=1,ntimes
c	back
        call fft(aa(1),aa(2),nn*nm,nn,nn,-2)
        call fft(aa(1),aa(2),nn*nm,nn,nn*nm,-2)
c	forward
	do k=1,nm*nn*2
		aa(k)=aa(k)/(nm*nn)
		enddo
	enddo
        call fft(aa(1),aa(2),nn*nm,nn,nn,2)
        call fft(aa(1),aa(2),nn*nm,nn,nn*nm,2)
	err=dtime(timer)
	call tprint(timer,'fft 256x256',ntimes)

c	Now repeat with a size of 250x250
	do j=1,nsize
		a(j)=0.1d0*cos(j*0.01)*sin(j*0.1)*sin(j*0.0333)
	enddo
	id=250
	nm=250
	nn=250
	ISIG=0
	iord=1
	call C2FFT(A,ID,NM,NN,WM,WN,ISIG,IORD,IS,IERR)
	if(ierr.ne.0)then
		write(6,*)'Error Code ',ierr,' in C2FFT'
		stop
		endif
c	initialize timer
	err=dtime(timer)
	do j=1,ntimes
	ISIG=-1
	call C2FFT(A,ID,NM,NN,WM,WN,ISIG,IORD,IS,IERR)
	if(ierr.ne.0)then
		write(6,*)'Error Code ',ierr,' in C2FFT'
		stop
		endif
	isig=1
c	normalize
	do k=1,nm*nn
		a(k)=a(k)/(nm*nn)
		enddo
	enddo
	call C2FFT(A,ID,NM,NN,WM,WN,ISIG,IORD,IS,IERR)
	if(ierr.ne.0)then
		write(6,*)'Error Code ',ierr,' in C2FFT'
		stop
		endif
	err=dtime(timer)
	call tprint(timer,'C2FFT 250x250',ntimes)

c	setup tables
	ifset=0
	do j=1,nsize,2
		aa(j)=0.1d0*cos(j*0.01)*sin(j*0.1)*sin(j*0.0333)
		aa(j+1)=0.0d0
	enddo
	nm=250
	nn=250
c	initialize timer
	err=dtime(timer)
	do j=1,ntimes
c	back
        call fft(aa(1),aa(2),nn*nm,nn,nn,-2)
        call fft(aa(1),aa(2),nn*nm,nn,nn*nm,-2)
c	normalize
	do k=1,nm*nn*2
		aa(k)=aa(k)/(nm*nn)
		enddo
c	forward
        call fft(aa(1),aa(2),nn*nm,nn,nn,2)
        call fft(aa(1),aa(2),nn*nm,nn,nn*nm,2)
	enddo
	err=dtime(timer)
	call tprint(timer,'fft 250x250',ntimes)
	end
	
	subroutine tprint(timer,label,ntimes)
	character*(*) label
	real*4 timer(2)
	write(6,*)
	write(6,*)'Timing for routine ',label
	write(6,*)'Total User time:',timer(1)
	write(6,*)'Total System time:',timer(2)
	timer(1)=timer(1)/dble(ntimes*2)
	timer(2)=timer(2)/dble(ntimes*2)
	write(6,*)'User time per FFT:',timer(1)
	write(6,*)'System time per FFT:',timer(2)
	return
	end
	
	
	
