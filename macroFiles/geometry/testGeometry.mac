# Define world volume = "rockBox"
# Same size: 0.2 m cube
/volume/new rockBox
/volume/setShape box
/volume/setMaterial Norite
/volume/setPosition 0 0 0 mm
/volume/box/setHalfLengths 2 2 2 m
/volume/setRGB 0 0 0.5
/volume/setAlpha 0.5
/volume/finalize

# Define air cavity volume
# Size: 0.15 m cube
/volume/new AirCavity
/volume/setParent rockBox
/volume/setShape box
/volume/setMaterial air
/volume/setPosition 0 0 0 mm
/volume/box/setHalfLengths .6 .6 .6 m
/volume/setRGB 0 0.5 0
/volume/setAlpha 0.5
/volume/finalize

# Define target volume inside air cavity
# Size: 10 cm cube
/volume/new Target
/volume/setParent AirCavity
/volume/setShape box
/volume/setMaterial Norite
/volume/setPosition 0 0 0 mm
/volume/box/setHalfLengths .50 .50 .50 cm
/volume/setRGB 0.5 0 0
/volume/setAlpha 1.
/volume/finalize
