/*
 * copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of BVBase.
 *
 * BVBase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVBase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BVBase; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef BVUTIL_PIXFMT_H
#define BVUTIL_PIXFMT_H

/**
 * @file
 * pixel format definitions
 *
 */

#include "libbvutil/bbconfig.h"
#include "version.h"

#define BVPALETTE_SIZE 1024
#define BVPALETTE_COUNT 256

/**
 * Pixel format.
 *
 * @note
 * BV_PIX_FMT_RGB32 is handled in an endian-specific manner. An RGBA
 * color is put together as:
 *  (A << 24) | (R << 16) | (G << 8) | B
 * This is stored as BGRA on little-endian CPU architectures and ARGB on
 * big-endian CPUs.
 *
 * @par
 * When the pixel format is palettized RGB (BV_PIX_FMT_PAL8), the palettized
 * image data is stored in BVFrame.data[0]. The palette is transported in
 * BVFrame.data[1], is 1024 bytes long (256 4-byte entries) and is
 * formatted the same as in BV_PIX_FMT_RGB32 described above (i.e., it is
 * also endian-specific). Note also that the individual RGB palette
 * components stored in BVFrame.data[1] should be in the range 0..255.
 * This is important as many custom PAL8 video codecs that were designed
 * to run on the IBM VGA graphics adapter use 6-bit palette components.
 *
 * @par
 * For all the 8bit per pixel formats, an RGB32 palette is in data[1] like
 * for pal8. This palette is filled in automatically by the function
 * allocating the picture.
 *
 * @note
 * Make sure that all newly added big-endian formats have (pix_fmt & 1) == 1
 * and that all newly added little-endian formats have (pix_fmt & 1) == 0.
 * This allows simpler detection of big vs little-endian.
 */
enum BVPixelFormat {
    BV_PIX_FMT_NONE = -1,
    BV_PIX_FMT_YUV420P,   ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    BV_PIX_FMT_YUYV422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
    BV_PIX_FMT_RGB24,     ///< packed RGB 8:8:8, 24bpp, RGBRGB...
    BV_PIX_FMT_BGR24,     ///< packed RGB 8:8:8, 24bpp, BGRBGR...
    BV_PIX_FMT_YUV422P,   ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    BV_PIX_FMT_YUV444P,   ///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
    BV_PIX_FMT_YUV410P,   ///< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
    BV_PIX_FMT_YUV411P,   ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
    BV_PIX_FMT_GRAY8,     ///<        Y        ,  8bpp
    BV_PIX_FMT_MONOWHITE, ///<        Y        ,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb
    BV_PIX_FMT_MONOBLACK, ///<        Y        ,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb
    BV_PIX_FMT_PAL8,      ///< 8 bit with PIX_FMT_RGB32 palette
    BV_PIX_FMT_YUVJ420P,  ///< planar YUV 4:2:0, 12bpp, full scale (JPEG), deprecated in favor of PIX_FMT_YUV420P and setting color_range
    BV_PIX_FMT_YUVJ422P,  ///< planar YUV 4:2:2, 16bpp, full scale (JPEG), deprecated in favor of PIX_FMT_YUV422P and setting color_range
    BV_PIX_FMT_YUVJ444P,  ///< planar YUV 4:4:4, 24bpp, full scale (JPEG), deprecated in favor of PIX_FMT_YUV444P and setting color_range
#if BV_API_XVMC
    BV_PIX_FMT_XVMC_MPEG2_MC,///< XVideo Motion Acceleration via common packet passing
    BV_PIX_FMT_XVMC_MPEG2_IDCT,
#define BV_PIX_FMT_XVMC BV_PIX_FMT_XVMC_MPEG2_IDCT
#endif /* BV_API_XVMC */
    BV_PIX_FMT_UYVY422,   ///< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
    BV_PIX_FMT_UYYVYY411, ///< packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
    BV_PIX_FMT_BGR8,      ///< packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
    BV_PIX_FMT_BGR4,      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1B 2G 1R(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
    BV_PIX_FMT_BGR4_BYTE, ///< packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
    BV_PIX_FMT_RGB8,      ///< packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)
    BV_PIX_FMT_RGB4,      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1R 2G 1B(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
    BV_PIX_FMT_RGB4_BYTE, ///< packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb)
    BV_PIX_FMT_NV12,      ///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    BV_PIX_FMT_NV21,      ///< as above, but U and V bytes are swapped

    BV_PIX_FMT_ARGB,      ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
    BV_PIX_FMT_RGBA,      ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
    BV_PIX_FMT_ABGR,      ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
    BV_PIX_FMT_BGRA,      ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...

    BV_PIX_FMT_GRAY16BE,  ///<        Y        , 16bpp, big-endian
    BV_PIX_FMT_GRAY16LE,  ///<        Y        , 16bpp, little-endian
    BV_PIX_FMT_YUV440P,   ///< planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples)
    BV_PIX_FMT_YUVJ440P,  ///< planar YUV 4:4:0 full scale (JPEG), deprecated in favor of PIX_FMT_YUV440P and setting color_range
    BV_PIX_FMT_YUVA420P,  ///< planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
#if BV_API_VDPAU
    BV_PIX_FMT_VDPAU_H264,///< H.264 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
    BV_PIX_FMT_VDPAU_MPEG1,///< MPEG-1 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
    BV_PIX_FMT_VDPAU_MPEG2,///< MPEG-2 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
    BV_PIX_FMT_VDPAU_WMV3,///< WMV3 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
    BV_PIX_FMT_VDPAU_VC1, ///< VC-1 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
#endif
    BV_PIX_FMT_RGB48BE,   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as big-endian
    BV_PIX_FMT_RGB48LE,   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as little-endian

    BV_PIX_FMT_RGB565BE,  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian
    BV_PIX_FMT_RGB565LE,  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian
    BV_PIX_FMT_RGB555BE,  ///< packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), big-endian, most significant bit to 0
    BV_PIX_FMT_RGB555LE,  ///< packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), little-endian, most significant bit to 0

    BV_PIX_FMT_BGR565BE,  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian
    BV_PIX_FMT_BGR565LE,  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian
    BV_PIX_FMT_BGR555BE,  ///< packed BGR 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), big-endian, most significant bit to 1
    BV_PIX_FMT_BGR555LE,  ///< packed BGR 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), little-endian, most significant bit to 1

    BV_PIX_FMT_VAAPI_MOCO, ///< HW acceleration through VA API at motion compensation entry-point, Picture.data[3] contains a vaapi_render_state struct which contains macroblocks as well as various fields extracted from headers
    BV_PIX_FMT_VAAPI_IDCT, ///< HW acceleration through VA API at IDCT entry-point, Picture.data[3] contains a vaapi_render_state struct which contains fields extracted from headers
    BV_PIX_FMT_VAAPI_VLD,  ///< HW decoding through VA API, Picture.data[3] contains a vaapi_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers

    BV_PIX_FMT_YUV420P16LE,  ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    BV_PIX_FMT_YUV420P16BE,  ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    BV_PIX_FMT_YUV422P16LE,  ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    BV_PIX_FMT_YUV422P16BE,  ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    BV_PIX_FMT_YUV444P16LE,  ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    BV_PIX_FMT_YUV444P16BE,  ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
#if BV_API_VDPAU
    BV_PIX_FMT_VDPAU_MPEG4,  ///< MPEG4 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
#endif
    BV_PIX_FMT_DXVA2_VLD,    ///< HW decoding through DXVA2, Picture.data[3] contains a LPDIRECT3DSURFACE9 pointer

    BV_PIX_FMT_RGB444LE,  ///< packed RGB 4:4:4, 16bpp, (msb)4A 4R 4G 4B(lsb), little-endian, most significant bits to 0
    BV_PIX_FMT_RGB444BE,  ///< packed RGB 4:4:4, 16bpp, (msb)4A 4R 4G 4B(lsb), big-endian, most significant bits to 0
    BV_PIX_FMT_BGR444LE,  ///< packed BGR 4:4:4, 16bpp, (msb)4A 4B 4G 4R(lsb), little-endian, most significant bits to 1
    BV_PIX_FMT_BGR444BE,  ///< packed BGR 4:4:4, 16bpp, (msb)4A 4B 4G 4R(lsb), big-endian, most significant bits to 1
    BV_PIX_FMT_YA8,       ///< 8bit gray, 8bit alpha

    BV_PIX_FMT_Y400A = BV_PIX_FMT_YA8, ///< alias for BV_PIX_FMT_YA8
    BV_PIX_FMT_GRAY8A= BV_PIX_FMT_YA8, ///< alias for BV_PIX_FMT_YA8

    BV_PIX_FMT_BGR48BE,   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian
    BV_PIX_FMT_BGR48LE,   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian

    /**
     * The following 12 formats have the disadvantage of needing 1 format for each bit depth.
     * Notice that each 9/10 bits sample is stored in 16 bits with extra padding.
     * If you want to support multiple bit depths, then using BV_PIX_FMT_YUV420P16* with the bpp stored separately is better.
     */
    BV_PIX_FMT_YUV420P9BE, ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    BV_PIX_FMT_YUV420P9LE, ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    BV_PIX_FMT_YUV420P10BE,///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    BV_PIX_FMT_YUV420P10LE,///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    BV_PIX_FMT_YUV422P10BE,///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    BV_PIX_FMT_YUV422P10LE,///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    BV_PIX_FMT_YUV444P9BE, ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    BV_PIX_FMT_YUV444P9LE, ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    BV_PIX_FMT_YUV444P10BE,///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    BV_PIX_FMT_YUV444P10LE,///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    BV_PIX_FMT_YUV422P9BE, ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    BV_PIX_FMT_YUV422P9LE, ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    BV_PIX_FMT_VDA_VLD,    ///< hardware decoding through VDA

#ifdef BV_PIX_FMT_ABI_GIT_MASTER
    BV_PIX_FMT_RGBA64BE,  ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    BV_PIX_FMT_RGBA64LE,  ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
    BV_PIX_FMT_BGRA64BE,  ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    BV_PIX_FMT_BGRA64LE,  ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
#endif
    BV_PIX_FMT_GBRP,      ///< planar GBR 4:4:4 24bpp
    BV_PIX_FMT_GBRP9BE,   ///< planar GBR 4:4:4 27bpp, big-endian
    BV_PIX_FMT_GBRP9LE,   ///< planar GBR 4:4:4 27bpp, little-endian
    BV_PIX_FMT_GBRP10BE,  ///< planar GBR 4:4:4 30bpp, big-endian
    BV_PIX_FMT_GBRP10LE,  ///< planar GBR 4:4:4 30bpp, little-endian
    BV_PIX_FMT_GBRP16BE,  ///< planar GBR 4:4:4 48bpp, big-endian
    BV_PIX_FMT_GBRP16LE,  ///< planar GBR 4:4:4 48bpp, little-endian

    /**
     * duplicated pixel formats for compatibility with libav.
     * BVBase supports these formats since May 8 2012 and Jan 28 2012 (commits f9ca1ac7 and 143a5c55)
     * Libav added them Oct 12 2012 with incompatible values (commit 6d5600e85)
     */
    BV_PIX_FMT_YUVA422P_LIBBV,  ///< planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples)
    BV_PIX_FMT_YUVA444P_LIBBV,  ///< planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples)

    BV_PIX_FMT_YUVA420P9BE,  ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), big-endian
    BV_PIX_FMT_YUVA420P9LE,  ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), little-endian
    BV_PIX_FMT_YUVA422P9BE,  ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), big-endian
    BV_PIX_FMT_YUVA422P9LE,  ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), little-endian
    BV_PIX_FMT_YUVA444P9BE,  ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), big-endian
    BV_PIX_FMT_YUVA444P9LE,  ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
    BV_PIX_FMT_YUVA420P10BE, ///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
    BV_PIX_FMT_YUVA420P10LE, ///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
    BV_PIX_FMT_YUVA422P10BE, ///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
    BV_PIX_FMT_YUVA422P10LE, ///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
    BV_PIX_FMT_YUVA444P10BE, ///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
    BV_PIX_FMT_YUVA444P10LE, ///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)
    BV_PIX_FMT_YUVA420P16BE, ///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
    BV_PIX_FMT_YUVA420P16LE, ///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
    BV_PIX_FMT_YUVA422P16BE, ///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
    BV_PIX_FMT_YUVA422P16LE, ///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
    BV_PIX_FMT_YUVA444P16BE, ///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
    BV_PIX_FMT_YUVA444P16LE, ///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)

    BV_PIX_FMT_VDPAU,     ///< HW acceleration through VDPAU, Picture.data[3] contains a VdpVideoSurface

    BV_PIX_FMT_XYZ12LE,      ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as little-endian, the 4 lower bits are set to 0
    BV_PIX_FMT_XYZ12BE,      ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as big-endian, the 4 lower bits are set to 0
    BV_PIX_FMT_NV16,         ///< interleaved chroma YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    BV_PIX_FMT_NV20LE,       ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    BV_PIX_FMT_NV20BE,       ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian

    /**
     * duplicated pixel formats for compatibility with libav.
     * BVBase supports these formats since Sat Sep 24 06:01:45 2011 +0200 (commits 9569a3c9f41387a8c7d1ce97d8693520477a66c3)
     * also see Fri Nov 25 01:38:21 2011 +0100 92afb431621c79155fcb7171d26f137eb1bee028
     * Libav added them Sun Mar 16 23:05:47 2014 +0100 with incompatible values (commit 1481d24c3a0abf81e1d7a514547bd5305232be30)
     */
    BV_PIX_FMT_RGBA64BE_LIBBV,     ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    BV_PIX_FMT_RGBA64LE_LIBBV,     ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
    BV_PIX_FMT_BGRA64BE_LIBBV,     ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    BV_PIX_FMT_BGRA64LE_LIBBV,     ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian

    BV_PIX_FMT_YVYU422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cr Y1 Cb

    BV_PIX_FMT_VDA,          ///< HW acceleration through VDA, data[3] contains a CVPixelBufferRef

    BV_PIX_FMT_YA16BE,       ///< 16bit gray, 16bit alpha (big-endian)
    BV_PIX_FMT_YA16LE,       ///< 16bit gray, 16bit alpha (little-endian)


#ifndef BV_PIX_FMT_ABI_GIT_MASTER
    BV_PIX_FMT_RGBA64BE=0x123,  ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    BV_PIX_FMT_RGBA64LE,  ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
    BV_PIX_FMT_BGRA64BE,  ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    BV_PIX_FMT_BGRA64LE,  ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
#endif
    BV_PIX_FMT_0RGB=0x123+4,      ///< packed RGB 8:8:8, 32bpp, 0RGB0RGB...
    BV_PIX_FMT_RGB0,      ///< packed RGB 8:8:8, 32bpp, RGB0RGB0...
    BV_PIX_FMT_0BGR,      ///< packed BGR 8:8:8, 32bpp, 0BGR0BGR...
    BV_PIX_FMT_BGR0,      ///< packed BGR 8:8:8, 32bpp, BGR0BGR0...
    BV_PIX_FMT_YUVA444P,  ///< planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples)
    BV_PIX_FMT_YUVA422P,  ///< planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples)

    BV_PIX_FMT_YUV420P12BE, ///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    BV_PIX_FMT_YUV420P12LE, ///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    BV_PIX_FMT_YUV420P14BE, ///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    BV_PIX_FMT_YUV420P14LE, ///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    BV_PIX_FMT_YUV422P12BE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    BV_PIX_FMT_YUV422P12LE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    BV_PIX_FMT_YUV422P14BE, ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    BV_PIX_FMT_YUV422P14LE, ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    BV_PIX_FMT_YUV444P12BE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    BV_PIX_FMT_YUV444P12LE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    BV_PIX_FMT_YUV444P14BE, ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    BV_PIX_FMT_YUV444P14LE, ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    BV_PIX_FMT_GBRP12BE,    ///< planar GBR 4:4:4 36bpp, big-endian
    BV_PIX_FMT_GBRP12LE,    ///< planar GBR 4:4:4 36bpp, little-endian
    BV_PIX_FMT_GBRP14BE,    ///< planar GBR 4:4:4 42bpp, big-endian
    BV_PIX_FMT_GBRP14LE,    ///< planar GBR 4:4:4 42bpp, little-endian
    BV_PIX_FMT_GBRAP,       ///< planar GBRA 4:4:4:4 32bpp
    BV_PIX_FMT_GBRAP16BE,   ///< planar GBRA 4:4:4:4 64bpp, big-endian
    BV_PIX_FMT_GBRAP16LE,   ///< planar GBRA 4:4:4:4 64bpp, little-endian
    BV_PIX_FMT_YUVJ411P,    ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) full scale (JPEG), deprecated in favor of PIX_FMT_YUV411P and setting color_range

    BV_PIX_FMT_BAYER_BGGR8,    ///< bayer, BGBG..(odd line), GRGR..(even line), 8-bit samples */
    BV_PIX_FMT_BAYER_RGGB8,    ///< bayer, RGRG..(odd line), GBGB..(even line), 8-bit samples */
    BV_PIX_FMT_BAYER_GBRG8,    ///< bayer, GBGB..(odd line), RGRG..(even line), 8-bit samples */
    BV_PIX_FMT_BAYER_GRBG8,    ///< bayer, GRGR..(odd line), BGBG..(even line), 8-bit samples */
    BV_PIX_FMT_BAYER_BGGR16LE, ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, little-endian */
    BV_PIX_FMT_BAYER_BGGR16BE, ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, big-endian */
    BV_PIX_FMT_BAYER_RGGB16LE, ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, little-endian */
    BV_PIX_FMT_BAYER_RGGB16BE, ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, big-endian */
    BV_PIX_FMT_BAYER_GBRG16LE, ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, little-endian */
    BV_PIX_FMT_BAYER_GBRG16BE, ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, big-endian */
    BV_PIX_FMT_BAYER_GRBG16LE, ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, little-endian */
    BV_PIX_FMT_BAYER_GRBG16BE, ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, big-endian */
#if !BV_API_XVMC
    BV_PIX_FMT_XVMC,///< XVideo Motion Acceleration via common packet passing
#endif /* !BV_API_XVMC */

    BV_PIX_FMT_NB,        ///< number of pixel formats, DO NOT USE THIS if you want to link with shared libav* because the number of formats might differ between versions

#if BV_API_PIX_FMT
//#include "old_pix_fmts.h"
#endif
};

#if BV_HAVE_INCOMPATIBLE_LIBBV_ABI
#define BV_PIX_FMT_YUVA422P BV_PIX_FMT_YUVA422P_LIBBV
#define BV_PIX_FMT_YUVA444P BV_PIX_FMT_YUVA444P_LIBBV
#define BV_PIX_FMT_RGBA64BE BV_PIX_FMT_RGBA64BE_LIBBV
#define BV_PIX_FMT_RGBA64LE BV_PIX_FMT_RGBA64LE_LIBBV
#define BV_PIX_FMT_BGRA64BE BV_PIX_FMT_BGRA64BE_LIBBV
#define BV_PIX_FMT_BGRA64LE BV_PIX_FMT_BGRA64LE_LIBBV
#endif


#define BV_PIX_FMT_Y400A BV_PIX_FMT_GRAY8A
#define BV_PIX_FMT_GBR24P BV_PIX_FMT_GBRP

#if BV_HAVE_BIGENDIAN
#   define BV_PIX_FMT_NE(be, le) BV_PIX_FMT_##be
#else
#   define BV_PIX_FMT_NE(be, le) BV_PIX_FMT_##le
#endif

#define BV_PIX_FMT_RGB32   BV_PIX_FMT_NE(ARGB, BGRA)
#define BV_PIX_FMT_RGB32_1 BV_PIX_FMT_NE(RGBA, ABGR)
#define BV_PIX_FMT_BGR32   BV_PIX_FMT_NE(ABGR, RGBA)
#define BV_PIX_FMT_BGR32_1 BV_PIX_FMT_NE(BGRA, ARGB)
#define BV_PIX_FMT_0RGB32  BV_PIX_FMT_NE(0RGB, BGR0)
#define BV_PIX_FMT_0BGR32  BV_PIX_FMT_NE(0BGR, RGB0)

#define BV_PIX_FMT_GRAY16 BV_PIX_FMT_NE(GRAY16BE, GRAY16LE)
#define BV_PIX_FMT_YA16   BV_PIX_FMT_NE(YA16BE,   YA16LE)
#define BV_PIX_FMT_RGB48  BV_PIX_FMT_NE(RGB48BE,  RGB48LE)
#define BV_PIX_FMT_RGB565 BV_PIX_FMT_NE(RGB565BE, RGB565LE)
#define BV_PIX_FMT_RGB555 BV_PIX_FMT_NE(RGB555BE, RGB555LE)
#define BV_PIX_FMT_RGB444 BV_PIX_FMT_NE(RGB444BE, RGB444LE)
#define BV_PIX_FMT_RGBA64 BV_PIX_FMT_NE(RGBA64BE, RGBA64LE)
#define BV_PIX_FMT_BGR48  BV_PIX_FMT_NE(BGR48BE,  BGR48LE)
#define BV_PIX_FMT_BGR565 BV_PIX_FMT_NE(BGR565BE, BGR565LE)
#define BV_PIX_FMT_BGR555 BV_PIX_FMT_NE(BGR555BE, BGR555LE)
#define BV_PIX_FMT_BGR444 BV_PIX_FMT_NE(BGR444BE, BGR444LE)
#define BV_PIX_FMT_BGRA64 BV_PIX_FMT_NE(BGRA64BE, BGRA64LE)

#define BV_PIX_FMT_YUV420P9  BV_PIX_FMT_NE(YUV420P9BE , YUV420P9LE)
#define BV_PIX_FMT_YUV422P9  BV_PIX_FMT_NE(YUV422P9BE , YUV422P9LE)
#define BV_PIX_FMT_YUV444P9  BV_PIX_FMT_NE(YUV444P9BE , YUV444P9LE)
#define BV_PIX_FMT_YUV420P10 BV_PIX_FMT_NE(YUV420P10BE, YUV420P10LE)
#define BV_PIX_FMT_YUV422P10 BV_PIX_FMT_NE(YUV422P10BE, YUV422P10LE)
#define BV_PIX_FMT_YUV444P10 BV_PIX_FMT_NE(YUV444P10BE, YUV444P10LE)
#define BV_PIX_FMT_YUV420P12 BV_PIX_FMT_NE(YUV420P12BE, YUV420P12LE)
#define BV_PIX_FMT_YUV422P12 BV_PIX_FMT_NE(YUV422P12BE, YUV422P12LE)
#define BV_PIX_FMT_YUV444P12 BV_PIX_FMT_NE(YUV444P12BE, YUV444P12LE)
#define BV_PIX_FMT_YUV420P14 BV_PIX_FMT_NE(YUV420P14BE, YUV420P14LE)
#define BV_PIX_FMT_YUV422P14 BV_PIX_FMT_NE(YUV422P14BE, YUV422P14LE)
#define BV_PIX_FMT_YUV444P14 BV_PIX_FMT_NE(YUV444P14BE, YUV444P14LE)
#define BV_PIX_FMT_YUV420P16 BV_PIX_FMT_NE(YUV420P16BE, YUV420P16LE)
#define BV_PIX_FMT_YUV422P16 BV_PIX_FMT_NE(YUV422P16BE, YUV422P16LE)
#define BV_PIX_FMT_YUV444P16 BV_PIX_FMT_NE(YUV444P16BE, YUV444P16LE)

#define BV_PIX_FMT_GBRP9     BV_PIX_FMT_NE(GBRP9BE ,    GBRP9LE)
#define BV_PIX_FMT_GBRP10    BV_PIX_FMT_NE(GBRP10BE,    GBRP10LE)
#define BV_PIX_FMT_GBRP12    BV_PIX_FMT_NE(GBRP12BE,    GBRP12LE)
#define BV_PIX_FMT_GBRP14    BV_PIX_FMT_NE(GBRP14BE,    GBRP14LE)
#define BV_PIX_FMT_GBRP16    BV_PIX_FMT_NE(GBRP16BE,    GBRP16LE)
#define BV_PIX_FMT_GBRAP16   BV_PIX_FMT_NE(GBRAP16BE,   GBRAP16LE)

#define BV_PIX_FMT_BAYER_BGGR16 BV_PIX_FMT_NE(BAYER_BGGR16BE,    BAYER_BGGR16LE)
#define BV_PIX_FMT_BAYER_RGGB16 BV_PIX_FMT_NE(BAYER_RGGB16BE,    BAYER_RGGB16LE)
#define BV_PIX_FMT_BAYER_GBRG16 BV_PIX_FMT_NE(BAYER_GBRG16BE,    BAYER_GBRG16LE)
#define BV_PIX_FMT_BAYER_GRBG16 BV_PIX_FMT_NE(BAYER_GRBG16BE,    BAYER_GRBG16LE)


#define BV_PIX_FMT_YUVA420P9  BV_PIX_FMT_NE(YUVA420P9BE , YUVA420P9LE)
#define BV_PIX_FMT_YUVA422P9  BV_PIX_FMT_NE(YUVA422P9BE , YUVA422P9LE)
#define BV_PIX_FMT_YUVA444P9  BV_PIX_FMT_NE(YUVA444P9BE , YUVA444P9LE)
#define BV_PIX_FMT_YUVA420P10 BV_PIX_FMT_NE(YUVA420P10BE, YUVA420P10LE)
#define BV_PIX_FMT_YUVA422P10 BV_PIX_FMT_NE(YUVA422P10BE, YUVA422P10LE)
#define BV_PIX_FMT_YUVA444P10 BV_PIX_FMT_NE(YUVA444P10BE, YUVA444P10LE)
#define BV_PIX_FMT_YUVA420P16 BV_PIX_FMT_NE(YUVA420P16BE, YUVA420P16LE)
#define BV_PIX_FMT_YUVA422P16 BV_PIX_FMT_NE(YUVA422P16BE, YUVA422P16LE)
#define BV_PIX_FMT_YUVA444P16 BV_PIX_FMT_NE(YUVA444P16BE, YUVA444P16LE)

#define BV_PIX_FMT_XYZ12      BV_PIX_FMT_NE(XYZ12BE, XYZ12LE)
#define BV_PIX_FMT_NV20       BV_PIX_FMT_NE(NV20BE,  NV20LE)


#if BV_API_PIX_FMT && 0
#define PixelFormat BVPixelFormat

#define PIX_FMT_Y400A BV_PIX_FMT_Y400A
#define PIX_FMT_GBR24P BV_PIX_FMT_GBR24P

#define PIX_FMT_NE(be, le) BV_PIX_FMT_NE(be, le)

#define PIX_FMT_RGB32   BV_PIX_FMT_RGB32
#define PIX_FMT_RGB32_1 BV_PIX_FMT_RGB32_1
#define PIX_FMT_BGR32   BV_PIX_FMT_BGR32
#define PIX_FMT_BGR32_1 BV_PIX_FMT_BGR32_1
#define PIX_FMT_0RGB32  BV_PIX_FMT_0RGB32
#define PIX_FMT_0BGR32  BV_PIX_FMT_0BGR32

#define PIX_FMT_GRAY16 BV_PIX_FMT_GRAY16
#define PIX_FMT_RGB48  BV_PIX_FMT_RGB48
#define PIX_FMT_RGB565 BV_PIX_FMT_RGB565
#define PIX_FMT_RGB555 BV_PIX_FMT_RGB555
#define PIX_FMT_RGB444 BV_PIX_FMT_RGB444
#define PIX_FMT_BGR48  BV_PIX_FMT_BGR48
#define PIX_FMT_BGR565 BV_PIX_FMT_BGR565
#define PIX_FMT_BGR555 BV_PIX_FMT_BGR555
#define PIX_FMT_BGR444 BV_PIX_FMT_BGR444

#define PIX_FMT_YUV420P9  BV_PIX_FMT_YUV420P9
#define PIX_FMT_YUV422P9  BV_PIX_FMT_YUV422P9
#define PIX_FMT_YUV444P9  BV_PIX_FMT_YUV444P9
#define PIX_FMT_YUV420P10 BV_PIX_FMT_YUV420P10
#define PIX_FMT_YUV422P10 BV_PIX_FMT_YUV422P10
#define PIX_FMT_YUV444P10 BV_PIX_FMT_YUV444P10
#define PIX_FMT_YUV420P12 BV_PIX_FMT_YUV420P12
#define PIX_FMT_YUV422P12 BV_PIX_FMT_YUV422P12
#define PIX_FMT_YUV444P12 BV_PIX_FMT_YUV444P12
#define PIX_FMT_YUV420P14 BV_PIX_FMT_YUV420P14
#define PIX_FMT_YUV422P14 BV_PIX_FMT_YUV422P14
#define PIX_FMT_YUV444P14 BV_PIX_FMT_YUV444P14
#define PIX_FMT_YUV420P16 BV_PIX_FMT_YUV420P16
#define PIX_FMT_YUV422P16 BV_PIX_FMT_YUV422P16
#define PIX_FMT_YUV444P16 BV_PIX_FMT_YUV444P16

#define PIX_FMT_RGBA64 BV_PIX_FMT_RGBA64
#define PIX_FMT_BGRA64 BV_PIX_FMT_BGRA64
#define PIX_FMT_GBRP9  BV_PIX_FMT_GBRP9
#define PIX_FMT_GBRP10 BV_PIX_FMT_GBRP10
#define PIX_FMT_GBRP12 BV_PIX_FMT_GBRP12
#define PIX_FMT_GBRP14 BV_PIX_FMT_GBRP14
#define PIX_FMT_GBRP16 BV_PIX_FMT_GBRP16
#endif

/**
  * Chromaticity coordinates of the source primaries.
  */
enum BVColorPrimaries {
    BVCOL_PRI_RESERVED0   = 0,
    BVCOL_PRI_BT709       = 1, ///< also ITU-R BT1361 / IEC 61966-2-4 / SMPTE RP177 Annex B
    BVCOL_PRI_UNSPECIFIED = 2,
    BVCOL_PRI_RESERVED    = 3,
    BVCOL_PRI_BT470M      = 4, ///< also FCC Title 47 Code of Federal Regulations 73.682 (a)(20)

    BVCOL_PRI_BT470BG     = 5, ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM
    BVCOL_PRI_SMPTE170M   = 6, ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
    BVCOL_PRI_SMPTE240M   = 7, ///< functionally identical to above
    BVCOL_PRI_FILM        = 8, ///< colour filters using Illuminant C
    BVCOL_PRI_BT2020      = 9, ///< ITU-R BT2020
    BVCOL_PRI_NB,              ///< Not part of ABI
};

/**
 * Color Transfer Characteristic.
 */
enum BVColorTransferCharacteristic {
    BVCOL_TRC_RESERVED0    = 0,
    BVCOL_TRC_BT709        = 1,  ///< also ITU-R BT1361
    BVCOL_TRC_UNSPECIFIED  = 2,
    BVCOL_TRC_RESERVED     = 3,
    BVCOL_TRC_GAMMA22      = 4,  ///< also ITU-R BT470M / ITU-R BT1700 625 PAL & SECAM
    BVCOL_TRC_GAMMA28      = 5,  ///< also ITU-R BT470BG
    BVCOL_TRC_SMPTE170M    = 6,  ///< also ITU-R BT601-6 525 or 625 / ITU-R BT1358 525 or 625 / ITU-R BT1700 NTSC
    BVCOL_TRC_SMPTE240M    = 7,
    BVCOL_TRC_LINEAR       = 8,  ///< "Linear transfer characteristics"
    BVCOL_TRC_LOG          = 9,  ///< "Logarithmic transfer characteristic (100:1 range)"
    BVCOL_TRC_LOG_SQRT     = 10, ///< "Logarithmic transfer characteristic (100 * Sqrt(10) : 1 range)"
    BVCOL_TRC_IEC61966_2_4 = 11, ///< IEC 61966-2-4
    BVCOL_TRC_BT1361_ECG   = 12, ///< ITU-R BT1361 Extended Colour Gamut
    BVCOL_TRC_IEC61966_2_1 = 13, ///< IEC 61966-2-1 (sRGB or sYCC)
    BVCOL_TRC_BT2020_10    = 14, ///< ITU-R BT2020 for 10 bit system
    BVCOL_TRC_BT2020_12    = 15, ///< ITU-R BT2020 for 12 bit system
    BVCOL_TRC_NB,                ///< Not part of ABI
};

/**
 * YUV colorspace type.
 */
enum BVColorSpace {
    BVCOL_SPC_RGB         = 0,  ///< order of coefficients is actually GBR, also IEC 61966-2-1 (sRGB)
    BVCOL_SPC_BT709       = 1,  ///< also ITU-R BT1361 / IEC 61966-2-4 xvYCC709 / SMPTE RP177 Annex B
    BVCOL_SPC_UNSPECIFIED = 2,
    BVCOL_SPC_RESERVED    = 3,
    BVCOL_SPC_FCC         = 4,  ///< FCC Title 47 Code of Federal Regulations 73.682 (a)(20)
    BVCOL_SPC_BT470BG     = 5,  ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601
    BVCOL_SPC_SMPTE170M   = 6,  ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC / functionally identical to above
    BVCOL_SPC_SMPTE240M   = 7,
    BVCOL_SPC_YCOCG       = 8,  ///< Used by Dirac / VC-2 and H.264 FRext, see ITU-T SG16
    BVCOL_SPC_BT2020_NCL  = 9,  ///< ITU-R BT2020 non-constant luminance system
    BVCOL_SPC_BT2020_CL   = 10, ///< ITU-R BT2020 constant luminance system
    BVCOL_SPC_NB,               ///< Not part of ABI
};
#define BVCOL_SPC_YCGCO BVCOL_SPC_YCOCG


/**
 * MPEG vs JPEG YUV range.
 */
enum BVColorRange {
    BVCOL_RANGE_UNSPECIFIED = 0,
    BVCOL_RANGE_MPEG        = 1, ///< the normal 219*2^(n-8) "MPEG" YUV ranges
    BVCOL_RANGE_JPEG        = 2, ///< the normal     2^n-1   "JPEG" YUV ranges
    BVCOL_RANGE_NB,              ///< Not part of ABI
};

/**
 * Location of chroma samples.
 *
 *  X   X      3 4 X      X are luma samples,
 *             1 2        1-6 are possible chroma positions
 *  X   X      5 6 X      0 is undefined/unknown position
 */
enum BVChromaLocation {
    BVCHROMA_LOC_UNSPECIFIED = 0,
    BVCHROMA_LOC_LEFT        = 1, ///< mpeg2/4, h264 default
    BVCHROMA_LOC_CENTER      = 2, ///< mpeg1, jpeg, h263
    BVCHROMA_LOC_TOPLEFT     = 3, ///< DV
    BVCHROMA_LOC_TOP         = 4,
    BVCHROMA_LOC_BOTTOMLEFT  = 5,
    BVCHROMA_LOC_BOTTOM      = 6,
    BVCHROMA_LOC_NB,              ///< Not part of ABI
};

#endif /* BVUTIL_PIXFMT_H */
