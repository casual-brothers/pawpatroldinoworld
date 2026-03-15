import os
import subprocess
import sys

# Re-launch with pythonw.exe (no console) if running under python.exe
if sys.executable.lower().endswith("python.exe"):
    pythonw = sys.executable[:-10] + "pythonw.exe"
    if os.path.isfile(pythonw):
        subprocess.Popen([pythonw] + sys.argv)
        sys.exit(0)

import json
import queue as _queue
import shutil
import threading
import tkinter as tk
import zipfile
from pathlib import Path
from tkinter import messagebox, scrolledtext, ttk

import modificar_po
import patch_ops

SCRIPT_DIR = Path(__file__).resolve().parent
WORKSPACE_ROOT = SCRIPT_DIR
ENGINE_ROOT = Path(os.environ.get("PDW_ENGINE_ROOT", WORKSPACE_ROOT / "Engine"))
PROJECT_DIR = Path(os.environ.get("PDW_PROJECT_DIR", WORKSPACE_ROOT / "PDW"))
PROJECT_PATH = PROJECT_DIR / "PDW.uproject"
TARGET_NAME = os.environ.get("PDW_TARGET_NAME", "PDW")
UNREAL_PATH = ENGINE_ROOT / "Build" / "BatchFiles"
UNREALEXE = Path(os.environ.get("PDW_UNREAL_CMD", ENGINE_ROOT / "Binaries" / "Win64" / "UnrealEditor-Cmd.exe"))
BUILDS_DIR = Path(os.environ.get("PDW_BUILDS_DIR", WORKSPACE_ROOT / "Builds"))

CONFIG_FILE = SCRIPT_DIR / "build_config.json"

EPIC_BUILD_PATCH_TOOL = os.environ.get(
    "PDW_EPIC_BUILD_PATCH_TOOL",
    r"C:\Epic\BuildPatchTool_1.7.0\Engine\Binaries\Win64\BuildPatchTool.exe",
)
EPIC_ORG_ID = os.environ.get("PDW_EPIC_ORG_ID", "o-pu7dpghwrh7267c9e8t7uxant62yvn")
EPIC_PRODUCT_ID = os.environ.get("PDW_EPIC_PRODUCT_ID", "c6574b21954f4b52a72a4548ca59f2df")
EPIC_ARTIFACT_ID = os.environ.get("PDW_EPIC_ARTIFACT_ID", "e9812fb9fd86429ba64066ecfca5bc4d")
EPIC_CLIENT_ID = os.environ.get("PDW_EPIC_CLIENT_ID", "xyza7891VhX1cOh7jgNawkhkZxLJzQCA")
EPIC_CLIENT_SECRET = os.environ.get("PDW_EPIC_CLIENT_SECRET", "Bf5mqd2miHtNYQAZcy+wbOI9WV3FfEhLoPY/ip92On0")
EPIC_CLOUD_DIR = os.environ.get("PDW_EPIC_CLOUD_DIR", r"C:\EPIC\Cloud")
EPIC_BUILD_ROOT = os.environ.get("PDW_EPIC_BUILD_ROOT", str(BUILDS_DIR / "EOS" / "Windows"))
EPIC_APP_LAUNCH = os.environ.get("PDW_EPIC_APP_LAUNCH", f"{TARGET_NAME}.exe")
EPIC_APP_ARGS = os.environ.get("PDW_EPIC_APP_ARGS", "")

COPY_TARGET_DIR = Path(os.environ.get("PDW_COPY_TARGET_DIR", r"C:\compartida"))
DELIVERY_TARGET_DIR = Path(
    os.environ.get(
        "PDW_DELIVERY_TARGET_DIR",
        rf"G:\Unidades compartidas\CB Builds\OG Casual Brothers\{TARGET_NAME}",
    )
)

# ---------------------------------------------------------------------------
# Stdout redirector — thread-safe via a queue polled on the main thread
# ---------------------------------------------------------------------------

_log_queue: _queue.Queue = _queue.Queue()
_current_proc: subprocess.Popen = None
_stop_event = threading.Event()


class _StdoutRedirector:
    def write(self, text: str):
        _log_queue.put(text)

    def flush(self):
        pass


def _kill_current_proc():
    """Kill the active subprocess tree via taskkill /T /F."""
    proc = _current_proc
    if proc is None:
        return
    try:
        subprocess.run(
            ["taskkill", "/T", "/F", "/PID", str(proc.pid)],
            creationflags=subprocess.CREATE_NO_WINDOW,
        )
    except Exception as exc:
        print(f"Error killing process tree: {exc}")


def _run_streaming(command, **kwargs) -> int:
    """Run a shell command, streaming stdout+stderr to print(). Returns returncode."""
    global _current_proc
    proc = subprocess.Popen(
        command,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        encoding="utf-8",
        errors="replace",
        creationflags=subprocess.CREATE_NO_WINDOW,
        **kwargs,
    )
    _current_proc = proc
    for line in proc.stdout:
        if _stop_event.is_set():
            break
        print(line, end="", flush=True)
    proc.wait()
    _current_proc = None
    return proc.returncode


# ---------------------------------------------------------------------------
# Config persistence
# ---------------------------------------------------------------------------

def save_config():
    config_data = {
        "platforms": {platform: var.get() for platform, var in platform_vars.items()},
        "configuration": config_var.get(),
        "rebuild": rebuild_var.get(),
        "compile": compile_var.get(),
        "submission": submission_var.get(),
        "copy": copy_var.get(),
        "delivery": delivery_var.get(),
        "cook_incremental": cook_incremental_var.get(),
        "cook_full": cook_full_var.get(),
        "modify_po": modify_po_var.get(),
    }
    with open(CONFIG_FILE, "w", encoding="utf-8") as f:
        json.dump(config_data, f, indent=4)
    try:
        patch_panel.save_config()
    except Exception as exc:
        print(f"Error saving patch panel config: {exc}")


def load_config():
    if not CONFIG_FILE.exists():
        return
    try:
        with open(CONFIG_FILE, "r", encoding="utf-8") as f:
            config_data = json.load(f)
        for platform, selected in config_data.get("platforms", {}).items():
            if platform in platform_vars:
                platform_vars[platform].set(selected)
        config_var.set(config_data.get("configuration", "Shipping"))
        rebuild_var.set(config_data.get("rebuild", False))
        compile_var.set(config_data.get("compile", True))
        submission_var.set(config_data.get("submission", True))
        copy_var.set(config_data.get("copy", False))
        delivery_var.set(config_data.get("delivery", False))
        cook_incremental_var.set(config_data.get("cook_incremental", False))
        cook_full_var.set(config_data.get("cook_full", False))
        modify_po_var.set(config_data.get("modify_po", False))
        patch_panel.load_config()
    except Exception as exc:
        print(f"Error loading config: {exc}")


# ---------------------------------------------------------------------------
# Build helpers
# ---------------------------------------------------------------------------

def clean_directory(path: Path):
    if path.exists():
        shutil.rmtree(path)
        print(f"Deleted {path}")


def clear_folder_contents(path: Path):
    if not path.exists():
        return
    for entry in path.iterdir():
        try:
            if entry.is_file() or entry.is_symlink():
                entry.unlink()
        except Exception as exc:
            print(f"Failed to delete {entry}. Reason: {exc}")
    print(f"Cleared files from {path}")


def copy_built_files(platform: str):
    platform_ext = {
        "PS4": ".pkg",
        "PS5": ".pkg",
        "XB1": ".xvc",
        "XSX": ".xvc",
    }
    ext = platform_ext.get(platform)
    if not ext:
        return

    source_dir = BUILDS_DIR / platform
    if not source_dir.exists():
        print(f"Build directory {source_dir} does not exist.")
        return

    COPY_TARGET_DIR.mkdir(parents=True, exist_ok=True)
    for root, _, files in os.walk(source_dir):
        for file in files:
            if file.endswith(ext):
                src = Path(root) / file
                dst = COPY_TARGET_DIR / file
                shutil.copy2(src, dst)
                print(f"Copied {src} to {dst}")


def zip_folder(folder_path: Path, output_zip: Path):
    with zipfile.ZipFile(output_zip, "w", zipfile.ZIP_DEFLATED) as zipf:
        for root, _, files in os.walk(folder_path):
            for file in files:
                full_path = Path(root) / file
                rel_path = full_path.relative_to(folder_path)
                zipf.write(full_path, rel_path)


def get_delivery_name() -> str:
    config_path = PROJECT_DIR / "Config" / "DefaultGame.ini"
    if not config_path.exists():
        return ""
    with open(config_path, "r", encoding="utf-8") as f:
        for line in f:
            if line.startswith("BuildVersionString="):
                return line.strip().split("=", 1)[1]
            if line.startswith("ProjectVersion="):
                return line.strip().split("=", 1)[1]
    return ""


def run_buildpatchtool(build_version: str) -> int:
    cmd = (
        f'"{EPIC_BUILD_PATCH_TOOL}" '
        f'-OrganizationId="{EPIC_ORG_ID}" '
        f'-ProductId="{EPIC_PRODUCT_ID}" '
        f'-ArtifactId="{EPIC_ARTIFACT_ID}" '
        f'-ClientId="{EPIC_CLIENT_ID}" '
        f'-ClientSecret="{EPIC_CLIENT_SECRET}" '
        f'-mode=UploadBinary '
        f'-BuildRoot="{EPIC_BUILD_ROOT}" '
        f'-CloudDir="{EPIC_CLOUD_DIR}" '
        f'-BuildVersion="{build_version}" '
        f'-AppLaunch="{EPIC_APP_LAUNCH}" '
        f'-AppArgs="{EPIC_APP_ARGS}" '
        f'-PrereqPath="vc_redist.x64.exe" '
        f'-PrereqArgs="/quiet /norestart" '
        f'-PrereqIds="VC2015_2022_x64"'
    )
    print(f"[EOS Delivery] {cmd}")
    return _run_streaming(cmd, shell=True, cwd=WORKSPACE_ROOT)


def _clear_log():
    log_text.configure(state="normal")
    log_text.delete("1.0", tk.END)
    log_text.configure(state="disabled")


def build():
    selected_platforms = [platform for platform, var in platform_vars.items() if var.get()]
    if not selected_platforms:
        messagebox.showerror("Error", "Please select at least one platform.")
        return

    configuration = config_var.get()
    rebuild = rebuild_var.get()
    compile_flag = compile_var.get()
    submission = submission_var.get()
    copy_enabled = copy_var.get()
    delivery_enabled = delivery_var.get()
    delivery_text = get_delivery_name()
    delivery_name_var.set(f"Delivery name: {delivery_text or 'Unavailable'}")

    cook_mode = "None"
    if cook_incremental_var.get():
        cook_mode = "Incremental"
    elif cook_full_var.get():
        cook_mode = "Full"

    if delivery_enabled and not delivery_text:
        messagebox.showerror("Error", "Could not determine Delivery name from DefaultGame.ini")
        return

    if cook_mode == "Full":
        cook_str = "-cook"
        skipcook_str = ""
    elif cook_mode == "Incremental":
        cook_str = "-cook -iterate"
        skipcook_str = ""
    else:
        cook_str = ""
        skipcook_str = "-skipcook"

    rebuild_flag = "-clean" if rebuild else ""
    compile_str = "-build" if compile_flag else "-nocompile"
    submission_str = "-distribution" if submission else ""

    save_config()
    _clear_log()

    clean_directory(PROJECT_DIR / "Saved" / "Packages")

    if copy_enabled:
        clear_folder_contents(COPY_TARGET_DIR)

    for platform in selected_platforms:
        if _stop_event.is_set():
            break
        if modify_po_var.get():
            modificar_po.modify_po_for_platform(platform)

        build_status_var.set(f"Building: {platform} ({configuration})")
        print(f"\nPreparing for build: {platform}")

        if copy_enabled or delivery_enabled:
            clean_directory(BUILDS_DIR / platform)

        uat_platform = "Win64" if platform == "EOS" else platform

        release_flag = ""
        enabled, original_path, _last_patch_path = patch_panel.get_for(platform)
        if platform == "PS4":
            if enabled:
                release_flag = f"-basedonreleaseversion={original_path}"
            elif original_path:
                release_flag = f"-createreleaseversion={original_path}"

        archive_dir = BUILDS_DIR / platform
        print(f"\nBuilding for {platform}...")
        command = (
            f'call "{UNREAL_PATH / "RunUAT.bat"}" '
            f'-ScriptsForProject="{PROJECT_PATH}" Turnkey -command=VerifySdk -platform={uat_platform} -UpdateIfNeeded '
            f'-EditorIO -EditorIOPort=54096 '
            f'BuildCookRun -nop4 -utf8output -nocompileeditor -skipbuildeditor {cook_str} {skipcook_str} '
            f'-project="{PROJECT_PATH}" -target={TARGET_NAME} -unrealexe="{UNREALEXE}" '
            f'-platform={uat_platform} -stage -archive -package {compile_str} -pak -compressed '
            f'-archivedirectory="{archive_dir}" -manifests -clientconfig={configuration} '
            f'-nodebuginfo -nocompileuat {rebuild_flag} {submission_str} '
            f'{release_flag}'
        )
        returncode = _run_streaming(command, shell=True, cwd=WORKSPACE_ROOT)
        if returncode != 0:
            messagebox.showerror("Build Failed", f"Build failed for platform {platform}.\n\nSkipping Copy and Delivery.")
            continue

        if platform == "PS4" and submission:
            try:
                from CreateISO import process_gp4

                process_gp4(str(PROJECT_DIR))
            except Exception as exc:
                print(f"Error running process_gp4: {exc}")

        if platform != "PS4" and submission:
            enabled, original_path, last_patch_path = patch_panel.get_for(platform)
            if enabled:
                print(f"[PATCH] Preparing patch for {platform}...")
                rc_patch = patch_ops.run_patch(platform, original_path, last_patch_path)
                if rc_patch != 0:
                    print(f"[PATCH] ERROR: run_patch() returned {rc_patch} for {platform}")

        if delivery_enabled:
            if platform == "EOS":
                build_version_fixed = delivery_text.replace("_", ".")
                rc = run_buildpatchtool(build_version_fixed)
                if rc != 0:
                    print("ERROR: EOS BuildPatchTool upload failed.")
                continue

            source_dir = BUILDS_DIR / platform
            if source_dir.exists():
                DELIVERY_TARGET_DIR.mkdir(parents=True, exist_ok=True)
                for archivo in DELIVERY_TARGET_DIR.iterdir():
                    if archivo.is_file() and archivo.name.startswith(platform):
                        archivo.unlink()
                        print(f"Removed: {archivo}")
                zip_name = f"{platform}_{delivery_text}.zip"
                zip_path = DELIVERY_TARGET_DIR / zip_name
                print(f"Creating delivery zip: {zip_path}")
                zip_folder(source_dir, zip_path)
            else:
                print(f"Cannot deliver: build folder for {platform} does not exist.")
        elif copy_enabled:
            copy_built_files(platform)

    if _stop_event.is_set():
        build_status_var.set("Build stopped.")
        print("\nBuild stopped by user.")
    else:
        build_status_var.set("Build completed.")
        print("Build completed.")


def _build_thread():
    _stop_event.clear()
    try:
        build()
    finally:
        root.after(0, lambda: stop_btn.pack_forget())
        root.after(0, lambda: launch_btn.pack())
        if not _stop_event.is_set():
            root.after(0, root.bell)


def on_build_click():
    _stop_event.clear()
    launch_btn.pack_forget()
    stop_btn.pack()
    threading.Thread(target=_build_thread, daemon=True).start()


def on_stop_click():
    _stop_event.set()
    _kill_current_proc()
    build_status_var.set("Stopping...")


def on_close():
    save_config()
    _stop_event.set()
    _kill_current_proc()
    root.destroy()


# ---------------------------------------------------------------------------
# UI
# ---------------------------------------------------------------------------

root = tk.Tk()
root.title(f"{TARGET_NAME} Build Launcher")

main_frame = tk.Frame(root)
main_frame.pack(padx=10, pady=5)

platform_frame = tk.LabelFrame(main_frame, text="Platforms")
platform_frame.pack(side=tk.LEFT, padx=10)

platforms = ["Switch", "Switch2", "XB1", "XSX", "WinGDK", "PS4", "PS5", "EOS"]
platform_vars = {}
for plat in platforms:
    var = tk.BooleanVar(value=False)
    chk = tk.Checkbutton(platform_frame, text=plat, variable=var)
    chk.pack(anchor="w")
    platform_vars[plat] = var

config_frame = tk.LabelFrame(main_frame, text="Configuration")
config_frame.pack(side=tk.LEFT, padx=10)

config_var = tk.StringVar(value="Shipping")
config_dropdown = ttk.Combobox(config_frame, textvariable=config_var, state="readonly")
config_dropdown["values"] = ["Shipping", "Development", "Test"]
config_dropdown.pack(padx=10, pady=10)

options_frame = tk.LabelFrame(main_frame, text="Build Options")
options_frame.pack(side=tk.LEFT, padx=10)

rebuild_var = tk.BooleanVar(value=False)
compile_var = tk.BooleanVar(value=True)
submission_var = tk.BooleanVar(value=True)
copy_var = tk.BooleanVar(value=False)
delivery_var = tk.BooleanVar(value=False)
modify_po_var = tk.BooleanVar(value=False)

tk.Label(options_frame, text="Compile Options:", fg="red").pack(anchor="w", padx=5, pady=(5, 0))
tk.Checkbutton(options_frame, text="Rebuild", variable=rebuild_var).pack(anchor="w", padx=10)
tk.Checkbutton(options_frame, text="Compile", variable=compile_var).pack(anchor="w", padx=10)

tk.Label(options_frame, text="Cook Options:", fg="red").pack(anchor="w", padx=5, pady=(10, 0))
cook_incremental_var = tk.BooleanVar(value=False)
cook_full_var = tk.BooleanVar(value=False)
tk.Checkbutton(options_frame, text="Cook Incremental", variable=cook_incremental_var).pack(anchor="w", padx=10)
tk.Checkbutton(options_frame, text="Cook Full", variable=cook_full_var).pack(anchor="w", padx=10)

tk.Label(options_frame, text="Delivery Options:", fg="red").pack(anchor="w", padx=5, pady=(10, 0))
tk.Checkbutton(options_frame, text="Submission", variable=submission_var).pack(anchor="w", padx=10)
tk.Checkbutton(options_frame, text="Copy", variable=copy_var).pack(anchor="w", padx=10)
tk.Checkbutton(options_frame, text="Delivery", variable=delivery_var).pack(anchor="w", padx=10)
tk.Checkbutton(options_frame, text="Modify PO", variable=modify_po_var).pack(anchor="w", padx=10)

patch_panel = patch_ops.PatchPanel(main_frame, platforms=["PS4", "PS5", "Switch", "Switch2"])
patch_panel.pack(fill="x", padx=10, pady=8)

btn_row = tk.Frame(root)
btn_row.pack(pady=10)
launch_btn = tk.Button(btn_row, text="Build", command=on_build_click, width=12)
launch_btn.pack()
stop_btn = tk.Button(btn_row, text="Stop", command=on_stop_click, width=12)
# stop_btn starts hidden; shown only during build

build_status_var = tk.StringVar(value="")
build_status_label = tk.Label(root, textvariable=build_status_var, fg="blue")
build_status_label.pack()

delivery_name_var = tk.StringVar(value="")
delivery_name_label = tk.Label(root, textvariable=delivery_name_var, fg="green")
delivery_name_label.pack()

log_frame = tk.LabelFrame(root, text="Output")
log_frame.pack(fill="both", expand=True, padx=10, pady=(0, 10))
log_text = scrolledtext.ScrolledText(
    log_frame, wrap=tk.WORD, height=20, state="disabled", font=("Consolas", 9)
)
log_text.pack(fill="both", expand=True, padx=5, pady=5)


def _poll_log():
    try:
        while True:
            text = _log_queue.get_nowait()
            log_text.configure(state="normal")
            log_text.insert(tk.END, text)
            log_text.see(tk.END)
            log_text.configure(state="disabled")
    except _queue.Empty:
        pass
    root.after(50, _poll_log)


sys.stdout = _StdoutRedirector()
sys.stderr = _StdoutRedirector()
_poll_log()

load_config()
root.protocol("WM_DELETE_WINDOW", on_close)
root.mainloop()
