import json
import os
import subprocess
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, ttk
from typing import List, Optional

SCRIPT_DIR = Path(__file__).resolve().parent
WORKSPACE_ROOT = SCRIPT_DIR
PROJECT_DIR = Path(os.environ.get("PDW_PROJECT_DIR", WORKSPACE_ROOT / "PDW"))
PATCH_CONFIG_FILE = SCRIPT_DIR / "patch_config.json"
OUTPUT_BASE_DIR = Path(os.environ.get("PDW_BUILDS_DIR", WORKSPACE_ROOT / "Builds"))

PS5_STAGE_DIR = PROJECT_DIR / "Saved" / "StagedBuilds" / "PS5"
PS4_STAGE_DIR = PROJECT_DIR / "Saved" / "StagedBuilds" / "PS4"

SWITCH_TOOL = os.environ.get(
    "PDW_SWITCH_AUTHORING_TOOL",
    r"C:\Nintendo\NativeSDK18.3.1\NintendoSDK\Tools\CommandLineTools\AuthoringTool\AuthoringTool.exe",
)
SWITCH_DESC = os.environ.get(
    "PDW_SWITCH_DESC",
    r"C:\Nintendo\NativeSDK18.3.1\NintendoSDK\Resources\SpecFiles\Application.desc",
)
SWITCH_CACHE = os.environ.get("PDW_SWITCH_CACHE", r"d:\patch-cache")

SWITCH2_TOOL = os.environ.get(
    "PDW_SWITCH2_AUTHORING_TOOL",
    r"C:\Nintendo\NativeSDK20.5.28_Ounce\NintendoSDK\Tools\CommandLineTools\AuthoringTool\AuthoringTool.exe",
)
SWITCH2_DESC = os.environ.get(
    "PDW_SWITCH2_DESC",
    r"C:\Nintendo\NativeSDK20.5.28_Ounce\NintendoSDK\Resources\SpecFiles\Desc\Ounce-ounce-a64\Application.autogen.desc",
)
SWITCH2_CACHE = os.environ.get("PDW_SWITCH2_CACHE", r"d:\patch-cache-switch2")

PROSPERO_SDK_DIR = os.environ.get("SCE_PROSPERO_SDK_DIR", "")
PS5_TOOL = os.path.join(
    PROSPERO_SDK_DIR, "..", "..", "PROSPERO", "Tools", "Publishing Tools", "bin", "prospero-pub-cmd.exe"
)

ORBIS_SDK_DIR = os.environ.get("SCE_ORBIS_SDK_DIR", "")
PS4_TOOL = os.path.join(
    ORBIS_SDK_DIR, "..", "..", "ORBIS", "Tools", "Publishing Tools", "bin", "orbis-pub-cmd.exe"
)


def _platform_outdir(platform: str) -> Path:
    out = OUTPUT_BASE_DIR / platform
    out.mkdir(parents=True, exist_ok=True)
    return out


def _is_executable(path: str) -> bool:
    return bool(path) and os.path.isfile(path)


def _print_cmd(label: str, cmdlist: List[str]):
    line = " ".join(f'"{c}"' if (" " in str(c) or "\\" in str(c)) else str(c) for c in cmdlist)
    print(f"[{label}] CMD: {line}")


def _run_cmd(cmdlist: List[str]) -> int:
    """Run a command, printing its output, and return the exit code."""
    result = subprocess.run(
        cmdlist,
        shell=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        encoding="utf-8",
        errors="replace",
        creationflags=subprocess.CREATE_NO_WINDOW,
    )
    if result.stdout:
        print(result.stdout, end="")
    return result.returncode


def _find_single_by_ext_prefixed(root_dir: Path, ext: str, prefix: Optional[str]) -> Optional[str]:
    if not root_dir.is_dir():
        return None
    ext = ext.lower()
    pfx = (prefix or "").lower().strip()
    found = []
    for dirpath, _dirs, files in os.walk(root_dir):
        for fn in files:
            if not fn.lower().endswith(ext):
                continue
            name_wo_ext, _ = os.path.splitext(fn)
            if pfx and not name_wo_ext.lower().startswith(pfx):
                continue
            fp = os.path.join(dirpath, fn)
            try:
                found.append((os.path.getmtime(fp), fp))
            except Exception:
                found.append((0, fp))
    if not found:
        return None
    found.sort(key=lambda item: item[0], reverse=True)
    return found[0][1]


def _stem(path: str) -> str:
    return os.path.splitext(os.path.basename(path))[0]


def _auto_artifact(latest_path: str, stage_dir: Path, ext: str, platform_label: str, prefix: Optional[str] = None) -> Optional[str]:
    if latest_path and os.path.isfile(latest_path):
        print(f"[PATCH][{platform_label}] Using provided {ext}: {latest_path}")
        return latest_path

    picked = _find_single_by_ext_prefixed(stage_dir, ext, prefix)
    if picked:
        if prefix:
            print(f"[PATCH][{platform_label}] Auto-picked {prefix}*{ext}: {picked}")
        else:
            print(f"[PATCH][{platform_label}] Auto-picked {ext}: {picked}")
    else:
        if prefix:
            print(f"[PATCH][{platform_label}] ERROR: No {prefix}*{ext} found in {stage_dir}")
        else:
            print(f"[PATCH][{platform_label}] ERROR: No {ext} found in {stage_dir}")
    return picked


class PatchPanel(tk.LabelFrame):
    def __init__(self, parent, platforms):
        super().__init__(parent, text="Patches")
        self.platforms = list(platforms)

        self.style = ttk.Style(self)
        self.style.configure("PatchHeading.TLabel", font=("", 9, "bold"))

        self.enabled_vars = {}
        self.original_vars = {}
        self.latest_vars = {}

        for col in (2, 4):
            self.grid_columnconfigure(col, weight=1, uniform="cols", minsize=420)

        ttk.Label(self, text="Platform", style="PatchHeading.TLabel").grid(row=0, column=0, sticky="w", padx=(10, 6), pady=(8, 4))
        ttk.Label(self, text="Patch", style="PatchHeading.TLabel").grid(row=0, column=1, sticky="w", padx=(0, 6), pady=(8, 4))
        ttk.Label(self, text="Original build", style="PatchHeading.TLabel").grid(row=0, column=2, sticky="w", padx=(0, 6), pady=(8, 4))
        ttk.Label(self, text="", style="PatchHeading.TLabel").grid(row=0, column=3, sticky="w", padx=(0, 6), pady=(8, 4))
        ttk.Label(self, text="Latest patch", style="PatchHeading.TLabel").grid(row=0, column=4, sticky="w", padx=(0, 6), pady=(8, 4))
        ttk.Label(self, text="", style="PatchHeading.TLabel").grid(row=0, column=5, sticky="w", padx=(0, 10), pady=(8, 4))

        ttk.Separator(self, orient="horizontal").grid(row=1, column=0, columnspan=6, sticky="ew", padx=10, pady=(0, 6))

        start_row = 2
        for i, plat in enumerate(self.platforms):
            row = start_row + i
            self.enabled_vars[plat] = tk.BooleanVar(value=False)
            self.original_vars[plat] = tk.StringVar(value="")
            self.latest_vars[plat] = tk.StringVar(value="")

            ttk.Label(self, text=plat, width=10).grid(row=row, column=0, sticky="w", padx=(10, 6), pady=2)
            ttk.Checkbutton(self, variable=self.enabled_vars[plat]).grid(row=row, column=1, sticky="w", padx=(0, 6))

            e_orig = ttk.Entry(self, textvariable=self.original_vars[plat], width=64)
            e_orig.grid(row=row, column=2, sticky="we", padx=(0, 6))
            ttk.Button(self, text="...", width=3, command=lambda p=plat: self._browse_file(self.original_vars[p])).grid(row=row, column=3, sticky="w", padx=(0, 6))

            e_last = ttk.Entry(self, textvariable=self.latest_vars[plat], width=64)
            e_last.grid(row=row, column=4, sticky="we", padx=(0, 6))
            ttk.Button(self, text="...", width=3, command=lambda p=plat: self._browse_file(self.latest_vars[p])).grid(row=row, column=5, sticky="w", padx=(0, 10))

    def save_config(self):
        try:
            data = {}
            for plat in self.platforms:
                data[plat] = {
                    "enabled": bool(self.enabled_vars[plat].get()),
                    "original": self.original_vars[plat].get() or "",
                    "latest": self.latest_vars[plat].get() or "",
                }
            with open(PATCH_CONFIG_FILE, "w", encoding="utf-8") as f:
                json.dump(data, f, indent=2)
        except Exception as exc:
            print(f"[PatchPanel] Error saving {PATCH_CONFIG_FILE}: {exc}")

    def load_config(self):
        if not PATCH_CONFIG_FILE.exists():
            return
        try:
            with open(PATCH_CONFIG_FILE, "r", encoding="utf-8") as f:
                data = json.load(f)
            for plat, cfg in data.items():
                if plat in self.platforms:
                    self.enabled_vars[plat].set(bool(cfg.get("enabled", False)))
                    self.original_vars[plat].set(cfg.get("original", ""))
                    self.latest_vars[plat].set(cfg.get("latest", ""))
        except Exception as exc:
            print(f"[PatchPanel] Error loading {PATCH_CONFIG_FILE}: {exc}")

    def get_for(self, platform):
        if platform not in self.platforms:
            return (False, "", "")
        return (
            bool(self.enabled_vars[platform].get()),
            self.original_vars[platform].get() or "",
            self.latest_vars[platform].get() or "",
        )

    def _browse_file(self, var: tk.StringVar):
        path = filedialog.askopenfilename(title="Select a file")
        if path:
            var.set(path)


def run_patch(platform: str, original_path: str, latest_patch_path: str) -> int:
    platform_norm = (platform or "").strip().upper()
    print(f"[PATCH] Platform={platform_norm}")
    print(f"[PATCH]   Original: {original_path}")
    print(f"[PATCH]   Latest:   {latest_patch_path}")

    if platform_norm in ("SWITCH", "NSW", "NINTENDO SWITCH"):
        return _run_patch_switch(original_path, latest_patch_path)
    if platform_norm == "SWITCH2":
        return _run_patch_switch2(original_path, latest_patch_path)
    if platform_norm == "PS4":
        return _run_patch_ps4(original_path, latest_patch_path)
    if platform_norm == "PS5":
        return _run_patch_ps5(original_path, latest_patch_path)

    print(f"[PATCH] Unsupported platform: {platform_norm}")
    return 1


def _run_patch_switch(original_path: str, latest_path: str) -> int:
    if not _is_executable(SWITCH_TOOL):
        print(f"[PATCH][SWITCH] ERROR: AuthoringTool not found: {SWITCH_TOOL}")
        return 2
    if not os.path.isfile(SWITCH_DESC):
        print(f"[PATCH][SWITCH] ERROR: DESC file not found: {SWITCH_DESC}")
        return 3
    if not os.path.isfile(original_path):
        print(f"[PATCH][SWITCH] ERROR: Original NSP not found: {original_path}")
        return 4

    latest_exists = bool(latest_path) and os.path.isfile(latest_path)
    base_for_name = latest_path if latest_exists else original_path
    out_path = _platform_outdir("Switch") / f"{_stem(base_for_name)}_PATCH.nsp"

    switch_outdir = _platform_outdir("Switch")
    current_path = None
    for name in os.listdir(switch_outdir):
        if name.lower().endswith(".nsp"):
            current_path = str(switch_outdir / name)
            break
    if not current_path:
        print(f"[PATCH][SWITCH] ERROR: No .nsp found in {switch_outdir} to use as --current")
        return 5

    cmd = [
        SWITCH_TOOL,
        "makepatch",
        "-o",
        str(out_path),
        "--cache-directory",
        SWITCH_CACHE,
        "--desc",
        SWITCH_DESC,
        "--original",
        original_path,
        "--current",
        current_path,
    ]
    if latest_exists:
        cmd += ["--previous", latest_path]

    _print_cmd("PATCH][SWITCH", cmd)
    rc = _run_cmd(cmd)
    if rc != 0:
        print(f"[PATCH][SWITCH] makepatch failed with code {rc}")
        return rc
    if not out_path.is_file():
        print(f"[PATCH][SWITCH] ERROR: Output not found: {out_path}")
        return 6

    print(f"[PATCH][SWITCH] OK -> {out_path}")
    return 0


def _run_patch_switch2(original_path: str, latest_path: str) -> int:
    if not _is_executable(SWITCH2_TOOL):
        print(f"[PATCH][SWITCH2] ERROR: AuthoringTool not found: {SWITCH2_TOOL}")
        return 2
    if not os.path.isfile(SWITCH2_DESC):
        print(f"[PATCH][SWITCH2] ERROR: DESC file not found: {SWITCH2_DESC}")
        return 3
    if not os.path.isfile(original_path):
        print(f"[PATCH][SWITCH2] ERROR: Original NSP not found: {original_path}")
        return 4

    latest_exists = bool(latest_path) and os.path.isfile(latest_path)
    base_for_name = latest_path if latest_exists else original_path
    out_path = _platform_outdir("Switch2") / f"{_stem(base_for_name)}_PATCH.nsp"

    switch2_outdir = _platform_outdir("Switch2")
    current_path = None
    for name in os.listdir(switch2_outdir):
        if name.lower().endswith(".nsp"):
            current_path = str(switch2_outdir / name)
            break
    if not current_path:
        print(f"[PATCH][SWITCH2] ERROR: No .nsp found in {switch2_outdir} to use as --current")
        return 5

    cmd = [
        SWITCH2_TOOL,
        "makepatch",
        "-o",
        str(out_path),
        "--cache-directory",
        SWITCH2_CACHE,
        "--desc",
        SWITCH2_DESC,
        "--original",
        original_path,
        "--current",
        current_path,
    ]
    if latest_exists:
        cmd += ["--previous", latest_path]

    _print_cmd("PATCH][SWITCH2", cmd)
    rc = _run_cmd(cmd)
    if rc != 0:
        print(f"[PATCH][SWITCH2] makepatch failed with code {rc}")
        return rc
    if not out_path.is_file():
        print(f"[PATCH][SWITCH2] ERROR: Output not found: {out_path}")
        return 6

    print(f"[PATCH][SWITCH2] OK -> {out_path}")
    return 0


def _run_patch_ps5(original_pkg: str, gp5_path: str) -> int:
    if not PROSPERO_SDK_DIR or not _is_executable(PS5_TOOL):
        print(f"[PATCH][PS5] ERROR: Prospero tool not found: {PS5_TOOL}")
        return 12
    if not original_pkg or not os.path.isfile(original_pkg):
        print("[PATCH][PS5] ERROR: Base PKG not found (Original should point to BASE.pkg)")
        return 11

    gp5 = _auto_artifact(gp5_path, PS5_STAGE_DIR, ".gp5", "PS5", prefix="HP")
    if not gp5:
        return 10

    out_pkg = _platform_outdir("PS5") / f"{_stem(gp5)}.pkg"
    cmd = [PS5_TOOL, "img_create", "--for_submission", "--ref_pkg_path", original_pkg, gp5, str(out_pkg)]
    _print_cmd("PATCH][PS5", cmd)
    rc = _run_cmd(cmd)
    if rc != 0:
        print(f"[PATCH][PS5] img_create failed with code {rc}")
        return rc
    if not out_pkg.is_file():
        print(f"[PATCH][PS5] ERROR: Output not found: {out_pkg}")
        return 13

    print(f"[PATCH][PS5] OK -> {out_pkg}")
    return 0


def _run_patch_ps4(original_pkg: str, gp4_path: str) -> int:
    if not ORBIS_SDK_DIR or not _is_executable(PS4_TOOL):
        print(f"[PATCH][PS4] ERROR: Orbis tool not found: {PS4_TOOL}")
        return 21
    if not original_pkg or not os.path.isfile(original_pkg):
        print("[PATCH][PS4] ERROR: Base PKG not found (Original should point to BASE.pkg)")
        return 26

    gp4 = _auto_artifact(gp4_path, PS4_STAGE_DIR, ".gp4", "PS4", prefix="HP")
    if not gp4:
        return 20

    out_dir = _platform_outdir("PS4")

    cmd_update = [PS4_TOOL, "gp4_proj_update", "--app_path", original_pkg, gp4]
    _print_cmd("PATCH][PS4", cmd_update)
    rc = _run_cmd(cmd_update)
    if rc != 0:
        print(f"[PATCH][PS4] gp4_proj_update failed with code {rc}")
        return rc

    cmd_create = [PS4_TOOL, "img_create", "--oformat", "pkg", gp4, str(out_dir)]
    _print_cmd("PATCH][PS4", cmd_create)
    rc = _run_cmd(cmd_create)
    if rc != 0:
        print(f"[PATCH][PS4] img_create failed with code {rc}")
        return rc

    expected_pkg = out_dir / f"{_stem(gp4)}.pkg"
    if expected_pkg.is_file():
        print(f"[PATCH][PS4] OK -> {expected_pkg}")
        return 0

    try:
        pkgs = [out_dir / name for name in os.listdir(out_dir) if name.lower().endswith(".pkg")]
        if pkgs:
            newest = max(pkgs, key=lambda item: item.stat().st_mtime)
            print(f"[PATCH][PS4] OK -> {newest}")
            return 0
    except Exception as exc:
        print(f"[PATCH][PS4] WARN: Could not inspect output dir: {exc}")

    print(f"[PATCH][PS4] ERROR: No .pkg found in {out_dir}")
    return 22
