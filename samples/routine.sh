for ((i=199; i<=199; i++))
do 
./setconfig
./checkmode
./disablemask
./setspeed $i 250 135
./setnormal
./checkmode
./checkspeed
./monitor
done
