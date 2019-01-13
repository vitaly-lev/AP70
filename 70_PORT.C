/*
	Vitaly Lev	Fido Net  2:462/10
				Internet  vl@ccrd.lviv.ua
	<C> <Lion>	

	70_port.c

	Programm for Communication ES <-> IBM

	Subroutines for I/O port

	930608	V1.0

*/
#include <string.h>
#include <memory.h>
#include <process.h>
#include <dos.h>
#include <conio.h>
#include <bios.h>
#include <stdio.h>
#include "70_def.h"

struct fos_inform {
       word      strsize;       /* size of the structure in bytes     */
       byte      majver;        /* FOSSIL spec driver conforms to     */
       byte      minver;        /* rev level of this specific driver  */
       char far *ident;         /* FAR pointer to ASCII ID string     */
       word      ibufr;         /* size of the input buffer (bytes)   */
       word      ifree;         /* number of bytes left in buffer     */
       word      obufr;         /* size of the output buffer (bytes)  */
       word      ofree;         /* number of bytes left in the buffer */
       byte      swidth;        /* width of screen on this adapter    */
       byte      sheight;       /* height of screen    "      "       */
       byte      speed;         /* ACTUAL speed, computer to modem    */
};

/*
	Local Defines
*/
#define I_8259A	0x20
#define I_MASK	0x21
#define EOI		0x20
#define RCV_DATA	4
#define THR_EMPTY	2
#define MDST_CHNG	0
#define TX_ONE  	0
#define TX_BLOCK	1
#define RX_BLOCK	0
#define PORT		0x03f8
#define LSR		0x03fd
#define TR		0x0020
#define DR		0x0001
#define RX_BLOCK	0

#define S_600	0x03<<5
#define S_1200	0x04<<5
#define S_2400	0x05<<5
#define S_4800	0x06<<5
#define S_9600	0x07<<5

#define FOS_SIGNATURE   0x1954
#define FOS_SETSPEED    0x00
#define FOS_PUTC        0x01
#define FOS_GETSTATUS   0x03
#define FOS_INIT        0x04
#define FOS_DEINIT      0x05
#define FOS_SETDTR      0x06
#define FOS_DUMP        0x09
#define FOS_PURGE       0x0a
#define FOS_SETFLOW     0x0f
/*
	Local functions
*/
int	set_port(int);
void	send_b(byte, int);
int 	receive_b(int, int);
void	send_block(int, int);
void	j_exit(void);
void	snd_block(int);
void	rcv_block(int);
void	modem_status(int);
int	drvr_ins(int);
void	drvr_rem(int);
void 	_interrupt _far ascode(void);
int	fos_init(int);
int	get_fos_info(int);
int	fos_fillinbuf(int);
void	com_purge(int);
void	rx_block_progress(int);
/*
	Global variable
*/
uint	port_addr[] = { 0x03f8, 0x02f8, 0x03e8, 0x2e8 };
byte	hard_int[]  = { 0x0c,   0x0b,   0x0c,   0x0b  };
byte	iorq_int[]  = { 0xef,   0xf7,   0xef,   0xf7  };
byte	linestat;
int	fossil;
union	REGS regs;
struct	SREGS sregs;
struct 	fos_inform fos_info;
/*
	External Functions
*/
extern	void	timerins(void);
extern	void	timerrem(void);
extern	void	trigger_tx(int);
extern	void	timerrem(void);
extern	void	modem_st(void);
extern	void screen_char_in(byte);
extern	void screen_char_out(byte);
extern	int 	log_fprintf(char *, ...);
extern	int	num_error;
/*
	External variable
*/
extern	int	num_c;		/* global address channels */
extern 	int 	iir_add;		/* int id reg address */
extern	FILE *log_file;
extern	struct bl channel_s[];
extern	int flag_debug_i_o;
extern	int flag_computer;


/*--------------------------*
 *	F U N C T I O N S   	*
 *--------------------------*/

int set_port(int number)
{
int baudrate;
char tmps[80];
int tmpi;

	switch (channel_s[number].ch.speed)
		{
		case 600 : baudrate = S_600;	break;
		case 1200: baudrate = S_1200; break;
		case 2400: baudrate = S_2400; break;
		case 4800: baudrate = S_4800; break;
		case 9600: baudrate = S_9600; break;
		}

	switch (channel_s[number].ch.bits)
		{
		case 7: baudrate |= (byte)0x02; break;
		case 8: baudrate |= (byte)0x03; break;
		}

	switch (channel_s[number].ch.stop)
		{
		case 1: break;
		case 2: baudrate |= 0x04; break;
		}

	switch (channel_s[number].ch.parity)
		{
		case 'n': break;
		case 'o': baudrate |= 0x08; break;
		case 'e': baudrate |= 0x18; break;
		}

	channel_s[number].ch.baudrate = baudrate;
	channel_s[number].ch.port_add = port_addr[channel_s[number].ch.comn];

	
	fossil = TRUE;

	if (!channel_s[number].ch.ch_active && !fos_init(number))
		{
   	fossil = FALSE;
   	return(drvr_ins(number));
		}
	else
		{
		tmpi = get_fos_info(number);

		_fmemcpy(tmps, fos_info.ident, _fstrlen(fos_info.ident));
		log_fprintf("%s %s", FOSSIL_DRV_FOUND, tmps);

		if (!tmpi || fos_info.majver < 5)
			{
   		if (!channel_s[number].ch.ch_active)
				{
				regs.h.ah = FOS_DEINIT;
				int86(0x14, &regs, &regs);
				}
			log_fprintf("%s", FOSSIL_WRONG_VERSION);
			j_exit();
			return(FALSE);
			}
		else	 
			{
/* set baudrate in fossil */

			regs.h.ah = 0x00;
			regs.h.al = (byte)baudrate;
			regs.x.dx = channel_s[number].ch.comn;
			int86(0x14, &regs, &regs);
			channel_s[number].ch.ch_active = 1;
			}

		}
}

void send_b(byte tx_byte, int number)
{
	if (fossil)
		{
		regs.h.ah = FOS_PUTC;
		regs.h.al = tx_byte;
		regs.x.dx = channel_s[number].ch.comn;
		int86(0x14, &regs, &regs);
/*!!!*/	screen_char_out(tx_byte);
		}
	else
		{
		channel_s[number].temp_tx[0] = tx_byte;
		channel_s[number].tx_number = 1;
		while(channel_s[number].modem_out_busy)
			{
				;
			}
		trigger_tx(number);
		}
}


void send_block(int bnum, int number)
{
int i;

	if (fossil)
		{
		regs.x.ax = 0x1900;
		regs.x.dx = channel_s[number].ch.comn;
		regs.x.cx = bnum;
		segread(&sregs);
		sregs.es  = sregs.ds;
		regs.x.di = (uint)&channel_s[number].temp_tx[0];
		int86x(0x14,&regs,&regs,&sregs);
		channel_s[number].block_tx = TRUE;
		channel_s[number].tx_count = bnum;
		for (i=0; i<bnum; i++)
/*!!!*/		screen_char_out(channel_s[number].temp_tx[i]);

		}
	else
		{
		channel_s[number].tx_number = bnum;
		while(channel_s[number].modem_out_busy)
			{
				;
			}
		trigger_tx(number);
		}
}

int receive_b(int sec, int number)
{
	channel_s[number].rx_count = 0;
	*(channel_s[number].timer) = sec;
	while ((channel_s[number].timer != 0) && (channel_s[number].rx_count == 0))
		{
		if (fossil)
			{
           	if ((channel_s[number].fos_inread < channel_s[number].fos_inwrite) || fos_fillinbuf(number))
				{
				channel_s[number].rx_count++;
/*!!!*/			screen_char_in(channel_s[number].temp_rx[channel_s[number].rx_count]);
				}
			}
		}

	if (channel_s[number].rx_count == 0)
		return(-1);
	else
	{
		com_purge(number);
		return(channel_s[number].temp_rx[0]);
	}
}

/*
void receive_block(int number)
{
	channel_s[number].rx_state = RX_BLOCK;
	channel_s[number].rx_count = 0;
	*(channel_s[number].timer) = 10;
	channel_s[number].sum = 0;

	if (fossil)
		{
        regs.x.ax = 0x1800;
        regs.x.dx = channel_s[number].ch.comn;
        regs.x.cx = RX_BLOCK_LENGTH;
        segread(&sregs);
        sregs.es  = sregs.ds
        regs.x.di = &channel_s[number].temp_rx[0];
        int86x(0x14,&regs,&regs,&sregs);
		}
}
*/

void modem_status(int number)
{
	if (fossil)
		{
		regs.h.ah = FOS_GETSTATUS;
		regs.x.dx = channel_s[number].ch.comn;
		int86(0x14, &regs, &regs);
		channel_s[number].modem_state = regs.x.ax;
		}
	else
		channel_s[number].modem_state = inp(channel_s[number].ch.port_add+6);
}

void j_exit(void)
{
int i;
	fflush(log_file);
	fclose(log_file);
	timerrem();

	for (i=0; i<num_c; i++)
		drvr_rem(i);
}


void _interrupt _far ascode(void)
{
int	int_id_reg;			/* value in int id reg */
int	ch;


/* Handle interrupts for as long as there are any. 
*/

for (;;)
	{

/*	
	if (port1 == 1)
		{
		if (channel_s[0].ch.port_add == 0x3f8)
			ch=0;
		else
			ch=1;
		}
	else
		{
		if (channel_s[1].ch.port_add == 0x2f8)
			ch=1;
		else
			ch=0;
		}
*/
	ch=0;

	int_id_reg = inp(channel_s[ch].ch.port_add+2);

	if ((int_id_reg & BIT0_ON) == 0x01)
		{
			outp(I_8259A, EOI);
			return;
		}

	switch (int_id_reg)
		{
		case RCV_DATA:				/* Receive buffer full interrupt */
					rcv_block(ch);
					break;
		
		case THR_EMPTY:			/* Transmit buffer empty interrupt */
					snd_block(ch);
					break;

		case MDST_CHNG:			/* Modem status change interrupt */
					break;
		}
	}
}

/*
	trigger_sf - initiate send framer
*/
void trigger_tx(int number)
{

	if (!channel_s[number].modem_out_busy)
	{
		channel_s[number].block_tx = FALSE;
		channel_s[number].tx_count = 0;
		channel_s[number].modem_out_busy = 1;
		channel_s[number].tx_state = TX_ONE;
		outp(channel_s[number].ch.port_add, channel_s[number].temp_tx[0]);

/*!!!*/	screen_char_out(channel_s[number].temp_tx[0]);
		
		if (channel_s[number].tx_number == 0x01)
			channel_s[number].st_busy = FALSE;
		else
		{
			channel_s[number].st_busy = TRUE;
			channel_s[number].tx_count++;
		}
	}
}

/*
	Receive Framer
*/
void rcv_block(int number)
{
byte rcv_char;

rcv_char = (char)inp(channel_s[number].ch.port_add);

	switch (channel_s[number].rx_state)
	{
		case RX_BLOCK:
		{
			channel_s[number].temp_rx[channel_s[number].rx_count] = rcv_char;
			channel_s[number].rx_count++;
			channel_s[number].sum += rcv_char;
			*(channel_s[number].timer) = 3;

/*!!!*/		screen_char_in(rcv_char);


			break;
		}
	}

}

/*
	Handle a transmit holding register empty
	interrupt.  This subroutine sends a byte-mode frame.
*/
void snd_block(int number)
{
byte snd_char;

switch (channel_s[number].tx_state)
	{

    	case TX_ONE:
		if (channel_s[number].st_busy)
		{
			snd_char = channel_s[number].temp_tx[channel_s[number].tx_count];
			outp(channel_s[number].ch.port_add, snd_char);
  
/*!!!*/		screen_char_out(snd_char);

			channel_s[number].tx_count++;
			channel_s[number].tx_state++;
			if (channel_s[number].tx_number == channel_s[number].tx_count)
			   channel_s[number].st_busy = FALSE;
			break;
		}
		else
		{
			channel_s[number].modem_out_busy = 0;
			channel_s[number].block_tx = TRUE;
			return;
		}

    	case TX_BLOCK :

		snd_char = channel_s[number].temp_tx[channel_s[number].tx_count];
		outp(channel_s[number].ch.port_add, snd_char);
		
/*!!!*/	screen_char_out(snd_char);

		channel_s[number].tx_count++;
		if (channel_s[number].tx_count == channel_s[number].tx_number)
		{
			channel_s[number].st_busy = FALSE;
			channel_s[number].tx_state = TX_ONE;
		}
		break;

	}
}

int drvr_ins(int number)
{
int mask;
uint _far *fp;
int comn; 	/* COM port number -1 */
int addr;	/* Address for COM port */

	if (channel_s[number].ch.ch_active == 1)
		return(TRUE);

	channel_s[number].modem_out_busy = 0;
	comn = channel_s[number].ch.comn;
	addr = channel_s[number].ch.port_add;


	FP_SEG(fp) = 0;
	FP_OFF(fp) = 0x400+comn*2;
	*fp = addr;

	mask=inp(I_MASK);
	outp(I_MASK, 0xff);
	channel_s[number].ch.old_vector = _dos_getvect(hard_int[comn]);
	_dos_setvect(hard_int[comn], ascode);
 	
	_bios_serialcom(_COM_INIT, comn, channel_s[number].ch.baudrate);

	linestat = (byte)inp(addr+6);
	linestat = (byte)inp(addr+6);

	outp(addr+1, 0x03);	/* enable rcvr and xmtr interrupts */
	inp(addr+2);  		/* clear pending interrupts */
	inp(addr);		/* clear rcvr interrupt by reading rxb */

	outp(addr+4, 0x0b);	/* enable dtr, rts and interrupt (out 2) */
	outp(I_MASK, mask & iorq_int[comn]);

	switch (comn)
		{
		case 0: outp(port_addr[2], 0xff); break;
		case 1: outp(port_addr[3], 0xff); break;
		case 2: outp(port_addr[0], 0xff); break;
		case 3: outp(port_addr[1], 0xff); break;
		}

	if ((inp(addr+2) & 0xf8) != 0)
		{
		log_fprintf("%s (COM%i)", COM_PORT_NOT_FOUND, comn+1);
		channel_s[number].ch.ch_active = -1;
		j_exit();
		return(FALSE);
		}
	else
		{
		channel_s[number].ch.ch_active = 1;
		return(TRUE);
		}
}

void drvr_rem(int number)
{
int mask;


	if (channel_s[number].ch.ch_active != 1)
		return;

	if (fossil)
		{
		regs.h.ah = FOS_DEINIT;
		regs.x.dx = channel_s[number].ch.comn;
		int86(0x14, &regs, &regs);
		channel_s[number].ch.ch_active=0;
		}
	else
		{
		mask=inp(I_MASK);
		outp(I_MASK, 0xff);
		_dos_setvect(hard_int[channel_s[number].ch.comn], channel_s[number].ch.old_vector);
		outp(I_MASK, mask);
		channel_s[number].ch.ch_active=0;
		}
}

int fos_init(int number)
{

	regs.h.ah = FOS_INIT;
	regs.h.al = 0;
	regs.x.dx = channel_s[number].ch.comn;
	int86(0x14, &regs, &regs);
   return ((regs.x.ax != FOS_SIGNATURE || regs.h.bh < 5 || regs.h.bl < 0x1b) ? 0 : 1);
}

int get_fos_info(int number)
{
	regs.x.ax = 0x1b00;
	regs.x.cx = sizeof (struct fos_inform);
	regs.x.dx = channel_s[number].ch.comn;
	segread(&sregs);
	sregs.es  = sregs.ds;
	regs.x.di = (uint)&fos_info;
	int86x(0x14,&regs,&regs,&sregs);

	return (regs.x.ax == sizeof (struct fos_inform));
}

void com_purge(int number)
{
	channel_s[number].rx_state = RX_BLOCK;
	channel_s[number].rx_count = 0;
	channel_s[number].n_rx = 0;
	*(channel_s[number].timer) = 10;
	channel_s[number].sum = 0;

	if (fossil)
		{
		regs.h.ah = FOS_PURGE;
		int86(0x14, &regs, &regs);
		channel_s[number].fos_inread = 0;
		channel_s[number].fos_inwrite = 0;
		}
}

int fos_fillinbuf(int number)
{
uint i;

	regs.x.ax = 0x1800;
	regs.x.dx = channel_s[number].ch.comn;
	regs.x.cx = RX_BLOCK_LENGTH;
	segread(&sregs);
	sregs.es  = sregs.ds;
	regs.x.di = (uint)&channel_s[number].temp_rx[channel_s[number].rx_count];
	int86x(0x14,&regs,&regs,&sregs);
	channel_s[number].fos_inwrite = regs.x.ax;
	channel_s[number].fos_inread = 0;
	channel_s[number].rx_count += regs.x.ax;

	for (i=0; i<regs.x.ax; i++)
/*!!!*/		screen_char_in(channel_s[number].temp_rx[i]);

	return (channel_s[number].fos_inwrite);
}

void rx_block_progress(int number)
{
       	fos_fillinbuf(number);

/*
	get_fos_info(number);
	if ((int)fos_info.ifree >= channel_s[number].n_rx)
		{
		fos_fillinbuf(number);
		for (i=0; i<channel_s[number].n_rx; i++)
			channel_s[number].sum += channel_s[number].temp_rx[i];
		channel_s[number].rx_count=i;
		}
*/
}
