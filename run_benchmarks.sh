#!/bin/bash
make
for f in ./test/*.txt; do
    echo "Running benchmark $f";
    #echo "Tautology check:";
    #/usr/bin/time -v ./tc < $f 2> /tmp/ts;
    #grep 'User time|System time|Maximum resident' /tmp/ts;
    echo "Complement Computation loopback test:";
    /usr/bin/time -v ./cc < $f > /tmp/ccf 2> /tmp/ts;
    echo "CC execution stats";
    grep 'User time' /tmp/ts;
    grep 'System time' /tmp/ts;
    grep 'Maximum resident' /tmp/ts;
    num_vars=`sed -n '1p' $f`;
    num_cubes_bm=`sed -n '2p' $f`;
    num_cubes_cc=`sed -n '2p' /tmp/ccf`;
    num_cubes=$(( $num_cubes_bm + $num_cubes_cc ));
    echo "Union cube count: $num_cubes";
    echo $num_vars > /tmp/catf;
    echo $num_cubes >> /tmp/catf;
    cat $f | sed '1,2d'  >> /tmp/catf;
    cat /tmp/ccf | sed '1,2d' >> /tmp/catf;
    /usr/bin/time -v ./tc < /tmp/catf 2> /tmp/ts;
    echo "Union execution stats";
    grep 'User time' /tmp/ts;
    grep 'System time' /tmp/ts;
    grep 'Maximum resident' /tmp/ts;
done
