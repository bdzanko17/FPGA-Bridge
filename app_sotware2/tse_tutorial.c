#include <altera_avalon_sgdma.h>
#include <altera_avalon_sgdma_descriptor.h>
#include <altera_avalon_sgdma_regs.h>

#include "sys/alt_stdio.h"
#include "sys/alt_irq.h"
#include <unistd.h>

// Function Prototypes
void rx_ethernet_isr0 (void *context);
void rx_ethernet_isr1 (void *context);

unsigned int text_length;

unsigned char tabela[100][7] ={{0}};

int counter = 0;

void insertInTable(unsigned char sourceAddress[], char port)
{
    int i=0;
    int j=0;
    if(counter==100)
        counter=0;
    
    for(j=0;j<6;j++){
        
        tabela[counter][j]=sourceAddress[j];
    }
       
        tabela[counter][6]=port;
        counter++;
}

int SearchInTable(unsigned char sourceAddress[], char * port){
    int i,j,br=0;
    for(i=0;i<100;i++){
        for(j=0;j<6;j++){
            if(sourceAddress[j]==tabela[i][j])
            br++;   
                }
        if(br==6) {
            
            *port=tabela[i][6];
            return 1;
            
        }
        br=0;
    }
 
    return 0;
}

// Global Variables
unsigned int text_length;

// Create a transmit frame
unsigned char tx_frame0[1024] = {
    0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 	// destination address (broadcast)
	0x01,0x60,0x6E,0x11,0x02,0x0F, 	// source address
	0x7A,0x05, 						// length or type of the payload data
	'O','d','g','o','v','o','r'		// payload data (ended with termination character)
};

unsigned char tx_frame1[1024] = {
    0x00,0x00, 						// for 32-bit alignment
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 	// destination address (broadcast)
	0x01,0x60,0x6E,0x11,0x03,0x0F, 	// source address
	0x00,0x2E, 						// length or type of the payload data
	'\0' 
    
};


// Create a receive frame
unsigned char rx_frame0[1024] = { 0 };
unsigned char rx_frame1[1024] = { 0 };

// Create sgdma transmit and receive devices
alt_sgdma_dev * sgdma_tx_dev0;
alt_sgdma_dev * sgdma_rx_dev0;

alt_sgdma_dev * sgdma_tx_dev1;
alt_sgdma_dev * sgdma_rx_dev1;


// Allocate descriptors in the descriptor_memory (onchip memory)
alt_sgdma_descriptor tx_descriptor0		__attribute__ (( section ( ".descriptor_memory0" )));
alt_sgdma_descriptor tx_descriptor0_end	__attribute__ (( section ( ".descriptor_memory0" )));

alt_sgdma_descriptor rx_descriptor0  	__attribute__ (( section ( ".descriptor_memory0" )));
alt_sgdma_descriptor rx_descriptor0_end  __attribute__ (( section ( ".descriptor_memory0" )));


alt_sgdma_descriptor tx_descriptor1		__attribute__ (( section ( ".descriptor_memory1" )));
alt_sgdma_descriptor tx_descriptor1_end	__attribute__ (( section ( ".descriptor_memory1" )));

alt_sgdma_descriptor rx_descriptor1  	__attribute__ (( section ( ".descriptor_memory1" )));
alt_sgdma_descriptor rx_descriptor1_end  __attribute__ (( section ( ".descriptor_memory1" )));



int main(void)
{	
	// Open the sgdma transmit device
	sgdma_tx_dev0 = alt_avalon_sgdma_open ("/dev/sgdma_tx0");
    sgdma_tx_dev1 = alt_avalon_sgdma_open ("/dev/sgdma_tx1");

	if (sgdma_tx_dev0 == NULL || sgdma_tx_dev1 == NULL) {
		alt_printf ("Error: could not open scatter-gather dma transmit device\n");
		return -1;
	} else alt_printf ("Opened scatter-gather dma transmit device.\n");
    
		
	// Open the sgdma receive device
	sgdma_rx_dev0 = alt_avalon_sgdma_open ("/dev/sgdma_rx0");
    sgdma_rx_dev1 = alt_avalon_sgdma_open ("/dev/sgdma_rx1");

	if (sgdma_rx_dev0 == NULL || sgdma_rx_dev1 == NULL) {
		alt_printf ("Error: could not open scatter-gather dma receive device\n");
		return -1;
	} else alt_printf ("Opened scatter-gather dma receive device.\n");

	// Set interrupts for the sgdma receive device
	alt_avalon_sgdma_register_callback( sgdma_rx_dev0, (alt_avalon_sgdma_callback) rx_ethernet_isr0, 0x00000014, NULL );
    alt_avalon_sgdma_register_callback( sgdma_rx_dev1, (alt_avalon_sgdma_callback) rx_ethernet_isr1, 0x00000014, NULL );


	// Create sgdma receive descriptor
	alt_avalon_sgdma_construct_stream_to_mem_desc( &rx_descriptor0, &rx_descriptor0_end, (alt_u32 *)rx_frame0, 0, 0 );
    alt_avalon_sgdma_construct_stream_to_mem_desc( &rx_descriptor1, &rx_descriptor1_end, (alt_u32 *)rx_frame1, 0, 0 );


	// Set up non-blocking transfer of sgdma receive descriptor
	alt_avalon_sgdma_do_async_transfer( sgdma_rx_dev0, &rx_descriptor0 );
    alt_avalon_sgdma_do_async_transfer( sgdma_rx_dev1, &rx_descriptor1 );

	// Triple-speed Ethernet MegaCore base address
	volatile int * tse0 = (int *) 0x00103400;
    volatile int * tse1 = (int *) 0x00103000;
 	
	// Initialize the MAC address 
	*(tse0 + 3) = 0x116E6001;
	*(tse0 + 4) = 0x00000F02; 
    
	*(tse1 + 3) = 0x116E6001;
	*(tse1 + 4) = 0x00000F03;
    


	// Specify the addresses of the PHY devices to be accessed through MDIO interface
	//*(tse1 + 0x0F) = 0x10;
	*(tse0 + 0x10) = 0x10;
	
	// Write to register 16 of the PHY chip for Ethernet port 0 to enable automatic crossover for all modes
	*(tse0 + 0xB0) = *(tse0 + 0xB0) | 0x0060;
	
	// Write to register 20 of the PHY chip for Ethernet port 0 to set up delay for input/output clk
	*(tse0 + 0xB4) = *(tse0 + 0xB4) | 0x0082;
	
	// Software reset the second PHY chip and wait
	*(tse0 + 0xA0) = *(tse0 + 0xA0) | 0x8000;
	while ( *(tse0 + 0xA0) & 0x8000  )
		;	 
	 
	// Enable read and write transfers, gigabit Ethernet operation, and CRC forwarding
	*(tse0 + 2) = *(tse0 + 2) | 0x0000005B;	

    
    // Specify the addresses of the PHY devices to be accessed through MDIO interface
	//*(tse1 + 0x0F) = 0x10;
	*(tse1 + 0x10) = 0x11;
	
	// Write to register 16 of the PHY chip for Ethernet port 0 to enable automatic crossover for all modes
	*(tse1 + 0xB0) = *(tse1 + 0xB0) | 0x0060;
	
	// Write to register 20 of the PHY chip for Ethernet port 0 to set up delay for input/output clk
	*(tse1 + 0xB4) = *(tse1 + 0xB4) | 0x0082;
	
	// Software reset the second PHY chip and wait
	*(tse1 + 0xA0) = *(tse1 + 0xA0) | 0x8000;
	while ( *(tse1 + 0xA0) & 0x8000  )
		;	 
	 
	// Enable read and write transfers, gigabit Ethernet operation, and CRC forwarding
	*(tse1 + 2) = *(tse1 + 2) | 0x0000005B;	

  
	while (1) {
        
       


	}    
	
	return 0;
}


void rx_ethernet_isr0 (void *context)
{
	int i,j;
    char port='s';
    unsigned char kontekstSrc[6] = { 0 };       //6 bajti za source address
    unsigned char kontekstDest[6] = { 0 };      //6 bajti za dest adresskontekstDest
	
	alt_printf("Packet arrived on port 0.\n");
    
    
	// Wait until receive descriptor transfer is complete
	while (alt_avalon_sgdma_check_descriptor_status(&rx_descriptor0) != 0); 

    
    
    for(i=0;i<6;i++){
        kontekstSrc[i]=rx_frame0[i+8];         
        kontekstDest[i]=rx_frame0[i+2];
    }
    
      
   

    


    if(!SearchInTable(kontekstDest,&port)){

        alt_avalon_sgdma_construct_mem_to_stream_desc( &tx_descriptor0, &tx_descriptor0_end, (alt_u32 *)rx_frame0, 62, 0, 1, 1, 0 );
		
         alt_avalon_sgdma_do_async_transfer( sgdma_tx_dev0, &tx_descriptor0 );
		
         while (alt_avalon_sgdma_check_descriptor_status(&tx_descriptor0) != 0)
			;

        alt_avalon_sgdma_construct_mem_to_stream_desc( &tx_descriptor1, &tx_descriptor1_end, (alt_u32 *)rx_frame0, 62, 0, 1, 1, 0 );
		
         alt_avalon_sgdma_do_async_transfer( sgdma_tx_dev1, &tx_descriptor1 );
		
         while (alt_avalon_sgdma_check_descriptor_status(&tx_descriptor1) != 0)
			;
		alt_printf("No destination address in table, packet forwarded on all ports.\n");
		
    }else if (port=='0')
    {
        alt_avalon_sgdma_construct_mem_to_stream_desc( &tx_descriptor0, &tx_descriptor0_end, (alt_u32 *)rx_frame0, 62, 0, 1, 1, 0 );
		
         alt_avalon_sgdma_do_async_transfer( sgdma_tx_dev0, &tx_descriptor0 );
		
         while (alt_avalon_sgdma_check_descriptor_status(&tx_descriptor0) != 0)
			;
		alt_printf("Packet forwarded to port 0 from port 0.\n");

    }else if (port=='1')
    {

        alt_avalon_sgdma_construct_mem_to_stream_desc( &tx_descriptor1, &tx_descriptor1_end, (alt_u32 *)rx_frame0, 62, 0, 1, 1, 0 );
		
         alt_avalon_sgdma_do_async_transfer( sgdma_tx_dev1, &tx_descriptor1 );
		
         while (alt_avalon_sgdma_check_descriptor_status(&tx_descriptor1) != 0)
			;    
		alt_printf("Packet forwarded to port 1 from port 0.\n");

    }

    if(!SearchInTable(kontekstSrc,&port)){
        insertInTable(kontekstSrc,'0');
        alt_printf("Source address didn't exist in table, now is added.\n");
    }
    
    alt_printf("\n");
    alt_printf("MAC address table:    \n");
    for(i=0;i<counter;i++){
   
    for(j=0;j<6;j++){
        if(j<5){
                alt_printf("%x:",tabela[i][j]);
        } else {
            alt_printf("%x",tabela[i][j]);
        }
        
        
    }
    alt_printf("\n");
  
    }
    alt_printf("\n");

    

	// Create new receive sgdma descriptor
	alt_avalon_sgdma_construct_stream_to_mem_desc( &rx_descriptor0, &rx_descriptor0_end, (alt_u32 *)rx_frame0, 0, 0 );
	
	// Set up non-blocking transfer of sgdma receive descriptor
	alt_avalon_sgdma_do_async_transfer( sgdma_rx_dev0, &rx_descriptor0 );
}


void rx_ethernet_isr1 (void *context)
{
    int i,j;
    char port='s';
    unsigned char kontekstSrc[6] = { 0 };       //6 bajti za source address
    unsigned char kontekstDest[6] = { 0 };      //6 bajti za dest adress
    
    
	// Wait until receive descriptor transfer is complete
	while (alt_avalon_sgdma_check_descriptor_status(&rx_descriptor1) != 0)
		;
	
		alt_printf("Packet arrived on port 1. \n");

    
    for(i=0;i<6;i++){
        kontekstSrc[i]=rx_frame1[i+8];         //Provjeriti format ping okvira ! 
        
        kontekstDest[i]=rx_frame1[i+2];
    }
    
  
    


    
    if(!SearchInTable(kontekstDest,&port)){

        alt_avalon_sgdma_construct_mem_to_stream_desc( &tx_descriptor0, &tx_descriptor0_end, (alt_u32 *)rx_frame1, 62, 0, 1, 1, 0 );
		
        alt_avalon_sgdma_do_async_transfer( sgdma_tx_dev0, &tx_descriptor0 );
		
         while (alt_avalon_sgdma_check_descriptor_status(&tx_descriptor0) != 0)
			;

        alt_avalon_sgdma_construct_mem_to_stream_desc( &tx_descriptor1, &tx_descriptor1_end, (alt_u32 *)rx_frame1, 62, 0, 1, 1, 0 );
		
        alt_avalon_sgdma_do_async_transfer( sgdma_tx_dev1, &tx_descriptor1 );
		
         while (alt_avalon_sgdma_check_descriptor_status(&tx_descriptor1) != 0)
			;
		
		alt_printf("No destination address in table, packet forwarded on all ports.\n");

    }else if (port=='0')
    {
        alt_avalon_sgdma_construct_mem_to_stream_desc( &tx_descriptor0, &tx_descriptor0_end, (alt_u32 *)rx_frame1, 62, 0, 1, 1, 0 );
		
         alt_avalon_sgdma_do_async_transfer( sgdma_tx_dev0, &tx_descriptor0 );
		
         while (alt_avalon_sgdma_check_descriptor_status(&tx_descriptor0) != 0)
			;
	alt_printf("Packet forwarded to port 0 from port 1. \n");

    }else if (port=='1')
    {

        alt_avalon_sgdma_construct_mem_to_stream_desc( &tx_descriptor1, &tx_descriptor1_end, (alt_u32 *)rx_frame1, 62, 0, 1, 1, 0 );
		
         alt_avalon_sgdma_do_async_transfer( sgdma_tx_dev1, &tx_descriptor1 );
		
         while (alt_avalon_sgdma_check_descriptor_status(&tx_descriptor1) != 0)
			;    
	alt_printf("Packet forwarded to port 1 from port 1. \n");

    }

    if(!SearchInTable(kontekstSrc,&port)){
        insertInTable(kontekstSrc,'1');
     alt_printf("Source address didn't exist in table, now is added.\n");
    }
    
    alt_printf("\n");
    alt_printf("MAC address table:    \n");
    for(i=0;i<counter;i++){
   
    for(j=0;j<6;j++){
        if(j<5){
                alt_printf("%x:",tabela[i][j]);
        } else {
            alt_printf("%x",tabela[i][j]);
        }
        
        
    }
    alt_printf("\n");
  
    }
    alt_printf("\n");
    

	
	// Create new receive sgdma descriptor
	alt_avalon_sgdma_construct_stream_to_mem_desc( &rx_descriptor1, &rx_descriptor1_end, (alt_u32 *)rx_frame1, 0, 0 );
	
	// Set up non-blocking transfer of sgdma receive descriptor
	alt_avalon_sgdma_do_async_transfer( sgdma_rx_dev1, &rx_descriptor1 );
}
 
