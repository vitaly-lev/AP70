/*
	Vitaly Lev	Fido Net  2:462/10
				Internet  vl@ccrd.lviv.ua
	<C> <Lion>	

	70_util.c

	Programm for communication ES <-> IBM

	Varios Functions

	930608	V1.0

*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <io.h>
#include <dos.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <errno.h>
#include "70_def.h"

/*
			Local functions prototype
*/
int	tx_file(char *, int);
void	write_csum(byte *, int, int *);
void	ibm_es(byte *, int, int *);
void	es_ibm(byte *, int *);
void	tx_id_block(int);
void	tx_block(int, int);
int	read_file(int);
void	send_quest(int);
int	open_rx(char *fn_tx, int number_c);
void	verify_id_block(int number);
void	send_byte_state(int, byte, int);
void	verify_block(int number);
void	write_block_file(int number);
int	calc_rx_csum(int, int, int);
int	log_printf(char *);
int	start_log(void);
void	*malloc_e(uint);
void	path_name(char *, char *, char *);
void	path_name_l(char *, char *, char *);
FILE	*open_next(char *, struct find_t *, char *, char *);
void	what_sign(char *, byte *, byte *);
char	*zerro_name(char *);
int	is_name_digit(char *, byte *, byte);
char	*next_name(char *, byte *, byte);
int	log_fprintf(char *, ...);
int	open_tx(char *, int);
void	sleep_until_next(int);
int	system_h_m(void);
void	print_debug(int);
int	print_modem_status(int);
int	system_s(void);
long	system_h_m_s(void);
int	start_debug(void);
void	prn_debug_time(char *format, ...);
void	prn_debug_string(char *format, ...);
void	prn_debug_bytes(byte *start, int number, byte *string);
void	screen_char_in(byte);
void	screen_char_out(byte);
void	print_ch_scr(void);

/*
			External functions prototype
*/
extern	void	set_port(int);
extern	void	send_b(byte, int);
extern	int 	receive_b(int, int);
extern	void	send_block(int, int);
extern	void dostime(int *, int *, int *, int *);
extern	void	dosdate(int *, int *, int *, int *);
extern	void j_exit(void);
extern	char	*add_backslash (char *);
extern	void	modem_status(int);
extern	int	_GetCurPos(void);
extern	void _SetCurPos(int, int);
extern	void	_PutChar(char);
extern	void com_purge(int);
extern	int 	set_shedule(char);

/*
			Global variable
*/
long timer1, timer2, timer3;
/*
			External variable prototype
*/
extern	int	set_one;
extern 	struct bl channel_s[];
extern 	long ttt;
extern 	char *text;
extern	char *id_remoute;
extern	char *id_my;
extern	FILE *log_file;
extern	int	flag_log;
extern	int	num_c;
extern	char log_name[];
extern	int	num_error;

extern	FILE *debug_file;
extern	char *debug_name;
extern	int  number_debug;
extern	char *debug_string;
extern	int	debug_in_cur_row;
extern	int	debug_in_cur_col;
extern	int	debug_out_cur_row;
extern	int	debug_out_cur_col;

extern	byte cin[];
extern	byte cout[];
extern	byte *pcin;
extern	byte *pcinend;
extern	byte *pcout;
extern	byte *pcoutend;
extern	int  cinnum;
extern	int  cintemp;
extern	int  coutnum;
extern	int  couttemp;
extern	byte *psin;
extern	byte *psout;

void write_block_file(int number)
{

/*!!!	*/
	es_ibm(channel_s[number].to_file_p, channel_s[number].rxn_p);

	if (fwrite(channel_s[number].to_file_p, *(channel_s[number].rxn_p), 1, channel_s[number].file_rx) < 1)
		{
		log_printf(ERROR_WRITE_DISK);
		channel_s[number].state = SEND_EOT;
		fclose(channel_s[number].file_rx);
		set_shedule('r');
		}
	else
		{
		*(channel_s[number].rxn_p) = 0;

		if (channel_s[number].rxb_n == 0)
			{
			channel_s[number].to_file_p = &(channel_s[number].buffer_rx[0][0]);
			channel_s[number].rxn_p = &(channel_s[number].to_file_n[0]);
			}
		else
			{
			channel_s[number].to_file_p = &(channel_s[number].buffer_rx[1][0]);
			channel_s[number].rxn_p = &(channel_s[number].to_file_n[1]);
			}
		}
}

void verify_block(int number)
{
int num;

	if (channel_s[number].attempt <= 3)
		{
		if (channel_s[number].error == ENQ)
			{
			channel_s[number].n_rx++;
			if (calc_rx_csum(number, channel_s[number].offset, channel_s[number].n_rx) == FALSE)
				{
				strcpy(text, BLOCK_CSUM_ERROR);
				channel_s[number].attempt++;
				channel_s[number].state = SEND_NAK_BLOCK;
				channel_s[number].error = NAK;
				}
			else
				{
				channel_s[number].attempt=0;
				channel_s[number].state = SEND_ACK_BLOCK;
				}
			}
		else
			{
			channel_s[number].error = channel_s[number].temp_rx[channel_s[number].n_rx-1]; /*memory ETB or ETX*/

			if (calc_rx_csum(number, channel_s[number].offset, channel_s[number].n_rx) == FALSE)
				{
				strcpy(text, BLOCK_CSUM_ERROR);
				channel_s[number].attempt++;
				channel_s[number].state = SEND_NAK_BLOCK;
				channel_s[number].error = NAK;
				}
			else
				{
				if (channel_s[number].temp_rx[channel_s[number].offset] != channel_s[number].number)
					{
					sprintf(text, "%s %s=%c %s=%c", BLOCK_NUMBER_ERROR,
											  REAL_NUMBER, channel_s[number].temp_rx[channel_s[number].offset],
											  WAITING_NUMBER, channel_s[number].number
						  );

					/*strcpy(text, BLOCK_NUMBER_ERROR);*/

					channel_s[number].attempt++;
					channel_s[number].state = SEND_NAK_BLOCK;
					channel_s[number].error = NAK;
					}
				else
					{
					if (++channel_s[number].number > '9')
						channel_s[number].number = '0';
					num = channel_s[number].n_rx-2;	/* 3: block number+ETB+csum (2->buffer offset=0) */
					memcpy(channel_s[number].rxb_p, &(channel_s[number].temp_rx[channel_s[number].offset+1]), num);
					channel_s[number].rxb_p += num;
					channel_s[number].many += num;
					channel_s[number].to_file_n[channel_s[number].rxb_n] = channel_s[number].many;
					channel_s[number].state = SEND_ACK_BLOCK;

/*					channel_s[number].error = ACK;*/

					if (channel_s[number].many+32 > 512)
						{
						if (channel_s[number].queue & WRITE_BLOCK != 0)
							{
							strcpy(text, BLOCK_NOT_WRITE_TO_DISK);
							channel_s[number].state = SEND_EOT;
							}
						else
							{
							channel_s[number].queue |= WRITE_BLOCK; /* write to disk RX buffer number */
							channel_s[number].many = 0;

							if (channel_s[number].rxb_n == 0)
								{
								channel_s[number].rxb_p = &(channel_s[number].buffer_rx[1][0]);
								channel_s[number].to_file_p = &(channel_s[number].buffer_rx[0][0]);
								channel_s[number].rxn_p = &(channel_s[number].to_file_n[0]);
								channel_s[number].rxb_n = 1;
								}
							else
								{
								channel_s[number].rxb_p = &(channel_s[number].buffer_rx[0][0]);
								channel_s[number].to_file_p = &(channel_s[number].buffer_rx[1][0]);
								channel_s[number].rxn_p = &(channel_s[number].to_file_n[1]);
								channel_s[number].rxb_n = 0;
								}

							} /* if previos buffer write on disk o'key */

						} /* if number RX bytes > |512| */

					} /* if RX block number o'key */

				} /* if not error in RX block csum */

			} /* if first RX symbol != ENQ */

		} /* if number attempt of RX block < 3 */
	else
		{
		strcpy(text, NUMBER_ATTEMPT_TOO_BIG);
		channel_s[number].state = SEND_EOT;
		}
	channel_s[number].offset = 0;
}

void verify_id_block(int number)
{
char id_block[40];
char *buf_st;

	buf_st = (char *)(&(channel_s[number].temp_rx[channel_s[number].offset+2]));

	if (channel_s[number].attempt <= 3)
		{
		if (calc_rx_csum(number, channel_s[number].offset, channel_s[number].n_rx) == FALSE)
			{
				strcpy(text, ID_BLOCK_CSUM_ERROR);
				channel_s[number].attempt++;
				channel_s[number].state = SEND_NAK_WAIT_STX;
			}
		else
			{
			strcpy(id_block, id_remoute);
			strcat(id_block, id_my);
			strcat(id_block, "\0");

			if (strncmp(buf_st, id_block, strlen(id_block)) == 0)
				{
				channel_s[number].error = ETB;
				channel_s[number].state = SEND_ACK_BLOCK;
				}
			else
				{
				strcpy(id_block, id_my);
				strcat(id_block, id_my);
				strcat(id_block, "\0");

				if (strcmp(buf_st, id_block) == 0)
					channel_s[number].state = SEND_ACK_WAIT_STX;
				else
					{
/*!!!*/				if (strncmp(buf_st-2, "[H[", 3) == 0)
						channel_s[number].state = SEND_ACK_WAIT_STX;
					else
						{
						strcpy(text, ERROR_IN_ID_BLOCK);
						channel_s[number].state = SEND_NAK_WAIT_STX;
						}
					}
				}
			}
		}
	else
		channel_s[number].state = SEND_EOT;

	channel_s[number].offset = 0;
}

void send_quest(int number)
{
int num;

	channel_s[number].temp_tx[0] = ENQ;
	channel_s[number].temp_tx[1] = ETB;
	num=2;
	write_csum(&(channel_s[number].temp_tx[0]), 0, &num);
	channel_s[number].state = WAIT_ACK_TX_BLOCK;
}

int tx_file(char *fn_tx, int number_c)
{
int ctl=TRUE;


	if((channel_s[number_c].file_tx=fopen(fn_tx,"rb")) != NULL)
		{
		if((channel_s[number_c].length=filelength(fileno(channel_s[number_c].file_tx))) == 0l)
			{
			log_fprintf("%s (%s)", LENGTH_OF_TX_FILE_ZERRO, channel_s[number_c].file_tx);
			ctl = FALSE;
			}
		else
			{
			if (read_file(number_c))
	  			{
				channel_s[number_c].state  = START_TX_ID_BLOCK;
				channel_s[number_c].tx_state  = TRUE;
				}
			else
				{
				channel_s[number_c].state  = SEND_EOT;
				channel_s[number_c].tx_state  = FALSE;
				ctl = FALSE;
				}
			}
		}
	else
		{
		log_fprintf("%s (%s)", NO_FILE_FOR_TX, channel_s[number_c].file_tx);
		ctl = FALSE;
		}
	return ctl;
}


int open_tx(char *fn_tx, int number_c)
{
struct find_t f_buffer;
char *path;
char *name;
char *full_name;
int ctl=FALSE;
int fnext=TRUE;

	full_name=malloc_e(100);
	path=malloc_e(100);
	name=malloc_e(20);

	if (_dos_findfirst(fn_tx, _A_NORMAL, &f_buffer) == 0)
		{
		path_name(fn_tx, path, name);
		strcpy(name, f_buffer.name);

		full_name=strcat(strcpy(full_name, path), name);

		if (tx_file(full_name, number_c))
			{
			channel_s[number_c].queue |= SHEDULE_TX_FILE;
			strcpy(channel_s[number_c].file_tx_name, name);
			strcpy(channel_s[number_c].file_tx_path, path);
			ctl=TRUE;
			}
		else /* file for TX open ERROR, try open next file */
			{
			while ((_dos_findnext(&f_buffer) == 0) && fnext)
				{
				strcpy(name, f_buffer.name);
				full_name=strcat(strcpy(full_name, path), name);
				if (tx_file(full_name, number_c))
					{
					strcpy(channel_s[number_c].file_tx_name, name);
					strcpy(channel_s[number_c].file_tx_path, path);
					fnext=FALSE; /* stop find_next */
					ctl=TRUE;	   /* signal what file open o'key */
					log_fprintf("%s (%s%s)", FILE_TX_OPEN_OKEY,
										channel_s[number_c].file_tx_path,
										channel_s[number_c].file_tx_name
				 			);
					}
				}
			}
		}
	else
		{
		log_fprintf("%s (%s)", ERROR_OPEN_TX_FILE, fn_tx);
		ctl = FALSE;
		}

	free(name);
	free(path);
	free(full_name);
	return ctl;
}

void sleep_until_next(int number_c)
{
int h_m_sb;
int h_m_se;
int h_m_sys;

	channel_s[number_c].queue &= ~SHEDULE_TX_OK;
	channel_s[number_c].queue &= ~SHEDULE_TX_FILE;


	if ((strcmp(&channel_s[number_c].sh[channel_s[number_c].tx_sh_active].move_path[0], "\0")) == 0)
		{
		strcpy(text, &channel_s[number_c].file_tx_path[0]);
		strcat(text, &channel_s[number_c].file_tx_name[0]);
		if (remove(text) == 0)
			{
			log_fprintf("%s (%s%s)", FILE_DELETED,
								channel_s[number_c].file_tx_path,
								channel_s[number_c].file_tx_name
					 );
			}
		else
			{
			if (errno == EACCES)
				log_fprintf("%s (%s%s)", ERROR_DELETE_TX_FILE_RO,
									&channel_s[number_c].file_tx_path[0],
									&channel_s[number_c].file_tx_name[0]
						 );
			else
			if (errno == ENOENT)
				log_fprintf("%s (%s%s)", ERROR_DELETE_TX_FILE_NF,
									&channel_s[number_c].file_tx_path[0],
									&channel_s[number_c].file_tx_name[0]
						 );
			else
				log_fprintf("%s (%s%s)", ERROR_DELETE_TX_FILE_UN,
									&channel_s[number_c].file_tx_path[0],
									&channel_s[number_c].file_tx_name[0]
							);

			}
			/*if () if delete error ???*/
		}

	else	/* if exist catalog for move file */
		{
		;
		}

	channel_s[number_c].queue |= SHEDULE_TX_SLEEP;
	h_m_sb = channel_s[number_c].sh[channel_s[number_c].tx_sh_active].int_hm; /* between search file time */
	h_m_se = channel_s[number_c].sh[channel_s[number_c].tx_sh_active].int_hm; /* end shedule time */
	h_m_sys = system_h_m();
	if (h_m_sb != 0)
		{
		if ((h_m_sb + h_m_sys) < h_m_se)
			*(channel_s[number_c].sh_timer_tx) = (long)(h_m_sb*60);
		else
		if ((h_m_sys + 1) < h_m_se)
			*(channel_s[number_c].sh_timer_tx) = BETWEEN_FILES;
		else
			*(channel_s[number_c].sh_timer_tx) = (long)(60-system_s());
		}
	else
		*(channel_s[number_c].sh_timer_tx) = BETWEEN_FILES;
}


int open_rx(char *rx_name, int number_c)
{
char name[20];
char path[100];

	
	
	if (strchr(rx_name,(int)'?') == NULL)
		{
		if((channel_s[number_c].file_rx=fopen(rx_name,"ab")) == NULL)
			{
			log_fprintf("%s (%s)", ERROR_OPEN_RX_FILE, rx_name);
			return FALSE;	 
			}
		else
			{
			log_fprintf("%s (%s)", FILE_RX_OPEN_OKEY, rx_name);
/* not!!!	channel_s[number_c].state = WAIT_STX;*/
			path_name(rx_name, path, name);
			}
		}
	else
		{
		if ((channel_s[number_c].file_rx = open_next(rx_name, 	&channel_s[number_c].f_buf_rx, path, name)) == NULL)
			{
			log_fprintf("%s (%s)", ERROR_OPEN_RX_FILE, rx_name);
			return FALSE;
			}
		else
			log_fprintf("%s (%s%s)", FILE_RX_OPEN_OKEY, path, name);
		}

	strcpy(channel_s[number_c].file_rx_path, path);
	strcpy(channel_s[number_c].file_rx_name, name);

	return TRUE;
}


/*
	search for next file & open it (name of file have ??? or *)

	input:	string with path and name of file
	
	otput:	pointer to open file if o'key, or NULL pointer if
			error in path&name string or open file filure
*/
FILE *open_next(char *full_name, struct find_t *f_buffer, char *path, char *name)
{
char *name_nul;
char *temp_path;
char *temp_name;
byte *sign;
byte number;
FILE *ctl;
struct stat sb;

	sign=malloc_e(12);
	name_nul=malloc_e(15);
	temp_path=malloc_e(100);
	temp_name=malloc_e(100);

	path_name(full_name, path, name);

	what_sign(name, &sign[0], &number);
	name_nul = zerro_name(name);
	strcpy(temp_path, path);

	if (_dos_findfirst(full_name, _A_NORMAL, f_buffer) == 0)
		{
		if (stricmp(f_buffer->name, name_nul) > 0)
			strcpy(name_nul, f_buffer->name);

		while (_dos_findnext(f_buffer) == 0)
			{
			
			if (
				(strlen(f_buffer->name) == strlen(name_nul)) &&
				(is_name_digit(f_buffer->name, sign, number))&&
			     (stricmp(f_buffer->name, name_nul) > 0)
			)
				strcpy(name_nul, f_buffer->name);
			
			}

		strcpy(temp_name, temp_path);
		strcat(temp_name, name_nul),
		stat(temp_name, &sb);

		if (sb.st_size == 0l)
			ctl=fopen(temp_name, "ab");
		else
			ctl=fopen(temp_name=strcat(temp_path, next_name(name_nul, sign, number)), "ab");
		}
	else
		{
		ctl=fopen(temp_name=strcat(temp_path, name_nul), "ab");
		}

	if (ctl)
		path_name(temp_name, path, name);

	free(temp_name);
	free(temp_path);
	free(sign);
	free(name_nul);

	return ctl;
}


/*
	calculate next figure for file name

	input:	name of file, digit's place in name, digit's number

	output:	next name of file
*/
char *next_name(char *name, byte *sign, byte number)
{
	for (; --number!=0;)
		if (*(name+*(sign+number)) == '9')
			*(name+*(sign+number)) = '0';
		else
		  	{
			(*(name+*(sign+number)))++;
			break;
			}
	return name;
}


/*
	search name of file to is digit in sign[]

	input:	name of file
	output:	TRUE if need simbol is digit
*/
int is_name_digit(char *name, byte *sign, byte number)
{
byte i;

	for (i=0; i<number; i++)
		if (!isdigit(*(name+*(sign+i))))
			return FALSE;

	return TRUE;
}


/*
	replace all sign ??? to 0 and
	allocate memory for output string
	
	input:	string for search
	output:	replace string
*/
char *zerro_name(char *name)
{
char *name_nul;
uint i;

	name_nul = malloc_e(strlen(name));
	strcpy(name_nul, name);

	for (i=0; i<strlen(name); i++)
		if(*(name+i) == '?')
			*(name_nul+i) = '0';

	return name_nul;
}

/*
	search name of file to ??? & * and store pointers
	to sign ??? on array (sign), store number of ???
	on variable (number)

	input:	name of file (name)

	output: 	array sign with pointer of sign ???
			number sign in (number) variable
*/
void what_sign(char *name, byte *sign, byte *number)
{
byte i;

	for (*number=0, i=0; i<(byte)strlen(name); i++)
		if (name[i] == '?')
			{
			*(sign+*number) = i;
			(*number)++;
			}
}

/*
	divide full string (path+name of file) to path & name

	input: string with path & file name in variable *full_name

	output:	file name in variable *name
		   	path in variable *path
*/
void path_name(char *full_name, char *path, char *name)
{
char *t_name;
int len;

	if (
		((t_name=strrchr(full_name, (int)'\\')) != NULL) ||
		((t_name=strrchr(full_name, (int)':')) != NULL)
	   )
		{
		len=(strlen(full_name)-strlen(t_name));
		strncpy(path, full_name, len);
		strcpy(path+len, "\\\0");
		strcpy(name, full_name+strlen(path));
		}
	else
		{
		strcpy(path, '\0');
		strcpy(name, full_name);
		}
}


int read_file(int number_c)
{
uint number;

	if (channel_s[number_c].length < 512)
		number = (uint)channel_s[number_c].length;
	else
		number = 512;

	if(fread(channel_s[number_c].buffer, 1, number, channel_s[number_c].file_tx) < number)
		{
		strcpy(text, ERROR_READ_FILE);
		return(FALSE);
		}
	else
		{
		channel_s[number_c].read = number;
		channel_s[number_c].offset = 0;
	 	return(TRUE);
		}
}

void tx_block(int number, int new_old)
{
int num;
byte state;

	if ((int)channel_s[number].offset >= (int)channel_s[number].read)
		{
		if (channel_s[number].length <= 0)
			{
			sprintf(text, "%s (%s%s)", FILE_TX_OK,
								  channel_s[number].file_tx_path,
								  channel_s[number].file_tx_name
				  );
			channel_s[number].state = SEND_EOT;
			channel_s[number].queue |= SHEDULE_TX_OK;
			fclose(channel_s[number].file_tx);
			}
		else
			{
			if (read_file(number) == FALSE)
				channel_s[number].state = SEND_EOT;
			}
		}

	if (channel_s[number].state != SEND_EOT)
		{
		if (new_old == NEW_NUMBER)
			{
			if(++channel_s[number].number > '9')
				channel_s[number].number = '0';
			}

		channel_s[number].temp_tx[0] = channel_s[number].number;

		if (channel_s[number].length <= 32)
			{
			num = (int)channel_s[number].length;
			state = ETX;			
			}
		else
			{
			num = 32;
			state = ETB;
			}

		channel_s[number].state = WAIT_TX_BLOCK;
		memcpy(&(channel_s[number].temp_tx[1]), channel_s[number].buffer+channel_s[number].offset, num);
		channel_s[number].many = num;
		num++;
		channel_s[number].temp_tx[num] = state;

/*!!!	*/
		ibm_es((byte *)(&(channel_s[number].temp_tx[0])), 0, &num);
		write_csum((byte *)(&(channel_s[number].temp_tx[0])), 0, &num);
		com_purge(number);
		send_block(num, number);
		}
}

void tx_id_block(int number)
{
int num;
char id_block[40];

	strcpy(id_block, id_my);
	strcat(id_block, id_remoute);
	/*strcat(strcat(id_block, id_my), id_remoute);*/

	num=0;
	channel_s[number].temp_tx[num++] = STX;
	channel_s[number].temp_tx[num++] = NONE;
	channel_s[number].temp_tx[num++] = ENQ;
	strcpy((char *)(&(channel_s[number].temp_tx[num])), id_block);
	num+=strlen(id_block);
	channel_s[number].temp_tx[num] = ETX;
	write_csum((byte *)(&(channel_s[number].temp_tx[0])), (int)1, &num);

	channel_s[number].state = WAIT_TX_ID_BLOCK;
	channel_s[number].offset = 0;


	com_purge(number);
	send_block(num, number);
}
		

void send_byte_state(int number, byte tx_b, int state)
{
	channel_s[number].offset = 0;
	channel_s[number].n_rx = 0;
	channel_s[number].temp_tx[0] = tx_b;
	com_purge(number);
	send_block(1, number);
	channel_s[number].state = state;
}

int calc_rx_csum(int number, int start, int end)
{
byte csum;
int i;

	for (csum=0, i=start; i<=end; i++)
		csum ^= channel_s[number].temp_rx[i];

	if (csum)
		return(FALSE);
	else
		return(TRUE);
}

void write_csum(byte *buf, int st, int *num)
{
byte csum;
int i;

	for (csum=0,i=st; i<=*num; i++)
		csum ^= *(buf+i);
	*(buf+i) = csum;
/*	printf(" number=%5i  csum=%2x all=%4x\n", i, *(buf+i), csum);*/
	*num=++i;
}

char ibm_es_tab[128] =
{
	0x61, 0x62, 0x77, 0x67, 0x64, 0x65, 0x76, 0x7a, 0x69, 0x6a, 0x6b, 0x6c,
/*        ¡     ¢     £     ¤     ¥     ¦     §     ¨     ©     ª     «   */
	0x6d, 0x6e, 0x6f, 0x70, 0x72, 0x73, 0x74, 0x75, 0x66, 0x68, 0x63, 0x7e,
/*  ¬     ­     ®     ¯     à     á     â     ã     ä     å     æ     ç   */
	0x7b, 0x7d, 0x78, 0x79, 0x78, 0x7c, 0x60, 0x71,
/*  è     é    ê-ì    ë     ì     í     î     ï                           */

	0x61, 0x62, 0x77, 0x67, 0x64, 0x65, 0x76, 0x7a, 0x69, 0x6a, 0x6b, 0x6c,
/*        ¡     ¢     £     ¤     ¥     ¦     §     ¨     ©     ª     «   */
	0x6d, 0x6e, 0x6f, 0x70, 0x72, 0x73, 0x74, 0x75, 0x66, 0x68, 0x63, 0x7e,
/*  ¬     ­     ®     ¯     à     á     â     ã     ä     å     æ     ç   */
	0x7b, 0x7d, 0x78, 0x79, 0x78, 0x7c, 0x60, 0x71,
/*  è     é    ê-ì    ë     ì     í     î     ï                           */

	
	0x61, 0x62, 0x77, 0x67, 0x64, 0x65, 0x76, 0x7a, 0x69, 0x6a, 0x6b, 0x6c,
/*        ¡     ¢     £     ¤     ¥     ¦     §     ¨     ©     ª     «   */
	0x6d, 0x6e, 0x6f, 0x70, 0x72, 0x73, 0x74, 0x75, 0x66, 0x68, 0x63, 0x7e,
/*  ¬     ­     ®     ¯     à     á     â     ã     ä     å     æ     ç   */
	0x7b, 0x7d, 0x78, 0x79, 0x78, 0x7c, 0x60, 0x71,
/*  è     é    ê-ì    ë     ì     í     î     ï                           */

	
	0x61, 0x62, 0x77, 0x67, 0x64, 0x65, 0x76, 0x7a, 0x69, 0x6a, 0x6b, 0x6c,
/*        ¡     ¢     £     ¤     ¥     ¦     §     ¨     ©     ª     «   */
	0x6d, 0x6e, 0x6f, 0x70, 0x72, 0x73, 0x74, 0x75, 0x66, 0x68, 0x63, 0x7e,
/*  ¬     ­     ®     ¯     à     á     â     ã     ä     å     æ     ç   */
	0x7b, 0x7d, 0x78, 0x79, 0x78, 0x7c, 0x60, 0x71,
/*  è     é    ê-ì    ë     ì     í     î     ï                           */

};

void ibm_es(byte *buf, int st, int *num)
{
int i;

	for (i=st; i<(*num); i++)
		{
		if ((*(buf+i)) >= (byte)0x80)
			*(buf+i) = ibm_es_tab[(*(buf+i))-0x80];
		}

/*	log_fprintf("\n%32s", buf+st);*/

/*	printf(" number=%5i  csum=%2x all=%4x\n", i, *(buf+i), csum);*/
	*num=i;
}

byte const es_ibm_tab[32] =
{
	0x9e, 0x80, 0x81, 0x96, 0x84, 0x85, 0x94, 0x83, 0x95, 0x88, 0x89, 0x8a,
	0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x9f, 0x90, 0x91, 0x92, 0x93, 0x86, 0x82,
	0x9c, 0x9b, 0x87, 0x98, 0x9d, 0x99, 0x97, 0x5f
};

void es_ibm(byte *buf, int *num)
{
int i;
	
	for (i=0; i<*num; i++)
		{
		if ((*(buf+i)) >= 0x60)
			*(buf+i) = es_ibm_tab[(*(buf+i))-0x60];
		
		else
		if ((*(buf+i)) == 0x01)
			*(buf+i) = 0x20;
		}
}

int log_fprintf(char *format, ...)
{
char *temp;
va_list marker;
int ret;

	if ((temp = malloc_e(256)) != NULL)
		{
		va_start(marker, format);
		ret = vsprintf(temp, format, marker);
		va_end(marker);
		
		if (ret)
			ret = log_printf(temp);
  
		free(temp);
		}
	strcpy(text, "\0");
	return ret;
}


int log_printf(char *text)
{

char *temp;
time_t ltime;
int ctl=TRUE;
char str_err[10];

	num_error=0;
	time(&ltime);

	temp=malloc_e(256);
	sprintf(temp, "\n%.5s %s",ctime(&ltime)+11, text);
	
	printf("%.79s", temp);
	if (number_debug == DEBUG_LARGE)
		prn_debug_string("%s", temp);

	if (ctl)
		{
		if ((log_file = fopen(log_name, "a+t")) == NULL)
			{
			log_fprintf("%s: %s", ERROR_OPEN_LOG_FILE, log_name);
			ctl=FALSE;
			}
		else
			{
			if (fwrite(temp, strlen(temp), 1, log_file) != 1)
				{
				log_file = NULL;
				printf("\n%s", ERROR_WRITE_LOG_FILE);
				ctl=FALSE;
				}
			else
				{
				fflush(log_file);
				fclose(log_file);
				}

			if (*(temp+13) == '(')
				{
				strncpy(str_err, temp+14, 3);
				num_error = atoi(str_err);
				}
			else
			if (*(temp+13+3) == '(')
				{
				strncpy(str_err, temp+14+3, 3);
				num_error = atoi(str_err);
				}


			}
		}

	strcpy(text, "\0");
	flag_log = TRUE;
	free(temp);
	return ctl;
}

int start_log(void)
{
char *temp;
time_t ltime;
int ctl=TRUE;

	tzset();
	time(&ltime);


	temp=malloc_e(256);
	sprintf(temp, "\n\n%s %s", SYSTEM_OPEN, ctime(&ltime));
	
	printf("%s", temp);
	if (fwrite(temp, strlen(temp), 1, log_file) != 1)
		{
		log_file = NULL;
		ctl = FALSE;
		}

	flag_log = TRUE;
	free(temp);
	return ctl;
}


char page_debug[] =
{

	"\n"
	"program status:"
	"\n"
	"\n"
	"START_TX_ID_BLOCK  0x01      WAIT_STX                 0x20\n"
	"WAIT_TX_ID_BLOCK   0x02      WAIT_RX_ID_BLOCK         0x21\n"
	"END_TX_ID_BLOCK    0x03      WAIT_RX_CSUM_ID_BLOCK    0x22\n"
	"START_TX_BLOCK     0x04      WAIT_START_RX_BLOCK      0x23\n"
	"WAIT_TX_BLOCK      0x05      WAIT_RX_BLOCK            0x24\n"
	"WAIT_ACK_TX_BLOCK  0x06      WAIT_RX_CSUM_BLOCK       0x25\n"
	"CALL_ACK           0x07      SEND_ACK_BLOCK           0x26\n"
	"SEND_EOT           0x08      SEND_NAK_WAIT_STX        0x27\n"
	"END_TX             0x09      SEND_ACK_WAIT_STX        0x28\n"
	"                             SEND_NAK_BLOCK           0x29\n"
	"                             SEND_NAK_BLOCK_1         0x2A\n"
	"                             WAIT_EOT                 0x2B\n"
	"                             AGAIN_ACK                0x2C\n"
	"Queue :\n"
	"\n"
	"WRITE_BLOCK        0x0001    /* write block on receiv's file    */\n"
	"CLOSE_RX_FILE      0x0002    /* close receive file              */\n"
	"COMMAND_RX_FILE    0x0004    /* receive file in command line    */\n"
	"COMMAND_TX_FILE    0x0008    /* transmitt file in command line  */\n"
	"COMMAND_LINE       0x0010    /* comand line present             */\n"
	"SHEDULE_RX_FILE    0x0020    /* sheduling receive file          */\n"
	"SHEDULE_TX_FILE    0x0040    /* sheduling transmitt file        */\n"
	"SHEDULE_RX_OK      0x0080    /* shedule's receive file OK       */\n"
	"SHEDULE_TX_OK      0x0100    /* shedule's transmitt file OK     */\n"
	"SHEDULE_RX_NOTOK   0x0200    /* shedule's receive file NOT OK   */\n"
	"SHEDULE_TX_NOTOK   0x0400    /* shedule's transmitt file NOT OK */\n"
	"SHEDULE_TX_SLEEP   0x0800    /* sleep until TX next file        */\n"
	"\n"
};

int start_debug(void)
{
char *temp;
time_t ltime;
int ctl=TRUE;

	tzset();
	time(&ltime);


	if (filelength(fileno(debug_file)) == 0l)
		fwrite(page_debug, sizeof(page_debug), 1, debug_file);

	temp=malloc_e(256);
	sprintf(temp, "\n\n%s %s", SYSTEM_OPEN, ctime(&ltime));
	
	if (fwrite(temp, strlen(temp), 1, debug_file) != 1)
		{
		debug_file = NULL;
		number_debug = 0;
		ctl = FALSE;
		}

	free(temp);
	return ctl;
}

void *malloc_e(uint mem)
{
void *p;

	if ((p=malloc(mem)) == NULL)
		{
		log_printf(MEMORY_ALLOCATE_ERROR);
		j_exit();
		exit(num_error);
		}
	else
		return p;
}

int system_h_m(void)
{
time_t ltime;
char *temp;
char tc[3];
int h,m;


	temp = malloc_e(100);

	time(&ltime);
	temp=ctime(&ltime);

	strncpy(tc, temp+11, 2); *(tc+2)='\0'; h=atoi(tc);
	strncpy(tc, temp+14, 2); *(tc+2)='\0'; m=atoi(tc);

	free(temp);

	return (h*60+m);
}

long system_h_m_s(void)
{
time_t ltime;
char *temp;
char tc[3];
int h,m,s;


	temp = malloc_e(100);

	time(&ltime);
	temp=ctime(&ltime);
	h=atoi(strcat(strncpy(tc, temp+11, 2), '\0'));
	m=atoi(strcat(strncpy(tc, temp+14, 2), '\0'));
	s=atoi(strcat(strncpy(tc, temp+17, 2), '\0'));

	free(temp);

	return ((h*60+m)*60+s);


}

int system_s(void)
{
time_t ltime;
char *temp;
char tc[3];
int s;


	temp = malloc_e(100);

	time(&ltime);
	temp=ctime(&ltime);
	s=atoi(strcat(strncpy(tc, temp+17, 2), '\0'));

	free(temp);

	return (s);


}

/*
	print: \n, time & format string in debug file
*/
void prn_debug_time(char *format, ...)
{
time_t ltime;
char *temp, *temp1;
va_list marker;
int ret;

	time(&ltime);

	if ( ((temp = malloc_e(256)) != NULL) && ((temp1 = malloc_e(256)) != NULL) )
		{
		va_start(marker, format);
		ret = vsprintf(temp, format, marker);
		va_end(marker);
		
		if (ret)
			{
			sprintf(temp1, "\n%.8s %s", ctime(&ltime)+11, temp);
			strcat(debug_string, temp1);
			}
		free(temp1);
		free(temp);
		}
}

/*
	print format string in debug file
*/
void prn_debug_string(char *format, ...)
{
char *temp;
va_list marker;
int ret;

	if ((temp = malloc_e(256)) != NULL)
		{
		va_start(marker, format);
		ret = vsprintf(temp, format, marker);
		va_end(marker);
		
		if (ret)
			{
			if (fwrite(temp, strlen(temp), 1, debug_file) != 1)
				log_fprintf("\n%s", ERROR_WRITE_DEBUG_FILE);
			}
		free(temp);
		}
	strcpy(debug_string, "\0");
}

/*
	print NMBER of bytes START of buffer to debug file
	if (string != NULL) print string at start position
*/
void prn_debug_bytes(byte *start, int number, byte *string)
{
int i;
byte temp[10];
byte *sp;

	if ( (sp=malloc_e(number*4)) != NULL)	/* print byte as ASCII symbols in hex format */
		{

		strcpy(sp, "\0");
		for (i=0; i<number; i++)
			{
			sprintf(temp, "%.2X ", *(start+i));
			strcat(sp, temp);
			}

		if (strcmp(string, "") != 0)
			strcat(debug_string, string);

		strcat(debug_string, sp);
		free(sp);
		}
}


void print_debug(int number_c)
{

	if (flag_log)
		{
		printf("\n");
		flag_log = FALSE;
		}

	if (	timer1 != *(channel_s[number_c].timer) ||
		timer2 != *(channel_s[number_c].sh_timer_rx) ||
		timer3 != *(channel_s[number_c].sh_timer_tx)
		)
			{
			printf("\rttt=%07.lu trx=%02u:%02u:%02u ttx=%02u:%02u:%02u q=%04X  s=%04x",
					timer1=*(channel_s[number_c].timer),
					(uint)((timer2=*(channel_s[number_c].sh_timer_rx)) / (60*60l)),
					(uint)((*(channel_s[number_c].sh_timer_rx) % (60*60l)) / 60),
					(uint)((*(channel_s[number_c].sh_timer_rx) % (60*60l)) % 60),
					(uint)((timer3=*(channel_s[number_c].sh_timer_tx)) / (60*60l)),
					(uint)((*(channel_s[number_c].sh_timer_tx) % (60*60l)) / 60),
					(uint)((*(channel_s[number_c].sh_timer_tx) % (60*60l)) % 60),
					channel_s[number_c].queue,
					channel_s[number_c].state
				);

/*			printf("\ntimer=%li", *channel_s[number_c].timer);*/
			}
}

int print_modem_status(int can_nl)
{
int ctl=TRUE;

	modem_status(can_nl);
	if ((channel_s[can_nl].modem_state & MODEM_DCD) == 0)
		{
		log_printf(MODEM_DCD_ERROR);
		ctl=FALSE;
		}

	if ((channel_s[can_nl].modem_state & MODEM_DSR) == 0)
		{
		log_printf(MODEM_DSR_ERROR);
		ctl=FALSE;
		}
	
	if ((channel_s[can_nl].modem_state & MODEM_CTS) == 0)
		{
		log_printf(MODEM_CTS_ERROR);
		ctl=FALSE;
		}

	if (ctl)
		log_printf(MODEM_READY);
	return ctl;
}

void screen_char_in(byte ch)
{
/*
int rowcol;
	rowcol=_GetCurPos();
	_SetCurPos(debug_in_cur_row, debug_in_cur_col);


	if (ch < 0x20)
		_PutChar('.');
	else
		_PutChar(ch);

	_SetCurPos((rowcol>>8)&0x00ff, rowcol&0x00ff);
*/


	*pcin = (ch < 0x20)? (byte)'.':ch;
	if (++pcin >= pcinend)
		pcin=&cin[0];
	cinnum++;

}

void screen_char_out(byte ch)
{
/*
int rowcol;
	
	rowcol=_GetCurPos();
	_SetCurPos(debug_out_cur_row, debug_out_cur_col);

	if (ch < 0x20)
		_PutChar('.');
	else
		_PutChar(ch);

	_SetCurPos((rowcol>>8)&0x00ff, rowcol&0x00ff);
*/


	*pcout = (ch < 0x20)? (byte)'.':ch;
	if (++pcout >= pcoutend)
		pcout=&cout[0];
	coutnum++;

}

void print_ch_scr(void)
{
int rowcol;
int t;
	if (cinnum != cintemp)
		{
		rowcol=_GetCurPos();
		_SetCurPos(debug_in_cur_row, debug_in_cur_col);

/*		_PutChar(*pcin);*/

		t=((t=(cinnum & 0x0f)) < 10)? t+'0':t-10+'A';
		_PutChar((char)t);
		_SetCurPos(debug_in_cur_row, debug_in_cur_col-1);
		t=((t=((cinnum & 0xf0)>>4)) < 10)? t+'0':t-10+'A';
		_PutChar((char)t);

		cintemp=cinnum;


		if (++psin == pcinend)
			psin=&cin[0];

/*		--cinnum;*/

		_SetCurPos((rowcol>>8)&0x00ff, rowcol&0x00ff);
		}

	if (coutnum != couttemp)
		{
		rowcol=_GetCurPos();
		_SetCurPos(debug_out_cur_row, debug_out_cur_col);

		
/*		_PutChar(*pcout);*/

		t=((t=(coutnum & 0x0f)) < 10)? t+'0':t-10+'A';
		_PutChar((char)t);
		_SetCurPos(debug_out_cur_row, debug_out_cur_col-1);
		t=((t=((coutnum & 0xf0)>>4)) < 10)? t+'0':t-10+'A';
		_PutChar((char)t);

		
		couttemp=coutnum;				
		
		
		if (++psout == pcoutend)
			psout=&cout[0];
		
		
/*		--coutnum;*/
		
		
		_SetCurPos((rowcol>>8)&0x00ff, rowcol&0x00ff);
		}
}

