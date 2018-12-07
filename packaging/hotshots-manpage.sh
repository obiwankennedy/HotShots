#!/bin/sh
cat <<EOF
.TH syncwall 1 "April 10, 2014" "version $1" "USER COMMANDS"
.SH NAME
HotShots - Screenshot tool with annotation features
.SH SYNOPSIS
.B hotshots [options]
.I option option
.B ["
.I --help --reset-config --no-singleinstance
.B ..."] 
.SH DESCRIPTION
HotShots is an application for capturing screens and saving them in a variety of image formats as well as adding annotations and graphical data (arrows, lines, texts, ...).
You can also upload your creations to the web (FTP/some web services).
Because HotShots is written with Qt, HotShots runs on Windows, Linux (MacOSX isn't tested yet).
.SH OPTIONS
HotShot have some options of its own. As a Qt application it supports Qt
options; please see:
http://doc.trolltech.com/4.2/qapplication.html#QApplication
.TP 5
--help
displays this help
.TP
--reset-config
clear the saved preference parameters
.TP
--no-singleinstance
enable the use of multiple instance of program (not recommended).
.SH AUTHOR
xbee (xbee (at) xbee.net)
EOF