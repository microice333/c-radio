#!/bin/bash
sox -S "sample.mp3" -r 44100 -b 16 -e signed-integer -c 2 -t raw - | pv -q -L $((44100*4))