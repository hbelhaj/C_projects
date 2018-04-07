while read line
    do
        (wget -O - "$line" 2>/dev/null | grep -E -o 'href="http://[^"]*"' | sed -r 's/href="(.*)"/\1/' ) &
    done 

wait
