Import("env")
import os
import subprocess
from datetime import datetime

# ----------------------------
# Project metadata (EDIT HERE)
# ----------------------------
PROJECT_NAME = "MacroPad OR2"
PROJECT_VERSION = "1.0"
PROJECT_DESC = "WCH554 AUX KEYBOARD INTERFACE"
PROJECT_MFG = "PR77"
SPLASH_LINE1 = " MACRO PAD OR2 "
SPLASH_LINE2 = "    BY PR77    "

# ----------------------------
# Git hash
# ----------------------------
try:
    git_hash = subprocess.check_output(
        ["git", "rev-parse", "--short", "HEAD"]
    ).decode().strip()
except:
    git_hash = "unknown"

# ----------------------------
# Build timestamp
# ----------------------------
build_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

# ----------------------------
# Build number (AUTO INCREMENT)
# ----------------------------
BUILD_FILE = ".build_number"

if os.path.exists(BUILD_FILE):
    with open(BUILD_FILE, "r") as f:
        try:
            build_number = int(f.read().strip())
        except:
            build_number = 0
else:
    build_number = 0

build_number += 1

with open(BUILD_FILE, "w") as f:
    f.write(str(build_number))

# ----------------------------
# Generate header file
# ----------------------------
os.makedirs("include", exist_ok=True)

with open("include/firmware_info.h", "w") as f:
    f.write("""#ifndef __CH554_FIRMWARE_INFO_H__
#define __CH554_FIRMWARE_INFO_H__

#define FW_NAME         "%s"
#define FW_VERSION      "%s"
#define FW_DESC         "%s"
#define FW_MFG          "%s"
#define FW_SLASH_1      "%s"
#define FW_SLASH_2      "%s"
#define FW_GIT          "%s"
#define FW_BUILD_TIME   "%s"
#define FW_BUILD_NUMBER %d

#endif // __CH554_FIRMWARE_INFO_H__
""" % (
    PROJECT_NAME,
    PROJECT_VERSION,
    PROJECT_DESC,
    PROJECT_MFG,
    SPLASH_LINE1,
    SPLASH_LINE2,
    git_hash,
    build_time,
    build_number
))
    
print("======================================")
print("Firmware Build Info")
print("Name:            ", PROJECT_NAME)
print("Version:         ", PROJECT_VERSION)
print("Git:             ", git_hash)
print("Time:            ", build_time)
print("Build number:    ", build_number)
print("======================================")