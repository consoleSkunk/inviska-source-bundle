#include "IMKVCodecLookup.h"

/*******************************************************
Codec ID            Extension       Name
********************************************************
V_MPEG4/ISO/AVC     h264            H.264 AVC
V_MPEGH/ISO/HEVC    h265            H.265 HEVC
V_MS/VFW/FOURCC     avi             DivX/Xvid
V_VP8               ivf             VP8
V_VP9               ivf             VP9
V_MPEG1             mpg             MPEG1
V_MPEG2             mpg             MPEG2
V_THEORA            ogg             Theora
V_REAL *            rm              RealVideo

V_UNCOMPRESSED      [Unsupported]   Uncompressed
V_MPEG4/ISO/SP      [Unsupported]   DivX4
V_MPEG4/ISO/ASP     [Unsupported]   DivX5/Xvid/FFMPEG
V_MPEG4/MS/V3       [Unsupported]   Microsoft MPEG4 V3
V_QUICKTIME         [Unsupported]   QuickTime Video
V_PRORES            [Unsupported]   Apple ProRes


A_FLAC              flac            FLAC
A_DTS *             dts             DTS
A_AC3 *             ac3             AC3 Dolby Digital
A_AAC *             aac             AAC
A_VORBIS            ogg             Vorbis
A_MPEG/L3           mp3             MP3 MPEG Layer 3
A_MPEG/L2           mp2             MP2 MPEG Layer 2
A_MPEG/L1           mp1             MP3 MPEG Layer 1
A_PCM/INT/LIT       wav             PCM Little Endian
A_PCM/INT/BIG       wav             PCM Big Endian
A_WAVPACK4          wv              WavPack
A_OPUS              opus            Opus
A_ALAC              caf             Apple Lossless
A_TTA1              tta             TTA
A_REAL *            rm              RealAudio

A_PCM/FLOAT/IEEE    [Unsupported]   PCM Floating Point
A_MPC               [Unsupported]   MusePack
A_MS/ACM            [Unsupported]   Microsoft ACM
A_QUICKTIME         [Unsupported]   QuickTime Audio


S_TEXT/ASS          ass             ASS Advanced Substation Alpha
S_TEXT/SSA          ssa             SSA Substation Alpha
S_TEXT/UTF8         srt             SRT
S_VOBSUB            sub             VobSub
S_KATE              ogg             KATE
S_TEXT/USF          usf             USF
S_HDMV/PGS          sup             PSG
S_TEXT/WEBVTT       wvtt            WebVTT

S_IMAGE/BMP         [Unsupported]   Bitmap


Tags                xml
Chapters            xml
Timestamps          txt

https://MKVToolNix.download/doc/mkvextract.html
https://matroska.org/technical/specs/codecid/index.html

*********************************************************************************/



void IMKVCodecLookup::LookupCodec(const QString & krqstrCodecID, QString & rqstrCodecName, bool & rbSupported)
{
    rqstrCodecName.clear();
    rbSupported = false;

    if (krqstrCodecID.startsWith("V_"))
    {
        if       (krqstrCodecID == "V_MPEG4/ISO/AVC")
            rqstrCodecName = "H.264 AVC";
        else if  (krqstrCodecID == "V_MPEGH/ISO/HEVC")
            rqstrCodecName = "H.265 HEVC";
        else if  (krqstrCodecID == "V_MS/VFW/FOURCC")
            rqstrCodecName = "DivX/Xvid";
        else if  (krqstrCodecID == "V_VP8")
            rqstrCodecName = "VP8";
        else if  (krqstrCodecID == "V_VP9")
            rqstrCodecName = "VP9";
        else if  (krqstrCodecID == "V_MPEG1")
            rqstrCodecName = "MPEG1";
        else if  (krqstrCodecID == "V_MPEG2")
            rqstrCodecName = "MPEG2";
        else if  (krqstrCodecID == "V_THEORA")
            rqstrCodecName = "Theora";
        else if  (krqstrCodecID.startsWith("V_REAL"))
            rqstrCodecName = "RealVideo";

        if (rqstrCodecName.isNull() == false)
        {
            rbSupported = true;
            return;
        }

        if       (krqstrCodecID == "V_UNCOMPRESSED")
            rqstrCodecName = "Uncompressed";
        else if  (krqstrCodecID == "V_MPEG4/ISO/SP")
            rqstrCodecName = "DivX4";
        else if  (krqstrCodecID == "V_MPEG4/ISO/ASP")
            rqstrCodecName = "DivX5/Xvid/FFMPEG";
        else if  (krqstrCodecID == "V_MPEG4/MS/V3")
            rqstrCodecName = "Microsoft MPEG4 V3";
        else if  (krqstrCodecID == "V_QUICKTIME")
            rqstrCodecName = "QuickTime Video";
        else if  (krqstrCodecID == "V_PRORES")
            rqstrCodecName = "Apple ProRes";
        else
            rqstrCodecName = "Unknown Video";
    }
    else if (krqstrCodecID.startsWith("A_"))
    {
        if       (krqstrCodecID == "A_FLAC")
            rqstrCodecName = "FLAC";
        else if  (krqstrCodecID.startsWith("A_DTS"))
            rqstrCodecName = "DTS";
        else if  (krqstrCodecID.startsWith("A_AC3"))
            rqstrCodecName = "AC3 Dolby Digital";
        else if  (krqstrCodecID.startsWith("A_AAC"))
            rqstrCodecName = "AAC";
        else if  (krqstrCodecID == "A_VORBIS")
            rqstrCodecName = "Vorbis";
        else if  (krqstrCodecID == "A_MPEG/L3")
            rqstrCodecName = "MP3 MPEG Layer 3";
        else if  (krqstrCodecID == "A_MPEG/L2")
            rqstrCodecName = "MP2 MPEG Layer 3";
        else if  (krqstrCodecID == "A_MPEG/L1")
            rqstrCodecName = "MP1 MPEG Layer 1";
        else if  (krqstrCodecID == "A_PCM/INT/LIT")
            rqstrCodecName = "PCM Little Endian";
        else if  (krqstrCodecID == "A_PCM/INT/BIG")
            rqstrCodecName = "PCM Big Endian";
        else if  (krqstrCodecID == "A_WAVPACK4")
            rqstrCodecName = "WavPack";
        else if  (krqstrCodecID == "A_OPUS")
            rqstrCodecName = "Opus";
        else if  (krqstrCodecID == "A_ALAC")
            rqstrCodecName = "Apple Lossless";
        else if  (krqstrCodecID == "A_TTA1")
            rqstrCodecName = "TTA";
        else if  (krqstrCodecID.startsWith("A_REAL/"))
            rqstrCodecName = "RealAudio";

        if (rqstrCodecName.isNull() == false)
        {
            rbSupported = true;
            return;
        }

        if       (krqstrCodecID == "A_PCM/FLOAT/IEEE")
            rqstrCodecName = "PCM Floating Point";
        else if  (krqstrCodecID == "A_MPC")
            rqstrCodecName = "MusePack";
        else if  (krqstrCodecID == "A_MS/ACM")
            rqstrCodecName = "Microsoft ACM";
        else if  (krqstrCodecID == "A_QUICKTIME")
            rqstrCodecName = "QuickTime Audio";
        else
            rqstrCodecName = "Unknown Audio";
    }
    else if (krqstrCodecID.startsWith("S_"))
    {
        if       (krqstrCodecID == "S_TEXT/ASS")
            rqstrCodecName = "ASS";
        else if  (krqstrCodecID == "S_TEXT/SSA")
            rqstrCodecName = "SSA";
        else if  (krqstrCodecID == "S_TEXT/UTF8")
            rqstrCodecName = "SRT";
        else if  (krqstrCodecID == "S_VOBSUB")
            rqstrCodecName = "VobSub";
        else if  (krqstrCodecID == "S_KATE")
            rqstrCodecName = "KATE";
        else if  (krqstrCodecID == "S_TEXT/USF")
            rqstrCodecName = "USF";
        else if  (krqstrCodecID == "S_HDMV/PGS")
            rqstrCodecName = "PSG";
        else if  (krqstrCodecID == "S_TEXT/WEBVTT")
            rqstrCodecName = "WebVTT";

        if (rqstrCodecName.isNull() == false)
        {
            rbSupported = true;
            return;
        }

        if       (krqstrCodecID == "S_IMAGE/BMP")
            rqstrCodecName = "BMP";
        else
            rqstrCodecName = "Unknown Sub";
    }
    else
    {
        rqstrCodecName = "Unknown";
    }
}


bool IMKVCodecLookup::CodecSupported(const QString & krqstrCodecID)
{
    if (krqstrCodecID.startsWith("V_"))
    {
        if (krqstrCodecID == "V_UNCOMPRESSED" ||
            krqstrCodecID == "V_MPEG4/ISO/SP" ||
            krqstrCodecID == "V_MPEG4/ISO/ASP" ||
            krqstrCodecID == "V_MPEG4/MS/V3" ||
            krqstrCodecID == "V_QUICKTIME" ||
            krqstrCodecID == "V_PRORES")
        {
            return false;
        }

    }
    else if (krqstrCodecID.startsWith("A_"))
    {
        if (krqstrCodecID == "A_PCM/FLOAT/IEEE" ||
            krqstrCodecID == "A_MPC" ||
            krqstrCodecID == "A_MS/ACM" ||
            krqstrCodecID == "A_QUICKTIME")
        {
            return false;
        }

    }
    else if (krqstrCodecID.startsWith("S_"))
    {
        if  (krqstrCodecID == "S_IMAGE/BMP")
            return false;
    }

    return true;
}


QString IMKVCodecLookup::LookupExtension(const QString & krqstrCodecID)
{
    if (krqstrCodecID.startsWith("V_"))
    {
        if       (krqstrCodecID == "V_MPEG4/ISO/AVC")
            return "h264";
        else if  (krqstrCodecID == "V_MPEGH/ISO/HEVC")
            return "h265";
        else if  (krqstrCodecID == "V_MS/VFW/FOURCC")
            return "avi";
        else if  (krqstrCodecID == "V_VP8")
            return "ivf";
        else if  (krqstrCodecID == "V_VP9")
            return "ivf";
        else if  (krqstrCodecID == "V_MPEG1")
            return "mpg";
        else if  (krqstrCodecID == "V_MPEG2")
            return "mpg";
        else if  (krqstrCodecID == "V_THEORA")
            return "ogg";
        else if  (krqstrCodecID.startsWith("V_REAL/"))
            return "rm";
        else
            return "unknown_video";
    }
    else if (krqstrCodecID.startsWith("A_"))
    {
        if       (krqstrCodecID == "A_FLAC")
            return "flac";
        else if  (krqstrCodecID.startsWith("A_DTS"))
            return "dts";
        else if  (krqstrCodecID.startsWith("A_AC3"))
            return "ac3";
        else if  (krqstrCodecID.startsWith("A_AAC"))
            return "aac";
        else if  (krqstrCodecID == "A_VORBIS")
            return "ogg";
        else if  (krqstrCodecID == "A_MPEG/L3")
            return "mp3";
        else if  (krqstrCodecID == "A_MPEG/L2")
            return "mp2";
        else if  (krqstrCodecID == "A_MPEG/L1")
            return "mp1";
        else if  (krqstrCodecID == "A_PCM/INT/LIT")
            return "wav";
        else if  (krqstrCodecID == "A_PCM/INT/BIG")
            return "wav";
        else if  (krqstrCodecID == "A_WAVPACK4")
            return "wv";
        else if  (krqstrCodecID == "A_OPUS")
            return "opus";
        else if  (krqstrCodecID == "A_ALAC")
            return "caf";
        else if  (krqstrCodecID == "A_TTA1")
            return "tta";
        else if  (krqstrCodecID.startsWith("A_REAL"))
            return "rm";
        else
            return "unknown_audio";
    }
    else if (krqstrCodecID.startsWith("S_"))
    {
        if       (krqstrCodecID == "S_TEXT/ASS")
            return "ass";
        else if  (krqstrCodecID == "S_TEXT/SSA")
            return "ssa";
        else if  (krqstrCodecID == "S_TEXT/UTF8")
            return "srt";
        else if  (krqstrCodecID == "S_VOBSUB")
            return "sub";
        else if  (krqstrCodecID == "S_KATE")
            return "ogg";
        else if  (krqstrCodecID == "S_TEXT/USF")
            return "usf";
        else if  (krqstrCodecID == "S_HDMV/PGS")
            return "sup";
        else if  (krqstrCodecID == "S_TEXT/WEBVTT")
            return "wvtt";
        else
            return "unknown_sub";
    }
    else
    {
        return "unknown";
    }
}

