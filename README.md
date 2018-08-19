# votrax-speak
Bad software emulation of the Votrax SC-01

Not polished at all: phoneme samples and transitions between phonemes need work

## Dependencies
* Uses [eSpeak](http://espeak.sourceforge.net/) to convert words into phonemes
* Uses [pulseaudio](https://www.freedesktop.org/wiki/Software/PulseAudio/) to play the samples

## Building
```shell
git clone https://github.com/sealj553/votrax-speak.git
cd votrax-speak/phonemegen
make
```
