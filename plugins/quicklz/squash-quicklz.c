/* Copyright (c) 2013-2015 The Squash Authors
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors:
 *   Evan Nemerson <evan@nemerson.com>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

#include <squash/squash.h>

#include "quicklz.h"

typedef struct SquashQuickLZOptions_s {
  SquashOptions base_object;

  int level;
} SquashQuickLZOptions;

SQUASH_PLUGIN_EXPORT
SquashStatus                 squash_plugin_init_codec       (SquashCodec* codec, SquashCodecImpl* impl);

static size_t
squash_quicklz_get_max_compressed_size (SquashCodec* codec, size_t uncompressed_size) {
  return uncompressed_size + 400;
}

static size_t
squash_quicklz_get_uncompressed_size (SquashCodec* codec,
                                      size_t compressed_size,
                                      const uint8_t compressed[SQUASH_ARRAY_PARAM(compressed_size)]) {
  return qlz_size_decompressed ((const char*) compressed);
}

static SquashStatus
squash_quicklz_decompress_buffer (SquashCodec* codec,
                                  size_t* decompressed_size,
                                  uint8_t decompressed[SQUASH_ARRAY_PARAM(*decompressed_size)],
                                  size_t compressed_size,
                                  const uint8_t compressed[SQUASH_ARRAY_PARAM(compressed_size)],
                                  SquashOptions* options) {
  qlz_state_decompress* qlz_s;
  const size_t decompressed_s = qlz_size_decompressed ((const char*) compressed);

  if (*decompressed_size < decompressed_s) {
    return SQUASH_BUFFER_FULL;
  }

  qlz_s = (qlz_state_decompress*) malloc (sizeof (qlz_state_decompress));

  *decompressed_size = qlz_decompress ((const char*) compressed,
                                         (void*) decompressed,
                                         qlz_s);

  free (qlz_s);

  return (decompressed_s == *decompressed_size) ? SQUASH_OK : squash_error (SQUASH_FAILED);
}

static SquashStatus
squash_quicklz_compress_buffer (SquashCodec* codec,
                                size_t* compressed_size,
                                uint8_t compressed[SQUASH_ARRAY_PARAM(*compressed_size)],
                                size_t uncompressed_size,
                                const uint8_t uncompressed[SQUASH_ARRAY_PARAM(uncompressed_size)],
                                SquashOptions* options) {
  qlz_state_compress* qlz_s;

  if (*compressed_size < squash_quicklz_get_max_compressed_size (codec, uncompressed_size)) {
    return squash_error (SQUASH_BUFFER_FULL);
  }

  qlz_s = (qlz_state_compress*) malloc (sizeof (qlz_state_compress));

  *compressed_size = qlz_compress ((const void*) uncompressed,
                                     (char*) compressed,
                                     uncompressed_size,
                                     qlz_s);

  free (qlz_s);

  return (*compressed_size == 0) ? squash_error (SQUASH_FAILED) : SQUASH_OK;
}

SquashStatus
squash_plugin_init_codec (SquashCodec* codec, SquashCodecImpl* impl) {
  const char* name = squash_codec_get_name (codec);

  if (strcmp ("quicklz", name) == 0) {
    impl->get_uncompressed_size = squash_quicklz_get_uncompressed_size;
    impl->get_max_compressed_size = squash_quicklz_get_max_compressed_size;
    impl->decompress_buffer = squash_quicklz_decompress_buffer;
    impl->compress_buffer = squash_quicklz_compress_buffer;
  } else {
    return squash_error (SQUASH_UNABLE_TO_LOAD);
  }

  return SQUASH_OK;
}
