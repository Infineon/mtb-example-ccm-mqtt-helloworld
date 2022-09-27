/*******************************************************************************
 * File Name:   main.c
 *
 * Description: This is the source code for the AIROC™ CCM MQTT HELLO WORLD Example
 *              for ModusToolbox.
 *
 * Related Document: See README.md
 *
 *
 *******************************************************************************
 * $ Copyright 2022 Cypress Semiconductor $
 *******************************************************************************/
#include "CCM.h"

/*******************************************************************************
 * Macros
 *******************************************************************************/
/* define MODIFY_SSID macro as 1 for disconnecting the existing Wi-Fi connection and
 * connect to different Access point*/
#define MODIFY_SSID_AFTER_CONNECTED (0)

/* define CIRRENT_APP_ONBOARDING macro as 1 for Wi-Fi Onboarding via Cirrent APP.*/
#define CIRRENT_APP_ONBOARDING (0)

/*define AWS_FLOW macro as 1 for choosing AWS flow and 0 for Cirrent flow*/
#define AWS_FLOW (0)

/*Max response delay in milliseconds for AT+CONNECT command*/
#define MAX_CONNECT_DELAY (120000)

/* Max response delay in milliseconds for AT commands*/
#define RESPONSE_DELAY (5000)

#define SUCCESS 1

#define FAILURE 0

/* Set SSID, Passphrase and Endpoint as follows
 * AT+CONF SSID=XXXX\n; where XXXX is the required SSID
 * AT+CONF Passphrase=YYYY\n ; YYYY is the Passphrase
 * AT+CONF EndPoint=ZZZZ\n; ZZZZ is the endpoint
 */

#define SET_SSID "AT+CONF SSID=\n"
#define SET_PASSPHRASE "AT+CONF Passphrase=\n"
#define SET_ENDPOINT "AT+CONF Endpoint=\n"

/******************************************************************************
 * Function Prototypes
 *******************************************************************************/
static void wifionboarding();

/******************************************************************************
 * Global Variables
 *******************************************************************************/
int result = 0;

/*******************************************************************************
 * Function Name: main
 ********************************************************************************
 * Summary:
 *  System entrance point. This function
 *  - performs initial setup of device
 *  - initializes UART peripherals to send AT Commands to CCM and view debug messages.
 *  - sends required AT Commands to CCM module
 *
 * Return:
 *  int
 *
 *******************************************************************************/
int main()
{

    bsp_init();

    uart_init();

    printf("\r ******************AIROC™ CCM MQTT HELLO WORLD******************\n");

#if MODIFY_SSID_AFTER_CONNECTED

    /* AT command for disconnecting from Wi-Fi network */
    at_command_send_receive("AT+DISCONNECT\n", RESPONSE_DELAY, &result, NULL);

#endif

#if AWS_FLOW

    /*Check if CCM module already connected to AWS*/
    if (!is_aws_connected())
    {

        /*AT command for getting device name*/
        at_command_send_receive("AT+CONF? ThingName\n", RESPONSE_DELAY, &result, NULL);

        /*AT command for getting device certificate*/
        at_command_send_receive("AT+CONF? Certificate pem\n", RESPONSE_DELAY, &result, NULL);

        /*AT command for sending Device Endpoint*/
        at_command_send_receive(SET_ENDPOINT, RESPONSE_DELAY, &result, NULL);

        /*Connect to Wi-Fi network if it is not connected already*/
        if (!is_wifi_connected())
        {
            wifionboarding();
        }

        /*AT command for Connecting to AWS Cloud*/
        at_command_send_receive("AT+CONNECT\n", MAX_CONNECT_DELAY, &result, "OK 1 CONNECTED\r\n");

        if (result != SUCCESS)
        {
            handle_error();
        }
    }

#else

    /*Check if CCM module already connected to AWS*/
    if (!is_aws_connected())
    {

        /*Connect to Wi-Fi network if it is not connected already*/
        if (!is_wifi_connected())
        {
            wifionboarding();
        }

        /*AT command for Connecting CCM device to AWS staging*/
        at_command_send_receive("AT+CONNECT\n", MAX_CONNECT_DELAY, &result, "OK 1 CONNECTED\r\n");

        if (result != SUCCESS)
        {
            handle_error();
        }

        /*AT command for Getting Endpoint from Cirrent Cloud*/
        at_command_send_receive("AT+CLOUD_SYNC\n", RESPONSE_DELAY, &result, NULL);

        /* Check in Cirrent console if the Job executed succesfully */
        printf("\nThe Connection Automatically switches to the new endpoint after 120 seconds\n\n");

        delay_ms(MAX_CONNECT_DELAY);

        while (!is_aws_connected());
    }

#endif

    /* The device subscribes to a topic "data" .
       Send AT command for sending message to AWS cloud*/

    at_command_send_receive("AT+CONF Topic1=data\n", RESPONSE_DELAY, &result, NULL);

    at_command_send_receive("AT+SEND1 Hello World!\n", RESPONSE_DELAY, &result, NULL);

    return 0;
}

/*******************************************************************************
 * Function Name: WifiOnboarding
 ********************************************************************************
 * Summary: Send AT commands to set SSID and Passphrase for CCM module.
 *                                  or
 *          Send AT command to enter Onboarding mode and connect to Wi-Fi via Cirrent APP
 * Return:
 *  void
 *
 *******************************************************************************/

static void wifionboarding()
{

#if CIRRENT_APP_ONBOARDING

    /* AT command to enter Wi-Fi onboarding mode*/
    at_command_send_receive("AT+CONFMODE\n", RESPONSE_DELAY, &result, NULL);

    printf("\n\rOpen Cirrent APP on your mobile device and choose your Wi-Fi SSID. \n\rThe program continues after successfully connecting to Wi-Fi SSID.\n\r");

    while (!is_wifi_connected());

#else

    /* AT command for sending SSID */
    at_command_send_receive(SET_SSID, RESPONSE_DELAY, &result, NULL);

    /*AT command for sending Passphrase*/
    at_command_send_receive(SET_PASSPHRASE, RESPONSE_DELAY, &result, NULL);

#endif
}

/* [] END OF FILE */
