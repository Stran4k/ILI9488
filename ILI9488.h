
#ifndef ILI9488_H
#define	ILI9488_H

//#include "font.h"
/*
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI1);
    rcu_periph_clock_enable(RCU_DMA0); 
    
#define SPI1_PORT						      GPIOB
#define SPI1_PIN_SCK					    GPIO_PIN_13
#define SPI1_PIN_MISO					    GPIO_PIN_14
#define SPI1_PIN_MOSI					    GPIO_PIN_15
#define SPI1_PIN_CS						    GPIO_PIN_12 //NSS
#define TFT_PORT						      GPIOB
#define TFT_PIN_RST						    GPIO_PIN_11
#define TFT_PIN_DC						    GPIO_PIN_10

  gpio_init( TFT_PORT, GPIO_MODE_OUT_PP,        GPIO_OSPEED_50MHZ, TFT_PIN_DC  ); // Переключение команда/данные | A0/DC
  gpio_init( TFT_PORT, GPIO_MODE_OUT_PP,        GPIO_OSPEED_50MHZ, TFT_PIN_RST ); // Сброс экрана | RST  
  
  gpio_init( SPI1_PORT, GPIO_MODE_OUT_PP,       GPIO_OSPEED_50MHZ, SPI1_PIN_CS);  // Выбор устройства CS
  gpio_init( SPI1_PORT, GPIO_MODE_AF_PP,        GPIO_OSPEED_50MHZ, SPI1_PIN_SCK); // SCLK
  gpio_init( SPI1_PORT, GPIO_MODE_IN_FLOATING,  GPIO_OSPEED_50MHZ, SPI1_PIN_MISO);// Прием данных MISO
  gpio_init( SPI1_PORT, GPIO_MODE_AF_PP,        GPIO_OSPEED_50MHZ, SPI1_PIN_MOSI);// Отправка данных MOSI
*/
#include "main.h"
#define SPI_DMA_MODE 0  // 1 or 0-dma  // | need correct  long symbol in font

static unsigned int WIDTH  = 480; 
static unsigned int HEIGHT = 320;

#define WAITING_END_DATA_TRANSMISSION  while ( !(SPI_STAT(SPI1) & SPI_FLAG_TBE) && (SPI_STAT(SPI1) & SPI_FLAG_RBNE) ) {}// waiting end transmit data on lcd


  
#define LANDSCAPE   1 //Portrait or Landscape orientation. Update WIDTH and HEIGHT above.
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04





//ILI9488 registers found at
//https://github.com/jaretburkett/ILI9488/blob/master/ILI9488.cpp
//And Thanks Tommy
#define ILI9488_NOP		  	0x00
#define ILI9488_SWRESET		0x01
#define ILI9488_RDDID	  	0x04
#define ILI9488_RDDST	  	0x09

#define ILI9488_SLPIN		  0x10
#define ILI9488_SLPOUT		0x11
#define ILI9488_PTLON		  0x12
#define ILI9488_NORON		  0x13

#define ILI9488_RDMODE		0x0A
#define ILI9488_RDMADCTL	0x0B
#define ILI9488_RDPIXFMT	0x0C
#define ILI9488_RDIMGFMT	0x0D
#define ILI9488_RDSELFDIAG  0x0F

#define ILI9488_INVOFF		0x20
#define ILI9488_INVON		  0x21
#define ILI9488_GAMMASET	0x26
#define ILI9488_DISPOFF		0x28
#define ILI9488_DISPON		0x29

#define ILI9488_CASET		  0x2A
#define ILI9488_PASET		  0x2B
#define ILI9488_RAMWR		  0x2C
#define ILI9488_RAMRD		  0x2E

#define ILI9488_PTLAR		  0x30
#define ILI9488_MADCTL		0x36
#define ILI9488_PIXFMT		0x3A

#define ILI9488_FRMCTR1		0xB1
#define ILI9488_FRMCTR2		0xB2
#define ILI9488_FRMCTR3		0xB3
#define ILI9488_INVCTR		0xB4
#define ILI9488_DFUNCTR		0xB6

#define ILI9488_PWCTR1		0xC0
#define ILI9488_PWCTR2		0xC1
#define ILI9488_PWCTR3		0xC2
#define ILI9488_PWCTR4		0xC3
#define ILI9488_PWCTR5		0xC4
#define ILI9488_VMCTR1		0xC5
#define ILI9488_VMCTR2		0xC7

#define ILI9488_RDID1		  0xDA
#define ILI9488_RDID2		  0xDB
#define ILI9488_RDID3		  0xDC
#define ILI9488_RDID4	  	0xDD

#define ILI9488_GMCTRP1		0xE0
#define ILI9488_GMCTRN1		0xE1

/* RGB 16-bit color table definition (RG565) */
#define COLOR_BLACK         0x0000      /*   0,   0,   0 */
#define COLOR_WHITE         0xFFFF      /* 255, 255, 255 */
#define COLOR_BLUE          0x001F      /*   0,   0, 255 */
#define COLOR_GREEN         0x07E0      /*   0, 255,   0 */
#define COLOR_RED           0xF800      /* 255,   0,   0 */
#define COLOR_NAVY          0x000F      /*   0,   0, 128 */
#define COLOR_DARKBLUE      0x0011      /*   0,   0, 139 */
#define COLOR_DARKGREEN     0x03E0      /*   0, 128,   0 */
#define COLOR_DARKCYAN      0x03EF      /*   0, 128, 128 */
#define COLOR_CYAN          0x07FF      /*   0, 255, 255 */
#define COLOR_TURQUOISE     0x471A      /*  64, 224, 208 */
#define COLOR_INDIGO        0x4810      /*  75,   0, 130 */
#define COLOR_DARKRED       0x8000      /* 128,   0,   0 */
#define COLOR_OLIVE         0x7BE0      /* 128, 128,   0 */
#define COLOR_GRAY          0x8410      /* 128, 128, 128 */
#define COLOR_GREY          0x8410      /* 128, 128, 128 */
#define COLOR_SKYBLUE       0x867D      /* 135, 206, 235 */
#define COLOR_BLUEVIOLET    0x895C      /* 138,  43, 226 */
#define COLOR_LIGHTGREEN    0x9772      /* 144, 238, 144 */
#define COLOR_DARKVIOLET    0x901A      /* 148,   0, 211 */
#define COLOR_YELLOWGREEN   0x9E66      /* 154, 205,  50 */
#define COLOR_BROWN         0xA145      /* 165,  42,  42 */
#define COLOR_DARKGRAY      0x7BEF      /* 128, 128, 128 */
#define COLOR_DARKGREY      0x7BEF      /* 128, 128, 128 */
#define COLOR_SIENNA        0xA285      /* 160,  82,  45 */
#define COLOR_LIGHTBLUE     0xAEDC      /* 172, 216, 230 */
#define COLOR_GREENYELLOW   0xAFE5      /* 173, 255,  47 */
#define COLOR_SILVER        0xC618      /* 192, 192, 192 */
#define COLOR_LIGHTGRAY     0xC618      /* 192, 192, 192 */
#define COLOR_LIGHTGREY     0xC618      /* 192, 192, 192 */
#define COLOR_LIGHTCYAN     0xE7FF      /* 224, 255, 255 */
#define COLOR_VIOLET        0xEC1D      /* 238, 130, 238 */
#define COLOR_AZUR          0xF7FF      /* 240, 255, 255 */
#define COLOR_BEIGE         0xF7BB      /* 245, 245, 220 */
#define COLOR_MAGENTA       0xF81F      /* 255,   0, 255 */
#define COLOR_TOMATO        0xFB08      /* 255,  99,  71 */
#define COLOR_GOLD          0xFEA0      /* 255, 215,   0 */
#define COLOR_ORANGE        0xFD20      /* 255, 165,   0 */
#define COLOR_SNOW          0xFFDF      /* 255, 250, 250 */
#define COLOR_YELLOW        0xFFE0      /* 255, 255,   0 */




#define LCD_CS_0        gpio_bit_reset	(SPI1_PORT, SPI1_PIN_CS)
#define LCD_CS_1        gpio_bit_set	  (SPI1_PORT, SPI1_PIN_CS)

#define LCD_RST_0       gpio_bit_reset	(TFT_PORT,  TFT_PIN_RST)
#define LCD_RST_1       gpio_bit_set	  (TFT_PORT,  TFT_PIN_RST)

#define LCD_DC_0        gpio_bit_reset	(TFT_PORT,  TFT_PIN_DC)
#define LCD_DC_1        gpio_bit_set   	(TFT_PORT,  TFT_PIN_DC)

#if (SPI_DMA_MODE==1)
 void dma0_spi1_config         (void);
 void dma0_spi1_reconfig       (uint8_t * data, uint16_t len); 
 void DMA0_Channel4_IRQHandler (void);
 void write_buffer             (uint8_t * data, uint16_t len);
#else

 void write_buffer(void);

#endif

void SPI1_config(void) ;

void write16BitColor(uint16_t color);
void spi_write(uint8_t data);
void lcd_write_command(uint8_t data);
void lcd_write_data(uint8_t data);
void lcd_write_reg(uint8_t data);
void lcd_init_spi(void);
void swap_int(unsigned int *num1, unsigned int *num2);
void swap_char(char *num1, char *num2);
void lcd_init_command_list(void);
void draw_pixel(int16_t x, int16_t y, uint16_t colour);
void set_draw_window(unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end);
void fill_rectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int colour);
void draw_bitmap(unsigned int x, unsigned int y, int scale, const unsigned int *bmp);
void setRotation(uint8_t r);

void draw_fast_string_s ( unsigned int x, unsigned int y, unsigned int colour, unsigned int bg_colour, char *str ) ;
void draw_fast_char_s   ( unsigned int x, unsigned int y, unsigned int colour, unsigned int bg_colour, char c ) ;

#endif	/* ILI9488_H */

