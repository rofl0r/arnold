STSound recording feature
=========================

The allows you to record the PSG output in the YM3
format which can be played with STSound. The output
file, has the extension ".YM3". Before this file can
be played with STSound, it must be converted into
a ".YM" file. This is done, by compressing the file
using LHA.

lha -h0 <filename>.ym <filename>.ym3

To start recording the sound, select "STSound->Start
Recording". You will be asked for the name of the output
".YM3" file. Recording will start, and will continue
until you select "STSound->Stop Recording". While
recording, a temporary file holds the raw PSG sound
data. When "Stop Recording" is selected, this raw data
is read in and converted to the ".YM3" format.

You can start and stop capture of PSG sound at any time.

This feature, currently, will only capture normal 
musics. It cannot handle Digi-Tracker music, or sounds
with Digi-effects. Therefore, it will not be able
to capture some CPC sounds properly.
