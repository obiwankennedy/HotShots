# only for macx target
mkdir -p Release/hotshots.app/Contents/translations
cp ../lang/*.qm Release/hotshots.app/Contents/translations
cp ../CREDITS.txt ../README.txt ../AUTHORS.txt ../Changelog.txt Release/hotshots.app/Contents/Resources
rm Release/*.dmg 2> /dev/null
macdeployqt Release/hotshots.app -dmg
