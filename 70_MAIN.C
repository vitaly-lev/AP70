/*
	Vitaly Lev
				
	<C> <Lion>	

	70_main.c

	Programm for communication IBM-XT <-> IBM

	Main Function

	930608	V1.0

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bios.h>
#include <signal.h>
#include <conio.h>
#include <dos.h>
#include "70_def.h"


#define BREAK_KEY	0x2d00 /* alt-x */

/*
			Local functions prototype
*/

/*
			External functions prototype
*/					     
extern	int	set_port(int);	 
extern	void	send_b(byte, int);
extern	void	send_block(int);
extern	void j_exit(void);
extern	void write_csum(byte *, int, int *);
extern	void mes_prg(char *str);
extern	void tx_id_block(int);
extern	void tx_block(int, int);
extern	int 	read_file(int);
extern	void send_quest(int);
extern	void verify_id_block(int number);
extern	void send_byte_state(int, byte, int);
extern	void verify_block(int number);
extern	void write_block_file(int number);
extern	int 	calc_rx_csum(int, int);
extern	int 	parse_ctl(int);
extern	void timerins(void);
extern	int 	log_printf(char *text);
extern	int 	com_str_analize(int, char *[]);
extern	void *malloc_e(uint);
extern	int 	log_fprintf(char *, ...);
extern	void sleep_until_next(int);
extern	int	try_tx_next(int);
extern	int 	set_shedule(char);
extern	void print_debug(int);
extern	int 	print_modem_status(int);
extern	void	prn_debug_time(char *format, ...);
extern	void	prn_debug_string(char *format, ...);
extern	void	prn_debug_bytes(byte *start, int number, byte *string);
extern	int	_GetVidConfig(void);
extern	void print_ch_scr(void);
extern	void com_purge(int);
extern	int fos_fillinbuf(int);
extern	void rx_block_progress(int);
/*
			Global variable
*/
char *fn_tx;
char *fn_rx;
char *fn_rx_temp;
int  work_c;
long temp_timer;
int  exit_flag=FALSE;
int  error_flag=FALSE;
int  num_c=channel;
int  flag_log;
int  flag_debug;
int  flag_debug_i_o;			/* all channel I/O information on screen */
char *debug_string;
int  debug_in_cur_row=24;		/* default cursor position for */
int  debug_in_cur_col=70;		/* debug information on screen */
int  debug_out_cur_row=24;		/* default cursor position for */
int  debug_out_cur_col=73;		/* debug information on screen */
int  flag_computer;			/* computer type (IBM or ES1841 */

byte cin[DEBUGBUFF];
byte cout[DEBUGBUFF];
byte *pcin=&cin[0];			/* pointer of story's debug char (RX char) */
byte *psin=&cin[0];  			/* pointer of screen's debug char (RX char) */
byte *pcinend=&cin[DEBUGBUFF];
byte *pcout=&cout[0];			/* pointer of story's debug char (TX char) */
byte *psout=&cout[0];			/* pointer of screen's debug char (TX char) */
byte *pcoutend=&cout[DEBUGBUFF];
int  cinnum=0; 				/* number of receive chars */
int  cintemp=0;
int  coutnum=0;				/* number of sended chars */
int  couttemp=0;
int  flag_prn_es=0;			/* flag for print or not ES... in debug file */

char remoute[80];
char my[80];
char *id_remoute = remoute;
char *id_my      = my;
char *config_name = "70.ctl";
char *path70 = NULL;

FILE *log_file;
FILE *debug_file;
char log_name[80];
char debug_name[80];
int  number_debug;

long ttt=0;
char *text;
int  tx_count;
int  tx_number;
int  old_state;				/* for debug */
char *tmp_debug_string;
struct bl channel_s[channel];
int num_error;
/*
			External variable prototipe
*/
extern	int	set_one;
extern	long	timer;
extern	int	timer_no;
extern	long timer1;
extern	int	fossil;

int main(int argc, char *argv[])
{
int work_c;
int i;
int queue;
int *np;

	np=0;

	timer_no=TNUMBER*channel;
	signal(SIGINT, SIG_IGN);		/* close control-break */
	_GetVidConfig();
	
	fn_tx=malloc_e(100);
	fn_rx=malloc_e(100);
	fn_rx_temp=malloc_e(100);
	debug_string=malloc_e(1000);
	tmp_debug_string=malloc_e(1000);
	text=malloc_e(120);

	strcpy(fn_rx_temp, RX_TEMP_FILE_NAME);

	printf("\nES <-> IBM AP70 COMMUNICATION V1.1 07-15-1993");
	printf("\nCopyright 1993 by Vitaly Lev");
	printf("\nUkraine, Lviv, FIDO NET 2:462/10, Internet vl@ccrd.lviv.ua");
	
	printf("\n(press ALT-X to terminate program)");


	for (i=0; i<num_c; i++)
		{
		channel_s[i].timer 		= (&timer)+i*4*TNUMBER;
		channel_s[i].sh_timer_rx = channel_s[i].timer+1;
		channel_s[i].sh_timer_tx = channel_s[i].timer+2;
		channel_s[i].number = '0';
		channel_s[i].n_rx = 0;
		channel_s[i].attempt = 0;
		channel_s[i].error = 0;
		channel_s[i].rxb_n = 0;
		channel_s[i].rxb_p = &(channel_s[i].buffer_rx[0][0]);
		channel_s[i].to_file_p = &(channel_s[i].buffer_rx[0][0]);
		channel_s[i].rxn_p = &(channel_s[i].to_file_n[0]);
		channel_s[i].many = 0;
		channel_s[i].state = WAIT_STX;
		channel_s[i].ch.ch_active = 0;
		}
	
	if (parse_ctl(argc) == FALSE)
		exit(num_error);

	timerins();
	

	for (i=0; i<num_c; i++)
		{
		if (!set_port(i))
			{
			j_exit();
			exit(num_error);
			}
		else
			com_purge(i);
		}

	*channel_s[0].timer = 1;
	while (*channel_s[0].timer != 0)	/* !!! need replace */
			;


	for (i=0; i<num_c; i++)
		{
		print_modem_status(i);
		}

	if (argc > 1)
		{
		if (!com_str_analize(argc, argv))
			exit_flag = TRUE;
		}

	strcpy(text, '\0');
	strcpy(debug_string, '\0');


	for(;;)
		{
		work_c = 0;
		do
			{

			switch (channel_s[work_c].state)
				{
				case 0:	  /* Nothing to do */
					
	 				break; 

				case START_TX_ID_BLOCK: /* 1 */

					if (channel_s[work_c].attempt < ID_BLOCK_ATTEMPT)
						{
						tx_id_block(work_c); /* activate TX ID block */
						channel_s[work_c].state = WAIT_TX_ID_BLOCK;
						}
					else
						{
						switch (channel_s[work_c].error)
							{
							case 0:	strcpy(text, NO_ANSWER_FOR_ID_BLOCK);
									*channel_s[work_c].timer = BETWEEN_ID_GROUP;
									break;

							case EOT:	strcpy(text, ERROR_IN_ID_BLOCK);		break;
							case NAK: strcpy(text, ERROR_CSUM_IN_ID_BLOCK);	break;
							default:	strcpy(text, UNKNOWN_SYM_IN_ID_BLOCK);	break;
							}

						channel_s[work_c].state=SEND_EOT;
						error_flag = TRUE;
						}
					break;

				case WAIT_TX_ID_BLOCK: /* 2 */

					if (flag_debug_i_o)
						print_ch_scr();

					if (!channel_s[work_c].block_tx)
						{
						if ((channel_s[work_c].tx_count-channel_s[work_c].tx_number) <=5)
							channel_s[work_c].n_rx=channel_s[work_c].rx_count;
						}
					else
						{
						*(channel_s[work_c].timer) = BETWEEN_ID_BLOCK;
						channel_s[work_c].state = END_TX_ID_BLOCK;
						}
					break;

				case END_TX_ID_BLOCK: /* 3 */

					if (*(channel_s[work_c].timer) != 0 && channel_s[work_c].rx_count <= channel_s[work_c].n_rx)
						{
						if (flag_debug_i_o)
							print_ch_scr();
						}
					else
						{
						if (*(channel_s[work_c].timer) == 0)
							{
							channel_s[work_c].attempt++;
							channel_s[work_c].state=START_TX_ID_BLOCK;
							flag_prn_es=0;
							}
						else
							if ((channel_s[work_c].error=channel_s[work_c].temp_rx[--channel_s[work_c].rx_count]) != ACK)
								{
								channel_s[work_c].state=START_TX_ID_BLOCK;
								channel_s[work_c].attempt++;
								flag_prn_es=0;
								}
							else
								{
								channel_s[work_c].state=START_TX_BLOCK;
								flag_prn_es=0;
								}

						if (number_debug == DEBUG_LARGE)
							{
							if (!flag_prn_es)
								{
								prn_debug_time(" ES(%i): ", work_c);
								flag_prn_es=1;
								}
							prn_debug_bytes(&channel_s[work_c].error, 1, "\0");
							}
/*						else
							if
*/
						}

					break;

				case START_TX_BLOCK:	/* 4 */

					channel_s[work_c].number = '9';
					channel_s[work_c].attempt = 0;
					tx_block(work_c, NEW_NUMBER);
					break;

				case WAIT_TX_BLOCK:		/* 5 */

					if (flag_debug_i_o)
						print_ch_scr();

					if (!channel_s[work_c].block_tx)
						{
						if ((channel_s[work_c].tx_count-channel_s[work_c].tx_number) <=5)
							channel_s[work_c].n_rx=channel_s[work_c].rx_count;
						}
					else
						{
						*(channel_s[work_c].timer) = 3;
						channel_s[work_c].state=WAIT_ACK_TX_BLOCK;
						}
			 		break;

				case WAIT_ACK_TX_BLOCK: /* 6 */

					if (flag_debug_i_o)
						print_ch_scr();

					if (*(channel_s[work_c].timer) != 0 && channel_s[work_c].rx_count <= channel_s[work_c].n_rx)
						break;
					else
						if (*(channel_s[work_c].timer) == 0)
							{
							channel_s[work_c].state=CALL_ACK;
							break;
							}
						else
							{
							switch (channel_s[work_c].temp_rx[--channel_s[work_c].rx_count])
								{
								case ACK:
									channel_s[work_c].offset += channel_s[work_c].many;
									channel_s[work_c].length -= channel_s[work_c].many;
									tx_block(work_c, NEW_NUMBER);
									break;

								case NAK:
									if (channel_s[work_c].attempt++ < 3)
										{
										com_purge(work_c);
										tx_block(work_c, OLD_NUMBER);
										}
									else
										{
										strcpy(text, NAK_ON_TX_BLOCK);
										channel_s[work_c].state = SEND_EOT;
										channel_s[work_c].queue |= SHEDULE_TX_NOTOK;
										}
									break;

								case EOT:

									strcpy(text, EOT_IN_TX);
									channel_s[work_c].state = SEND_EOT;
									channel_s[work_c].queue |= SHEDULE_TX_NOTOK;
									break;

								default:
									
									channel_s[work_c].state = CALL_ACK;
									break;
								}

							if (number_debug == DEBUG_LARGE)
								{
								if (!flag_prn_es)
									{
									prn_debug_time(" ES(%i): ", work_c);
									flag_prn_es=1;
									}
								prn_debug_bytes(&channel_s[work_c].temp_rx[channel_s[work_c].rx_count], 1, "\0");
								}
							
							flag_prn_es=0;							
							break;
							}

				case CALL_ACK:		/* 7 */

					if (channel_s[work_c].attempt++ <= 3)
						{
						com_purge(work_c);
						send_quest(work_c);
						}
					else
						{
						if (channel_s[work_c].rx_count == 0)
							strcpy(text, TIMEOUT_ACK_TX_BLOCK);
						else
							strcpy(text, UNKNOWN_SYM_ON_RX_ACK);
						channel_s[work_c].state = SEND_EOT;
						channel_s[work_c].queue |= SHEDULE_TX_NOTOK;
						}
					break;
						
				case SEND_EOT: /* 8 */

					send_byte_state(work_c, EOT, END_TX);
					break;

				case END_TX: /* 9 */

					if (channel_s[work_c].block_tx || *(channel_s[work_c].timer) == 0)
						{
						channel_s[work_c].number = '0';
						channel_s[work_c].n_rx = 0;
						channel_s[work_c].attempt = 0;
						channel_s[work_c].error = 0;
						channel_s[work_c].rxb_n = 0;
						channel_s[work_c].rxb_p = &(channel_s[work_c].buffer_rx[0][0]);
						channel_s[work_c].to_file_p = &(channel_s[work_c].buffer_rx[0][0]);
						channel_s[work_c].rxn_p = &(channel_s[work_c].to_file_n[0]);
						channel_s[work_c].many = 0;
						channel_s[work_c].state = WAIT_STX;

						if (number_debug == DEBUG_LARGE)
							{
							prn_debug_time(" ES(%i): ", work_c);
							flag_prn_es=1;
							}

						com_purge(work_c);
						}
					break;

				case WAIT_STX: /* 20 */
				
					if (flag_debug_i_o)
						print_ch_scr();

					if (channel_s[work_c].n_rx < channel_s[work_c].rx_count)
						{
						if (number_debug == DEBUG_LARGE)
							{
							if (!flag_prn_es)
								{
								prn_debug_time(" ES(%i): ", work_c);
								flag_prn_es=1;
								}
							prn_debug_bytes(&channel_s[work_c].temp_rx[channel_s[work_c].n_rx], 1, "\0");
							}
						
						
						if (channel_s[work_c].temp_rx[channel_s[work_c].n_rx++] != STX)
							{
							if (channel_s[work_c].n_rx >= 512)
								{
								strcpy(text, DUST_IN_CHANEL);
								channel_s[work_c].n_rx=0;
								}
							}
						else
							{
							channel_s[work_c].state = WAIT_RX_ID_BLOCK;
							channel_s[work_c].offset = channel_s[work_c].n_rx;
							*channel_s[work_c].timer = 2;
							flag_prn_es=0;
							}
						}
					break;
						
				case WAIT_RX_ID_BLOCK: /* 21 */

					if (*channel_s[work_c].timer == 0)
						{
						strcpy(text, TIME_OUT_IN_RX);
						channel_s[work_c].state = SEND_EOT;
						}
					else
						{
/*!!! no work in many*/
/*channels revime*/							
						
/*						do
							{
*/							if (channel_s[work_c].n_rx < channel_s[work_c].rx_count)
								{
								
								if (flag_debug_i_o)
									print_ch_scr();

								if (number_debug == DEBUG_LARGE)
									{
									if (!flag_prn_es)
										{
										prn_debug_time(" ES(%i): ", work_c);
										flag_prn_es=1;
										}

									prn_debug_bytes(&channel_s[work_c].temp_rx[channel_s[work_c].n_rx], 1, "\0");
									}

								if (channel_s[work_c].temp_rx[channel_s[work_c].n_rx] != ETX)
									{
									if ((++channel_s[work_c].n_rx - channel_s[work_c].offset) > 40)
										{
										strcpy(text, ID_BLOCK_TOO_BIG);
										channel_s[work_c].state = SEND_EOT;
										}
									}
								else
									{
									++channel_s[work_c].n_rx;
									channel_s[work_c].state = WAIT_RX_CSUM_ID_BLOCK;
									flag_prn_es=0;
									}
								}
/*							}
						while (channel_s[work_c].n_rx < channel_s[work_c].rx_count && channel_s[work_c].state==WAIT_RX_ID_BLOCK);
*/
						}
					break;

				case WAIT_RX_CSUM_ID_BLOCK: /* 22 */

					if (*(channel_s[work_c].timer) == 0)
						{
						strcpy(text, TIME_OUT_IN_RX);
						channel_s[work_c].state = SEND_EOT;
						}
					else
						{
						if (channel_s[work_c].n_rx < channel_s[work_c].rx_count)
							{
							if (number_debug == DEBUG_LARGE)
								{
								if (!flag_prn_es)
									{
									prn_debug_time(" ES(%i): ", work_c);
									flag_prn_es=1;
									}

								prn_debug_bytes(&channel_s[work_c].temp_rx[channel_s[work_c].n_rx], 1, "\0");
								}
							verify_id_block(work_c);
							}
						}
					break;

				case WAIT_START_RX_BLOCK: /* 23 */

					if (*(channel_s[work_c].timer) == 0)
						{
						strcpy(text, TIME_OUT_IN_RX);
						channel_s[work_c].state = SEND_EOT;
						}
					else
						{
						if (channel_s[work_c].n_rx < channel_s[work_c].rx_count )
							{
							if (flag_debug_i_o)
								print_ch_scr();

							if (number_debug == DEBUG_LARGE)
								{
								if (!flag_prn_es)
									{
									prn_debug_time(" ES(%i): ", work_c);
									flag_prn_es=1;
									}
								prn_debug_bytes(&channel_s[work_c].temp_rx[0], 1, "\0");
								}

							if (channel_s[work_c].temp_rx[0] == EOT)
								{
								strcpy(text, EOT_IN_TX);
								channel_s[work_c].state = SEND_EOT;
								flag_prn_es=0;
								}
							else
							if (channel_s[work_c].temp_rx[0] == ENQ)
								{
								strcpy(text, CALL_ACK_ON_RX);
								channel_s[work_c].n_rx++;
								channel_s[work_c].state = AGAIN_ACK;
								channel_s[work_c].error = ENQ;
								flag_prn_es=0;
								}
							else
								{
								channel_s[work_c].state = WAIT_RX_BLOCK;
								channel_s[work_c].n_rx++;
								flag_prn_es=0;
								}
							}
						}
					break;

				case WAIT_RX_BLOCK: /* 24 */

					if (*(channel_s[work_c].timer) == 0)
						{
						strcpy(text, TIME_OUT_IN_RX);
						channel_s[work_c].state = SEND_EOT;
						}
					else
						{
/*
						do
							{
*/
							if (channel_s[work_c].n_rx < channel_s[work_c].rx_count)
								{

								if (flag_debug_i_o)
									print_ch_scr();

								if (number_debug == DEBUG_LARGE)
									{
									if (!flag_prn_es)
										{
										prn_debug_time(" ES(%i): ", work_c);
										flag_prn_es=1;
										}
									prn_debug_bytes(&channel_s[work_c].temp_rx[channel_s[work_c].n_rx], 1, "\0");
									}

								if (channel_s[work_c].n_rx <= 32+2) /* 3: block number + (ETX || ETB) + csum */
									{
									if (channel_s[work_c].temp_rx[channel_s[work_c].n_rx] == ETB ||
								    	channel_s[work_c].temp_rx[channel_s[work_c].n_rx] == ETX)
										{
										channel_s[work_c].state = WAIT_RX_CSUM_BLOCK;
										channel_s[work_c].n_rx++;
										flag_prn_es=0;
										}
									else
										channel_s[work_c].n_rx++;
									}
								else
									{
									if (channel_s[work_c].attempt++ <=3)
										{
										strcpy(text, BLOCK_TOO_BIG);
										channel_s[work_c].n_rx = 0;
										channel_s[work_c].rx_count = 0;
										channel_s[work_c].state = SEND_NAK_BLOCK_1;
										flag_prn_es=0;
										}
									}
								}

/*							}
						while (channel_s[work_c].n_rx<channel_s[work_c].rx_count && channel_s[work_c].state==WAIT_RX_BLOCK);
*/
						}
					break;

				case WAIT_RX_CSUM_BLOCK: /* 25 */

					if (*(channel_s[work_c].timer) == 0)
						{
						strcpy(text, TIME_OUT_IN_RX);
						channel_s[work_c].state = SEND_EOT;
						}
					else
						{
						if (channel_s[work_c].n_rx < channel_s[work_c].rx_count)
							{
							if (flag_debug_i_o)
								print_ch_scr();

							if (number_debug == DEBUG_LARGE)
								{
								if (!flag_prn_es)
									{
									prn_debug_time(" ES(%i): ", work_c);
									flag_prn_es=1;
									}
								prn_debug_bytes(&channel_s[work_c].temp_rx[channel_s[work_c].n_rx], 1, "\0");
								}
							verify_block(work_c);
							flag_prn_es=0;
							}
						}
					break;

				case SEND_ACK_BLOCK: /* 26 */

					if (channel_s[work_c].error == ETB) /* This is ending block? */
						send_byte_state(work_c, ACK, WAIT_START_RX_BLOCK);
					else
						send_byte_state(work_c, ACK, WAIT_EOT);
					break;

				case SEND_NAK_WAIT_STX: /* 27 */

					send_byte_state(work_c, NAK, END_TX);
					break;

				case SEND_ACK_WAIT_STX: /* 28 */

					send_byte_state(work_c, ACK, WAIT_STX);
					break;

				case SEND_NAK_BLOCK: /* 29 */

					send_byte_state(work_c, NAK, WAIT_RX_BLOCK);
					break;

				case SEND_NAK_BLOCK_1: /* 2A */

					if (*(channel_s[work_c].timer) == 0)
						send_byte_state(work_c, NAK, WAIT_RX_BLOCK);
					else
						{
						if (channel_s[work_c].n_rx < channel_s[work_c].rx_count)
							{
							if (number_debug == DEBUG_LARGE)
								{
								if (!flag_prn_es)
									{
									prn_debug_time(" ES(%i): ", work_c);
									flag_prn_es=1;
									}
								prn_debug_bytes(&channel_s[work_c].temp_rx[channel_s[work_c].n_rx], 1, "\0");
								}

							if (++channel_s[work_c].n_rx > 512)
								{
								strcpy(text, DUST_IN_CHANEL);
								channel_s[work_c].n_rx=0;
								send_byte_state(work_c, NAK, SEND_EOT);
								}
							}
						}
					flag_prn_es=0;
					break;

				case WAIT_EOT: /* 2B */

					if (*(channel_s[work_c].timer) != 0 && channel_s[work_c].rx_count == 0)
						break;
					else
						{
						channel_s[work_c].queue |= WRITE_BLOCK+CLOSE_RX_FILE;
						channel_s[work_c].state = SEND_EOT;
						break;
						}

				case AGAIN_ACK: /* 2C */

					if (*(channel_s[work_c].timer) == 0)
						{
						strcpy(text, TIME_OUT_IN_RX);
						channel_s[work_c].state = SEND_EOT;
						}
					else
						{
						if (channel_s[work_c].n_rx < 5) /* 3: block number + (ETX || ETB) + csum */
							{
							if (flag_debug_i_o)
								print_ch_scr();

							if (number_debug == DEBUG_LARGE)
								{
								if (!flag_prn_es)
									{
									prn_debug_time(" ES(%i): ", work_c);
									flag_prn_es=1;
									}
								prn_debug_bytes(&channel_s[work_c].temp_rx[channel_s[work_c].n_rx], 1, "\0");
								}

							if (channel_s[work_c].temp_rx[channel_s[work_c].n_rx] == ETB ||
							channel_s[work_c].temp_rx[channel_s[work_c].n_rx] == ETX)
								{
								channel_s[work_c].state = WAIT_RX_CSUM_BLOCK;
								flag_prn_es=0;
								}
							else
								channel_s[work_c].n_rx++;
							}
						else
							{
							if (channel_s[work_c].attempt++ <=3)
								{
								channel_s[work_c].n_rx = 0;
								channel_s[work_c].rx_count = 0;
								channel_s[work_c].state = WAIT_START_RX_BLOCK;
								flag_prn_es=0;
								}
							else
								{
								strcpy(text, UNKNOWN_BLOCK);
								channel_s[work_c].state = SEND_EOT;
								flag_prn_es=0;
								}
							}
						}


				} /* switch STATE */


			if (fossil)
				rx_block_progress(work_c);
			
			}while(++work_c < num_c);

			
			
/*
end of scanining channel
*/





			if (strcmp(text, "\0") != 0)
				{
				if (log_printf(text) == FALSE)
					{
					j_exit();
					exit(num_error);
					}
				}



			if (old_state != channel_s[0].state)
				{


				old_state = channel_s[0].state;

				if (number_debug == DEBUG_LARGE)
					{
					strcpy(tmp_debug_string, debug_string);
					strcpy(debug_string, "\0");

					prn_debug_time(" status(%i): ", work_c-1);
					prn_debug_string("%s st=%.2x ofst=%i rxc=%i q=%.4x",
														debug_string,
														channel_s[0].state,
														channel_s[0].offset,
														channel_s[0].rx_count,
														channel_s[0].queue
								 );

					strcpy(debug_string, tmp_debug_string);
					}
				
/*
				log_fprintf("st=%x ofst=%i rxc=%i q=%x",
					channel_s[0].state,
					channel_s[0].offset,
					channel_s[0].rx_count,
					channel_s[0].queue);
*/
				}


			for (i=0; i<num_c; i++)
				{
				queue=channel_s[i].queue;
		
				if ((queue & WRITE_BLOCK) != 0)
					{
					write_block_file(i);
					channel_s[i].queue &= ~WRITE_BLOCK;
					}
		
				if ((queue & CLOSE_RX_FILE) != 0)
					{
					log_fprintf("%s (%s%s)",	FILE_RX_OK,
										channel_s[i].file_rx_path,
										channel_s[i].file_rx_name
							 );

					if (fclose(channel_s[i].file_rx) == 0)
						{
						log_fprintf("%s (%s%s)", FILE_CLOSE_OK,
											channel_s[i].file_rx_path,
											channel_s[i].file_rx_name
								 );
						channel_s[i].queue &= ~CLOSE_RX_FILE;
						channel_s[i].queue |= SHEDULE_RX_OK;
						if ((queue & COMMAND_RX_FILE) != 0)
							exit_flag = TRUE;
						}
					else
						{
						log_fprintf("%s sys_error_number=%i: %s)", ERROR_CLOSE_RECEIVED_FILE, errno, sys_errlist[errno]);
						channel_s[i].queue |= SHEDULE_RX_NOTOK;
						exit_flag = TRUE;
						}
					}
				else
					{
					if (((queue & COMMAND_RX_FILE) != 0) && ((queue & COMMAND_TX_FILE) == 0))
						{
						if (*channel_s[i].sh_timer_rx == 0 && channel_s[i].state == WAIT_STX)
							{
							log_printf(TIME_OUT_RX_FILE);
							exit_flag = TRUE;
							}
						}
					}


				if ((queue & COMMAND_TX_FILE) != 0)
					{
					if (error_flag)
						exit_flag = TRUE;
			
					if (channel_s[i].state == WAIT_STX)
						{
						if ((queue & COMMAND_RX_FILE) == 0)
							{
							exit_flag = TRUE;
							}
						else
							{
							*channel_s[i].sh_timer_rx = temp_timer;
							channel_s[i].queue &= ~COMMAND_TX_FILE;
/*							printf("\nOK, timer=%li\n", *channel_s[i].timer);*/
							}
						}
					}

				if ((queue & COMMAND_LINE) == 0)
					{
					if ((queue & SHEDULE_RX_OK) != 0)
						{
						channel_s[i].queue &= ~SHEDULE_RX_OK;
						set_shedule('r');

						if ((channel_s[i].queue & SHEDULE_TX_SLEEP) != 0)
							*(channel_s[i].sh_timer_tx) = BETWEEN_FILES;
						}

					else	
					if	((queue & SHEDULE_TX_OK) != 0)
						sleep_until_next(i);		/* sleep to TX next file for RX file */
					
					else
					if ((queue & SHEDULE_TX_SLEEP) != 0)
						{
						if ((channel_s[i].state == WAIT_STX) && (*(channel_s[i].sh_timer_tx) == 0l))
							{
							channel_s[i].queue &= ~SHEDULE_TX_SLEEP;
							set_shedule('t');
							}
						}
					
					else
					if ((queue & SHEDULE_TX_FILE) != 0)
						{
						if ((channel_s[i].state == WAIT_STX) && (*(channel_s[i].timer) == 0l))
							channel_s[i].state = START_TX_ID_BLOCK;
						}
					}

					if (flag_debug)
						{
						print_debug(i);
						if (strlen(debug_string) != 0)
							prn_debug_string("%s", debug_string);
						}

					if (flag_debug_i_o)
						print_ch_scr();

				} /* for number channel */

			if (_bios_keybrd(1) != 0 && _bios_keybrd(0) == BREAK_KEY)
				{
				log_printf(ERROR_BREAK);
				j_exit();
				exit(num_error);
				}

			if (exit_flag)
				{
				j_exit();
				exit(num_error);
				}
		}

	log_printf(INCORRECT_EXIT);
	prn_debug_time(" status(%i): ", work_c-1);
	prn_debug_string("%s st=%.2x ofst=%i rxc=%i q=%.4x",
										debug_string,
										channel_s[0].state,
										channel_s[0].offset,
										channel_s[0].rx_count,
										channel_s[0].queue
					);
	j_exit();
	return(num_error);

}

