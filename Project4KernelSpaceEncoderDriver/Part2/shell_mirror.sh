
#!/bin/bash
#  script to mirror pin 27 to pin 17

# export pin 17
echo 17 > /sys/class/gpio/export
sleep 0.5
echo "out" > /sys/class/gpio/gpio17/direction

# export pin 27
echo 27 > /sys/class/gpio/export
sleep 0.5
echo "in" > /sys/class/gpio/gpio27/direction


COUNTER=0
while [ $COUNTER -lt 100000 ]; do
    cat /sys/class/gpio/gpio27/value > /sys/class/gpio/gpio17/value
    let COUNTER=COUNTER+1
done

echo 17 > /sys/class/gpio/unexport



