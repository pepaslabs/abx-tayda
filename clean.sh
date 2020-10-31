#!/bin/bash

set -e

rm -vf *.kicad_pcb-bak
rm -vf *-cache.lib
rm -vf *.bak

cd parts
./clean.sh
