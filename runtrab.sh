
# execute ./runtrab.sh p1 p2 p3
# p1 number of interactions for distance
# p2 simulation time in seconds
# p3 distance to Rx in meters
#
cd ~/repos/ns-3-allinone/ns-3-dev/results
file="data.log"
if [ -f $file ] ; then
  rm $file
fi
touch data.log
var1="simTime, distanceToRx, pktReceived, pktSent, totalEnergy"
echo $var1 &>> data.log
cd ..
n=$(($1+1))
for ((i=1; i<$n; i++))
do
  k=$(($i*$3))
  echo processing file-t$2-d$k.log .........
  ./waf --run "scratch/trabprat --simTime=$2 --distanceToRx=$k" &>> results/file-t$2-d$k.log
  o1=$(cat results/file-t$2-d$k.log | grep -c Received)
  o2=$(cat results/file-t$2-d$k.log | grep -c sent)
  te=$(grep End "results/file-t$2-d$k.log" | tail -1 | grep -Eo "[0-9]+[.][0-9]+)?")
  echo pktReceived $o1
  echo pktSent $o2
  echo total energy $te
  var1="${2}, ${k}, ${o1}, ${o2}, ${te}"
  echo $var1 &>> results/data.log
  rm results/file-t$2-d$k.log
  
done
cd scratch
python stattrab.py
