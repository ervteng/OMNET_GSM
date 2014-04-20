#
# OMNeT++/OMNEST Makefile for OMNET_GSM
#
# This file was generated with the command:
#  opp_makemake -f --deep -I../../inet/src/linklayer/ethernet -I../../inet/src/linklayer/ieee80211/radio -I../../inet/src/networklayer/common -I../../inet/src -I../../inet/src/networklayer/icmpv6 -I../../inet/src/world/obstacles -I../../inet/src/networklayer/xmipv6 -I../../inet/src/networklayer/contract -I../../inet/src/mobility -I../../inet/src/mobility/models -I../../inet/src/networklayer/manetrouting/base -I../../inet/src/networklayer/autorouting/ipv4 -I../../inet/src/linklayer/ieee80211/mgmt -I../../inet/src/util -I../../inet/src/linklayer/common -I../../inet/src/transport/contract -I../../inet/src/status -I../../inet/src/linklayer/radio/propagation -I../../inet/src/linklayer/ieee80211/radio/errormodel -I../../inet/src/linklayer/radio -I../../inet/src/util/headerserializers/tcp -I../../inet/src/networklayer/ipv4 -I../../inet/src/util/headerserializers/ipv4 -I../../inet/src/base -I../../inet/src/util/headerserializers -I../../inet/src/world/radio -I../../inet/src/linklayer/ieee80211/mac -I../../inet/src/transport/sctp -I../../inet/src/networklayer/ipv6 -I../../inet/src/util/headerserializers/udp -I../../inet/src/networklayer/ipv6tunneling -I../../inet/src/battery/models -I../../inet/src/applications/pingapp -I../../inet/src/linklayer/contract -I../../inet/src/util/headerserializers/sctp -I../../inet/src/transport/tcp_common -I../../inet/src/networklayer/arp -I../../inet/src/transport/udp -L../../inet/out/$$\(CONFIGNAME\)/src -linet -DINET_IMPORT -KINET_PROJ=../../inet
#

# Name of target to be created (-o option)
TARGET = OMNET_GSM$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I../../inet/src/linklayer/ethernet \
    -I../../inet/src/linklayer/ieee80211/radio \
    -I../../inet/src/networklayer/common \
    -I../../inet/src \
    -I../../inet/src/networklayer/icmpv6 \
    -I../../inet/src/world/obstacles \
    -I../../inet/src/networklayer/xmipv6 \
    -I../../inet/src/networklayer/contract \
    -I../../inet/src/mobility \
    -I../../inet/src/mobility/models \
    -I../../inet/src/networklayer/manetrouting/base \
    -I../../inet/src/networklayer/autorouting/ipv4 \
    -I../../inet/src/linklayer/ieee80211/mgmt \
    -I../../inet/src/util \
    -I../../inet/src/linklayer/common \
    -I../../inet/src/transport/contract \
    -I../../inet/src/status \
    -I../../inet/src/linklayer/radio/propagation \
    -I../../inet/src/linklayer/ieee80211/radio/errormodel \
    -I../../inet/src/linklayer/radio \
    -I../../inet/src/util/headerserializers/tcp \
    -I../../inet/src/networklayer/ipv4 \
    -I../../inet/src/util/headerserializers/ipv4 \
    -I../../inet/src/base \
    -I../../inet/src/util/headerserializers \
    -I../../inet/src/world/radio \
    -I../../inet/src/linklayer/ieee80211/mac \
    -I../../inet/src/transport/sctp \
    -I../../inet/src/networklayer/ipv6 \
    -I../../inet/src/util/headerserializers/udp \
    -I../../inet/src/networklayer/ipv6tunneling \
    -I../../inet/src/battery/models \
    -I../../inet/src/applications/pingapp \
    -I../../inet/src/linklayer/contract \
    -I../../inet/src/util/headerserializers/sctp \
    -I../../inet/src/transport/tcp_common \
    -I../../inet/src/networklayer/arp \
    -I../../inet/src/transport/udp \
    -I. \
    -Iresults

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = -L../../inet/out/$(CONFIGNAME)/src  -linet
LIBS += -Wl,-rpath,`abspath ../../inet/out/$(CONFIGNAME)/src`

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/air.o \
    $O/bsc.o \
    $O/bts.o \
    $O/GSMMac.o \
    $O/GSMRadio.o \
    $O/GSMRadioModel.o \
    $O/ms.o

# Message files
MSGFILES =

# Other makefile variables (-K)
INET_PROJ=../../inet

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS) -DINET_IMPORT $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(Q)$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	$(qecho) Cleaning...
	$(Q)-rm -rf $O
	$(Q)-rm -f OMNET_GSM OMNET_GSM.exe libOMNET_GSM.so libOMNET_GSM.a libOMNET_GSM.dll libOMNET_GSM.dylib
	$(Q)-rm -f ./*_m.cc ./*_m.h
	$(Q)-rm -f results/*_m.cc results/*_m.h

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc results/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/GSMMac.o: GSMMac.cc \
	GSMMac.h
$O/GSMRadio.o: GSMRadio.cc \
	GSMRadio.h \
	$(INET_PROJ)/src/base/BasicModule.h \
	$(INET_PROJ)/src/base/Coord.h \
	$(INET_PROJ)/src/base/ILifecycle.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/base/INotifiable.h \
	$(INET_PROJ)/src/base/ModuleAccess.h \
	$(INET_PROJ)/src/base/NotificationBoard.h \
	$(INET_PROJ)/src/base/NotifierConsts.h \
	$(INET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INET_PROJ)/src/linklayer/radio/AirFrame_m.h \
	$(INET_PROJ)/src/linklayer/radio/INoiseGenerator.h \
	$(INET_PROJ)/src/linklayer/radio/IRadioModel.h \
	$(INET_PROJ)/src/linklayer/radio/ModulationType.h \
	$(INET_PROJ)/src/linklayer/radio/Radio.h \
	$(INET_PROJ)/src/linklayer/radio/SnrList.h \
	$(INET_PROJ)/src/linklayer/radio/propagation/IReceptionModel.h \
	$(INET_PROJ)/src/util/FWMath.h \
	$(INET_PROJ)/src/world/annotations/AnnotationManager.h \
	$(INET_PROJ)/src/world/obstacles/Obstacle.h \
	$(INET_PROJ)/src/world/obstacles/ObstacleControl.h \
	$(INET_PROJ)/src/world/radio/ChannelAccess.h \
	$(INET_PROJ)/src/world/radio/IChannelControl.h
$O/GSMRadioModel.o: GSMRadioModel.cc \
	GSMRadioModel.h \
	$(INET_PROJ)/src/base/Coord.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/linklayer/radio/AirFrame_m.h \
	$(INET_PROJ)/src/linklayer/radio/IModulation.h \
	$(INET_PROJ)/src/linklayer/radio/IRadioModel.h \
	$(INET_PROJ)/src/linklayer/radio/Modulation.h \
	$(INET_PROJ)/src/linklayer/radio/ModulationType.h \
	$(INET_PROJ)/src/linklayer/radio/SnrList.h \
	$(INET_PROJ)/src/util/FWMath.h
$O/air.o: air.cc \
	air.h \
	gsmsim.h
$O/bsc.o: bsc.cc \
	bsc.h \
	gsmsim.h
$O/bts.o: bts.cc \
	GSMRadio.h \
	bts.h \
	gsmsim.h \
	$(INET_PROJ)/src/base/BasicModule.h \
	$(INET_PROJ)/src/base/Coord.h \
	$(INET_PROJ)/src/base/ILifecycle.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/base/INotifiable.h \
	$(INET_PROJ)/src/base/ModuleAccess.h \
	$(INET_PROJ)/src/base/NotificationBoard.h \
	$(INET_PROJ)/src/base/NotifierConsts.h \
	$(INET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INET_PROJ)/src/linklayer/radio/AirFrame_m.h \
	$(INET_PROJ)/src/linklayer/radio/INoiseGenerator.h \
	$(INET_PROJ)/src/linklayer/radio/IRadioModel.h \
	$(INET_PROJ)/src/linklayer/radio/ModulationType.h \
	$(INET_PROJ)/src/linklayer/radio/Radio.h \
	$(INET_PROJ)/src/linklayer/radio/Radio80211aControlInfo_m.h \
	$(INET_PROJ)/src/linklayer/radio/SnrList.h \
	$(INET_PROJ)/src/linklayer/radio/propagation/IReceptionModel.h \
	$(INET_PROJ)/src/util/FWMath.h \
	$(INET_PROJ)/src/world/annotations/AnnotationManager.h \
	$(INET_PROJ)/src/world/obstacles/Obstacle.h \
	$(INET_PROJ)/src/world/obstacles/ObstacleControl.h \
	$(INET_PROJ)/src/world/radio/ChannelAccess.h \
	$(INET_PROJ)/src/world/radio/IChannelControl.h
$O/ms.o: ms.cc \
	GSMRadio.h \
	gsmsim.h \
	ms.h \
	$(INET_PROJ)/src/base/BasicModule.h \
	$(INET_PROJ)/src/base/Coord.h \
	$(INET_PROJ)/src/base/ILifecycle.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/base/INotifiable.h \
	$(INET_PROJ)/src/base/ModuleAccess.h \
	$(INET_PROJ)/src/base/NotificationBoard.h \
	$(INET_PROJ)/src/base/NotifierConsts.h \
	$(INET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INET_PROJ)/src/linklayer/radio/AirFrame_m.h \
	$(INET_PROJ)/src/linklayer/radio/INoiseGenerator.h \
	$(INET_PROJ)/src/linklayer/radio/IRadioModel.h \
	$(INET_PROJ)/src/linklayer/radio/ModulationType.h \
	$(INET_PROJ)/src/linklayer/radio/Radio.h \
	$(INET_PROJ)/src/linklayer/radio/SnrList.h \
	$(INET_PROJ)/src/linklayer/radio/propagation/IReceptionModel.h \
	$(INET_PROJ)/src/mobility/IMobility.h \
	$(INET_PROJ)/src/mobility/models/LinearMobility.h \
	$(INET_PROJ)/src/mobility/models/MobilityBase.h \
	$(INET_PROJ)/src/mobility/models/MovingMobilityBase.h \
	$(INET_PROJ)/src/util/FWMath.h \
	$(INET_PROJ)/src/world/annotations/AnnotationManager.h \
	$(INET_PROJ)/src/world/obstacles/Obstacle.h \
	$(INET_PROJ)/src/world/obstacles/ObstacleControl.h \
	$(INET_PROJ)/src/world/radio/ChannelAccess.h \
	$(INET_PROJ)/src/world/radio/IChannelControl.h

