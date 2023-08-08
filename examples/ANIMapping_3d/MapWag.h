/*
  ___        _            
 / _ \      (_)
/ /_\ \_ __  _ _ __ ___
|  _  | '_ \| | '_ ` _ \
| | | | | | | | | | | | |
\_| |_/_| |_|_|_| |_| |_

by Clark Reinholtz, 
This is the set of functions and objects specific to the wiggle waggle, a 4 foot diameter sphere with ws2815's

VO.42 beta version
 
This code is licensed under a Creative Commons Attribution 
License CC BY-NC 3.0

*/

#pragma once
#define ART_WAG true


//******************************************************************************************************************
//Octo stuff
//******************************************************************************************************************
#include <OctoWS2811.h>

// Any group of digital pins may be used
const int numPins = 8;

//top jack, bottom jack
//orange, blue, green, brown, orange, blue, green, brown
//defaults {2, 14, 7, 8, 6, 20, 21, 5};
//I USED {2, 14, 7, 8, 6, 20, 21,***9***}; because I needed pin 5 for I2S
byte pinList[numPins] = {2, 14, 7, 8, 6, 20, 21, 9};

const int ledsPerStrip = 86; // set this to your MAX leds per strip
const int bytesPerLED = 3;  // change to 4 if using RGBW
DMAMEM int displayMemory[ledsPerStrip * numPins * bytesPerLED / 4];
int drawingMemory[ledsPerStrip * numPins * bytesPerLED / 4];
const int config = WS2811_GRB | WS2811_800kHz;


OctoWS2811 oleds(ledsPerStrip, displayMemory, drawingMemory, config, numPins, pinList);


//******************************************************************************************************************
//python generated
//******************************************************************************************************************



// generated via python
// total circumference inches:  797.9645340118075
const int nRings =  8 ; // number of rings, use this for assigning to pins
#define NUM_LEDS  604
//const int nMaxPixels = 86
const int nPixelsPerRing [ 8 ] = { //number of pixels per ring, used to mapto buffers and pins later
 65
, 86
, 86
, 65
, 65
, 86
, 86
, 65
};
float ledMap [ 604 ][3] = { // { x,y,z} pixel map
//ring:  0  nPixels this ring: 65
  { 11.799596126562978 ,  6.812499999999999 ,  -15.0 },
  { 11.089750512803452 ,  7.915684339573905 ,  -15.0 },
  { 10.277102440536884 ,  8.945489948946944 ,  -15.0 },
  { 9.369185191926253 ,  9.892370486359164 ,  -15.0 },
  { 8.374415198447663 ,  10.747548328991536 ,  -15.0 },
  { 7.302014020212915 ,  11.50309594190251 ,  -15.0 },
  { 6.16192286171539 ,  12.152009366613788 ,  -15.0 },
  { 4.964710416441956 ,  12.688273148103045 ,  -15.0 },
  { 3.7214748946352376 ,  13.106916098327616 ,  -15.0 },
  { 2.4437411424130557 ,  13.4040573793489 ,  -15.0 },
  { 1.1433538059551562 ,  13.576942478864963 ,  -15.0 },
  { -0.16763246887019034 ,  13.62396874465662 ,  -15.0 },
  { -1.4770647835856532 ,  13.54470024124163 ,  -15.0 },
  { -2.7728046449901598 ,  13.339871791015122 ,  -15.0 },
  { -4.042840489506666 ,  13.01138216241478 ,  -15.0 },
  { -5.2753990308869785 ,  12.562276468256728 ,  -15.0 },
  { -6.459054399075546 ,  11.996717937410333 ,  -15.0 },
  { -7.582834058510424 ,  11.319949321489664 ,  -15.0 },
  { -8.636320523994206 ,  10.538244294323245 ,  -15.0 },
  { -9.60974793122447 ,  9.658849294731077 ,  -15.0 },
  { -10.494092566770892 ,  8.689916351728819 ,  -15.0 },
  { -11.281156518282174 ,  7.640427514872424 ,  -15.0 },
  { -11.963643669481925 ,  6.520111590277045 ,  -15.0 },
  { -12.535227335476574 ,  5.339353954170944 ,  -15.0 },
  { -12.990608911393188 ,  4.109100280016526 ,  -15.0 },
  { -13.325566990671815 ,  2.8407550716522034 ,  -15.0 },
  { -13.536996497683663 ,  1.5460759430572164 ,  -15.0 },
  { -13.622937471914142 ,  0.23706462477037168 ,  -15.0 },
  { -13.58259323688012 ,  -1.0741442926620361 ,  -15.0 },
  { -13.416337785354768 ,  -2.3753958468562466 ,  -15.0 },
  { -13.125712312438667 ,  -3.654627380608857 ,  -15.0 },
  { -12.713410928615735 ,  -4.89998036324167 ,  -15.0 },
  { -12.18325568523455 ,  -6.099910319685038 ,  -15.0 },
  { -11.540161143929932 ,  -7.243293848252312 ,  -15.0 },
  { -10.790088818427757 ,  -8.319531735046167 ,  -15.0 },
  { -9.939991911059368 ,  -9.318647209121846 ,  -15.0 },
  { -8.997750856280518 ,  -10.231378427577743 ,  -15.0 },
  { -7.972100268709062 ,  -11.04926433323273 ,  -15.0 },
  { -6.872547972876294 ,  -11.76472308898573 ,  -15.0 },
  { -5.709286865289501 ,  -12.37112236176786 ,  -15.0 },
  { -4.493100425848197 ,  -12.8628408045518 ,  -15.0 },
  { -3.2352627545272536 ,  -13.235320166477603 ,  -15.0 },
  { -1.9474340599909836 ,  -13.485107548031905 ,  -15.0 },
  { -0.6415525689633691 ,  -13.609887409573105 ,  -15.0 },
  { 0.670276141640989 ,  -13.60850303648226 ,  -15.0 },
  { 1.9758913639292883 ,  -13.48096726195675 ,  -15.0 },
  { 3.2631899893094563 ,  -13.228462348045996 ,  -15.0 },
  { 4.520238704821113 ,  -12.853329026032018 ,  -15.0 },
  { 5.735384615453812 ,  -12.359044797750988 ,  -15.0 },
  { 6.8973632669789895 ,  -11.750191699003423 ,  -15.0 },
  { 7.995403067919285 ,  -11.03241382388763 ,  -15.0 },
  { 9.019325142659348 ,  -10.212365003807538 ,  -15.0 },
  { 9.959637690055285 ,  -9.297647126172848 ,  -15.0 },
  { 10.807623972834628 ,  -8.29673966457971 ,  -15.0 },
  { 11.555423122121265 ,  -7.21892107373015 ,  -15.0 },
  { 12.196103008023956 ,  -6.074182777762701 ,  -15.0 },
  { 12.72372450077502 ,  -4.873136549326056 ,  -15.0 },
  { 13.133396526715643 ,  -3.62691613799568 ,  -15.0 },
  { 13.421321408756548 ,  -2.3470740599421114 ,  -15.0 },
  { 13.584830071005996 ,  -1.045474505615138 ,  -15.0 },
  { 13.622406781216712 ,  0.2658166418057874 ,  -15.0 },
  { 13.533703201688244 ,  1.574643657661429 ,  -15.0 },
  { 13.319541618372233 ,  2.868873659896861 ,  -15.0 },
  { 12.981907318246705 ,  4.13650908139371 ,  -15.0 },
  { 12.523930185621428 ,  5.365798887927171 ,  -15.0 },
//ring:  1  nPixels this ring: 86
  { 11.650525425568526 ,  13.884555531531477 ,  -8.0 },
  { 10.615570041063908 ,  14.690993761596472 ,  -8.0 },
  { 9.524987344254296 ,  15.420448796704832 ,  -8.0 },
  { 8.384492165717342 ,  16.069098173918302 ,  -8.0 },
  { 7.200060885546944 ,  16.633542865078894 ,  -8.0 },
  { 5.977900116150256 ,  17.11082508826885 ,  -8.0 },
  { 4.72441417858993 ,  17.49844380706863 ,  -8.0 },
  { 3.446171542900921 ,  17.794367836394187 ,  -8.0 },
  { 2.1498704082399294 ,  17.997046486236965 ,  -8.0 },
  { 0.8423036032332272 ,  18.10541768753155 ,  -8.0 },
  { -0.46967700954935376 ,  18.11891355757019 ,  -8.0 },
  { -1.7791964384678927 ,  18.037463375800467 ,  -8.0 },
  { -3.079392588885922 ,  17.861493954412506 ,  -8.0 },
  { -4.363452221661555 ,  17.591927401773713 ,  -8.0 },
  { -5.624646655627954 ,  17.23017629043108 ,  -8.0 },
  { -6.856367026976186 ,  16.77813625500144 ,  -8.0 },
  { -8.052158920775703 ,  16.23817605873801 ,  -8.0 },
  { -9.205756193157546 ,  15.613125180825955 ,  -8.0 },
  { -10.311113806926663 ,  14.90625898945159 ,  -8.0 },
  { -11.362439508539259 ,  14.12128157834076 ,  -8.0 },
  { -12.354224180452727 ,  13.262306356705729 ,  -8.0 },
  { -13.281270709796974 ,  12.333834494312343 ,  -8.0 },
  { -14.1387212220905 ,  11.340731334618935 ,  -8.0 },
  { -14.922082537292201 ,  10.288200899585851 ,  -8.0 },
  { -15.627249714795044 ,  9.181758619754618 ,  -8.0 },
  { -16.250527563981993 ,  8.027202432495487 ,  -8.0 },
  { -16.78865000762542 ,  6.8305823998733075 ,  -8.0 },
  { -17.238797196662563 ,  5.598169005339065 ,  -8.0 },
  { -17.598610286662943 ,  4.336420295377435 ,  -8.0 },
  { -17.866203798556523 ,  3.0519480382936495 ,  -8.0 },
  { -18.0401754988505 ,  1.7514830774729895 ,  -8.0 },
  { -18.119613747560663 ,  0.4418400606675168 ,  -8.0 },
  { -18.10410227535312 ,  -0.8701182698656641 ,  -8.0 },
  { -17.99372236486323 ,  -2.1775170392492447 ,  -8.0 },
  { -17.789052424761397 ,  -3.4735052654473804 ,  -8.0 },
  { -17.491164958797626 ,  -4.751291759524986 ,  -8.0 },
  { -17.101620945707566 ,  -6.00418071258154 ,  -8.0 },
  { -16.622461659430247 ,  -7.225606782877924 ,  -8.0 },
  { -16.056197972500854 ,  -8.409169499294173 ,  -8.0 },
  { -15.405797198670614 ,  -9.548666800838356 ,  -8.0 },
  { -14.674667543700426 ,  -10.63812753645421 ,  -8.0 },
  { -13.866640245808785 ,  -11.671842754823082 ,  -8.0 },
  { -12.985949499360855 ,  -12.644395620196704 ,  -8.0 },
  { -12.037210267001806 ,  -13.55068979749689 ,  -8.0 },
  { -11.025394096502536 ,  -14.385976157939615 ,  -8.0 },
  { -9.955803069041336 ,  -15.14587766524169 ,  -8.0 },
  { -8.834042015436813 ,  -15.826412312002272 ,  -8.0 },
  { -7.665989145923482 ,  -16.424013986068793 ,  -8.0 },
  { -6.457765247374731 ,  -16.935551157544275 ,  -8.0 },
  { -5.215701609384908 ,  -17.358343288512867 ,  -8.0 },
  { -3.9463068472828216 ,  -17.690174879494233 ,  -8.0 },
  { -2.6562327959295575 ,  -17.92930707902088 ,  -8.0 },
  { -1.3522396530228145 ,  -18.07448679550246 ,  -8.0 },
  { -0.04116055456222458 ,  -18.124953263629347 ,  -8.0 },
  { 1.2701342318938844 ,  -18.080442030906525 ,  -8.0 },
  { 2.5747733085469697 ,  -17.94118634342763 ,  -8.0 },
  { 3.8659201545996362 ,  -17.7079159236275 ,  -8.0 },
  { 5.136808950735604 ,  -17.381853146418035 ,  -8.0 },
  { 6.380780033112587 ,  -16.964706633745003 ,  -8.0 },
  { 7.591314791083229 ,  -16.458662301131312 ,  -8.0 },
  { 8.76206982577422 ,  -15.866371903124447 ,  -8.0 },
  { 9.88691019052748 ,  -15.190939137671636 ,  -8.0 },
  { 10.959941539017608 ,  -14.435903382238202 ,  -8.0 },
  { 11.97554101258466 ,  -13.605221146894408 ,  -8.0 },
  { 12.928386704927412 ,  -12.703245341559619 ,  -8.0 },
  { 13.813485549757239 ,  -11.734702466046933 ,  -8.0 },
  { 14.62619948527701 ,  -10.704667842435958 ,  -8.0 },
  { 15.362269758378726 ,  -9.618539019560219 ,  -8.0 },
  { 16.017839241201898 ,  -8.482007488974094 ,  -8.0 },
  { 16.589472643110447 ,  -7.301028860612051 ,  -8.0 },
  { 17.074174512173975 ,  -6.081791654424592 ,  -8.0 },
  { 17.469404931822666 ,  -4.830684871528042 ,  -8.0 },
  { 17.77309283042296 ,  -3.5542645148002063 ,  -8.0 },
  { 17.983646834029475 ,  -2.2592192343598874 ,  -8.0 },
  { 18.09996360544313 ,  -0.9523352779531453 ,  -8.0 },
  { 18.12143362587741 ,  0.3595390700878664 ,  -8.0 },
  { 18.047944388935818 ,  1.6695293749677234 ,  -8.0 },
  { 17.879880990163745 ,  2.9707710745833684 ,  -8.0 },
  { 17.618124109085155 ,  4.2564454509452405 ,  -8.0 },
  { 17.264045394298805 ,  5.519815361367649 ,  -8.0 },
  { 16.81950027581663 ,  6.754260542191449 ,  -8.0 },
  { 16.286818242308925 ,  7.953312300041615 ,  -8.0 },
  { 15.668790634205134 ,  9.110687408831753 ,  -8.0 },
  { 14.96865601661634 ,  10.220321034890024 ,  -8.0 },
  { 14.190083208727685 ,  11.276398517673298 ,  -8.0 },
  { 13.337152058589663 ,  12.273385839533343 ,  -8.0 },
//ring:  2  nPixels this ring: 86
  { 18.125 ,  0.0 ,  8.0 },
  { 18.077511027122057 ,  1.311190323439149 ,  8.0 },
  { 17.935292958424224 ,  2.615509796482974 ,  8.0 },
  { 17.6990910397004 ,  3.906123573108904 ,  8.0 },
  { 17.370143007393278 ,  5.176268627371108 ,  8.0 },
  { 16.95017260265426 ,  6.41928919275561 ,  8.0 },
  { 16.441380538659633 ,  7.6286716394785445 ,  8.0 },
  { 15.846432968515893 ,  8.79807860696485 ,  8.0 },
  { 15.168447514184125 ,  9.921382212647664 ,  8.0 },
  { 14.410976929634248 ,  10.992696163068889 ,  8.0 },
  { 13.577990483836805 ,  12.006406599013593 ,  8.0 },
  { 12.673853161148447 ,  12.957201513044685 ,  8.0 },
  { 11.703302788084496 ,  13.84009858528521 ,  8.0 },
  { 10.671425206337982 ,  14.650471291584251 ,  8.0 },
  { 9.583627622142613 ,  15.384073147255416 ,  8.0 },
  { 8.445610271633285 ,  16.037059959346735 ,  8.0 },
  { 7.263336550682323 ,  16.606009970836528 ,  8.0 },
  { 6.043001765735914 ,  17.087941791196286 ,  8.0 },
  { 4.791000669401439 ,  17.48033001936162 ,  8.0 },
  { 3.5138939509044858 ,  17.78111847724425 ,  8.0 },
  { 2.2183738570110707 ,  17.988730984439393 ,  8.0 },
  { 0.911229123566894 ,  18.102079617667236 ,  8.0 },
  { -0.4006905985817927 ,  18.120570411667735 ,  8.0 },
  { -1.7105106368718905 ,  18.044106471675125 ,  8.0 },
  { -3.011367321424957 ,  17.873088481162227 ,  8.0 },
  { -4.296443951792897 ,  17.608412602193937 ,  8.0 },
  { -5.5590065175762025 ,  17.25146577939235 ,  8.0 },
  { -6.792438985732855 ,  16.80411847212154 ,  8.0 },
  { -7.990277969666775 ,  16.26871485297649 ,  8.0 },
  { -9.146246598424002 ,  15.6480605239377 ,  8.0 },
  { -10.254287408516674 ,  14.945407814560845 ,  8.0 },
  { -11.308594086016173 ,  14.164438739241312 ,  8.0 },
  { -12.303641892581762 ,  13.309245702860363 ,  8.0 },
  { -13.234216615987433 ,  12.384310055918421 ,  8.0 },
  { -14.095441893441393 ,  11.394478611530118 ,  8.0 },
  { -14.882804764519591 ,  10.344938247335891 ,  8.0 },
  { -15.592179319811857 ,  9.241188725420093 ,  8.0 },
  { -16.219848321357745 ,  8.089013872663859 ,  8.0 },
  { -16.762522681577586 ,  6.894451272552226 ,  8.0 },
  { -17.21735869862596 ,  5.663760627255439 ,  8.0 },
  { -17.58197295785157 ,  4.403390955772179 ,  8.0 },
  { -17.854454821277322 ,  3.1199468000219155 ,  8.0 },
  { -18.033376439653903 ,  1.8201536159718679 ,  8.0 },
  { -18.117800234621882 ,  0.5108225311546389 ,  8.0 },
  { -18.107283811774693 ,  -0.8011853467469976 ,  8.0 },
  { -18.001882278877233 ,  -2.10899488321947 ,  8.0 },
  { -17.802147957092206 ,  -3.4057529437406036 ,  8.0 },
  { -17.509127486727486 ,  -4.684664305318369 ,  8.0 },
  { -17.124356342670772 ,  -5.939027264563725 ,  8.0 },
  { -16.64985078825165 ,  -7.1622687557055436 ,  8.0 },
  { -16.08809730969404 ,  -8.347978794523584 ,  8.0 },
  { -15.442039586524258 ,  -9.489944067707551 ,  8.0 },
  { -14.715063066211897 ,  -10.582180491628677 ,  8.0 },
  { -13.91097722387487 ,  -11.618964569910462 ,  8.0 },
  { -13.033995600010641 ,  -12.594863385479941 ,  8.0 },
  { -12.088713720859221 ,  -13.504763069936104 ,  8.0 },
  { -11.080085017098792 ,  -14.34389560105144 ,  8.0 },
  { -10.0133948670639 ,  -15.107863787983343 ,  8.0 },
  { -8.894232900503985 ,  -15.792664313268755 ,  8.0 },
  { -7.728463708014991 ,  -16.394708710858367 ,  8.0 },
  { -6.522196109631601 ,  -16.910842170261848 ,  8.0 },
  { -5.281751143617308 ,  -17.338360068267622 ,  8.0 },
  { -4.013628943196694 ,  -17.67502214160802 ,  8.0 },
  { -2.724474674800856 ,  -17.91906422630291 ,  8.0 },
  { -1.421043716315263 ,  -18.069207502165693 ,  8.0 },
  { -0.11016625780181172 ,  -18.12466519402888 ,  8.0 },
  { 1.201288489807217 ,  -18.085146694573883 ,  8.0 },
  { 2.5064482904909053 ,  -17.95085908716057 ,  8.0 },
  { 3.7984738947732595 ,  -17.722506060676803 ,  8.0 },
  { 5.070594878502058 ,  -17.401284222094322 ,  8.0 },
  { 6.31614512097181 ,  -16.988876826053687 ,  8.0 },
  { 7.528597736479564 ,  -16.48744495433646 ,  8.0 },
  { 8.701599276266878 ,  -15.89961619144537 ,  8.0 },
  { 9.829003021623805 ,  -15.228470855634525 ,  8.0 },
  { 10.904901193693975 ,  -14.47752585754112 ,  8.0 },
  { 11.923655911195992 ,  -13.650716271002077 ,  8.0 },
  { 12.87992873383757 ,  -12.752374712627656 ,  8.0 },
  { 13.768708636610333 ,  -11.787208638186229 ,  8.0 },
  { 14.585338268375292 ,  -10.760275674770941 ,  8.0 },
  { 15.325538357139697 ,  -9.676957118011831 ,  8.0 },
  { 15.985430134137797 ,  -8.542929733212095 ,  8.0 },
  { 16.561555659209578 ,  -7.364136008175248 ,  8.0 },
  { 17.05089594096916 ,  -6.146753013603315 ,  8.0 },
  { 17.45088675681036 ,  -4.8971600342423836 ,  8.0 },
  { 17.75943208984988 ,  -3.6219051403937037 ,  8.0 },
  { 17.974915112396424 ,  -2.327670874961189 ,  8.0 },
//ring:  3  nPixels this ring: 65
  { 13.625 ,  0.0 ,  15.0 },
  { 13.561847835506248 ,  1.310308470007923 ,  15.0 },
  { 13.372976765273467 ,  2.608470324817975 ,  15.0 },
  { 13.060137632148697 ,  3.882451548871333 ,  15.0 },
  { 12.626230468189947 ,  5.120442282083833 ,  15.0 },
  { 12.075277611245776 ,  6.310966297750803 ,  15.0 },
  { 11.412386417712527 ,  7.442987387656292 ,  15.0 },
  { 10.643701917123474 ,  8.506011668192214 ,  15.0 },
  { 9.77634984746394 ,  9.490184859105314 ,  15.0 },
  { 8.818370599277362 ,  10.38638363309389 ,  15.0 },
  { 7.778644680903269 ,  11.18630018943955 ,  15.0 },
  { 6.6668103957876195 ,  11.882519267672917 ,  15.0 },
  { 5.4931744950002726 ,  12.468586887353293 ,  15.0 },
  { 4.268616633214592 ,  12.939070176741593 ,  15.0 },
  { 3.004488513846302 ,  13.289607735752234 ,  15.0 },
  { 1.71250865828043 ,  13.516950066317273 ,  15.0 },
  { 0.4046537746801094 ,  13.618989695371575 ,  15.0 },
  { -0.9069522666070537 ,  13.594780711217682 ,  15.0 },
  { -2.2101508218422934 ,  13.444547532167448 ,  15.0 },
  { -3.4928611850398075 ,  13.169682826174755 ,  15.0 },
  { -4.74319257662463 ,  12.772734600744393 ,  15.0 },
  { -5.949554371464563 ,  12.257382582794211 ,  15.0 },
  { -7.100763544457707 ,  11.628404107430239 ,  15.0 },
  { -8.186148337650362 ,  10.891629831847217 ,  15.0 },
  { -9.195647187886753 ,  10.053889684888654 ,  15.0 },
  { -10.119901997927046 ,  9.122949553316197 ,  15.0 },
  { -10.95034488640642 ,  8.107439291709362 ,  15.0 },
  { -11.67927761245942 ,  7.016772723346836 ,  15.0 },
  { -12.29994293873991 ,  5.861060373664668 ,  15.0 },
  { -12.806587271298408 ,  4.6510157452558225 ,  15.0 },
  { -13.194513995642504 ,  3.397856003245881 ,  15.0 },
  { -13.460127014552773 ,  2.1131979916956785 ,  15.0 },
  { -13.600964084056828 ,  0.8089505449631738 ,  15.0 },
  { -13.615719638535566 ,  -0.5027959076970849 ,  15.0 },
  { -13.504256893371938 ,  -1.8098814209269332 ,  15.0 },
  { -13.267609112950165 ,  -3.1001892564747964 ,  15.0 },
  { -12.907970032250985 ,  -4.361758205874152 ,  15.0 },
  { -12.42867352083516 ,  -5.58289347135434 ,  15.0 },
  { -11.834162677730982 ,  -6.75227507711283 ,  15.0 },
  { -11.12994864371746 ,  -7.859062805971961 ,  15.0 },
  { -10.32255951281512 ,  -8.892996688652824 ,  15.0 },
  { -9.419479816577024 ,  -9.84449211412646 ,  15.0 },
  { -8.429081142163215 ,  -10.704728679365417 ,  15.0 },
  { -7.360544527372 ,  -11.46573195485461 ,  15.0 },
  { -6.22377535202942 ,  -12.1204474078918 ,  15.0 },
  { -5.029311514697331 ,  -12.662805798405534 ,  15.0 },
  { -3.788225745905941 ,  -13.087779441068504 ,  15.0 },
  { -2.5120229634712605 ,  -13.391428812154178 ,  15.0 },
  { -1.2125336214180518 ,  -13.57093906908917 ,  15.0 },
  { 0.0981959588317188 ,  -13.624646144163492 ,  15.0 },
  { 1.4080152583703196 ,  -13.55205217050895 ,  15.0 },
  { 2.704782196630929 ,  -13.353830097346169 ,  15.0 },
  { 3.9764756889904147 ,  -13.03181745171671 ,  15.0 },
  { 5.2113070827346295 ,  -12.588999304529327 ,  15.0 },
  { 6.397829438370028 ,  -12.02948059882578 ,  15.0 },
  { 7.525043643239201 ,  -11.358448096784407 ,  15.0 },
  { 8.582500373762612 ,  -10.582122298214315 ,  15.0 },
  { 9.560396961112215 ,  -9.70769977625783 ,  15.0 },
  { 10.449668262368064 ,  -8.743286464851625 ,  15.0 },
  { 11.242070694778382 ,  -7.697822516374688 ,  15.0 },
  { 11.930258654121971 ,  -6.580999426055879 ,  15.0 },
  { 12.507852608771525 ,  -5.403170191401281 ,  15.0 },
  { 12.969498238222878 ,  -4.1752533394674005 ,  15.0 },
  { 13.310916067873617 ,  -2.908631711651372 ,  15.0 },
  { 13.528941139934501 ,  -1.615046944267497 ,  15.0 },
//ring:  4  nPixels this ring: 65
  { 13.625 ,  -15.0 ,  0.0 },
  { 13.561847835506248 ,  -15.0 ,  1.310308470007923 },
  { 13.372976765273467 ,  -15.0 ,  2.608470324817975 },
  { 13.060137632148697 ,  -15.0 ,  3.882451548871333 },
  { 12.626230468189947 ,  -15.0 ,  5.120442282083833 },
  { 12.075277611245776 ,  -15.0 ,  6.310966297750803 },
  { 11.412386417712527 ,  -15.0 ,  7.442987387656292 },
  { 10.643701917123474 ,  -15.0 ,  8.506011668192214 },
  { 9.77634984746394 ,  -15.0 ,  9.490184859105314 },
  { 8.818370599277362 ,  -15.0 ,  10.38638363309389 },
  { 7.778644680903269 ,  -15.0 ,  11.18630018943955 },
  { 6.6668103957876195 ,  -15.0 ,  11.882519267672917 },
  { 5.4931744950002726 ,  -15.0 ,  12.468586887353293 },
  { 4.268616633214592 ,  -15.0 ,  12.939070176741593 },
  { 3.004488513846302 ,  -15.0 ,  13.289607735752234 },
  { 1.71250865828043 ,  -15.0 ,  13.516950066317273 },
  { 0.4046537746801094 ,  -15.0 ,  13.618989695371575 },
  { -0.9069522666070537 ,  -15.0 ,  13.594780711217682 },
  { -2.2101508218422934 ,  -15.0 ,  13.444547532167448 },
  { -3.4928611850398075 ,  -15.0 ,  13.169682826174755 },
  { -4.74319257662463 ,  -15.0 ,  12.772734600744393 },
  { -5.949554371464563 ,  -15.0 ,  12.257382582794211 },
  { -7.100763544457707 ,  -15.0 ,  11.628404107430239 },
  { -8.186148337650362 ,  -15.0 ,  10.891629831847217 },
  { -9.195647187886753 ,  -15.0 ,  10.053889684888654 },
  { -10.119901997927046 ,  -15.0 ,  9.122949553316197 },
  { -10.95034488640642 ,  -15.0 ,  8.107439291709362 },
  { -11.67927761245942 ,  -15.0 ,  7.016772723346836 },
  { -12.29994293873991 ,  -15.0 ,  5.861060373664668 },
  { -12.806587271298408 ,  -15.0 ,  4.6510157452558225 },
  { -13.194513995642504 ,  -15.0 ,  3.397856003245881 },
  { -13.460127014552773 ,  -15.0 ,  2.1131979916956785 },
  { -13.600964084056828 ,  -15.0 ,  0.8089505449631738 },
  { -13.615719638535566 ,  -15.0 ,  -0.5027959076970849 },
  { -13.504256893371938 ,  -15.0 ,  -1.8098814209269332 },
  { -13.267609112950165 ,  -15.0 ,  -3.1001892564747964 },
  { -12.907970032250985 ,  -15.0 ,  -4.361758205874152 },
  { -12.42867352083516 ,  -15.0 ,  -5.58289347135434 },
  { -11.834162677730982 ,  -15.0 ,  -6.75227507711283 },
  { -11.12994864371746 ,  -15.0 ,  -7.859062805971961 },
  { -10.32255951281512 ,  -15.0 ,  -8.892996688652824 },
  { -9.419479816577024 ,  -15.0 ,  -9.84449211412646 },
  { -8.429081142163215 ,  -15.0 ,  -10.704728679365417 },
  { -7.360544527372 ,  -15.0 ,  -11.46573195485461 },
  { -6.22377535202942 ,  -15.0 ,  -12.1204474078918 },
  { -5.029311514697331 ,  -15.0 ,  -12.662805798405534 },
  { -3.788225745905941 ,  -15.0 ,  -13.087779441068504 },
  { -2.5120229634712605 ,  -15.0 ,  -13.391428812154178 },
  { -1.2125336214180518 ,  -15.0 ,  -13.57093906908917 },
  { 0.0981959588317188 ,  -15.0 ,  -13.624646144163492 },
  { 1.4080152583703196 ,  -15.0 ,  -13.55205217050895 },
  { 2.704782196630929 ,  -15.0 ,  -13.353830097346169 },
  { 3.9764756889904147 ,  -15.0 ,  -13.03181745171671 },
  { 5.2113070827346295 ,  -15.0 ,  -12.588999304529327 },
  { 6.397829438370028 ,  -15.0 ,  -12.02948059882578 },
  { 7.525043643239201 ,  -15.0 ,  -11.358448096784407 },
  { 8.582500373762612 ,  -15.0 ,  -10.582122298214315 },
  { 9.560396961112215 ,  -15.0 ,  -9.70769977625783 },
  { 10.449668262368064 ,  -15.0 ,  -8.743286464851625 },
  { 11.242070694778382 ,  -15.0 ,  -7.697822516374688 },
  { 11.930258654121971 ,  -15.0 ,  -6.580999426055879 },
  { 12.507852608771525 ,  -15.0 ,  -5.403170191401281 },
  { 12.969498238222878 ,  -15.0 ,  -4.1752533394674005 },
  { 13.310916067873617 ,  -15.0 ,  -2.908631711651372 },
  { 13.528941139934501 ,  -15.0 ,  -1.615046944267497 },
//ring:  5  nPixels this ring: 86
  { 18.125 ,  -8.0 ,  0.0 },
  { 18.077511027122057 ,  -8.0 ,  1.311190323439149 },
  { 17.935292958424224 ,  -8.0 ,  2.615509796482974 },
  { 17.6990910397004 ,  -8.0 ,  3.906123573108904 },
  { 17.370143007393278 ,  -8.0 ,  5.176268627371108 },
  { 16.95017260265426 ,  -8.0 ,  6.41928919275561 },
  { 16.441380538659633 ,  -8.0 ,  7.6286716394785445 },
  { 15.846432968515893 ,  -8.0 ,  8.79807860696485 },
  { 15.168447514184125 ,  -8.0 ,  9.921382212647664 },
  { 14.410976929634248 ,  -8.0 ,  10.992696163068889 },
  { 13.577990483836805 ,  -8.0 ,  12.006406599013593 },
  { 12.673853161148447 ,  -8.0 ,  12.957201513044685 },
  { 11.703302788084496 ,  -8.0 ,  13.84009858528521 },
  { 10.671425206337982 ,  -8.0 ,  14.650471291584251 },
  { 9.583627622142613 ,  -8.0 ,  15.384073147255416 },
  { 8.445610271633285 ,  -8.0 ,  16.037059959346735 },
  { 7.263336550682323 ,  -8.0 ,  16.606009970836528 },
  { 6.043001765735914 ,  -8.0 ,  17.087941791196286 },
  { 4.791000669401439 ,  -8.0 ,  17.48033001936162 },
  { 3.5138939509044858 ,  -8.0 ,  17.78111847724425 },
  { 2.2183738570110707 ,  -8.0 ,  17.988730984439393 },
  { 0.911229123566894 ,  -8.0 ,  18.102079617667236 },
  { -0.4006905985817927 ,  -8.0 ,  18.120570411667735 },
  { -1.7105106368718905 ,  -8.0 ,  18.044106471675125 },
  { -3.011367321424957 ,  -8.0 ,  17.873088481162227 },
  { -4.296443951792897 ,  -8.0 ,  17.608412602193937 },
  { -5.5590065175762025 ,  -8.0 ,  17.25146577939235 },
  { -6.792438985732855 ,  -8.0 ,  16.80411847212154 },
  { -7.990277969666775 ,  -8.0 ,  16.26871485297649 },
  { -9.146246598424002 ,  -8.0 ,  15.6480605239377 },
  { -10.254287408516674 ,  -8.0 ,  14.945407814560845 },
  { -11.308594086016173 ,  -8.0 ,  14.164438739241312 },
  { -12.303641892581762 ,  -8.0 ,  13.309245702860363 },
  { -13.234216615987433 ,  -8.0 ,  12.384310055918421 },
  { -14.095441893441393 ,  -8.0 ,  11.394478611530118 },
  { -14.882804764519591 ,  -8.0 ,  10.344938247335891 },
  { -15.592179319811857 ,  -8.0 ,  9.241188725420093 },
  { -16.219848321357745 ,  -8.0 ,  8.089013872663859 },
  { -16.762522681577586 ,  -8.0 ,  6.894451272552226 },
  { -17.21735869862596 ,  -8.0 ,  5.663760627255439 },
  { -17.58197295785157 ,  -8.0 ,  4.403390955772179 },
  { -17.854454821277322 ,  -8.0 ,  3.1199468000219155 },
  { -18.033376439653903 ,  -8.0 ,  1.8201536159718679 },
  { -18.117800234621882 ,  -8.0 ,  0.5108225311546389 },
  { -18.107283811774693 ,  -8.0 ,  -0.8011853467469976 },
  { -18.001882278877233 ,  -8.0 ,  -2.10899488321947 },
  { -17.802147957092206 ,  -8.0 ,  -3.4057529437406036 },
  { -17.509127486727486 ,  -8.0 ,  -4.684664305318369 },
  { -17.124356342670772 ,  -8.0 ,  -5.939027264563725 },
  { -16.64985078825165 ,  -8.0 ,  -7.1622687557055436 },
  { -16.08809730969404 ,  -8.0 ,  -8.347978794523584 },
  { -15.442039586524258 ,  -8.0 ,  -9.489944067707551 },
  { -14.715063066211897 ,  -8.0 ,  -10.582180491628677 },
  { -13.91097722387487 ,  -8.0 ,  -11.618964569910462 },
  { -13.033995600010641 ,  -8.0 ,  -12.594863385479941 },
  { -12.088713720859221 ,  -8.0 ,  -13.504763069936104 },
  { -11.080085017098792 ,  -8.0 ,  -14.34389560105144 },
  { -10.0133948670639 ,  -8.0 ,  -15.107863787983343 },
  { -8.894232900503985 ,  -8.0 ,  -15.792664313268755 },
  { -7.728463708014991 ,  -8.0 ,  -16.394708710858367 },
  { -6.522196109631601 ,  -8.0 ,  -16.910842170261848 },
  { -5.281751143617308 ,  -8.0 ,  -17.338360068267622 },
  { -4.013628943196694 ,  -8.0 ,  -17.67502214160802 },
  { -2.724474674800856 ,  -8.0 ,  -17.91906422630291 },
  { -1.421043716315263 ,  -8.0 ,  -18.069207502165693 },
  { -0.11016625780181172 ,  -8.0 ,  -18.12466519402888 },
  { 1.201288489807217 ,  -8.0 ,  -18.085146694573883 },
  { 2.5064482904909053 ,  -8.0 ,  -17.95085908716057 },
  { 3.7984738947732595 ,  -8.0 ,  -17.722506060676803 },
  { 5.070594878502058 ,  -8.0 ,  -17.401284222094322 },
  { 6.31614512097181 ,  -8.0 ,  -16.988876826053687 },
  { 7.528597736479564 ,  -8.0 ,  -16.48744495433646 },
  { 8.701599276266878 ,  -8.0 ,  -15.89961619144537 },
  { 9.829003021623805 ,  -8.0 ,  -15.228470855634525 },
  { 10.904901193693975 ,  -8.0 ,  -14.47752585754112 },
  { 11.923655911195992 ,  -8.0 ,  -13.650716271002077 },
  { 12.87992873383757 ,  -8.0 ,  -12.752374712627656 },
  { 13.768708636610333 ,  -8.0 ,  -11.787208638186229 },
  { 14.585338268375292 ,  -8.0 ,  -10.760275674770941 },
  { 15.325538357139697 ,  -8.0 ,  -9.676957118011831 },
  { 15.985430134137797 ,  -8.0 ,  -8.542929733212095 },
  { 16.561555659209578 ,  -8.0 ,  -7.364136008175248 },
  { 17.05089594096916 ,  -8.0 ,  -6.146753013603315 },
  { 17.45088675681036 ,  -8.0 ,  -4.8971600342423836 },
  { 17.75943208984988 ,  -8.0 ,  -3.6219051403937037 },
  { 17.974915112396424 ,  -8.0 ,  -2.327670874961189 },
//ring:  6  nPixels this ring: 86
  { 18.125 ,  8.0 ,  0.0 },
  { 18.077511027122057 ,  8.0 ,  1.311190323439149 },
  { 17.935292958424224 ,  8.0 ,  2.615509796482974 },
  { 17.6990910397004 ,  8.0 ,  3.906123573108904 },
  { 17.370143007393278 ,  8.0 ,  5.176268627371108 },
  { 16.95017260265426 ,  8.0 ,  6.41928919275561 },
  { 16.441380538659633 ,  8.0 ,  7.6286716394785445 },
  { 15.846432968515893 ,  8.0 ,  8.79807860696485 },
  { 15.168447514184125 ,  8.0 ,  9.921382212647664 },
  { 14.410976929634248 ,  8.0 ,  10.992696163068889 },
  { 13.577990483836805 ,  8.0 ,  12.006406599013593 },
  { 12.673853161148447 ,  8.0 ,  12.957201513044685 },
  { 11.703302788084496 ,  8.0 ,  13.84009858528521 },
  { 10.671425206337982 ,  8.0 ,  14.650471291584251 },
  { 9.583627622142613 ,  8.0 ,  15.384073147255416 },
  { 8.445610271633285 ,  8.0 ,  16.037059959346735 },
  { 7.263336550682323 ,  8.0 ,  16.606009970836528 },
  { 6.043001765735914 ,  8.0 ,  17.087941791196286 },
  { 4.791000669401439 ,  8.0 ,  17.48033001936162 },
  { 3.5138939509044858 ,  8.0 ,  17.78111847724425 },
  { 2.2183738570110707 ,  8.0 ,  17.988730984439393 },
  { 0.911229123566894 ,  8.0 ,  18.102079617667236 },
  { -0.4006905985817927 ,  8.0 ,  18.120570411667735 },
  { -1.7105106368718905 ,  8.0 ,  18.044106471675125 },
  { -3.011367321424957 ,  8.0 ,  17.873088481162227 },
  { -4.296443951792897 ,  8.0 ,  17.608412602193937 },
  { -5.5590065175762025 ,  8.0 ,  17.25146577939235 },
  { -6.792438985732855 ,  8.0 ,  16.80411847212154 },
  { -7.990277969666775 ,  8.0 ,  16.26871485297649 },
  { -9.146246598424002 ,  8.0 ,  15.6480605239377 },
  { -10.254287408516674 ,  8.0 ,  14.945407814560845 },
  { -11.308594086016173 ,  8.0 ,  14.164438739241312 },
  { -12.303641892581762 ,  8.0 ,  13.309245702860363 },
  { -13.234216615987433 ,  8.0 ,  12.384310055918421 },
  { -14.095441893441393 ,  8.0 ,  11.394478611530118 },
  { -14.882804764519591 ,  8.0 ,  10.344938247335891 },
  { -15.592179319811857 ,  8.0 ,  9.241188725420093 },
  { -16.219848321357745 ,  8.0 ,  8.089013872663859 },
  { -16.762522681577586 ,  8.0 ,  6.894451272552226 },
  { -17.21735869862596 ,  8.0 ,  5.663760627255439 },
  { -17.58197295785157 ,  8.0 ,  4.403390955772179 },
  { -17.854454821277322 ,  8.0 ,  3.1199468000219155 },
  { -18.033376439653903 ,  8.0 ,  1.8201536159718679 },
  { -18.117800234621882 ,  8.0 ,  0.5108225311546389 },
  { -18.107283811774693 ,  8.0 ,  -0.8011853467469976 },
  { -18.001882278877233 ,  8.0 ,  -2.10899488321947 },
  { -17.802147957092206 ,  8.0 ,  -3.4057529437406036 },
  { -17.509127486727486 ,  8.0 ,  -4.684664305318369 },
  { -17.124356342670772 ,  8.0 ,  -5.939027264563725 },
  { -16.64985078825165 ,  8.0 ,  -7.1622687557055436 },
  { -16.08809730969404 ,  8.0 ,  -8.347978794523584 },
  { -15.442039586524258 ,  8.0 ,  -9.489944067707551 },
  { -14.715063066211897 ,  8.0 ,  -10.582180491628677 },
  { -13.91097722387487 ,  8.0 ,  -11.618964569910462 },
  { -13.033995600010641 ,  8.0 ,  -12.594863385479941 },
  { -12.088713720859221 ,  8.0 ,  -13.504763069936104 },
  { -11.080085017098792 ,  8.0 ,  -14.34389560105144 },
  { -10.0133948670639 ,  8.0 ,  -15.107863787983343 },
  { -8.894232900503985 ,  8.0 ,  -15.792664313268755 },
  { -7.728463708014991 ,  8.0 ,  -16.394708710858367 },
  { -6.522196109631601 ,  8.0 ,  -16.910842170261848 },
  { -5.281751143617308 ,  8.0 ,  -17.338360068267622 },
  { -4.013628943196694 ,  8.0 ,  -17.67502214160802 },
  { -2.724474674800856 ,  8.0 ,  -17.91906422630291 },
  { -1.421043716315263 ,  8.0 ,  -18.069207502165693 },
  { -0.11016625780181172 ,  8.0 ,  -18.12466519402888 },
  { 1.201288489807217 ,  8.0 ,  -18.085146694573883 },
  { 2.5064482904909053 ,  8.0 ,  -17.95085908716057 },
  { 3.7984738947732595 ,  8.0 ,  -17.722506060676803 },
  { 5.070594878502058 ,  8.0 ,  -17.401284222094322 },
  { 6.31614512097181 ,  8.0 ,  -16.988876826053687 },
  { 7.528597736479564 ,  8.0 ,  -16.48744495433646 },
  { 8.701599276266878 ,  8.0 ,  -15.89961619144537 },
  { 9.829003021623805 ,  8.0 ,  -15.228470855634525 },
  { 10.904901193693975 ,  8.0 ,  -14.47752585754112 },
  { 11.923655911195992 ,  8.0 ,  -13.650716271002077 },
  { 12.87992873383757 ,  8.0 ,  -12.752374712627656 },
  { 13.768708636610333 ,  8.0 ,  -11.787208638186229 },
  { 14.585338268375292 ,  8.0 ,  -10.760275674770941 },
  { 15.325538357139697 ,  8.0 ,  -9.676957118011831 },
  { 15.985430134137797 ,  8.0 ,  -8.542929733212095 },
  { 16.561555659209578 ,  8.0 ,  -7.364136008175248 },
  { 17.05089594096916 ,  8.0 ,  -6.146753013603315 },
  { 17.45088675681036 ,  8.0 ,  -4.8971600342423836 },
  { 17.75943208984988 ,  8.0 ,  -3.6219051403937037 },
  { 17.974915112396424 ,  8.0 ,  -2.327670874961189 },
//ring:  7  nPixels this ring: 65
  { 13.625 ,  15.0 ,  0.0 },
  { 13.561847835506248 ,  15.0 ,  1.310308470007923 },
  { 13.372976765273467 ,  15.0 ,  2.608470324817975 },
  { 13.060137632148697 ,  15.0 ,  3.882451548871333 },
  { 12.626230468189947 ,  15.0 ,  5.120442282083833 },
  { 12.075277611245776 ,  15.0 ,  6.310966297750803 },
  { 11.412386417712527 ,  15.0 ,  7.442987387656292 },
  { 10.643701917123474 ,  15.0 ,  8.506011668192214 },
  { 9.77634984746394 ,  15.0 ,  9.490184859105314 },
  { 8.818370599277362 ,  15.0 ,  10.38638363309389 },
  { 7.778644680903269 ,  15.0 ,  11.18630018943955 },
  { 6.6668103957876195 ,  15.0 ,  11.882519267672917 },
  { 5.4931744950002726 ,  15.0 ,  12.468586887353293 },
  { 4.268616633214592 ,  15.0 ,  12.939070176741593 },
  { 3.004488513846302 ,  15.0 ,  13.289607735752234 },
  { 1.71250865828043 ,  15.0 ,  13.516950066317273 },
  { 0.4046537746801094 ,  15.0 ,  13.618989695371575 },
  { -0.9069522666070537 ,  15.0 ,  13.594780711217682 },
  { -2.2101508218422934 ,  15.0 ,  13.444547532167448 },
  { -3.4928611850398075 ,  15.0 ,  13.169682826174755 },
  { -4.74319257662463 ,  15.0 ,  12.772734600744393 },
  { -5.949554371464563 ,  15.0 ,  12.257382582794211 },
  { -7.100763544457707 ,  15.0 ,  11.628404107430239 },
  { -8.186148337650362 ,  15.0 ,  10.891629831847217 },
  { -9.195647187886753 ,  15.0 ,  10.053889684888654 },
  { -10.119901997927046 ,  15.0 ,  9.122949553316197 },
  { -10.95034488640642 ,  15.0 ,  8.107439291709362 },
  { -11.67927761245942 ,  15.0 ,  7.016772723346836 },
  { -12.29994293873991 ,  15.0 ,  5.861060373664668 },
  { -12.806587271298408 ,  15.0 ,  4.6510157452558225 },
  { -13.194513995642504 ,  15.0 ,  3.397856003245881 },
  { -13.460127014552773 ,  15.0 ,  2.1131979916956785 },
  { -13.600964084056828 ,  15.0 ,  0.8089505449631738 },
  { -13.615719638535566 ,  15.0 ,  -0.5027959076970849 },
  { -13.504256893371938 ,  15.0 ,  -1.8098814209269332 },
  { -13.267609112950165 ,  15.0 ,  -3.1001892564747964 },
  { -12.907970032250985 ,  15.0 ,  -4.361758205874152 },
  { -12.42867352083516 ,  15.0 ,  -5.58289347135434 },
  { -11.834162677730982 ,  15.0 ,  -6.75227507711283 },
  { -11.12994864371746 ,  15.0 ,  -7.859062805971961 },
  { -10.32255951281512 ,  15.0 ,  -8.892996688652824 },
  { -9.419479816577024 ,  15.0 ,  -9.84449211412646 },
  { -8.429081142163215 ,  15.0 ,  -10.704728679365417 },
  { -7.360544527372 ,  15.0 ,  -11.46573195485461 },
  { -6.22377535202942 ,  15.0 ,  -12.1204474078918 },
  { -5.029311514697331 ,  15.0 ,  -12.662805798405534 },
  { -3.788225745905941 ,  15.0 ,  -13.087779441068504 },
  { -2.5120229634712605 ,  15.0 ,  -13.391428812154178 },
  { -1.2125336214180518 ,  15.0 ,  -13.57093906908917 },
  { 0.0981959588317188 ,  15.0 ,  -13.624646144163492 },
  { 1.4080152583703196 ,  15.0 ,  -13.55205217050895 },
  { 2.704782196630929 ,  15.0 ,  -13.353830097346169 },
  { 3.9764756889904147 ,  15.0 ,  -13.03181745171671 },
  { 5.2113070827346295 ,  15.0 ,  -12.588999304529327 },
  { 6.397829438370028 ,  15.0 ,  -12.02948059882578 },
  { 7.525043643239201 ,  15.0 ,  -11.358448096784407 },
  { 8.582500373762612 ,  15.0 ,  -10.582122298214315 },
  { 9.560396961112215 ,  15.0 ,  -9.70769977625783 },
  { 10.449668262368064 ,  15.0 ,  -8.743286464851625 },
  { 11.242070694778382 ,  15.0 ,  -7.697822516374688 },
  { 11.930258654121971 ,  15.0 ,  -6.580999426055879 },
  { 12.507852608771525 ,  15.0 ,  -5.403170191401281 },
  { 12.969498238222878 ,  15.0 ,  -4.1752533394674005 },
  { 13.310916067873617 ,  15.0 ,  -2.908631711651372 },
  { 13.528941139934501 ,  15.0 ,  -1.615046944267497 }
};
//just printing constants again to prevent scrolling
// total circumference inches:  797.9645340118075
//const int nRings =  8 ; // number of rings, use this for assigning to pins
//#define NUM_LEDS  604
//const int nMaxPixels =  86
// end generated via python
