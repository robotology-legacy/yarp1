for f in `cat inventory.txt`; do
    xv src/$f.ppm &
#    xv mask/$f.ppm &
    read x
    echo $x | tee label/$f.txt
    killall xv
done
