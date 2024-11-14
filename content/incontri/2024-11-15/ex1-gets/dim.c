#include <stdio.h>

void impossibile_da_chiamare() {
    printf("Come hai fatto???\n");
}

void funzione_normalissima() {
    char buffer[64];
    printf("Come ti chiami? ");
    gets(buffer);  // Non usarmi!
    printf("Ciao %s!!\n", buffer);
}

int main() {
    printf("Qualche professore amerebbe questo programma\n");
    funzione_normalissima();
    return 0;
}
