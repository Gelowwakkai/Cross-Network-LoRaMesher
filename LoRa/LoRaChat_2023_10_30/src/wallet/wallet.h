/**
  Project: LoRa Trust firmware
  File name: Wallet.h
  File type: Class declarations (header)
  Purpose: Wallet software. Key and data signatures management.
  Detail: This class aims to provide the hardware with the functions necessary to create
  a private key (32 random bytes vector) and its corresponding ECDSA public key. The private
  key will be used to sign data. The public key will be used to identify the device and will
  be printed in the screen. Signature functions are provided in this class too.

  @author Joan Miquel Solé, Eloi Cruz Harillo
  @version 3.0 2023/04/24
*/

#ifndef Wallet_h
#define Wallet_h

#include <Arduino.h>
#include <uECC.h>
#include "configuration/configService.h"
#include "helpers/helper.h"
#include "Web3ECrypto.h"

#include "message/messageManager.h"

#include "message/messageService.h"

#include "walletCommandService.h"

#define PRIVATE_KEY_SIZE 32
#define PUBLIC_KEY_SIZE 64
#define ADDRESS_SIZE 20
#define HASH_LENGTH 32

#define CHAIN_ID 1337

class Wallet : public MessageService {
public:
    static Wallet& getInstance() {
        static Wallet instance;
        return instance;
    }

    void begin();
    static int RNG(uint8_t* dest, unsigned size);
    String getAddress();

    /**
     * @brief Sign data with the private key
     *
     * @param data data to sign
     * @param result pointer to the result of the signature. Must be at least ETHERS_SIGNATURE_LENGTH (65)
     * @return String signature
     */
    String signHex(String data, uint8_t* result = nullptr);

    String signJson(String json, uint8_t* result = nullptr);

    String getPrivateKey();

#ifdef PIO_UNIT_TESTING
    void setPrivateKey(String privateKey);
#endif

private:
    WalletCommandService* walletCommandService = new WalletCommandService();
    uint8_t generateRecoveryId();

    Wallet() : MessageService(appPort::WalletApp, String("Wallet")) {
        commandService = walletCommandService;
    };

    void generateKeyAddress(bool reset);

    uint8_t* privateKey = new uint8_t[PRIVATE_KEY_SIZE];
    uint8_t* publicKey = new uint8_t[PUBLIC_KEY_SIZE];
    String address;

    String sign(uint8_t* data, size_t length, uint8_t* result = nullptr);
};


#endif