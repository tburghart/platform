#!/bin/ksh -e

cd "$(dirname "$0")"

readonly  sysarch="$(uname -m | tr '[A-Z]' '[a-z]')"
readonly  systype="$(uname -s | tr '[A-Z]' '[a-z]')"
readonly  sysvers="$(uname -r | tr '[A-Z]' '[a-z]')"

readonly  outbase="gen/$systype.$sysvers.$sysarch"

[[ -d 'gen' ]] || mkdir 'gen'
printf 'int main(void)\n{\n\treturn 0;\n}\n' > 'gen/simple.c'

for c in "$@" cc gcc icc
do
    CC="$(which "$c" || true)"
    [[ -n "$CC" ]] || continue

    # if it's not GCC-ish don't know how to get the predefs
    pd='-E -dM -nostdinc'
    $CC $pd -o /dev/null 'gen/simple.c' 1>/dev/null 2>&1 || continue

    for mm in '' 32 64
    do
        for std in '' c90 c95 c99
        do
            cf="$pd"
            of="$outbase.${CC##*/}"
            if [[ -n "$mm" ]]
            then
                cf+=" -m$mm"
                of+=".$mm"
            fi
            if [[ -n "$std" ]]
            then
                cf+=" -std=$std"
                of+=".$std"
            fi
            if $CC $cf -o /dev/null 'gen/simple.c' 1>/dev/null 2>&1
            then
                printf '' | $CC $cf -o "$of.predef.c" -
                cat 'platform.h' | $CC $cf -o "$of.platform.c" -
            fi
        done
    done
done
