#
# add any your own library as necessary.
#
LIBS = 

#
# Two C source files you are asking to create for this project.
# You can add any additional files as necessary for this project.
#
SRCS = main.c errors.c fileio.c numerics.c utils.c init.c \
		optimize.c validate.c random.c operate.c operator.c \
		sort.c evaluate.c evalfunc.c
  

#
# Object files will be created. If you add any additional source file
# , add here also.
#
OBJS = main.o errors.o fileio.o numerics.o utils.o init.o \
		optimize.o validate.o random.o operate.o operator.o \
		sort.o evaluate.o evalfunc.o
#
# header file used in this project.
#
DEBUGFLAG = -g

INCLUDEDIRS = 


HDRS =	

LDFLAGS =

#
# supresses the loading phase.
#
CFLAGS = 	$(INCLUDEDIRS) $(DEBUGFLAG) -v -Xc

LDLIBS = -lm
LDFLAGS = 
#
# Final program to be made. To run this project, simply type proj1.
#
PROGRAM =	gcop3

$(PROGRAM):	main.o $(OBJS) $(LIBS)
		cc $(CFLAGS)  main.o $(OBJS) $(LDFLAGS) $(LDLIBS) -o $(PROGRAM)
		@echo type $(PROGRAM) to run.


main.o: 	main.c
		cc -c $(CFLAGS) main.c

errors.o: 	errors.c
		cc -c $(CFLAGS) errors.c

numerics.o: 	numerics.c
		cc -c $(CFLAGS) numerics.c

fileio.o: 	fileio.c
		cc -c $(CFLAGS) fileio.c

init.o: 	init.c
		cc -c $(CFLAGS) init.c
		
utils.o: 	utils.c
		cc -c $(CFLAGS) utils.c
		
optimize.o: 	optimize.c
		cc -c $(CFLAGS) optimize.c

validate.o: 	validate.c nleq.c nlineq.c
		cc -c $(CFLAGS) validate.c

random.o: 	random.c
		cc -c $(CFLAGS) random.c

operate.o: 	operate.c
		cc -c $(CFLAGS) operate.c

operator.o: 	operator.c
		cc -c $(CFLAGS) operator.c
				
sort.o: 	sort.c
		cc -c $(CFLAGS) sort.c

evaluate.o: 	evaluate.c
		cc -c $(CFLAGS) evaluate.c

evalfunc.o: 	evalfunc.c function.c
		cc -c $(CFLAGS) evalfunc.c
				
clean:		/bin/rm -f $(OBJS) core
