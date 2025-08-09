#Overburden, 1.2m
/volume/new ReactorDome
/volume/setParent None
/volume/setShape box
/volume/setMaterial concrete
/volume/setPosition 0 0 0 mm
/volume/box/setHalfLengths 5 5 3 m
/volume/setPositionType absolute
/volume/setRGB 0.5 0.5 0.5
/volume/setAlpha 0.1
/volume/setVolumeNumber 0
/volume/finalize

# Steel containment, 0.038m
/volume/new Containment
/volume/setParent ReactorDome
/volume/setShape box
/volume/setMaterial steel
/volume/setPosition 0 0 0 mm
/volume/setPositionType relative
/volume/box/setHalfLengths 3.8 3.8 1.8 m
/volume/setRGB 0.75 0.75 0.75
/volume/setAlpha 0.1
/volume/setVolumeNumber 1
/volume/finalize

#ZA28R027 Roof, 0.35m
/volume/new Roof
/volume/setParent Containment
/volume/setShape box
/volume/setMaterial concrete
/volume/setPosition 0 0 0 cm
/volume/setPositionType relative
/volume/box/setHalfLengths 3.762 3.762 1.762 m
/volume/setRGB 0.5 0.5 0.5
/volume/setAlpha 0.1
/volume/setVolumeNumber 2
/volume/finalize

#ZA28R027 Air 
/volume/new AirCavity
/volume/setParent Roof
/volume/setShape box
/volume/setMaterial air
/volume/setPosition 0 0 0 cm
/volume/setPositionType relative
/volume/box/setHalfLengths 3.412 3.412 1.412 m
/volume/setRGB 0 1 1
/volume/setAlpha 0.05
/volume/setVolumeNumber 3
/volume/finalize

#Water shielding
/volume/new WaterShielding30cm
/volume/setParent AirCavity
/volume/setShape box
/volume/setMaterial water
/volume/setPosition 0 0 -1.0758 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.3362 0.3362 0.3362 m
/volume/setRGB 0 0 1
/volume/setAlpha 0.1
/volume/setVolumeNumber 4
/volume/finalize

#Water shielding
/volume/new WaterShielding25cm
/volume/setParent WaterShielding30cm
/volume/setShape box
/volume/setMaterial water
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.2862 0.2862 0.2862 m
/volume/setRGB 0 0 1
/volume/setAlpha 0.1
/volume/setVolumeNumber 5
/volume/finalize

#Water shielding
/volume/new WaterShielding20cm 
/volume/setParent WaterShielding25cm
/volume/setShape box
/volume/setMaterial water
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.2362 0.2362 0.2362 m
/volume/setRGB 0 0 1
/volume/setAlpha 0.1
/volume/setVolumeNumber 6
/volume/finalize

#Water shielding
/volume/new WaterShielding15cm
/volume/setParent WaterShielding20cm
/volume/setShape box
/volume/setMaterial water
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.1862 0.1862 0.1862 m
/volume/setRGB 0 0 1
/volume/setAlpha 0.1
/volume/setVolumeNumber 7
/volume/finalize

#Water shielding
/volume/new WaterShielding10cm
/volume/setParent WaterShielding15cm
/volume/setShape box
/volume/setMaterial water
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.1362 0.1362 0.1362 m
/volume/setRGB 0 0 1
/volume/setAlpha 0.1
/volume/setVolumeNumber 8
/volume/finalize

#Water shielding
/volume/new WaterShielding5cm
/volume/setParent WaterShielding10cm
/volume/setShape box
/volume/setMaterial water
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.0862 0.0862 0.0862 m
/volume/setRGB 0 0 1
/volume/setAlpha 0.1
/volume/setVolumeNumber 9
/volume/finalize

#Water LiF
/volume/new LiF
/volume/setParent WaterShielding5cm
/volume/setShape box
/volume/setMaterial lif
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.0362 0.0362 0.0362 m
/volume/setRGB 1 0 0
/volume/setAlpha 0.5
/volume/setVolumeNumber 10
/volume/finalize

