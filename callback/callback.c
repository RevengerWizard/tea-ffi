#include <stdio.h>

/*
** gcc -shared -o callback.dll callback.c
*/

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

typedef void (*CallbackFunc)(int value);

static CallbackFunc registered_callback = NULL;

EXPORT void register_callback(CallbackFunc callback)
{
    registered_callback = callback;
    printf("C: Callback registered\n");
}

EXPORT void trigger_callback(int value)
{
    if(registered_callback)
    {
        printf("C: Triggering callback with value %d\n", value);
        registered_callback(value);
    }
    else
    {
        printf("C: No callback registered\n");
    }
}
