/*
 * LCD driver for the ILI9488 TFT LCD chips from gd32f303
 *
 * Feel free to use, change, distribute this code as desired. Use under
 * GPLv3 open-source license.
 *
 * File:   ILI9488.c
 * Author: Stran4k
 *
 * Created on 07.07.25
 */


#include "ILI9488.h"
#include "font.h"

#define HEIGHT_SIMBOL TAHOMA_HEIGHT
#define WIDTH_SIMBOL  (TAHOMA_WIDTH)

#define BUFFER_SIZE  (3*HEIGHT_SIMBOL*WIDTH_SIMBOL) //1350//=1 simvol ascii(this font FONT_Tahoma26) 3*H*W

volatile static uint8_t buffer[BUFFER_SIZE];

volatile static uint16_t buffer_counter = 0;


//static unsigned int WIDTH  = 480; 
//static unsigned int HEIGHT = 320;

#if (SPI_DMA_MODE == 1)
	
 volatile static _Bool dma_free = 1;
 
	
  void dma0_spi1_reconfig(uint8_t* data, uint16_t len)
	{
		/* SPI1 transmit dma config: DMA0,DMA_CH4  */
    dma_channel_disable(DMA0, DMA_CH4);
    // Dropping the flags DMA 
    dma_flag_clear(DMA0, DMA_CH4, DMA_FLAG_G);
    // Reloading the DMA settings
    dma_memory_address_config(DMA0, DMA_CH4, (uint32_t)data);
    dma_transfer_number_config(DMA0, DMA_CH4, len);
    //Starting DMA again
    dma_interrupt_flag_clear(DMA0, DMA_CH4, DMA_INT_FLAG_FTF);
    dma_channel_enable(DMA0, DMA_CH4);
	} 
  
	void dma0_spi1_config(void)
	{
		/* SPI1 transmit dma config: DMA0,DMA_CH4  */
    dma_parameter_struct dma0_spi1_init_struct;
    
		dma_deinit(DMA0, DMA_CH4);
		dma0_spi1_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI1);
		dma0_spi1_init_struct.memory_addr  = (uint32_t)buffer;
		dma0_spi1_init_struct.direction	   = DMA_MEMORY_TO_PERIPHERAL;
		dma0_spi1_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
		dma0_spi1_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
		dma0_spi1_init_struct.priority	   = DMA_PRIORITY_LOW;
		dma0_spi1_init_struct.number       = BUFFER_SIZE;
		dma0_spi1_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
		dma0_spi1_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
		dma_init(DMA0, DMA_CH4, &dma0_spi1_init_struct);
		/* configure DMA mode */
		dma_circulation_disable(DMA0, DMA_CH4);
		dma_memory_to_memory_disable(DMA0, DMA_CH4);
		dma_interrupt_enable(DMA0, DMA_CH4, DMA_INT_FTF);
    nvic_irq_enable(DMA0_Channel4_IRQn,    1, 5);  
	}

	void  DMA0_Channel4_IRQHandler(void)
	{
		if (dma_interrupt_flag_get(DMA0, DMA_CH4, DMA_INT_FLAG_FTF)) {
			if (!spi_i2s_flag_get(SPI1, SPI_FLAG_TRANS)) {
				dma_interrupt_flag_clear(DMA0, DMA_CH4, DMA_INT_FLAG_FTF);
				dma_free = 1;
				buffer_counter = 0;
			}
		}
	}

#endif


void delay_ms(double millis);
void delay_us(long int mickro);


void SPI1_config(void)  // spi2 dma1 ch1
{
		spi_parameter_struct spi1_init = {0};

		spi_i2s_deinit(SPI1);

	/* SPI0 parameter config */
		spi1_init.trans_mode		  = SPI_TRANSMODE_FULLDUPLEX;
		spi1_init.device_mode		  = SPI_MASTER;
		spi1_init.frame_size		  = SPI_FRAMESIZE_8BIT;
		spi1_init.clock_polarity_phase	  = SPI_CK_PL_LOW_PH_1EDGE;
		spi1_init.nss			  = SPI_NSS_SOFT;
		spi1_init.prescale		  = SPI_PSC_2;					
		spi1_init.endian		  = SPI_ENDIAN_MSB;
		spi_init(SPI1, &spi1_init);
  
#if (SPI_DMA_MODE == 1)
		spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);
#endif
		spi_enable(SPI1); 
} 
/*
 * Writes a byte to SPI without changing chip select (CS) state.
 * Called by the write_command() and write_data() functions which
 * control these pins as required.
 */
void spi_write(uint8_t data) {
	 while (!(SPI_STAT(SPI1) & SPI_FLAG_TBE)){}
    SPI_DATA(SPI1) = data;
//while (!(SPI_STAT(SPIqq1) & SPI_FLAG_RBNE));
  for(volatile int j=0; j<2; j++);
}

/*
 * Writes the V-RAM buffer to the display.
 * TODO: currently only uses SPI.
 */

#if (SPI_DMA_MODE == 1)
void write_buffer (uint8_t * data, uint16_t len)
{  
	while (!dma_free);
	dma_free = 0;
	dma0_spi1_reconfig(data, len);
	spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);
	dma_channel_enable(DMA0, DMA_CH4);
#else
void write_buffer(void)
{
  for (int i = 0; i < buffer_counter; i++){
		while (!(SPI_STAT(SPI1) & SPI_FLAG_TBE));
		spi_i2s_data_transmit(SPI1, buffer[i]);
		for(volatile int j=0; j<2; j++);
	}
	buffer_counter = 0;
#endif
}


/*
 * Writes a data byte to the display. Pulls CS low as required.
 */
void lcd_write_data(uint8_t data) {

    LCD_DC_1;
    LCD_CS_0;

    	spi_write(data);
LCD_CS_1;
}

/*
 * Writes a command byte to the display
 */
void lcd_write_command(uint8_t data) {
    LCD_DC_0;
    LCD_CS_0;

    	spi_write(data);
LCD_CS_1;
}

void lcd_write_reg(uint8_t data) {
    LCD_DC_1;
    LCD_CS_0;

    	spi_write(data);
LCD_CS_1;
}

/*
 * Swaps two 16-bit integers
 */
void swap_int(unsigned int *num1, unsigned int *num2) {
    unsigned int temp = *num2;
    *num2 = *num1;
    *num1 = temp;
}

/*
 * Swaps two 8-bit integers
 */
void swap_char(char *num1, char *num2) {
    char temp = *num2;
    *num2 = *num1;
    *num1 = temp;
}

/*
 * Delay calcualted on 32MHz clock.
 * Does NOT adjust to clock setting
 */
void delay_ms(double millis) {
    int multiplier = 4;
    double counter = millis;
    while(multiplier--) {
        while(counter--);
        counter = millis;
    }
}

/*
 * A short microsecond delay routine
 * (not measured)
 */
void delay_us(long int micro) {
    while(micro--);
}


/*
 * Same as above, but initialises with an SPI port instead.
 */
void lcd_init_spi() {
    //SET control pins for the LCD HIGH (they are active LOW)
    LCD_RST_1;
    LCD_CS_1; //Chip Select Active LOW
    LCD_DC_1; //Data / Command select Active LOW
    //Cycle reset pin
    for(volatile int j=0; j<20; j++);
    LCD_RST_0;
    for(volatile int j=0; j<20; j++);
    LCD_RST_1;
    for(volatile int j=0; j<20; j++);
    //LCD_RST_0;
    lcd_init_command_list();

}

/**
 * This is the magic initialisation routine.
 */
void lcd_init_command_list(void)
{

	//********Start Initial Sequence*******//
	lcd_write_command(0xE0); //P-Gamma
	lcd_write_data(0x00);
	lcd_write_data(0x13);
	lcd_write_data(0x18);
	lcd_write_data(0x04);
	lcd_write_data(0x0F);
	lcd_write_data(0x06);
	lcd_write_data(0x3A);
	lcd_write_data(0x56);
	lcd_write_data(0x4D);
	lcd_write_data(0x03);
	lcd_write_data(0x0A);
	lcd_write_data(0x06);
	lcd_write_data(0x30);
	lcd_write_data(0x3E);
	lcd_write_data(0x0F);
	lcd_write_command(0XE1); //N-Gamma
	lcd_write_data(0x00);
	lcd_write_data(0x13);
	lcd_write_data(0x18);
	lcd_write_data(0x01);
	lcd_write_data(0x11);
	lcd_write_data(0x06);
	lcd_write_data(0x38);
	lcd_write_data(0x34);
	lcd_write_data(0x4D);
	lcd_write_data(0x06);
	lcd_write_data(0x0D);
	lcd_write_data(0x0B);
	lcd_write_data(0x31);
	lcd_write_data(0x37);
	lcd_write_data(0x0F);
	lcd_write_command(0xC0);
	lcd_write_data(0x18);
	lcd_write_data(0x16);
	lcd_write_command(0xC1);
	lcd_write_data(0x45);
	lcd_write_command(0xC5); //VCOM
	lcd_write_data(0x00);
	lcd_write_data(0x63);
	lcd_write_data(0x01);

	lcd_write_command(0x36); //RAM address mode
	//0xF8 and 0x3C are landscape mode. 0x5C and 0x9C for portrait mode.
	if(LANDSCAPE)
		lcd_write_data(0xF8);
	else
		lcd_write_data(0x5C);

	lcd_write_command(0x3A); //Interface Mode Control
	lcd_write_data(0x66); //16-bit serial mode
	lcd_write_command(0xB0); //Interface Mode Control
	lcd_write_data(0x80); //SDO not in use
	lcd_write_command(0xB1); //Frame rate 70HZ
	lcd_write_data(0x00); //
	lcd_write_data(0x10);
	lcd_write_command(0xB4);
	lcd_write_data(0x02);

	lcd_write_command(0xB6); //RGB/MCU Interface Control
	lcd_write_data(0x02);
	//lcd_write_data(0x22);

	lcd_write_command(0xE9);
	lcd_write_data(0x00);
	lcd_write_command(0xF7);
	lcd_write_data(0xA9);
	lcd_write_data(0x51);
	lcd_write_data(0x2C);
	lcd_write_data(0x82);
	lcd_write_command(0x11);
	delay_ms(120);
	lcd_write_command(0x20);


	delay_ms(120);
	lcd_write_command(0x29);
}

/*
 * Sets the X,Y position for following commands on the display.
 * Should only be called within a function that draws something
 * to the display.
 */
void set_draw_window(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {

    //Check that the values are in order
    if(x2 < x1)
        swap_int(&x2, &x1);
    if(y2 < y1)
        swap_int(&y2, &y1);

    lcd_write_command(ILI9488_CASET);
    lcd_write_data(x1 >> 8);
    lcd_write_data(x1 & 0xFF);

    lcd_write_data(x2 >> 8);
    lcd_write_data(x2 & 0xFF);

    lcd_write_command(ILI9488_PASET);
    lcd_write_data(y1 >> 8);
    lcd_write_data(y1 & 0xFF);

    lcd_write_data(y2 >> 8);
    lcd_write_data(y2 & 0xFF);

    lcd_write_command(ILI9488_RAMWR);

	//LCD_CS_0;
	//LCD_DC_1;
}

void draw_pixel(int16_t x, int16_t y, uint16_t colour) {


if ((x < 0) || (x >= WIDTH) || (y < 0) || (y >= HEIGHT))
		return;
    //All my colours are in 16-bit RGB 5-6-5 so they have to be converted to 18-bit RGB
    set_draw_window(x, y, x + 1, y + 1);
    //LCD_DC_1;
   // LCD_CS_0;
	write16BitColor(colour);
	//LCD_CS_1;
}




void write16BitColor(uint16_t color)
{

	unsigned char r = (color >> 8) & 0xF8;
    unsigned char g = (color >> 3) & 0xFC;
    unsigned char b = (color << 3);


	  spi_write(r);
	  spi_write(g);
	  spi_write(b);
}










/*
 * Fills a rectangle with a given colour
 */
void fill_rectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int colour) {
    //All my colours are in 16-bit RGB 5-6-5 so they have to be converted to 18-bit RGB
    unsigned char r = (colour >> 8) & 0xF8;
    unsigned char g = (colour >> 3) & 0xFC;
    unsigned char b = (unsigned char)(colour << 3);


    //Set the drawing region
    set_draw_window(x1, y1, x2, y2);

    //We will do the SPI write manually here for speed
    //( the data sheet says it doesn't matter if CS changes between
    // data sections but I don't trust it.)
    //CS low to begin data
    LCD_DC_1;
    LCD_CS_0;


    //Write colour to each pixel
    for(volatile int y = 0; y < y2-y1+1 ; y++) {
        for(volatile int x = 0; x < x2-x1+1; x++) {
          #if (SPI_DMA_MODE == 1)
            while (!dma_free){}
          #endif

            
          buffer[buffer_counter] = r;
          buffer_counter++;
          buffer[buffer_counter] = g;
          buffer_counter++;
          buffer[buffer_counter] = b;
          buffer_counter++;

			//If the buffer is full then send it to the dispaly
			if(buffer_counter > (BUFFER_SIZE - 3)) {
          #if (SPI_DMA_MODE == 1)
            write_buffer(&buffer,buffer_counter);
          #else
            write_buffer();    
          #endif
			}
        }
    }
    //Send the remaining bytes
#if (SPI_DMA_MODE == 1)
    write_buffer(&buffer,buffer_counter);
    while (!dma_free) {}
#else
    write_buffer();    
#endif
    LCD_CS_1;
}


void draw_fast_string_s ( unsigned int x, unsigned int y, unsigned int colour, unsigned int bg_colour, char *str ) {
    //Work out the size of each character
    //int char_width =  18;
    //Iterate through each character in the string
  #if (SPI_DMA_MODE == 1)
      while (!dma_free){}
  #endif
    int counter = 0;
    while(str[counter] != '\0') {
        //Calculate character position
        int char_pos = x + (counter * WIDTH_SIMBOL);
        //Write char to the display
        draw_fast_char_s(char_pos, y, colour, bg_colour, str[counter]);
        //Next character
        counter++;
    }
}
void draw_fast_char_s   ( unsigned int x, unsigned int y, unsigned int colour, unsigned int bg_colour, char c ) {
    uint64_t line;
    //char width = 18;   // 8
    //char height = 25; // 13
    unsigned int font_index = (c - 0);
    unsigned int this_px = bg_colour;
    //If the buffer is too small to fit a full character then we have to write each pixel
    int smallBuffer = 0;
    if(BUFFER_SIZE < HEIGHT_SIMBOL * WIDTH_SIMBOL) {
    	smallBuffer++;
    }

    //Set the drawing region
    set_draw_window(x, y, x + WIDTH_SIMBOL - 1, y + HEIGHT_SIMBOL);

    //We will do the SPI write manually here for speed
    //CS low to begin data
    LCD_DC_1;
    LCD_CS_0;

    //Get the line of pixels from the font file

    for(volatile int i= HEIGHT_SIMBOL -1; i >=0 ; i-- ) {

        line = FONT_Tahoma26 [font_index][HEIGHT_SIMBOL -1 - i];
        //Draw the pixels to screen

        for(volatile int j = 0; j < WIDTH_SIMBOL; j++) {
            //Default pixel colour is the background colour, unless changed below
            this_px = bg_colour;
			if((line >> (j)) & 0x01)
          this_px = colour;

		    buffer[buffer_counter++] = (this_px >> 8) & 0xF8;
		    buffer[buffer_counter++] = (this_px >> 3) & 0xFC;
		    buffer[buffer_counter++] = (this_px << 3);      
        if (smallBuffer) {
          #if (SPI_DMA_MODE == 1)
            write_buffer(&buffer,buffer_counter);
          #else
            write_buffer();    
          #endif
        }
      }
    }
    //Write the vram buffer to the display
	if (!smallBuffer) {	
#if (SPI_DMA_MODE == 1)
  write_buffer(&buffer,buffer_counter);
	while (!dma_free) {}
#else
  write_buffer();    
#endif
    }
    LCD_CS_1;
}



void draw_bitmap(unsigned int x1, unsigned int y1, int scale, const unsigned int *bmp) {
	int width = bmp[0];
	int height = bmp[1];
	unsigned int this_byte;
	int x2 = x1 + (width * scale);
	int y2 = y1 + (height * scale);

	//Set the drawing region
	set_draw_window(x1, y1, x2 + scale - 1, y2);

	//We will do the SPI write manually here for speed
    LCD_DC_1;
	//CS low to begin data
    LCD_CS_0;

	//Write colour to each pixel
	for (int i = 0; i < height; i++) {
		//this loop does the vertical axis scaling (two of each line))
		for (int sv = 0; sv < scale; sv++) {
			for (int j = 0; j <= width; j++) {
				//Choose which byte to display depending on the screen orientation
				//NOTE: We add 2 bytes because of the first two bytes being dimension data in the array
				this_byte = bmp[(width * (i)) + j + 2];

				//And this loop does the horizontal axis scale (three bytes per pixel))
				for (int sh = 0; sh < scale; sh++) {
            #if (SPI_DMA_MODE == 1)
              while (!dma_free){}
            #endif
				    unsigned char r = (this_byte >> 8) & 0xF8;
				    unsigned char g = (this_byte >> 3) & 0xFC;
				    unsigned char b = (this_byte << 3);
				    buffer[buffer_counter] = r;
				    buffer_counter++;
				    buffer[buffer_counter] = g;
				    buffer_counter++;
				    buffer[buffer_counter] = b;
				    buffer_counter++;

					//If the buffer is full then send it to the dispaly
					if(buffer_counter > BUFFER_SIZE - 3) {
						#if (SPI_DMA_MODE == 1)
                write_buffer(&buffer,buffer_counter);
            #else
              write_buffer();    
            #endif
					}
				}
			}
		}
	}
	//Send the remaining bytes
	
#if (SPI_DMA_MODE == 1)
  write_buffer(&buffer,buffer_counter);
	while (!dma_free) {}
#else
  write_buffer();    
#endif
    LCD_CS_1;

}

void setRotation(uint8_t r)
{

	lcd_write_command(ILI9488_MADCTL);
	uint8_t rotation = r % 4; // can't be higher than 3
	switch (rotation) {
	case 0:
		lcd_write_data(MADCTL_MX | MADCTL_BGR);
		WIDTH = WIDTH;
		HEIGHT = HEIGHT;
		break;
	case 1:
		lcd_write_data(MADCTL_MV | MADCTL_BGR);
		WIDTH = HEIGHT;
		HEIGHT = WIDTH;
		break;
	case 2:
		lcd_write_data(MADCTL_MY | MADCTL_BGR);
		WIDTH = WIDTH;
		HEIGHT = HEIGHT;
		break;
	case 3:
		lcd_write_data(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
		WIDTH = HEIGHT;
		HEIGHT = WIDTH;
		break;
	}

}



