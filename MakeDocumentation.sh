#!/bin/bash

WORKDIR=$(dirname $0)

cd ${WORKDIR}


doxygen Doxyfile

exit 0
