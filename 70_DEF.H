/*

	Vitaly Lev
			
	<C> <Lion>	

	prg_def.h

	Programm for Programmer on 8048

	Defines & i.t.c

	930608	V1.0

*/

#define byte unsigned char
#define uint unsigned int
#define word unsigned int

/* 
	Number channel = 1
*/
#define channel 1	/* Channel's number */
#define TNUMBER 3	/* Number of channel's number = 3 :
						         1 - local timer
							2 - timer for rx shedule
							3 - timer for tx shedule
			*/
/*
*/
#define TRUE 1
#define FALSE 0

#define NEW_NUMBER 1
#define OLD_NUMBER 2

#define DEBUG_NO 	0
#define DEBUG_SMALL	1
#define DEBUG_MIDDLE	2
#define DEBUG_LARGE	3
#define DEBUG_MAX_NUMBER	3

#define DEBUG_CUR_ROW_MAX 25
#define DEBUG_CUR_COL_MAX 79

#define NOACTIVE FALSE
#define ACTIVE TRUE
#define RX_TEMP_FILE_NAME "\\ap70temp.rx"


#define RX_BLOCK_LENGTH	1024		/* length receiv's block */
#define N_TX_COMMAND 	3		/* attempts send command number */
#define N_RESET		2		/* number attempts send "reset" */
#define BIT0_ON 	(0x01)
#define RX_TIMEOUT_MAX	10000		/* max value receive timeout (if t/o from comand line) */
#define RX_TIMEOUT_MIN	1		/* min value receive timeout (if t/o from comand line) */
#define RX_TIMEOUT_DEFAULT 30		/* receive timeout default (if t/o from comand line) */
#define ID_BLOCK_ATTEMPT	5		/* number of tx file attempt */
#define BETWEEN_ID_BLOCK 10		/* number second between ID blocks */
#define BETWEEN_ID_GROUP 15 /* 60 */	/* number second between ID groups */
#define BETWEEN_FILES	15 /* 60 */	/* number second between TX file in shedule */

#define IBM		01		/* computer type for flag Computer */
#define ES1841		02		/* computer type for flag Computer */

#define DEBUGBUFF	100

/**/
/*
	Modem Status
*/
#define MODEM_DCD	0x0080		/* received line detect signal */
#define MODEM_DSR	0x0020		/* data set ready (DSR) */
#define MODEM_CTS	0x0010		/* clear to send  (CTS) */
/* 
	Queue 
*/
#define WRITE_BLOCK	0x0001		/* write block on receiv's file */
#define CLOSE_RX_FILE	0x0002		/* close receive file */
#define COMMAND_RX_FILE	0x0004		/* receive file in command line */
#define COMMAND_TX_FILE	0x0008		/* transmitt file in command line */
#define COMMAND_LINE	0x0010		/* comand line present */
#define SHEDULE_RX_FILE	0x0020		/* sheduling receive file */
#define SHEDULE_TX_FILE	0x0040		/* sheduling transmitt file */
#define SHEDULE_RX_OK	0x0080		/* shedule's receive file OK */
#define SHEDULE_TX_OK	0x0100		/* shedule's transmitt file OK */
#define SHEDULE_RX_NOTOK	0x0200		/* shedule's receive file NOT OK */
#define SHEDULE_TX_NOTOK	0x0400		/* shedule's transmitt file NOT OK */
#define SHEDULE_TX_SLEEP	0x0800		/* sleep until TX next file */
/**/
#define HEX_END          	2+256
#define HEX_ERROR_SUM		3+256

/*
			Constants
*/
#define CR 			0x0D
#define LF			0x0A
#define BEL			0x07
/**/
#define STX			0x02
#define ETX			0x03
#define EOT			0x04
#define ENQ			0x05
#define ACK			0x06
#define NAK 			0x15
#define ETB			0x17
#define EM			0x19
/**/
#define NONE			0x00
#define CRASH			0xff
/**/
#define START_TX_ID_BLOCK 	0x01
#define WAIT_TX_ID_BLOCK	 	0x02
#define END_TX_ID_BLOCK		0x03
#define START_TX_BLOCK		0x04
#define WAIT_TX_BLOCK	  	0x05
#define WAIT_ACK_TX_BLOCK  	0x06
#define CALL_ACK		0x07
#define SEND_EOT		0x08
#define END_TX			0x09
/**/
#define WAIT_STX		0x20
#define WAIT_RX_ID_BLOCK		0x21
#define WAIT_RX_CSUM_ID_BLOCK	0x22
#define WAIT_START_RX_BLOCK	0x23
#define WAIT_RX_BLOCK		0x24
#define WAIT_RX_CSUM_BLOCK	0x25
#define SEND_ACK_BLOCK		0x26
#define SEND_NAK_WAIT_STX	0x27
#define SEND_ACK_WAIT_STX	0x28
#define SEND_NAK_BLOCK		0x29
#define SEND_NAK_BLOCK_1		0x2A
#define WAIT_EOT		0x2B
#define AGAIN_ACK		0x2C
/**/
#define OK_M 			"OK"
#define INCORRECT_EXIT		"�訡��(001): ��������!!! �����४�� ��室 �� �ணࠬ��\n (���졠 ᮮ���� �� ⥫. 27-22 �� �좮��)\n"
#define MODEM_DCD_ERROR		"�訡��(010): ��� ����饩 �� 㤠������� ��������"
#define MODEM_DSR_ERROR		"�訡��(011): ��� ᨣ���� DSR (���� � ������ ��� ����� �몫. ��� ����ࠢ��)"
#define MODEM_CTS_ERROR		"�訡��(012): ��� ᨣ���� CTS (���� � ������ ��� ����� �몫. ��� ����ࠢ��)"
#define DUST_IN_CHANEL		"�訡��(013): ���� � ������"
#define FOSSIL_WRONG_VERSION	"�訡��(014): �ॡ���� FOSSIL driver � ����ࠬ� �㭪権 > 1B"
#define MEMORY_ALLOCATE_ERROR	"�訡��(015): �� �뤥����� �����"
#define SYSTEM_TIME_ERROR	"�訡��(016): �� � �� � ���浪� � ��⥬�묨 �ᠬ�"
#define MODEM_READY		"����� ��⮢ � ࠡ��"
#define OK_ID			"��� ��⠭������"
#define FOSSIL_DRV_FOUND		"FOSSIL:"

#define NO_ANSWER_FOR_ID_BLOCK 	"�訡��(020): ��� �⢥� �� �����䨪�樮��� ����"
#define ERROR_IN_ID_BLOCK	"�訡��(021): ���ࠢ���� �����䨪�樮��� ����"
#define ERROR_CSUM_IN_ID_BLOCK	"�訡��(022): ����ୠ� ���஫쭮� �㬬� �� ��।�� ��. �����"
#define UNKNOWN_SYM_IN_ID_BLOCK	"�訡��(023): ��������� ᨬ��� �� �ਥ�� ���⢥ত���� �� ��. ����"
#define UNKNOWN_SYM_ON_RX_ACK	"�訡��(024): ��������� ᨬ��� �� �ਥ�� ���⢥ত���� �� ���. ����"
#define NAK_ON_TX_BLOCK		"�訡��(025): ����⥫�� �⢥� �� �ਥ�� ���⢥ত���� �� �ਭ��� ����"
#define EOT_IN_TX		"�訡��(026): �������� ����� �� ����砭�� ᥠ�� �裡"
#define TIMEOUT_ACK_TX_BLOCK	"�訡��(027): Time-out �� �ਥ�� ���⢥ত���� �� ���. ����"
#define TIME_OUT_IN_RX		"�訡��(028): Time-out �� �ਥ�� �����"
#define TIME_OUT_RX_FILE		"�訡��(029): Time-out �� ����⪥ �ਭ��� 䠩�"
#define ID_BLOCK_CSUM_ERROR	"�訡��(030): ����஫쭮� �㬬� � ��. �����"
#define ID_BLOCK_TOO_BIG		"�訡��(031): ᫨誮� ������ �����䨪�樮��� ����"
#define BLOCK_TOO_BIG		"�訡��(032): ᫨誮� ������ ���ଠ樮��� ����"
#define BLOCK_NUMBER_ERROR	"�訡��(033): �ਭ��� � �������� ����� ����� ��ᮢ������"
#define BLOCK_NOT_WRITE_TO_DISK	"�訡��(034): ��墠⪠ �६��� ��� ����� �ਭ�⮣� ����� �� ���"
#define NUMBER_ATTEMPT_TOO_BIG	"�訡��(035): �ॢ�襭� ���ᨬ��쭮� ���-�� ����⮪"
#define ERROR_CLOSE_RECEIVED_FILE	"�訡��(036): �� �����⨨ �ਭ�⮣� 䠩��"
#define BLOCK_CSUM_ERROR		"�訡��(037): ����ୠ� ����஫쭠� �㬬� � �ਭ�⮬ �����"
#define CALL_ACK_ON_RX		"�訡��(038): ����� �� ������� ��।��� ���⢥ত����"
#define UNKNOWN_BLOCK		"�訡��(039): ���ଠ�� ����???"

#define NO_FILE_FOR_TX		"�訡��(050): 䠩� ��� ��।�� �� ������"
#define LENGTH_OF_TX_FILE_ZERRO	"�訡��(051): ����� 䠩�� ��� ��।�� ࠢ�� ���"
#define ERROR_READ_FILE		"�訡��(052): �� �⥭�� 䠩��"
#define ERROR_OPEN_RX_FILE	"�訡��(053): �� ����⨨ 䠩�� ��� �ਥ�� ���ଠ樨"
#define ERROR_OPEN_TX_FILE	"�訡��(054): �� ����⨨ 䠩�� ��� ��।�� ���ଠ樨"
#define ERROR_OPEN_LOG_FILE	"�訡��(055): �� ����⨨ LOG 䠩��"
#define ERROR_OPEN_DEBUG_FILE	"�訡��(056): �� ����⨨ DEBUG 䠩��"
#define ERROR_OPEN_CONFIG_FILE	"�訡��(057): �� ����⨨ 䠩�� ���䨣��樨"
#define ERROR_DELETE_RX_FILE	"�訡��(058): �� 㤠����� �������饣� 䠩�� ��� �ਥ�� ���ଠ樨"
#define ERROR_DELETE_TX_FILE_RO	"�訡��(059): �� 㤠����� ��।������ 䠩�� (��ਡ�� - ⮫쪮 ��� �⥭��)"
#define ERROR_DELETE_TX_FILE_NF	"�訡��(060): �� 㤠����� ��।������ 䠩�� (䠩� ���������)"
#define ERROR_DELETE_TX_FILE_UN	"�訡��(061): �� 㤠����� ��।������ 䠩�� (��稭� �������⭠)"
#define ERROR_WRITE_DISK		"�訡��(062): �� ����� �ਭ�⮩ ���ଠ樨 �� ���"
#define ERROR_WRITE_LOG_FILE	"�訡��(063): �� ����� ���ଠ樨 � LOG 䠩�!!!"
#define ERROR_WRITE_DEBUG_FILE	"�訡��(064): �� ����� ���ଠ樨 � DEBUG 䠩�!!!"
#define TEMP_FILE_RX_OPEN_ERROR	"�訡��(065): �� ����⨨ �६����� 䠩�� ��� �ਥ��"
#define FILE_RX_OPEN_OKEY	"䠩� ��� �ਥ�� ���ଠ樨 ����� �ᯥ譮"
#define FILE_TX_OPEN_OKEY	"䠩� ��� ��।�� ���ଠ樨 ����� �ᯥ譮"
#define TX_FILE			"����⪠ ��।��� 䠩�"
#define FILE_RX_OK		"���ଠ�� ���� � 䠩�"
#define FILE_CLOSE_OK		"䠩� ������ �ᯥ譮"

#define UNKNOWN_CHANNEL_NUMBER	"�訡��(080): ����୮� ���-�� �������"
#define NUMBER_COM_PORT_ERROR	"�訡��(081): ����୮ ����� ����� COM ����"
#define UNKNOWN_CHANNEL_PARAMETERS "�訡��(082): ��ࠬ���� ������ ������ ����୮"
#define UNKNOWN_DEBUG_FILE_NUMBER	"�訡��(083): ������ ����� ��ਠ�� DEBUG 䠩��"
#define CHANNEL_NUMBER_ERROR	"�訡��(084): ������ ����� ������"
#define CHANNEL_NUMBER_PAR_ERROR	"�訡��(085): ����୮� ���-�� ��ࠬ��஢"
#define CHANNEL_SPEED_ERROR	"�訡��(086): ����୮ ������ ᪮���� ������ � ������"
#define CHANNEL_BIT_ERROR	"�訡��(087): ���-�� ��� � ��ࠬ���� ������ ������ ����୮"
#define CHANNEL_PARITY_ERROR	"�訡��(088): �⭮��� � ��ࠬ���� ������ ������ ����୮"
#define COM_PORT_NOT_FOUND	"�訡��(089): COM ���� �� ������ ��� ����ࠢ��"
#define CHANNEL_STOP_ERROR	"�訡��(090): ���-�� �⮯-��� � ��ࠬ���� ������ ������ ����୮"
#define UNKNOWN_CTL_COMMAND	"�訡��(091): �������⭠� ������� � ���䨣��樮���� 䠩��"
#define DEBUG_CTL_COMMAND_ERROR	"�訡��(092): � ���䨣. 䠩��, � ������� debug (⮫쪮 ON ��� OFF)"
#define ERROR_RX_TIMEOUT		"�訡��(093): Time-out ��� �ਥ�� ����� ����୮"
#define ERROR_COMMAND_LINE	"�訡��(094): �������⭮� ���祢�� ᫮�� � ��������� ��ப�"
#define ERROR_BREAK		"�訡��(095): ࠡ�� �ணࠬ�� ��ࢠ�� �����஬"
#define WRONG_DEBUG_CUR_ROW	"�訡��(096): ����୮� ���祭�� �浪�"
#define WRONG_DEBUG_CUR_COL	"�訡��(097): ����୮� ���祭�� �������"

#define RX_DESABLE		"�訡��(110): �ਥ� ���ଠ樨 ����饭"
#define TIME_2359_ERROR		"�訡��(111): �६����� ����⠭� �ॢ�蠥� 23�59�"
#define INTERVAL_ERROR		"�訡��(112): ���ࢠ� �६��� ����� ᮡ��ﬨ ����� �६��� ᮡ���"
#define ERROR_BIG_24		"�訡��(113): ���ࢠ� �६��� �� ����� ���ᥪ��� �⬥�� 00:00"
#define ERROR_SHEDULE_OVERRIDE_RX	"�訡��(114): ᮡ��� ��� �ਥ�� ���ᥪ�����"
#define ERROR_SHEDULE_OVERRIDE_TX	"�訡��(115): ᮡ��� ��� ��।�� ���ᥪ�����"
#define NUMBER_SHEDULE_PAR_ERROR	"�訡��(116): ����୮� ���-�� ��ࠬ��஢ � �ᯨᠭ��"
#define SHEDULE_PATH_ERROR	"�訡��(117): ��� ��⠫��� �� ��᪥"
#define SHEDULE_MOVE_PATH_ERROR	"�訡��(118): ��� ���. �� ��᪥ ��� ��६�饭�� ��।����� 䠩���"
#define ERROR_STAR		"�訡��(119): � ����� 䠩�� �������⨬� ᨬ���� '*'"

#define EXCLUDE_FROM_SHEDULE	"�᪫�砥��� �� �ᯨᠭ��"
#define NEXT_SHEDULE_TX		"᫥���饥 ᮡ�⨥ ��� ��।�� 䠩���"
#define NEXT_SHEDULE_RX		"᫥���饥 ᮡ�⨥ ��� �ਥ�� 䠩���"
#define TEMP_FILE_RX_OPEN	"�६���� 䠩� ��� �ਥ�� ���ଠ樨 ����� �ᯥ譮"
#define SHEDULE_MOVE_PATH_NOT	"�� ����� ��⠫�� ��� ��६�饭�� 䠩��� ��᫥ ��।��"
#define NO_SHEDULE_TX		"��� ᮡ�⨩ ��� ��।�� 䠩���"
#define NO_SHEDULE_RX		"��� ᮡ�⨩ ��� �ਥ�� 䠩���"
#define NO_TX_FILES		"��� 䠩��� ��� ��।��"

#define FILES_BETWEEN		"䠩��, ��।���� �����"
#define WILL_DELETE		"���� 㤠���� � ��᪠"

#define FILE_TX_OK		"䠩� ��।��"
#define FILE_RECEIVED_OK		"䠩� �ਭ��"
#define FILE_DELETED		"䠩� 㤠���"
#define FILE_NOT		"䠩� ���������"
#define STRING			"��ப�:"
#define SHEDULE_NUMBER		"��।� �����"
#define SHEDULE			"ᮡ�⨥"

#define WARIANTS		"�����⨬� ��ਠ���:"
#define REAL_NUMBER		"�ਭ���"
#define WAITING_NUMBER		"��������"

#define SYSTEM_OPEN		" --- ��������� ��-70 ��������"
/*					
#define NOT_ACK_M 		"NAK"	
#define QUESTIONS_M 		"???"	
#define NOT_ANSWER_M		"NOT ANSWER" 
#define SUM_ERR_M		"CHECK SUMM ERR"
#define TIME_OUT_M		"TIME OUT"
*/
typedef struct ch_s
{
	int ch_active;	/* channel activity : -1: channel absent
					     0: channel present, but not initializing
					     1: channel present & initializing*/
	int  comn;
	int  port_add;
	int  interrupt_n;
	int  speed;
	int  bits;
	char parity;
	int  stop; 
	int  baudrate;
	void (_interrupt _far *old_vector)( void );
};

typedef struct sh_s
{
	char tx_rx[3];
	int str_hm;
	int end_hm;
	int int_hm;
	char path[80];
	char move_path[80];
	int active;
};

typedef struct bl
{
	struct sh_s sh[20];	/* shedule */
	int sh_n;		/* shedules number */
	struct ch_s ch;		/* chanal's parameters */
	int tx_sh_active;		/* number of tx active shedule */
	int rx_sh_active;		/* number of rx active shedule */
	long length;		/* length of TX file */
	byte number;		/* TX block number '0' - '9' */
	int many;		/* number bytes in TX block, on RX block */
	int n_rx;		/* number RX byte in buffer (temp for rx ACK) */
	byte attempt;		/* number attempt for RX, TX (variable) */
	int state;		/* Channel state */
	byte error;		/* last error in TX; ETX or ETB on RX */
	FILE *file_tx;		/* file handler for TX file */
	FILE *file_rx;		/* file handler for RX file */
	char file_tx_name[13]; 	/* name of file for TX */
	char file_tx_path[80]; 	/* path of file for TX */
	struct find_t f_buf_rx; 	/* buffer with RX file's parameter for find_next */
	struct find_t f_buf_tx; 	/* buffer with TX file's parameter for find_next */
	char file_rx_name[13]; 	/* name of file for RX */
	char file_rx_path[80]; 	/* path of file for RX */
	byte buffer[512];		/* TX file reading in that buffer */
	byte buffer_rx[2][512]; 	/* RX buffers */
	byte temp_tx[100]; 	/* TX buffer */
	byte temp_rx[1024];	/* RX buffer (after RX channel interrupt byte write in that buffer */
	int fos_inwrite;
	int fos_inread;
	int rx_state;		/* RX state */
	int rx_count;		/* RX count */
	int tx_state;
	int tx_count;
	int tx_number;
	int st_busy;
	byte sum;		/* block csum */
	long *timer;		/* main timer */
	long *sh_timer_rx;	/* shedule RX timer */
	long *sh_timer_tx;	/* shedule TX timer */
	int block_tx;
	int modem_out_busy;	/* is modem busy TX symbol */
	int to_file_n[2];		/* number bytes to write in file */
	int *rxn_p;		/* pointer on number bytes to write on disk */
	int rxb_n;		/* RX buffer number */
	byte *rxb_p;		/* pointer on rx buffer */
	byte *to_file_p;		/* pointer of buffer for write to file */
	uint read;		/* number bytes in buffer for TX */
	uint  offset;		/* offset on TX block, on RX block */
	int queue;		/* queue (bit's number) */
	int modem_state;		/* modem status */
};


