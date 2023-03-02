#!/bin/bash
#
# https://stackoverflow.com/questions/48648952/set-get-property-using-dbus-send
#
. ./dbus.conf

gdbus \
	call \
	--session \
	--dest ${DBUS_DEST} \
	--object-path "${DBUS_PATH}" \
	--method org.freedesktop.DBus.Properties.Get \
	"${DBUS_INTERFACE}" \
	"url"

