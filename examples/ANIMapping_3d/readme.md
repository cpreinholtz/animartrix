


WHY GENERATED PATTERNS ARE SO DARK????

bugfixes
  use every n millis for decay intensity
  audio figure out how to smooth out the signal better
    NEED TO POLL FASTER!!!!!!! (WITHOUT INTERUPTS!!!!) make function calls inside render loop?
    --try sampling multiple times during update?  make a callbalck / interupt for more regular sampling?
  bpms
    make a "lock" signal, increment when you get a periodic beat, decrement otherwise
    audio create beat ratios to find "biggest" beats rather than delta, and increase delta, put inside the beat detection circuit, make larger than 100ms which is 600 bpm
    really just need to coordinate bpms...
  --audio create serial plotter stream    
  --audio find out what weights make for better volume detection, .99 was not the intent, i think .999 was but that may not be enough either
  audio make the weights universal, aka immune to sample time changes
  audio test hardware lowpass?? I think i'm getting aliasing

  audio create intensity moddable in art, NOT use global brightness  
    --audio make the brightness flash proportional to the beat amplitude
  audio set min peak amplitude??

  try setting a slightly slower IIR average when we get a peak manually to disable double peaks?

  try setting BIN as a modable?
  scaling (darkening) gets weird at high volume






features
  make pixel spacing a part of scaling???
  make moodables able to have multiple inputs, for now CANNOT modulate ghue because it is being modulated inside hue shift
  audio create more settings for different modulators
  audio play with mic gain
  bpm
    audio if no beats in a while disable until bpm stabalizes
    audio fix the sort function


modables
  fractaly--intger?-- controls number of spirals, can mult with existing setup, use FLOOR if cutting off oddly 
  --intensity--0 to 255--
  speed
  --scale













features: 
  modes: all, low power, chill
  darkwad check, 
  fade patterns into one another, 
  switch input, 
  color picker?, play with palettes, 
  music reactive,  incoorperate ANIMaudio
  IMU reactive
  change master speed to be BPM related
  global hue shift, requires shifting my palets to CHSV
  distance and radial normalization (Spiralus)
  play with phi
  change my hsiF usage to color palettes Module_Experiment9_Hsi
  try some pallets with non zero starting points
  add more effects like Module_Experiment9_Hsi
  palette blending
  effect blending
  perform hue shift within class
  

    make floats
    make run off noise function



update readme
hardware
  imu
  microphone
  level shifter
  leds
req libraries
  Adafruit ICM20X
  Adafruit BusIO?
  dafruit Unified Sensor


  


speed adjust:
Complex_Kaleido_3

PatternAndNameList gChillPatterns = {
  {Module_Experiment8,"Module_Experiment8"},// pretty much just reds and pinks and orange
  {Module_Experiment7,"Module_Experiment7"}, //boring but chill, too black sometimes
  {Module_Experiment6,"Module_Experiment6"},//bit better, only yellow and red, slow
  mod5
  mod3
  Module_Experiment2

  {Parametric_Water,"Parametric_Water"},// could tone down radius / blue to save power?
  water
  {Complex_Kaleido_2,"Complex_Kaleido_2"},
  {Complex_Kaleido_1,"Complex_Kaleido_1"},
  {SM10,"SM10"},
  sm9
  {SM6,"SM6"},
  SM5
  Slow_Fade
  Zoom2
  Zoom
  Hot_Blob
  Spiralus2
  Spiralus
  Scaledemo1
  Rotating_Blob
};

PatternAndNameList gHypePatterns = {
  {Complex_Kaleido_3,"Complex_Kaleido_3"},
  {SM8,"SM8"},//fun strobe
  SM1, beautiful swirls
  RGB_Blobs
  Caleido3, maybe make a bit faster? morwe color?, see ledMap Y
  Caleido2, speed up?
  Caleido1, speed up?
  Rings
};

bpm patterns


low power?
rotating blob?

..remove or tweak
PatternAndNameList gNeedsWorkPatterns = {
  {Chasing_Spirals_Hsi, "Chasing_Spirals_Hsi"}, try making the plane dynamic and re-adding other layers and movmnet
  {demoBpm, "demoBpm"}, //has potential, try incoorperating bpm into other patterns
  {Module_Experiment11_Hsi, "Module_Experiment11_Hsi"}, //might get too black, really sweet though
  Module_Experiment9 blue and green not touched, different color every time, also dont really need since we have hsi versions
  Module_Experiment5 red only, fix g b 
  Module_Experiment4 boring colors, only rgb with no mixing
  water, no green fix that
  Complex_Kaleido_5 red only, fix
  RGB_Blobs5, not dynamic enough fix radius?
  RGB_Blobs4 radius?
  RGB_Blobs3 green blue broken?
  RGB_Blobs2, still boring...
  Polar_Waves has potential but seems off
  zoom no blue???
  zoom2 no green???
  hot blob missing blue
  Yves, what is it even doing???  seems to get more dynamic with time?
  Lava1 no blue, really boring???
  Distance_Experiment, boring
  waves, was too slow with master timing 0.01
  Scaledemo1, what happened this was a fav?
  Big_Caleido seems broken, need to make less black
};


good enough.. maybe play with or just leave alone
  Module_Experiment10
  Module_Experiment3 really like this one, again colors could be cooler
  Module_Experiment2 this one even better! love the fades and speed
  SM10
  SM9, seems too blue, not quite right
  SM8 fun strobe, could use better colors
  SM3 better colors... has potentiall to be nice chill
  SM2, too black, but really really cool
  Big_Caleido, angle mult, too black
  RGB_Blobs1, pretty good, try playing with color
  Spiralus2 maybe change angle mult
  Chasing_Spirals pretty good...


perfect dont touch
  Module_Experiment9_Hsi
  {Module_Experiment11_Hsi, "Module_Experiment11_Hsi"}, //might get too black, really sweet though
  Module_Experiment1 pretty cool but blue only, fix r g
  Parametric_Water
  Water
  Complex_Kaleido_6, maybe just change the angle multiplier to less than 16
  Complex_Kaleido_4
  Complex_Kaleido_3
  Complex_Kaleido_2
  Complex_Kaleido_1
  SM4, SM5 amazing
  SM1 awesome swirl!!!!!
  Slow_Fade, maybe too black
  Spiralus
  Caleido3
  Center_Field so fing cool


      /*
      //put this delta into the buffer
      bpm_deltas[bpm_current_sample] = delta;

      //bpm_deltas is a round robin buffer, increment bpm_current_sample index and wrap around if needed
      bpm_current_sample ++;
      if (bpm_current_sample >= bpm_num_samples) bpm_current_sample = 0;

      //sort then for median
      //selectionSort(bpm_deltas, bpm_sorted_deltas, bpm_num_samples);
      //todo really this isnt so good, because many 8th notes will skew the quarter notes,  
      //really you should somehow detect if samples are multiples of one another within some margin, 
      //and throw out the smaller ones, that should leave you with only quarters?
      //honestly the whole bpm thing is totally uneeded anyways
      //bpm_median_delta = bpm_sorted_deltas[bpm_median_sample];
      bpm = 60000/bpm_median_delta;// to convert millis per beat to bpm, invert to get beats per milli, then multiply by 60000 millis per 1 second
      */

