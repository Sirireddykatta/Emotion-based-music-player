/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
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
//   * The name of Intel Corporation may not be used to endorse or promote products
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

/*
  This is a regression test for stereo matching algorithms. This test gets some quality metrics
  discribed in "A Taxonomy and Evaluation of Dense Two-Frame Stereo Correspondence Algorithms".
  Daniel Scharstein, Richard Szeliski
*/

#include "test_precomp.hpp"
#include <limits>
#include <cstdio>

using namespace std;
using namespace cv;

const float EVAL_BAD_THRESH = 1.f;
const int EVAL_TEXTURELESS_WIDTH = 3;
const float EVAL_TEXTURELESS_THRESH = 4.f;
const float EVAL_DISP_THRESH = 1.f;
const float EVAL_DISP_GAP = 2.f;
const int EVAL_DISCONT_WIDTH = 9;
const int EVAL_IGNORE_BORDER = 10;

const int ERROR_KINDS_COUNT = 6;

//============================== quality measuring functions =================================================

/*
  Calculate textureless regions of image (regions where the squared horizontal intensity gradient averaged over
  a square window of size=evalTexturelessWidth is below a threshold=evalTexturelessThresh) and textured regions.
*/
void computeTextureBasedMasks( const Mat& _img, Mat* texturelessMask, Mat* texturedMask,
             int texturelessWidth = EVAL_TEXTURELESS_WIDTH, float texturelessThresh = EVAL_TEXTURELESS_THRESH )
{
    if( !texturelessMask && !texturedMask )
        return;
    if( _img.empty() )
        CV_Error( CV_StsBadArg, "img is empty" );

    Mat img = _img;
    if( _img.channels() > 1)
    {
        Mat tmp; cvtColor( _img, tmp, CV_BGR2GRAY ); img = tmp;
    }
    Mat dxI; Sobel( img, dxI, CV_32FC1, 1, 0, 3 );
    Mat dxI2; pow( dxI / 8.f/*normalize*/, 2, dxI2 );
    Mat avgDxI2; boxFilter( dxI2, avgDxI2, CV_32FC1, Size(texturelessWidth,texturelessWidth) );

    if( texturelessMask )
        *texturelessMask = avgDxI2 < texturelessThresh;
    if( texturedMask )
        *texturedMask = avgDxI2 >= texturelessThresh;
}

void checkTypeAndSizeOfDisp( const Mat& dispMap, const Size* sz )
{
    if( dispMap.empty() )
        CV_Error( CV_StsBadArg, "dispMap is empty" );
    if( dispMap.type() != CV_32FC1 )
        CV_Error( CV_StsBadArg, "dispMap must have CV_32FC1 type" );
    if( sz && (dispMap.rows != sz->height || dispMap.cols != sz->width) )
        CV_Error( CV_StsBadArg, "dispMap has incorrect size" );
}

void checkTypeAndSizeOfMask( const Mat& mask, Size sz )
{
    if( mask.empty() )
        CV_Error( CV_StsBadArg, "mask is empty" );
    if( mask.type() != CV_8UC1 )
        CV_Error( CV_StsBadArg, "mask must have CV_8UC1 type" );
    if( mask.rows != sz.height || mask.cols != sz.width )
        CV_Error( CV_StsBadArg, "mask has incorrect size" );
}

void checkDispMapsAndUnknDispMasks( const Mat& leftDispMap, const Mat& rightDispMap,
                                    const Mat& leftUnknDispMask, const Mat& rightUnknDispMask )
{
    // check type and size of disparity maps
    checkTypeAndSizeOfDisp( leftDispMap, 0 );
    if( !rightDispMap.empty() )
    {
        Size sz = leftDispMap.size();
        checkTypeAndSizeOfDisp( rightDispMap, &sz );
    }

    // check size and type of unknown disparity maps
    if( !leftUnknDispMask.empty() )
        checkTypeAndSizeOfMask( leftUnknDispMask, leftDispMap.size() );
    if( !rightUnknDispMask.empty() )
        checkTypeAndSizeOfMask( rightUnknDispMask, rightDispMap.size() );

    // check values of disparity maps (known disparity values musy be positive)
    double leftMinVal = 0, rightMinVal = 0;
    if( leftUnknDispMask.empty() )
        minMaxLoc( leftDispMap, &leftMinVal );
    else
        minMaxLoc( leftDispMap, &leftMinVal, 0, 0, 0, ~leftUnknDispMask );
    if( !rightDispMap.empty() )
    {
        if( rightUnknDispMask.empty() )
            minMaxLoc( rightDispMap, &rightMinVal );
        else
            minMaxLoc( rightDispMap, &rightMinVal, 0, 0, 0, ~rightUnknDispMask );
    }
    if( leftMinVal < 0 || rightMinVal < 0)
        CV_Error( CV_StsBadArg, "known disparity values must be positive" );
}

/*
  Calculate occluded regions of reference image (left image) (regions that are occluded in the matching image (right image),
  i.e., where the forward-mapped disparity lands at a location with a larger (nearer) disparity) and non occluded regions.
*/
void computeOcclusionBasedMasks( const Mat& leftDisp, const Mat& _rightDisp,
                             Mat* occludedMask, Mat* nonOccludedMask,
                             const Mat& leftUnknDispMask = Mat(), const Mat& rightUnknDispMask = Mat(),
                             float dispThresh = EVAL_DISP_THRESH )
{
    if( !occludedMask && !nonOccludedMask )
        return;
    checkDispMapsAndUnknDispMasks( leftDisp, _rightDisp, leftUnknDispMask, rightUnknDispMask );

    Mat rightDisp;
    if( _rightDisp.empty() )
    {
        if( !rightUnknDispMask.empty() )
           CV_Error( CV_StsBadArg, "rightUnknDispMask must be empty if _rightDisp is empty" );
        rightDisp.create(leftDisp.size(), CV_32FC1);
        rightDisp.setTo(Scalar::all(0) );
        for( int leftY = 0; leftY < leftDisp.rows; leftY++ )
        {
            for( int leftX = 0; leftX < leftDisp.cols; leftX++ )
            {
                if( !leftUnknDispMask.empty() && leftUnknDispMask.at<uchar>(leftY,leftX) )
                    continue;
                float leftDispVal = leftDisp.at<float>(leftY, leftX);
                int rightX = leftX - cvRound(leftDispVal), rightY = leftY;
                if( rightX >= 0)
                    rightDisp.at<float>(rightY,rightX) = max(rightDisp.at<float>(rightY,rightX), leftDispVal);
            }
        }
    }
    else
        _rightDisp.copyTo(rightDisp);

    if( occludedMask )
    {
        occludedMask->create(leftDisp.size(), CV_8UC1);
        occludedMask->setTo(Scalar::all(0) );
    }
    if( nonOccludedMask )
    {
        nonOccludedMask->create(leftDisp.size(), CV_8UC1);
        nonOccludedMask->setTo(Scalar::all(0) );
    }
    for( int leftY = 0; leftY < leftDisp.rows; leftY++ )
    {
        for( int leftX = 0; leftX < leftDisp.cols; leftX++ )
        {
            if( !leftUnknDispMask.empty() && leftUnknDispMask.at<uchar>(leftY,leftX) )
                continue;
            float leftDispVal = leftDisp.at<float>(leftY, leftX);
            int rightX = leftX - cvRound(leftDispVal), rightY = leftY;
            if( rightX < 0 && occludedMask )
                occludedMask->at<uchar>(leftY, leftX) = 255;
            else
            {
                if( !rightUnknDispMask.empty() && rightUnknDispMask.at<uchar>(rightY,rightX) )
                    continue;
                float rightDispVal = rightDisp.at<float>(rightY, rightX);
                if( rightDispVal > leftDispVal + dispThresh )
                {
                    if( occludedMask )
                        occludedMask->at<uchar>(leftY, leftX) = 255;
                }
                else
                {
                    if( nonOccludedMask )
                        nonOccludedMask->at<uchar>(leftY, leftX) = 255;
                }
            }
        }
    }
}

/*
  Calculate depth discontinuty regions: pixels whose neiboring disparities differ by more than
  dispGap, dilated by window of width discontWidth.
*/
void computeDepthDiscontMask( const Mat& disp, Mat& depthDiscontMask, const Mat& unknDispMask = Mat(),
                                 float dispGap = EVAL_DISP_GAP, int discontWidth = EVAL_DISCONT_WIDTH )
{
    if( disp.empty() )
        CV_Error( CV_StsBadArg, "disp is empty" );
    if( disp.type() != CV_32FC1 )
        CV_Error( CV_StsBadArg, "disp must have CV_32FC1 type" );
    if( !unknDispMask.empty() )
        checkTypeAndSizeOfMask( unknDispMask, disp.size() );

    Mat curDisp; disp.copyTo( curDisp );
    if( !unknDispMask.empty() )
        curDisp.setTo( Scalar(numeric_limits<float>::min()), unknDispMask );
    Mat maxNeighbDisp; dilate( curDisp, maxNeighbDisp, Mat(3, 3, CV_8UC1, Scalar(1)) );
    if( !unknDispMask.empty() )
        curDisp.setTo( Scalar(numeric_limits<float>::max()), unknDispMask );
    Mat minNeighbDisp; erode( curDisp, minNeighbDisp, Mat(3, 3, CV_8UC1, Scalar(1)) );
    depthDiscontMask = max( (Mat)(maxNeighbDisp-disp), (Mat)(disp-minNeighbDisp) ) > dispGap;
    if( !unknDispMask.empty() )
        depthDiscontMask &= ~unknDispMask;
    dilate( depthDiscontMask, depthDiscontMask, Mat(discontWidth, discontWidth, CV_8UC1, Scalar(1)) );
}

/*
   Get evaluation masks excluding a border.
*/
Mat getBorderedMask( Size maskSize, int border = EVAL_IGNORE_BORDER )
{
    CV_Assert( border >= 0 );
    Mat mask(maskSize, CV_8UC1, Scalar(0));
    int w = maskSize.width - 2*border, h = maskSize.height - 2*border;
    if( w < 0 ||  h < 0 )
        mask.setTo(Scalar(0));
    else
        mask( Rect(Point(border,border),Size(w,h)) ).setTo(Scalar(255));
    return mask;
}

/*
  Calculate root-mean-squared error between the computed disparity map (computedDisp) and ground truth map (groundTruthDisp).
*/
float dispRMS( const Mat& computedDisp, const Mat& groundTruthDisp, const Mat& mask )
{
    checkTypeAndSizeOfDisp( groundTruthDisp, 0 );
    Size sz = groundTruthDisp.size();
    checkTypeAndSizeOfDisp( computedDisp, &sz );

    int pointsCount = sz.height*sz.width;
    if( !mask.empty() )
    {
        checkTypeAndSizeOfMask( mask, sz );
        pointsCount = countNonZero(mask);
    }
    return 1.f/sqrt((float)pointsCount) * (float)norm(computedDisp, groundTruthDisp, NORM_L2, mask);
}

/*
  Calculate fraction of bad matching pixels.
*/
float badMatchPxlsFraction( const Mat& computedDisp, const Mat& groundTruthDisp, const Mat& mask,
                            float _badThresh = EVAL_BAD_THRESH )
{
    int badThresh = cvRound(_badThresh);
    checkTypeAndSizeOfDisp( groundTruthDisp, 0 );
    Size sz = groundTruthDisp.size();
    checkTypeAndSizeOfDisp( computedDisp, &sz );

    Mat badPxlsMap;
    absdiff( computedDisp, groundTruthDisp, badPxlsMap );
    badPxlsMap = badPxlsMap > badThresh;
    int pointsCount = sz.height*sz.width;
    if( !mask.empty() )
    {
        checkTypeAndSizeOfMask( mask, sz );
        badPxlsMap = badPxlsMap & mask;
        pointsCount = countNonZero(mask);
    }
    return 1.f/pointsCount * countNonZero(badPxlsMap);
}

//===================== regression test for stereo matching algorithms ==============================

const string ALGORITHMS_DIR = "stereomatching/algorithms/";
const string DATASETS_DIR = "stereomatching/datasets/";
const string DATASETS_FILE = "datasets.xml";

const string RUN_PARAMS_FILE = "_params.xml";
const string RESULT_FILE = "_res.xml";

const string LEFT_IMG_NAME = "im2.png";
const string RIGHT_IMG_NAME = "im6.png";
const string TRUE_LEFT_DISP_NAME = "disp2.png";
const string TRUE_RIGHT_DISP_NAME = "disp6.png";

string ERROR_PREFIXES[] = { "borderedAll",
                            "borderedNoOccl",
                            "borderedOccl",
                            "borderedTextured",
                            "borderedTextureless",
                            "borderedDepthDiscont" }; // size of ERROR_KINDS_COUNT


const string RMS_STR = "RMS";
const string BAD_PXLS_FRACTION_STR = "BadPxlsFraction";

class QualityEvalParams
{
public:
    QualityEvalParams() { setDefaults(); }
    QualityEvalParams( int _ignoreBorder )
    {
        setDefaults();
        ignoreBorder = _ignoreBorder;
    }
    void setDefaults()
    {
        badThresh = EVAL_BAD_THRESH;
        texturelessWidth = EVAL_TEXTURELESS_WIDTH;
        texturelessThresh = EVAL_TEXTURELESS_THRESH;
        dispThresh = EVAL_DISP_THRESH;
        dispGap = EVAL_DISP_GAP;
        discontWidth = EVAL_DISCONT_WIDTH;
        ignoreBorder = EVAL_IGNORE_BORDER;
    }
    float badThresh;
    int texturelessWidth;
    float texturelessThresh;
    float dispThresh;
    float dispGap;
    int discontWidth;
    int ignoreBorder;
};

class CV_StereoMatchingTest : public cvtest::BaseTest
{
public:
    CV_StereoMatchingTest()
    { rmsEps.resize( ERROR_KINDS_COUNT, 0.01f );  fracEps.resize( ERROR_KINDS_COUNT, 1.e-6f ); }
protected:
    // assumed that left image is a reference image
    virtual int runStereoMatchingAlgorithm( const Mat& leftImg, const Mat& rightImg,
                   Mat& leftDisp, Mat& rightDisp, int caseIdx ) = 0; // return ignored border width

    int readDatasetsParams( FileStorage& fs );
    virtual int readRunParams( FileStorage& fs );
    void writeErrors( const string& errName, const vector<float>& errors, FileStorage* fs = 0 );
    void readErrors( FileNode& fn, const string& errName, vector<float>& errors );
    int compareErrors( const vector<float>& calcErrors, const vector<float>& validErrors,
                       const vector<float>& eps, const string& errName );
    int processStereoMatchingResults( FileStorage& fs, int caseIdx, bool isWrite,
                  const Mat& leftImg, const Mat& rightImg,
                  const Mat& trueLeftDisp, const Mat& trueRightDisp,
                  const Mat& leftDisp, const Mat& rightDisp,
                  const QualityEvalParams& qualityEvalParams  );
    void run( int );

    vector<float> rmsEps;
    vector<float> fracEps;

    struct DatasetParams
    {
        int dispScaleFactor;
        int dispUnknVal;
    };
    map<string, DatasetParams> datasetsParams;

    vector<string> caseNames;
    vector<string> caseDatasets;
};

void CV_StereoMatchingTest::run(int)
{
    string dataPath = ts->get_data_path();
    string algorithmName = name;
    assert( !algorithmName.empty() );
    if( dataPath.empty() )
    {
        ts->printf( cvtest::TS::LOG, "dataPath is empty" );
        ts->set_failed_test_info( cvtest::TS::FAIL_BAD_ARG_CHECK );
        return;
    }

    FileStorage datasetsFS( dataPath + DATASETS_DIR + DATASETS_FILE, FileStorage::READ );
    int code = readDatasetsParams( datasetsFS );
    if( code != cvtest::TS::OK )
    {
        ts->set_failed_test_info( code );
        return;
    }
    FileStorage runParamsFS( dataPath + ALGORITHMS_DIR + algorithmName + RUN_PARAMS_FILE, FileStorage::READ );
    code = readRunParams( runParamsFS );
    if( code != cvtest::TS::OK )
    {
        ts->set_failed_test_info( code );
        return;
    }

    string fullResultFilename = dataPath + ALGORITHMS_DIR + algorithmName + RESULT_FILE;
    FileStorage resFS( fullResultFilename, FileStorage::READ );
    bool isWrite = true; // write or compare results
    if( resFS.isOpened() )
        isWrite = false;
    else
    {
        resFS.open( fullResultFilename, FileStorage::WRITE );
        if( !resFS.isOpened() )
        {
            ts->printf( cvtest::TS::LOG, "file %s can not be read or written\n", fullResultFilename.c_str() );
            ts->set_failed_test_info( cvtest::TS::FAIL_BAD_ARG_CHECK );
            return;
        }
        resFS << "stereo_matching" << "{";
    }

    int progress = 0, caseCount = (int)caseNames.size();
    for( int ci = 0; ci < caseCount; ci++)
    {
        progress = update_progress( progress, ci, caseCount, 0 );
        printf("progress: %d%%\n", progress);
        fflush(stdout);
        string datasetName = caseDatasets[ci];
        string datasetFullDirName = dataPath + DATASETS_DIR + datasetName + "/";
        Mat leftImg = imread(datasetFullDirName + LEFT_IMG_NAME);
        Mat rightImg = imread(datasetFullDirName + RIGHT_IMG_NAME);
        Mat trueLeftDisp = imread(datasetFullDirName + TRUE_LEFT_DISP_NAME, 0);
        Mat trueRightDisp = imread(datasetFullDirName + TRUE_RIGHT_DISP_NAME, 0);

        if( leftImg.empty() || rightImg.empty() || trueLeftDisp.empty() )
        {
            ts->printf( cvtest::TS::LOG, "images or left ground-truth disparities of dataset %s can not be read", datasetName.c_str() );
            code = cvtest::TS::FAIL_INVALID_TEST_DATA;
            continue;
        }
        int dispScaleFactor = datasetsParams[datasetName].dispScaleFactor;
        Mat tmp; trueLeftDisp.convertTo( tmp, CV_32FC1, 1.f/dispScaleFactor ); trueLeftDisp = tmp; tmp.release();
        if( !trueRightDisp.empty() )
        {
            trueRightDisp.convertTo( tmp, CV_32FC1, 1.f/dispScaleFactor ); trueRightDisp = tmp; tmp.release();
        }

        Mat leftDisp, rightDisp;
        int ignBorder = max(runStereoMatchingAlgorithm(leftImg, rightImg, leftDisp, rightDisp, ci), EVAL_IGNORE_BORDER);
        leftDisp.convertTo( tmp, CV_32FC1 ); leftDisp = tmp; tmp.release();
        rightDisp.convertTo( tmp, CV_32FC1 ); rightDisp = tmp; tmp.release();

        int tempCode = processStereoMatchingResults( resFS, ci, isWrite,
                   leftImg, rightImg, trueLeftDisp, trueRightDisp, leftDisp, rightDisp, QualityEvalParams(ignBorder));
        code = tempCode==cvtest::TS::OK ? code : tempCode;
    }

    if( isWrite )
        resFS << "}"; // "stereo_matching"

    ts->set_failed_test_info( code );
}

void calcErrors( const Mat& leftImg, const Mat& /*rightImg*/,
                 const Mat& trueLeftDisp, const Mat& trueRightDisp,
                 const Mat& trueLeftUnknDispMask, const Mat& trueRightUnknDispMask,
                 const Mat& calcLeftDisp, const Mat& /*calcRightDisp*/,
                 vector<float>& rms, vector<float>& badPxlsFractions,
                 const QualityEvalParams& qualityEvalParams )
{
    Mat texturelessMask, texturedMask;
    computeTextureBasedMasks( leftImg, &texturelessMask, &texturedMask,
                              qualityEvalParams.texturelessWidth, qualityEvalParams.texturelessThresh );
    Mat occludedMask, nonOccludedMask;
    computeOcclusionBasedMasks( trueLeftDisp, trueRightDisp, &occludedMask, &nonOccludedMask,
                                trueLeftUnknDispMask, trueRightUnknDispMask, qualityEvalParams.dispThresh);
    Mat depthDiscontMask;
    computeDepthDiscontMask( trueLeftDisp, depthDiscontMask, trueLeftUnknDispMask,
                             qualityEvalParams.dispGap, qualityEvalParams.discontWidth);

    Mat borderedKnownMask = getBorderedMask( leftImg.size(), qualityEvalParams.ignoreBorder ) & ~trueLeftUnknDispMask;

    nonOccludedMask &= borderedKnownMask;
    occludedMask &= borderedKnownMask;
    texturedMask &= nonOccludedMask; // & borderedKnownMask
    texturelessMask &= nonOccludedMask; // & borderedKnownMask
    depthDiscontMask &= nonOccludedMask; // & borderedKnownMask

    rms.resize(ERROR_KINDS_COUNT);
    rms[0] = dispRMS( calcLeftDisp, trueLeftDisp, borderedKnownMask );
    rms[1] = dispRMS( calcLeftDisp, trueLeftDisp, nonOccludedMask );
    rms[2] = dispRMS( calcLeftDisp, trueLeftDisp, occludedMask );
    rms[3] = dispRMS( calcLeftDisp, trueLeftDisp, texturedMask );
    rms[4] = dispRMS( calcLeftDisp, trueLeftDisp, texturelessMask );
    rms[5] = dispRMS( calcLeftDisp, trueLeftDisp, depthDiscontMask );

    badPxlsFractions.resize(ERROR_KINDS_COUNT);
    badPxlsFractions[0] = badMatchPxlsFraction( calcLeftDisp, trueLeftDisp, borderedKnownMask, qualityEvalParams.badThresh );
    badPxlsFractions[1] = badMatchPxlsFraction( calcLeftDisp, trueLeftDisp, nonOccludedMask, qualityEvalParams.badThresh );
    badPxlsFractions[2] = badMatchPxlsFraction( calcLeftDisp, trueLeftDisp, occludedMask, qualityEvalParams.badThresh );
    badPxlsFractions[3] = badMatchPxlsFraction( calcLeftDisp, trueLeftDisp, texturedMask, qualityEvalParams.badThresh );
    badPxlsFractions[4] = badMatchPxlsFraction( calcLeftDisp, trueLeftDisp, texturelessMask, qualityEvalParams.badThresh );
    badPxlsFractions[5] = badMatchPxlsFraction( calcLeftDisp, trueLeftDisp, depthDiscontMask, qualityEvalParams.badThresh );
}

int CV_StereoMatchingTest::processStereoMatchingResults( FileStorage& fs, int caseIdx, bool isWrite,
              const Mat& leftImg, const Mat& rightImg,
              const Mat& trueLeftDisp, const Mat& trueRightDisp,
              const Mat& leftDisp, const Mat& rightDisp,
              const QualityEvalParams& qualityEvalParams )
{
    // rightDisp is not used in current test virsion
    int code = cvtest::TS::OK;
    assert( fs.isOpened() );
    assert( trueLeftDisp.type() == CV_32FC1 && (trueRightDisp.empty() || trueRightDisp.type() == CV_32FC1) );
    assert( leftDisp.type() == CV_32FC1 && rightDisp.type() == CV_32FC1 );

    // get masks for unknown ground truth disparity values
    Mat leftUnknMask, rightUnknMask;
    DatasetParams params = datasetsParams[caseDatasets[caseIdx]];
    absdiff( trueLeftDisp, Scalar(params.dispUnknVal), leftUnknMask );
    leftUnknMask = leftUnknMask < numeric_limits<float>::epsilon();
    assert(leftUnknMask.type() == CV_8UC1);
    if( !trueRightDisp.empty() )
    {
        absdiff( trueRightDisp, Scalar(params.dispUnknVal), rightUnknMask );
        rightUnknMask = rightUnknMask < numeric_limits<float>::epsilon();
        assert(leftUnknMask.type() == CV_8UC1);
    }

    // calculate errors
    vector<float> rmss, badPxlsFractions;
    calcErrors( leftImg, rightImg, trueLeftDisp, trueRightDisp, leftUnknMask, rightUnknMask,
                leftDisp, rightDisp, rmss, badPxlsFractions, qualityEvalParams );

    if( isWrite )
    {
        fs << caseNames[caseIdx] << "{";
        cvWriteComment( fs.fs, RMS_STR.c_str(), 0 );
        writeErrors( RMS_STR, rmss, &fs );
        cvWriteComment( fs.fs, BAD_PXLS_FRACTION_STR.c_str(), 0 );
        writeErrors( BAD_PXLS_FRACTION_STR, badPxlsFractions, &fs );
        fs << "}"; // datasetName
    }
    else // compare
    {
        ts->printf( cvtest::TS::LOG, "\nquality of case named %s\n", caseNames[caseIdx].c_str() );
        ts->printf( cvtest::TS::LOG, "%s\n", RMS_STR.c_str() );
        writeErrors( RMS_STR, rmss );
        ts->printf( cvtest::TS::LOG, "%s\n", BAD_PXLS_FRACTION_STR.c_str() );
        writeErrors( BAD_PXLS_FRACTION_STR, badPxlsFractions );

        FileNode fn = fs.getFirstTopLevelNode()[caseNames[caseIdx]];
        vector<float> validRmss, validBadPxlsFractions;

        readErrors( fn, RMS_STR, validRmss );
        readErrors( fn, BAD_PXLS_FRACTION_STR, validBadPxlsFractions );
        int tempCode = compareErrors( rmss, validRmss, rmsEps, RMS_STR );
        code = tempCode==cvtest::TS::OK ? code : tempCode;
        tempCode = compareErrors( badPxlsFractions, validBadPxlsFractions, fracEps, BAD_PXLS_FRACTION_STR );
        code = tempCode==cvtest::TS::OK ? code : tempCode;
    }
    return code;
}

int CV_StereoMatchingTest::readDatasetsParams( FileStorage& fs )
{
    if( !fs.isOpened() )
    {
        ts->printf( cvtest::TS::LOG, "datasetsParams can not be read " );
        return cvtest::TS::FAIL_INVALID_TEST_DATA;
    }
    datasetsParams.clear();
    FileNode fn = fs.getFirstTopLevelNode();
    assert(fn.isSeq());
    for( int i = 0; i < (int)fn.size(); i+=3 )
    {
        string nm = fn[i];
        DatasetParams params;
        string sf = fn[i+1]; params.dispScaleFactor = atoi(sf.c_str());
        string uv = fn[i+2]; params.dispUnknVal = atoi(uv.c_str());
        datasetsParams[nm] = params;
    }
    return cvtest::TS::OK;
}

int CV_StereoMatchingTest::readRunParams( FileStorage& fs )
{
    if( !fs.isOpened() )
    {
        ts->printf( cvtest::TS::LOG, "runParams can not be read " );
        return cvtest::TS::FAIL_INVALID_TEST_DATA;
    }
    caseNames.clear();;
    caseDatasets.clear();
    return cvtest::TS::OK;
}

void CV_StereoMatchingTest::writeErrors( const string& errName, const vector<float>& errors, FileStorage* fs )
{
    assert( (int)errors.size() == ERROR_KINDS_COUNT );
    vector<float>::const_iterator it = errors.begin();
    if( fs )
        for( int i = 0; i < ERROR_KINDS_COUNT; i++, ++it )
            *fs << ERROR_PREFIXES[i] + errName << *it;
    else
        for( int i = 0; i < ERROR_KINDS_COUNT; i++, ++it )
            ts->printf( cvtest::TS::LOG, "%s = %f\n", string(ERROR_PREFIXES[i]+errName).c_str(), *it );
}

void CV_StereoMatchingTest::readErrors( FileNode& fn, const string& errName, vector<float>& errors )
{
    errors.resize( ERROR_KINDS_COUNT );
    vector<float>::iterator it = errors.begin();
    for( int i = 0; i < ERROR_KINDS_COUNT; i++, ++it )
        fn[ERROR_PREFIXES[i]+errName] >> *it;
}

int CV_StereoMatchingTest::compareErrors( const vector<float>& calcErrors, const vector<float>& validErrors,
                   const vector<float>& eps, const string& errName )
{
    assert( (int)calcErrors.size() == ERROR_KINDS_COUNT );
    assert( (int)validErrors.size() == ERROR_KINDS_COUNT );
    assert( (int)eps.size() == ERROR_KINDS_COUNT );
    vector<float>::const_iterator calcIt = calcErrors.begin(),
                                  validIt = validErrors.begin(),
                                  epsIt = eps.begin();
    bool ok = true;
    for( int i = 0; i < ERROR_KINDS_COUNT; i++, ++calcIt, ++validIt, ++epsIt )
        if( *calcIt - *validIt > *epsIt )
        {
            ts->printf( cvtest::TS::LOG, "bad accuracy of %s (valid=%f; calc=%f)\n", string(ERROR_PREFIXES[i]+errName).c_str(), *validIt, *calcIt );
            ok = false;
        }
    return ok ? cvtest::TS::OK : cvtest::TS::FAIL_BAD_ACCURACY;
}

//----------------------------------- StereoGC test -----------------------------------------------------

class CV_StereoGCTest : public CV_StereoMatchingTest
{
public:
    CV_StereoGCTest()
    {
        name = "stereogc";
        fill(rmsEps.begin(), rmsEps.end(), 3.f);
        fracEps[0] = 0.05f; // all
        fracEps[1] = 0.05f; // noOccl
        fracEps[2] = 0.25f; // occl
        fracEps[3] = 0.05f; // textured
        fracEps[4] = 0.10f; // textureless
        fracEps[5] = 0.10f; // borderedDepthDiscont
    }
protected:
    struct RunParams
    {
        int ndisp;
        int iterCount;
    };
    vector<RunParams> caseRunParams;

    virtual int readRunParams( FileStorage& fs )
    {
        int code = CV_StereoMatchingTest::readRunParams(fs);
        FileNode fn = fs.getFirstTopLevelNode();
        assert(fn.isSeq());
        for( int i = 0; i < (int)fn.size(); i+=4 )
        {
            string caseName = fn[i], datasetName = fn[i+1];
            RunParams params;
            string ndisp = fn[i+2]; params.ndisp = atoi(ndisp.c_str());
            string iterCount = fn[i+3]; params.iterCount = atoi(iterCount.c_str());
            caseNames.push_back( caseName );
            caseDatasets.push_back( datasetName );
            caseRunParams.push_back( params );
        }
        return code;
    }

    virtual int runStereoMatchingAlgorithm( const Mat& _leftImg, const Mat& _rightImg,
                   Mat& leftDisp, Mat& rightDisp, int caseIdx )
    {
        RunParams params = caseRunParams[caseIdx];
        assert( _leftImg.type() == CV_8UC3 && _rightImg.type() == CV_8UC3 );
        Mat leftImg, rightImg, tmp;
        cvtColor( _leftImg, leftImg, CV_BGR2GRAY );
        cvtColor( _rightImg, rightImg, CV_BGR2GRAY );

        leftDisp.create( leftImg.size(), CV_16SC1 );
        rightDisp.create( rightImg.size(), CV_16SC1 );

        CvMat _limg = leftImg, _rimg = rightImg, _ldisp = leftDisp, _rdisp = rightDisp;
        CvStereoGCState *state = cvCreateStereoGCState( params.ndisp, params.iterCount );
        cvFindStereoCorrespondenceGC( &_limg, &_rimg, &_ldisp, &_rdisp, state );
        cvReleaseStereoGCState( &state );

        leftDisp = - leftDisp;
        return 0;
    }

};


TEST(Legacy_StereoGC, regression) { CV_StereoGCTest test; test.safe_run(); }
