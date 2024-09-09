# Hand detect

Detect hand motion from video (in real time from webcamera or from a video file)

![](sample.jpg)

Dependencies:
```sh
sudo apt install libopencv-dev
```

Installation:
``` sh
mkdir build && cd build
cmake ..
make
```

Run:
``` sh
./motion <path to file>
```

Either provide a path to a video file, or don't provide any arguments (to use web camera).
Press ESC to quit.

Example program output:

```
Num poi: 1
Area: 50740	 Aspect: 1.71512	 Center: [86, 251.5]
Num poi: 1
Area: 46955.3	 Aspect: 1.48216	 Center: [68.5418, 242.087]
Num poi: 1
Area: 38973.6	 Aspect: 1.57136	 Center: [79.4218, 227.175]
Num poi: 1
Area: 41099.5	 Aspect: 1.44962	 Center: [90.5813, 229.786
```

Also see the image shown above (the hand is surrounded by a faint rectangle).

License: MIT
