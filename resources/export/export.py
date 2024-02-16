import io
import json
import sys
from dataclasses import dataclass
from enum import Enum
from pathlib import Path
from typing import BinaryIO

import cstruct

from windef import (
    STRINGS_BLOCK_SIZE,
    POPUP,
    ENDMENU,
    HELP,
    BitmapFileHeader,
    BitmapInfoHeader,
    BitmapV4Header,
    BitmapV5Header,
    CieXYZ,
    CieXYZTriple,
    CursorDir,
    IconDirectoryEntry,
    IconResDir,
    NewHeader,
    ResDir,
    ResourceHeaderPost,
    ResourceHeaderPre,
    ResourceType,
)


@dataclass
class ResourceItem:
    name: str
    type: ResourceType
    data: bytes


base_path = Path.cwd() / ".."
bitmaps_dir = base_path / "bitmaps"
icons_dir = base_path / "icons"
strings_dir = base_path / "strings"
menus_dir = base_path / "menus"


def read_word(f: BinaryIO) -> int:
    return ord(f.read(1)) + ord(f.read(1)) * 256


def read_str_or_id(f: BinaryIO) -> str | int:
    chars = [f.read(1), f.read(1)]
    if chars == [b"\xff", b"\xff"]:
        return read_word(f)
    while True:
        c0 = f.read(1)
        c1 = f.read(1)
        if c0 == b"\x00":
            # print(chars)
            return (b"".join(chars)).decode("UTF-16LE")
        chars.append(c0)
        chars.append(c1)


def read_res(filename: str):
    res_file = Path(filename)
    with res_file.open("rb") as f:
        while True:
            pos = f.tell()
            pre_data = f.read(len(ResourceHeaderPre))
            if not pre_data:
                break
            pre = ResourceHeaderPre(pre_data)
            res_type = read_str_or_id(f)
            res_name = read_str_or_id(f)
            # print(f"{pre} {res_type=} {res_name=}")
            post = ResourceHeaderPost(f.read(len(ResourceHeaderPost)))
            pos2 = f.tell()
            f.read(pre.HeaderSize - pos2 + pos)
            data = f.read(pre.DataSize)
            if pre.DataSize % 4:
                padding = 4 - (pre.DataSize % 4)
                f.read(padding)
            yield ResourceItem(name=res_name, type=ResourceType(res_type), data=data)


def export_icon(
    resource: ResourceItem,
    resources: list[ResourceItem],
    icons: dict[str, ResourceItem],
) -> None:
    icons_dir.mkdir(exist_ok=True)
    header = NewHeader(resource.data)
    res_dir = ResDir(resource.data[len(header) :])
    # print(res_dir.Icon, res_dir.IconCursorId)
    icon = icons.get(res_dir.IconCursorId)
    icon_dir_entry = IconDirectoryEntry()
    icon_dir_entry.bWidth = res_dir.Icon.Width
    icon_dir_entry.bHeight = res_dir.Icon.Width
    icon_dir_entry.wBitCount = res_dir.BitCount
    icon_dir_entry.wPlanes = res_dir.Planes
    bColorCount = 1 << (icon_dir_entry.wBitCount * icon_dir_entry.wPlanes)
    icon_dir_entry.bColorCount = bColorCount if bColorCount < 8 else 0
    icon_dir_entry.dwBytesInRes = res_dir.BytesInRes
    icon_dir_entry.dwImageOffset = len(header) + len(icon_dir_entry)
    icon_file = icons_dir / f"{resource.name}.ico"
    icon_file.write_bytes(header.pack() + icon_dir_entry.pack() + icon.data)


def export_bitmap(resource: ResourceItem, resources: list[ResourceItem]) -> None:
    bitmaps_dir.mkdir(exist_ok=True)
    bitmap_header = BitmapInfoHeader(resource.data)
    header = BitmapFileHeader(
        bfType=0x4D42,  # BM
        bfSize=len(resource.data),
        bfReserved1=0,
        bfReserved2=0,
        bfOffBits=len(BitmapFileHeader)
        + len(resource.data)
        - bitmap_header.biSizeImage,
        # bfOffBits=bitmap_header.biSize + len(BitmapFileHeader),
    )
    # print(header, len(header))
    bitmap_file = bitmaps_dir / f"{resource.name}.bmp"
    bitmap_file.write_bytes(header.pack() + resource.data)


def export_strings(resource: ResourceItem, resources: list[ResourceItem]) -> None:
    strings_dir.mkdir(exist_ok=True)
    base = (resource.name - 1) << 4
    data = {}
    with io.BytesIO(resource.data) as f:
        for i in range(0, STRINGS_BLOCK_SIZE):
            string_len = read_word(f)
            if string_len:
                string_id = base + i
                t = f.read(string_len * 2).decode("UTF-16LE")
                # print(f"{string_id=} {t=}")
                data[str(string_id)] = t
    strings_file = strings_dir / f"{resource.name}.json"
    with strings_file.open("w", encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=4)
        f.write("\n")


def export_menu(
    resource: ResourceItem,
    resources: list[ResourceItem],
) -> None:
    menus_dir.mkdir(exist_ok=True)
    with io.BytesIO(resource.data) as f:
        with (menus_dir / f"{resource.name}.rc").open("w") as menu_rc:
            menu_rc.write(f"{resource.name} MENU\n{{\n")
            read_word(f)
            read_word(f)
            menu_stack = [[]]
            while True:
                try:
                    flags = read_word(f)
                except:
                    break
                if flags & POPUP:
                    menu_id = -1
                    menu = {
                        "kind": "popup",
                        "label": read_str_or_id(f),
                        "items": [],
                    }
                    menu_stack[-1].append(menu)
                    rc_flags = ""
                    if flags & HELP:
                        menu["flags"] = ["help"]
                        rc_flags = ", HELP"
                    menu_stack.append(menu_stack[-1][-1]["items"])
                    menu_rc.write(f"  POPUP \"{menu['label']}\"\n")
                    menu_rc.write("  {\n")
                    continue
                else:
                    menu_id = read_word(f)
                if flags == 0 and menu_id == 0:
                    _ = read_word(f)
                    menu_stack[-1].append(
                        {
                            "kind": "separator",
                            "label": "",
                        }
                    )
                    menu_rc.write("    MENUITEM SEPARATOR\n")
                else:
                    menu = {
                        "kind": "menuitem",
                        "label": read_str_or_id(f),
                        "menu_id": menu_id,
                    }
                    menu_stack[-1].append(menu)
                    menu_rc.write(
                        f"    MENUITEM \"{menu['label']}\", {menu['menu_id']}\n"
                    )
                if flags & ENDMENU:
                    menu_stack.pop()
                    menu_rc.write("  }\n\n")
            menu_rc.write("}\n")
    menu_file = menus_dir / f"{resource.name}.json"
    with menu_file.open("w", encoding='utf-8') as f:
        json.dump(menu_stack[0], f, ensure_ascii=False, indent=4)
        f.write("\n")


def main() -> None:
    for filename in sys.argv[1:]:
        resources = list(read_res(filename))
        icons = {x.name: x for x in resources if x.type == ResourceType.icon}

        for resource in resources:
            print(f"{resource.name} {resource.type}")
            if resource.type == ResourceType.group_icon:
                export_icon(resource, resources, icons)
            elif resource.type == ResourceType.bitmap:
                export_bitmap(resource, resources)
            elif resource.type == ResourceType.string:
                export_strings(resource, resources)
            elif resource.type == ResourceType.menu:
                export_menu(resource, resources)


if __name__ == "__main__":
    main()
