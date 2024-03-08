from enum import Enum

import cstruct

__all__ = [
    "STRINGS_BLOCK_SIZE",
    "POPUP",
    "ENDMENU",
    "HELP",
    "ResourceType",
    "ResourceHeaderPre",
    "ResourceHeaderPost",
    "NewHeader",
    "IconResDir",
    "CursorDir",
    "ResDir",
    "IconDirectoryEntry",
    "BitmapFileHeader",
    "CieXYZ",
    "CieXYZTriple",
    "BitmapInfoHeader",
    "BitmapV4Header",
    "BitmapV5Header",
]

STRINGS_BLOCK_SIZE = 16
POPUP = 0x0010
ENDMENU = 0x0080
HELP = 0x4000


class ResourceType(Enum):
    none = 0
    cursor = 1
    bitmap = 2
    icon = 3
    menu = 4
    dialog = 5
    string = 6
    fontdir = 7
    font = 8
    accelerator = 9
    rcdata = 10
    messagetable = 11
    group_cursor = 12
    group_icon = 14
    version = 16
    dlginclude = 17
    plugplay = 19
    vxd = 20
    anicursor = 21
    aniicon = 22
    html = 23
    manifest = 24


cstruct.parse(
    """
    typedef unsigned char BYTE;
    typedef unsigned short WORD;
    typedef unsigned long DWORD;
    typedef long LONG;
    typedef long FXPT2DOT30;
"""
)


class ResourceHeaderPre(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            DWORD DataSize;
            DWORD HeaderSize;
        }
    """


class ResourceHeaderPost(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            DWORD DataVersion;
            WORD  MemoryFlags;
            WORD  LanguageId;
            DWORD Version;
            DWORD Characteristics;
        }
    """


class NewHeader(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            WORD Reserved;
            WORD ResType;
            WORD ResCount;
        }
    """


class IconResDir(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            BYTE Width;
            BYTE Height;
            BYTE ColorCount;
            BYTE reserved;
        }
    """


class CursorDir(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            BYTE Width;
            BYTE Height;
        }
    """


class ResDir(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        typedef struct IconResDir ICONRESDIR;
        typedef struct CursorDir CURSORDIR;

        struct {
            union
            {
                ICONRESDIR   Icon;
                CURSORDIR    Cursor;
            };
            WORD       Planes;
            WORD       BitCount;
            DWORD      BytesInRes;
            WORD       IconCursorId;
        }
    """


class IconDirectoryEntry(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            BYTE  bWidth;
            BYTE  bHeight;
            BYTE  bColorCount;
            BYTE  bReserved;
            WORD  wPlanes;
            WORD  wBitCount;
            DWORD dwBytesInRes;
            DWORD dwImageOffset;
        }
    """


class BitmapFileHeader(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            WORD  bfType;
            DWORD bfSize;
            WORD  bfReserved1;
            WORD  bfReserved2;
            DWORD bfOffBits;
        }
    """


class CieXYZ(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            FXPT2DOT30 ciexyzX;
            FXPT2DOT30 ciexyzY;
            FXPT2DOT30 ciexyzZ;
        }
    """


class CieXYZTriple(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        typedef struct CieXYZ CIEXYZ;

        struct {
            CIEXYZ ciexyzRed;
            CIEXYZ ciexyzGreen;
            CIEXYZ ciexyzBlue;
        }
    """


class BitmapInfoHeader(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        typedef struct CieXYZTriple CIEXYZTRIPLE;

        struct {
            DWORD biSize;
            LONG  biWidth;
            LONG  biHeight;
            WORD  biPlanes;
            WORD  biBitCount;
            DWORD biCompression;
            DWORD biSizeImage;
            LONG  biXPelsPerMeter;
            LONG  biYPelsPerMeter;
            DWORD biClrUsed;
            DWORD biClrImportant;
        }
    """


class BitmapV4Header(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
          DWORD        bV4Size;
          LONG         bV4Width;
          LONG         bV4Height;
          WORD         bV4Planes;
          WORD         bV4BitCount;
          DWORD        bV4V4Compression;
          DWORD        bV4SizeImage;
          LONG         bV4XPelsPerMeter;
          LONG         bV4YPelsPerMeter;
          DWORD        bV4ClrUsed;
          DWORD        bV4ClrImportant;
          DWORD        bV4RedMask;
          DWORD        bV4GreenMask;
          DWORD        bV4BlueMask;
          DWORD        bV4AlphaMask;
          DWORD        bV4CSType;
          CIEXYZTRIPLE bV4Endpoints;
          DWORD        bV4GammaRed;
          DWORD        bV4GammaGreen;
          DWORD        bV4GammaBlue;
        }
    """


class BitmapV5Header(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
          DWORD        bV5Size;
          LONG         bV5Width;
          LONG         bV5Height;
          WORD         bV5Planes;
          WORD         bV5BitCount;
          DWORD        bV5Compression;
          DWORD        bV5SizeImage;
          LONG         bV5XPelsPerMeter;
          LONG         bV5YPelsPerMeter;
          DWORD        bV5ClrUsed;
          DWORD        bV5ClrImportant;
          DWORD        bV5RedMask;
          DWORD        bV5GreenMask;
          DWORD        bV5BlueMask;
          DWORD        bV5AlphaMask;
          DWORD        bV5CSType;
          CIEXYZTRIPLE bV5Endpoints;
          DWORD        bV5GammaRed;
          DWORD        bV5GammaGreen;
          DWORD        bV5GammaBlue;
          DWORD        bV5Intent;
          DWORD        bV5ProfileData;
          DWORD        bV5ProfileSize;
          DWORD        bV5Reserved;
        }
    """
