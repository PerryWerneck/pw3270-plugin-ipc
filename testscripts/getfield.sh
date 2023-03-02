#!/bin/bash

. ./dbus.conf

dbus-send \
	--session \
	--dest=${DBUS_DEST}\
	--print-reply \
	"${DBUS_PATH}" \
	"${DBUS_INTERFACE}.getField"

