#!/bin/sh

make_bdm_db() {
    FILE="$1"

    echo "Making bdm database file $FILE"
    rm -f "$FILE"
    sqlite3 "$FILE" < $ROOT/data-manager/server/schema
}

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

time_watch() {
    TIME="$1"
    shift

    echo "Waiting up to $TIME seconds for '$@' to succeed"

    for i in `seq $TIME`; do
	if "$@" 2>/dev/null; then
	    break
	fi
	sleep 1
    done

    if "$@" >/dev/null 2>&1; then
	echo "...Success in $i seconds"
        return 0;
    else 
	echo "...Timeout Expired"
        return 1;
    fi
}


file_size() {
    if [ -n "$ENABLE_DARWIN" ]; then
        stat -f %z $1
    else
        stat -c %s $1
    fi
}

skip_if_no_valgrind() {
    valgrind --version > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "Can't seem to find valgrind. Skipping test"
        exit $SKIP_CODE;
    fi
}

test_log() {
	#printf "%s: %s\n" "`date +'%F %r'`" "$@"
	datestr=`date +'%F %r'`
	echo "$datestr: $@"
}

normalize_bdm_client() {
  sort $1 | sed 's/^[^,]*,//' 
}
