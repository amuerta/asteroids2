//
// static array stuff
//

#define ARRAY_LEN(arr)           (sizeof(arr) / sizeof((arr)[0]))
#define ARRAY_LITTERAL_LEN(...)  (ARRAY_LENGTH((__VA_ARGS__)))


// 
// Dynamic array generic macro
//
// it is just more useful then a standalone implementation 
// of dynamic array with void*
#ifndef da_append // if no "custom" da_append = implement it
#define DA_GROW_FACTOR 2

#define da_append(DA, ...) do { \
    if ((DA)->capacity == 0) {(DA)->capacity = 32; (DA)->items = calloc(32,sizeof(*(DA)->items));}\
    if ((DA)->count >= (DA)->capacity) {\
        (DA)->capacity *= DA_GROW_FACTOR;\
        (DA)->items = realloc((DA)->items,sizeof(*(DA)->items) * (DA)->capacity);\
    }\
    (DA)->items[((DA)->count)++] = (__VA_ARGS__);\
} while(0);

#endif  // da_append end


// Linked list 
//
// (good for trees too)
#ifndef li_append // linked list

#define LI_EXPECT_FIELD(LI)\
    (void)(LI)->next;     /*EXPECTED NEXT    TO EXIST*/\
    (void)(LI)->tail;     /*EXPECTED TAIL    TO EXIST*/\

#define li_append(L, I) do {\
    if(!(L)) {(L) = &(I); (L)->tail = (L);}\
    else {\
        (L)->tail->next = &(I);\
        (L)->tail = &(I);\
    }\
}while(0)

#define li_foreach(LI, T, I, ...) do {\
   T* next = (LI);\
   T* prev = (LI);\
   while(next) {\
       T* I = next;\
       {__VA_ARGS__}\
       prev = next;\
       next = next->next;\
   }\
} while(0)

#define li_defer(LI, T, ...) do {\
   T* next = (LI);\
   T* prev = (LI);\
   while(next) {\
       prev = next;\
       next = next->next;\
       {__VA_ARGS__}\
       prev = 0;\
   } (LI) = 0;\
} while(0)

#endif
