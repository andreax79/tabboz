import re
import sys
import json
from pathlib import Path


PRE = """
<!DOCTYPE html>
<html>
<head>
  <title>{title}</title>
  <meta charset="UTF-8" />
  <base href="../../.." />
  <link rel="stylesheet" href="resources/dialogs/css/98.css" />
  <link rel="stylesheet" href="resources/dialogs/css/twcc.css" />
  <script src="resources/dialogs/js/twcc.js"></script>
  <script>
    document.addEventListener("DOMContentLoaded", () => {{
        addMainMenu(document.querySelector('ul.main-menu'));
        makeDraggable(document.querySelector("body > div.window"));
    }});
  </script>
  <style>
  </style>
</head>

<body>
"""

TITLE_BAR_CLOSE = """
"""

PRE_WIN = """
  <div class="{classes}" style="position: absolute; margin: 32px; width: {width}px; height: {height}px">
    <div class="title-bar">
      <div class="title-bar-text">
        {title}
      </div>
      <div class="title-bar-controls">
        {controls}
      </div>
    </div>
    <div id="menubar">{menu}</div>
    <div class="window-body; position: relative;">
"""

POST_WIN = """
    </div>
  </div>
"""

POST = """
</body>
</html>
"""

SCALE = 2
MENU_HEIGHT = 18
PATTERN = re.compile(r'(?:[^,"]|"(?:\\.|[^"])*")*')

includes_dir = Path.cwd() / "includes"
html_dir = Path.cwd() / "html"
includes_dir.mkdir(parents=True, exist_ok=True)
html_dir.mkdir(parents=True, exist_ok=True)


def strip(list_of_strings: list[str]) -> list[str]:
    return [x.strip() for x in list_of_strings]


def split_string(string: str) -> list[str]:
    matches = PATTERN.findall(string)
    result = [match.strip() for match in matches if match]
    return result


def unquote(string: str) -> str:
    return string.lstrip('"').rstrip('"')


def prepare_text(text: str, alt: bool = True) -> str:
    text = unquote(text)
    text = text.replace("\\n", "\n").replace('\\"', '"')
    if alt:
        return re.sub(
            r"([^&]|^)&([^&\s])", r'\1<span class="hotkey">\2</span>', text
        ).replace("&&", "&")
    else:
        return text.replace("&", "")


class Dialog:
    dialog_id: int | None = None
    elements: list[str] | None = None
    title: str | None = None
    style: list[str] | None = None
    menu: str | None = None
    width: int | None = None
    height: int | None = None
    dx: int = 4
    dy: int = 20 + 4
    menu: str = ""

    def __init__(self, dialog_id: str, line: str) -> None:
        self.elements = []
        self.dialog_id = dialog_id
        _, _, width, height = [x.strip() for x in line[6:].split(",")]
        self.width = int(width) * SCALE + self.dx - SCALE
        self.height = int(height) * SCALE + self.dy - SCALE

    def add(self, element: str) -> None:
        self.elements.insert(0, element)

    @property
    def classes(self) -> str:
        classes = "window"
        if "WS_DLGFRAME" in self.style:
            classes += " dlgframe"
        return classes

    @property
    def controls(self) -> str:
        controls = ""
        if "WS_MINIMIZEBOX" in self.style:
            controls += '<button class="control61472" aria-label="Minimize"></button>'
            controls += '<button class="control61488" disabled="" aria-label="Maximize"></button>'
        if "WS_SYSMENU" in self.style:
            controls += '<button class="control61536" aria-label="Close"></button>'
        else:
            controls += '<button class="control61536" disabled="" aria-label="Close"></button>'
        return controls

    def write(self) -> None:
        with (html_dir / f"{self.dialog_id}.html").open("w") as out:
            out.write(PRE.format(title=self.title))
            out.write(
                PRE_WIN.format(
                    title=self.title,
                    width=self.width,
                    height=self.height,
                    controls=self.controls,
                    menu=self.menu,
                    classes=self.classes,
                )
            )
            for element in self.elements:
                out.write(element)
            out.write(POST_WIN)
            out.write(POST)
        with (includes_dir / f"{self.dialog_id}.inc.html").open("w") as out:
            out.write(
                PRE_WIN.format(
                    title=self.title,
                    width=self.width,
                    height=self.height,
                    controls=self.controls,
                    menu=self.menu,
                    classes=self.classes,
                )
            )
            for element in self.elements:
                out.write(element)
            out.write(POST_WIN)

    def parse_control(self, line: str) -> dict[str, str | int]:
        x = split_string(line)
        control = {
            "text": prepare_text(x[0]),
            "id": int(x[1]),
            "combined_id": (int(x[1]) << 16) + int(self.dialog_id),
            "class": unquote(x[2]),
            "style": strip(x[3].split("|")),
            "x": int(x[4]) * SCALE + self.dx,
            "y": int(x[5]) * SCALE + self.dy,
            "width": int(x[6]) * SCALE,
            "height": int(x[7]) * SCALE,
            "css_class": f"control{int(x[1])}" if int(x[1]) != -1 else "",
            "extra": "",
        }
        if "WS_DISABLED" in control["style"]:
            control["extra"] += ' disabled="disabled"'
            control["style"] = [x for x in control["style"] if x != "WS_DISABLED"]

        if control["class"] != "BorCheck":
            if "0x1" in control["style"]:
                control["css_class"] += " ss_center"
            if "0x2" in control["style"]:  # horizontal bump ALT
                control["css_class"] += " horizontal_bump_alt"
                control["height"] = "auto"
            if "0x3" in control["style"]:  # vertical bump ALT
                control["css_class"] += " vertical_bump_alt"
                control["width"] = "auto"
            if "0x4" in control["style"]:  # horizontal bump
                control["css_class"] += " horizontal_bump"
                control["height"] = "auto"
            if "0x5" in control["style"]:  # vertical bump
                control["css_class"] += " vertical_bump"
                control["width"] = "auto"

        styles = [
            x.lower()
            for x in control["style"]
            if x
            not in (
                "BS_PUSHBUTTON",
                "BS_DEFPUSHBUTTON",
                "BS_AUTORADIOBUTTON",
                "ES_AUTOHSCROLL",
                "WS_CHILD",
                "WS_GROUP",
                "SS_LEFT",
                "WS_TABSTOP",
                "WS_VISIBLE",
                "SS_ICON",
                "CBS_DROPDOWNLIST",
                "CBS_SORT",
                "ES_OEMCONVERT",
                "ES_LEFT",
            )
            and not x.startswith("0")
        ]
        for style in styles:
            control["css_class"] += f" {style}"
        if control["class"] not in (
            "STATIC",
            "BUTTON",
            "COMBOBOX",
            "EDIT",
            "BorShade",
            "BorCheck",
            "BorRadio",
            "BorBtn",
        ):
            control["css_class"] += " " + control["class"].lower()
        return control

    def add_control(self, line: str) -> dict[str, str | int]:
        control = self.parse_control(line)
        if (
            "BS_PUSHBUTTON" in control["style"]
            or "BS_DEFPUSHBUTTON" in control["style"]
        ):  # Button
            self.add(
                """
                      <button class="{css_class}" style="position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px">{text}</button>
""".format(
                    **control
                )
            )
        elif "SS_ICON" in control["style"]:  # Icon
            self.add(
                """
<img src="resources/icons/{text}.gif" class="{css_class}" style="position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px" />
""".format(
                    **control
                )
            )
        elif control["class"] == "BorRadio":  # Radio button
            self.add(
                """
<div style="position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px">
<input class="{css_class} bwcc" {extra} type="radio" id="{combined_id}" name="bor_radio{dialog_id}" />
<label for="{combined_id}">{text}</label>
</div>""".format(
                    dialog_id=self.dialog_id,
                    **control,
                )
            )

        elif control["class"] == "BUTTON":
            if "BS_AUTORADIOBUTTON" in control["style"]:  # Radio button
                self.add(
                """
<div style="position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px">
<input class="{css_class}" {extra} type="radio" id="{combined_id}" name="bor_radio{dialog_id}" />
<label for="{combined_id}">{text}</label>
</div>""".format(
                        dialog_id=self.dialog_id,
                        **control,
                    )
                )
            elif "BS_CHECKBOX" in control["style"] or "BS_AUTOCHECKBOX" in control["style"]:  # Checkbox
                self.add(
                """
<div style="position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px">
<input class="{css_class}" {extra} type="checkbox" id="{combined_id}" />
<label for="{combined_id}">{text}</label>
</div>""".format(
                        **control
                    )
                )
            elif "BS_GROUPBOX" in control["style"]:  #  Group box
                control["height"] -= 25
                control["width"] -= 25
                self.add(
                """
<fieldset class="{css_class}" style="position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px">
  <legend>{text}</legend>
</fieldset>""".format(
                        **control
                    )
                )
            else:
                print(control)

        elif control["class"] == "BorCheck":  # Checkbox
            self.add(
                """
<div style="position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px">
<input class="{css_class} bwcc" {extra} type="checkbox" id="{combined_id}" />
<label for="{combined_id}">{text}</label>
</div>""".format(
                    **control
                )
            )

        elif control["class"] == "COMBOBOX" and "CBS_DROPDOWNLIST" in control["style"]:  # Dropdown
            self.add(
                """
<div class="{css_class}" style="position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px">
<select class="{css_class}" {extra} id="{combined_id}" style="width: {width}px;"></select>
</div>""".format(
                    **control
                )
            )

        elif control["class"] == "EDIT":  # Text input
            self.add(
                """
<input style="position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px" class="{css_class}" {extra} type="text" id="{id}" />
""".format(
                    dialog_id=self.dialog_id,
                    **control,
                )
            )

        elif control["class"] == "BorBtn":  # Buttons
            if control["id"] == 1:  # Ok
                control["text"] = "OK"
                self.add(
                    """
                      <button class="button_ok {css_class}" style="position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px">
                      </button>
""".format(
                        **control
                    )
                )
            elif control["id"] == 2:  # Cancel
                control["text"] = "Cancel"
                self.add(
                    """
                      <button class="button_cancel {css_class}" style="position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px">
                      </button>
""".format(
                        **control
                    )
                )
            else:
                control["text"] = control["id"] + 1000
                self.add(
                    """
<img src="resources/bitmaps/{text}.png" class="{css_class}" style="background-color: yellow; position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px" />
""".format(
                        **control
                    )
                )
        elif control["class"] == "BorShade":
            control["css_class"] += " ws_border"
            self.add(
                """
<div class="{css_class}" style="position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px">
{text}
</div>""".format(
                    **control
                )
            )
        elif control["class"] == "msctls_progress":  # Progress bars
            control["css_class"] += " ws_border"
            self.add(
                """
<div class="{css_class}" style="position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px">
{text}
</div>""".format(
                    **control
                )
            )
        elif control["class"] == "STATIC" or control["class"] == "BorStatic":
            self.add(

                """
<div class="{css_class}" style="position: absolute; left: {x}px; top: {y}px; width: {width}px; height: {height}px">
{text}
</div>""".format(
                    **control
                )
            )
        else:  # Custom
            self.add(

                """
<canvas class="{css_class} {class}" width="{width}" height="{height}" style="position: absolute; left: {x}px; top: {y}px;">
</canvas>""".format(
                    **control
                )
            )

    def load_menu(self, menu_name: str) -> None:
        self.dy += MENU_HEIGHT
        self.height += MENU_HEIGHT
        menu_json_file = Path.cwd() / ".." / "menus" / f"{menu_name}.json"
        with menu_json_file.open("r") as f:
            menu_json = json.load(f)
            self.menu += '<ul class="main-menu">\n'
            for item in menu_json:
                if "help" in item.get("flags", []):
                    self.menu += '<span style="flex-grow: 1"></span>'
                self.menu += "<li>"
                self.menu += prepare_text(item["label"], alt=False)
                self.menu += "\n"
                if item.get("items"):
                    self.menu += "<ul>\n"
                    for item in item["items"]:
                        if item["kind"] == "menuitem":
                            self.menu += f"<li class=\"menu menu{item['menu_id']}\">"
                            self.menu += prepare_text(item["label"], alt=False)
                            self.menu += "</li>\n"
                        elif item["kind"] == "separator":
                            self.menu += '<li class="separator"></li>\n'
                    self.menu += "</ul>\n"
                self.menu += "</li>"
            self.menu += "</ul>\n"


def convert_rc(filename: str) -> None:
    dialog: Dialog | None = None
    with open(filename, "r") as file:
        in_definition = False
        for line in file:
            line = line.strip()
            if not line:
                continue
            if " " in line:
                t, line = line.split(" ", 1)
                if line.startswith("DIALOG"):
                    if dialog is not None:
                        dialog.write()
                    dialog = Dialog(t, line)
                    sys.stdout.write(".")
            else:
                t = line
            if t == "LANGUAGE":
                continue
            if t == "CLASS":
                continue
            if t == "CAPTION":
                dialog.title = unquote(line)
                continue
            if t == "STYLE":
                dialog.style = strip(line.split("|"))
                continue
            if t == "MENU":
                dialog.load_menu(unquote(line).upper())
                continue
            if not in_definition:
                if t == "{":
                    in_definition = True
                    continue
                else:
                    x = split_string(line)
            elif t == "}":
                in_definition = False
                continue
            if in_definition:
                if t == "CONTROL":
                    dialog.add_control(line)
    if dialog is not None:
        dialog.write()
        dialog = None


def main() -> None:
    for filename in sys.argv[1:]:
        convert_rc(filename)
    sys.stdout.write("\n")


if __name__ == "__main__":
    main()
