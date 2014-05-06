OOWR
====

OSX OpenCV Webcam Recording Utility

This software was created to let OSX users record from cameras (originally an IEEE1394 Basler camera) connected to their machine.

It is based on OpenCV and requires the boost library in order to create an independent recording thread.

# Software details
In order to work, Oowr requires a lot of dependencies.
- Homebrew
- libboost
- OpenCV
- Several hardware libraries

# Install instructions
<pre>
brew --version
# if this does not output a number, homebrew is not installed and you must run
# ruby -e "$(curl -fsSL https://raw.github.com/Homebrew/homebrew/go/install)"
# don't forget to brew doctor your system.

brew install git

brew install boost
# This is quite long :)

pip install numpy

brew install libraw

install XQuartz from https://xquartz.macosforge.org/landing/

brew install libdc1394
# This is optional, and is required in order to use a IEEE1394 camera (a.k.a. any firewire camera).
# It may give you errors when installing SDL,
# see the following *Issues with libdc1394 and SDL* section in this readme

brew install /homebrew/science/opencv --with-ffmpeg --without-quicktime
# These options are important, OpenCV has a hard time encoding and writing your videos otherwise.</pre>


### Issues with libdc1394 and SDL
Installing libdc1394 will install sdl on your machine, as it is one of its dependencies. You will probably get "palette" related errors, if you do, you'll need to edit the recipe.
<pre>brew edit sdl</pre>
Add lines:
<pre>patch do
  url "http://hg.libsdl.org/SDL/raw-rev/bbfb41c13a87"
end</pre>

To the section of the recipe where you see other patches.
Now it installs :)

### More issues with libdc1394
Sometimes opencv won't find libdc1394, because homebrew can't find the headers. In that case run:
<pre>brew install /homebrew/science/opencv --with-ffmpeg --without-quicktime -v 2>&1</pre>
and make sure the configure log says it has found libDC1394. If it hasn't, solve your $PATH issues. [One way of doing this is here.](https://github.com/bpinto/oh-my-fish)

# Usage instructions
Use the compile script to generate the *live* executable. Then you can try in a terminal:
<pre>./live --help</pre>
to get general information about the available options or
<pre>./live --fps 50</pre>
to dive immediatly into action!

# Thanks
I would like to acknowledge [Aakash Patel](http://aaka.sh/patel/) for his great [blog post](http://aaka.sh/patel/2013/06/28/live-video-webcam-recording-with-opencv/) on using libboost to get real live recording from a camera. I also acknowledge all resources on the *new* syntax of OpenCV I could find...
