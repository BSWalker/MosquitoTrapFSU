repo="/home/mosqtrap/MosquitoTrapFSU"
mosquito="/home/mosqtrap/mosquito"
run="/home/mosqtrap/mosquito/runnables"
log="/home/mosqtrap/mosquito/logs"
collect="home/mosqtrap/mosquito/collection-data"

rm -rf $mosquito
mkdir -p /home/mosqtrap/mosquito/{runnables,logs,collection-data}
cp $repo/startcollection.sh $run
cp $repo/stopcollection.sh $run
cp $repo/boot-test.sh $run
cp $repo/adc.x $run
cp $repo/fansense.x $run
cp $repo/crontab_file_gen.x $run
cp $repo/pressure_sensor.x $run
cp $repo/runtest.sh $run
cp $repo/tempsense.x $run
cp $repo/trap_test.x $run
cp $repo/PC_sunfile.txt $run


