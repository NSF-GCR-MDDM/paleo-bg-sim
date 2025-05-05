#Goal is to generate a README file documenting geometry macro commands 
#automatically based on the PaleoSimGeometryMessenger.hh and PaleoSimGeometryMessenger.cc
import datetime
now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M")

class MacroCommand:
    def __init__(self):
        self.path = ""
        self.description = ""
        self.variable = ""
        self.optional = False
        self.range = None
        self.units = None
        self.default = None

def find_default_value(varname, hhLines):
    for line in hhLines:
        if varname in line and "=" in line:
            rhs = line.split("=")[1].strip().rstrip(";")
            return rhs
    return None

ccFile = open("../src/PaleoSimGeometryMessenger.cc","r")
hhFile = open("../include/PaleoSimGeometryMessenger.hh","r")
hhLines = hhFile.readlines()

directories = {}
cmdSet=0
for line in ccFile:
    if "new" in line and "G4UIdirectory" in line:
        directoryName = line.split('"')[1]
        directories[directoryName] = []
        cmdSet=0

    if "new" in line and "G4UIcmd" in line:
        directories[directoryName].append(MacroCommand())
        directories[directoryName][-1].path = line.split('"')[1]
        cmdSet=1

    if cmdSet==1:
        if "SetGuidance" in line:
            directories[directoryName][-1].description = line.split('"')[1]
        elif "SetParameterName" in line:
            directories[directoryName][-1].variable = line.split('"')[1]
            if ("true") in line.split('"')[-1]:        
                directories[directoryName][-1].optional = True
            else:
                directories[directoryName][-1].optional = False
        elif "SetRange" in line:
            directories[directoryName][-1].range = line.split('"')[1]
        elif "SetDefaultUnit" in line:
            directories[directoryName][-1].units = line.split('"')[1]
        elif "SetDefaultValue" in line:
            defaultValueString = line[line.find("(")+1:line.find(")")]
            directories[directoryName][-1].default = find_default_value(defaultValueString,hhLines)

#Make README 
output = open("geometry_commands.md", "w")
output.write("# Geometry Command Options\n\n")
output.write("Automatically generated from PaleoSimGeometryMessenger.hh and PaleoSimGeometryMessenger.cc with genGeomDoc.py\n\n")
output.write(f"Last generated on: {now}\n\n")


for directory in directories:
    is_subdir = any(other != directory and directory.startswith(other) for other in directories)

    if is_subdir:
        output.write(f"### {directory}\n\n")
    else:
        output.write(f"## {directory}\n\n")

    for cmd in directories[directory]:
        optionality = "optional" if cmd.optional else "required"
        output.write(f"- **{cmd.path}** ({optionality}) — `{cmd.variable}`\n")

        if cmd.description:
            output.write(f"  - Description: {cmd.description}\n")
        if cmd.default is not None:
            output.write(f"  - Default: `{cmd.default}`\n")
        if cmd.units is not None:
            output.write(f"  - Units: `{cmd.units}`\n")
        if cmd.range is not None:
            output.write(f"  - Valid range: `{cmd.range}`\n")

        output.write("\n")