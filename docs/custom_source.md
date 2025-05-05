The following describes the locations in the code where changes will need to be made to implement custom sources. Right now the main code for these will be located in PaleoSimPrimaryGeneratorAction, but eventually we will move these to their own folder.

## PaleoSimMessenger.hh
* Any parameters read in via the macro need to have "Getters" defined as public functions
of the PaleoSimMessenger class, for instance, if they are to be used in generating primaries.
* The name of your generator needs to be added to the vector fValidSourceTypes. This should
not have any spaces in it. In the macro, your generator will then be called with the command 
generator/setSourceType <name you put>
* Any macro commands you want to set should be defined in the private section, along with the
variables that are set by these commands. For organization, the commands should be in their
own directory, i.e. /generator/meiHimeMuonGenerator/setMuonDepth <#>. The directory object
should be defined here. Please include units if reading in macro arguments.

## PaleoSimMessenger.cc
* In the constructor, the directory for your custom directory should be created. Then the G4ui commands should be created. The code that auto-generates the documentation isn't that
general purpose, so try to follow the existing examples. The order of setting parameter values
for the commands is not that important. The genMacroDoc.py script will need to be re-run after
updating this to update the documentation, and the macro_commands.md that is generated will 
then have to be committed and pushed. Try to include information to help with the documentation!
* In the SetNewValue() function, you will need to actually set the value of your defined variables based on the command line arguments, following the examples.

## PaleoSimPrimaryGeneratorAction.hh
* Any variables used in multiple functions within your code in PaleoSimPrimaryGeneratorAction.cc should be created here in the private section. You should also define any functions used
by your generator.

## PaleoSimPrimaryGeneratorAction.cc
*In the constructor, if there is any one-time set up to be done for your generator, include that in the list of "if (sourceType == < name >)".
* In GeneratePrimaries() call your specific generator if the sourceType is a match
* Your generator should ultimately call fGPS->GeneratePrimaryVertex(anEvent);
* OPTIONAL: If you have information about the generated primary events you want stored to the 
output tree, you will need to add that here after generating the event. Follow the existing example.

## OPTIONAL: PaleoSimUserEventInformation.hh
Only if there are additional quantities about the primaries you want stored in the output tree
* Here you should create vectors of the quantities you want stored in PrimariesTree for each event. For a general-purpose code, we use vectors in case multiple primaries were generated, so please do this even if you are only ever generating one particle per event

## OPTIONAL: PaleoSimOutputManager.hh
Only if there are additional quantities about the primaries you want stored in the output tree
* Make branches of the underlying variables holding the data to be written to the tree here. Again, use vectors.
* Add functions to push data about primaries into these vectors, preferably inline.

## OPTIONAL: PaleoSimOutputManager.cc
Only if there are additional quantities about the primaries you want stored in the output tree
* Add your branches to the PrimariesTree is CreateOutputFileAndTrees() if the sourceType is correct.
* You can also add any variables you want stored in the header tree here. This is useful as it is a permanent record of what variables were when the sim was ran.
* Add functions to clear your vectors in ClearPrimariesTreeEvent()

## OPTIONAL: PaleoSimEventAction.cc
Only if there are additional quantities about the primaries you want stored in the output tree
* In the BeginningOfEventAction, push your quantities from the UserEventInfo to your vectors using your Push functions, and then pop the variables out of the UserEventInfo vectors as you do.