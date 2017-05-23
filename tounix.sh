#
find . -type f -name \*.cpp | xargs dos2unix 
find . -type f -name \*.h | xargs dos2unix
find . -type f -name \*.ui | xargs dos2unix
find . -type f -name \*.sh | xargs dos2unix
find . -type f -name \*.pro | xargs dos2unix
find . -type f -name \*.txt | xargs dos2unix
find . -type f -name \*.cfg | xargs dos2unix
