#ifndef CPU_COLOURS_ENUM_H
#define CPU_COLOURS_ENUM_H

#define DEF_COLOUR(name, esc_string) COLOUR_##name,

enum Colours {
    #include "colours.h"
};

#undef DEF_COLOUR

#endif //CPU_COLOURS_ENUM_H
