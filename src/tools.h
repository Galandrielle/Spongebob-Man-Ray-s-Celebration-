#ifndef TOOLS_H
#define TOOLS_H
//This is the main library of this startup project, it has a couple image libraries, a hash map, a
//dynamic array sean barrett style, an arena allocator, quaternions, matrices, vectors, all that. Use at your own risk!! :)

#if defined(BUILD_WIN32)
#include "windows.h"
#endif

#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#if !defined(CRTLESS)
#define CRTLESS 0
#endif

typedef uint8_t   u8;
typedef int8_t    i8;
typedef uint16_t  u16;
typedef int16_t   i16;
typedef uint32_t  u32;
typedef int32_t   i32;
typedef uint64_t  u64;
typedef int64_t   i64;
typedef float     f32;
typedef double    f64;
typedef int32_t   b32;
typedef char      b8;

#define INLINE static inline
#define INTERNAL static

#define kilobytes(val) ((val)*1024LL)
#define megabytes(val) ((kilobytes(val))*1024LL)
#define gigabytes(val) ((megabytes(val))*1024LL)
#define terabytes(val) ((gigabytes(val))*1024LL)

#define PI 3.1415926535897f

#ifdef CRTLESS | 1 
#define offsetof2(type, member) ((unsigned int)((unsigned char*)&((type*)0)->member - (unsigned char*)0)) 
INLINE i32 abs2(i32 x)
{
  return x < 0 ? -x : x;
}
INLINE f32 fabs2(f32 x)
{
    return x < 0 ? -x : x;
}


//cos_32 computes a cosine to about 3.2 decimal digits of accuracy
INLINE f32 
cos_32s(f32 x)
{
    const float c1= 0.99940307;
    const float c2=-0.49558072;
    const float c3= 0.03679168;
    float x2; // The input argument squared
    x2= x * x;
    return (c1 + x2*(c2 + c3 * x2));
}

//this is a horrible implementation change pls
INLINE f32 fmodf2(f32 a, f32 b)
{
    f32 div = a/b;
    i32 mod = (i32)div;
    return (f32)mod;
}
INLINE f32
cos_32(f32 x){
    i32 quad; // what quadrant are we in?
    x= fmodf(x,(2.f*PI)); // Get rid of values > 2* pi
    if(x<0)x=-x; // cos(-x) = cos(x)
    quad=(i32)(x/(PI/2.f)); // Get quadrant # (0 to 3) switch (quad){
    switch(quad){
        case 0: return cos_32s(x);
        case 1: return -cos_32s(PI-x);
        case 2: return -cos_32s(x-PI);
        case 3: return cos_32s((2.f*PI)-x);
        default: return cos_32s(x);
    }
}

INLINE f32 sin_32(f32 x)
{
    return cos_32(PI/2.f - x);
}
#define sinf(x) sin_32(x)
#define cosf(x) cos_32(x)
#endif
INLINE f32 fmodf(f32 a, f32 b)
{
    f32 div = a/b;
    i32 mod = (i32)div;
    return (f32)mod;
}

#define align_pow2(val, align) (((val) + ((align) - 1)) & ~(((val) - (val)) + (align) - 1))
#define align4(val) (((val) + 3) & ~3)
#define align8(val) (((val) + 7) & ~7)
#define align16(val) (((val) + 15) & ~15)

#define equalf(a, b, epsilon) (fabs(b - a) <= epsilon)
#define maximum(a, b) ((a) > (b) ? (a) : (b))
#define minimum(a, b) ((a) < (b) ? (a) : (b))
#define step(threshold, value) ((value) < (threshold) ? 0 : 1) 
#define clamp(x, a, b)  (maximum(a, minimum(x, b)))
#define array_count(a) (sizeof(a) / sizeof((a)[0]))

//make an ifdef for each different platform
#define ALLOC(x) malloc(x)
#define REALLOC(x, y) realloc(x, y)
#define FREE(x) free(x)

#ifdef SIN_APPROX
#define SINF sin_32
#endif

#ifdef COS_APPROX
#define COSF cos_32
#endif



INLINE b32 is_pow2(u32 val)
{
    b32 res = ((val & ~(val - 1)) == val);
    return(res);
}
static b32
char_is_alpha(i32 c)
{
    return ((c >='A' && c <= 'z') || (c >= 'a' && c <= 'z'));
}
static b32 char_is_digit(i32 c)
{
    return c >= '0'&& c <= '9';
}

static i32 char_to_lower(i32 c)
{
    if (c >= 'A' && c <= 'z')
    {
        c += 32;
    }
    return c;
}

static u32 
str_size(char* str)
{
    u32 i = 0;
    while (str[i] != 0)++i;
    return i;
}

static u32
find_char_in_string(char *string,i32 start_index, char tofind)
{
    i32 iter = start_index;
    while (string[iter] != '\0' && string[iter] != tofind) iter++;
    return iter;
}

static void
seed_random()
{
    srand((u32)time(0));
}

//random between [-1,1]
INLINE f32
random(void)
{
    //seed_random();
    f32 r = (f32)rand();
	r /= RAND_MAX;
	r = 2.0f * r - 1.0f;
	return r;
}

INLINE f32
random01(void)
{
    //seed_random();
    f32 r = (f32)rand();
	r /= RAND_MAX;
	return r;
}

INLINE f32 
rrandom(f32 lo, f32 hi)
{
    //seed_random();
	f32 r = (f32)rand();
	r /= RAND_MAX;
	r = (hi - lo) * r + lo;
	return r;
}


//NOTE(ilias): maybe make a free_file because our game leaks :(
INLINE char * 
read_whole_file(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = (char*)ALLOC(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0; 

    return (char*)string;
}
INLINE u32
get_file_size(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f)return 0;
    fseek(f, 0, SEEK_END);
    u32 fsize = ftell(f);
    return fsize;
}

INLINE b32 
file_exists(char* filename) {
  b32 ret;
  FILE* fp = fopen(filename, "rb");
  if (fp) {
    ret = 1;
    fclose(fp);
  } else {
    ret = 0;
  }

  return ret;
}

static i32 
get_num_from_string(char *str)
{
   char num[64];
   int k = 0;
   i32 i = 0;
   while (str[i] != '\0')
   {
      if (char_is_digit(str[i]))
      {
          num[k++] = str[i];
      }else if (k > 0)break; //we only get the first number in the string
      ++i;
   }
   num[k] = '\0';
   i32 res = atoi(num);
   return res;
}

//MATH LIB
typedef union vec2
{
    struct
    {
        f32 x,y;
    };
    struct 
    {
        f32 u,v;
    };
    struct
    {
        f32 r,g;
    };
    f32 elements[2];
#ifdef __cplusplus
    inline f32 &operator[](i32 &index)
    {
        return elements[index];
    }
#endif

}vec2;


typedef union vec3
{
    struct
    {
        f32 x,y,z;
    };
    struct
    {
        f32 r,g,b;
    };
    f32 elements[3];
#ifdef __cplusplus
    inline f32 &operator[](i32 &index)
    {
        return elements[index];
    }
#endif
}vec3;

typedef vec3 color3;
typedef vec3 float3;

typedef union vec4
{
    struct
    {
        f32 x,y,z,w;
    };
    struct
    {
        f32 r,g,b,a;
    };
    f32 elements[4];
#ifdef TOOLS_SSE
    __m128 elements_sse; //because __m128 = 128 = 4 * float = 4*32 = 128 bits
#endif
#ifdef __cplusplus
    inline f32 &operator[](i32 &index)
    {
        return elements[index];
    }
#endif

}vec4;

typedef vec4 color4;
typedef vec4 float4;

typedef union mat4
{
    f32 elements[4][4];//{x.x,x.y,x.z,0,y.x,y.y,y.z,0,z.x,z.y,z.z,0,p.x,p.y,p.z,1} 

    f32 raw[16]; //{x.x,x.y,x.z,0,y.x,y.y,y.z,0,z.x,z.y,z.z,0,p.x,p.y,p.z,1} 

#ifdef TOOLS_SSE
    __m128 cols[4]; //same as elements (our matrices are column major)
#endif
#ifdef __cplusplus
    inline vec4 operator[](i32 &Index)
    {
        f32* col = elements[Index];

        vec4 res;
        res.elements[0] = col[0];
        res.elements[1] = col[1];
        res.elements[2] = col[2];
        res.elements[3] = col[3];

        return res;
    }
#endif
}mat4;


typedef union ivec3
{
    struct
    {
        i32 x,y,z;
    };
    struct
    {
        i32 r,g,b;
    };
    i32 elements[3];
}ivec3;

typedef union ivec2
{
    struct
    {
        i32 x,y;
    };
    struct
    {
        i32 r,g;
    };
    i32 elements[2];
}ivec2;


INLINE b32 equals_ivec3(ivec3 l, ivec3 r)
{
    i32 res = ((l.x == r.x) && (l.y == r.y) && (l.z == r.z));
    return res;
}

INLINE f32 to_radians(float degrees)
{
    f32 res = degrees * (PI / 180.0f);
    return(res);
}

INLINE f32 lerp(f32 A, f32 B, f32 t)
{
    f32 res = (1.0f - t)*A + t*B;
    return res;
}

INLINE vec2 v2(f32 x, f32 y)
{
    vec2 res;
    res.x = x;
    res.y = y;
    return res;
}

INLINE vec3 v3(f32 x, f32 y, f32 z)
{
    vec3 res;
    res.x = x;
    res.y = y;
    res.z = z;
    return res;
}

INLINE vec4 v4(f32 x, f32 y, f32 z, f32 w)
{
    vec4 res;
    res.x = x;
    res.y = y;
    res.z = z;
    res.w = w;
    return res;
}

INLINE vec2 add_vec2(vec2 l, vec2 r)
{
    vec2 res;
    res.x = l.x + r.x;
    res.y = l.y + r.y;
    return res;
}

INLINE vec2 add_vec2f(vec2 v,f32 val)
{
    vec2 res;
    res.x = v.x + val;
    res.y = v.y + val;
    return res;
}


INLINE vec2 sub_vec2(vec2 l, vec2 r)
{
    vec2 res;
    res.x = l.x - r.x;
    res.y = l.y - r.y;
    return res;
}

INLINE vec2 sub_vec2f(vec2 v,f32 val)
{
    vec2 res;
    res.x = v.x - val;
    res.y = v.y - val;
    return res;
}

INLINE vec2 mul_vec2(vec2 l, vec2 r)
{
    vec2 res;
    res.x = l.x * r.x;
    res.y = l.y * r.y;
    return res;
}

INLINE vec2 mul_vec2f(vec2 l, f32 r)
{
    vec2 res;
    res.x = l.x * r;
    res.y = l.y * r;
    return res;
}

INLINE vec2 div_vec2(vec2 l, vec2 r)
{
    vec2 res;
    res.x = l.x / r.x;
    res.y = l.y / r.y;
    return res;
}

INLINE vec2 div_vec2f(vec2 l, f32 r)
{
    vec2 res;
    res.x = l.x / r;
    res.y = l.y / r;
    return res;
}

INLINE f32 dot_vec2(vec2 l, vec2 r)
{
    f32 res = (l.x + r.x)+(l.y + r.y); // Ó(Ai*Bi)
    return res;
}

INLINE vec2 sqrt_vec2(vec2 v)
{
    vec2 res;
    res.x = sqrt(v.x);
    res.y = sqrt(v.y);
    return res;
}

INLINE vec2 rotate_vec2(vec2 v, f32 a) {
	vec2 res;
    f32 sn = sin(a);
	f32 cs = cos(a);
    res.x = v.x * cs - v.y * sn;
    res.y = v.x * sn + v.y * cs;
    return res;
} 

INLINE f32 length_vec2(vec2 v)
{
    f32 res = sqrt(dot_vec2(v,v)); // (x^2 + y^2)^(1/2)
    return res;
}

INLINE vec2 abs_vec2(vec2 v)
{
    vec2 res = v2(fabs(v.x), fabs(v.y));
    return res;
}
   
INLINE vec2 normalize_vec2(vec2 v)
{
    vec2 res = {0}; //in case length is zero we return zero vector
    f32 vec_length = length_vec2(v);
    if (vec_length > 0.1)
    {
        res.x = v.x * (1.0f/vec_length);
        res.y = v.y * (1.0f/vec_length);
    }
    return res;
}

INLINE vec3 add_vec3(vec3 l, vec3 r)
{
    vec3 res;
    res.x = l.x + r.x;
    res.y = l.y + r.y;
    res.z = l.z + r.z;
    return res;
}

INLINE vec3 sub_vec3(vec3 l, vec3 r)
{
    vec3 res;
    res.x = l.x - r.x;
    res.y = l.y - r.y;
    res.z = l.z - r.z;
    return res;
}

INLINE vec3 mul_vec3(vec3 l, vec3 r)
{
    vec3 res;
    res.x = l.x * r.x;
    res.y = l.y * r.y;
    res.z = l.z * r.z;
    return res;
}

INLINE vec3 mul_vec3f(vec3 l, f32 r)
{
    vec3 res;
    res.x = l.x * r;
    res.y = l.y * r;
    res.z = l.z * r;
    return res;
}

INLINE vec3 div_vec3(vec3 l, vec3 r)
{
    vec3 res;
    res.x = l.x / r.x;
    res.y = l.y / r.y;
    res.z = l.z / r.z;
    return res;
}

INLINE vec3 div_vec3f(vec3 l, f32 r)
{
    vec3 res;
    res.x = l.x / r;
    res.y = l.y / r;
    res.z = l.z / r;
    return res;
}

INLINE f32 dot_vec3(vec3 l, vec3 r)
{
    f32 res = (l.x * r.x)+(l.y * r.y)+(l.z * r.z); // Ó(Ai*Bi)
    return res;
}

INLINE f32 length_vec3(vec3 v)
{
    f32 res = sqrt(dot_vec3(v,v)); // (x^2 + y^2)^(1/2)
    return res;
}

INLINE vec3 rotate_vec3(vec3 v, f32 a)
{
    vec3 res;
    //TBA
    return res;
}


INLINE vec3 normalize_vec3(vec3 v)
{
    vec3 res = {0}; //in case length is zero we return zero vector
    f32 vec_length = length_vec3(v);
    if (vec_length != 0)
    {
        res.x = v.x * (1.0f/vec_length);
        res.y = v.y * (1.0f/vec_length);
        res.z = v.z * (1.0f/vec_length);
    }
    return res;
}

INLINE vec3 lerp_vec3(vec3 l, vec3 r, f32 time)
{
    vec3 res;

    f32 x = lerp(l.x, r.x, time);
    f32 y = lerp(l.y, r.y, time);
    f32 z = lerp(l.z, r.z, time);
    res = v3(x,y,z); 
    
    return res;
}

INLINE vec3 cross_vec3(vec3 l,vec3 r)
{
    vec3 res;
    res.x = (l.y*r.z) - (l.z*r.y);
    res.y = (l.z * r.x) - (l.x*r.z);
    res.z = (l.x * r.y) - (l.y * r.x);
    return (res);
}

INLINE vec3 blender_to_opengl_vec3(vec3 v)
{
    return v3(v.x, -v.z, v.y);
}


INLINE vec4 add_vec4(vec4 l, vec4 r)
{
    vec4 res;
#ifdef TOOLS_SSE
    res.elements_sse = _mm_add_ps(l.elements_sse, r.elements_sse);
#else
    res.x = l.x + r.x;
    res.y = l.y + r.y;
    res.z = l.z + r.z;
    res.w = l.w + r.w;
#endif
    return res;
}

INLINE vec4 sub_vec4(vec4 l, vec4 r)
{
    vec4 res;
#ifdef TOOLS_SSE
    res.elements_sse = _mm_sub_ps(l.elements_sse, r.elements_sse);
#else
    res.x = l.x - r.x;
    res.y = l.y - r.y;
    res.z = l.z - r.z;
    res.w = l.w - r.w;
#endif
    return res;
}

INLINE vec4 mul_vec4(vec4 l, vec4 r)
{
    vec4 res;
#ifdef TOOLS_SSE
    res.selements_sse = _mm_mul_ps(l.elements_sse, r.elements_sse);
#else
    res.x = l.x * r.x;
    res.y = l.y * r.y;
    res.z = l.z * r.z;
    res.w = l.w * r.w;
#endif
    return res;
}

INLINE vec4 mul_vec4f(vec4 l, f32 r)
{
    vec4 res;
#ifdef TOOLS_SSE
    __m128 scalar = _mm_set1_ps(r); // [r r r r]
    res.elements_sse = _mm_mul_ps(l.elements_sse, scalar);
#else
    res.x = l.x * r;
    res.y = l.y * r;
    res.z = l.z * r;
    res.w = l.w * r;
#endif
    return res;
}

INLINE vec4 div_vec4(vec4 l, vec4 r)
{
    vec4 res;
#ifdef TOOLS_SSE
    res.elements_sse = _mm_div_ps(l.elements_sse, r.elements_sse);
#else
    res.x = l.x / r.x;
    res.y = l.y / r.y;
    res.z = l.z / r.z;
    res.w = l.w / r.w;
#endif
    return res;
}

INLINE vec4 div_vec4f(vec4 l, f32 r)
{
    vec4 res;
#ifdef TOOLS_SSE
    __m128 scalar = _mm_set1_ps(r);
    res.elements_sse = _mm_div_ps(l.elements_sse, r.elements_sse);
#else
    res.x = l.x / r;
    res.y = l.y / r;
    res.z = l.z / r;
    res.w = l.w / r;
#endif
    return res;
}

INLINE f32 dot_vec4(vec4 l, vec4 r)
{
    f32 res = (l.x + r.x)+(l.y + r.y)+(l.z + r.z)+(l.w + r.w); // Ó(Ai*Bi)
    return res;
}

INLINE f32 length_vec4(vec4 v)
{
    f32 res = sqrt(dot_vec4(v,v)); // (x^2 + y^2)^(1/2)
    return res;
}
   
INLINE vec4 normalize_vec4(vec4 v)
{
    vec4 res = {0}; //in case length is zero we return zero vector
    f32 vec_length = length_vec4(v);
    if (!equalf(vec_length, 0.f, 0.01))
    {
        res.x = v.x * (1.0f/vec_length);
        res.y = v.y * (1.0f/vec_length);
        res.z = v.z * (1.0f/vec_length);
        res.w = v.w * (1.0f/vec_length);
    }
    return res;
}


INLINE mat4 m4(void)
{
    mat4 res = {0};
    return res;
}

INLINE mat4 m4d(f32 d)
{
    mat4 res = m4();
    res.elements[0][0] = d;
    res.elements[1][1] = d;
    res.elements[2][2] = d;
    res.elements[3][3] = d;
    return res;
}

INLINE mat4 transpose_mat4(mat4 m)
{
    mat4 res;
    for (u32 i = 0; i < 4;++i)
    {
        for (u32 j = 0; j< 4;++j)
        {
            res.elements[j][i] = m.elements[i][j];
        }
    }
    return res;
}
INLINE mat4 mul_mat4f(mat4 m, f32 s)
{
    mat4 res;
    for (u32 i = 0; i < 4; ++i)
    {
        for (u32 j = 0; j < 4; ++j)
        {
            res.elements[i][j] = (f32)m.elements[i][j] * s;
        }
    }
    return res;
}
INLINE vec4 mul_mat4v(mat4 mat, vec4 vec)
{
    vec4 res;

    i32 cols, rows;
    for(rows = 0; rows < 4; ++rows)
    {
        f32 s = 0;
        for(cols = 0; cols < 4; ++cols)
        {
            s += mat.elements[cols][rows] * vec.elements[cols];
        }

        res.elements[rows] = s;
    }

    return (res);
}

#ifdef TOOLS_SSE
INLINE __m128 linear_combine_sse(__m128 l, mat4 r)
{
    __m128 res;
    //some complex shit
    return (res);
}
#endif

INLINE mat4 div_mat4f(mat4 m, f32 s)
{
    mat4 res = {0};
    
    if (s != 0.0f)
    {
        for (u32 i = 0; i < 4; ++i)
        {
            for (u32 j = 0; j < 4; ++j)
            {
                res.elements[i][j] = m.elements[i][j] / s;
            }
        }
    }
    return res;
}

INLINE mat4 add_mat4(mat4 l, mat4 r)
{
    mat4 res;
    for (u32 i = 0; i < 4; ++i)
    {
        for (u32 j = 0; j < 4; ++j)
        {
            res.elements[i][j] = (f32)l.elements[i][j] + (f32)r.elements[i][j];
        }
    }
    return res;
}


INLINE mat4 sub_mat4(mat4 l, mat4 r)
{
    mat4 res;
    for (u32 i = 0; i < 4; ++i)
    {
        for (u32 j = 0; j < 4; ++j)
        {
            res.elements[i][j] = l.elements[i][j] - r.elements[i][j];
        }
    }
    return res;
}

//r is done first and then l
INLINE mat4 mul_mat4(mat4 l, mat4 r)
{
    mat4 res;
    for (u32 col = 0; col < 4; ++col)
    {
        for (u32 row = 0; row < 4; ++row)
        {
            f32 sum = 0;
            for (u32 current_index = 0; current_index < 4; ++current_index)
            {
                sum += (f32)l.elements[current_index][row] * (f32)r.elements[col][current_index];
            }
            res.elements[col][row] = sum;
        }
    }
    return res;
}


INLINE mat4 translate_mat4(vec3 t) //TODO(ilias): check handedness
{
    mat4 res = m4d(1.0f);
    res.elements[3][0] = t.x;
    res.elements[3][1] = t.y;
    res.elements[3][2] = t.z;
    return res;
}

INLINE mat4 rotate_mat4(float angle, vec3 axis)
{
    mat4 res = m4d(1.0f);

    axis = normalize_vec3(axis);

    float sinA = sin(to_radians(angle));
    float cosA = cos(to_radians(angle));
    float cos_val = 1.0f - cosA;

    res.elements[0][0] = (axis.x * axis.x * cos_val) + cosA;
    res.elements[0][1] = (axis.x * axis.y * cos_val) + (axis.z * sinA);
    res.elements[0][2] = (axis.x * axis.z * cos_val) - (axis.y * sinA);

    res.elements[1][0] = (axis.y * axis.x * cos_val) - (axis.z * sinA);
    res.elements[1][1] = (axis.y * axis.y * cos_val) + cosA;
    res.elements[1][2] = (axis.y * axis.z * cos_val) + (axis.x * sinA);

    res.elements[2][0] = (axis.z * axis.x * cos_val) + (axis.y * sinA);
    res.elements[2][1] = (axis.z * axis.y * cos_val) - (axis.x * sinA);
    res.elements[2][2] = (axis.z * axis.z * cos_val) + cosA;

    return (res);
}

INLINE mat4 scale_mat4(vec3 s)
{
    mat4 res = m4d(1.f);
    res.elements[0][0] *= s.x;
    res.elements[1][1] *= s.y;
    res.elements[2][2] *= s.z;
    return res;
}
//what the fuuuuuuuck is this shit??
INLINE mat4 inv_mat4(mat4 m)
{
    mat4 res;
    f32 det;
    mat4 inv, inv_out;
    i32 i;

    inv.raw[0] = m.raw[5]  * m.raw[10] * m.raw[15] - 
             m.raw[5]  * m.raw[11] * m.raw[14] - 
             m.raw[9]  * m.raw[6]  * m.raw[15] + 
             m.raw[9]  * m.raw[7]  * m.raw[14] +
             m.raw[13] * m.raw[6]  * m.raw[11] - 
             m.raw[13] * m.raw[7]  * m.raw[10];

    inv.raw[4] = -m.raw[4]  * m.raw[10] * m.raw[15] + 
              m.raw[4]  * m.raw[11] * m.raw[14] + 
              m.raw[8]  * m.raw[6]  * m.raw[15] - 
              m.raw[8]  * m.raw[7]  * m.raw[14] - 
              m.raw[12] * m.raw[6]  * m.raw[11] + 
              m.raw[12] * m.raw[7]  * m.raw[10];

    inv.raw[8] = m.raw[4]  * m.raw[9] * m.raw[15] - 
             m.raw[4]  * m.raw[11] * m.raw[13] - 
             m.raw[8]  * m.raw[5] * m.raw[15] + 
             m.raw[8]  * m.raw[7] * m.raw[13] + 
             m.raw[12] * m.raw[5] * m.raw[11] - 
             m.raw[12] * m.raw[7] * m.raw[9];

    inv.raw[12] = -m.raw[4]  * m.raw[9] * m.raw[14] + 
               m.raw[4]  * m.raw[10] * m.raw[13] +
               m.raw[8]  * m.raw[5] * m.raw[14] - 
               m.raw[8]  * m.raw[6] * m.raw[13] - 
               m.raw[12] * m.raw[5] * m.raw[10] + 
               m.raw[12] * m.raw[6] * m.raw[9];

    inv.raw[1] = -m.raw[1]  * m.raw[10] * m.raw[15] + 
              m.raw[1]  * m.raw[11] * m.raw[14] + 
              m.raw[9]  * m.raw[2] * m.raw[15] - 
              m.raw[9]  * m.raw[3] * m.raw[14] - 
              m.raw[13] * m.raw[2] * m.raw[11] + 
              m.raw[13] * m.raw[3] * m.raw[10];

    inv.raw[5] = m.raw[0]  * m.raw[10] * m.raw[15] - 
             m.raw[0]  * m.raw[11] * m.raw[14] - 
             m.raw[8]  * m.raw[2] * m.raw[15] + 
             m.raw[8]  * m.raw[3] * m.raw[14] + 
             m.raw[12] * m.raw[2] * m.raw[11] - 
             m.raw[12] * m.raw[3] * m.raw[10];

    inv.raw[9] = -m.raw[0]  * m.raw[9] * m.raw[15] + 
              m.raw[0]  * m.raw[11] * m.raw[13] + 
              m.raw[8]  * m.raw[1] * m.raw[15] - 
              m.raw[8]  * m.raw[3] * m.raw[13] - 
              m.raw[12] * m.raw[1] * m.raw[11] + 
              m.raw[12] * m.raw[3] * m.raw[9];

    inv.raw[13] = m.raw[0]  * m.raw[9] * m.raw[14] - 
              m.raw[0]  * m.raw[10] * m.raw[13] - 
              m.raw[8]  * m.raw[1] * m.raw[14] + 
              m.raw[8]  * m.raw[2] * m.raw[13] + 
              m.raw[12] * m.raw[1] * m.raw[10] - 
              m.raw[12] * m.raw[2] * m.raw[9];

    inv.raw[2] = m.raw[1]  * m.raw[6] * m.raw[15] - 
             m.raw[1]  * m.raw[7] * m.raw[14] - 
             m.raw[5]  * m.raw[2] * m.raw[15] + 
             m.raw[5]  * m.raw[3] * m.raw[14] + 
             m.raw[13] * m.raw[2] * m.raw[7] - 
             m.raw[13] * m.raw[3] * m.raw[6];

    inv.raw[6] = -m.raw[0]  * m.raw[6] * m.raw[15] + 
              m.raw[0]  * m.raw[7] * m.raw[14] + 
              m.raw[4]  * m.raw[2] * m.raw[15] - 
              m.raw[4]  * m.raw[3] * m.raw[14] - 
              m.raw[12] * m.raw[2] * m.raw[7] + 
              m.raw[12] * m.raw[3] * m.raw[6];

    inv.raw[10] = m.raw[0]  * m.raw[5] * m.raw[15] - 
              m.raw[0]  * m.raw[7] * m.raw[13] - 
              m.raw[4]  * m.raw[1] * m.raw[15] + 
              m.raw[4]  * m.raw[3] * m.raw[13] + 
              m.raw[12] * m.raw[1] * m.raw[7] - 
              m.raw[12] * m.raw[3] * m.raw[5];

    inv.raw[14] = -m.raw[0]  * m.raw[5] * m.raw[14] + 
               m.raw[0]  * m.raw[6] * m.raw[13] + 
               m.raw[4]  * m.raw[1] * m.raw[14] - 
               m.raw[4]  * m.raw[2] * m.raw[13] - 
               m.raw[12] * m.raw[1] * m.raw[6] + 
               m.raw[12] * m.raw[2] * m.raw[5];

    inv.raw[3] = -m.raw[1] * m.raw[6] * m.raw[11] + 
              m.raw[1] * m.raw[7] * m.raw[10] + 
              m.raw[5] * m.raw[2] * m.raw[11] - 
              m.raw[5] * m.raw[3] * m.raw[10] - 
              m.raw[9] * m.raw[2] * m.raw[7] + 
              m.raw[9] * m.raw[3] * m.raw[6];

    inv.raw[7] = m.raw[0] * m.raw[6] * m.raw[11] - 
             m.raw[0] * m.raw[7] * m.raw[10] - 
             m.raw[4] * m.raw[2] * m.raw[11] + 
             m.raw[4] * m.raw[3] * m.raw[10] + 
             m.raw[8] * m.raw[2] * m.raw[7] - 
             m.raw[8] * m.raw[3] * m.raw[6];

    inv.raw[11] = -m.raw[0] * m.raw[5] * m.raw[11] + 
               m.raw[0] * m.raw[7] * m.raw[9] + 
               m.raw[4] * m.raw[1] * m.raw[11] - 
               m.raw[4] * m.raw[3] * m.raw[9] - 
               m.raw[8] * m.raw[1] * m.raw[7] + 
               m.raw[8] * m.raw[3] * m.raw[5];

    inv.raw[15] = m.raw[0] * m.raw[5] * m.raw[10] - 
              m.raw[0] * m.raw[6] * m.raw[9] - 
              m.raw[4] * m.raw[1] * m.raw[10] + 
              m.raw[4] * m.raw[2] * m.raw[9] + 
              m.raw[8] * m.raw[1] * m.raw[6] - 
              m.raw[8] * m.raw[2] * m.raw[5];

    det = m.raw[0] * inv.raw[0] + m.raw[1] * inv.raw[4] + 
        m.raw[2] * inv.raw[8] + m.raw[3] * inv.raw[12];

    if (det == 0) //in case the matrix is non-invertible
        return m4d(0.f); 

    det = 1.f / det;

    for (i = 0; i < 16; ++i)
        inv_out.raw[i] = inv.raw[i] * det;

    return inv_out;
}

INLINE mat4 orthographic_proj(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f)
{
    mat4 res = m4();

    //the quotents are in reverse because we were supposed to do one more matrix multiplication to negate z..
    //its basically two steps in one..
    res.elements[0][0] = 2.0f / (r - l);
    res.elements[1][1] = 2.0f / (t - b);
    res.elements[2][2] = 2.0f / (n - f);
    res.elements[3][3] = 1.0f;

    res.elements[3][0] = (l + r) / (l - r);
    res.elements[3][1] = (b + t) / (b - t);
    res.elements[3][2] = (f + n) / (n - f);

    return res;
}

INLINE mat4 perspective_proj(f32 fov, f32 aspect, f32 n, f32 f)
{
    mat4 res = m4();

    f32 cot = 1.0f / tanf(fov * (PI / 360.0f));

    res.elements[0][0] = cot / aspect;
    res.elements[1][1] = cot;
    res.elements[2][3] = -1.0f;

    res.elements[2][2] = (n + f)/(n - f);

    res.elements[3][2] = (2.f * n * f) / (n - f);
    res.elements[3][3] = 0.0f;

    return res;
}

INLINE mat4 look_at(vec3 eye, vec3 center, vec3 fake_up)
{
    mat4 res = m4();

    vec3 f = normalize_vec3(sub_vec3(center, eye));
    vec3 r = normalize_vec3(cross_vec3(f, fake_up));
    vec3 up = cross_vec3(r, f);

    res.elements[0][0] = r.x;
    res.elements[0][1] = up.x;
    res.elements[0][2] = -f.x;
    res.elements[0][3] = 0.0f;

    res.elements[1][0] = r.y;
    res.elements[1][1] = up.y;
    res.elements[1][2] = -f.y;
    res.elements[1][3] = 0.0f;

    res.elements[2][0] = r.z;
    res.elements[2][1] = up.z;
    res.elements[2][2] = -f.z;
    res.elements[2][3] = 0.0f;

    res.elements[3][0] = -dot_vec3(r, eye);
    res.elements[3][1] = -dot_vec3(up, eye);
    res.elements[3][2] = dot_vec3(f, eye);
    res.elements[3][3] = 1.0f;

    return res;
}

INLINE mat4 
swap_cols_mat4(mat4 mat, u32 col1, u32 col2)
{
    vec4 second_column = v4(mat.elements[col2][0],mat.elements[col2][1], mat.elements[col2][2], mat.elements[col2][3]);
    for (u32 i = 0; i < 4; ++i)
        mat.elements[col2][i] = mat.elements[col1][i];
    for (u32 i = 0; i < 4; ++i)
        mat.elements[col1][i] = second_column.elements[i];
    return mat;
}

INLINE mat4
swap_rows_mat4(mat4 mat, u32 row1, u32 row2)
{
   vec4 second_row = v4(mat.elements[0][row2], mat.elements[1][row2], mat.elements[2][row2], mat.elements[3][row2]); 
   for (u32 i = 0; i < 4; ++i)
       mat.elements[i][row2] = mat.elements[i][row1];
   for (u32 i = 0; i < 4; ++i)
       mat.elements[i][row1] = second_row.elements[i];
   return mat;
}

INLINE mat4 negate_row_mat4(mat4 mat, u32 row)
{
    for (u32 i = 0; i < 4; ++i)
        mat.elements[i][row] = -1.f * mat.elements[i][row];
    return mat;
}

INLINE mat4 negate_col_mat4(mat4 mat, u32 col)
{
    for (u32 i = 0; i < 4; ++i)
        mat.elements[col][i] = -1.f * mat.elements[col][i];
    return mat;
}
//we must swap columns 2 and 3 then swap rows 2 and 3
//and then negate column 3 and row 3 TODO(ilias): check this shit
INLINE mat4
blender_to_opengl_mat4(mat4 mat)
{
   mat = swap_cols_mat4(mat, 2,3);
   mat = swap_rows_mat4(mat,2,3);
   mat = negate_col_mat4(mat, 3);
   mat = negate_row_mat4(mat, 3);
   return mat;
}
INLINE mat4
blender_to_opengl(mat4 mat)
{
   return mat;
}



INLINE mat4
maya_to_opengl(mat4 mat)
{
    return mat;
}


//some operator overloading
#ifdef __cplusplus
INLINE vec2 operator+(vec2 l, vec2 r)
{
    vec2 res = add_vec2(l, r);

    return res;
}
INLINE vec2 operator-(vec2 l, vec2 r)
{
    vec2 res = sub_vec2(l, r);

    return res;
}
INLINE vec2 operator*(vec2 l, vec2 r)
{
    vec2 res = mul_vec2(l, r);

    return res;
}
INLINE vec2 operator*(vec2 l, f32 x)
{
    vec2 res = mul_vec2f(l, x);

    return res;
}

INLINE vec2 operator/(vec2 l, vec2 r)
{
    vec2 res = div_vec2(l, r);

    return res;
}

INLINE vec2 operator/(vec2 l,f32 r)
{
    vec2 res = div_vec2f(l, r);

    return res;
}

INLINE vec3 operator+(vec3 l, vec3 r)
{
    vec3 res = add_vec3(l, r);

    return res;
}
INLINE vec3 operator-(vec3 l, vec3 r)
{
    vec3 res = sub_vec3(l, r);

    return res;
}
INLINE vec3 operator*(vec3 l, vec3 r)
{
    vec3 res = mul_vec3(l, r);

    return res;
}
INLINE vec3 operator*(vec3 l, f32 r)
{
    vec3 res = mul_vec3f(l, r);

    return res;
}
INLINE vec3 operator/(vec3 l, vec3 r)
{
    vec3 res = div_vec3(l, r);

    return res;
}
INLINE vec3 operator/(vec3 l,f32 r)
{
    vec3 res = div_vec3f(l, r);

    return res;
}

INLINE vec4 operator+(vec4 l, vec4 r)
{
    vec4 res = add_vec4(l, r);

    return res;
}
INLINE vec4 operator-(vec4 l, vec4 r)
{
    vec4 res = sub_vec4(l, r);

    return res;
}
INLINE vec4 operator*(vec4 l, vec4 r)
{
    vec4 res = mul_vec4(l, r);

    return res;
}
INLINE vec4 operator*(vec4 l, f32 r)
{
    vec4 res = mul_vec4f(l, r);

    return res;
}
INLINE vec4 operator/(vec4 l, vec4 r)
{
    vec4 res = div_vec4(l, r);

    return res;
}
INLINE vec4 operator/(vec4 l,f32 r)
{
    vec4 res = div_vec4f(l, r);

    return res;
}


INLINE mat4 operator+(mat4 l, mat4 r)
{
    mat4 res = add_mat4(l,r);

    return res;
}
INLINE mat4 operator-(mat4 l, mat4 r)
{
    mat4 res = sub_mat4(l,r);

    return res;
}

INLINE mat4 operator*(mat4 l, mat4 r)
{
    mat4 res = mul_mat4(l,r);

    return res;
}

INLINE mat4 operator*(mat4 l,f32 r)
{
    mat4 res = mul_mat4f(l,r);

    return res;
}

#endif

//QUATERNION LIB 
typedef union Quaternion
{
    struct
    {
        union
        {
            vec3 xyz;
            struct
            {
                f32 x,y,z;
            };
        };
        f32 w;
    };
    f32 elements[4];

}Quaternion;

INLINE Quaternion quat(f32 x, f32 y, f32 z, f32 w)
{
    Quaternion res;

    res.x = x;
    res.y = y;
    res.z = z;
    res.w = w;
    //res = {x,y,z,w};

    return res;
}

INLINE Quaternion quat_vec4(vec4 vec)
{
    Quaternion res;

    res.x = vec.x;
    res.y = vec.y;
    res.z = vec.z;
    res.w = vec.w;

    return res;
}

INLINE Quaternion add_quat(Quaternion l, Quaternion r)
{
    Quaternion res;

    res.x = l.x + r.x;
    res.y = l.y + r.y;
    res.z = l.z + r.z;
    res.w = l.w + r.w;

    return res;
}

INLINE Quaternion sub_quat(Quaternion l, Quaternion r)
{
    Quaternion res;

    res.x = l.x - r.x;
    res.y = l.y - r.y;
    res.z = l.z - r.z;
    res.w = l.w - r.w;

    return res;
}

//TODO(ilias): check the scalars..
INLINE Quaternion mul_quat(Quaternion l, Quaternion r)
{
    Quaternion res;

    res.w = (l.w * r.w) - (l.x * r.x) - (l.y * r.y) - (l.z * r.z);
    res.x = (l.w * r.w) + (l.x * r.w) + (l.y * r.z) - (l.z * r.y);
    res.y = (l.w * r.y) - (l.x * r.z) + (l.y * r.w) + (l.z * r.x);
    res.z = (l.w * r.z) - (l.x * r.y) - (l.y * r.x) + (l.z * r.w);
    
    return res;
}


INLINE Quaternion mul_quatf(Quaternion l, f32 val)
{
    Quaternion res;

    res.x = l.x * val;
    res.y = l.y * val;
    res.z = l.z * val;
    res.w = l.w * val;

    return res;
}

INLINE Quaternion div_quatf(Quaternion l, f32 val)
{
    assert(val);
    Quaternion res;
    
    res.x = l.x / val;
    res.y = l.y / val;
    res.z = l.z / val;
    res.w = l.w / val;

    return res;
}

INLINE f32 dot_quat(Quaternion l, Quaternion r)
{
   f32 res;

   res = (l.x * r.x) + (l.y * r.y) + (l.z * r.z) + (l.w * r.w);

   return res;
}

INLINE b32 equals_quat(Quaternion l, Quaternion r)
{
    f32 dot = dot_quat(l,r);
    return 1 ? 0 : fabs(dot - 1.f) < 0.001f;
}


INLINE Quaternion inv_quat(Quaternion l)
{
    Quaternion res;

    f32 len = sqrt(dot_quat(l,l));
    res = div_quatf(l, len);

    return res;
}

INLINE Quaternion slerp(Quaternion l, Quaternion r, f32 time)
{
    Quaternion res;

    //some complex shit

    return res;
}

INLINE Quaternion quat_from_angle(vec3 axis, f32 angle)
{
    Quaternion res;

    vec3 axis_normalized = normalize_vec3(axis);
    //this because quaternions are (i)q(i^-1) so angles are double
    f32 sintheta = sin(angle / 2.f); 

    res.xyz = mul_vec3f(axis_normalized, sintheta);
    res.w = cos(angle / 2.f);
    
    return res;
}

INLINE Quaternion normalize_quat(Quaternion l)
{
    Quaternion res;

    f32 len = sqrtf(dot_quat(l,l)) ;
    res = div_quatf(l,len);

    return res;
}

INLINE Quaternion nlerp(Quaternion l, Quaternion r, f32 time)
{
    Quaternion res;

    //we gotta interpolate all quaternion components
    res.x = lerp(l.x, r.x, time);
    res.y = lerp(l.y, r.y, time);
    res.z = lerp(l.z, r.z, time);
    res.w = lerp(l.w, r.w, time);

    res = normalize_quat(res);
    
    return res;
}

//taken from HMMATH.. investigate further..
INLINE mat4 quat_to_mat4(Quaternion l)
{
    mat4 res;

    Quaternion norm_quat = normalize_quat(l);

    f32 XX, YY, ZZ, XY, XZ, YZ, WX, WY, WZ;

    XX = norm_quat.x * norm_quat.x;
    YY = norm_quat.y * norm_quat.y;
    ZZ = norm_quat.z * norm_quat.z;
    XY = norm_quat.x * norm_quat.y;
    XZ = norm_quat.x * norm_quat.z;
    YZ = norm_quat.y * norm_quat.z;
    WX = norm_quat.w * norm_quat.x;
    WY = norm_quat.w * norm_quat.y;
    WZ = norm_quat.w * norm_quat.z;

    res.elements[0][0] = 1.0f - 2.0f * (YY + ZZ);
    res.elements[0][1] = 2.0f * (XY + WZ);
    res.elements[0][2] = 2.0f * (XZ - WY);
    res.elements[0][3] = 0.0f;

    res.elements[1][0] = 2.0f * (XY - WZ);
    res.elements[1][1] = 1.0f - 2.0f * (XX + ZZ);
    res.elements[1][2] = 2.0f * (YZ + WX);
    res.elements[1][3] = 0.0f;

    res.elements[2][0] = 2.0f * (XZ + WY);
    res.elements[2][1] = 2.0f * (YZ - WX);
    res.elements[2][2] = 1.0f - 2.0f * (XX + YY);
    res.elements[2][3] = 0.0f;

    res.elements[3][0] = 0.0f;
    res.elements[3][1] = 0.0f;
    res.elements[3][2] = 0.0f;
    res.elements[3][3] = 1.0f;

    return res;
}

//taken directly from HandmadeMath.. investigate its authenticity 
INLINE Quaternion 
mat4_to_quat(mat4 m)
{
    float T;
    Quaternion Q;

    if (m.elements[2][2] < 0.0f) {
        if (m.elements[0][0] > m.elements[1][1]) {
            T = 1 + m.elements[0][0] - m.elements[1][1] - m.elements[2][2];
            Q = quat(
                T,
                m.elements[0][1] + m.elements[1][0],
                m.elements[2][0] + m.elements[0][2],
                m.elements[1][2] - m.elements[2][1]
            );
        } else {
            T = 1 - m.elements[0][0] + m.elements[1][1] - m.elements[2][2];
            Q = quat(
                m.elements[0][1] + m.elements[1][0],
                T,
                m.elements[1][2] + m.elements[2][1],
                m.elements[2][0] - m.elements[0][2]
            );
        }
    } else {
        if (m.elements[0][0] < -m.elements[1][1]) {

            T = 1 - m.elements[0][0] - m.elements[1][1] + m.elements[2][2];
            Q = quat(
                m.elements[2][0] + m.elements[0][2],
                m.elements[1][2] + m.elements[2][1],
                T,
                m.elements[0][1] - m.elements[1][0]
            );
        } else {
            T = 1 + m.elements[0][0] + m.elements[1][1] + m.elements[2][2];
            Q = quat(
                m.elements[1][2] - m.elements[2][1],
                m.elements[2][0] - m.elements[0][2],
                m.elements[0][1] - m.elements[1][0],
                T
            );
        }
    }

    Q = mul_quatf(Q, 0.5f / sqrt(T));

    return Q;
}


//TGA LIB 
enum {
    TGA_ERROR_FILE_OPEN = 1,
    TGA_ERROR_READING_FILE,
    TGA_ERROR_INDEXED_COLOR, 
    TGA_ERROR_MEMORY,
    TGA_ERROR_COMPRESSED_FILE, 
    TGA_OK
};

typedef struct TGAInfo
{
    i32 status;
    u8 type, pixel_depth;
    i16 width, height;
    u8 *image_data;
}TGAInfo;

static TGAInfo* tga_init_image_RGB(i16 width, i16 height)
{
    TGAInfo* info;
    info = (TGAInfo*)ALLOC(sizeof(TGAInfo));
    info->width = width;
    info->height = height;
    info->pixel_depth = 24;
    info->type = 2; 
    info->status = TGA_OK;
    info->image_data = (u8*)ALLOC(sizeof(u8) * width * height * (info->pixel_depth / 8));
    return info;
}

static void 
tga_load_header(FILE *file, TGAInfo *info) {

	u8 c_garbage;
	i16 i_garbage;

	fread(&c_garbage, sizeof(u8), 1, file);
	fread(&c_garbage, sizeof(u8), 1, file);

    // type must be 2 or 3
	fread(&info->type, sizeof(u8), 1, file);

	fread(&i_garbage, sizeof(i16), 1, file);
	fread(&i_garbage, sizeof(i16), 1, file);
	fread(&c_garbage, sizeof(u8), 1, file);
	fread(&i_garbage, sizeof(i16), 1, file);
	fread(&i_garbage, sizeof(i16), 1, file);

	fread(&info->width, sizeof(i16), 1, file);
	fread(&info->height, sizeof(i16), 1, file);
	fread(&info->pixel_depth, sizeof(u8), 1, file);

	fread(&c_garbage, sizeof(u8), 1, file);
}
static void tga_load_image_data(FILE *file, TGAInfo *info) {

	i32 mode,total,i;
	u8 aux;

    // mode equal the number of components for each pixel
	mode = info->pixel_depth / 8;
    // total is the number of bytes we'll have to read
	total = info->height * info->width * mode;
	
	fread(info->image_data,sizeof(u8),total,file);

    // mode=3 or 4 implies that the image is RGB(A). However TGA
    // stores it as BGR(A) so we'll have to swap R and B.
	if (mode >= 3)
		for (i=0; i < total; i+= mode) {
			aux = info->image_data[i];
			info->image_data[i] = info->image_data[i+2];
			info->image_data[i+2] = aux;
		}
}

static TGAInfo* 
tga_load(const char * filename)
{
    TGAInfo *info;
    FILE* file;
    i32 mode,total;

    //allocate memory for TGAInfo
    info = (TGAInfo*)ALLOC(sizeof(TGAInfo));
    if(info == NULL)return(NULL);

    //open file for binary reading
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        info->status = TGA_ERROR_FILE_OPEN;
        //fclose(file);
        return info;
    }

    //we load the header and fill out neccesary info
    tga_load_header(file, info);

    //check if color indexed
    if (info->type == 1)
    {
        info->status = TGA_ERROR_INDEXED_COLOR;
        fclose(file);
        return info;
    }

    //check if compressed
    if ((info->type != 2) && (info->type != 3))
    {
       info->status = TGA_ERROR_COMPRESSED_FILE; 
    }

    mode = info->pixel_depth / 8;
    total =info->height * info->width * mode;
    info->image_data = (u8*)ALLOC(total * sizeof(u8));

    //check if memory is ok
    if (info->image_data == NULL)
    {
        info->status = TGA_ERROR_MEMORY;
        fclose(file);
        return info;
    }

    //load the fucking image
    tga_load_image_data(file, info);
    if (ferror(file))
    {
        info->status = TGA_ERROR_READING_FILE;
        fclose(file);
        info->status = TGA_OK;
        return info;
    }
    fclose(file);
    info->status = TGA_OK;

    return info;
}


static i16 
tga_save(const char *filename, i16 width, i16 height, u8 pixel_depth, u8 *image_data)
{
    u8 c_garbage = 0, type,mode, aux;
    i16 i_garbage;
    i32 i;
    FILE* file;
    file = fopen(filename, "wb");
    if (file == NULL)
        return TGA_ERROR_FILE_OPEN;
    mode = pixel_depth / 8;
    if ((pixel_depth == 24) || (pixel_depth == 32))
        type = 2;
    else
        type = 3;

    // write the header
	fwrite(&c_garbage, sizeof(u8), 1, file);
	fwrite(&c_garbage, sizeof(u8), 1, file);

	fwrite(&type, sizeof(u8), 1, file);

	fwrite(&i_garbage, sizeof(i16), 1, file);
	fwrite(&i_garbage, sizeof(i16), 1, file);
	fwrite(&c_garbage, sizeof(u8), 1, file);
	fwrite(&i_garbage, sizeof(i16), 1, file);
	fwrite(&i_garbage, sizeof(i16), 1, file);

	fwrite(&width, sizeof(i16), 1, file);
	fwrite(&height, sizeof(i16), 1, file);
	fwrite(&pixel_depth, sizeof(u8), 1, file);

	fwrite(&c_garbage, sizeof(u8), 1, file);


    // convert the image data from RGB(a) to BGR(A)
	if (mode >= 3)
        for (i=0; i < width * height * mode ; i+= mode) {
            aux = image_data[i];
            image_data[i] = image_data[i+2];
            image_data[i+2] = aux;
        }

    // save the image data
	fwrite(image_data, sizeof(u8), width * height * mode, file);
	fclose(file);
	//free(image_data);
    //image_data = NULL;

	return TGA_OK;
}

static void
tga_destroy(TGAInfo * info)
{
    if (info != NULL)
    {
        if (info->image_data != NULL)
            free(info->image_data);
        free(info);
    }
    
}

//PPM LIB

typedef struct PPMInfo
{
    i32 status;
    i32 width, height;
    i32 max_color;
    char type[3];
    f32 *image_data;
}PPMInfo;

enum {
    PPM_ERROR_FILE_OPEN = 1,
    PPM_ERROR_READING_FILE,
    PPM_ERROR_INDEXED_COLOR, 
    PPM_ERROR_MEMORY,
    PPM_ERROR_UNSUPPORTED_FILE, 
    PPM_OK
};


static color3
ppm_get_pixel(i32 i, i32 j, PPMInfo* info)
{
    assert(i + j * info->height < info->width * info->height);
    f32 x = info->image_data[(i + (info->height -1- j) * info->height)*3]; 
    f32 y = info->image_data[(i + (info->height -1- j)* info->height)*3 + 1]; 
    f32 z = info->image_data[(i + (info->height -1- j)* info->height)*3 + 2]; 

    vec3 ret = {x,y,z};
    return ret;
}

static color3
ppm_set_pixel(i32 i, i32 j, PPMInfo* info, color3 col)
{
    assert(i + j * info->height < info->width * info->height);
    info->image_data[(i + (info->height -1- j) * info->height)*3] = col.x; 
    info->image_data[(i + (info->height -1- j)* info->height)*3 + 1] = col.y; 
    info->image_data[(i + (info->height -1- j)* info->height)*3 + 2] = col.z; 
    return col;
}

static PPMInfo*
ppm_init(i32 width, i32 height)
{
    PPMInfo* info;
    info = (PPMInfo*)ALLOC(sizeof(PPMInfo));
    if (info == NULL)return NULL;
    info->type[0] = 'P';
    info->type[1] = '3';
    info->type[2] = 0;
    info->width = width;
    info->height = height;
    info->max_color = 255;
    info->image_data = (f32*)ALLOC(sizeof(f32) * info->width * info->height * 3); 
    if (info->image_data == NULL)return NULL;
    i32 i;
    for (i = 0; i < info->width * info->height*3; ++i)
    {
        info->image_data[i] = 0; 
    }
#if 0
    for (i = 0; i < info->width * info->height; ++i)
    {
        i32 x = i % info->width;
        i32 y = i / info->height;
        if (x == y)
            ppm_set_pixel(x,y,info, {1,1,1});
    }
#endif
 
    return info;
}
static void 
ppm_load_header(FILE* file, PPMInfo* info)
{
	i32 i_garbage;
    char buff;

    info->type[0] = fgetc(file);
    info->type[1] = fgetc(file);
    info->type[2] = 0;

    fscanf (file, "%d", &i_garbage);
    info->width = i_garbage;
    fscanf (file, "%d", &i_garbage);
    info->height = i_garbage;
    fscanf (file, "%d", &i_garbage);
    info->max_color = i_garbage;


}
static void ppm_load_P3_data(FILE* file, PPMInfo* info)
{
    i32 garbage[3];
    u32 iter = 0;
    int i;
    for (i = 0; i < info->width * info->height; ++i)
    {
        fscanf (file, "%d %d %d", &garbage[0], &garbage[1], &garbage[2]);
        info->image_data[iter++] = ((f32)garbage[0]) / (f32)info->max_color;
        info->image_data[iter++] = ((f32)garbage[1]) / (f32)info->max_color;
        info->image_data[iter++] = ((f32)garbage[2]) / (f32)info->max_color;
    }
}
static PPMInfo*
ppm_read(const char *filename)
{
    PPMInfo *info;
    FILE* file;

    //allocate memory for PPMInfo
    info = (PPMInfo*)ALLOC(sizeof(PPMInfo));
    if(info == NULL)return(NULL);

    file = fopen(filename, "r");
    if (file == NULL)
    {
        info->status = PPM_ERROR_FILE_OPEN;
        //fclose(file);
        return info;
    }

    //we load the header and fill out neccesary info
    ppm_load_header(file, info);

    info->image_data = (f32*)ALLOC(sizeof(f32) * info->width * info->height * 3);
    if (info->image_data == NULL)
    {
        info->status = PPM_ERROR_MEMORY;
        fclose(file);
        return info;
    }

    if (strcmp(info->type, "P3") == 0)ppm_load_P3_data(file, info);
    else
    {
        info->status = PPM_ERROR_UNSUPPORTED_FILE;
        fclose(file);
        return info;
    }

    if (ferror(file))
    {
        info->status = PPM_ERROR_READING_FILE;
        fclose(file);
        //info->status = TGA_OK;
        return info;
    }
    fclose(file);
    info->status = PPM_OK;

    //flip_image_horizontally(info->width, info->height, info->image_data);
    return info;
}

static i32
ppm_write(PPMInfo* info, const char *filename)
{
    //flip_image_horizontally(info->width, info->height, info->image_data);
    FILE* file;
    file = fopen(filename, "w");
    if (file == NULL)
    {
        fclose(file);
        return PPM_ERROR_FILE_OPEN;
    }
    if (strcmp(info->type, "P3") != 0)
    {
       fclose(file);
       return PPM_ERROR_UNSUPPORTED_FILE;
    }
    fputc(info->type[0], file);
    fputc(info->type[1], file);
    fputc('\n', file);
    fprintf(file, "%d\n", info->width);
    fprintf(file, "%d\n", info->height);
    fprintf(file, "%d\n", info->max_color);

    int i;
    for (i = 0; i < info->width * info->height; ++i)
    {
        i32 cmp[3];
        cmp[0]= (i32)(info->image_data[i*3] * info->max_color);
        cmp[1]= (i32)(info->image_data[i*3+1] * info->max_color);
        cmp[2]= (i32)(info->image_data[i*3+2] * info->max_color);
        fprintf(file, "%d ", cmp[0]);
        fprintf(file, "%d ", cmp[1]);
        fprintf(file, "%d", cmp[2]);
        fputc('\n', file);
    }
    return PPM_OK;
}

static i32
ppm_write01(PPMInfo* info, const char *filename)
{
    //flip_image_horizontally(info->width, info->height, info->image_data);
    FILE* file;
    file = fopen(filename, "w");
    if (file == NULL)
    {
        fclose(file);
        return PPM_ERROR_FILE_OPEN;
    }
    if (strcmp(info->type, "P3") != 0)
    {
       fclose(file);
       return PPM_ERROR_UNSUPPORTED_FILE;
    }
    fputc(info->type[0], file);
    fputc(info->type[1], file);
    fputc('\n', file);
    fprintf(file, "%d\n", info->width);
    fprintf(file, "%d\n", info->height);
    fprintf(file, "%d\n", info->max_color);
    int j;
    int i;
    for (j = info->height-1; j >=0; --j)
    {
        for (i = 0; i < info->width; ++i)
        {
            int index = info->width * 3 *j + 3 * i;
            i32 cmp[3];
            cmp[0]= (i32)(info->image_data[index] * info->max_color);
            cmp[1]= (i32)(info->image_data[index+1] * info->max_color);
            cmp[2]= (i32)(info->image_data[index+2] * info->max_color);
            fprintf(file, "%d ", cmp[0]);
            fprintf(file, "%d ", cmp[1]);
            fprintf(file, "%d", cmp[2]);
            fputc('\n', file);

        }
    }

    return PPM_OK;
}

typedef struct DATA{i32 i;i32 j;f32 *data;}DATA, *PMYDATA;

//TODO: put this whole thing on a thread of its own
static i32 ppm_save_pixels(i32 width, i32 height, f32* pixels)
{
    PPMInfo *info = ppm_init(width,height);
    i32 i;
    memcpy(info->image_data, pixels , sizeof(f32) * info->width * info->height * 3);
    ppm_write01(info, "score.ppm");
    return 1;
}
static i32 ppm_save_pixels2(LPVOID lpParam)
{
  PMYDATA real_data = (PMYDATA)(lpParam);
  i32 width = real_data->i;
  i32 height= real_data->j;
  f32 *pixels = real_data->data;
    PPMInfo *info = ppm_init(width,height);
    i32 i;
    memcpy(info->image_data, pixels , sizeof(f32) * info->width * info->height * 3);
    //ppm_write01(info, "score.ppm");
    return 1;
}




/*  example usage of ppm_save_current_framebuffer
 if (global_platform.key_pressed[KEY_K])
    {
        f32 *pixels = (f32*)ALLOC(sizeof(f32) * 3 * global_platform.window_width* global_platform.window_height); 
        glReadPixels(0, 0, global_platform.window_width,global_platform.window_height,GL_RGB, GL_FLOAT, pixels);
        ppm_save_pixels( global_platform.window_width, global_platform.window_height, pixels);
        free(pixels);
    }
*/

//TODO: make a ppm_write_FBO to write color attachments of a framebuffer as PPM images.. 

//MEMORY STUFF

typedef struct Arena
{
    void *memory;
    u32 memory_size;
    u32 current_offset;
} Arena;


static Arena 
arena_init(void* memory, u32 size)
{
    Arena a = {0};
    a.memory = memory;
    a.memory_size = size;
    a.current_offset = 0;

    return a;
}

static void
arena_free(Arena* arena, u32 size)
{
    //do nothing
}

static void * 
arena_alloc(Arena* arena, u32 size)
{
    void* mem = 0;

    if (arena->current_offset + size <= arena->memory_size)
    {
        //position of next available byte
        mem = (void *)((u8*)arena->memory + arena->current_offset); 
        arena->current_offset += size;
    }
    //we return a pointer to size free bytes of memory
    return mem;
}

static void
arena_clear(Arena* arena)
{
    arena->current_offset = 0;
}

static void 
arena_zero(Arena* arena)
{
    memset(arena->memory, 0, arena->memory_size);
}

//STRING STUFF

typedef struct String
{
    char* data;
    u32 len;
    u32 size;
    u32 max_size;
    b32 is_constant;
}String;

static String
init_string_in_arena(Arena* arena, u32 size)
{
    String str = {0};

    str.data = (char*)arena_alloc(arena, size);
    if (str.data)
    {
        str.len = size - 1;
        str.size = size;
        str.max_size = size;
        str.is_constant = 0;

        str.data[size-1] = '\0';
    }
    return str;
}

static String str(Arena* arena, char* characters)
{

    String s = init_string_in_arena(arena, str_size(characters) + 1);
    memcpy(s.data, characters, str_size(characters) + 1);
    return s; 
}

static String substr(Arena* arena, char* characters, i32 start, i32 finish)
{
    assert(finish - start < str_size(characters));
    assert(start < str_size(characters));
    String s = init_string_in_arena(arena, finish - start + 1);
    memcpy(s.data , characters + start, finish - start);
    return s; 
}



//A stretchy buffer implementation [C99]

//HOW TO USE: declare your array of prefered type as TYPE *arr = NULL;
//then every time you need to insert something, buf_push(arr, ELEMENT);
//if you want to access a certain element you arr[i];
//that's all, also you can buf_free(arr);

#ifdef __cplusplus
extern "C" {
#endif
typedef struct BufHdr
{
    u32 len;
    u32 cap;
    char buf[0];
}BufHdr;



#define buf__hdr(b) ((BufHdr*)((char*)b - offsetof(BufHdr, buf)))

#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_end(b) ((b) + buf_len(b))

#define buf_fit(b, n) ((n) <= buf_cap(b) ? 0 : (*((void**)&(b)) = buf__grow((b), (n), sizeof(*(b)))))
#define buf_push(b, ...) (buf_fit((b), 1 + buf_len(b)), (b)[buf__hdr(b)->len++] = (__VA_ARGS__))
#define buf_free(b) ((b) ? (free(buf__hdr(b)), (b) = NULL) : 0)

static void *buf__grow(const void *buf, u32 new_len, u32 element_size)
{
   u32 new_cap = max(16, max(1 + 2*buf_cap(buf), new_len));
   assert(new_len <= new_cap);
   u32 new_size = offsetof(BufHdr, buf) + new_cap * element_size;
   BufHdr *new_hdr; 
   if(buf) 
   { 
       new_hdr = (BufHdr*)REALLOC(buf__hdr(buf), new_size); 
   }
   else
   { 
       new_hdr = (BufHdr*)ALLOC(new_size);
       new_hdr->len = 0;
   }
   new_hdr->cap = new_cap;
   return new_hdr->buf;// + offsetof(BufHdr, buf);
}
/* example usage of stretchy buffer
{
        int *arr = NULL;
        buf_push(arr, 1);
        buf_push(arr, 2);
        buf_push(arr, 3);
        buf_push(arr, 4);
        buf_push(arr, 5);
        buf_push(arr, 6);

        for (int i = 0; i < 6; ++i)
        {
            int x = arr[i];
            assert(x == i+1);
        }

        buf_free(arr);
}
*/
typedef struct Vertex
{
   vec3 position; 
   vec3 normal;
   vec2 tex_coord;
}Vertex;

static Vertex vert(vec3 p, vec3 n, vec2 t)
{
    Vertex res;
    res.position = p;
    res.normal = n;
    res.tex_coord = t;
    return res;
}















#ifdef __cplusplus
}
#endif


#endif










