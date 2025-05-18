import os
import shutil
import subprocess

def copy_axf_file(source_path, dest_folder):
    """Copy the AXF file to a destination folder."""
    if not os.path.isfile(source_path):
        raise FileNotFoundError(f"Source file not found: {source_path}")

    dest_path = os.path.join(dest_folder, "silmotion_xG12.axf")
    shutil.copy2(source_path, dest_path)
    print(f"File copied to: {dest_path}")
    return dest_path

def run_renode(axf_path):
    commands = [
        'emulation CreateServerSocketTerminal 12345 "terminal" false',
        "mach create",
        "machine LoadPlatformDescription @platforms/boards/silabs/efr32mg24board.repl",
        "connector Connect sysbus.usart2 terminal",
        f"sysbus LoadELF '{axf_path}'",
        "showAnalyzer sysbus.usart2",
        "start"
    ]

    renode_process = subprocess.Popen(["renode", "--console"], stdin=subprocess.PIPE, text=True)
    for command in commands:
        print(f"Executing Renode command: {command}")
        renode_process.stdin.write(f"{command}\n")
    renode_process.stdin.flush() 
    renode_process.wait()


if __name__ == "__main__":
    source_axf = r"E:\\AIoT\\Project\\Nova\\silmotion_xG12\\GNU ARM v12.2.1 - Debug\\silmotion_xG12.axf"
    dest_folder = os.getcwd()

    try:
        # Copy AXF file
        axf_path = copy_axf_file(source_axf, dest_folder)

        # Run Renode directly
        run_renode(axf_path)

    except Exception as e:
        print(f"An error occurred: {e}")
