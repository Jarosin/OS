#include <stdio.h>

int main() {
    char word[256];
    char word_reverse[256];
    char len = 0;
    printf("Input a word: ");
    fgets(word, sizeof(word), stdin);
    for (int i = 0; i < 256; i++) {
        if (word[i] == '\0') {
            len = i;
            break;
        }
    }
    printf("Your word len: %d\n", len - 1);
    return 0;
}
