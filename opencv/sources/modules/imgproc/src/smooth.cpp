/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "precomp.hpp"

/*
 * This file includes the code, contributed by Simon Perreault
 * (the function icvMedianBlur_8u_O1)
 *
 * Constant-time median filtering -- http://nomis80.org/ctmf.html
 * Copyright (C) 2006 Simon Perreault
 *
 * Contact:
 *  Laboratoire de vision et systemes numeriques
 *  Pavillon Adrien-Pouliot
 *  Universite Laval
 *  Sainte-Foy, Quebec, Canada
 *  G1K 7P4
 *
 *  perreaul@gel.ulaval.ca
 */

namespace cv
{

/****************************************************************************************\
                                         Box Filter
\****************************************************************************************/

template<typename T, typename ST> struct RowSum : public BaseRowFilter
{
    RowSum( int _ksize, int _anchor )
    {
        ksize = _ksize;
        anchor = _anchor;
    }

    void operator()(const uchar* src, uchar* dst, int width, int cn)
    {
        const T* S = (const T*)src;
        ST* D = (ST*)dst;
        int i = 0, k, ksz_cn = ksize*cn;

        width = (width - 1)*cn;
        for( k = 0; k < cn; k++, S++, D++ )
        {
            ST s = 0;
            for( i = 0; i < ksz_cn; i += cn )
                s += S[i];
            D[0] = s;
            for( i = 0; i < width; i += cn )
            {
                s += S[i + ksz_cn] - S[i];
                D[i+cn] = s;
            }
        }
    }
};


template<typename ST, typename T> struct ColumnSum : public BaseColumnFilter
{
    ColumnSum( int _ksize, int _anchor, double _scale )
    {
        ksize = _ksize;
        anchor = _anchor;
        scale = _scale;
        sumCount = 0;
    }

    void reset() { sumCount = 0; }

    void operator()(const uchar** src, uchar* dst, int dststep, int count, int width)
    {
        int i;
        ST* SUM;
        bool haveScale = scale != 1;
        double _scale = scale;

        if( width != (int)sum.size() )
        {
            sum.resize(width);
            sumCount = 0;
        }

        SUM = &sum[0];
        if( sumCount == 0 )
        {
            for( i = 0; i < width; i++ )
                SUM[i] = 0;
            for( ; sumCount < ksize - 1; sumCount++, src++ )
            {
                const ST* Sp = (const ST*)src[0];
                for( i = 0; i <= width - 2; i += 2 )
                {
                    ST s0 = SUM[i] + Sp[i], s1 = SUM[i+1] + Sp[i+1];
                    SUM[i] = s0; SUM[i+1] = s1;
                }

                for( ; i < width; i++ )
                    SUM[i] += Sp[i];
            }
        }
        else
        {
            CV_Assert( sumCount == ksize-1 );
            src += ksize-1;
        }

        for( ; count--; src++ )
        {
            const ST* Sp = (const ST*)src[0];
            const ST* Sm = (const ST*)src[1-ksize];
            T* D = (T*)dst;
            if( haveScale )
            {
                for( i = 0; i <= width - 2; i += 2 )
                {
                    ST s0 = SUM[i] + Sp[i], s1 = SUM[i+1] + Sp[i+1];
                    D[i] = saturate_cast<T>(s0*_scale);
                    D[i+1] = saturate_cast<T>(s1*_scale);
                    s0 -= Sm[i]; s1 -= Sm[i+1];
                    SUM[i] = s0; SUM[i+1] = s1;
                }

                for( ; i < width; i++ )
                {
                    ST s0 = SUM[i] + Sp[i];
                    D[i] = saturate_cast<T>(s0*_scale);
                    SUM[i] = s0 - Sm[i];
                }
            }
            else
            {
                for( i = 0; i <= width - 2; i += 2 )
                {
                    ST s0 = SUM[i] + Sp[i], s1 = SUM[i+1] + Sp[i+1];
                    D[i] = saturate_cast<T>(s0);
                    D[i+1] = saturate_cast<T>(s1);
                    s0 -= Sm[i]; s1 -= Sm[i+1];
                    SUM[i] = s0; SUM[i+1] = s1;
                }

                for( ; i < width; i++ )
                {
                    ST s0 = SUM[i] + Sp[i];
                    D[i] = saturate_cast<T>(s0);
                    SUM[i] = s0 - Sm[i];
                }
            }
            dst += dststep;
        }
    }

    double scale;
    int sumCount;
    vector<ST> sum;
};


template<> struct ColumnSum<int, uchar> : public BaseColumnFilter
{
    ColumnSum( int _ksize, int _anchor, double _scale )
    {
        ksize = _ksize;
        anchor = _anchor;
        scale = _scale;
        sumCount = 0;
    }

    void reset() { sumCount = 0; }

    void operator()(const uchar** src, uchar* dst, int dststep, int count, int width)
    {
        int i;
        int* SUM;
        bool haveScale = scale != 1;
        double _scale = scale;

        #if CV_SSE2
            bool haveSSE2 = checkHardwareSupport(CV_CPU_SSE2);
        #endif

        if( width != (int)sum.size() )
        {
            sum.resize(width);
            sumCount = 0;
        }

        SUM = &sum[0];
        if( sumCount == 0 )
        {
            memset((void*)SUM, 0, width*sizeof(int));
            for( ; sumCount < ksize - 1; sumCount++, src++ )
            {
                const int* Sp = (const int*)src[0];
                i = 0;
                #if CV_SSE2
                if(haveSSE2)
                {
                    for( ; i < width-4; i+=4 )
                    {
                        __m128i _sum = _mm_loadu_si128((const __m128i*)(SUM+i));
                        __m128i _sp = _mm_loadu_si128((const __m128i*)(Sp+i));
                        _mm_storeu_si128((__m128i*)(SUM+i),_mm_add_epi32(_sum, _sp));
                    }
                }
                #endif
                for( ; i < width; i++ )
                    SUM[i] += Sp[i];
            }
        }
        else
        {
            CV_Assert( sumCount == ksize-1 );
            src += ksize-1;
        }

        for( ; count--; src++ )
        {
            const int* Sp = (const int*)src[0];
            const int* Sm = (const int*)src[1-ksize];
            uchar* D = (uchar*)dst;
            if( haveScale )
            {
                i = 0;
                #if CV_SSE2
                if(haveSSE2)
                {
                    const __m128 scale4 = _mm_set1_ps((float)_scale);
                    for( ; i < width-8; i+=8 )
                    {
                        __m128i _sm  = _mm_loadu_si128((const __m128i*)(Sm+i));
                        __m128i _sm1  = _mm_loadu_si128((const __m128i*)(Sm+i+4));

                        __m128i _s0  = _mm_add_epi32(_mm_loadu_si128((const __m128i*)(SUM+i)),
                                                     _mm_loadu_si128((const __m128i*)(Sp+i)));
                        __m128i _s01  = _mm_add_epi32(_mm_loadu_si128((const __m128i*)(SUM+i+4)),
                                                      _mm_loadu_si128((const __m128i*)(Sp+i+4)));

                        __m128i _s0T = _mm_cvtps_epi32(_mm_mul_ps(scale4, _mm_cvtepi32_ps(_s0)));
                        __m128i _s0T1 = _mm_cvtps_epi32(_mm_mul_ps(scale4, _mm_cvtepi32_ps(_s01)));

                        _s0T = _mm_packs_epi32(_s0T, _s0T1);

                        _mm_storel_epi64((__m128i*)(D+i), _mm_packus_epi16(_s0T, _s0T));

                        _mm_storeu_si128((__m128i*)(SUM+i), _mm_sub_epi32(_s0,_sm));
                        _mm_storeu_si128((__m128i*)(SUM+i+4),_mm_sub_epi32(_s01,_sm1));
                    }
                }
                #endif
                for( ; i < width; i++ )
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i] = saturate_cast<uchar>(s0*_scale);
                    SUM[i] = s0 - Sm[i];
                }
            }
            else
            {
                i = 0;
                #if CV_SSE2
                if(haveSSE2)
                {
                    for( ; i < width-8; i+=8 )
                    {
                        __m128i _sm  = _mm_loadu_si128((const __m128i*)(Sm+i));
                        __m128i _sm1  = _mm_loadu_si128((const __m128i*)(Sm+i+4));

                        __m128i _s0  = _mm_add_epi32(_mm_loadu_si128((const __m128i*)(SUM+i)),
                                                     _mm_loadu_si128((const __m128i*)(Sp+i)));
                        __m128i _s01  = _mm_add_epi32(_mm_loadu_si128((const __m128i*)(SUM+i+4)),
                                                      _mm_loadu_si128((const __m128i*)(Sp+i+4)));

                        __m128i _s0T = _mm_packs_epi32(_s0, _s01);

                        _mm_storel_epi64((__m128i*)(D+i), _mm_packus_epi16(_s0T, _s0T));

                        _mm_storeu_si128((__m128i*)(SUM+i), _mm_sub_epi32(_s0,_sm));
                        _mm_storeu_si128((__m128i*)(SUM+i+4),_mm_sub_epi32(_s01,_sm1));
                    }
                }
                #endif

                for( ; i < width; i++ )
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i] = saturate_cast<uchar>(s0);
                    SUM[i] = s0 - Sm[i];
                }
            }
            dst += dststep;
        }
    }

    double scale;
    int sumCount;
    vector<int> sum;
};

template<> struct ColumnSum<int, short> : public BaseColumnFilter
{
    ColumnSum( int _ksize, int _anchor, double _scale )
    {
        ksize = _ksize;
        anchor = _anchor;
        scale = _scale;
        sumCount = 0;
    }

    void reset() { sumCount = 0; }

    void operator()(const uchar** src, uchar* dst, int dststep, int count, int width)
    {
        int i;
        int* SUM;
        bool haveScale = scale != 1;
        double _scale = scale;

        #if CV_SSE2
            bool haveSSE2 = checkHardwareSupport(CV_CPU_SSE2);
        #endif

        if( width != (int)sum.size() )
        {
            sum.resize(width);
            sumCount = 0;
        }
        SUM = &sum[0];
        if( sumCount == 0 )
        {
            memset((void*)SUM, 0, width*sizeof(int));
            for( ; sumCount < ksize - 1; sumCount++, src++ )
            {
                const int* Sp = (const int*)src[0];
                i = 0;
                #if CV_SSE2
                if(haveSSE2)
                {
                    for( ; i < width-4; i+=4 )
                    {
                        __m128i _sum = _mm_loadu_si128((const __m128i*)(SUM+i));
                        __m128i _sp = _mm_loadu_si128((const __m128i*)(Sp+i));
                        _mm_storeu_si128((__m128i*)(SUM+i),_mm_add_epi32(_sum, _sp));
                    }
                }
                #endif
                for( ; i < width; i++ )
                    SUM[i] += Sp[i];
            }
        }
        else
        {
            CV_Assert( sumCount == ksize-1 );
            src += ksize-1;
        }

        for( ; count--; src++ )
        {
            const int* Sp = (const int*)src[0];
            const int* Sm = (const int*)src[1-ksize];
            short* D = (short*)dst;
            if( haveScale )
            {
                i = 0;
                #if CV_SSE2
                if(haveSSE2)
                {
                    const __m128 scale4 = _mm_set1_ps((float)_scale);
                    for( ; i < width-8; i+=8 )
                    {
                        __m128i _sm   = _mm_loadu_si128((const __m128i*)(Sm+i));
                        __m128i _sm1  = _mm_loadu_si128((const __m128i*)(Sm+i+4));

                        __m128i _s0  = _mm_add_epi32(_mm_loadu_si128((const __m128i*)(SUM+i)),
                                                     _mm_loadu_si128((const __m128i*)(Sp+i)));
                        __m128i _s01  = _mm_add_epi32(_mm_loadu_si128((const __m128i*)(SUM+i+4)),
                                                      _mm_loadu_si128((const __m128i*)(Sp+i+4)));

                        __m128i _s0T  = _mm_cvtps_epi32(_mm_mul_ps(scale4, _mm_cvtepi32_ps(_s0)));
                        __m128i _s0T1 = _mm_cvtps_epi32(_mm_mul_ps(scale4, _mm_cvtepi32_ps(_s01)));

                        _mm_storeu_si128((__m128i*)(D+i), _mm_packs_epi32(_s0T, _s0T1));

                        _mm_storeu_si128((__m128i*)(SUM+i),_mm_sub_epi32(_s0,_sm));
                        _mm_storeu_si128((__m128i*)(SUM+i+4), _mm_sub_epi32(_s01,_sm1));
                    }
                }
                #endif
                for( ; i < width; i++ )
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i] = saturate_cast<short>(s0*_scale);
                    SUM[i] = s0 - Sm[i];
                }
            }
            else
            {
                i = 0;
                #if CV_SSE2
                if(haveSSE2)
                {
                    for( ; i < width-8; i+=8 )
                    {

                        __m128i _sm  = _mm_loadu_si128((const __m128i*)(Sm+i));
                        __m128i _sm1  = _mm_loadu_si128((const __m128i*)(Sm+i+4));

                        __m128i _s0  = _mm_add_epi32(_mm_loadu_si128((const __m128i*)(SUM+i)),
                                                     _mm_loadu_si128((const __m128i*)(Sp+i)));
                        __m128i _s01  = _mm_add_epi32(_mm_loadu_si128((const __m128i*)(SUM+i+4)),
                                                      _mm_loadu_si128((const __m128i*)(Sp+i+4)));

                        _mm_storeu_si128((__m128i*)(D+i), _mm_packs_epi32(_s0, _s01));

                        _mm_storeu_si128((__m128i*)(SUM+i), _mm_sub_epi32(_s0,_sm));
                        _mm_storeu_si128((__m128i*)(SUM+i+4),_mm_sub_epi32(_s01,_sm1));
                    }
                }
                #endif

                for( ; i < width; i++ )
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i] = saturate_cast<short>(s0);
                    SUM[i] = s0 - Sm[i];
                }
            }
            dst += dststep;
        }
    }

    double scale;
    int sumCount;
    vector<int> sum;
};


template<> struct ColumnSum<int, ushort> : public BaseColumnFilter
{
    ColumnSum( int _ksize, int _anchor, double _scale )
    {
        ksize = _ksize;
        anchor = _anchor;
        scale = _scale;
        sumCount = 0;
    }

    void reset() { sumCount = 0; }

    void operator()(const uchar** src, uchar* dst, int dststep, int count, int width)
    {
        int i;
        int* SUM;
        bool haveScale = scale != 1;
        double _scale = scale;
        #if CV_SSE2
                bool haveSSE2 =  checkHardwareSupport(CV_CPU_SSE2);
        #endif

        if( width != (int)sum.size() )
        {
            sum.resize(width);
            sumCount = 0;
        }
        SUM = &sum[0];
        if( sumCount == 0 )
        {
            memset((void*)SUM, 0, width*sizeof(int));
            for( ; sumCount < ksize - 1; sumCount++, src++ )
            {
                const int* Sp = (const int*)src[0];
                i = 0;
                #if CV_SSE2
                if(haveSSE2)
                {
                    for( ; i < width-4; i+=4 )
                    {
                        __m128i _sum = _mm_loadu_si128((const __m128i*)(SUM+i));
                        __m128i _sp = _mm_loadu_si128((const __m128i*)(Sp+i));
                        _mm_storeu_si128((__m128i*)(SUM+i), _mm_add_epi32(_sum, _sp));
                    }
                }
                #endif
                for( ; i < width; i++ )
                    SUM[i] += Sp[i];
            }
        }
        else
        {
            CV_Assert( sumCount == ksize-1 );
            src += ksize-1;
        }

        for( ; count--; src++ )
        {
            const int* Sp = (const int*)src[0];
            const int* Sm = (const int*)src[1-ksize];
            ushort* D = (ushort*)dst;
            if( haveScale )
            {
                i = 0;
                #if CV_SSE2
                if(haveSSE2)
                {
                    const __m128 scale4 = _mm_set1_ps((float)_scale);
                    const __m128i delta0 = _mm_set1_epi32(0x8000);
                    const __m128i delta1 = _mm_set1_epi32(0x80008000);

                    for( ; i < width-4; i+=4)
                    {
                        __m128i _sm   = _mm_loadu_si128((const __m128i*)(Sm+i));
                        __m128i _s0   = _mm_add_epi32(_mm_loadu_si128((const __m128i*)(SUM+i)),
                                                      _mm_loadu_si128((const __m128i*)(Sp+i)));

                        __m128i _res = _mm_cvtps_epi32(_mm_mul_ps(scale4, _mm_cvtepi32_ps(_s0)));

                        _res = _mm_sub_epi32(_res, delta0);
                        _res = _mm_add_epi16(_mm_packs_epi32(_res, _res), delta1);

                        _mm_storel_epi64((__m128i*)(D+i), _res);
                        _mm_storeu_si128((__m128i*)(SUM+i), _mm_sub_epi32(_s0,_sm));
                    }
                }
                #endif
                for( ; i < width; i++ )
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i] = saturate_cast<ushort>(s0*_scale);
                    SUM[i] = s0 - Sm[i];
                }
            }
            else
            {
                i = 0;
                #if  CV_SSE2
                if(haveSSE2)
                {
                    const __m128i delta0 = _mm_set1_epi32(0x8000);
                    const __m128i delta1 = _mm_set1_epi32(0x80008000);

                    for( ; i < width-4; i+=4 )
                    {
                        __m128i _sm   = _mm_loadu_si128((const __m128i*)(Sm+i));
                        __m128i _s0   = _mm_add_epi32(_mm_loadu_si128((const __m128i*)(SUM+i)),
                                                      _mm_loadu_si128((const __m128i*)(Sp+i)));

                        __m128i _res = _mm_sub_epi32(_s0, delta0);
                        _res = _mm_add_epi16(_mm_packs_epi32(_res, _res), delta1);

                        _mm_storel_epi64((__m128i*)(D+i), _res);
                        _mm_storeu_si128((__m128i*)(SUM+i), _mm_sub_epi32(_s0,_sm));
                    }
                }
                #endif

                for( ; i < width; i++ )
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i] = saturate_cast<ushort>(s0);
                    SUM[i] = s0 - Sm[i];
                }
            }
            dst += dststep;
        }
    }

    double scale;
    int sumCount;
    vector<int> sum;
};


}

cv::Ptr<cv::BaseRowFilter> cv::getRowSumFilter(int srcType, int sumType, int ksize, int anchor)
{
    int sdepth = CV_MAT_DEPTH(srcType), ddepth = CV_MAT_DEPTH(sumType);
    CV_Assert( CV_MAT_CN(sumType) == CV_MAT_CN(srcType) );

    if( anchor < 0 )
        anchor = ksize/2;

    if( sdepth == CV_8U && ddepth == CV_32S )
        return Ptr<BaseRowFilter>(new RowSum<uchar, int>(ksize, anchor));
    if( sdepth == CV_8U && ddepth == CV_64F )
        return Ptr<BaseRowFilter>(new RowSum<uchar, double>(ksize, anchor));
    if( sdepth == CV_16U && ddepth == CV_32S )
        return Ptr<BaseRowFilter>(new RowSum<ushort, int>(ksize, anchor));
    if( sdepth == CV_16U && ddepth == CV_64F )
        return Ptr<BaseRowFilter>(new RowSum<ushort, double>(ksize, anchor));
    if( sdepth == CV_16S && ddepth == CV_32S )
        return Ptr<BaseRowFilter>(new RowSum<short, int>(ksize, anchor));
    if( sdepth == CV_32S && ddepth == CV_32S )
        return Ptr<BaseRowFilter>(new RowSum<int, int>(ksize, anchor));
    if( sdepth == CV_16S && ddepth == CV_64F )
        return Ptr<BaseRowFilter>(new RowSum<short, double>(ksize, anchor));
    if( sdepth == CV_32F && ddepth == CV_64F )
        return Ptr<BaseRowFilter>(new RowSum<float, double>(ksize, anchor));
    if( sdepth == CV_64F && ddepth == CV_64F )
        return Ptr<BaseRowFilter>(new RowSum<double, double>(ksize, anchor));

    CV_Error_( CV_StsNotImplemented,
        ("Unsupported combination of source format (=%d), and buffer format (=%d)",
        srcType, sumType));

    return Ptr<BaseRowFilter>(0);
}


cv::Ptr<cv::BaseColumnFilter> cv::getColumnSumFilter(int sumType, int dstType, int ksize,
                                                     int anchor, double scale)
{
    int sdepth = CV_MAT_DEPTH(sumType), ddepth = CV_MAT_DEPTH(dstType);
    CV_Assert( CV_MAT_CN(sumType) == CV_MAT_CN(dstType) );

    if( anchor < 0 )
        anchor = ksize/2;

    if( ddepth == CV_8U && sdepth == CV_32S )
        return Ptr<BaseColumnFilter>(new ColumnSum<int, uchar>(ksize, anchor, scale));
    if( ddepth == CV_8U && sdepth == CV_64F )
        return Ptr<BaseColumnFilter>(new ColumnSum<double, uchar>(ksize, anchor, scale));
    if( ddepth == CV_16U && sdepth == CV_32S )
        return Ptr<BaseColumnFilter>(new ColumnSum<int, ushort>(ksize, anchor, scale));
    if( ddepth == CV_16U && sdepth == CV_64F )
        return Ptr<BaseColumnFilter>(new ColumnSum<double, ushort>(ksize, anchor, scale));
    if( ddepth == CV_16S && sdepth == CV_32S )
        return Ptr<BaseColumnFilter>(new ColumnSum<int, short>(ksize, anchor, scale));
    if( ddepth == CV_16S && sdepth == CV_64F )
        return Ptr<BaseColumnFilter>(new ColumnSum<double, short>(ksize, anchor, scale));
    if( ddepth == CV_32S && sdepth == CV_32S )
        return Ptr<BaseColumnFilter>(new ColumnSum<int, int>(ksize, anchor, scale));
    if( ddepth == CV_32F && sdepth == CV_32S )
        return Ptr<BaseColumnFilter>(new ColumnSum<int, float>(ksize, anchor, scale));
    if( ddepth == CV_32F && sdepth == CV_64F )
        return Ptr<BaseColumnFilter>(new ColumnSum<double, float>(ksize, anchor, scale));
    if( ddepth == CV_64F && sdepth == CV_32S )
        return Ptr<BaseColumnFilter>(new ColumnSum<int, double>(ksize, anchor, scale));
    if( ddepth == CV_64F && sdepth == CV_64F )
        return Ptr<BaseColumnFilter>(new ColumnSum<double, double>(ksize, anchor, scale));

    CV_Error_( CV_StsNotImplemented,
        ("Unsupported combination of sum format (=%d), and destination format (=%d)",
        sumType, dstType));

    return Ptr<BaseColumnFilter>(0);
}


cv::Ptr<cv::FilterEngine> cv::createBoxFilter( int srcType, int dstType, Size ksize,
                    Point anchor, bool normalize, int borderType )
{
    int sdepth = CV_MAT_DEPTH(srcType);
    int cn = CV_MAT_CN(srcType), sumType = CV_64F;
    if( sdepth <= CV_32S && (!normalize ||
        ksize.width*ksize.height <= (sdepth == CV_8U ? (1<<23) :
            sdepth == CV_16U ? (1 << 15) : (1 << 16))) )
        sumType = CV_32S;
    sumType = CV_MAKETYPE( sumType, cn );

    Ptr<BaseRowFilter> rowFilter = getRowSumFilter(srcType, sumType, ksize.width, anchor.x );
    Ptr<BaseColumnFilter> columnFilter = getColumnSumFilter(sumType,
        dstType, ksize.height, anchor.y, normalize ? 1./(ksize.width*ksize.height) : 1);

    return Ptr<FilterEngine>(new FilterEngine(Ptr<BaseFilter>(0), rowFilter, columnFilter,
           srcType, dstType, sumType, borderType ));
}


void cv::boxFilter( InputArray _src, OutputArray _dst, int ddepth,
                Size ksize, Point anchor,
                bool normalize, int borderType )
{
    Mat src = _src.getMat();
    int sdepth = src.depth(), cn = src.channels();
    if( ddepth < 0 )
        ddepth = sdepth;
    _dst.create( src.size(), CV_MAKETYPE(ddepth, cn) );
    Mat dst = _dst.getMat();
    if( borderType != BORDER_CONSTANT && normalize && (borderType & BORDER_ISOLATED) != 0 )
    {
        if( src.rows == 1 )
            ksize.height = 1;
        if( src.cols == 1 )
            ksize.width = 1;
    }
#ifdef HAVE_TEGRA_OPTIMIZATION
    if ( tegra::box(src, dst, ksize, anchor, normalize, borderType) )
        return;
#endif

    Ptr<FilterEngine> f = createBoxFilter( src.type(), dst.type(),
                        ksize, anchor, normalize, borderType );
    f->apply( src, dst );
}

void cv::blur( InputArray src, OutputArray dst,
           Size ksize, Point anchor, int borderType )
{
    boxFilter( src, dst, -1, ksize, anchor, true, borderType );
}

/****************************************************************************************\
                                     Gaussian Blur
\****************************************************************************************/

cv::Mat cv::getGaussianKernel( int n, double sigma, int ktype )
{
    const int SMALL_GAUSSIAN_SIZE = 7;
    static const float small_gaussian_tab[][SMALL_GAUSSIAN_SIZE] =
    {
        {1.f},
        {0.25f, 0.5f, 0.25f},
        {0.0625f, 0.25f, 0.375f, 0.25f, 0.0625f},
        {0.03125f, 0.109375f, 0.21875f, 0.28125f, 0.21875f, 0.109375f, 0.03125f}
    };

    const float* fixed_kernel = n % 2 == 1 && n <= SMALL_GAUSSIAN_SIZE && sigma <= 0 ?
        small_gaussian_tab[n>>1] : 0;

    CV_Assert( ktype == CV_32F || ktype == CV_64F );
    Mat kernel(n, 1, ktype);
    float* cf = (float*)kernel.data;
    double* cd = (double*)kernel.data;

    double sigmaX = sigma > 0 ? sigma : ((n-1)*0.5 - 1)*0.3 + 0.8;
    double scale2X = -0.5/(sigmaX*sigmaX);
    double sum = 0;

    int i;
    for( i = 0; i < n; i++ )
    {
        double x = i - (n-1)*0.5;
        double t = fixed_kernel ? (double)fixed_kernel[i] : std::exp(scale2X*x*x);
        if( ktype == CV_32F )
        {
            cf[i] = (float)t;
            sum += cf[i];
        }
        else
        {
            cd[i] = t;
            sum += cd[i];
        }
    }

    sum = 1./sum;
    for( i = 0; i < n; i++ )
    {
        if( ktype == CV_32F )
            cf[i] = (float)(cf[i]*sum);
        else
            cd[i] *= sum;
    }

    return kernel;
}


cv::Ptr<cv::FilterEngine> cv::createGaussianFilter( int type, Size ksize,
                                        double sigma1, double sigma2,
                                        int borderType )
{
    int depth = CV_MAT_DEPTH(type);
    if( sigma2 <= 0 )
        sigma2 = sigma1;

    // automatic detection of kernel size from sigma
    if( ksize.width <= 0 && sigma1 > 0 )
        ksize.width = cvRound(sigma1*(depth == CV_8U ? 3 : 4)*2 + 1)|1;
    if( ksize.height <= 0 && sigma2 > 0 )
        ksize.height = cvRound(sigma2*(depth == CV_8U ? 3 : 4)*2 + 1)|1;

    CV_Assert( ksize.width > 0 && ksize.width % 2 == 1 &&
        ksize.height > 0 && ksize.height % 2 == 1 );

    sigma1 = std::max( sigma1, 0. );
    sigma2 = std::max( sigma2, 0. );

    Mat kx = getGaussianKernel( ksize.width, sigma1, std::max(depth, CV_32F) );
    Mat ky;
    if( ksize.height == ksize.width && std::abs(sigma1 - sigma2) < DBL_EPSILON )
        ky = kx;
    else
        ky = getGaussianKernel( ksize.height, sigma2, std::max(depth, CV_32F) );

    return createSeparableLinearFilter( type, type, kx, ky, Point(-1,-1), 0, borderType );
}


void cv::GaussianBlur( InputArray _src, OutputArray _dst, Size ksize,
                   double sigma1, double sigma2,
                   int borderType )
{
    Mat src = _src.getMat();
    _dst.create( src.size(), src.type() );
    Mat dst = _dst.getMat();

    if( borderType != BORDER_CONSTANT && (borderType & BORDER_ISOLATED) != 0 )
    {
        if( src.rows == 1 )
            ksize.height = 1;
        if( src.cols == 1 )
            ksize.width = 1;
    }

    if( ksize.width == 1 && ksize.height == 1 )
    {
        src.copyTo(dst);
        return;
    }

#ifdef HAVE_TEGRA_OPTIMIZATION
    if(sigma1 == 0 && sigma2 == 0 && tegra::gaussian(src, dst, ksize, borderType))
        return;
#endif

#if defined HAVE_IPP && (IPP_VERSION_MAJOR >= 7)
    if(src.type() == CV_32FC1 && sigma1 == sigma2 && ksize.width == ksize.height && sigma1 != 0.0 )
    {
        IppiSize roi = {src.cols, src.rows};
        int bufSize = 0;
        ippiFilterGaussGetBufferSize_32f_C1R(roi, ksize.width, &bufSize);
        AutoBuffer<uchar> buf(bufSize+128);
        if( ippiFilterGaussBorder_32f_C1R((const Ipp32f *)src.data, (int)src.step,
                                          (Ipp32f *)dst.data, (int)dst.step,
                                          roi, ksize.width, (Ipp32f)sigma1,
                                          (IppiBorderType)borderType, 0.0,
                                          alignPtr(&buf[0],32)) >= 0 )
            return;
    }
#endif

    Ptr<FilterEngine> f = createGaussianFilter( src.type(), ksize, sigma1, sigma2, borderType );
    f->apply( src, dst );
}


/****************************************************************************************\
                                      Median Filter
\****************************************************************************************/

namespace cv
{
typedef ushort HT;

/**
 * This structure represents a two-tier histogram. The first tier (known as the
 * "coarse" level) is 4 bit wide and the second tier (known as the "fine" level)
 * is 8 bit wide. Pixels inserted in the fine level also get inserted into the
 * coarse bucket designated by the 4 MSBs of the fine bucket value.
 *
 * The structure is aligned on 16 bits, which is a prerequisite for SIMD
 * instructions. Each bucket is 16 bit wide, which means that extra care must be
 * taken to prevent overflow.
 */
typedef struct
{
    HT coarse[16];
    HT fine[16][16];
} Histogram;


#if CV_SSE2
#define MEDIAN_HAVE_SIMD 1

static inline void histogram_add_simd( const HT x[16], HT y[16] )
{
    const __m128i* rx = (const __m128i*)x;
    __m128i* ry = (__m128i*)y;
    __m128i r0 = _mm_add_epi16(_mm_load_si128(ry+0),_mm_load_si128(rx+0));
    __m128i r1 = _mm_add_epi16(_mm_load_si128(ry+1),_mm_load_si128(rx+1));
    _mm_store_si128(ry+0, r0);
    _mm_store_si128(ry+1, r1);
}

static inline void histogram_sub_simd( const HT x[16], HT y[16] )
{
    const __m128i* rx = (const __m128i*)x;
    __m128i* ry = (__m128i*)y;
    __m128i r0 = _mm_sub_epi16(_mm_load_si128(ry+0),_mm_load_si128(rx+0));
    __m128i r1 = _mm_sub_epi16(_mm_load_si128(ry+1),_mm_load_si128(rx+1));
    _mm_store_si128(ry+0, r0);
    _mm_store_si128(ry+1, r1);
}

#else
#define MEDIAN_HAVE_SIMD 0
#endif


static inline void histogram_add( const HT x[16], HT y[16] )
{
    int i;
    for( i = 0; i < 16; ++i )
        y[i] = (HT)(y[i] + x[i]);
}

static inline void histogram_sub( const HT x[16], HT y[16] )
{
    int i;
    for( i = 0; i < 16; ++i )
        y[i] = (HT)(y[i] - x[i]);
}

static inline void histogram_muladd( int a, const HT x[16],
        HT y[16] )
{
    for( int i = 0; i < 16; ++i )
        y[i] = (HT)(y[i] + a * x[i]);
}

static void
medianBlur_8u_O1( const Mat& _src, Mat& _dst, int ksize )
{
/**
 * HOP is short for Histogram OPeration. This macro makes an operation \a op on
 * histogram \a h for pixel value \a x. It takes care of handling both levels.
 */
#define HOP(h,x,op) \
    h.coarse[x>>4] op, \
    *((HT*)h.fine + x) op

#define COP(c,j,x,op) \
    h_coarse[ 16*(n*c+j) + (x>>4) ] op, \
    h_fine[ 16 * (n*(16*c+(x>>4)) + j) + (x & 0xF) ] op

    int cn = _dst.channels(), m = _dst.rows, r = (ksize-1)/2;
    size_t sstep = _src.step, dstep = _dst.step;
    Histogram CV_DECL_ALIGNED(16) H[4];
    HT CV_DECL_ALIGNED(16) luc[4][16];

    int STRIPE_SIZE = std::min( _dst.cols, 512/cn );

    vector<HT> _h_coarse(1 * 16 * (STRIPE_SIZE + 2*r) * cn + 16);
    vector<HT> _h_fine(16 * 16 * (STRIPE_SIZE + 2*r) * cn + 16);
    HT* h_coarse = alignPtr(&_h_coarse[0], 16);
    HT* h_fine = alignPtr(&_h_fine[0], 16);
#if MEDIAN_HAVE_SIMD
    volatile bool useSIMD = checkHardwareSupport(CV_CPU_SSE2);
#endif

    for( int x = 0; x < _dst.cols; x += STRIPE_SIZE )
    {
        int i, j, k, c, n = std::min(_dst.cols - x, STRIPE_SIZE) + r*2;
        const uchar* src = _src.data + x*cn;
        uchar* dst = _dst.data + (x - r)*cn;

        memset( h_coarse, 0, 16*n*cn*sizeof(h_coarse[0]) );
        memset( h_fine, 0, 16*16*n*cn*sizeof(h_fine[0]) );

        // First row initialization
        for( c = 0; c < cn; c++ )
        {
            for( j = 0; j < n; j++ )
                COP( c, j, src[cn*j+c], += (cv::HT)(r+2) );

            for( i = 1; i < r; i++ )
            {
                const uchar* p = src + sstep*std::min(i, m-1);
                for ( j = 0; j < n; j++ )
                    COP( c, j, p[cn*j+c], ++ );
            }
        }

        for( i = 0; i < m; i++ )
        {
            const uchar* p0 = src + sstep * std::max( 0, i-r-1 );
            const uchar* p1 = src + sstep * std::min( m-1, i+r );

            memset( H, 0, cn*sizeof(H[0]) );
            memset( luc, 0, cn*sizeof(luc[0]) );
            for( c = 0; c < cn; c++ )
            {
                // Update column histograms for the entire row.
                for( j = 0; j < n; j++ )
                {
                    COP( c, j, p0[j*cn + c], -- );
                    COP( c, j, p1[j*cn + c], ++ );
                }

                // First column initialization
                for( k = 0; k < 16; ++k )
                    histogram_muladd( 2*r+1, &h_fine[16*n*(16*c+k)], &H[c].fine[k][0] );

            #if MEDIAN_HAVE_SIMD
                if( useSIMD )
                {
                    for( j = 0; j < 2*r; ++j )
                        histogram_add_simd( &h_coarse[16*(n*c+j)], H[c].coarse );

                    for( j = r; j < n-r; j++ )
                    {
                        int t = 2*r*r + 2*r, b, sum = 0;
                        HT* segment;

                        histogram_add_simd( &h_coarse[16*(n*c + std::min(j+r,n-1))], H[c].coarse );

                        // Find median at coarse level
                        for ( k = 0; k < 16 ; ++k )
                        {
                            sum += H[c].coarse[k];
                            if ( sum > t )
                            {
                                sum -= H[c].coarse[k];
                                break;
                            }
                        }
                        assert( k < 16 );

                        /* Update corresponding histogram segment */
                        if ( luc[c][k] <= j-r )
                        {
                            memset( &H[c].fine[k], 0, 16 * sizeof(HT) );
                            for ( luc[c][k] = cv::HT(j-r); luc[c][k] < MIN(j+r+1,n); ++luc[c][k] )
                                histogram_add_simd( &h_fine[16*(n*(16*c+k)+luc[c][k])], H[c].fine[k] );

                            if ( luc[c][k] < j+r+1 )
                            {
                                histogram_muladd( j+r+1 - n, &h_fine[16*(n*(16*c+k)+(n-1))], &H[c].fine[k][0] );
                                luc[c][k] = (HT)(j+r+1);
                            }
                        }
                        else
                        {
                            for ( ; luc[c][k] < j+r+1; ++luc[c][k] )
                            {
                                histogram_sub_simd( &h_fine[16*(n*(16*c+k)+MAX(luc[c][k]-2*r-1,0))], H[c].fine[k] );
                                histogram_add_simd( &h_fine[16*(n*(16*c+k)+MIN(luc[c][k],n-1))], H[c].fine[k] );
                            }
                        }

                        histogram_sub_simd( &h_coarse[16*(n*c+MAX(j-r,0))], H[c].coarse );

                        /* Find median in segment */
                        segment = H[c].fine[k];
                        for ( b = 0; b < 16 ; b++ )
                        {
                            sum += segment[b];
                            if ( sum > t )
                            {
                                dst[dstep*i+cn*j+c] = (uchar)(16*k + b);
                                break;
                            }
                        }
                        assert( b < 16 );
                    }
                }
                else
            #endif
                {
                    for( j = 0; j < 2*r; ++j )
                        histogram_add( &h_coarse[16*(n*c+j)], H[c].coarse );

                    for( j = r; j < n-r; j++ )
                    {
                        int t = 2*r*r + 2*r, b, sum = 0;
                        HT* segment;

                        histogram_add( &h_coarse[16*(n*c + std::min(j+r,n-1))], H[c].coarse );

                        // Find median at coarse level
                        for ( k = 0; k < 16 ; ++k )
                        {
                            sum += H[c].coarse[k];
                            if ( sum > t )
                            {
                                sum -= H[c].coarse[k];
                                break;
                            }
                        }
                        assert( k < 16 );

                        /* Update corresponding histogram segment */
                        if ( luc[c][k] <= j-r )
                        {
                            memset( &H[c].fine[k], 0, 16 * sizeof(HT) );
                            for ( luc[c][k] = cv::HT(j-r); luc[c][k] < MIN(j+r+1,n); ++luc[c][k] )
                                histogram_add( &h_fine[16*(n*(16*c+k)+luc[c][k])], H[c].fine[k] );

                            if ( luc[c][k] < j+r+1 )
                            {
                                histogram_muladd( j+r+1 - n, &h_fine[16*(n*(16*c+k)+(n-1))], &H[c].fine[k][0] );
                                luc[c][k] = (HT)(j+r+1);
                            }
                        }
                        else
                        {
                            for ( ; luc[c][k] < j+r+1; ++luc[c][k] )
                            {
                                histogram_sub( &h_fine[16*(n*(16*c+k)+MAX(luc[c][k]-2*r-1,0))], H[c].fine[k] );
                                histogram_add( &h_fine[16*(n*(16*c+k)+MIN(luc[c][k],n-1))], H[c].fine[k] );
                            }
                        }

                        histogram_sub( &h_coarse[16*(n*c+MAX(j-r,0))], H[c].coarse );

                        /* Find median in segment */
                        segment = H[c].fine[k];
                        for ( b = 0; b < 16 ; b++ )
                        {
                            sum += segment[b];
                            if ( sum > t )
                            {
                                dst[dstep*i+cn*j+c] = (uchar)(16*k + b);
                                break;
                            }
                        }
                        assert( b < 16 );
                    }
                }
            }
        }
    }

#undef HOP
#undef COP
}

static void
medianBlur_8u_Om( const Mat& _src, Mat& _dst, int m )
{
    #define N  16
    int     zone0[4][N];
    int     zone1[4][N*N];
    int     x, y;
    int     n2 = m*m/2;
    Size    size = _dst.size();
    const uchar* src = _src.data;
    uchar*  dst = _dst.data;
    int     src_step = (int)_src.step, dst_step = (int)_dst.step;
    int     cn = _src.channels();
    const uchar*  src_max = src + size.height*src_step;

    #define UPDATE_ACC01( pix, cn, op ) \
    {                                   \
        int p = (pix);                  \
        zone1[cn][p] op;                \
        zone0[cn][p >> 4] op;           \
    }

    //CV_Assert( size.height >= nx && size.width >= nx );
    for( x = 0; x < size.width; x++, src += cn, dst += cn )
    {
        uchar* dst_cur = dst;
        const uchar* src_top = src;
        const uchar* src_bottom = src;
        int k, c;
        int src_step1 = src_step, dst_step1 = dst_step;

        if( x % 2 != 0 )
        {
            src_bottom = src_top += src_step*(size.height-1);
            dst_cur += dst_step*(size.height-1);
            src_step1 = -src_step1;
            dst_step1 = -dst_step1;
        }

        // init accumulator
        memset( zone0, 0, sizeof(zone0[0])*cn );
        memset( zone1, 0, sizeof(zone1[0])*cn );

        for( y = 0; y <= m/2; y++ )
        {
            for( c = 0; c < cn; c++ )
            {
                if( y > 0 )
                {
                    for( k = 0; k < m*cn; k += cn )
                        UPDATE_ACC01( src_bottom[k+c], c, ++ );
                }
                else
                {
                    for( k = 0; k < m*cn; k += cn )
                        UPDATE_ACC01( src_bottom[k+c], c, += m/2+1 );
                }
            }

            if( (src_step1 > 0 && y < size.height-1) ||
                (src_step1 < 0 && size.height-y-1 > 0) )
                src_bottom += src_step1;
        }

        for( y = 0; y < size.height; y++, dst_cur += dst_step1 )
        {
            // find median
            for( c = 0; c < cn; c++ )
            {
                int s = 0;
                for( k = 0; ; k++ )
                {
                    int t = s + zone0[c][k];
                    if( t > n2 ) break;
                    s = t;
                }

                for( k *= N; ;k++ )
                {
                    s += zone1[c][k];
                    if( s > n2 ) break;
                }

                dst_cur[c] = (uchar)k;
            }

            if( y+1 == size.height )
                break;

            if( cn == 1 )
            {
                for( k = 0; k < m; k++ )
                {
                    int p = src_top[k];
                    int q = src_bottom[k];
                    zone1[0][p]--;
                    zone0[0][p>>4]--;
                    zone1[0][q]++;
                    zone0[0][q>>4]++;
                }
            }
            else if( cn == 3 )
            {
                for( k = 0; k < m*3; k += 3 )
                {
                    UPDATE_ACC01( src_top[k], 0, -- );
                    UPDATE_ACC01( src_top[k+1], 1, -- );
                    UPDATE_ACC01( src_top[k+2], 2, -- );

                    UPDATE_ACC01( src_bottom[k], 0, ++ );
                    UPDATE_ACC01( src_bottom[k+1], 1, ++ );
                    UPDATE_ACC01( src_bottom[k+2], 2, ++ );
                }
            }
            else
            {
                assert( cn == 4 );
                for( k = 0; k < m*4; k += 4 )
                {
                    UPDATE_ACC01( src_top[k], 0, -- );
                    UPDATE_ACC01( src_top[k+1], 1, -- );
                    UPDATE_ACC01( src_top[k+2], 2, -- );
                    UPDATE_ACC01( src_top[k+3], 3, -- );

                    UPDATE_ACC01( src_bottom[k], 0, ++ );
                    UPDATE_ACC01( src_bottom[k+1], 1, ++ );
                    UPDATE_ACC01( src_bottom[k+2], 2, ++ );
                    UPDATE_ACC01( src_bottom[k+3], 3, ++ );
                }
            }

            if( (src_step1 > 0 && src_bottom + src_step1 < src_max) ||
                (src_step1 < 0 && src_bottom + src_step1 >= src) )
                src_bottom += src_step1;

            if( y >= m/2 )
                src_top += src_step1;
        }
    }
#undef N
#undef UPDATE_ACC
}


struct MinMax8u
{
    typedef uchar value_type;
    typedef int arg_type;
    enum { SIZE = 1 };
    arg_type load(const uchar* ptr) { return *ptr; }
    void store(uchar* ptr, arg_type val) { *ptr = (uchar)val; }
    void operator()(arg_type& a, arg_type& b) const
    {
        int t = CV_FAST_CAST_8U(a - b);
        b += t; a -= t;
    }
};

struct MinMax16u
{
    typedef ushort value_type;
    typedef int arg_type;
    enum { SIZE = 1 };
    arg_type load(const ushort* ptr) { return *ptr; }
    void store(ushort* ptr, arg_type val) { *ptr = (ushort)val; }
    void operator()(arg_type& a, arg_type& b) const
    {
        arg_type t = a;
        a = std::min(a, b);
        b = std::max(b, t);
    }
};

struct MinMax16s
{
    typedef short value_type;
    typedef int arg_type;
    enum { SIZE = 1 };
    arg_type load(const short* ptr) { return *ptr; }
    void store(short* ptr, arg_type val) { *ptr = (short)val; }
    void operator()(arg_type& a, arg_type& b) const
    {
        arg_type t = a;
        a = std::min(a, b);
        b = std::max(b, t);
    }
};

struct MinMax32f
{
    typedef float value_type;
    typedef float arg_type;
    enum { SIZE = 1 };
    arg_type load(const float* ptr) { return *ptr; }
    void store(float* ptr, arg_type val) { *ptr = val; }
    void operator()(arg_type& a, arg_type& b) const
    {
        arg_type t = a;
        a = std::min(a, b);
        b = std::max(b, t);
    }
};

#if CV_SSE2

struct MinMaxVec8u
{
    typedef uchar value_type;
    typedef __m128i arg_type;
    enum { SIZE = 16 };
    arg_type load(const uchar* ptr) { return _mm_loadu_si128((const __m128i*)ptr); }
    void store(uchar* ptr, arg_type val) { _mm_storeu_si128((__m128i*)ptr, val); }
    void operator()(arg_type& a, arg_type& b) const
    {
        arg_type t = a;
        a = _mm_min_epu8(a, b);
        b = _mm_max_epu8(b, t);
    }
};


struct MinMaxVec16u
{
    typedef ushort value_type;
    typedef __m128i arg_type;
    enum { SIZE = 8 };
    arg_type load(const ushort* ptr) { return _mm_loadu_si128((const __m128i*)ptr); }
    void store(ushort* ptr, arg_type val) { _mm_storeu_si128((__m128i*)ptr, val); }
    void operator()(arg_type& a, arg_type& b) const
    {
        arg_type t = _mm_subs_epu16(a, b);
        a = _mm_subs_epu16(a, t);
        b = _mm_adds_epu16(b, t);
    }
};


struct MinMaxVec16s
{
    typedef short value_type;
    typedef __m128i arg_type;
    enum { SIZE = 8 };
    arg_type load(const short* ptr) { return _mm_loadu_si128((const __m128i*)ptr); }
    void store(short* ptr, arg_type val) { _mm_storeu_si128((__m128i*)ptr, val); }
    void operator()(arg_type& a, arg_type& b) const
    {
        arg_type t = a;
        a = _mm_min_epi16(a, b);
        b = _mm_max_epi16(b, t);
    }
};


struct MinMaxVec32f
{
    typedef float value_type;
    typedef __m128 arg_type;
    enum { SIZE = 4 };
    arg_type load(const float* ptr) { return _mm_loadu_ps(ptr); }
    void store(float* ptr, arg_type val) { _mm_storeu_ps(ptr, val); }
    void operator()(arg_type& a, arg_type& b) const
    {
        arg_type t = a;
        a = _mm_min_ps(a, b);
        b = _mm_max_ps(b, t);
    }
};


#else

typedef MinMax8u MinMaxVec8u;
typedef MinMax16u MinMaxVec16u;
typedef MinMax16s MinMaxVec16s;
typedef MinMax32f MinMaxVec32f;

#endif

template<class Op, class VecOp>
static void
medianBlur_SortNet( const Mat& _src, Mat& _dst, int m )
{
    typedef typename Op::value_type T;
    typedef typename Op::arg_type WT;
    typedef typename VecOp::arg_type VT;

    const T* src = (const T*)_src.data;
    T* dst = (T*)_dst.data;
    int sstep = (int)(_src.step/sizeof(T));
    int dstep = (int)(_dst.step/sizeof(T));
    Size size = _dst.size();
    int i, j, k, cn = _src.channels();
    Op op;
    VecOp vop;
    volatile bool useSIMD = checkHardwareSupport(CV_CPU_SSE2);

    if( m == 3 )
    {
        if( size.width == 1 || size.height == 1 )
        {
            int len = size.width + size.height - 1;
            int sdelta = size.height == 1 ? cn : sstep;
            int sdelta0 = size.height == 1 ? 0 : sstep - cn;
            int ddelta = size.height == 1 ? cn : dstep;

            for( i = 0; i < len; i++, src += sdelta0, dst += ddelta )
                for( j = 0; j < cn; j++, src++ )
                {
                    WT p0 = src[i > 0 ? -sdelta : 0];
                    WT p1 = src[0];
                    WT p2 = src[i < len - 1 ? sdelta : 0];

                    op(p0, p1); op(p1, p2); op(p0, p1);
                    dst[j] = (T)p1;
                }
            return;
        }

        size.width *= cn;
        for( i = 0; i < size.height; i++, dst += dstep )
        {
            const T* row0 = src + std::max(i - 1, 0)*sstep;
            const T* row1 = src + i*sstep;
            const T* row2 = src + std::min(i + 1, size.height-1)*sstep;
            int limit = useSIMD ? cn : size.width;

            for(j = 0;; )
            {
                for( ; j < limit; j++ )
                {
                    int j0 = j >= cn ? j - cn : j;
                    int j2 = j < size.width - cn ? j + cn : j;
                    WT p0 = row0[j0], p1 = row0[j], p2 = row0[j2];
                    WT p3 = row1[j0], p4 = row1[j], p5 = row1[j2];
                    WT p6 = row2[j0], p7 = row2[j], p8 = row2[j2];

                    op(p1, p2); op(p4, p5); op(p7, p8); op(p0, p1);
                    op(p3, p4); op(p6, p7); op(p1, p2); op(p4, p5);
                    op(p7, p8); op(p0, p3); op(p5, p8); op(p4, p7);
                    op(p3, p6); op(p1, p4); op(p2, p5); op(p4, p7);
                    op(p4, p2); op(p6, p4); op(p4, p2);
                    dst[j] = (T)p4;
                }

                if( limit == size.width )
                    break;

                for( ; j <= size.width - VecOp::SIZE - cn; j += VecOp::SIZE )
                {
                    VT p0 = vop.load(row0+j-cn), p1 = vop.load(row0+j), p2 = vop.load(row0+j+cn);
                    VT p3 = vop.load(row1+j-cn), p4 = vop.load(row1+j), p5 = vop.load(row1+j+cn);
                    VT p6 = vop.load(row2+j-cn), p7 = vop.load(row2+j), p8 = vop.load(row2+j+cn);

                    vop(p1, p2); vop(p4, p5); vop(p7, p8); vop(p0, p1);
                    vop(p3, p4); vop(p6, p7); vop(p1, p2); vop(p4, p5);
                    vop(p7, p8); vop(p0, p3); vop(p5, p8); vop(p4, p7);
                    vop(p3, p6); vop(p1, p4); vop(p2, p5); vop(p4, p7);
                    vop(p4, p2); vop(p6, p4); vop(p4, p2);
                    vop.store(dst+j, p4);
                }

                limit = size.width;
            }
        }
    }
    else if( m == 5 )
    {
        if( size.width == 1 || size.height == 1 )
        {
            int len = size.width + size.height - 1;
            int sdelta = size.height == 1 ? cn : sstep;
            int sdelta0 = size.height == 1 ? 0 : sstep - cn;
            int ddelta = size.height == 1 ? cn : dstep;

            for( i = 0; i < len; i++, src += sdelta0, dst += ddelta )
                for( j = 0; j < cn; j++, src++ )
                {
                    int i1 = i > 0 ? -sdelta : 0;
                    int i0 = i > 1 ? -sdelta*2 : i1;
                    int i3 = i < len-1 ? sdelta : 0;
                    int i4 = i < len-2 ? sdelta*2 : i3;
                    WT p0 = src[i0], p1 = src[i1], p2 = src[0], p3 = src[i3], p4 = src[i4];

                    op(p0, p1); op(p3, p4); op(p2, p3); op(p3, p4); op(p0, p2);
                    op(p2, p4); op(p1, p3); op(p1, p2);
                    dst[j] = (T)p2;
                }
            return;
        }

        size.width *= cn;
        for( i = 0; i < size.height; i++, dst += dstep )
        {
            const T* row[5];
            row[0] = src + std::max(i - 2, 0)*sstep;
            row[1] = src + std::max(i - 1, 0)*sstep;
            row[2] = src + i*sstep;
            row[3] = src + std::min(i + 1, size.height-1)*sstep;
            row[4] = src + std::min(i + 2, size.height-1)*sstep;
            int limit = useSIMD ? cn*2 : size.width;

            for(j = 0;; )
            {
                for( ; j < limit; j++ )
                {
                    WT p[25];
                    int j1 = j >= cn ? j - cn : j;
                    int j0 = j >= cn*2 ? j - cn*2 : j1;
                    int j3 = j < size.width - cn ? j + cn : j;
                    int j4 = j < size.width - cn*2 ? j + cn*2 : j3;
                    for( k = 0; k < 5; k++ )
                    {
                        const T* rowk = row[k];
                        p[k*5] = rowk[j0]; p[k*5+1] = rowk[j1];
                        p[k*5+2] = rowk[j]; p[k*5+3] = rowk[j3];
                        p[k*5+4] = rowk[j4];
                    }

                    op(p[1], p[2]); op(p[0], p[1]); op(p[1], p[2]); op(p[4], p[5]); op(p[3], p[4]);
                    op(p[4], p[5]); op(p[0], p[3]); op(p[2], p[5]); op(p[2], p[3]); op(p[1], p[4]);
                    op(p[1], p[2]); op(p[3], p[4]); op(p[7], p[8]); op(p[6], p[7]); op(p[7], p[8]);
                    op(p[10], p[11]); op(p[9], p[10]); op(p[10], p[11]); op(p[6], p[9]); op(p[8], p[11]);
                    op(p[8], p[9]); op(p[7], p[10]); op(p[7], p[8]); op(p[9], p[10]); op(p[0], p[6]);
                    op(p[4], p[10]); op(p[4], p[6]); op(p[2], p[8]); op(p[2], p[4]); op(p[6], p[8]);
                    op(p[1], p[7]); op(p[5], p[11]); op(p[5], p[7]); op(p[3], p[9]); op(p[3], p[5]);
                    op(p[7], p[9]); op(p[1], p[2]); op(p[3], p[4]); op(p[5], p[6]); op(p[7], p[8]);
                    op(p[9], p[10]); op(p[13], p[14]); op(p[12], p[13]); op(p[13], p[14]); op(p[16], p[17]);
                    op(p[15], p[16]); op(p[16], p[17]); op(p[12], p[15]); op(p[14], p[17]); op(p[14], p[15]);
                    op(p[13], p[16]); op(p[13], p[14]); op(p[15], p[16]); op(p[19], p[20]); op(p[18], p[19]);
                    op(p[19], p[20]); op(p[21], p[22]); op(p[23], p[24]); op(p[21], p[23]); op(p[22], p[24]);
                    op(p[22], p[23]); op(p[18], p[21]); op(p[20], p[23]); op(p[20], p[21]); op(p[19], p[22]);
                    op(p[22], p[24]); op(p[19], p[20]); op(p[21], p[22]); op(p[23], p[24]); op(p[12], p[18]);
                    op(p[16], p[22]); op(p[16], p[18]); op(p[14], p[20]); op(p[20], p[24]); op(p[14], p[16]);
                    op(p[18], p[20]); op(p[22], p[24]); op(p[13], p[19]); op(p[17], p[23]); op(p[17], p[19]);
                    op(p[15], p[21]); op(p[15], p[17]); op(p[19], p[21]); op(p[13], p[14]); op(p[15], p[16]);
                    op(p[17], p[18]); op(p[19], p[20]); op(p[21], p[22]); op(p[23], p[24]); op(p[0], p[12]);
                    op(p[8], p[20]); op(p[8], p[12]); op(p[4], p[16]); op(p[16], p[24]); op(p[12], p[16]);
                    op(p[2], p[14]); op(p[10], p[22]); op(p[10], p[14]); op(p[6], p[18]); op(p[6], p[10]);
                    op(p[10], p[12]); op(p[1], p[13]); op(p[9], p[21]); op(p[9], p[13]); op(p[5], p[17]);
                    op(p[13], p[17]); op(p[3], p[15]); op(p[11], p[23]); op(p[11], p[15]); op(p[7], p[19]);
                    op(p[7], p[11]); op(p[11], p[13]); op(p[11], p[12]);
                    dst[j] = (T)p[12];
                }

                if( limit == size.width )
                    break;

                for( ; j <= size.width - VecOp::SIZE - cn*2; j += VecOp::SIZE )
                {
                    VT p[25];
                    for( k = 0; k < 5; k++ )
                    {
                        const T* rowk = row[k];
                        p[k*5] = vop.load(rowk+j-cn*2); p[k*5+1] = vop.load(rowk+j-cn);
                        p[k*5+2] = vop.load(rowk+j); p[k*5+3] = vop.load(rowk+j+cn);
                        p[k*5+4] = vop.load(rowk+j+cn*2);
                    }

                    vop(p[1], p[2]); vop(p[0], p[1]); vop(p[1], p[2]); vop(p[4], p[5]); vop(p[3], p[4]);
                    vop(p[4], p[5]); vop(p[0], p[3]); vop(p[2], p[5]); vop(p[2], p[3]); vop(p[1], p[4]);
                    vop(p[1], p[2]); vop(p[3], p[4]); vop(p[7], p[8]); vop(p[6], p[7]); vop(p[7], p[8]);
                    vop(p[10], p[11]); vop(p[9], p[10]); vop(p[10], p[11]); vop(p[6], p[9]); vop(p[8], p[11]);
                    vop(p[8], p[9]); vop(p[7], p[10]); vop(p[7], p[8]); vop(p[9], p[10]); vop(p[0], p[6]);
                    vop(p[4], p[10]); vop(p[4], p[6]); vop(p[2], p[8]); vop(p[2], p[4]); vop(p[6], p[8]);
                    vop(p[1], p[7]); vop(p[5], p[11]); vop(p[5], p[7]); vop(p[3], p[9]); vop(p[3], p[5]);
                    vop(p[7], p[9]); vop(p[1], p[2]); vop(p[3], p[4]); vop(p[5], p[6]); vop(p[7], p[8]);
                    vop(p[9], p[10]); vop(p[13], p[14]); vop(p[12], p[13]); vop(p[13], p[14]); vop(p[16], p[17]);
                    vop(p[15], p[16]); vop(p[16], p[17]); vop(p[12], p[15]); vop(p[14], p[17]); vop(p[14], p[15]);
                    vop(p[13], p[16]); vop(p[13], p[14]); vop(p[15], p[16]); vop(p[19], p[20]); vop(p[18], p[19]);
                    vop(p[19], p[20]); vop(p[21], p[22]); vop(p[23], p[24]); vop(p[21], p[23]); vop(p[22], p[24]);
                    vop(p[22], p[23]); vop(p[18], p[21]); vop(p[20], p[23]); vop(p[20], p[21]); vop(p[19], p[22]);
                    vop(p[22], p[24]); vop(p[19], p[20]); vop(p[21], p[22]); vop(p[23], p[24]); vop(p[12], p[18]);
                    vop(p[16], p[22]); vop(p[16], p[18]); vop(p[14], p[20]); vop(p[20], p[24]); vop(p[14], p[16]);
                    vop(p[18], p[20]); vop(p[22], p[24]); vop(p[13], p[19]); vop(p[17], p[23]); vop(p[17], p[19]);
                    vop(p[15], p[21]); vop(p[15], p[17]); vop(p[19], p[21]); vop(p[13], p[14]); vop(p[15], p[16]);
                    vop(p[17], p[18]); vop(p[19], p[20]); vop(p[21], p[22]); vop(p[23], p[24]); vop(p[0], p[12]);
                    vop(p[8], p[20]); vop(p[8], p[12]); vop(p[4], p[16]); vop(p[16], p[24]); vop(p[12], p[16]);
                    vop(p[2], p[14]); vop(p[10], p[22]); vop(p[10], p[14]); vop(p[6], p[18]); vop(p[6], p[10]);
                    vop(p[10], p[12]); vop(p[1], p[13]); vop(p[9], p[21]); vop(p[9], p[13]); vop(p[5], p[17]);
                    vop(p[13], p[17]); vop(p[3], p[15]); vop(p[11], p[23]); vop(p[11], p[15]); vop(p[7], p[19]);
                    vop(p[7], p[11]); vop(p[11], p[13]); vop(p[11], p[12]);
                    vop.store(dst+j, p[12]);
                }

                limit = size.width;
            }
        }
    }
}

}

void cv::medianBlur( InputArray _src0, OutputArray _dst, int ksize )
{
    Mat src0 = _src0.getMat();
    _dst.create( src0.size(), src0.type() );
    Mat dst = _dst.getMat();

    if( ksize <= 1 )
    {
        src0.copyTo(dst);
        return;
    }

    CV_Assert( ksize % 2 == 1 );

#ifdef HAVE_TEGRA_OPTIMIZATION
    if (tegra::medianBlur(src0, dst, ksize))
        return;
#endif

    bool useSortNet = ksize == 3 || (ksize == 5
#if !CV_SSE2
            && src0.depth() > CV_8U
#endif
        );

    Mat src;
    if( useSortNet )
    {
        if( dst.data != src0.data )
            src = src0;
        else
            src0.copyTo(src);

        if( src.depth() == CV_8U )
            medianBlur_SortNet<MinMax8u, MinMaxVec8u>( src, dst, ksize );
        else if( src.depth() == CV_16U )
            medianBlur_SortNet<MinMax16u, MinMaxVec16u>( src, dst, ksize );
        else if( src.depth() == CV_16S )
            medianBlur_SortNet<MinMax16s, MinMaxVec16s>( src, dst, ksize );
        else if( src.depth() == CV_32F )
            medianBlur_SortNet<MinMax32f, MinMaxVec32f>( src, dst, ksize );
        else
            CV_Error(CV_StsUnsupportedFormat, "");

        return;
    }
    else
    {
        cv::copyMakeBorder( src0, src, 0, 0, ksize/2, ksize/2, BORDER_REPLICATE );

        int cn = src0.channels();
        CV_Assert( src.depth() == CV_8U && (cn == 1 || cn == 3 || cn == 4) );

        double img_size_mp = (double)(src0.total())/(1 << 20);
        if( ksize <= 3 + (img_size_mp < 1 ? 12 : img_size_mp < 4 ? 6 : 2)*(MEDIAN_HAVE_SIMD && checkHardwareSupport(CV_CPU_SSE2) ? 1 : 3))
            medianBlur_8u_Om( src, dst, ksize );
        else
            medianBlur_8u_O1( src, dst, ksize );
    }
}

/****************************************************************************************\
                                   Bilateral Filtering
\****************************************************************************************/

namespace cv
{

class BilateralFilter_8u_Invoker :
    public ParallelLoopBody
{
public:
    BilateralFilter_8u_Invoker(Mat& _dest, const Mat& _temp, int _radius, int _maxk,
        int* _space_ofs, float *_space_weight, float *_color_weight) :
        temp(&_temp), dest(&_dest), radius(_radius),
        maxk(_maxk), space_ofs(_space_ofs), space_weight(_space_weight), color_weight(_color_weight)
    {
    }

    virtual void operator() (const Range& range) const
    {
        int i, j, cn = dest->channels(), k;
        Size size = dest->size();
        #if CV_SSE3
        int CV_DECL_ALIGNED(16) buf[4];
        float CV_DECL_ALIGNED(16) bufSum[4];
        static const unsigned int CV_DECL_ALIGNED(16) bufSignMask[] = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
        bool haveSSE3 = checkHardwareSupport(CV_CPU_SSE3);
        #endif

        for( i = range.start; i < range.end; i++ )
        {
            const uchar* sptr = temp->ptr(i+radius) + radius*cn;
            uchar* dptr = dest->ptr(i);

            if( cn == 1 )
            {
                for( j = 0; j < size.width; j++ )
                {
                    float sum = 0, wsum = 0;
                    int val0 = sptr[j];
                    k = 0;
                    #if CV_SSE3
                    if( haveSSE3 )
                    {
                        __m128 _val0 = _mm_set1_ps(static_cast<float>(val0));
                        const __m128 _signMask = _mm_load_ps((const float*)bufSignMask);

                        for( ; k <= maxk - 4; k += 4 )
                        {
                            __m128 _valF = _mm_set_ps(sptr[j + space_ofs[k+3]], sptr[j + space_ofs[k+2]],
                                                      sptr[j + space_ofs[k+1]], sptr[j + space_ofs[k]]);

                            __m128 _val = _mm_andnot_ps(_signMask, _mm_sub_ps(_valF, _val0));
                            _mm_store_si128((__m128i*)buf, _mm_cvtps_epi32(_val));

                            __m128 _cw = _mm_set_ps(color_weight[buf[3]],color_weight[buf[2]],
                                                    color_weight[buf[1]],color_weight[buf[0]]);
                            __m128 _sw = _mm_loadu_ps(space_weight+k);
                            __m128 _w = _mm_mul_ps(_cw, _sw);
                             _cw = _mm_mul_ps(_w, _valF);

                             _sw = _mm_hadd_ps(_w, _cw);
                             _sw = _mm_hadd_ps(_sw, _sw);
                             _mm_storel_pi((__m64*)bufSum, _sw);

                             sum += bufSum[1];
                             wsum += bufSum[0];
                        }
                    }
                    #endif
                    for( ; k < maxk; k++ )
                    {
                        int val = sptr[j + space_ofs[k]];
                        float w = space_weight[k]*color_weight[std::abs(val - val0)];
                        sum += val*w;
                        wsum += w;
                    }
                    // overflow is not possible here => there is no need to use CV_CAST_8U
                    dptr[j] = (uchar)cvRound(sum/wsum);
                }
            }
            else
            {
                assert( cn == 3 );
                for( j = 0; j < size.width*3; j += 3 )
                {
                    float sum_b = 0, sum_g = 0, sum_r = 0, wsum = 0;
                    int b0 = sptr[j], g0 = sptr[j+1], r0 = sptr[j+2];
                    k = 0;
                    #if CV_SSE3
                    if( haveSSE3 )
                    {
                        const __m128 _b0 = _mm_set1_ps(static_cast<float>(b0));
                        const __m128 _g0 = _mm_set1_ps(static_cast<float>(g0));
                        const __m128 _r0 = _mm_set1_ps(static_cast<float>(r0));
                        const __m128 _signMask = _mm_load_ps((const float*)bufSignMask);

                        for( ; k <= maxk - 4; k += 4 )
                        {
                            const uchar* sptr_k  = sptr + j + space_ofs[k];
                            const uchar* sptr_k1 = sptr + j + space_ofs[k+1];
                            const uchar* sptr_k2 = sptr + j + space_ofs[k+2];
                            const uchar* sptr_k3 = sptr + j + space_ofs[k+3];

                            __m128 _b = _mm_set_ps(sptr_k3[0],sptr_k2[0],sptr_k1[0],sptr_k[0]);
                            __m128 _g = _mm_set_ps(sptr_k3[1],sptr_k2[1],sptr_k1[1],sptr_k[1]);
                            __m128 _r = _mm_set_ps(sptr_k3[2],sptr_k2[2],sptr_k1[2],sptr_k[2]);

                            __m128 bt = _mm_andnot_ps(_signMask, _mm_sub_ps(_b,_b0));
                            __m128 gt = _mm_andnot_ps(_signMask, _mm_sub_ps(_g,_g0));
                            __m128 rt = _mm_andnot_ps(_signMask, _mm_sub_ps(_r,_r0));

                            bt =_mm_add_ps(rt, _mm_add_ps(bt, gt));
                            _mm_store_si128((__m128i*)buf, _mm_cvtps_epi32(bt));

                            __m128 _w  = _mm_set_ps(color_weight[buf[3]],color_weight[buf[2]],
                                                    color_weight[buf[1]],color_weight[buf[0]]);
                            __m128 _sw = _mm_loadu_ps(space_weight+k);

                            _w = _mm_mul_ps(_w,_sw);
                            _b = _mm_mul_ps(_b, _w);
                            _g = _mm_mul_ps(_g, _w);
                            _r = _mm_mul_ps(_r, _w);

                             _w = _mm_hadd_ps(_w, _b);
                             _g = _mm_hadd_ps(_g, _r);

                             _w = _mm_hadd_ps(_w, _g);
                             _mm_store_ps(bufSum, _w);

                             wsum  += bufSum[0];
                             sum_b += bufSum[1];
                             sum_g += bufSum[2];
                             sum_r += bufSum[3];
                         }
                    }
                    #endif

                    for( ; k < maxk; k++ )
                    {
                        const uchar* sptr_k = sptr + j + space_ofs[k];
                        int b = sptr_k[0], g = sptr_k[1], r = sptr_k[2];
                        float w = space_weight[k]*color_weight[std::abs(b - b0) +
                                                               std::abs(g - g0) + std::abs(r - r0)];
                        sum_b += b*w; sum_g += g*w; sum_r += r*w;
                        wsum += w;
                    }
                    wsum = 1.f/wsum;
                    b0 = cvRound(sum_b*wsum);
                    g0 = cvRound(sum_g*wsum);
                    r0 = cvRound(sum_r*wsum);
                    dptr[j] = (uchar)b0; dptr[j+1] = (uchar)g0; dptr[j+2] = (uchar)r0;
                }
            }
        }
    }

private:
    const Mat *temp;
    Mat *dest;
    int radius, maxk, *space_ofs;
    float *space_weight, *color_weight;
};

#if defined (HAVE_IPP) && (IPP_VERSION_MAJOR >= 7)
class IPPBilateralFilter_8u_Invoker :
    public ParallelLoopBody
{
public:
    IPPBilateralFilter_8u_Invoker(Mat &_src, Mat &_dst, double _sigma_color, double _sigma_space, int _radius, bool *_ok) :
      ParallelLoopBody(), src(_src), dst(_dst), sigma_color(_sigma_color), sigma_space(_sigma_space), radius(_radius), ok(_ok)
      {
          *ok = true;
      }

      virtual void operator() (const Range& range) const
      {
          int d = radius * 2 + 1;
          IppiSize kernel = {d, d};
          IppiSize roi={dst.cols, range.end - range.start};
          int bufsize=0;
          ippiFilterBilateralGetBufSize_8u_C1R( ippiFilterBilateralGauss, roi, kernel, &bufsize);
          AutoBuffer<uchar> buf(bufsize);
          IppiFilterBilateralSpec *pSpec = (IppiFilterBilateralSpec *)alignPtr(&buf[0], 32);
          ippiFilterBilateralInit_8u_C1R( ippiFilterBilateralGauss, kernel, (Ipp32f)sigma_color, (Ipp32f)sigma_space, 1, pSpec );
          if( ippiFilterBilateral_8u_C1R( src.ptr<uchar>(range.start) + radius * ((int)src.step[0] + 1), (int)src.step[0], dst.ptr<uchar>(range.start), (int)dst.step[0], roi, kernel, pSpec ) < 0)
              *ok = false;
      }
private:
    Mat &src;
    Mat &dst;
    double sigma_color;
    double sigma_space;
    int radius;
    bool *ok;
    const IPPBilateralFilter_8u_Invoker& operator= (const IPPBilateralFilter_8u_Invoker&);
};
#endif

static void
bilateralFilter_8u( const Mat& src, Mat& dst, int d,
    double sigma_color, double sigma_space,
    int borderType )
{

    int cn = src.channels();
    int i, j, maxk, radius;
    Size size = src.size();

    CV_Assert( (src.type() == CV_8UC1 || src.type() == CV_8UC3) &&
              src.type() == dst.type() && src.size() == dst.size() &&
              src.data != dst.data );

    if( sigma_color <= 0 )
        sigma_color = 1;
    if( sigma_space <= 0 )
        sigma_space = 1;

    double gauss_color_coeff = -0.5/(sigma_color*sigma_color);
    double gauss_space_coeff = -0.5/(sigma_space*sigma_space);

    if( d <= 0 )
        radius = cvRound(sigma_space*1.5);
    else
        radius = d/2;
    radius = MAX(radius, 1);
    d = radius*2 + 1;

    Mat temp;
    copyMakeBorder( src, temp, radius, radius, radius, radius, borderType );

#if defined HAVE_IPP && (IPP_VERSION_MAJOR >= 7)
    if( cn == 1 )
    {
        bool ok;
        IPPBilateralFilter_8u_Invoker body(temp, dst, sigma_color * sigma_color, sigma_space * sigma_space, radius, &ok );
        parallel_for_(Range(0, dst.rows), body, dst.total()/(double)(1<<16));
        if( ok ) return;
    }
#endif

    vector<float> _color_weight(cn*256);
    vector<float> _space_weight(d*d);
    vector<int> _space_ofs(d*d);
    float* color_weight = &_color_weight[0];
    float* space_weight = &_space_weight[0];
    int* space_ofs = &_space_ofs[0];

    // initialize color-related bilateral filter coefficients

    for( i = 0; i < 256*cn; i++ )
        color_weight[i] = (float)std::exp(i*i*gauss_color_coeff);

    // initialize space-related bilateral filter coefficients
    for( i = -radius, maxk = 0; i <= radius; i++ )
    {
        j = -radius;

        for( ;j <= radius; j++ )
        {
            double r = std::sqrt((double)i*i + (double)j*j);
            if( r > radius )
                continue;
            space_weight[maxk] = (float)std::exp(r*r*gauss_space_coeff);
            space_ofs[maxk++] = (int)(i*temp.step + j*cn);
        }
    }

    BilateralFilter_8u_Invoker body(dst, temp, radius, maxk, space_ofs, space_weight, color_weight);
    parallel_for_(Range(0, size.height), body, dst.total()/(double)(1<<16));
}


class BilateralFilter_32f_Invoker :
    public ParallelLoopBody
{
public:

    BilateralFilter_32f_Invoker(int _cn, int _radius, int _maxk, int *_space_ofs,
        const Mat& _temp, Mat& _dest, float _scale_index, float *_space_weight, float *_expLUT) :
        cn(_cn), radius(_radius), maxk(_maxk), space_ofs(_space_ofs),
        temp(&_temp), dest(&_dest), scale_index(_scale_index), space_weight(_space_weight), expLUT(_expLUT)
    {
    }

    virtual void operator() (const Range& range) const
    {
        int i, j, k;
        Size size = dest->size();
        #if CV_SSE3
        int CV_DECL_ALIGNED(16) idxBuf[4];
        float CV_DECL_ALIGNED(16) bufSum32[4];
        static const unsigned int CV_DECL_ALIGNED(16) bufSignMask[] = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
        bool haveSSE3 = checkHardwareSupport(CV_CPU_SSE3);
        #endif

        for( i = range.start; i < range.end; i++ )
        {
            const float* sptr = temp->ptr<float>(i+radius) + radius*cn;
            float* dptr = dest->ptr<float>(i);

            if( cn == 1 )
            {
                for( j = 0; j < size.width; j++ )
                {
                    float sum = 0, wsum = 0;
                    float val0 = sptr[j];
                    k = 0;
                    #if CV_SSE3
                    if( haveSSE3 )
                    {
                        const __m128 _val0 = _mm_set1_ps(sptr[j]);
                        const __m128 _scale_index = _mm_set1_ps(scale_index);
                        const __m128 _signMask = _mm_load_ps((const float*)bufSignMask);

                        for( ; k <= maxk - 4 ; k += 4 )
                        {
                            __m128 _sw    = _mm_loadu_ps(space_weight + k);
                            __m128 _val   = _mm_set_ps(sptr[j + space_ofs[k+3]], sptr[j + space_ofs[k+2]],
                                                       sptr[j + space_ofs[k+1]], sptr[j + space_ofs[k]]);
                            __m128 _alpha = _mm_mul_ps(_mm_andnot_ps( _signMask, _mm_sub_ps(_val,_val0)), _scale_index);

                            __m128i _idx = _mm_cvtps_epi32(_alpha);
                            _mm_store_si128((__m128i*)idxBuf, _idx);
                            _alpha = _mm_sub_ps(_alpha, _mm_cvtepi32_ps(_idx));

                            __m128 _explut  = _mm_set_ps(expLUT[idxBuf[3]], expLUT[idxBuf[2]],
                                                         expLUT[idxBuf[1]], expLUT[idxBuf[0]]);
                            __m128 _explut1 = _mm_set_ps(expLUT[idxBuf[3]+1], expLUT[idxBuf[2]+1],
                                                         expLUT[idxBuf[1]+1], expLUT[idxBuf[0]+1]);

                            __m128 _w = _mm_mul_ps(_sw, _mm_add_ps(_explut, _mm_mul_ps(_alpha, _mm_sub_ps(_explut1, _explut))));
                            _val = _mm_mul_ps(_w, _val);

                             _sw = _mm_hadd_ps(_w, _val);
                             _sw = _mm_hadd_ps(_sw, _sw);
                             _mm_storel_pi((__m64*)bufSum32, _sw);

                             sum += bufSum32[1];
                             wsum += bufSum32[0];
                        }
                    }
                    #endif

                    for( ; k < maxk; k++ )
                    {
                        float val = sptr[j + space_ofs[k]];
                        float alpha = (float)(std::abs(val - val0)*scale_index);
                        int idx = cvFloor(alpha);
                        alpha -= idx;
                        float w = space_weight[k]*(expLUT[idx] + alpha*(expLUT[idx+1] - expLUT[idx]));
                        sum += val*w;
                        wsum += w;
                    }
                    dptr[j] = (float)(sum/wsum);
                }
            }
            else
            {
                assert( cn == 3 );
                for( j = 0; j < size.width*3; j += 3 )
                {
                    float sum_b = 0, sum_g = 0, sum_r = 0, wsum = 0;
                    float b0 = sptr[j], g0 = sptr[j+1], r0 = sptr[j+2];
                    k = 0;
                    #if  CV_SSE3
                    if( haveSSE3 )
                    {
                        const __m128 _b0 = _mm_set1_ps(b0);
                        const __m128 _g0 = _mm_set1_ps(g0);
                        const __m128 _r0 = _mm_set1_ps(r0);
                        const __m128 _scale_index = _mm_set1_ps(scale_index);
                        const __m128 _signMask = _mm_load_ps((const float*)bufSignMask);

                        for( ; k <= maxk-4; k += 4 )
                        {
                            __m128 _sw = _mm_loadu_ps(space_weight + k);

                            const float* sptr_k  = sptr + j + space_ofs[k];
                            const float* sptr_k1 = sptr + j + space_ofs[k+1];
                            const float* sptr_k2 = sptr + j + space_ofs[k+2];
                            const float* sptr_k3 = sptr + j + space_ofs[k+3];

                            __m128 _b = _mm_set_ps(sptr_k3[0], sptr_k2[0], sptr_k1[0], sptr_k[0]);
                            __m128 _g = _mm_set_ps(sptr_k3[1], sptr_k2[1], sptr_k1[1], sptr_k[1]);
                            __m128 _r = _mm_set_ps(sptr_k3[2], sptr_k2[2], sptr_k1[2], sptr_k[2]);

                            __m128 _bt = _mm_andnot_ps(_signMask,_mm_sub_ps(_b,_b0));
                            __m128 _gt = _mm_andnot_ps(_signMask,_mm_sub_ps(_g,_g0));
                            __m128 _rt = _mm_andnot_ps(_signMask,_mm_sub_ps(_r,_r0));

                            __m128 _alpha = _mm_mul_ps(_scale_index, _mm_add_ps(_rt,_mm_add_ps(_bt, _gt)));

                            __m128i _idx  = _mm_cvtps_epi32(_alpha);
                            _mm_store_si128((__m128i*)idxBuf, _idx);
                            _alpha = _mm_sub_ps(_alpha, _mm_cvtepi32_ps(_idx));

                            __m128 _explut  = _mm_set_ps(expLUT[idxBuf[3]], expLUT[idxBuf[2]], expLUT[idxBuf[1]], expLUT[idxBuf[0]]);
                            __m128 _explut1 = _mm_set_ps(expLUT[idxBuf[3]+1], expLUT[idxBuf[2]+1], expLUT[idxBuf[1]+1], expLUT[idxBuf[0]+1]);

                            __m128 _w = _mm_mul_ps(_sw, _mm_add_ps(_explut, _mm_mul_ps(_alpha, _mm_sub_ps(_explut1, _explut))));

                            _b = _mm_mul_ps(_b, _w);
                            _g = _mm_mul_ps(_g, _w);
                            _r = _mm_mul_ps(_r, _w);

                             _w = _mm_hadd_ps(_w, _b);
                             _g = _mm_hadd_ps(_g, _r);

                             _w = _mm_hadd_ps(_w, _g);
                             _mm_store_ps(bufSum32, _w);

                             wsum  += bufSum32[0];
                             sum_b += bufSum32[1];
                             sum_g += bufSum32[2];
                             sum_r += bufSum32[3];
                        }

                    }
                    #endif

                    for(; k < maxk; k++ )
                    {
                        const float* sptr_k = sptr + j + space_ofs[k];
                        float b = sptr_k[0], g = sptr_k[1], r = sptr_k[2];
                        float alpha = (float)((std::abs(b - b0) +
                            std::abs(g - g0) + std::abs(r - r0))*scale_index);
                        int idx = cvFloor(alpha);
                        alpha -= idx;
                        float w = space_weight[k]*(expLUT[idx] + alpha*(expLUT[idx+1] - expLUT[idx]));
                        sum_b += b*w; sum_g += g*w; sum_r += r*w;
                        wsum += w;
                    }
                    wsum = 1.f/wsum;
                    b0 = sum_b*wsum;
                    g0 = sum_g*wsum;
                    r0 = sum_r*wsum;
                    dptr[j] = b0; dptr[j+1] = g0; dptr[j+2] = r0;
                }
            }
        }
    }

private:
    int cn, radius, maxk, *space_ofs;
    const Mat* temp;
    Mat *dest;
    float scale_index, *space_weight, *expLUT;
};


static void
bilateralFilter_32f( const Mat& src, Mat& dst, int d,
                     double sigma_color, double sigma_space,
                     int borderType )
{
    int cn = src.channels();
    int i, j, maxk, radius;
    double minValSrc=-1, maxValSrc=1;
    const int kExpNumBinsPerChannel = 1 << 12;
    int kExpNumBins = 0;
    float lastExpVal = 1.f;
    float len, scale_index;
    Size size = src.size();

    CV_Assert( (src.type() == CV_32FC1 || src.type() == CV_32FC3) &&
        src.type() == dst.type() && src.size() == dst.size() &&
        src.data != dst.data );

    if( sigma_color <= 0 )
        sigma_color = 1;
    if( sigma_space <= 0 )
        sigma_space = 1;

    double gauss_color_coeff = -0.5/(sigma_color*sigma_color);
    double gauss_space_coeff = -0.5/(sigma_space*sigma_space);

    if( d <= 0 )
        radius = cvRound(sigma_space*1.5);
    else
        radius = d/2;
    radius = MAX(radius, 1);
    d = radius*2 + 1;
    // compute the min/max range for the input image (even if multichannel)

    minMaxLoc( src.reshape(1), &minValSrc, &maxValSrc );
    if(std::abs(minValSrc - maxValSrc) < FLT_EPSILON)
    {
        src.copyTo(dst);
        return;
    }

    // temporary copy of the image with borders for easy processing
    Mat temp;
    copyMakeBorder( src, temp, radius, radius, radius, radius, borderType );
    const double insteadNaNValue = -5. * sigma_color;
    patchNaNs( temp, insteadNaNValue ); // this replacement of NaNs makes the assumption that depth values are nonnegative
                                        // TODO: make insteadNaNValue avalible in the outside function interface to control the cases breaking the assumption
    // allocate lookup tables
    vector<float> _space_weight(d*d);
    vector<int> _space_ofs(d*d);
    float* space_weight = &_space_weight[0];
    int* space_ofs = &_space_ofs[0];

    // assign a length which is slightly more than needed
    len = (float)(maxValSrc - minValSrc) * cn;
    kExpNumBins = kExpNumBinsPerChannel * cn;
    vector<float> _expLUT(kExpNumBins+2);
    float* expLUT = &_expLUT[0];

    scale_index = kExpNumBins/len;

    // initialize the exp LUT
    for( i = 0; i < kExpNumBins+2; i++ )
    {
        if( lastExpVal > 0.f )
        {
            double val =  i / scale_index;
            expLUT[i] = (float)std::exp(val * val * gauss_color_coeff);
            lastExpVal = expLUT[i];
        }
        else
            expLUT[i] = 0.f;
    }

    // initialize space-related bilateral filter coefficients
    for( i = -radius, maxk = 0; i <= radius; i++ )
        for( j = -radius; j <= radius; j++ )
        {
            double r = std::sqrt((double)i*i + (double)j*j);
            if( r > radius )
                continue;
            space_weight[maxk] = (float)std::exp(r*r*gauss_space_coeff);
            space_ofs[maxk++] = (int)(i*(temp.step/sizeof(float)) + j*cn);
        }

    // parallel_for usage

    BilateralFilter_32f_Invoker body(cn, radius, maxk, space_ofs, temp, dst, scale_index, space_weight, expLUT);
    parallel_for_(Range(0, size.height), body, dst.total()/(double)(1<<16));
}

}

void cv::bilateralFilter( InputArray _src, OutputArray _dst, int d,
                      double sigmaColor, double sigmaSpace,
                      int borderType )
{
    Mat src = _src.getMat();
    _dst.create( src.size(), src.type() );
    Mat dst = _dst.getMat();

    if( src.depth() == CV_8U )
        bilateralFilter_8u( src, dst, d, sigmaColor, sigmaSpace, borderType );
    else if( src.depth() == CV_32F )
        bilateralFilter_32f( src, dst, d, sigmaColor, sigmaSpace, borderType );
    else
        CV_Error( CV_StsUnsupportedFormat,
        "Bilateral filtering is only implemented for 8u and 32f images" );
}


/****************************************************************************************\
                                  Adaptive Bilateral Filtering
\****************************************************************************************/

namespace cv
{
#ifndef ABF_CALCVAR
#define ABF_CALCVAR 1
#endif

#ifndef ABF_FIXED_WEIGHT
#define ABF_FIXED_WEIGHT 0
#endif

#ifndef ABF_GAUSSIAN
#define ABF_GAUSSIAN 1
#endif

class adaptiveBilateralFilter_8u_Invoker :
    public ParallelLoopBody
{
public:
    adaptiveBilateralFilter_8u_Invoker(Mat& _dest, const Mat& _temp, Size _ksize, double _sigma_space, double _maxSigmaColor, Point _anchor) :
        temp(&_temp), dest(&_dest), ksize(_ksize), sigma_space(_sigma_space), maxSigma_Color(_maxSigmaColor), anchor(_anchor)
    {
        if( sigma_space <= 0 )
            sigma_space = 1;
        CV_Assert((ksize.width & 1) && (ksize.height & 1));
        space_weight.resize(ksize.width * ksize.height);
        double sigma2 = sigma_space * sigma_space;
        int idx = 0;
        int w = ksize.width / 2;
        int h = ksize.height / 2;
        for(int y=-h; y<=h; y++)
            for(int x=-w; x<=w; x++)
        {
#if ABF_GAUSSIAN
            space_weight[idx++] = (float)exp ( -0.5*(x * x + y * y)/sigma2);
#else
            space_weight[idx++] = (float)(sigma2 / (sigma2 + x * x + y * y));
#endif
        }
    }
    virtual void operator()(const Range& range) const
    {
        int cn = dest->channels();
        int anX = anchor.x;

        const uchar *tptr;

        for(int i = range.start;i < range.end; i++)
        {
            int startY = i;
            if(cn == 1)
            {
                float var;
                int currVal;
                int sumVal = 0;
                int sumValSqr = 0;
                int currValCenter;
                int currWRTCenter;
                float weight;
                float totalWeight = 0.;
                float tmpSum = 0.;

                for(int j = 0;j < dest->cols *cn; j+=cn)
                {
                    sumVal = 0;
                    sumValSqr= 0;
                    totalWeight = 0.;
                    tmpSum = 0.;

                    // Top row: don't sum the very last element
                    int startLMJ = 0;
                    int endLMJ  = ksize.width  - 1;
                    int howManyAll = (anX *2 +1)*(ksize.width );
#if ABF_CALCVAR
                    for(int x = startLMJ; x< endLMJ; x++)
                    {
                        tptr = temp->ptr(startY + x) +j;
                        for(int y=-anX; y<=anX; y++)
                        {
                            currVal = tptr[cn*(y+anX)];
                            sumVal += currVal;
                            sumValSqr += (currVal *currVal);
                        }
                    }
                    var = ( (sumValSqr * howManyAll)- sumVal * sumVal )  /  ( (float)(howManyAll*howManyAll));

                    if(var < 0.01)
                        var = 0.01f;
                    else if(var > (float)(maxSigma_Color*maxSigma_Color) )
                        var =  (float)(maxSigma_Color*maxSigma_Color) ;

#else
                    var = maxSigmaColor*maxSigmaColor;
#endif
                    startLMJ = 0;
                    endLMJ = ksize.width;
                    tptr = temp->ptr(startY + (startLMJ+ endLMJ)/2);
                    currValCenter =tptr[j+cn*anX];
                    for(int x = startLMJ; x< endLMJ; x++)
                    {
                        tptr = temp->ptr(startY + x) +j;
                        for(int y=-anX; y<=anX; y++)
                        {
#if ABF_FIXED_WEIGHT
                            weight = 1.0;
#else
                            currVal = tptr[cn*(y+anX)];
                            currWRTCenter = currVal - currValCenter;

#if ABF_GAUSSIAN
                            weight = exp ( -0.5f * currWRTCenter * currWRTCenter/var ) * space_weight[x*ksize.width+y+anX];
#else
                            weight = var / ( var + (currWRTCenter * currWRTCenter) ) * space_weight[x*ksize.width+y+anX];
#endif

#endif
                            tmpSum += ((float)tptr[cn*(y+anX)] * weight);
                            totalWeight += weight;
                        }
                    }
                    tmpSum /= totalWeight;

                   dest->at<uchar>(startY ,j)= static_cast<uchar>(tmpSum);
                }
            }
            else
            {
                assert(cn == 3);
                float var_b, var_g, var_r;
                int currVal_b, currVal_g, currVal_r;
                int sumVal_b= 0, sumVal_g= 0, sumVal_r= 0;
                int sumValSqr_b= 0, sumValSqr_g= 0, sumValSqr_r= 0;
                int currValCenter_b= 0, currValCenter_g= 0, currValCenter_r= 0;
                int currWRTCenter_b, currWRTCenter_g, currWRTCenter_r;
                float weight_b, weight_g, weight_r;
                float totalWeight_b= 0., totalWeight_g= 0., totalWeight_r= 0.;
                float tmpSum_b = 0., tmpSum_g= 0., tmpSum_r = 0.;

                for(int j = 0;j < dest->cols *cn; j+=cn)
                {
                    sumVal_b= 0, sumVal_g= 0, sumVal_r= 0;
                    sumValSqr_b= 0, sumValSqr_g= 0, sumValSqr_r= 0;
                    totalWeight_b= 0., totalWeight_g= 0., totalWeight_r= 0.;
                    tmpSum_b = 0., tmpSum_g= 0., tmpSum_r = 0.;

                    // Top row: don't sum the very last element
                    int startLMJ = 0;
                    int endLMJ  = ksize.width - 1;
                    int howManyAll = (anX *2 +1)*(ksize.width);
#if ABF_CALCVAR
                    float max_var = (float)( maxSigma_Color*maxSigma_Color);
                    for(int x = startLMJ; x< endLMJ; x++)
                    {
                        tptr = temp->ptr(startY + x) +j;
                        for(int y=-anX; y<=anX; y++)
                        {
                            currVal_b = tptr[cn*(y+anX)], currVal_g = tptr[cn*(y+anX)+1], currVal_r =tptr[cn*(y+anX)+2];
                            sumVal_b += currVal_b;
                            sumVal_g += currVal_g;
                            sumVal_r += currVal_r;
                            sumValSqr_b += (currVal_b *currVal_b);
                            sumValSqr_g += (currVal_g *currVal_g);
                            sumValSqr_r += (currVal_r *currVal_r);
                        }
                    }
                    var_b =  ( (sumValSqr_b * howManyAll)- sumVal_b * sumVal_b )  /  ( (float)(howManyAll*howManyAll));
                    var_g =  ( (sumValSqr_g * howManyAll)- sumVal_g * sumVal_g )  /  ( (float)(howManyAll*howManyAll));
                    var_r =  ( (sumValSqr_r * howManyAll)- sumVal_r * sumVal_r )  /  ( (float)(howManyAll*howManyAll));

                    if(var_b < 0.01)
                        var_b = 0.01f;
                    else if(var_b > max_var )
                        var_b =  (float)(max_var) ;

                    if(var_g < 0.01)
                        var_g = 0.01f;
                    else if(var_g > max_var )
                        var_g =  (float)(max_var) ;

                    if(var_r < 0.01)
                        var_r = 0.01f;
                    else if(var_r > max_var )
                        var_r =  (float)(max_var) ;

#else
                    var_b = maxSigma_Color*maxSigma_Color; var_g = maxSigma_Color*maxSigma_Color; var_r = maxSigma_Color*maxSigma_Color;
#endif
                    startLMJ = 0;
                    endLMJ = ksize.width;
                    tptr = temp->ptr(startY + (startLMJ+ endLMJ)/2) + j;
                    currValCenter_b =tptr[cn*anX], currValCenter_g =tptr[cn*anX+1], currValCenter_r =tptr[cn*anX+2];
                    for(int x = startLMJ; x< endLMJ; x++)
                    {
                        tptr = temp->ptr(startY + x) +j;
                        for(int y=-anX; y<=anX; y++)
                        {
#if ABF_FIXED_WEIGHT
                            weight_b = 1.0;
                            weight_g = 1.0;
                            weight_r = 1.0;
#else
                            currVal_b = tptr[cn*(y+anX)];currVal_g=tptr[cn*(y+anX)+1];currVal_r=tptr[cn*(y+anX)+2];
                            currWRTCenter_b = currVal_b - currValCenter_b;
                            currWRTCenter_g = currVal_g - currValCenter_g;
                            currWRTCenter_r = currVal_r - currValCenter_r;

                            float cur_spw = space_weight[x*ksize.width+y+anX];

#if ABF_GAUSSIAN
                            weight_b = exp( -0.5f * currWRTCenter_b * currWRTCenter_b/ var_b ) * cur_spw;
                            weight_g = exp( -0.5f * currWRTCenter_g * currWRTCenter_g/ var_g ) * cur_spw;
                            weight_r = exp( -0.5f * currWRTCenter_r * currWRTCenter_r/ var_r ) * cur_spw;
#else
                            weight_b = var_b / ( var_b + (currWRTCenter_b * currWRTCenter_b) ) * cur_spw;
                            weight_g = var_g / ( var_g + (currWRTCenter_g * currWRTCenter_g) ) * cur_spw;
                            weight_r = var_r / ( var_r + (currWRTCenter_r * currWRTCenter_r) ) * cur_spw;
#endif
#endif
                            tmpSum_b += ((float)tptr[cn*(y+anX)]   * weight_b);
                            tmpSum_g += ((float)tptr[cn*(y+anX)+1] * weight_g);
                            tmpSum_r += ((float)tptr[cn*(y+anX)+2] * weight_r);
                            totalWeight_b += weight_b, totalWeight_g += weight_g, totalWeight_r += weight_r;
                        }
                    }
                    tmpSum_b /= totalWeight_b;
                    tmpSum_g /= totalWeight_g;
                    tmpSum_r /= totalWeight_r;

                    dest->at<uchar>(startY,j  )= static_cast<uchar>(tmpSum_b);
                    dest->at<uchar>(startY,j+1)= static_cast<uchar>(tmpSum_g);
                    dest->at<uchar>(startY,j+2)= static_cast<uchar>(tmpSum_r);
                }
            }
        }
    }
private:
    const Mat *temp;
    Mat *dest;
    Size ksize;
    double sigma_space;
    double maxSigma_Color;
    Point anchor;
    vector<float> space_weight;
};
static void adaptiveBilateralFilter_8u( const Mat& src, Mat& dst, Size ksize, double sigmaSpace, double maxSigmaColor, Point anchor, int borderType )
{
    Size size = src.size();

    CV_Assert( (src.type() == CV_8UC1 || src.type() == CV_8UC3) &&
              src.type() == dst.type() && src.size() == dst.size() &&
              src.data != dst.data );
    Mat temp;
    copyMakeBorder(src, temp, anchor.x, anchor.y, anchor.x, anchor.y, borderType);

    adaptiveBilateralFilter_8u_Invoker body(dst, temp, ksize, sigmaSpace, maxSigmaColor, anchor);
    parallel_for_(Range(0, size.height), body, dst.total()/(double)(1<<16));
}
}
void cv::adaptiveBilateralFilter( InputArray _src, OutputArray _dst, Size ksize,
                                  double sigmaSpace, double maxSigmaColor, Point anchor, int borderType )
{
    Mat src = _src.getMat();
    _dst.create(src.size(), src.type());
    Mat dst = _dst.getMat();

    CV_Assert(src.type() == CV_8UC1 || src.type() == CV_8UC3);

    anchor = normalizeAnchor(anchor,ksize);
    if( src.depth() == CV_8U )
        adaptiveBilateralFilter_8u( src, dst, ksize, sigmaSpace, maxSigmaColor, anchor, borderType );
    else
        CV_Error( CV_StsUnsupportedFormat,
        "Adaptive Bilateral filtering is only implemented for 8u images" );
}

//////////////////////////////////////////////////////////////////////////////////////////

CV_IMPL void
cvSmooth( const void* srcarr, void* dstarr, int smooth_type,
          int param1, int param2, double param3, double param4 )
{
    cv::Mat src = cv::cvarrToMat(srcarr), dst0 = cv::cvarrToMat(dstarr), dst = dst0;

    CV_Assert( dst.size() == src.size() &&
        (smooth_type == CV_BLUR_NO_SCALE || dst.type() == src.type()) );

    if( param2 <= 0 )
        param2 = param1;

    if( smooth_type == CV_BLUR || smooth_type == CV_BLUR_NO_SCALE )
        cv::boxFilter( src, dst, dst.depth(), cv::Size(param1, param2), cv::Point(-1,-1),
            smooth_type == CV_BLUR, cv::BORDER_REPLICATE );
    else if( smooth_type == CV_GAUSSIAN )
        cv::GaussianBlur( src, dst, cv::Size(param1, param2), param3, param4, cv::BORDER_REPLICATE );
    else if( smooth_type == CV_MEDIAN )
        cv::medianBlur( src, dst, param1 );
    else
        cv::bilateralFilter( src, dst, param1, param3, param4, cv::BORDER_REPLICATE );

    if( dst.data != dst0.data )
        CV_Error( CV_StsUnmatchedFormats, "The destination image does not have the proper type" );
}

/* End of file. */
