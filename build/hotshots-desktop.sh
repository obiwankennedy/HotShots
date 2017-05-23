#!/bin/sh
cat <<EOF
[Desktop Entry]
Version=$2
Name=HotShots
Comment=Screenshot manager
Type=Application
GenericName=Screenshot manager
TryExec=$1/bin/hotshots
Exec=$1/bin/hotshots 
Categories=Utility;Application;
Icon=$1/share/pixmaps/hotshots.png
MimeType=application/x-hot;
EOF
