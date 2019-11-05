docker run -ti --rm -e DISPLAY=$DISPLAY --net="host" -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v $HOME/.Xauthority:/home/vivado/.Xauthority -v ${PWD}/project:/home/vivado/project \
    petalinux:2017.4 /bin/bash
