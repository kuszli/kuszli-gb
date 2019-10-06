#define IF_REGISTER 0xFF0F
#define IE_REGISTER 0xFFFF

#define VBLANK_INT (1 << 0)
#define LCD_STAT_INT (1 << 1)
#define TIMER_OV_INT (1 << 2)
#define SERIAL_INT (1 << 3)
#define BUTTON_INT (1 << 4)


//video registers
#define LCDC 0 //0xFF40
#define STAT 1 //0xFF41
#define SCY  2 //0xFF42
#define SCX  3 //0xFF43
#define LY   4 //0xFF44
#define LYC  5 //0xFF45
#define DMA  6 //0xFF46
#define BGP  7 //0xFF47
#define OBP0 8 //0xFF48
#define OBP1 9 //0xFF49
#define WY   10 //0xFF4A
#define WX   11 //0xFF4B

//audio registers

#define S1_SWEEP 0 	 //0xFF10
#define S1_LENGTH 1 	 //0xFF11
#define S1_ENVELOPE 2 //0xFF12
#define S1_FREQ 3 	 //0xFF13
#define S1_CONT 4 	 //0xFF14

#define S2_LENGTH 6 	 //0xFF16
#define S2_ENVELOPE 7 //0xFF17
#define S2_FREQ 8 	 //0xFF18
#define S2_CONT 9 	 //0xFF19

#define S3_ONOFF 10 	 //0xFF1A
#define S3_LENGTH 11	 //0xFF1B
#define S3_PATTERN 12 //0xFF1C
#define S3_FREQ 13 	 //0xFF1D
#define S3_CONT 14 	 //0xFF1E

#define S4_LENGTH 16  //0xFF20
#define S4_ENVELOPE 17//0xFF21
#define S4_PCOUNTER 18//0xFF22
#define S4_CONT 19 	 //0xFF23

#define CHANNEL_CONT 20  //0xFF24
#define SOUND_LEVEL 21   //0xFF25
#define AUDIO_CONT 22    //0xFF26

#define WAVE_PATTERN 32  //0xFF30
#define WAVE_PATTERN_SIZE 16



#define OAM  0xFE00
#define OAM_SIZE 0xA0
