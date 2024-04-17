#include <stdio.h>
#include <arm_neon.h>
#include <string>

typedef union {
    int8x16_t int128;

    uint8_t m128_u8[16];
    int8_t m128_i8[16];

    uint16_t m128_u16[8];
    int16_t m128_i16[8];

    uint32_t m128_u32[4];
    int32_t m128_i32[4];

    uint64_t m128_u64[2];
    int64_t m128_i64[2];
} intVec;

void print_int_vector(int8x16_t a, const char* type)
{
    intVec tmp;
    tmp.int128 = a;
    printf ("[");
    if (std::string(type) == "u8")
    {
        for (int i=15; i>0; i--) 
            printf ("%X, ", tmp.m128_u8[i]);
        printf ("%X]\n\n", tmp.m128_u8[0]);
    }
    else if (std::string(type) == "i8")
    {
        for (int i=15; i>0; i--) 
            printf ("%d, ", tmp.m128_i8[i]);
        printf ("%d]\n\n", tmp.m128_i8[0]);
    }
    else if (std::string(type) == "u16")
    {
        for (int i=7; i>0; i--) 
            printf ("%X, ", tmp.m128_u16[i]);
        printf ("%X]\n\n", tmp.m128_u16[0]);
    }
    else if (std::string(type) == "i16")
    {
        for (int i=7; i>0; i--) 
            printf ("%d, ", tmp.m128_i16[i]);
        printf ("%d]\n\n", tmp.m128_i16[0]);
    }
    else if (std::string(type) == "u32")
    {
        for (int i=3; i>0; i--) 
            printf ("%X, ", tmp.m128_u32[i]);
        printf ("%X]\n\n", tmp.m128_u32[0]);
    }
    else if (std::string(type) == "i32")
    {
        for (int i=3; i>0; i--) 
            printf ("%d, ", tmp.m128_i32[i]);
        printf ("%d]\n\n", tmp.m128_i32[0]);
    }
    else if (std::string(type) == "u64")
    {
        for (int i=1; i>0; i--) 
            printf ("%lX, ", tmp.m128_u64[i]);
        printf ("%lX]\n\n", tmp.m128_u64[0]);
    }
    else if (std::string(type) == "i64")
    {
        for (int i=1; i>0; i--) 
            printf ("%ld, ", tmp.m128_i64[i]);
        printf ("%ld]\n\n", tmp.m128_i64[0]);
    }
}

typedef union {
    float32x4_t spfd128;

    float m128_ps[4];
} spfpVec;

void print_spfp_vector(float32x4_t a)
{
    spfpVec tmp;
    tmp.spfd128 = a;
    printf("[");
    for (int i=3; i>0; i--) 
        printf ("%f, ", tmp.m128_ps[i]);
    printf ("%f]\n\n", tmp.m128_ps[0]);
}

uint8_t intArray [16] = { 0X00, 0X11, 0X22, 0X33, 0X44, 0X55, 0X66, 0X77,
                          0X88, 0X99, 0XAA, 0XBB, 0XCC, 0XDD, 0XEE, 0XFF };

float psArray [4] = { -2.0, -10.0, 6.0, 3.14 };

int main(void)
{
    int8x16_t a;
    a = vld1q_s8((const int8_t*)intArray);

    printf ("Unsigned byte: ");
    print_int_vector(a, "u8");
    printf ("signed byte: ");
    print_int_vector(a, "i8");

    printf ("Unsigned word: ");
    print_int_vector(a, "u16");
    printf ("signed word: ");
    print_int_vector(a, "i16");

    printf ("Unsigned double word: ");
    print_int_vector(a, "u32");
    printf ("signed double word: ");
    print_int_vector(a, "i32");

    printf ("Unsigned quad word: ");
    print_int_vector(a, "u64");
    printf ("signed quad word: ");
    print_int_vector(a, "i64");




    return 0;
}
