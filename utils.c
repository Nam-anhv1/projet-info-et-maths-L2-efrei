#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

char *getID(int i)
{
    // Convertit de 1,2,3, .. ,500+ vers A,B,C,..,Z,AA,AB,...
    static char buffer[10];
    char temp[10];
    int index = 0;

    i--; // Ajustement a un index basé sur 0
    while (i >= 0)
    {
        temp[index++] = 'A' + (i % 26);
        i = (i / 26) - 1;
    }

    // Inverse la chaîne pour obtenir lordre correct
    for (int j = 0; j < index; j++)
    {
        buffer[j] = temp[index - j - 1];
    }
    buffer[index] = '\0';

    return buffer;
}