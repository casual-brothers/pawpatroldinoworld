import os
import subprocess
import sys
from pathlib import Path


def run_cmd(executable, working_dir, args):
    try:
        print(f"Running command: {' '.join([executable] + args)} in {working_dir}")
        result = subprocess.run(
            [executable] + args,
            cwd=working_dir,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        print(result.stdout)
        if result.returncode != 0:
            print(result.stderr)
        return result.returncode == 0
    except Exception as exc:
        print(f"Exception while running command: {exc}")
        return False


def process_gp4(project_path):
    orbis_pub_cmd_path = os.environ.get(
        "PDW_ORBIS_PUB_CMD",
        r"C:\Program Files (x86)\SCE\ORBIS\Tools\Publishing Tools\bin\orbis-pub-cmd.exe",
    )
    project_dir = Path(project_path)
    staged_path = project_dir / "Saved" / "StagedBuilds" / "PS4"
    output_path = Path(__file__).resolve().parent / "Builds" / "PS4"
    output_path.mkdir(parents=True, exist_ok=True)

    print(f"Searching for .gp4 file in {staged_path}...")

    gp4_file_path = None
    for root, _, files in os.walk(staged_path):
        for file in files:
            if file.endswith(".gp4"):
                gp4_file_path = Path(root) / file
                break
        if gp4_file_path:
            break

    if not gp4_file_path:
        print(f"No .gp4 file found in {staged_path}")
        return

    gp4_file_name = gp4_file_path.name
    working_dir = gp4_file_path.parent
    print(f"Found .gp4 file: {gp4_file_name} in {working_dir}")

    print("Updating gp4 project settings...")
    if not run_cmd(orbis_pub_cmd_path, working_dir, ["gp4_proj_update", "--storage_type", "bd25", gp4_file_name]):
        print(f"Failed to update gp4 project settings for {gp4_file_name}")
        return

    print("Updating gp4 scenario settings...")
    if not run_cmd(
        orbis_pub_cmd_path,
        working_dir,
        ["gp4_scenario_update", "--id", "0", "--initial_chunk_count", "1", "--initial_chunk_count_disc", "1", gp4_file_name],
    ):
        print(f"Failed to update gp4 scenario settings for {gp4_file_name}")
        return

    print("Creating package...")
    if not run_cmd(
        orbis_pub_cmd_path,
        working_dir,
        ["img_create", "--oformat", "pkg+iso", "--oformat", "pkg+iso+subitem", "--sparse", "on", "--compress", "on", gp4_file_name, str(output_path)],
    ):
        print(f"Failed to create pkg for {gp4_file_name}")
        return

    print("Package creation completed successfully.")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python CreateISO.py <path_to_unreal_project>")
    else:
        process_gp4(sys.argv[1])
