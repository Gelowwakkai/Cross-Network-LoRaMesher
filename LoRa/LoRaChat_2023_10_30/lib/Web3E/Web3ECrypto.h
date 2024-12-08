//
// Created by James Brown on 2018/09/13.
// Modified by Joan Miquel Solé on 2023/03/29.
//

#ifndef ARDUINO_WEB3_CRYPTO_H
#define ARDUINO_WEB3_CRYPTO_H
#include <Arduino.h>
#include <vector>
#include <string.h>

#include "Trezor/secp256k1.h"
#include "Trezor/ecdsa.h"
#include "Util.h"

using namespace std;

extern const char* PERSONAL_MESSAGE_PREFIX;

typedef unsigned char BYTE;
#define ETHERS_PRIVATEKEY_LENGTH       32
#define ETHERS_PUBLICKEY_LENGTH        64
#define ETHERS_ADDRESS_LENGTH          20
#define ETHERS_KECCAK256_LENGTH        32
#define ETHERS_SIGNATURE_LENGTH        65

class Crypto {

public:
    static bool Sign(uint8_t* privateKey, BYTE* digest, BYTE* result);

    static void ECRecover(BYTE* signature, BYTE* public_key, BYTE* message_hash);
    static bool Verify(const uint8_t* public_key, const uint8_t* message_hash, const uint8_t* signature);
    static void PrivateKeyToPublic(const uint8_t* privateKey, uint8_t* publicKey);
    static void PublicKeyToAddress(const uint8_t* publicKey, uint8_t* address);
    static void Keccak256(const uint8_t* data, uint16_t length, uint8_t* result);
    static string ECRecoverFromPersonalMessage(string* signature, string* message);
    static string ECRecoverFromHexMessage(string* signature, string* hex);
    static string ECRecoverFromHash(string* signature, BYTE* digest);
    static string GetPersonalHash(string message);

    static string Keccak256(vector<uint8_t>* bytes);
};


#endif //ARDUINO_WEB3_CRYPTO_H
