#Overburden
/volume/new Overburden
/volume/setParent None
/volume/setShape box
/volume/setVolumeNumber 0
/volume/setMaterial norite
/volume/setPosition 0 0 0 mm
/volume/setPositionType absolute
/volume/box/setHalfLengths 10 10 10 m
/volume/setRGB 0.5 0.5 0.5
/volume/setAlpha 0.5
/volume/finalize

# Define air cavity volume
/volume/new AirCavity
/volume/setParent Overburden
/volume/setShape box
/volume/setVolumeNumber 1
/volume/setMaterial air
/volume/setPosition 0 0 0 mm
/volume/setPositionType absolute
/volume/box/setHalfLengths 3 3 3 m
/volume/setRGB 0.9 0.9 0.9
/volume/setAlpha 0.1
/volume/finalize