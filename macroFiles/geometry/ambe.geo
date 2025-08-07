#Re-doing so z is oriented toward roof
#+X is toward the chemical cabinet
#+Y is toward the mesoSpim table
# World volume (concrete shell)
/volume/new World
/volume/setParent None
/volume/setVolumeNumber 0
/volume/setShape box
/volume/setMaterial concrete
/volume/setPosition  0 0 0 mm
/volume/setPositionType absolute
/volume/box/setHalfLengths 2324 2324 2324 mm
/volume/setRGB 0.4 0.4 0.4
/volume/setAlpha 1.0
/volume/finalize

# Air cavity (10 ft cube)
/volume/new Room
/volume/setParent World
/volume/setVolumeNumber 1
/volume/setShape box
/volume/setMaterial air
/volume/setPosition 0 0 0 mm
/volume/setPositionType absolute
/volume/box/setHalfLengths 1524 1524 1524 mm
/volume/setRGB 0 1 1
/volume/setAlpha 0.05
/volume/finalize

# HDPE collimator block
/volume/new Collimator
/volume/setParent Room
/volume/setVolumeNumber 2
/volume/setShape box
/volume/setMaterial borated_hdpe
/volume/setPosition 0 -1218.2 -1218.2 mm
/volume/setPositionType absolute
/volume/box/setHalfLengths 304.8 304.8 304.8 mm
/volume/setRGB 0.6 0.4 0.8
/volume/setAlpha 1.0
/volume/finalize

# Iron throat
/volume/new SSThroat
/volume/setParent Collimator
/volume/setVolumeNumber 10
/volume/setShape box
/volume/setMaterial stainless_steel
/volume/setPosition 0 143.66875 -25.4 mm
/volume/setPositionType relative
/volume/box/setHalfLengths 25.4 161.13125 25.4 mm
/volume/setRGB 0.5 0.2 0.2
/volume/setAlpha 1.0
/volume/finalize

# Throat Air
/volume/new ThroatAir
/volume/setParent SSThroat
/volume/setVolumeNumber 3
/volume/setShape box
/volume/setMaterial air
/volume/setPosition 0 0 0 mm
/volume/setPositionType relative
/volume/box/setHalfLengths 22.225 161.13125 22.225 mm
/volume/setAlpha 0.1
/volume/finalize

# Source Nook
/volume/new SourceNook
/volume/setParent Collimator
/volume/setVolumeNumber 4
/volume/setShape box
/volume/setMaterial air
/volume/setPosition 44.45 1.5875 -25.4 mm
/volume/setPositionType relative
/volume/box/setHalfLengths 19.05 19.05 25.4 mm
/volume/setRGB 0 1 1
/volume/setAlpha 0.1
/volume/finalize

# Source Lead
/volume/new SourceLead
/volume/setParent SourceNook
/volume/setVolumeNumber 5
/volume/setShape box
/volume/setMaterial lead
/volume/setPosition -6.35 6.35 0 mm
/volume/setPositionType relative
/volume/box/setHalfLengths 12.7 12.7 25.4 mm
/volume/setRGB 0.3 0.3 0.3
/volume/setAlpha 0.5
/volume/finalize

# Lead Air Cavity
/volume/new LeadAirCavity
/volume/setParent SourceLead
/volume/setVolumeNumber 6
/volume/setShape cylinder
/volume/setMaterial air
/volume/setPosition 0 0 9.398 mm
/volume/setPositionType relative
/volume/cylinder/setCylinderAxis 0 0 1
/volume/cylinder/setRadius 8.89 mm
/volume/cylinder/setHalfHeight 0.762 mm
/volume/setRGB 0 1 1
/volume/setAlpha 0.1
/volume/finalize

# Source SS
/volume/new SourceSS
/volume/setParent SourceLead
/volume/setVolumeNumber 7
/volume/setShape cylinder
/volume/setMaterial stainless_steel
/volume/setPosition 0 0 -0.889 mm
/volume/setPositionType relative
/volume/cylinder/setCylinderAxis 0 0 1
/volume/cylinder/setRadius 8.89 mm
/volume/cylinder/setHalfHeight 9.525 mm
/volume/setRGB 0.7 0.7 0.7
/volume/setAlpha 1.0
/volume/finalize

# AmBe Source
/volume/new AmBeSource
/volume/setParent SourceSS
/volume/setVolumeNumber 8
/volume/setShape cylinder
/volume/setMaterial air
/volume/setPosition 0 0 0 mm
/volume/setPositionType relative
/volume/cylinder/setCylinderAxis 0 0 1
/volume/cylinder/setRadius 5.08 mm
/volume/cylinder/setHalfHeight 5.715 mm
/volume/setRGB 0.2 1.0 0.2
/volume/setAlpha 1.0
/volume/finalize

# Target
/volume/new Target
/volume/setParent ThroatAir
/volume/setVolumeNumber 9
/volume/setShape box
/volume/setMaterial lif
/volume/setPosition 15 -129.91875 -15 mm
/volume/setPositionType relative
/volume/box/setHalfLengths 5 5 5 mm
/volume/setRGB 0 0 1
/volume/setAlpha 1.0
/volume/finalize