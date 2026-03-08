import os
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_DIR = Path(os.environ.get("PDW_PROJECT_DIR", SCRIPT_DIR / "PDW"))

languages = [
    "de-DE",
    "en",
    "es-419",
    "es-ES",
    "fr-FR",
    "it-IT",
    "ja-JP",
    "ko-KR",
    "pl-PL",
    "pt-BR",
    "tr-TR",
    "zh-Hant",
]

base_path = PROJECT_DIR / "Content" / "Localization" / "Game"


def get_suffix_from_platform(platform):
    platform = platform.upper()
    if platform in ["XB1", "XSX"]:
        return "_XBOX"
    if platform in ["PS4", "PS5"]:
        return "_PS"
    return "Generic"


def extract_msgid_msgstr(entry_lines):
    msgid = []
    msgstr = []
    current = None
    for line in entry_lines:
        if line.startswith("msgid"):
            current = msgid
            msgid.append(line)
        elif line.startswith("msgstr"):
            current = msgstr
            msgstr.append(line)
        elif current is not None:
            current.append(line)
    return msgid, msgstr


def replace_msgid_msgstr(entry_lines, new_msgid, new_msgstr):
    result = []
    mode = None
    for line in entry_lines:
        if line.startswith("msgid"):
            result.extend(new_msgid)
            mode = "msgid"
        elif line.startswith("msgstr"):
            result.extend(new_msgstr)
            mode = "msgstr"
        elif mode in ["msgid", "msgstr"] and line.startswith("\""):
            continue
        else:
            result.append(line)
            mode = None
    return result


def process_po_text(po_text, suffix):
    lines = po_text.splitlines()
    entries = []
    current_entry = []

    for line in lines + [""]:
        if line.strip() == "" and current_entry:
            entries.append(list(current_entry))
            current_entry.clear()
        else:
            current_entry.append(line)

    entry_dict = {}
    for entry in entries:
        for line in entry:
            if line.startswith("msgctxt "):
                key = line.split("\"")[1]
                entry_dict[key] = entry
                break

    for entry in entries:
        base_key = None
        for line in entry:
            if line.startswith("msgctxt "):
                key = line.split("\"")[1]
                if suffix == "_PS" and key.endswith("_PS"):
                    base_key = key[:-3]
                elif suffix == "_XBOX" and key.endswith("_XBOX"):
                    base_key = key[:-5]
                elif suffix == "Generic" and key.endswith("Generic"):
                    base_key = key[:-7]
                break

        if base_key and base_key in entry_dict:
            new_msgid, new_msgstr = extract_msgid_msgstr(entry)
            base_entry = entry_dict[base_key]
            entry_dict[base_key] = replace_msgid_msgstr(base_entry, new_msgid, new_msgstr)

    output = []
    for entry in entries:
        key = None
        for line in entry:
            if line.startswith("msgctxt "):
                key = line.split("\"")[1]
                break
        if key and key in entry_dict:
            output.extend(entry_dict[key])
        else:
            output.extend(entry)
        output.append("")

    return "\n".join(output)


def modify_po_files_for_platform(platform):
    suffix = get_suffix_from_platform(platform)
    for lang in languages:
        po_path = base_path / lang / "Game.po"
        if not po_path.exists():
            print(f"Not found: {po_path}")
            continue

        with open(po_path, "r", encoding="utf-8") as f:
            po_text = f.read()

        new_text = process_po_text(po_text, suffix)

        with open(po_path, "w", encoding="utf-8") as f:
            f.write(new_text)
        print(f"Updated: {po_path}")


def modify_po_for_platform(platform):
    modify_po_files_for_platform(platform)
