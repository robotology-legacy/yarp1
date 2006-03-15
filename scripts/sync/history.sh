
# get the current history of the repositories

# run fresh.sh first

rm -rf history
mkdir -p history
BASE=$PWD
for f in `cat keys.txt`; do
    echo $f
    cd state/$f
    cvs history -p `cat CVS/Repository` -x RAM -a > $BASE/history/$f
    cd $BASE
done
