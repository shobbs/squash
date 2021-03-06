/* Copyright (c) 2013-2017 The Squash Authors
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
/* IWYU pragma: private, include <squash.h> */

#ifndef SQUASH_CODEC_H
#define SQUASH_CODEC_H

#include <squash.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#if !defined (SQUASH_H_INSIDE) && !defined (SQUASH_COMPILATION)
#error "Only <squash.h> can be included directly."
#endif

HEDLEY_BEGIN_C_DECLS

typedef enum {
  SQUASH_CODEC_INFO_CAN_FLUSH               = 1 <<  0,
  SQUASH_CODEC_INFO_DECOMPRESS_UNSAFE       = 1 <<  1,
  SQUASH_CODEC_INFO_WRAP_SIZE               = 1 <<  2,

  SQUASH_CODEC_INFO_AUTO_MASK               = 0x00ff0000,
  SQUASH_CODEC_INFO_VALID                   = 1 << 16,
  SQUASH_CODEC_INFO_KNOWS_UNCOMPRESSED_SIZE = 1 << 17,
  SQUASH_CODEC_INFO_NATIVE_STREAMING        = 1 << 18,

  SQUASH_CODEC_INFO_MASK                    = 0x00ffffff
} SquashCodecInfo;

#define SQUASH_CODEC_INFO_INVALID ((SquashCodecInfo) 0)

typedef SquashStatus (*SquashReadFunc)  (size_t* data_size,
                                         uint8_t data[HEDLEY_ARRAY_PARAM(*data_size)],
                                         void* user_data);
typedef SquashStatus (*SquashWriteFunc) (size_t* data_size,
                                         const uint8_t data[HEDLEY_ARRAY_PARAM(*data_size)],
                                         void* user_data);

struct SquashCodecImpl_ {
  SquashCodecInfo           info;

  const SquashOptionInfo*   options;

  /* Streams */
  SquashStream*           (* create_stream)            (SquashCodec* codec, SquashStreamType stream_type, SquashOptions* options);
  SquashStatus            (* process_stream)           (SquashStream* stream, SquashOperation operation);

  /* Splicing */
  SquashStatus            (* splice)                   (SquashCodec* codec,
                                                        SquashOptions* options,
                                                        SquashStreamType stream_type,
                                                        SquashReadFunc read_cb,
                                                        SquashWriteFunc write_cb,
                                                        void* user_data);

  /* Buffers */
  SquashStatus            (* decompress_buffer)        (SquashCodec* codec,
                                                        size_t* decompressed_size,
                                                        uint8_t decompressed[HEDLEY_ARRAY_PARAM(*decompressed_size)],
                                                        size_t compressed_size,
                                                        const uint8_t compressed[HEDLEY_ARRAY_PARAM(compressed_size)],
                                                        SquashOptions* options);
  SquashStatus            (* compress_buffer)          (SquashCodec* codec,
                                                        size_t* compressed_size,
                                                        uint8_t compressed[HEDLEY_ARRAY_PARAM(*compressed_size)],
                                                        size_t uncompressed_size,
                                                        const uint8_t uncompressed[HEDLEY_ARRAY_PARAM(uncompressed_size)],
                                                        SquashOptions* options);
  SquashStatus            (* compress_buffer_unsafe)   (SquashCodec* codec,
                                                        size_t* compressed_size,
                                                        uint8_t compressed[HEDLEY_ARRAY_PARAM(*compressed_size)],
                                                        size_t uncompressed_size,
                                                        const uint8_t uncompressed[HEDLEY_ARRAY_PARAM(uncompressed_size)],
                                                        SquashOptions* options);

  /* Codecs */
  size_t                  (* get_uncompressed_size)    (SquashCodec* codec,
                                                        size_t compressed_size,
                                                        const uint8_t compressed[HEDLEY_ARRAY_PARAM(compressed_size)]);
  size_t                  (* get_max_compressed_size)  (SquashCodec* codec, size_t uncompressed_size);

  /* Reserved */
  void                    (* _reserved1)               (void);
  void                    (* _reserved2)               (void);
  void                    (* _reserved3)               (void);
  void                    (* _reserved4)               (void);
  void                    (* _reserved5)               (void);
  void                    (* _reserved6)               (void);
  void                    (* _reserved7)               (void);
  void                    (* _reserved8)               (void);
};

typedef void (*SquashCodecForeachFunc) (SquashCodec* codec, void* data);

HEDLEY_NON_NULL(1)
SQUASH_API SquashStatus            squash_codec_init                         (SquashCodec* codec);
HEDLEY_NON_NULL(1)
SQUASH_API const char*             squash_codec_get_name                     (SquashCodec* codec);
HEDLEY_NON_NULL(1)
SQUASH_API unsigned int            squash_codec_get_priority                 (SquashCodec* codec);
HEDLEY_NON_NULL(1)
SQUASH_API SquashPlugin*           squash_codec_get_plugin                   (SquashCodec* codec);
HEDLEY_NON_NULL(1)
SQUASH_API SquashContext*          squash_codec_get_context                  (SquashCodec* codec);
HEDLEY_NON_NULL(1)
SQUASH_API const char*             squash_codec_get_extension                (SquashCodec* codec);

HEDLEY_NON_NULL(1, 3)
SQUASH_API size_t                  squash_codec_get_uncompressed_size        (SquashCodec* codec,
                                                                              size_t compressed_size,
                                                                              const uint8_t compressed[HEDLEY_ARRAY_PARAM(compressed_size)]);
HEDLEY_NON_NULL(1)
SQUASH_API size_t                  squash_codec_get_max_compressed_size      (SquashCodec* codec, size_t uncompressed_size);

HEDLEY_SENTINEL(0)
HEDLEY_NON_NULL(1)
SQUASH_API SquashStream*           squash_codec_create_stream                (SquashCodec* codec, SquashStreamType stream_type, ...);
HEDLEY_NON_NULL(1)
SQUASH_API SquashStream*           squash_codec_create_stream_with_options   (SquashCodec* codec, SquashStreamType stream_type, SquashOptions* options);

HEDLEY_SENTINEL(0)
HEDLEY_NON_NULL(1, 2, 3, 5)
SQUASH_API SquashStatus            squash_codec_compress                     (SquashCodec* codec,
                                                                              size_t* compressed_size,
                                                                              uint8_t compressed[HEDLEY_ARRAY_PARAM(*compressed_size)],
                                                                              size_t uncompressed_size,
                                                                              const uint8_t uncompressed[HEDLEY_ARRAY_PARAM(uncompressed_size)],
                                                                              ...);
HEDLEY_NON_NULL(1, 2, 3, 5)
SQUASH_API SquashStatus            squash_codec_compress_with_options        (SquashCodec* codec,
                                                                              size_t* compressed_size,
                                                                              uint8_t compressed[HEDLEY_ARRAY_PARAM(*compressed_size)],
                                                                              size_t uncompressed_size,
                                                                              const uint8_t uncompressed[HEDLEY_ARRAY_PARAM(uncompressed_size)],
                                                                              SquashOptions* options);
HEDLEY_SENTINEL(0)
HEDLEY_NON_NULL(1, 2, 3, 5)
SQUASH_API SquashStatus            squash_codec_decompress                   (SquashCodec* codec,
                                                                              size_t* decompressed_size,
                                                                              uint8_t decompressed[HEDLEY_ARRAY_PARAM(*decompressed_size)],
                                                                              size_t compressed_size,
                                                                              const uint8_t compressed[HEDLEY_ARRAY_PARAM(compressed_size)],
                                                                              ...);
HEDLEY_NON_NULL(1, 2, 3, 5)
SQUASH_API SquashStatus            squash_codec_decompress_with_options      (SquashCodec* codec,
                                                                              size_t* decompressed_size,
                                                                              uint8_t decompressed[HEDLEY_ARRAY_PARAM(*decompressed_size)],
                                                                              size_t compressed_size,
                                                                              const uint8_t compressed[HEDLEY_ARRAY_PARAM(compressed_size)],
                                                                              SquashOptions* options);
HEDLEY_NON_NULL(1)
SQUASH_API SquashCodecInfo         squash_codec_get_info                     (SquashCodec* codec);
HEDLEY_NON_NULL(1)
SQUASH_API const SquashOptionInfo* squash_codec_get_option_info              (SquashCodec* codec);

HEDLEY_END_C_DECLS

#endif /* SQUASH_CODEC_H */
