#Overburden, 1.2m
/volume/new ReactorDome
/volume/setParent None
/volume/setShape box
/volume/setMaterial concrete
/volume/setPosition 0 0 0 mm
/volume/box/setHalfLengths 5 5 3 m
/volume/setPositionType absolute
/volume/setRGB 196 182 166
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
/volume/setRGB 206 210 215
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
/volume/setRGB 196 182 166
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
/volume/setAlpha 1
/volume/setVolumeNumber 3
/volume/finalize

#SS shielding
/volume/new ssShield
/volume/setParent AirCavity
/volume/setShape box
/volume/setMaterial stainless_steel
/volume/setPosition 0 0 -0.8786 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.5334 0.5334 0.5334 m
/volume/setRGB 207 212 217
/volume/setAlpha 0.000001
/volume/setVolumeNumber 4
/volume/finalize

#Lead #1
/volume/new lead1
/volume/setParent ssShield
/volume/setShape box
/volume/setMaterial lead
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.508 0.508 0.508 m
/volume/setRGB 33 33 33
/volume/setAlpha 0.1
/volume/setVolumeNumber 5
/volume/finalize

#Borated HDPE #1
/volume/new borated_HDPE1
/volume/setParent lead1
/volume/setShape box
/volume/setMaterial borated_hdpe
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.4572 0.4572 0.4572 m
/volume/setRGB 0 0 0
/volume/setAlpha 0.1
/volume/setVolumeNumber 6
/volume/finalize

#Lead #2
/volume/new lead2
/volume/setParent borated_HDPE1
/volume/setShape box
/volume/setMaterial lead
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.4064 0.4064 0.4064 m
/volume/setRGB 33 33 33
/volume/setAlpha 0.1
/volume/setVolumeNumber 7
/volume/finalize

#Borated HDPE #2
/volume/new borated_HDPE2
/volume/setParent lead2
/volume/setShape box
/volume/setMaterial borated_hdpe
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.3556 0.3556 0.3556 m
/volume/setRGB 0 0 0
/volume/setAlpha 0.1
/volume/setVolumeNumber 8
/volume/finalize

#HDPE #1
/volume/new HDPE1
/volume/setParent borated_HDPE2
/volume/setShape box
/volume/setMaterial hdpe
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.3048 0.3048 0.3048 m
/volume/setRGB 255 255 255
/volume/setAlpha 0.1
/volume/setVolumeNumber 9
/volume/finalize

#Lead #3
/volume/new lead3
/volume/setParent HDPE1
/volume/setShape box
/volume/setMaterial lead
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.254 0.254 0.254 m
/volume/setRGB 33 33 33
/volume/setAlpha 0.1
/volume/setVolumeNumber 10
/volume/finalize

#HDPE #2
/volume/new HDPE2
/volume/setParent lead3
/volume/setShape box
/volume/setMaterial hdpe
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.2032 0.2032 0.2032 m
/volume/setRGB 255 255 255
/volume/setAlpha 0.1
/volume/setVolumeNumber 11
/volume/finalize

#Lead #4
/volume/new lead4
/volume/setParent HDPE2
/volume/setShape box
/volume/setMaterial lead
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.1524 0.1524 0.1524 m
/volume/setRGB 33 33 33
/volume/setAlpha 0.1
/volume/setVolumeNumber 12
/volume/finalize

#Air cavity
/volume/new innerAir
/volume/setParent lead4
/volume/setShape box
/volume/setMaterial air
/volume/setPosition 0 0 0 m
/volume/setPositionType relative
/volume/box/setHalfLengths 0.1016 0.1016 0.1016 m
/volume/setRGB 0 1 1
/volume/setAlpha 0.1
/volume/setVolumeNumber 13
/volume/finalize

#Ge
/volume/new detector1
/volume/setParent innerAir
/volume/setShape cylinder
/volume/setMaterial germanium
/volume/setPosition 0 0.0705 0 m
/volume/setPositionType relative
/volume/cylinder/setCylinderAxis 0 0 1
/volume/cylinder/setRadius 3.1 cm
/volume/cylinder/setHalfHeight 3.1 cm
/volume/setRGB 184 115 51
/volume/setAlpha 0.1
/volume/setVolumeNumber 14
/volume/finalize

#Ge
/volume/new detector2
/volume/setParent innerAir
/volume/setShape cylinder
/volume/setMaterial germanium
/volume/setPosition 0.0705 0 0 m
/volume/setPositionType relative
/volume/cylinder/setCylinderAxis 0 0 1
/volume/cylinder/setRadius 3.1 cm
/volume/cylinder/setHalfHeight 3.1 cm
/volume/setRGB 184 115 51
/volume/setAlpha 0.1
/volume/setVolumeNumber 15
/volume/finalize

#Ge
/volume/new detector3
/volume/setParent innerAir
/volume/setShape cylinder
/volume/setMaterial germanium
/volume/setPosition 0 -0.0705 0 m
/volume/setPositionType relative
/volume/cylinder/setCylinderAxis 0 0 1
/volume/cylinder/setRadius 3.1 cm
/volume/cylinder/setHalfHeight 3.1 cm
/volume/setRGB 184 115 51
/volume/setAlpha 0.1
/volume/setVolumeNumber 16
/volume/finalize

#Ge
/volume/new detector4
/volume/setParent innerAir
/volume/setShape cylinder
/volume/setMaterial germanium
/volume/setPosition -0.0705 0 0 m
/volume/setPositionType relative
/volume/cylinder/setCylinderAxis 0 0 1
/volume/cylinder/setRadius 3.1 cm
/volume/cylinder/setHalfHeight 3.1 cm
/volume/setRGB 184 115 51
/volume/setAlpha 0.1
/volume/setVolumeNumber 17
/volume/finalize
