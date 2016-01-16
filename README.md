# Subtitles delayer

## Background story
[Mandatory xkcd](https://xkcd.com/196/)

I had a movie split into two CDs, but my .srt file was for the whole movie. 
Unfortunately, I noticed that VLC's subtitles delaying feature doesn't handle delaying the srt file by roughly 45 min (the length of the first movie).
Subsequently, I started writing this program to fix my subtitles and ruined my date night. 

I put this on GitHub for all people who want to save their movie date night quickly (if you came all the way here, you must be pretty despaired by now).
Also, since it was written in C++ you still have to compile it, and you can show off your geek skills by firing Visual Studio (solution and project included).


## Usage

```
subtitles_delayer <in_filename> <out_filename> <delay_ms>
```
A negative delay will make subtitles show up earlier, while a positive delay will make subtitles show up later.
