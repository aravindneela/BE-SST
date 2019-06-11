# -*- Makefile -*-
#
#

AM_CPPFLAGS = \
	$(BOOST_CPPFLAGS) \
	$(MPI_CPPFLAGS) 

compdir = $(pkglibdir)
comp_LTLIBRARIES = libbehavioralEmulation.la
libbehavioralEmulation_la_SOURCES = \
	behavioralEmulation.cc \
	BE_Component.h \
	BE_Component.cc \
	BE_CommEvent.h \
	Executor.h \
	Executor.cc \
	Routine.h \
	Routine.cc \
	Message.h \
	Message.cc \
	Layout.h \
	Router.h \
	Router.cc \
	Containers.h \
	Containers.cc \
	Simulation_Manager.h \
	Simulation_Manager.cc \
        Sim_Events.h \
        Process_Events.h \
        Event_Template.h \
	Instruction.h \
        Process.h \
	$(NULL)

EXTRA_DIST = \
    README \
    tests/test_beComponent.py \
    $(NULL)

libbehavioralEmulation_la_LDFLAGS = -module -avoid-version

##########################################################################
##########################################################################
##########################################################################


