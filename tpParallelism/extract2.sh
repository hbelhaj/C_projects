#!/bin/sh
wget -O - "$1" 2>/dev/null | grep -E -o 'href="http://[^"]*"' | sed -r 's/href="(.*)"/\1/'

