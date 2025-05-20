#Pass in a base file--essentially what results from trying to do a tab auto-complete in the unix shell
#and an output filename
import ROOT
import sys
import os
import array
ROOT.gErrorIgnoreLevel = 6001
# === Usage check ===
if not len(sys.argv) == 3:
    print("\nError!\nUsage: python3 MergeTreesSymple.py <baseFile> <output file>\n")
    sys.exit()

# === Get input files and sort by run number ===
inpPath = os.path.abspath(sys.argv[1])
parentDir = os.path.dirname(inpPath)
baseName = inpPath[inpPath.rfind("/")+1:]
print(inpPath,parentDir,baseName)

inpFilenames = [parentDir + "/" +i for i in os.listdir(parentDir) if i.startswith(baseName) and i.endswith(".root")]
runNumbers = [int(i[i.find(baseName)+len(baseName):i.find(".")]) for i in inpFilenames]
inpFilenames = [x for _, x in sorted(zip(runNumbers, inpFilenames))]

# === Output file setup ===
outputFilename = sys.argv[2]
outputFile = ROOT.TFile(outputFilename, 'RECREATE')

# === Containers ===
inputFiles = []
inputSimHeaderTrees = []
treeNames = ["primariesTree", "MINTree", "neutronTallyTree"]
treeLists = {name: ROOT.TList() for name in treeNames}
eventNums = {name: [] for name in treeNames}
offset = 0

# === Loop over files ===
for i, inpFilename in enumerate(inpFilenames):
    print(i, inpFilename)

    #####################
    # Get input file    #
    #####################
    inputFiles.append(ROOT.TFile(inpFilename, 'READ'))
    if inputFiles[-1].IsZombie():
        print("Bad file!")
        continue

    # Get headerTree and nps
    inputSimHeaderTrees.append(inputFiles[-1].Get('headerTree'))
    nps_array = array.array("i", [0])
    inputSimHeaderTrees[-1].SetBranchAddress("nps", nps_array)
    inputSimHeaderTrees[-1].GetEntry(0)
    nps = nps_array[0]

    ##########################
    # Loop through each tree #
    ##########################
    for name in treeNames:
        tree = inputFiles[-1].Get(name)
        branch = tree.GetBranch("eventID")
        leaf = branch.GetLeaf("eventID")
        for entry in range(tree.GetEntries()):
            tree.GetEntry(entry)
            eventNums[name].append(int(leaf.GetValue()) + offset)
        tree.SetBranchStatus("eventID", 0)  # Disable original branch
        treeLists[name].Add(tree)
        # Just for copying structure
        if i == 0 and name not in globals():
            outputFile.cd()
            globals()[f"output{name}"] = tree.CloneTree(0)

    # === Update offset ===
    offset += nps

# === Merge and replace eventID for each tree ===
eventID = array.array("i", [0])
for name in treeNames:
    ######################
    # Merge current tree #
    ######################
    outputFile.cd()
    mergedTree = ROOT.TTree.MergeTrees(treeLists[name])
    eventIDBranch = mergedTree.Branch("eventID", eventID, "eventID/i")
    for idx in range(mergedTree.GetEntries()):
        eventID[0] = eventNums[name][idx]
        eventIDBranch.Fill()
    mergedTree.Write(name, ROOT.TObject.kOverwrite)

# === Write headerTree from file 0 with updated nps ===
headerTree = inputSimHeaderTrees[0]
headerTree.SetBranchStatus("nps", 0)  # Disable original branch
headerTreeCopy = headerTree.CloneTree(0)
headerTree.GetEntry(0)
nps_arr = array.array("i", [0])
new_npsBranch = headerTreeCopy.Branch("nps",nps_arr,"nps/i")
print(offset)
nps_arr[0] = offset
headerTreeCopy.Fill()
outputFile.cd()
headerTreeCopy.Write("headerTree",ROOT.TObject.kOverwrite)

# === Close input files ===
for f in inputFiles:
    f.Close()
outputFile.Close()