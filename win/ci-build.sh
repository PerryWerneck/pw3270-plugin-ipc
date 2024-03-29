#!/bin/bash
#
# References:
#
#	* https://www.msys2.org/docs/ci/
#
#
echo "Running ${0}"

LOGFILE=build.log
rm -f ${LOGFILE}

die ( ) {
	[ -s $LOGFILE ] && tail $LOGFILE
	[ "$1" ] && echo "$*"
	exit -1
}

myDIR=$(dirname $(dirname $(readlink -f ${0})))
echo "myDIR=${myDIR}"

cd ${myDIR}
rm -fr ${myDIR}/.build

#
# Unpack LIB3270
#
echo "Unpacking lib3270"
tar -C / -Jxvf ${MINGW_PACKAGE_PREFIX}-lib3270.tar.xz 

#
# Build LIBV3270
#
echo "Unpacking libv3270"
tar -C / -Jxvf ${MINGW_PACKAGE_PREFIX}-libv3270.tar.xz 

#
# Build IPC Plugin
#
echo "Building IPC plugin"
cd ${myDIR}
./autogen.sh > $LOGFILE 2>&1 || die "Autogen failure"
./configure > $LOGFILE 2>&1 || die "Configure failure"
make clean > $LOGFILE 2>&1 || die "Make clean failure"
make all  > $LOGFILE 2>&1 || die "Make failure"

make DESTDIR=.bin/package install > $LOGFILE 2>&1 || die "Install failure"
tar --create --xz --file=${MINGW_PACKAGE_PREFIX}-pw3270-plugin-ipc.tar.xz --directory=.bin/package --verbose . > $LOGFILE 2>&1 || die "Package build failure"

echo "Build complete"

