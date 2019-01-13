/*
	Vitaly Lev	
				
	<C> <Lion>	

	70_ctl.c

	Programm for communication IBM-XT <-> IBM

	Parse CTL File

	930608	V1.0

!!! set_shedule - for 0 channel only

*/

#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <ctype.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <malloc.h>
#include <time.h>
#include <dos.h>
#include "70_def.h"

struct parse_list {
        int p_length;
        char *p_string;
        };
/*
			Local functions prototype
*/
int	parse_ctl(int);
int	com_str_analize(int, char *[]);
void	errxit (char *);
char	*fancy_str (char *);
char	*add_backslash (char *);
char	*delete_backslash (char *);
char	*ctl_string (char *);
char	*ctl_slash_string (char *);
char	*skip_blanks (char *);
int 	parse_config (char *, int);
void	b_initvars (void);
void	b_defaultvars (void);
int 	parse (char *, struct parse_list *);
int 	verify_time_tx(int,	int, int,	int, int,	int,	int, int, int);
int 	verify_time_rx(int,	int, int, int, int, int, int);
int	verify_shedule(char, int *, int *);
int	set_shedule(char);
int	activate_sh(int, int, int);

/*
			External functions prototype
*/
extern	int start_log(void);
extern	int start_debug(void);
extern	int log_printf(char *);
extern	int log_fprintf(char *, ...);
extern	int open_rx(char *, int);
extern	int tx_file(char *, int);
extern	void *malloc_e(uint);
extern	int open_tx(char *, int);
extern	path_name(char *, char *, char *);
extern	int system_h_m(void);


/*
			Global variable
*/

/*
			External variable prototype
*/
extern	int num_c;
extern	struct bl channel_s[];
extern	char text[];
extern	FILE *log_file;
extern	char log_name[];
extern	char	*config_name;
extern	char	*path70;
extern	char *fn_tx;
extern	char *fn_rx;
extern	int	work_c;
extern	long temp_timer;
extern	char *fn_rx_temp;
extern	int	flag_debug;
extern	int	flag_debug_i_o;
extern	FILE *debug_file;
extern	char	debug_name[];
extern	int	number_debug;
extern	int	debug_in_cur_row;
extern	int	debug_in_cur_col;
extern	int	debug_out_cur_row;
extern	int	debug_out_cur_col;
extern	int	flag_computer;
extern	char *id_remoute;
extern	char	*id_my;
extern	num_error;

int parse_ctl(int argc)
{
char *envptr;
struct stat statbuf;
int state;

	b_initvars ();

	envptr=malloc_e(100);
	
	envptr = getenv ("AP70");
										/* get path from environment */

	if ((envptr != NULL)                         /* If there was one, and     */
   	&& (stat (config_name,&statbuf) != 0))  /* No 70.CFG locally,   */
   	{
   	path70 = malloc (strlen (envptr) + 2);  /* make room for new */
   	strcpy (path70, envptr);                /* use 70 as our path   */

   	add_backslash (path70);
   	}

	state=parse_config(config_name, argc);

	free(envptr);
	b_defaultvars ();

	return (state);

}

/**
 ** b_initvars -- called before parse_config. Sets defaults that we want
 ** to have set FIRST.
 **/
void b_initvars (void)
{
int i=0;

	for (; i<num_c; i++)
		{
		channel_s[i].sh_n = 0;
		channel_s[i].ch.ch_active = -1;
		}
}

/**
 ** b_defaultvars -- called after all parse_config passes complete.
 ** sets anything not handled by parse_config to default if we know it.
 **/


void b_defaultvars ()
{
	;
}


struct parse_list config_lines[] = {
							   {15, "number_channels"}, 	/*1*/
                                    {7,  "channel"},			/*2*/
                                    {10, "shedule_tx"},		/*3*/
							   {10, "shedule_rx"},		/*4*/
							   {8,  "log_file"},		/*5*/
							   {10, "debug_info"},		/*6*/
							   {10, "debug_file"},		/*7*/
							   {12, "debug_format"},   	/*8*/
							   {9,  "debug_i_o"},		/*9*/
							   {16, "debug_in_cur_pos"},	/*10*/
							   {17, "debug_out_cur_pos"},	/*11*/
							   {10, "id_remoute"},		/*12*/
							   {5,  "id_my"},			/*13*/   
                                    {0,  NULL}
};


int parse_config (char *config_file, int argc)
{
FILE *stream;
char *temp;
char *path;
char *temp_str;
char *move_path;
char *c;
char *f_name;
char *f_path;
int i;
int str_num=0;
int num_c_local;
int ctl=TRUE;
int ctl1=TRUE;
int str_min;
int str_hour;
int end_min;
int end_hour;
int int_min;
int int_hour;
int sh_n_local=0;	/* shedules number */
int first, second;/* error's number string in shedule */
struct stat b_stat;
struct ch_s temp_ch;

	temp_str=malloc_e(100);
	temp=malloc_e(256);
	path=malloc_e(100);
	move_path=malloc_e(100);
	f_path=malloc_e(100);
	f_name=malloc_e(100);

	if (path70 != NULL)
   	sprintf (temp, "%s%s", path70, config_file);
	else
   	strcpy (temp, config_file);

	if ((stream = fopen (temp, "rt")) == NULL)    /* OK, let's open the file   */
		{
		printf("\n\n%s: %s\n", ERROR_OPEN_CONFIG_FILE, temp);
		return(FALSE);                           /* no file, no work to do    */
		}

	while (((fgets (temp, 255, stream)) != NULL))   /* Now we parse the file ... */
		{
		str_num++;
		c = temp;                                  /* Check out the first char  */
		if ((*c == '%') || (*c == ';'))            /* See if it's a comment
                                           		* line */
   		continue;

		i = strlen (temp);                         /* how long this line is     */

		if (i < 4)
   		continue;                               /* If too short, ignore it   */

		c = &temp[--i];                            /* point at last character   */
		if (*c == '\n')                            /* if it's a newline,        */
   		*c = '\0';                              /* strip it off              */

		switch (parse (temp, config_lines))
			{
			case 1:                                /* "number_channels"	*/
				c = skip_blanks (&temp[15]);
				num_c = atoi (c);
				if (num_c <= 0  || num_c > channel)
					{
					log_fprintf("%s %s", UNKNOWN_CHANNEL_NUMBER, &temp[0]);
					ctl = FALSE;
					}
				break;

			case 2:                                /* "channels"        */
				c = skip_blanks (&temp[7]);

				i =sscanf (c, "%i %x %i %d %d %c %i",
                  			&num_c_local,
							&temp_ch.comn,
							&temp_ch.interrupt_n,
							&temp_ch.speed,
							&temp_ch.bits,
							&temp_ch.parity,
							&temp_ch.stop);
				if (i < 7)
					{
					log_fprintf("%s %s %i\n", CHANNEL_NUMBER_PAR_ERROR, STRING, str_num);
					ctl = FALSE;			
					}
				else
					{
					num_c_local--;
					if (num_c!=0)
						{
						if (temp_ch.comn>=1 && temp_ch.comn<=4)
							{
							channel_s[num_c_local].ch.comn = --temp_ch.comn;
							}
						else
							{
							log_fprintf("%s %s %i\n", NUMBER_COM_PORT_ERROR, STRING, str_num);
							ctl = FALSE;			
							}
						channel_s[num_c_local].ch.interrupt_n = temp_ch.interrupt_n;
						
						i = temp_ch.speed;
						if ((i==300)  ||
						    (i==600)  ||
						    (i==1200) ||
						    (i==2400) ||
						    (i==4800) ||
						    (i==9600))
							channel_s[num_c_local].ch.speed = i;
						else
							{
							log_fprintf("%s %s %i\n", CHANNEL_SPEED_ERROR, STRING, str_num);
							ctl = FALSE;			
							}

						if ((temp_ch.bits < 7) || (temp_ch.bits > 8))
							{
							log_fprintf("%s %s %i\n", CHANNEL_BIT_ERROR, STRING, str_num);
							ctl = FALSE;			
							}
						else
							channel_s[num_c_local].ch.bits = temp_ch.bits;

						if ((temp_ch.parity=='n') ||
						    (temp_ch.parity=='o') ||
						    (temp_ch.parity=='e'))
							channel_s[num_c_local].ch.parity = temp_ch.parity;
						else
							{
							log_fprintf("%s %s %i\n", CHANNEL_PARITY_ERROR, STRING, str_num);
							ctl = FALSE;			
							}

						if ((temp_ch.stop>=0) && (temp_ch.stop<=2))
							channel_s[num_c_local].ch.stop = temp_ch.stop;
						else
							{
							log_fprintf("%s %s %i\n", CHANNEL_STOP_ERROR, STRING, str_num);
							ctl = FALSE;			
							}
						}
					else
						{
						log_fprintf("%s %s %i\n", CHANNEL_NUMBER_ERROR, STRING, str_num);
						ctl = FALSE;			
						}
					}

				if (ctl)
					channel_s[num_c_local].ch.ch_active = 0;
				break;


			case 3:                                /* "shedule_tx"        */
				if (argc == 1)
					{
					c = skip_blanks (&temp[10]);
 					i = sscanf (c, "%i %d:%d %d:%d %d:%d %s %s",
                  				&num_c_local,
								&str_hour,
								&str_min,
								&end_hour,
								&end_min,
								&int_hour,
								&int_min,
								path,
								move_path);
					if (i < 8)
						{
						log_fprintf("%s %s %i\n", NUMBER_SHEDULE_PAR_ERROR, STRING, str_num);
						ctl = FALSE;			
						}
					else
						{
						if ((num_c!=0) && (num_c_local<=num_c) && (num_c_local>0))
							{
							num_c_local--;
							channel_s[num_c_local].sh[sh_n_local].tx_rx[0] = 't';
							if (verify_time_tx(str_num, num_c_local, sh_n_local, str_hour, str_min, end_hour, end_min, int_hour, int_min))
								{
								path_name(path, f_path, f_name);

								if (!stat(delete_backslash(f_path), &b_stat))
									{
									strcpy(&(channel_s[num_c_local].sh[sh_n_local].path[0]), path);
									channel_s[num_c_local].sh[sh_n_local].active = NOACTIVE;
									}
								else
									{
									log_fprintf("%s (%s) /%s %i/", SHEDULE_PATH_ERROR, f_path, STRING, str_num);
									ctl = FALSE;
									}

								if (ctl)
									{
									if (i == 9)
										{
										if (!stat(delete_backslash(move_path), &b_stat))
											strcpy(&(channel_s[num_c_local].sh[sh_n_local].move_path[0]), move_path);
										else
											{
											log_fprintf("%s /%s %i/", SHEDULE_MOVE_PATH_ERROR, STRING, str_num);
											ctl=FALSE;
											}
										}
									else
										{
										log_fprintf("%s /%s %i/", SHEDULE_MOVE_PATH_NOT, STRING, str_num);
										log_fprintf("%s %02i:%02i %02i:%02i %s", FILES_BETWEEN,
																		 str_hour, str_min,
																		 end_hour, end_min,
																		 WILL_DELETE
												 );
										strcpy(&(channel_s[num_c_local].sh[sh_n_local].move_path[0]), "\0");
										}
									}
								}
							else
								ctl = FALSE;
							}
						else
							{
							log_fprintf("%s %s %i", CHANNEL_NUMBER_ERROR, STRING, str_num);
							ctl = FALSE;
							}
						}

					if (ctl)
						{
						channel_s[num_c_local].sh_n++;
						sh_n_local++;
						}
					else
						log_fprintf("%s: /%s/ %s", SHEDULE, c, EXCLUDE_FROM_SHEDULE);
					}
				break;

			case 4:                                /* "shedule_rx"        */
				if (argc == 1)
					{
					c = skip_blanks (&temp[10]);
 					i = sscanf (c, "%i %d:%d %d:%d %s",
                  				&num_c_local,
								&str_hour,
								&str_min,
								&end_hour,
								&end_min,
								&path[0]);
					if (i < 6)
						{
						log_fprintf("%s %s %i", NUMBER_SHEDULE_PAR_ERROR, STRING, str_num);
						ctl = FALSE;			
						}
					else
						{
						if ((num_c!=0) && (num_c_local<=num_c) && (num_c_local>0))
							{
							num_c_local--;
		    					channel_s[num_c_local].sh[sh_n_local].tx_rx[0] = 'r';
							if (verify_time_rx(str_num, num_c_local, sh_n_local, str_hour, str_min, end_hour, end_min))
								{
								path_name(path, f_path, f_name);

								if (!stat(delete_backslash(f_path), &b_stat))
									{
									strcpy(&(channel_s[num_c_local].sh[sh_n_local].path[0]), path);
									channel_s[num_c_local].sh[sh_n_local].active = NOACTIVE;
									}
								else
									{
									log_fprintf("%s (%s) /%s %i/", SHEDULE_PATH_ERROR, f_path, STRING, str_num);
									ctl = FALSE;
									}
								}
							else
								ctl = FALSE;
							}
						else
							{
							log_fprintf("%s %s %i", CHANNEL_NUMBER_ERROR, STRING, str_num);
							ctl = FALSE;
							}
						}

					if (ctl)
						{
						channel_s[num_c_local].sh_n++;
						sh_n_local++;
						}
					else
						log_fprintf("%s: /%s/ %s", SHEDULE, c, EXCLUDE_FROM_SHEDULE);
					}
				break;

			case 5:		/* log_file */
				c = skip_blanks (&temp[8]);
				strcpy(log_name, path70);
				strcat(log_name, c);

				if ((log_file = fopen (log_name, "a+t")) == NULL)    /* OK, let's open the file   */
					{
					log_fprintf("%s: %s", ERROR_OPEN_LOG_FILE, log_name);
					ctl=FALSE;
					}
				else
					{
					if (start_log() == FALSE)
						{
						printf("%s", ERROR_WRITE_LOG_FILE);
						ctl=FALSE;
						}
					fflush(log_file);
					fclose(log_file);
					}
				break;

			case 6:                                /* "debug_info" */
				c = skip_blanks (&temp[10]);
				strcpy(temp_str, c);

				if (stricmp(temp_str, "on") == 0)
					flag_debug = TRUE;
				else
				if (stricmp(temp_str, "off") == 0)
					flag_debug = FALSE;
				else
					log_fprintf("%s %s %i", DEBUG_CTL_COMMAND_ERROR, STRING, str_num);
				break;
				
			case 7:							/* debug_file */
				c = skip_blanks (&temp[10]);
				strcpy(debug_name, path70);
				strcat(debug_name, c);

				if ((debug_file = fopen (debug_name, "a+t")) == NULL)    /* OK, let's open the file   */
					{
					log_fprintf("%s: %s", ERROR_OPEN_DEBUG_FILE, debug_name);
					ctl=FALSE;
					}
				else
					{
					if (start_debug() == FALSE)
						{
						printf("%s", ERROR_WRITE_DEBUG_FILE);
						ctl=FALSE;
						}
					}
				break;

			case 8:                                /* "debug_format" */
				c = skip_blanks (&temp[12]);
				number_debug = atoi (c);
				if (num_c < 0  || number_debug > DEBUG_MAX_NUMBER)
					{
					log_fprintf("%s %s %s 0-%i", UNKNOWN_DEBUG_FILE_NUMBER, &temp[0], WARIANTS, DEBUG_MAX_NUMBER);
					ctl = FALSE;
					}
				break;

			case 9:                                /* "debug_i_o" */
				c = skip_blanks (&temp[10]);
				strcpy(temp_str, c);

				if (stricmp(temp_str, "on") == 0)
					flag_debug_i_o = TRUE;
				else
				if (stricmp(temp_str, "off") == 0)
					flag_debug_i_o = FALSE;
				else
					log_fprintf("%s %s %i", DEBUG_CTL_COMMAND_ERROR, STRING, str_num);
				break;

			case 10:							/* "debug_in_cur_pos" */

				c = skip_blanks (&temp[16]);
				sscanf(c, "%i %i", &debug_in_cur_row, &debug_in_cur_col);

				if (debug_in_cur_row<0 || debug_in_cur_row>DEBUG_CUR_ROW_MAX)
					{
					log_fprintf("%s %s %s 0-%i", WRONG_DEBUG_CUR_ROW, &temp[0], WARIANTS, DEBUG_CUR_ROW_MAX);
					debug_in_cur_row = 24;
					ctl = FALSE;
					}

				if (debug_in_cur_col<0 || debug_in_cur_col>DEBUG_CUR_COL_MAX)
					{
					log_fprintf("%s %s %s 0-%i", WRONG_DEBUG_CUR_COL, &temp[0], WARIANTS, DEBUG_CUR_COL_MAX);
					debug_in_cur_col=78;
					ctl = FALSE;
					}
  
				break;

			case 11:							/* "debug_out_cur_pos" */

				c = skip_blanks (&temp[17]);
				sscanf(c, "%i %i", &debug_out_cur_row, &debug_out_cur_col);

				if (debug_out_cur_row<0 || debug_out_cur_row>DEBUG_CUR_ROW_MAX)
					{
					log_fprintf("%s %s %s 0-%i", WRONG_DEBUG_CUR_ROW, &temp[0], WARIANTS, DEBUG_CUR_ROW_MAX);
					debug_out_cur_row = 24;
					ctl = FALSE;
					}

				if (debug_out_cur_col<0 || debug_out_cur_col>DEBUG_CUR_COL_MAX)
					{
					log_fprintf("%s %s %s 0-%i", WRONG_DEBUG_CUR_COL, &temp[0], WARIANTS, DEBUG_CUR_COL_MAX);
					debug_out_cur_col=78;
					ctl = FALSE;
					}
  
				break;

			case 12:							/* "id_remoute" */
				c = skip_blanks (&temp[10]);
				strcpy(id_remoute, c);
				break;

			case 13:							/* "id_my" */
				c = skip_blanks (&temp[5]);
				strcpy(id_my, c);
				break;

			default:
				log_fprintf("%s %s %i", UNKNOWN_CTL_COMMAND, STRING, str_num);
				break;
        	}

		if (log_file == NULL)
			break;

		if (!ctl)
			ctl1=FALSE;

		ctl = TRUE;
		}

	if (ctl1 && argc == 1)
		{
			ctl=verify_shedule('t', &first, &second);
			if (!ctl)
				{
				log_fprintf("%s. %s %i & %i", ERROR_SHEDULE_OVERRIDE_TX,
										SHEDULE_NUMBER, first, second);
				}

			if (ctl)
				{
				ctl=verify_shedule('r', &first, &second);
				if (!ctl)
					{
					log_fprintf("%s. %s %i & %i", ERROR_SHEDULE_OVERRIDE_RX,
											SHEDULE_NUMBER, first, second);
					}
				}
			if (ctl)
				{
				set_shedule('r');
				set_shedule('t');
				}
		}

	fclose (stream);                              /* close input file          */

	free(f_name);
	free(f_path);
	free(move_path);
	free(path);
	free(temp);
	free(temp_str);
	
	return(ctl & ctl1);
}

struct parse_list comand_line[] = {
							   {4, "send"},
                                    {7, "receive"},
                                    {10,"rx_timeout"},
							   {7, "channel"},
                                    {0,  NULL}
};

int com_str_analize (int argc, char **argv)
{
int ctl=TRUE;
long rx_timeout=RX_TIMEOUT_DEFAULT;


	strcpy(fn_rx, "\0");
	strcpy(fn_tx, "\0");
	strcpy(text, "\0");

	while (--argc)
		{
		++argv;
		switch (parse (argv[0], comand_line))
			{
			case 1:		/* send */
					++argv;
					--argc;
					strcpy(fn_tx, argv[0]);
					if (strchr(fn_tx, (int)'*') != NULL)
						sprintf(text, "%s (%s)", ERROR_STAR);
					break;

			case 2:		/* receive */
					++argv;
					--argc;
					strcpy(fn_rx, argv[0]);
					if (strchr(fn_rx, (int)'*') != NULL)
						sprintf(text, "%s (%s)", ERROR_STAR);
					break;

			case 3:		/* rx_timeout */
					++argv;
					--argc;
					rx_timeout = atol (argv[0]);
					if (rx_timeout<RX_TIMEOUT_MIN || rx_timeout>RX_TIMEOUT_MAX)
						sprintf(text, "%s\n", ERROR_RX_TIMEOUT);
					break;

			case 4:		/* channel */
					++argv;
					--argc;
					work_c = atoi (argv[0]);
					if (work_c < 1 || work_c > num_c)
						sprintf(text, "%s %s\n", CHANNEL_NUMBER_ERROR, argv[0]);
					else
						--work_c;
					break;

			default:
					sprintf(text, "%s %s\n", ERROR_COMMAND_LINE, argv[0]);
					break;
			}

		if (strcmp(text, "\0") != 0)
			{
			log_printf(text);
			argc=1;
			ctl = FALSE;
			}
		}

	if (ctl)
		{
		if (strcmp (fn_rx, "\0") != 0)
			{
			if (open_rx(fn_rx,work_c))
				{
				channel_s[work_c].queue |= COMMAND_RX_FILE;
				channel_s[work_c].queue |= COMMAND_LINE;
				}
			else
				ctl = FALSE;
			}

		if (strcmp (fn_tx, "\0") != 0) 
			{
			if (open_tx(fn_tx,work_c))
				{
				channel_s[work_c].queue |= COMMAND_TX_FILE;
				channel_s[work_c].queue |= COMMAND_LINE;
				}
			else
				ctl = FALSE;
			}
		
		*(channel_s[work_c].sh_timer_rx) = rx_timeout;
		temp_timer = rx_timeout;
		}
	
	return ctl;
}

/*
	verify shedule for search override
*/
int verify_shedule(char tx_rx, int *first, int *second)
{
int i, j, k;
int c_str_hm=24*60; /* max time (out of range) */
int c_end_hm=24*60; /* max time (out of range) */
int str_hm;
int end_hm;
int flag;

	for (i=0; i<num_c; i++)
		{
		for (k=0; k<channel_s[i].sh_n; k++)
			{
			flag=TRUE;

			for (j=k; j<channel_s[i].sh_n; j++)
				{
				if (channel_s[i].sh[j].tx_rx[0] == tx_rx)
					{
					str_hm = channel_s[i].sh[j].str_hm;
					end_hm = channel_s[i].sh[j].end_hm;
		
					if (flag)
						{
						c_str_hm = str_hm;
						c_end_hm = end_hm;
						flag = FALSE;
						}
					else
						{
						if ((c_str_hm == str_hm) ||
				    		    ((str_hm > c_str_hm) && (str_hm < c_end_hm)) ||
				    		    ((end_hm > c_str_hm) && (end_hm < c_end_hm))
				   		   )
							{
							*first = ++k;
							*second = ++j;
							return (FALSE);
							}
						}/* if flag */

					} /* if tx_rx */

				} /* for j */

			} /* for k */
		}

	return TRUE;
}


int set_shedule(char tx_rx)
{
int ctl=TRUE;
int sh_num=0;
int flag=FALSE;
int num_cl=0;
int h_m_n;
int h_m;

	h_m=system_h_m();
	


	while ((channel_s[num_cl].sh_n > sh_num) && ctl &&(!flag))
		{

		if ((channel_s[num_cl].sh[sh_num].tx_rx[0] == tx_rx) &&
			(channel_s[num_cl].sh[sh_num].str_hm <= h_m)     &&
			(channel_s[num_cl].sh[sh_num].end_hm >  h_m)
			)
			
			{
			ctl=activate_sh(num_cl, sh_num, h_m);
			if (ctl)
				{
				
				if (tx_rx == 't')
					*(channel_s[num_cl].sh_timer_tx) = (long)((long)(channel_s[num_cl].sh[sh_num].end_hm - h_m)*60l);
				else
					{
					*(channel_s[num_cl].sh_timer_rx) = (long)((long)(channel_s[num_cl].sh[sh_num].end_hm - h_m)*60l);
					}
				
				flag = TRUE;
				}
			}
		sh_num++;
		}
		sh_num--;

		if (tx_rx == 't')
			{
			if (flag) /* file for TX open o'key */
				{
				log_fprintf("%s (%s%s)",	TX_FILE,
									channel_s[num_cl].file_tx_path,
									channel_s[num_cl].file_tx_name);
				}
			else
				{
				if (channel_s[num_cl].sh[sh_num].active == ACTIVE) /* interval > 0 */
					{
					*(channel_s[num_cl].sh_timer_tx) = (long)((long)(channel_s[num_cl].sh[sh_num].int_hm)*60l);
					channel_s[num_cl].queue |= SHEDULE_TX_SLEEP;
					}
				else
					{
					log_printf(NO_TX_FILES);
					h_m_n=24*60;
					sh_num=0;

					while (channel_s[num_cl].sh_n > sh_num)
						{
						if ((channel_s[num_cl].sh[sh_num].tx_rx[0] == 't') &&
			    		    	(channel_s[num_cl].sh[sh_num].str_hm > h_m)
			   		   	)
							{
							if (channel_s[num_cl].sh[sh_num].str_hm < h_m_n)
								h_m_n = channel_s[num_cl].sh[sh_num].str_hm;
							}
						sh_num++;
						}

					*(channel_s[num_cl].sh_timer_tx) = (long)((long)(h_m_n-h_m)*60l);

					if (h_m_n == 24*60)
						log_printf(NO_SHEDULE_TX);
					else
						{
						channel_s[num_cl].queue |= SHEDULE_TX_SLEEP;
						log_fprintf("%s : %02i:%02i", NEXT_SHEDULE_TX, h_m_n/60, h_m_n % 60);
						}

					}
				}
			}
		else	/* if tx_rx = 'r' */
			{
			if (!flag) /* file for RX don't open */
				{
				open_rx(fn_rx_temp, num_cl);
				h_m_n=24*60;
				sh_num=0;

				while (channel_s[num_cl].sh_n > sh_num)
					{
					if ((channel_s[num_cl].sh[sh_num].tx_rx[0] == 'r') &&
			    		    (channel_s[num_cl].sh[sh_num].str_hm > h_m)
			   		   )
						{
						if (channel_s[num_cl].sh[sh_num].str_hm < h_m_n)
							h_m_n = channel_s[num_cl].sh[sh_num].str_hm;
						}
					sh_num++;
					}

				*(channel_s[num_cl].sh_timer_rx) = (long)((long)(h_m_n-h_m)*60l);
				if (h_m_n == 24*60)
					log_printf(NO_SHEDULE_RX);
				else
					log_fprintf("%s : %02i:%02i", NEXT_SHEDULE_RX, h_m_n/60, h_m_n % 60);

				}
			}

	return ctl;
}

int activate_sh(int num_cl, int sh_n, int h_m)
{
int ctl;

	if (channel_s[num_cl].sh[sh_n].tx_rx[0] == 't')
		{
		fclose(channel_s[num_cl].file_tx);
		strcpy(fn_tx, channel_s[num_cl].sh[sh_n].path);
		
		if((ctl=open_tx(fn_tx, num_cl)) == TRUE)
			{
			channel_s[num_cl].queue |= SHEDULE_TX_FILE;
			channel_s[num_cl].sh[sh_n].active = ACTIVE;
			channel_s[num_cl].tx_sh_active = sh_n;
			}
		else
			{

/*			printf("\ntime=%i\n", channel_s[num_cl].sh[sh_n].int_hm);*/

			if (channel_s[num_cl].sh[sh_n].int_hm == 0)
				{
				channel_s[num_cl].sh[sh_n].active = NOACTIVE;
				}

			else
/*!!!*/		if (channel_s[num_cl].sh[sh_n].int_hm + h_m < channel_s[num_cl].sh[sh_n].end_hm)
				{
				channel_s[num_cl].sh[sh_n].active = ACTIVE;
				channel_s[num_cl].tx_sh_active = sh_n;
				}
			}

		}
	else /* tx_rx = 'r' */
		{
		fclose(channel_s[num_cl].file_rx);
		strcpy(fn_rx, channel_s[num_cl].sh[sh_n].path);
		ctl=open_rx(fn_rx, num_cl);
		}

	return ctl;
}

int verify_time_tx(int str_num,	int num_c,
							int sh_n,
							int str_hour,
						  	int str_min,
						  	int end_hour,
						  	int end_min,
						  	int int_hour,
						  	int int_min)
{
int str_hm;
int end_hm;
int int_hm;
int all_t;

	str_hm=str_hour*60+str_min;
	end_hm=end_hour*60+end_min;
	int_hm=int_hour*60+int_min;

	if ( (str_hm > 60*23+59) ||
		(end_hm > 60*23+59) ||
		(int_hm > 60*23+59))
		{
		sprintf(text, "%s %s %i\n", TIME_2359_ERROR, STRING, str_num);
		return(FALSE);
		}
	else 
		{
		if ((end_hm-str_hm) < 0)
			{
			sprintf(text,"%s %s %i", ERROR_BIG_24, STRING, str_num);
			return(FALSE);
			}
		else
			{
			all_t=((end_hm-str_hm)>0)? end_hm-str_hm:str_hm-end_hm;
			if (int_hm >= all_t)
				{
				sprintf(text, "%s %s %i", INTERVAL_ERROR, STRING, str_num);
				return(FALSE);
				}
			}
		}

	channel_s[num_c].sh[sh_n].str_hm=str_hm;
	channel_s[num_c].sh[sh_n].end_hm=end_hm;
	channel_s[num_c].sh[sh_n].int_hm=int_hm;

	return(TRUE);
}

int verify_time_rx(int str_num,	int num_c,
							int sh_n,
						  	int str_hour,
						  	int str_min,
						  	int end_hour,
						  	int end_min)
{
int str_hm;
int end_hm;
	
	str_hm=str_hour*60+str_min;
	end_hm=end_hour*60+end_min;


	if ( (str_hm > 60*23+59) ||
		(end_hm > 60*23+59))
		{
		sprintf(text, "%s %s %i\n", TIME_2359_ERROR, STRING, str_num);
		return(FALSE);
		}

	channel_s[num_c].sh[sh_n].str_hm=str_hm;
	channel_s[num_c].sh[sh_n].end_hm=end_hm;

	return(TRUE);
}

int parse (char *input, struct parse_list list[])
{
	int i;

	for (i=0; list[i].p_length; i++)
   	{
		if (strnicmp (input, list[i].p_string, list[i].p_length) == 0)
        	return (++i);
   	}
	return (-1);
}

void errxit (char *error)
{
   printf ("\r\n%s\n", error);
   exit (0);
}

char *fancy_str (char *string)
{
int flag = 0;
char *s;

   s = string;

   while (*string)
      {
      if (isalpha (*string))                     /* If alphabetic,     */
         {
         if (flag)                               /* already saw one?   */
            *string = (char)tolower(*string);    /* Yes, lowercase it  */
         else
            {
            flag = 1;                            /* first one, flag it */
            *string = (char)toupper(*string);    /* Uppercase it       */
            }
         }
      else /* if not alphabetic  */ flag = 0;    /* reset alpha flag   */
      string++;
      }

   return (s);
}

char *add_backslash (char *str)
{
char *p;

   p = str + strlen (str) - 1;

   /* Strip off the trailing blanks */
   while ((p >= str) && (isspace (*p)))
      {
      *p = '\0';
      --p;
      }

   /* Put a backslash if there isn't one */
   if ((*p != '\\') && (*p != '/'))
      {
      *(++p) = '\\';
      *(++p) = '\0';
      }

   return (fancy_str (str));
}

char *delete_backslash (char *str)
{
char *p;

   p = str + strlen (str) - 1;

   if (p >= str)
      {
      /* Strip off the trailing blanks */
      while ((p >= str) && (isspace (*p)))
         {
         *p = '\0';
         --p;
         }

      /* Get rid of backslash if there is one */
      if ((p >=str) && ((*p == '\\') || (*p == '/')))
         {
         if ((p > str) && (*(p-1) != ':'))      /* Don't delete on root */
            *p = '\0';
         }
      }

   return (fancy_str (str));
}

char *ctl_string (char *source)             /* malloc & copy to ctl      */
{
char *dest, *c;

	c = skip_blanks (source);		/* get over the blanks       */
	dest = malloc (strlen (c) + 1);	/* allocate space for string */
	strcpy (dest, c);				/* copy the stuff over       */
	return (dest);					/* and return the address    */
}

char *ctl_slash_string (char *source)       /* malloc & copy to ctl      */
{
char *dest, *c, *t;
int i;
struct stat buffer;

	c = skip_blanks (source);                     /* get over the blanks       */
	i = strlen (c);                               /* get length of remainder   */
	if (i < 1)                                    /* must have at least 1      */
   	return (NULL);                             /* if not, return NULL       */
	t = dest = malloc (i+2);                      /* allocate space for string */
	strcpy (dest, c);                             /* copy the stuff over       */
	delete_backslash (dest);                      /* get rid of trailing stuff */
	/* Check to see if the directory exists */
	i = stat (dest, &buffer);
	if (i || (!(buffer.st_mode & S_IFDIR)))
   	{
   	return(NULL);						/* Directory does not exist */
   									/* AP70 may fail to execute properly because of this! */
   	}
	add_backslash (dest);                         /* add the backslash         */
	return (dest);                                /* return the directory name */
}

char *skip_blanks (char *string)
{
	while (*string == ' ' || *string == '\t')
		++string;
	return (string);
}
