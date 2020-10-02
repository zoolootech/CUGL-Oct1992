
#ifndef PROFILE_H

#define MAX_PATHNAME			64
#define MAX_SEGMENTS			128

#define PRF_FILE_ID			"bw"
#define PRF_FILE_VER			2
#define MAX_SEGNAME_LEN 		80
#define MAX_PUBNAME_LEN 		40
#define MAX_MODNAME_LEN 		16
#define SPACE				' '

#define PRF_ID_REC			0
#define PRF_HDR_REC			1
#define PRF_SEG_REC			2
#define PRF_MOD_REC			3
#define PRF_PUB_REC			4
#define PRF_LIN_REC			5
#define PRF_EOF_REC			6

#define RPT_OTH_REC			0
#define RPT_BIO_REC			1
#define RPT_DOS_REC			2
#define RPT_HIT_REC			3
#define RPT_SEG_REC			4
#define RPT_PUB_REC			5
#define RPT_LIN_REC			6
#define RPT_EOF_REC			7

#define REC_LEN 			PrfRec.Pfx.uRecLen
#define REC_TYPE			PrfRec.Pfx.cRecType

#define FILE_ID 			PrfRec.Rec.Id.cFileId
#define FILE_VER			PrfRec.Rec.Id.uFileVer

#define NO_SEGS 			PrfRec.Rec.Hdr.uNoSegs
#define NO_PUBS 			PrfRec.Rec.Hdr.uNoPubs
#define NO_LINS 			PrfRec.Rec.Hdr.uNoLins
#define NO_MODS 			PrfRec.Rec.Hdr.uNoMods

#define SEG_VAL 			PrfRec.Rec.Seg.uSegVal
#define SEG_LEN 			PrfRec.Rec.Seg.uSegLen
#define SEG_NAME_LEN			PrfRec.Rec.Seg.cSegNameLen
#define SEG_CLASS_LEN			PrfRec.Rec.Seg.cSegClassLen
#define SEG_NAMES			PrfRec.Rec.Seg.cSegNames

#define MOD_NUM 			PrfRec.Rec.Mod.cModNum
#define MOD_NAME_LEN			PrfRec.Rec.Mod.cModNameLen
#define MOD_NAME			PrfRec.Rec.Mod.cModName

#define PUB_SEG 			PrfRec.Rec.Pub.uPubSeg
#define PUB_OFS 			PrfRec.Rec.Pub.uPubOfs
#define PUB_ABS 			PrfRec.Rec.Pub.cAbs
#define PUB_NAME_LEN			PrfRec.Rec.Pub.cPubNameLen
#define PUB_NAME			PrfRec.Rec.Pub.cPubName

#define LIN_MOD_NUM			PrfRec.Rec.Lin.uLinModNum
#define LIN_NO				PrfRec.Rec.Lin.uLinNo
#define LIN_SEG 			PrfRec.Rec.Lin.uLinSeg
#define LIN_OFS 			PrfRec.Rec.Lin.uLinOfs


typedef unsigned long	       ulong;
typedef unsigned short	       ushort;
typedef unsigned char	       uchar;
typedef short		       bool;


struct sPrfFileID
{
    char	    cFileId[2];
    ushort	    uFileVer;
};
typedef struct sPrfFileID	FILEID, *PFILEID, far *LPFILEID;


struct sPrfFileHdr
{
    ushort	    uNoSegs;
    ushort	    uNoPubs;
    ushort	    uNoLins;
    ushort	    uNoMods;
};
typedef struct sPrfFileHdr	FILEHDR, *PFILEHDR, far *LPFILEHDR;


struct sPrfFileSeg
{
    ushort	    uSegVal;
    ushort	    uSegLen;
    uchar	    cSegNameLen;
    uchar	    cSegClassLen;
    char	    cSegNames[MAX_SEGNAME_LEN];
};
typedef struct sPrfFileSeg	FILESEG, *PFILESEG, far *LPFILESEG;


struct sPrfFileMod
{
    uchar	    cModNum;
    uchar	    cModNameLen;
    char	    cModName[MAX_MODNAME_LEN];
};
typedef struct sPrfFileMod	FILEMOD, *PFILEMOD, far *LPFILEMOD;


struct sPrfFilePub
{
    ushort	    uPubSeg;
    ushort	    uPubOfs;
    uchar	    cAbs;
    uchar	    cPubNameLen;
    char	    cPubName[MAX_PUBNAME_LEN];
};
typedef struct sPrfFilePub	FILEPUB, *PFILEPUB, far *LPFILEPUB;


struct sPrfFileLin
{
    ushort	    uLinModNum;
    ushort	    uLinNo;
    ushort	    uLinSeg;
    ushort	    uLinOfs;
};
typedef struct sPrfFileLin	FILELIN, *PFILELIN, far *LPFILELIN;


struct sPrfFILEPFX
{
    ushort	    uRecLen;
    uchar	    cRecType;
    uchar	    cFiller;
};
typedef struct sPrfFILEPFX   FILEPFX, *PFILEPFX, far *LPFILEPFX;


struct sPrfFileRec
{
    FILEPFX	    Pfx;
    union
    {
	FILEID	    Id;
	FILEHDR     Hdr;
	FILESEG     Seg;
	FILEMOD     Mod;
	FILEPUB     Pub;
	FILELIN     Lin;
    }		    Rec;
};
typedef struct sPrfFileRec   FILEREC, *PFILEREC, far *LPFILEREC;


struct sRptFileRec
{
    uchar	    cRecType;
    uchar	    cFiller;
    ushort	    uSegOfs;
    ulong	    lCnt;
};
typedef struct sRptFileRec   RPTREC, *PRPTREC, far *LPRPTREC;

#endif
