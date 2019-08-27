#define IF_REGISTER 0xFF0F

#define VBLANK_INT (1 << 0)
#define LCD_STAT_INT (1 << 1)
#define TIMER_OV_INT (1 << 2)
#define SERIAL_INT (1 << 3)
#define BUTTON_INT (1 << 4)


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


#define OAM  0xFE00
#define OAM_SIZE 0xA0
