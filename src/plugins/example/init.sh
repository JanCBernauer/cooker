#!/bin/bash
EXPECTED_ARGS=1
E_BADARGS=65

if [ $# -ne $EXPECTED_ARGS ]
then
  echo "Usage: `basename $0` <pluginname>"
  exit $E_BADARGS
fi


upper=`echo $1|tr '[:lower:]' '[:upper:]'`
echo "Renaming to $1"

echo "Renaming init/* "
mv init/example.xml init/$1.xml
mv init/example.xsd init/$1.xsd



echo "Updating Det_*.cc"
sed -e "s/Det_example/$1/g" -e "s/DET_EXAMPLE/${upper}/g" <src/Det_example.cpp >src/$1.cpp

echo "Updating Det_*.h"
sed -e "s/Det_example/$1/g" -e "s/DET_EXAMPLE/${upper}/g" <src/include/Det_example.h >src/include/$1.h

echo "Updateing LinkDef.h"
sed -e "s/Det_example/$1/g" -e "s/DET_EXAMPLE/${upper}/g" <src/include/Link.hh >src/include/LinkDef.hh



echo "Updating CMakeLists.txt"
sed -e "s/example/$1/g" -e "s/EXAMPLE/${upper}/g" <CMakeLists.in >CMakeLists.txt

echo "Cleaning up"
rm src/Det_example.cpp src/include/Det_example.h src/include/Link.hh CMakeLists.in
echo "All done." 
echo "Check linked libs in CMakeLists.txt!"
echo "Don't forget to add your new plugin to ../CMakeLists.txt"
rm init.sh