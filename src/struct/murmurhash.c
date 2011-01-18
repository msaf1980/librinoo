/**
 * @file   murmurhash.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan  9 19:11:36 2011
 *
 * @brief  Murmurhash functions - This implementation comes from http://sites.google.com/site/murmurhash/
 *
 *
 */

#include	"rinoo/rinoo.h"

u32	murmurhash(const void *key, u32 len, u32 seed)
{
  u32			h;
  u32			k;
  const u32		m = 0x5bd1e995;
  const int		r = 24;
  const unsigned char	*data = (const unsigned char *) key;

  h = seed ^ len;
  while (len >= 4)
    {
      k = *(u32 *) data;

      k *= m;
      k ^= k >> r;
      k *= m;

      h *= m;
      h ^= k;

      data += 4;
      len -= 4;
    }

  switch (len)
    {
    case 3:
      h ^= data[2] << 16;
    case 2:
      h ^= data[1] << 8;
    case 1:
      h ^= data[0];
      h *= m;
    };

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}

u64	murmurhash64(const void *key, u64 len, u64 seed)
{
  u64		h;
  u64		k;
  const u64	m = 0xc6a4a7935bd1e995ULL;
  const int	r = 47;
  const u64	*data = (const u64 *) key;
  const u64	*end = data + (len / 8);

  h = seed ^ (len * m);

  while (data != end)
    {
      k = *data++;

      k *= m;
      k ^= k >> r;
      k *= m;

      h ^= k;
      h *= m;
    }

  const unsigned char *data2 = (const unsigned char *) data;

  switch (len & 7)
    {
    case 7:
      h ^= (u64) data2[6] << 48;
    case 6:
      h ^= (u64) data2[5] << 40;
    case 5:
      h ^= (u64) data2[4] << 32;
    case 4:
      h ^= (u64) data2[3] << 24;
    case 3:
      h ^= (u64) data2[2] << 16;
    case 2:
      h ^= (u64) data2[1] << 8;
    case 1:
      h ^= (u64) data2[0];
      h *= m;
    };

  h ^= h >> r;
  h *= m;
  h ^= h >> r;

  return h;
}
