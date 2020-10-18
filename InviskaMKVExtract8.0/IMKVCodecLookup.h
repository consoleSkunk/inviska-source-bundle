#ifndef IMKVCodecLookup_h
#define IMKVCodecLookup_h

#include <QString>


class IMKVCodecLookup
{
public:
    // Returns codec name based on CodecID
    static void LookupCodec(const QString & krqstrCodecID, QString & rqstrCodecName, bool & rbSupported);

    // Determines if codec is supported for extraction
    static bool CodecSupported(const QString & krqstrCodecID);

    // Returns file extension for codec when extracted
    static QString LookupExtension(const QString & krqstrCodecID);
};

#endif // IMKVCodecLookup_h
