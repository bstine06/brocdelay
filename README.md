# BrocDelay

A healthy choice for your delay needs!

## Download

This plugin is currently in beta. To check out the latest release, [click here](https://github.com/bstine06/brocdelay/releases). Installation instructions for Mac and Windows are included in the README inside the zip for each build.

## Information

BrocDelay is a feedback delay with some extra functionality. Like most delays, it features feedback, delay time, gain, and mix parameters. There are also high- and low-pass filters to affect the frequency response of the wet signal. 

This plugin features two unique signal processing imlpementations: firstly, there is "flip-flop" setting, which inverts the stereo field of the input signal on each repetition of the delay feedback. Secondly, there are different "shift modes" you can select for "acceleration" and "deceleration" -- that is, when the delay time is being modulated to speed up or slow down, respectively. I wanted to give the user more control over the artifacts produced by the different algorithms used to affect a change in delay time.

I highly recommend modulating/automating the delay time parameter with both sync mode on and off. There is a lot of potential for textural, glitchy, and rising/downshifting types of effects when fiddling with the delay time and the shift modes.

This is my first audio effect plugin. I am using JUCE 8 with the Projucer GUI and following along with the concepts found in [The Complete Beginner’s Guide to Audio Plug-in Development by Matthijs Hollemans](https://www.theaudioprogrammer.com/books/beginners-plugin-book).

This project is still in development. I highly encourage you test it's current functionality and submit any issues, questions, or feature requests to this project's [GitHub issues page](https://github.com/bstine06/brocdelay/issues). Stay tuned for updates, and happy sound designing!
