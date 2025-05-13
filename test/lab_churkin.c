#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <x86intrin.h>

#define LIBDIVIDE_SSE2
#include "libdivide.h"

#define __PASTE2(a,b) a##b
#define __PASTE3(a,b,c) a##b##c
#define PASTE(N, ...)  __PASTE##N(__VA_ARGS__)

#define __UP_T_u8 u16
#define __UP_T_u16 u32
#define __UP_T_u32 u64
#define UP_T(T) __UP_T_##T

#define __GET_T_u8 uint8_t
#define __GET_T_u16 uint16_t
#define __GET_T_u32 uint32_t
#define __GET_T_u64 uint64_t
#define GET_T(T) PASTE(2, __GET_T_, T)

static char* next_row = NULL;

#define CALC_DET_INIT(FUNC_NAME,\
                      T,\
                      FIND_PIVOT,\
                      ADD,\
                      POW_MOD)\
static inline GET_T(T) FUNC_NAME(GET_T(T)** m, int n, GET_T(T) mod) {\
    int s = 0;\
    GET_T(T) det = 1;\
    /* генерируем ускоритель деления */\
    struct PASTE(3, libdivide_, UP_T(T), _t) fast_d = PASTE(3, libdivide_, UP_T(T), _gen)(mod);\
    for (int i = 0; i < n; i++) {\
        /* находим строку с ненулевым элементом в i-том столбце */\
        int prow = FIND_PIVOT((const GET_T(T)* const*)m, i, i, n);\
        if (prow == -1)\
            return 0; /* вырожденная */\
        /* при необходимости меняем строки местами */\
        if (prow != i) {\
            GET_T(T)* tmp = m[i];\
            m[i] = m[prow];\
            m[prow] = tmp;\
            s = !s;\
        }\
        /* множим det на диагональ */\
        GET_T(T) piv = m[i][i];\
        GET_T(UP_T(T)) prod_det_piv = (GET_T(UP_T(T)))det * piv;\
        det = (GET_T(T))(prod_det_piv - PASTE(3, libdivide_, UP_T(T), _do)(prod_det_piv, &fast_d) * mod);\
        /* вычисляем обратный элемент по модулю */\
        GET_T(T) inv = POW_MOD(piv, mod - 2, mod);\
        /* вычитаем i-ю строку из всех нижележащих */\
        /*_Pragma("omp parallel for") оставленно до лучших времён */\
        for (int k = i + 1; k < n; k++) {\
            GET_T(T) a = m[k][i];\
            GET_T(UP_T(T)) prod_a_inv = (GET_T(UP_T(T)))a * inv;\
            GET_T(T) t = (GET_T(T))(prod_a_inv - PASTE(3, libdivide_, UP_T(T), _do)(prod_a_inv, &fast_d) * mod);\
            GET_T(T) c = mod - t;\
            next_row = k + 1 < n ? (char*)m[k + 1] : NULL;\
            ADD(m[k], m[i], c, n, mod, &fast_d);\
        }\
    }\
    /* если было нечетное число перестановок, то det = -det mod */\
    return (s && det != 0) ? (mod - det) : det;\
}

#define POW_MOD_INIT(NAME, T)\
static inline GET_T(T) NAME(GET_T(T) a, GET_T(T) p, GET_T(T) mod) {\
    GET_T(T) res = 1;\
    while (p > 0) {\
        if (p & 1)\
            res = (GET_T(T))((GET_T(UP_T(T)))res * a % mod);\
        a = (GET_T(T))((GET_T(UP_T(T)))a * a % mod);\
        p >>= 1;\
    }\
    return res;\
}

#define FIND_PIVOT_INIT(NAME, T)\
static inline int NAME(const GET_T(T)* const* m, int i0, int j0, int n){ \
    for (int i = i0; i < n; i++)\
        if (m[i][j0] != 0)\
            return i;\
    return -1;\
}

// u32
static inline void add_u32_safe(uint32_t* row_i, const uint32_t* row_j, uint32_t c, int n, uint32_t mod, const struct libdivide_u64_t* fast_d_u64) { // почему-то моя SSE реализация была значительнее медленее
    uint32_t* restrict a = row_i;
    const uint32_t* restrict b = row_j;

    for (int i = 0; i < n; ++i) {
        if (next_row != NULL)
            _mm_prefetch((const char*)(next_row + i), _MM_HINT_T0);
        uint64_t sum = (uint64_t)a[i] + (uint64_t)b[i] * c;
        uint64_t q = libdivide_u64_do(sum, fast_d_u64);
        a[i] = (uint32_t)(sum - q * mod);
    }
}

FIND_PIVOT_INIT(find_pivot_u32, u32)
POW_MOD_INIT(pow_mod_u32, u32)
CALC_DET_INIT(calc_det_u32, u32, find_pivot_u32, add_u32_safe, pow_mod_u32)

// u16
static inline void add_sse4_u16_safe(uint16_t* row_i, const uint16_t* row_j, uint16_t c, int n, uint16_t mod, const struct libdivide_u32_t* fast_d_u32) {
    uint16_t* restrict a = row_i;
    const uint16_t* restrict b = row_j;
    __m128i zero = _mm_setzero_si128();
    __m128i c_vec = _mm_set1_epi32(c);
    __m128i mod_vec = _mm_set1_epi32(mod);

    int i = 0;
    // Основной цикл: по 8 элементов за итерацию
    for (; i + 7 < n; i += 8) {
        if (next_row != NULL)
            _mm_prefetch((const char*)(next_row + i), _MM_HINT_T0);

        // выровненная загрузка 8×16 бит
        __m128i vi = _mm_load_si128((__m128i*)(a + i));
        __m128i vj = _mm_load_si128((__m128i*)(b + i));

        // распаковка в 32-битные слова
        __m128i i_lo = _mm_unpacklo_epi16(vi, zero);
        __m128i i_hi = _mm_unpackhi_epi16(vi, zero);
        __m128i j_lo = _mm_unpacklo_epi16(vj, zero);
        __m128i j_hi = _mm_unpackhi_epi16(vj, zero);

        // вычисления
        __m128i sum_lo = _mm_add_epi32(i_lo, _mm_mullo_epi32(j_lo, c_vec));
        __m128i sum_hi = _mm_add_epi32(i_hi, _mm_mullo_epi32(j_hi, c_vec));
        __m128i q_lo   = libdivide_u32_do_vec128(sum_lo, fast_d_u32);
        __m128i q_hi   = libdivide_u32_do_vec128(sum_hi, fast_d_u32);
        __m128i rem_lo = _mm_sub_epi32(sum_lo, _mm_mullo_epi32(q_lo, mod_vec));
        __m128i rem_hi = _mm_sub_epi32(sum_hi, _mm_mullo_epi32(q_hi, mod_vec));

        // упаковка и сохранение
        __m128i out = _mm_packus_epi32(rem_lo, rem_hi);
        _mm_store_si128((__m128i*)(a + i), out);
    }

    // Обработка оставшихся элементов
    for (; i < n; ++i) {
        uint32_t sum = (uint32_t)a[i] + (uint32_t)b[i] * c;
        uint32_t q = libdivide_u32_do(sum, fast_d_u32);
        a[i] = (uint16_t)(sum - q * mod);
    }
}

FIND_PIVOT_INIT(find_pivot_u16, u16)
POW_MOD_INIT(pow_mod_u16, u16)
CALC_DET_INIT(calc_det_u16, u16, find_pivot_u16, add_sse4_u16_safe, pow_mod_u16)

// u8
static inline void add_sse4_u8_safe(uint8_t* row_i, const uint8_t* row_j, uint8_t c, int n, uint8_t mod, const struct libdivide_u16_t* fast_d_u16) {
    uint8_t* restrict a = row_i;
    const uint8_t* restrict b = row_j;
    __m128i zero = _mm_setzero_si128();
    __m128i c_vec = _mm_set1_epi16(c);
    __m128i mod_vec= _mm_set1_epi16(mod);

    int i;
    // Основной цикл с разворотом x2 и префетчингом
    for (i = 0; i + 15 < n; i += 16) {
        // Префетч следующие блоки
        if (next_row != NULL)
            _mm_prefetch((const char*)(next_row + i), _MM_HINT_T0);
        
        // Выгружаем по 16 байт из каждого массива
        __m128i vi = _mm_load_si128((__m128i*)(a + i));
        __m128i vj = _mm_load_si128((__m128i*)(b + i));
        // Расширяем байты в слова
        __m128i i_lo = _mm_unpacklo_epi8(vi, zero);
        __m128i i_hi = _mm_unpackhi_epi8(vi, zero);
        __m128i j_lo = _mm_unpacklo_epi8(vj, zero);
        __m128i j_hi = _mm_unpackhi_epi8(vj, zero);
        // Вычисляем c * j + i
        __m128i sum_lo = _mm_add_epi16(i_lo, _mm_mullo_epi16(j_lo, c_vec));
        __m128i sum_hi = _mm_add_epi16(i_hi, _mm_mullo_epi16(j_hi, c_vec));
        // Делим на mod векторно через libdivide
        __m128i quot_lo = libdivide_u16_do_vec128(sum_lo, fast_d_u16);
        __m128i quot_hi = libdivide_u16_do_vec128(sum_hi, fast_d_u16);
        // Получаем остатки sum - quot*mod
        __m128i rem_lo = _mm_sub_epi16(sum_lo, _mm_mullo_epi16(quot_lo, mod_vec));
        __m128i rem_hi = _mm_sub_epi16(sum_hi, _mm_mullo_epi16(quot_hi, mod_vec));
        // Упаковываем результаты в байты и сохраняем 16 байт
        __m128i out = _mm_packus_epi16(rem_lo, rem_hi);
        _mm_store_si128((__m128i*)(a + i), out);
    }

    // Обработка оставшихся элементов
    for (; i < n; ++i) {
        uint16_t sum = (uint16_t)a[i] + (uint16_t)b[i] * c;
        uint16_t q = libdivide_u16_do(sum, fast_d_u16);
        a[i] = (uint8_t)(sum - q * mod);
    }
}

FIND_PIVOT_INIT(find_pivot_u8, u8)
POW_MOD_INIT(pow_mod_u8, u8)
CALC_DET_INIT(calc_det_u8, u8, find_pivot_u8, add_sse4_u8_safe, pow_mod_u8)

#define BUF_SIZE (1 << 16)
#define _ADD_X_(N, m, i, j, is_x, x) {\
    (m)[(i)][(j)++] = (x);\
    (is_x) = 0;\
    (x) = 0;\
    if ((j) == (N)) {\
        (i)++;\
        (j) = 0;\
    }\
    if ((i) == (N))\
        return (m);\
}     
#define PARSE_MAT_INIT(PARSE_NAME, FREE_NAME, TYPE)\
static inline void FREE_NAME(TYPE** m, int N) {\
    for (int i = 0; i < N; i++)\
        _mm_free(m[i]);\
    free(m);\
}\
static inline TYPE** PARSE_NAME(int N, FILE* in) {\
    TYPE** m = malloc(N * sizeof(TYPE*));\
    for (int _i = 0; _i < N; _i++) \
        m[_i] = (TYPE*)_mm_malloc(sizeof(TYPE) * N, 64);\
    int i = 0, j = 0;\
    TYPE x = 0;\
    int is_x = 0;\
    char buff[BUF_SIZE];\
    size_t r = 0;\
    while ((r = fread(buff, 1, BUF_SIZE, in)) != 0) {\
        for (size_t k = 0; k < r; k++) {\
            if (buff[k] >= '0' && buff[k] <= '9') {\
                is_x = 1;\
                x = x * 10 + (TYPE)(buff[k] - '0');\
            } else if (is_x)\
                _ADD_X_(N, m, i, j, is_x, x);\
        }\
    }\
    if (is_x)\
        _ADD_X_(N, m, i, j, is_x, x);\
    FREE_NAME(m, N);\
    return NULL;\
}

PARSE_MAT_INIT(parse_mat_u32, free_mat_u32, uint32_t)
PARSE_MAT_INIT(parse_mat_u16, free_mat_u16, uint16_t)
PARSE_MAT_INIT(parse_mat_u8, free_mat_u8, uint8_t)

int main(void) {
    int N;
    uint32_t mod;
    if (scanf("%d%u", &N, &mod) != 2 || N > 5000) {
        puts("bad input");
        return 0;
    }

    unsigned long long start;
    if (mod < (uint32_t)1 << 8) {
        start = __rdtsc();
        uint8_t** m = parse_mat_u8(N, stdin);
        fprintf(stderr, "%llu ", __rdtsc() - start);
        if (m) {
            start = __rdtsc();
            printf("%u", calc_det_u8(m, N, mod));
            fprintf(stderr, "%llu ", __rdtsc() - start);
            free_mat_u8(m, N);
        } else
            puts("bad input");
    } else if (mod < (uint32_t)1 << 16) {
        start = __rdtsc();
        uint16_t** m = parse_mat_u16(N, stdin);
        fprintf(stderr, "%llu ", __rdtsc() - start);
        if (m) {
            start = __rdtsc();
            printf("%u", calc_det_u16(m, N, mod));
            fprintf(stderr, "%llu ", __rdtsc() - start);
            free_mat_u16(m, N);
        } else
            puts("bad input");
    } else {
        start = __rdtsc();
        uint32_t** m = parse_mat_u32(N, stdin);
        fprintf(stderr, "%llu ", __rdtsc() - start);
        if (m) {
            start = __rdtsc();
            printf("%u", calc_det_u32(m, N, mod));
            fprintf(stderr, "%llu ", __rdtsc() - start);
            free_mat_u32(m, N);
        } else
            puts("bad input");
    }

    return 0;
}