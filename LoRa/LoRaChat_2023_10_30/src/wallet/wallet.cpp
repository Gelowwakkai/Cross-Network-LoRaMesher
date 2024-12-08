/**
  Project: LoRa Trust firmware
  File name: Wallet.cpp
  File type: Class definitions (implementation)
  Purpose: Wallet software. Key and data signatures management.
  Detail: Definition of the functions declared int he file Wallet.h

  @authors Joan Miquel Solé, Eloi Cruz Harillo
  @version 3.0 2023/04/24
*/

#include "wallet.h"

/**
  Initialize class components.
*/
void Wallet::begin() {
	uECC_set_rng(&RNG);
	randomSeed(analogRead(A0));

	generateKeyAddress(false);
}

/**
  Device private key getter.

  @return key -> private key
*/
String Wallet::getPrivateKey() {
	String key;
	for (int i = 0; i < PRIVATE_KEY_SIZE; i++) {
		String aux = String(this->privateKey[i], HEX);
		if (aux.length() == 1) aux = "0" + aux;
		key = key + aux;
	}
	return key;
}

uint8_t Wallet::generateRecoveryId() {
	return 27; // CHAIN_ID * 2 + 35;
}

/**
  Device address getter.

  @return address
*/
String Wallet::getAddress() {
	return this->address;
}

/**
  Random seed generator.

  @param dest pointer to desr tandom number
  @param size size of poiner data
  @return 1 if success
*/
int Wallet::RNG(uint8_t* dest, unsigned size) {
	while (size) {
		uint8_t aux = (uint8_t) random(0, 255);
		*dest = aux;
		++dest;
		--size;
	}
	// Hashing data here could be a good idea to improve random data
	return 1;
}

/**
  Generate or recover from EEPROM private and public keys and public address.

  @param reset true->generate key thought it is in EEPROM already. false-> check
  in EEPROM memory if not null recover, if null generate it.

*/
void Wallet::generateKeyAddress(bool reset) {
	ConfigService& configService = ConfigService::getInstance();

	//Get the Key if it is in the device, else generate a new key
	String privateKey = configService.getConfig("privateKey", "");
	String address = configService.getConfig("address", "");

	bool isKeyGenerated = privateKey != "" && address != "";

	if (!reset && isKeyGenerated) {
		Serial.println("Private key and address found in EEPROM, recovering...");
		Helper::stringToByteArray(privateKey, this->privateKey);
		this->address = address;
	}
	else {
		Serial.println("Generating new private key...");
		const struct uECC_Curve_t* curve = uECC_secp256k1();
		if (uECC_make_key(this->publicKey, this->privateKey, curve)) Serial.println("success");
		else Serial.println("error");

		String hashedAddress = Helper::hash(this->publicKey, PUBLIC_KEY_SIZE);
		this->address = hashedAddress.substring(24, 64);

		privateKey = getPrivateKey();
		address = this->address;

		//Save to EEPROM
		configService.setConfig("privateKey", privateKey);
		configService.setConfig("address", this->address);
	}
	Serial.println("privateKey: " + privateKey);
	Serial.println("address: " + address);
}

/**
  ECDSA sign string data using private key

  @param data data string to be signed

*/
String Wallet::signHex(String data, uint8_t* result) {
	//Check for 0x, if it is there remove it
	if (data.substring(0, 2) == "0x") data = data.substring(2, data.length());

	uint8_t data8[data.length() / 2];
	Helper::stringToByteArray(data, data8);

	return sign(data8, data.length() / 2, result);
}

String Wallet::signJson(String json, uint8_t* result) {
	int size = json.length() + 1;
	uint8_t data[size];
	json.getBytes(data, size);

	return sign(data, json.length(), result);
}

String Wallet::sign(uint8_t* data, size_t length, uint8_t* result) {
	// Hash data
	String hashedData = Helper::hash(data, length);
	uint8_t hashedData8[length];
	Helper::stringToByteArray(hashedData, hashedData8);

	// Prefix data
	String prefix = "\u0019Ethereum Signed Message:\n" + String(HASH_LENGTH);
	uint8_t prefix8[prefix.length()];
	Helper::utf8ToByteArray(prefix, prefix8);

	// Concatenate prefix and hashed data
	uint8_t prefixedData8[prefix.length() + HASH_LENGTH];
	memcpy(prefixedData8, prefix8, prefix.length());
	memcpy(prefixedData8 + prefix.length(), hashedData8, HASH_LENGTH);

	// Hash prefixed data and hashed data
	String prefixedHashedData = Helper::hash(prefixedData8, prefix.length() + HASH_LENGTH);
	uint8_t prefixedHashedData8[prefixedHashedData.length() / 2];
	Helper::stringToByteArray(prefixedHashedData, prefixedHashedData8);

	// Sign data
	uint8_t signature[ETHERS_SIGNATURE_LENGTH];
	bool res = Crypto::Sign(this->privateKey, prefixedHashedData8, signature);

	// Check for error signing data
	if (!res) {
		Serial.println("Error signing data");
		return "";
	}

	Serial.println("Signature generated");

	// Generate recovery ID
	uint8_t recoveryId = generateRecoveryId();
	recoveryId += signature[64];
	signature[64] = recoveryId;

	// Convert to hex string
	String signStr = Helper::uint8ArrayToHexString(signature, ETHERS_SIGNATURE_LENGTH);

	if (result != nullptr)
		memcpy(result, signature, ETHERS_SIGNATURE_LENGTH);

	return signStr;
}

#ifdef PIO_UNIT_TESTING
void Wallet::setPrivateKey(String privateKey) {
	Helper::stringToByteArray(privateKey, this->privateKey);
}
#endif