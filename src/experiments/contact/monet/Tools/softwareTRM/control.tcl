#!/usr/bin/wish -f
proc scaler {n x} {
    set x2 [expr 100-$x]
    puts stdout "go 3 $n $x2"
}

scaler 0 4

scale .scroll_0 \
    -command {scaler 0}

scale .scroll_1 \
    -command {scaler 1}

scale .scroll_2 \
    -command {scaler 2}

scale .scroll_3 \
    -command {scaler 3}

scale .scroll_4 \
    -command {scaler 4}

scale .scroll_5 \
    -command {scaler 5}

scale .scroll_6 \
    -command {scaler 6}

scale .scroll_7 \
    -command {scaler 7}


pack .scroll_7 -side right -fill y
pack .scroll_6 -side right -fill y
pack .scroll_5 -side right -fill y
pack .scroll_4 -side right -fill y
pack .scroll_3 -side right -fill y
pack .scroll_2 -side right -fill y
pack .scroll_1 -side right -fill y
pack .scroll_0 -side right -fill y
