#!/bin/sh

set -e

CMAKE=${CMAKE:-cmake}
CTEST=${CTEST:-ctest}

for JUMP in fcontext boost
do

if [ -d _build_ci ] ; then
    rm -rf _build_ci || exit 1
fi
mkdir _build_ci || exit 1

cd _build_ci || exit 1

${CMAKE} $@ ..
echo "======================================"
echo "                Build (${JUMP})"
echo "======================================"
${CMAKE} --build . || exit 1

make

echo "======================================"
echo "         Running unit tests (${JUMP})"
echo "======================================"
echo

if ${CTEST} -V ; then
    echo "Test run has finished successfully (${JUMP})"
    cd ..
else
    echo "Test run failed (${JUMP})" >&2
    exit 1
fi

done
