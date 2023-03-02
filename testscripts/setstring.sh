#!/bin/bash
#
# https://stackoverflow.com/questions/48648952/set-get-property-using-dbus-send
#

dbus-send \
	--session \
	--dest=br.app.pw3270.terminal.a\
	--print-reply \
	"/br/com/bb/tn3270/session" \
	"br.com.bb.tn3270.session.setString" \
	string:${1}

