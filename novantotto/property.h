#ifndef PROPERTY_H
#define PROPERTY_H

#include "novantotto.h"

#define PROPERTIES_DEFAULT_ENTRIES 8

typedef struct
{
    LPCSTR key;   // key
    HANDLE hData; // value
} PROPERTY;

typedef struct
{
    int       len;      // current length
    int       capacity; // capacity
    PROPERTY *entries;
} PROPERTIES;

extern int         FindPropertyIndex(PROPERTIES *props, LPCSTR key);
extern int         FindEmptyPropertyIndex(PROPERTIES *props);
extern HANDLE      GetProperty(PROPERTIES *props, LPCSTR key);
extern HANDLE      DelProperty(PROPERTIES *props, LPCSTR key);
extern HANDLE      SetProperty(PROPERTIES *props, LPCSTR key, HANDLE hData);
extern PROPERTIES *AllocateProperties(void);
extern void        FreeProperties(PROPERTIES *props);

#endif
