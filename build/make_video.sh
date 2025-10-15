#!/bin/bash
rm output.mp4
ffmpeg -framerate 30 -i images/Step%05d.ppm -c:v libx264 -pix_fmt yuv420p -crf 18 output.mp4
