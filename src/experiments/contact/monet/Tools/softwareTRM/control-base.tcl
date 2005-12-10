#!/usr/bin/wish -f
proc scaler {n x} {
    set x2 [expr 100-$x]
    puts stdout "go 3 $n $x"
}

proc altscaler {n1 n2 x} {
    set x2 $x
    puts stdout "go $n1 $n2 $x2"
}

proc snd {glot_vol asp_vol} {
    .scroll_volume set $glot_vol
    .scroll_asp_volume set $asp_vol
}

proc fric {vol position center width} {
    .scroll_fric_volume set $vol
    .scroll_fric_position set $position
    .scroll_fric_center set $center
    .scroll_fric_width set $width
}

proc tube {r0 r1 r2 r3 r4 r5 r6 r7 velum} {
    .constrict.scroll_0 set $r0
    .constrict.scroll_1 set $r1
    .constrict.scroll_2 set $r2
    .constrict.scroll_3 set $r3
    .constrict.scroll_4 set $r4
    .constrict.scroll_5 set $r5
    .constrict.scroll_6 set $r6
    .constrict.scroll_7 set $r7
    .scroll_velum set $velum
}


frame .constrict

frame .phone

scale .constrict.scroll_0 \
    -command {scaler 0} -from 2 -to 0 -resolution 0.1

scale .constrict.scroll_1 \
    -command {scaler 1} -from 2 -to 0 -resolution 0.1

scale .constrict.scroll_2 \
    -command {scaler 2} -from 2 -to 0 -resolution 0.1

scale .constrict.scroll_3 \
    -command {scaler 3} -from 2 -to 0 -resolution 0.1

scale .constrict.scroll_4 \
    -command {scaler 4} -from 2 -to 0 -resolution 0.1

scale .constrict.scroll_5 \
    -command {scaler 5} -from 2 -to 0 -resolution 0.1

scale .constrict.scroll_6 \
    -command {scaler 6} -from 2 -to 0 -resolution 0.1

scale .constrict.scroll_7 \
    -command {scaler 7} -from 2 -to 0 -resolution 0.1

scale .scroll_volume \
    -command {altscaler 0 0} -orient horizontal -label "glottal"

scale .scroll_pitch \
    -command {altscaler 0 1} -orient horizontal

scale .scroll_asp_volume \
    -command {altscaler 1 0} -orient horizontal -label "aspiration"

scale .scroll_fric_volume \
    -command {altscaler 2 0} -orient horizontal -label "frication" \
    -from 0 -to 1 -resolution 0.01

scale .scroll_fric_position \
    -command {altscaler 2 1} -orient horizontal

scale .scroll_fric_center \
    -command {altscaler 2 2} -orient horizontal

scale .scroll_fric_width \
    -command {altscaler 2 3} -orient horizontal

scale .scroll_velum \
    -command {altscaler 4 0} -orient horizontal -label "velum" -from 0 -to 1 -resolution 0.05

scale .scroll_breathe \
    -command {altscaler 5 0} -orient horizontal -label "breathe"

scale .scroll_push \
    -command {altscaler 6 0} -orient horizontal -label "breath width"

scale .scroll_source \
    -command {altscaler 7 0} -orient horizontal -label "sound source"
