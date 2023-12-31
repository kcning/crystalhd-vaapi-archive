/*
 * exp golomb vlc stuff
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
 * Copyright (c) 2004 Alex Beregszaszi
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * @brief
 *     exp golomb vlc stuff
 * @author Michael Niedermayer <michaelni@gmx.at> and Alex Beregszaszi
 */

#ifndef AVCODEC_GOLOMB_H
#define AVCODEC_GOLOMB_H

#include <stdint.h>
#include "get_bits.h"

#define INVALID_VLC           0x80000000

extern const uint8_t ff_golomb_vlc_len[512];
extern const uint8_t ff_ue_golomb_vlc_code[512];
extern const  int8_t ff_se_golomb_vlc_code[512];
extern const uint8_t ff_ue_golomb_len[256];

extern const uint8_t ff_interleaved_golomb_vlc_len[256];
extern const uint8_t ff_interleaved_ue_golomb_vlc_code[256];
extern const  int8_t ff_interleaved_se_golomb_vlc_code[256];
extern const uint8_t ff_interleaved_dirac_golomb_vlc_code[256];


 /**
 * read unsigned exp golomb code.
 */
static inline int get_ue_golomb(GetBitContext *gb){
    unsigned int buf;
    int log;

    OPEN_READER(re, gb);
    UPDATE_CACHE(re, gb);
    buf=GET_CACHE(re, gb);

    if(buf >= (1<<27)){
        buf >>= 32 - 9;
        LAST_SKIP_BITS(re, gb, ff_golomb_vlc_len[buf]);
        CLOSE_READER(re, gb);

        return ff_ue_golomb_vlc_code[buf];
    }else{
        log= 2*av_log2(buf) - 31;
        buf>>= log;
        buf--;
        LAST_SKIP_BITS(re, gb, 32 - log);
        CLOSE_READER(re, gb);

        return buf;
    }
}

 /**
 * read unsigned exp golomb code, constraint to a max of 31.
 * the return value is undefined if the stored value exceeds 31.
 */
static inline int get_ue_golomb_31(GetBitContext *gb){
    unsigned int buf;

    OPEN_READER(re, gb);
    UPDATE_CACHE(re, gb);
    buf=GET_CACHE(re, gb);

    buf >>= 32 - 9;
    LAST_SKIP_BITS(re, gb, ff_golomb_vlc_len[buf]);
    CLOSE_READER(re, gb);

    return ff_ue_golomb_vlc_code[buf];
}

/**
 * read unsigned truncated exp golomb code.
 */
static inline int get_te0_golomb(GetBitContext *gb, int range){
    assert(range >= 1);

    if(range==1)      return 0;
    else if(range==2) return get_bits1(gb)^1;
    else              return get_ue_golomb(gb);
}

/**
 * read unsigned truncated exp golomb code.
 */
static inline int get_te_golomb(GetBitContext *gb, int range){
    assert(range >= 1);

    if(range==2) return get_bits1(gb)^1;
    else         return get_ue_golomb(gb);
}


/**
 * read signed exp golomb code.
 */
static inline int get_se_golomb(GetBitContext *gb){
    unsigned int buf;
    int log;

    OPEN_READER(re, gb);
    UPDATE_CACHE(re, gb);
    buf=GET_CACHE(re, gb);

    if(buf >= (1<<27)){
        buf >>= 32 - 9;
        LAST_SKIP_BITS(re, gb, ff_golomb_vlc_len[buf]);
        CLOSE_READER(re, gb);

        return ff_se_golomb_vlc_code[buf];
    }else{
        log= 2*av_log2(buf) - 31;
        buf>>= log;

        LAST_SKIP_BITS(re, gb, 32 - log);
        CLOSE_READER(re, gb);

        if(buf&1) buf= -(buf>>1);
        else      buf=  (buf>>1);

        return buf;
    }
}

/**
 * read unsigned golomb rice code (ffv1).
 */
static inline int get_ur_golomb(GetBitContext *gb, int k, int limit, int esc_len){
    unsigned int buf;
    int log;

    OPEN_READER(re, gb);
    UPDATE_CACHE(re, gb);
    buf=GET_CACHE(re, gb);

    log= av_log2(buf);

    if(log > 31-limit){
        buf >>= log - k;
        buf += (30-log)<<k;
        LAST_SKIP_BITS(re, gb, 32 + k - log);
        CLOSE_READER(re, gb);

        return buf;
    }else{
        LAST_SKIP_BITS(re, gb, limit);
        UPDATE_CACHE(re, gb);

        buf = SHOW_UBITS(re, gb, esc_len);

        LAST_SKIP_BITS(re, gb, esc_len);
        CLOSE_READER(re, gb);

        return buf + limit - 1;
    }
}

/**
 * read unsigned golomb rice code (jpegls).
 */
static inline int get_ur_golomb_jpegls(GetBitContext *gb, int k, int limit, int esc_len){
    unsigned int buf;
    int log;

    OPEN_READER(re, gb);
    UPDATE_CACHE(re, gb);
    buf=GET_CACHE(re, gb);

    log= av_log2(buf);

    if(log - k >= 32-MIN_CACHE_BITS+(MIN_CACHE_BITS==32) && 32-log < limit){
        buf >>= log - k;
        buf += (30-log)<<k;
        LAST_SKIP_BITS(re, gb, 32 + k - log);
        CLOSE_READER(re, gb);

        return buf;
    }else{
        int i;
        for(i=0; SHOW_UBITS(re, gb, 1) == 0; i++){
            LAST_SKIP_BITS(re, gb, 1);
            UPDATE_CACHE(re, gb);
        }
        SKIP_BITS(re, gb, 1);

        if(i < limit - 1){
            if(k){
                buf = SHOW_UBITS(re, gb, k);
                LAST_SKIP_BITS(re, gb, k);
            }else{
                buf=0;
            }

            CLOSE_READER(re, gb);
            return buf + (i<<k);
        }else if(i == limit - 1){
            buf = SHOW_UBITS(re, gb, esc_len);
            LAST_SKIP_BITS(re, gb, esc_len);
            CLOSE_READER(re, gb);

            return buf + 1;
        }else
            return -1;
    }
}

/**
 * read signed golomb rice code (ffv1).
 */
static inline int get_sr_golomb(GetBitContext *gb, int k, int limit, int esc_len){
    int v= get_ur_golomb(gb, k, limit, esc_len);

    v++;
    if (v&1) return v>>1;
    else return -(v>>1);

//    return (v>>1) ^ -(v&1);
}

/**
 * read signed golomb rice code (flac).
 */
static inline int get_sr_golomb_flac(GetBitContext *gb, int k, int limit, int esc_len){
    int v= get_ur_golomb_jpegls(gb, k, limit, esc_len);
    return (v>>1) ^ -(v&1);
}

/**
 * read unsigned golomb rice code (shorten).
 */
static inline unsigned int get_ur_golomb_shorten(GetBitContext *gb, int k){
        return get_ur_golomb_jpegls(gb, k, INT_MAX, 0);
}

/**
 * read signed golomb rice code (shorten).
 */
static inline int get_sr_golomb_shorten(GetBitContext* gb, int k)
{
    int uvar = get_ur_golomb_jpegls(gb, k + 1, INT_MAX, 0);
    if (uvar & 1)
        return ~(uvar >> 1);
    else
        return uvar >> 1;
}



#ifdef TRACE

static inline int get_ue(GetBitContext *s, char *file, const char *func, int line){
    int show= show_bits(s, 24);
    int pos= get_bits_count(s);
    int i= get_ue_golomb(s);
    int len= get_bits_count(s) - pos;
    int bits= show>>(24-len);

    print_bin(bits, len);

    av_log(NULL, AV_LOG_DEBUG, "%5d %2d %3d ue  @%5d in %s %s:%d\n", bits, len, i, pos, file, func, line);

    return i;
}

static inline int get_se(GetBitContext *s, char *file, const char *func, int line){
    int show= show_bits(s, 24);
    int pos= get_bits_count(s);
    int i= get_se_golomb(s);
    int len= get_bits_count(s) - pos;
    int bits= show>>(24-len);

    print_bin(bits, len);

    av_log(NULL, AV_LOG_DEBUG, "%5d %2d %3d se  @%5d in %s %s:%d\n", bits, len, i, pos, file, func, line);

    return i;
}

static inline int get_te(GetBitContext *s, int r, char *file, const char *func, int line){
    int show= show_bits(s, 24);
    int pos= get_bits_count(s);
    int i= get_te0_golomb(s, r);
    int len= get_bits_count(s) - pos;
    int bits= show>>(24-len);

    print_bin(bits, len);

    av_log(NULL, AV_LOG_DEBUG, "%5d %2d %3d te  @%5d in %s %s:%d\n", bits, len, i, pos, file, func, line);

    return i;
}

#define get_ue_golomb(a) get_ue(a, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define get_se_golomb(a) get_se(a, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define get_te_golomb(a, r) get_te(a, r, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define get_te0_golomb(a, r) get_te(a, r, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#endif

#endif /* AVCODEC_GOLOMB_H */
