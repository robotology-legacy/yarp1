#!/usr/bin/wish -f
proc scaler {n x} {
    set x2 [expr 100-$x]
    puts stdout "go 3 $n $x"
}

proc altscaler {n1 n2 x} {
    set x2 $x
    puts stdout "go $n1 $n2 $x2"
}



frame .constrict

scale .constrict.scroll_0 \
    -command {scaler 0}

scale .constrict.scroll_1 \
    -command {scaler 1}

scale .constrict.scroll_2 \
    -command {scaler 2}

scale .constrict.scroll_3 \
    -command {scaler 3}

scale .constrict.scroll_4 \
    -command {scaler 4}

scale .constrict.scroll_5 \
    -command {scaler 5}

scale .constrict.scroll_6 \
    -command {scaler 6}

scale .constrict.scroll_7 \
    -command {scaler 7}

scale .scroll_volume \
    -command {altscaler 0 0} -orient horizontal -label "glottal"

scale .scroll_pitch \
    -command {altscaler 0 1} -orient horizontal

scale .scroll_asp_volume \
    -command {altscaler 1 0} -orient horizontal -label "aspiration"

scale .scroll_fric_volume \
    -command {altscaler 2 0} -orient horizontal -label "frication"

scale .scroll_fric_position \
    -command {altscaler 2 1} -orient horizontal

scale .scroll_fric_center \
    -command {altscaler 2 2} -orient horizontal

scale .scroll_fric_width \
    -command {altscaler 2 3} -orient horizontal

scale .scroll_velum \
    -command {altscaler 4 0} -orient horizontal -label "velum"



pack .constrict.scroll_7 -side right -fill y
pack .constrict.scroll_6 -side right -fill y
pack .constrict.scroll_5 -side right -fill y
pack .constrict.scroll_4 -side right -fill y
pack .constrict.scroll_3 -side right -fill y
pack .constrict.scroll_2 -side right -fill y
pack .constrict.scroll_1 -side right -fill y
pack .constrict.scroll_0 -side right -fill y
pack .constrict -side top -fill x

pack .scroll_velum -side bottom -fill x
pack .scroll_fric_width -side bottom -fill x
pack .scroll_fric_center -side bottom -fill x
pack .scroll_fric_position -side bottom -fill x
pack .scroll_fric_volume -side bottom -fill x
pack .scroll_asp_volume -side bottom -fill x
pack .scroll_pitch -side bottom -fill x
pack .scroll_volume -side bottom -fill x
