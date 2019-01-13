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
#define INCORRECT_EXIT		"ошибка(001): ВНИМАНИЕ!!! некорректный выход из программы\n (просьба сообщить по тел. 27-22 во Львове)\n"
#define MODEM_DCD_ERROR		"ошибка(010): нет несущей от удаленного компьютера"
#define MODEM_DSR_ERROR		"ошибка(011): нет сигнала DSR (обрыв в кабеле или модем выкл. или неисправен)"
#define MODEM_CTS_ERROR		"ошибка(012): нет сигнала CTS (обрыв в кабеле или модем выкл. или неисправен)"
#define DUST_IN_CHANEL		"ошибка(013): мусор в канале"
#define FOSSIL_WRONG_VERSION	"ошибка(014): требуется FOSSIL driver с номерами функций > 1B"
#define MEMORY_ALLOCATE_ERROR	"ошибка(015): при выделении памяти"
#define SYSTEM_TIME_ERROR	"ошибка(016): что то не в порядке с системными часами"
#define MODEM_READY		"модем готов к работе"
#define OK_ID			"связь установлена"
#define FOSSIL_DRV_FOUND		"FOSSIL:"

#define NO_ANSWER_FOR_ID_BLOCK 	"ошибка(020): нет ответа на идентификационный блок"
#define ERROR_IN_ID_BLOCK	"ошибка(021): неправильный идентификационный блок"
#define ERROR_CSUM_IN_ID_BLOCK	"ошибка(022): неверная конрольной сумма при передаче ид. блока"
#define UNKNOWN_SYM_IN_ID_BLOCK	"ошибка(023): неизвестный символ при приеме подтверждения на ид. блок"
#define UNKNOWN_SYM_ON_RX_ACK	"ошибка(024): неизвестный символ при приеме подтверждения на инф. блок"
#define NAK_ON_TX_BLOCK		"ошибка(025): отрицательный ответ при приеме подтверждения на принятый блок"
#define EOT_IN_TX		"ошибка(026): внезапный запрос на окончание сеанса связи"
#define TIMEOUT_ACK_TX_BLOCK	"ошибка(027): Time-out при приеме подтверждения на инф. блок"
#define TIME_OUT_IN_RX		"ошибка(028): Time-out при приеме блока"
#define TIME_OUT_RX_FILE		"ошибка(029): Time-out при попытке принять файл"
#define ID_BLOCK_CSUM_ERROR	"ошибка(030): контрольной суммы в ид. блоке"
#define ID_BLOCK_TOO_BIG		"ошибка(031): слишком длинный идентификационный блок"
#define BLOCK_TOO_BIG		"ошибка(032): слишком длинный информационный блок"
#define BLOCK_NUMBER_ERROR	"ошибка(033): принятый и ожидаемый номер блока несовпадают"
#define BLOCK_NOT_WRITE_TO_DISK	"ошибка(034): нехватка времени для записи принятого блока на диск"
#define NUMBER_ATTEMPT_TOO_BIG	"ошибка(035): превышено максимальное кол-во попыток"
#define ERROR_CLOSE_RECEIVED_FILE	"ошибка(036): при закрытии принятого файла"
#define BLOCK_CSUM_ERROR		"ошибка(037): неверная контрольная сумма в принятом блоке"
#define CALL_ACK_ON_RX		"ошибка(038): запрос на повторную передачу подтверждения"
#define UNKNOWN_BLOCK		"ошибка(039): неформатный блок???"

#define NO_FILE_FOR_TX		"ошибка(050): файл для передачи не найден"
#define LENGTH_OF_TX_FILE_ZERRO	"ошибка(051): длина файла для передачи равна нулю"
#define ERROR_READ_FILE		"ошибка(052): при чтении файла"
#define ERROR_OPEN_RX_FILE	"ошибка(053): при открытии файла для приема информации"
#define ERROR_OPEN_TX_FILE	"ошибка(054): при открытии файла для передачи информации"
#define ERROR_OPEN_LOG_FILE	"ошибка(055): при открытии LOG файла"
#define ERROR_OPEN_DEBUG_FILE	"ошибка(056): при открытии DEBUG файла"
#define ERROR_OPEN_CONFIG_FILE	"ошибка(057): при открытии файла конфигурации"
#define ERROR_DELETE_RX_FILE	"ошибка(058): при удалении существующего файла для приема информации"
#define ERROR_DELETE_TX_FILE_RO	"ошибка(059): при удалении переданного файла (атрибут - только для чтения)"
#define ERROR_DELETE_TX_FILE_NF	"ошибка(060): при удалении переданного файла (файл отсутствует)"
#define ERROR_DELETE_TX_FILE_UN	"ошибка(061): при удалении переданного файла (причина неизвестна)"
#define ERROR_WRITE_DISK		"ошибка(062): при записи принятой информации на диск"
#define ERROR_WRITE_LOG_FILE	"ошибка(063): при записи информации в LOG файл!!!"
#define ERROR_WRITE_DEBUG_FILE	"ошибка(064): при записи информации в DEBUG файл!!!"
#define TEMP_FILE_RX_OPEN_ERROR	"ошибка(065): при открытии времнного файла для приема"
#define FILE_RX_OPEN_OKEY	"файл для приема информации открыт успешно"
#define FILE_TX_OPEN_OKEY	"файл для передачи информации открыт успешно"
#define TX_FILE			"попытка передать файл"
#define FILE_RX_OK		"информация прията в файл"
#define FILE_CLOSE_OK		"файл закрыт успешно"

#define UNKNOWN_CHANNEL_NUMBER	"ошибка(080): неверное кол-во каналов"
#define NUMBER_COM_PORT_ERROR	"ошибка(081): неверно задан номер COM порта"
#define UNKNOWN_CHANNEL_PARAMETERS "ошибка(082): параметры канала заданы неверно"
#define UNKNOWN_DEBUG_FILE_NUMBER	"ошибка(083): неверный номер варианта DEBUG файла"
#define CHANNEL_NUMBER_ERROR	"ошибка(084): неверный номер канала"
#define CHANNEL_NUMBER_PAR_ERROR	"ошибка(085): неверное кол-во параметров"
#define CHANNEL_SPEED_ERROR	"ошибка(086): неверно задана скорость обмена в канале"
#define CHANNEL_BIT_ERROR	"ошибка(087): кол-во бит в параметрах канала задано неверно"
#define CHANNEL_PARITY_ERROR	"ошибка(088): четность в параметрах канала задана неверно"
#define COM_PORT_NOT_FOUND	"ошибка(089): COM порт не найден или неисправен"
#define CHANNEL_STOP_ERROR	"ошибка(090): кол-во стоп-бит в параметрах канала задано неверно"
#define UNKNOWN_CTL_COMMAND	"ошибка(091): неизвестная команда в конфигурационном файле"
#define DEBUG_CTL_COMMAND_ERROR	"ошибка(092): в конфиг. файле, в команде debug (только ON или OFF)"
#define ERROR_RX_TIMEOUT		"ошибка(093): Time-out для приема задан неверно"
#define ERROR_COMMAND_LINE	"ошибка(094): неизвестное ключевое слово в командной строке"
#define ERROR_BREAK		"ошибка(095): работа программы прервана оператором"
#define WRONG_DEBUG_CUR_ROW	"ошибка(096): неверное значение рядка"
#define WRONG_DEBUG_CUR_COL	"ошибка(097): неверное значение колонки"

#define RX_DESABLE		"ошибка(110): прием информации запрещен"
#define TIME_2359_ERROR		"ошибка(111): временная константа превышает 23ч59м"
#define INTERVAL_ERROR		"ошибка(112): интервал времени между событиями больше времени события"
#define ERROR_BIG_24		"ошибка(113): интервал времени не может пересекать отметку 00:00"
#define ERROR_SHEDULE_OVERRIDE_RX	"ошибка(114): события для приема пересекаются"
#define ERROR_SHEDULE_OVERRIDE_TX	"ошибка(115): события для передачи пересекаются"
#define NUMBER_SHEDULE_PAR_ERROR	"ошибка(116): неверное кол-во параметров в расписании"
#define SHEDULE_PATH_ERROR	"ошибка(117): нет каталога на диске"
#define SHEDULE_MOVE_PATH_ERROR	"ошибка(118): нет кат. на диске для перемещения переданных файлов"
#define ERROR_STAR		"ошибка(119): в имени файла недопустимы символы '*'"

#define EXCLUDE_FROM_SHEDULE	"исключается из расписания"
#define NEXT_SHEDULE_TX		"следующее событие для передачи файлов"
#define NEXT_SHEDULE_RX		"следующее событие для приема файлов"
#define TEMP_FILE_RX_OPEN	"временный файл для приема информации открыт успешно"
#define SHEDULE_MOVE_PATH_NOT	"не задан каталог для перемещения файлов после передачи"
#define NO_SHEDULE_TX		"нет событий для передачи файлов"
#define NO_SHEDULE_RX		"нет событий для приема файлов"
#define NO_TX_FILES		"нет файлов для передачи"

#define FILES_BETWEEN		"файлы, переданные между"
#define WILL_DELETE		"будут удалены с диска"

#define FILE_TX_OK		"файл передан"
#define FILE_RECEIVED_OK		"файл принят"
#define FILE_DELETED		"файл удален"
#define FILE_NOT		"файл отсутствует"
#define STRING			"строка:"
#define SHEDULE_NUMBER		"очередь номер"
#define SHEDULE			"событие"

#define WARIANTS		"допустимые варианты:"
#define REAL_NUMBER		"принятый"
#define WAITING_NUMBER		"ожидаемый"

#define SYSTEM_OPEN		" --- ПРОГРАММА АП-70 ЗАПУЩЕНА"
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


