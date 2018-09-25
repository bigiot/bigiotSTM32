#ifndef __SPK_H
#define	__SPK_H

void char2voice(char * str, int char_set);
int change(const char s[], char bits[]);
void saveCache(char * str, int index);
void playCache(int times, int char_set);
int file2voice(char * path, int char_set);

#endif
