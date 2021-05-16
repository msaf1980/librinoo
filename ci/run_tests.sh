#!/bin/bash

set -e

CMAKE=${CMAKE:-cmake}
CTEST=${CTEST:-ctest}

if [ -d _build_ci ] ; then
        rm -rf _build_ci
fi
mkdir _build_ci

pushd _build_ci


${CMAKE} -DCMAKE_BUILD_TYPE=Release $@ ..
echo "======================================"
echo "                Build"
echo "======================================"
${CMAKE} --build .

make

echo "======================================"
echo "         Running unit tests"
echo "======================================"
echo

${CTEST} -V

popd

echo "Test run has finished successfully"
