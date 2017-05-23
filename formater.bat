cd src
dir   /b  *.cpp *.h > lfiles.txt
uncrustify.exe -F lfiles.txt --no-backup -c ..\uncrustify.cfg
del lfiles.txt
cd uploaders
dir   /b  *.cpp *.h > lfiles.txt
uncrustify.exe -F lfiles.txt --no-backup -c ..\..\uncrustify.cfg
del lfiles.txt
cd ..
cd editor
dir   /b  *.cpp *.h > lfiles.txt
uncrustify.exe -F lfiles.txt --no-backup -c ..\..\uncrustify.cfg
del lfiles.txt
cd items
dir   /b  *.cpp *.h >> lfiles.txt
uncrustify.exe -F lfiles.txt --no-backup -c ..\..\..\uncrustify.cfg
del lfiles.txt
cd ..
cd io
dir   /b  *.cpp *.h >> lfiles.txt
uncrustify.exe -F lfiles.txt --no-backup -c ..\..\..\uncrustify.cfg
del lfiles.txt
cd ..
cd widgets
dir   /b  *.cpp *.h >> lfiles.txt
uncrustify.exe -F lfiles.txt --no-backup -c ..\..\..\uncrustify.cfg
del lfiles.txt
pause
