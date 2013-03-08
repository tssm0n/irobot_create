//----------------------------------------------------------------
// PROTOTYPES, PORT, AND BAUD DEFINITIONS FOR CM9600 "SOFT" UART
//----------------------------------------------------------------

//------------------------------
//     FUNCTION PROTOTYPES
//------------------------------
//------------------------------
//COMMON
//------------------------------
uint8_t cm_init_UART(uint8_t port, uint8_t brate);  // Initialize "soft" UART
                                                    // Returns  handle to port.
//------------------------------
//TRANSMIT
//------------------------------
void cm_ByteXmt(uint8_t port, uint8_t);      // Send a single byte
void cm_send_line(uint8_t port,char *ibuf);  // Send an ASCIZ string
uint8_t cm_UART_busy(uint8_t port);          // Check if transmitter is busy

//------------------------------
//RECEIVE FUNCTIONS
//------------------------------
uint8_t cm_char_rdy(uint8_t port); // Return number of chars waiting to be read
char cm_read_char(uint8_t port);   // Read 1 char from FIFO
void cm_rcv_enable(uint8_t port);  // Enable receiver
void cm_rcv_disable(uint8_t port); // Disable receiver

//------------------------------
//   VALID ePORT VALUES
//------------------------------

#define CM_EPORT_CARGO  0
#define CM_EPORT_CENTER 1
#define CM_EPORT_RIGHT  2
#define CM_EPORT_LEFT   3

//------------------------------
//  VALID BAUD RATE VALUES
//------------------------------
#define CM_BAUD_2400    24
#define CM_BAUD_4800    12
#define CM_BAUD_9600     6
#define CM_BAUD_19200    3

