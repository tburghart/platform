#!/bin/sh -e -x

cd "$(dirname "$0")"

readonly  sysarch="$(uname -m | tr '[A-Z]' '[a-z]')"
readonly  systype="$(uname -s | tr '[A-Z]' '[a-z]')"
readonly  sysvers="$(uname -r | tr '[A-Z]' '[a-z]')"

readonly  outbase="$systype-$sysvers-$sysarch"

for d in gen scratch
do
    [[ -d "$d" ]] || mkdir "$d"
done
cat >'scratch/simple.c' <<EOF
int main(void)
{
    return 0;
}
EOF

for c in "$@" cc gcc icc
do
    CC="$(which "$c" || true)"
    [[ -n "$CC" ]] || continue

    # if it's not GCC-ish don't know how to get the predefs
    pd='-E -dM -nostdinc'
    $CC $pd -o /dev/null 'scratch/simple.c' 1>/dev/null 2>&1 || continue

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
            if $CC $cf -o /dev/null 'scratch/simple.c' 1>/dev/null 2>&1
            then
                printf '' | $CC $cf -o "scratch/$of.compiler.c" -
                cat 'platform.h' | $CC $cf -o "scratch/$of.platform.c" -
                diff "scratch/$of.compiler.c" "scratch/$of.platform.c" \
                    | grep '^> ' | grep -v '_platform_h_incl' \
                    | cut -c3- > "gen/$of.platform_defs.h"
            fi
        done
    done
done
