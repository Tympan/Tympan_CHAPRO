
// Instantiate the audio classess
AudioInputI2SQuad_F32      i2s_in(audio_settings);        //Digital audio in *from* the Teensy Audio Board ADC.
EarpieceMixer_F32_UI       earpieceMixer(audio_settings); //mixes earpiece mics, allows switching to analog inputs, mixes left+right, etc
AudioEffectBTNRH           BTNRH_alg1(audio_settings);    //for processing the left.  See tab "AudioEffectBTNRH.h"
AudioEffectBTNRH           BTNRH_alg2(audio_settings);    //for processing the right. See tab "AudioEffectBTNRH.h"
AudioOutputI2SQuad_F32     i2s_out(audio_settings);       //Digital audio out *to* the Teensy Audio Board DAC.
AudioSDWriter_F32_UI       audioSDWriter(audio_settings); //this is stereo by default


// Starting making the audio connections
AudioConnection_F32        patchcord1(i2s_in,0,earpieceMixer,0); //connect raw audio to the EarpieceMixer (control front-rear, left-right mixing)
AudioConnection_F32        patchcord2(i2s_in,1,earpieceMixer,1); //connect raw audio to the EarpieceMixer (control front-rear, left-right mixing)
AudioConnection_F32        patchcord3(i2s_in,2,earpieceMixer,2); //connect raw audio to the EarpieceMixer (control front-rear, left-right mixing)
AudioConnection_F32        patchcord4(i2s_in,3,earpieceMixer,3); //connect raw audio to the EarpieceMixer (control front-rear, left-right mixing)

//connect the left and right outputs of the earpiece mixer to the two filter modules (one for left, one for right)
AudioConnection_F32        patchCord11(earpieceMixer,  earpieceMixer.LEFT,  BTNRH_alg1, 0);   //connect the Left input to the BTNRH algorithm
AudioConnection_F32        patchCord12(earpieceMixer,  earpieceMixer.RIGHT, BTNRH_alg2, 0);   //connect the Right input to the BTNRH algorithm

//Connect the gain modules to the outputs so that you hear the audio
AudioConnection_F32        patchcord31(BTNRH_alg1, 0, i2s_out, EarpieceShield::OUTPUT_LEFT_TYMPAN);    //First AIC, Main tympan board headphone jack, left channel
AudioConnection_F32        patchcord32(BTNRH_alg2, 0, i2s_out, EarpieceShield::OUTPUT_RIGHT_TYMPAN);   //First AIC, Main tympan board headphone jack, right channel
AudioConnection_F32        patchcord33(BTNRH_alg1, 0, i2s_out, EarpieceShield::OUTPUT_LEFT_EARPIECE);  //Second AIC (Earpiece!), left output
AudioConnection_F32        patchcord34(BTNRH_alg2, 0, i2s_out, EarpieceShield::OUTPUT_RIGHT_EARPIECE); //Secibd AIC (Earpiece!), right output

// Note About the Outputs Being Defined Above: 
//   The four lines above assign audio to the four outputs available on the Tympan+EarpieceShield.  Two
//   of these outputs are part of the base Tympan board (and come out the Tympan's standard headphone jack).
//   The other two inputs are part of the Earpiece shield.  The earpiece shield's audio comes out the two
//   earpieces *and* it comes out of the additional headphone jack that is built into the Earpiece shield.
//
//   This is a lot of potential outputs.  Let's talk about flexibility...
//
//   The two outputs on the Tympan board and two of the outputs on the EarpieceShield can all be sent
//   indpendent signals.  Or they can be sent the same signals.  Whatever you want!  In the example above,
//   there is a left and right audio stream.  The left is sent to both the Tympan board and to the EarpieceShield.
//   The right is also sent to both the Tympan baord and to the Earpiece shield.  This is simple but it doesn't
//   have to be that way.  You could send different audio (such as the raw input audio?) to the Tympan's headphone
//   jack while still listening to the processed audio through the earpieces.  Fun!
//
//   The only rigid, non-flexible routing is that the headphone jack on the EarpieceShield is always outputing
//   the same audio that is going to the earpieces.  That is simply how they are wired.  There is not flexibility
//   here (unless you get out your soldering iron!)  :)

// Connect the raw mic audio to the SD writer
AudioConnection_F32     patchcord41(i2s_in, EarpieceShield::PDM_LEFT_FRONT,  audioSDWriter, 0);    //Left-Front Mic
AudioConnection_F32     patchcord42(i2s_in, EarpieceShield::PDM_LEFT_REAR,   audioSDWriter, 1);    //Left-Rear Mic
AudioConnection_F32     patchcord43(i2s_in, EarpieceShield::PDM_RIGHT_FRONT, audioSDWriter, 2);    //Right-Front Mic
AudioConnection_F32     patchcord44(i2s_in, EarpieceShield::PDM_RIGHT_REAR,  audioSDWriter, 3);    //Right-Rear Mic
