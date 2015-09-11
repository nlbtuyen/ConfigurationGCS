
#################################################################
# EXTERNAL LIBRARY CONFIGURATION

## Required libs

INCLUDEPATH += $$MAVLINKPATH
isEmpty(MAVLINK_CONF) {
         INCLUDEPATH += $$MAVLINKPATH/common
} else {
	 message("Adding support for additional MAVLink messages for: " $$MAVLINK_CONF)
	 INCLUDEPATH += $$MAVLINKPATH/$$MAVLINK_CONF
	 DEFINES += $$sprintf('QGC_USE_%1_MESSAGES', $$upper($$MAVLINK_CONF))
}

# EIGEN matrix library (header-only)
INCLUDEPATH += libs libs/eigen

# Include QWT plotting library
include(libs/qwt/qwt.pri)

# Include serial port library
include(libs/thirdParty/qextserialport/src/qextserialport.pri)


