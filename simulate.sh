#!/usr/bin/env bash
# Run RINASim scenarios from the command line.

if ! type opp_run >/dev/null 2>/dev/null; then
    echo "opp_run isn't present in \$PATH!"
    exit 1
fi

# initialize variables
rina_ui="Cmdenv"
rina_example=""
rina_conf=""
opp_xargs=""
rina_root="$( readlink -f "$( dirname $0 )" )"
rina_lib="${rina_root}/policies/librinasim"
mode="release"
gdb=""
valg=""
ned=${rina_root}

# process command line arguments
process_args()
{
    if [ -z "$*" ]; then
        echo "Usage: ./simulate example_folder [-G] [-c configuration] [-x additional opp_run options]"
        exit 0
    elif [ -d "$1" ] && [ -f "$1"/omnetpp.ini ]; then
        rina_example="$1"
    else
        echo "Invalid example folder provided!"
        exit 1
    fi

    shift

    while getopts "dgGvc:x:n:" opt; do
      case $opt in
        "d") rina_lib="${rina_lib}_dbg"
             mode="debug" ;;
        "G") rina_ui="Qtenv" ;;
        "c") rina_conf="$OPTARG" ;;
        "g") gdb="gdb --args" ;;
        "x") opp_xargs="$OPTARG" ;;
	"n") ned="${ned}:${ned}/$OPTARG" ;;
	"v") valg="valgrind --leak-check=full --track-origins=yes" ;;
        *) ;;
      esac
    done
}

check_library()
{
    if [ -f "$1.so" ]; then
        rina_lib="$1.so"
    elif [ -f "$1.dll" ]; then
        rina_lib="$1.dll"
    else
        echo "Cannot find the RINASim dynamic library ${rina_lib}!" \
             "Forgot to compile?"
        exit 1
    fi
}

# run given simulation
# args: example folder; interface; configuration ID; extra opp_run args
run_simulation()
{
    cd "$1"
    local cmd=opp_run
    if [ $mode = "debug" ]; then
        cmd=${cmd}_dbg
    fi

    local mem
    if [ -n "$gdb" ]; then
	mem=$gdb
    elif [ -n "$valg" ]; then
	mem=$valg
    fi
    echo $mem

    $mem $cmd -u "$2" \
 	      -c "$3" \
 	      -l "$rina_lib" \
 	      $opp_xargs omnetpp.ini
}

process_args $@

check_library ${rina_lib}

export NEDPATH="${ned}"

run_simulation "$rina_example" "$rina_ui" "$rina_conf" "$opp_xargs"

