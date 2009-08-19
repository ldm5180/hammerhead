#!/bin/sh


time_wait() {
    PID="$1"
    TIME="$2"

    sleep 1;
    echo "Waiting for $PID for $TIME seconds "

    waited=0
    while [ "$waited" -lt "$TIME" ] && kill -0 $PID; do
        sleep 1;
        waited=`expr $waited '+' '1'`
        echo -n "."
    done;
    echo "."

    if kill -0 $PID; then
        return 1;
    else 
        return 0;
    fi
}


